#include "tournament_builder/reference.hpp"
#include "tournament_builder/exceptions.hpp"
#include "tournament_builder/world.hpp"
#include "tournament_builder/competition.hpp"
#include "tournament_builder/competitor.hpp"

#include "tournament_builder/generic_utils.hpp"
#include "tournament_builder/json/json_helpers.hpp"

#include "nlohmann/json.hpp"


#include <algorithm>
#include <cassert>
#include <sstream>
#include <ranges>
#include <optional>
#include <variant>

namespace tournament_builder
{
	namespace internal_reference
	{
		constexpr char REF_DELIM = '.';
		constexpr char ARGUMENT_DIVIDER = ':';
		constexpr char SPECIAL_REF_INDICATOR = '@';
		constexpr std::string_view HERE{ "@HERE" };
		constexpr std::string_view ROOT{ "@ROOT" };
		constexpr std::string_view ANY_PREFIX { "@ANY" };
		constexpr std::string_view GLOB_PREFIX { "@GLOB" };
		constexpr std::string_view OUTER_PREFIX{ "@OUTER" };

		using TokenVector = std::vector<Token>;
		using TokenIterator = TokenVector::const_iterator;

		std::vector<IReferencable*> get_next_locations(IReferencable* target, const Token& token)
		{
			std::vector<IReferencable*> next_locations = target->get_next_locations();
			auto remove_result = std::ranges::remove_if(next_locations, [&token](const IReferencable* referencable)
				{
					return !referencable->matches_token(token);
				});
			next_locations.erase(begin(remove_result),end(remove_result));
			return next_locations;
		}

		enum class SpecialRefType : std::int8_t
		{
			normal,
			here,
			root,
			any_prefix,
			glob_prefix,
			outer_prefix,
			invalid
		};

		SpecialRefType get_type(Token t)
		{
			using enum SpecialRefType;
			const std::string_view s = t.to_string();
			if (s.empty()) return normal;
			if (s[0] != internal_reference::SPECIAL_REF_INDICATOR) return normal;
			if (s == HERE) return here;
			if (s == ROOT) return root;
			if (s.starts_with(ANY_PREFIX)) return any_prefix;
			if (s.starts_with(GLOB_PREFIX)) return glob_prefix;
			if (s.starts_with(OUTER_PREFIX)) return outer_prefix;
			return invalid;
		}

		std::pair<std::size_t, SpecialRefType> get_start_location_and_type(const TokenVector& elements)
		{
			for (std::size_t idx = 0u; idx < elements.size(); ++idx)
			{
				const std::size_t ridx = elements.size() - idx - 1;
				const SpecialRefType type = get_type(elements[ridx]);
				switch (type)
				{
				case SpecialRefType::root:
				case SpecialRefType::here:
					return { ridx + 1u, type };
				default:
					break;
				}
			}
			return { 0, SpecialRefType::here };
		}

		std::vector<ReferenceResultBase> dereference_to(const SoftReference& context, TokenIterator start, TokenIterator current, TokenIterator last, std::vector<IReferencable*>& current_working_location);

		namespace tag_processing
		{
			std::vector<ReferenceResultBase> dereference_normal_tag(Token target_token, const SoftReference& context, TokenIterator start, TokenIterator current, TokenIterator last, std::vector<IReferencable*>& current_working_location)
			{
				IReferencable* location = current_working_location.back();
				const std::vector<IReferencable*> next_locations = get_next_locations(location, target_token);
				std::vector<ReferenceResultBase> result;
				for (IReferencable* nl : next_locations)
				{
					current_working_location.emplace_back(nl);
					std::vector<ReferenceResultBase> fragment = dereference_to(context, start, current + 1, last, current_working_location);
					current_working_location.pop_back();
					std::ranges::copy(fragment, std::back_inserter(result));
				}
				return result;
			}

			std::vector<ReferenceResultBase> dereference_outer_tag(Token current_token, const SoftReference& context, TokenIterator start, TokenIterator current, TokenIterator last, std::vector<IReferencable*>& current_working_location)
			{
				auto throw_error = [current_token, start, current, &context](std::string_view explanation)
					{
						throw exception::InvalidReferenceToken{ context.to_string(), current_token, static_cast<std::size_t>(std::distance(start, current)), explanation };
					};
				const auto outer_args = Token::get_args(current_token, ARGUMENT_DIVIDER);

				int64_t num_levels = 1;
				if (!outer_args.empty())
				{
					if (outer_args.size() > 1u) [[unlikely]]
					{
						throw_error("Token has multiple arguments after the ':'. Can only have 1 argument or fewer.");
					}

					if (const std::string_view* str_arg = std::get_if<std::string_view>(&outer_args.front())) [[unlikely]]
					{
						throw_error(std::format("Argument to {} must be positive number. Got '{}', which is not a number", OUTER_PREFIX, *str_arg));
					}

					assert(std::holds_alternative<int64_t>(outer_args.front()));
					num_levels = std::get<int64_t>(outer_args.front());

					if (num_levels <= 0) [[unlikely]]
					{
						throw_error(std::format("Argument to {} must be positive number.Got '{}', which is less than 1", OUTER_PREFIX, num_levels));
					}
				}

				for (int i = 0; i < num_levels; ++i)
				{
					if (current_working_location.empty()) [[unlikely]]
					{
						throw_error("Outer command used, but already at the outermost part.");
					}
					current_working_location.pop_back();
				}
				return dereference_to(context, start, current + 1, last, current_working_location);
			}

			std::vector<ReferenceResultBase> dereference_any_tag(int64_t min_levels, int64_t max_levels, const SoftReference& context, TokenIterator start, TokenIterator current, TokenIterator last, std::vector<IReferencable*>& current_working_location)
			{
				assert(min_levels <= max_levels);
				assert(max_levels >= 0);

				// If we ran out of any tags, resume normal processing
				if (max_levels == 0)
				{
					return dereference_to(context, start, current + 1, last, current_working_location);
				}

				std::vector<ReferenceResultBase> result;

				// If we've reached the min levels, try a regular dereference.
				if (min_levels <= 0)
				{
					result = dereference_to(context, start, current + 1, last, current_working_location);
				}

				{
					IReferencable* location = current_working_location.back();
					std::vector<ReferenceResultBase> sub_result;
					for (IReferencable* next_location : location->get_next_locations())
					{
						current_working_location.push_back(next_location);
						sub_result = dereference_any_tag(min_levels - 1, max_levels - 1, context, start, current, last, current_working_location);
						current_working_location.pop_back();

						std::ranges::copy(sub_result, std::back_inserter(result));
						sub_result.clear();
					}
				}

				return result;
			}

			std::vector<ReferenceResultBase> dereference_any_tag(SpecialRefType any_or_glob, Token current_token, const SoftReference& context, TokenIterator start, TokenIterator current, TokenIterator last, std::vector<IReferencable*>& current_working_location)
			{
				auto throw_error = [current_token, start, current, &context](std::string_view explanation)
					{
						throw exception::InvalidReferenceToken{ context.to_string(), current_token, static_cast<std::size_t>(std::distance(start, current)), explanation };
					};
				const auto any_args = Token::get_args(current_token, ARGUMENT_DIVIDER);

				const bool is_any = (any_or_glob == SpecialRefType::any_prefix);
				assert(is_any || (any_or_glob == SpecialRefType::glob_prefix));

				int64_t min_levels = is_any ? 1 : 0;
				int64_t max_levels = is_any ? 1 : std::numeric_limits<int64_t>::max();
				if (!any_args.empty())
				{
					if (any_args.size() > 2u) [[unlikely]]
					{
						throw_error(std::format("Token has {} arguments after the ':'. Can only have 0, 1, or 2 arguments.", any_args.size()));
					}

					if (const std::string_view* str_arg = std::get_if<std::string_view>(&any_args.front())) [[unlikely]]
					{
						throw_error(std::format("Argument to {} must be positive number. Got '{}', which is not a number", ANY_PREFIX, *str_arg));
					}

					if (is_any || (any_args.size() == 2u))
					{
						assert(std::holds_alternative<int64_t>(any_args.front()));
						min_levels = std::get<int64_t>(any_args.front());

						assert(std::holds_alternative<int64_t>(any_args.back()));
						max_levels = std::get<int64_t>(any_args.back());
					}
					else // is_glob
					{
						switch (any_args.size())
						{
						case 0u:
							// Nothing to do
							break;
						case 1u:
							// The argument is the maximum number of levels we can use.
							assert(std::holds_alternative<int64_t>(any_args.front()));
							max_levels = std::get<int64_t>(any_args.front());
							break;
						default:
							assert(false);
						}
					}

					if (min_levels < 0) [[unlikely]]
					{
						throw_error(std::format("Min levels argument to {} must not be a negative number. Got '{}', which is less than 0", ANY_PREFIX, min_levels));
					}

					if (max_levels <= 0) [[unlikely]]
					{
						throw_error(std::format("Max levels argument to {} must be positive number. Got '{}', which is less than 1", ANY_PREFIX, max_levels));
					}

					if (min_levels > max_levels) [[unlikely]]
					{
						throw_error(std::format("Min levels ({}) is greater than max levels ({}).", min_levels, max_levels));
					}
				}

				return dereference_any_tag(min_levels, max_levels, context, start, current, last, current_working_location);
			}
		}

		std::vector<ReferenceResultBase> dereference_to(const SoftReference& context, TokenIterator start, TokenIterator current, TokenIterator last, std::vector<IReferencable*>& current_working_location)
		{
			if (current == last)
			{
				// Check current_working_location isn't empty.
				if (current_working_location.empty())
				{
					return {};
				}

				const ReferenceResultBase result = [&current_working_location]()
					{
						IReferencable* r = current_working_location.back();
						Location l;
						const std::size_t location_size = current_working_location.size() - 2; // Drop the current locatio and the world at the beginning.
						l.reserve(location_size);
						std::ranges::transform(current_working_location | std::views::drop(1) | std::views::take(location_size), std::back_inserter(l), [](IReferencable* ref) {return ref->get_reference_key(); });
						return ReferenceResultBase{ r, std::move(l) };
					}();

				return { result };
			}

			// Process current token.
			const Token current_token = *current;
			const SpecialRefType tag_type = get_type(current_token);
			switch (tag_type)
			{
			case SpecialRefType::normal:
				return tag_processing::dereference_normal_tag(current_token, context, start, current, last, current_working_location);
			case SpecialRefType::outer_prefix:
				return tag_processing::dereference_outer_tag(current_token, context, start, current, last, current_working_location);
			case SpecialRefType::any_prefix:
			case SpecialRefType::glob_prefix:
				return tag_processing::dereference_any_tag(tag_type, current_token, context, start, current, last, current_working_location);
			case SpecialRefType::here:
			case SpecialRefType::root:
			case SpecialRefType::invalid:
				// We should have filtered for these already.
				assert(false);
				break;
			}
			return {};
		}

		std::vector<ReferenceResultBase> dereference_to(const SoftReference& reference, World& context, const Location& location, const TokenVector& elements)
		{
			auto start_it = begin(elements);

			std::vector<IReferencable*> working_location;
			working_location.reserve(1u + elements.size() + location.size());
			working_location.emplace_back(&context);
			if (!location.empty())
			{
				const auto [start_idx, start_type] = get_start_location_and_type(elements);
				std::advance(start_it, start_idx);
				switch (start_type)
				{
				case SpecialRefType::here:
					// Use location to initialize working location.
					for (Name location_fragment : location)
					{
						std::vector<IReferencable*> next = get_next_locations(working_location.back(), location_fragment);
						assert(next.size() == 1u);
						working_location.emplace_back(next.front());
					}
					break;
				case SpecialRefType::root:
					// Leave working location empty.
					break;
				default:
					assert(false);
					break;
				}
			}

			return dereference_to(reference, begin(elements), start_it, end(elements), working_location);
		}

		std::string ReferenceResultBase::to_string() const
		{
			std::ostringstream oss;
			oss << "[";
			bool first_iteration = true;
			for (const Name& n : location)
			{
				if (first_iteration)
				{
					first_iteration = false;
				}
				else
				{
					oss << ".";
				}
				oss << n.to_string();
			}
			oss << ']' << (result != nullptr ? result->get_reference_key().to_string() : std::string{"!NULL!"});
			return oss.str();
		}
}

	SoftReference::SoftReference(std::string_view init)
	{
		utils::split_tokens<Token>(init, internal_reference::REF_DELIM, std::back_inserter(m_elements), { {'[',']'} });

		for (std::size_t idx=0u;idx<m_elements.size();++idx)
		{
			const Token t = m_elements[idx];
			if (internal_reference::get_type(t) == internal_reference::SpecialRefType::invalid)
			{
				throw exception::InvalidReferenceToken{ init, t, idx, "Unrecognised special token." };
			}
		}

		return;
	}

	std::string SoftReference::to_string() const
	{
		return utils::join_tokens(m_elements, internal_reference::REF_DELIM);
	}

	SoftReference SoftReference::parse(const nlohmann::json& input)
	{
		std::optional<SoftReference> result = try_parse(input);
		if (!result.has_value())
		{
			std::ostringstream oss;
			oss << input;
			throw exception::ReferenceParseException{ oss.str() };
		}
		return result.value();
	}

	std::vector<internal_reference::ReferenceResultBase> SoftReference::dereference_to_impl(World& context, const Location& location) const
	{
		return internal_reference::dereference_to(*this, context, location, m_elements);
	}

	std::optional<SoftReference> SoftReference::try_parse(const nlohmann::json& input)
	{
		if (input.is_string())
		{
			return SoftReference{ input.get<std::string>() };
		}
		if (std::optional<std::string> data = json_helper::get_optional_string(input, "ref"))
		{
			return SoftReference{ data.value() };
		}
		return std::nullopt;
	}

	void SoftReference::throw_invalid_dereference() const
	{
		throw exception::InvalidDereference{ *this };
	}

	void internal_reference::ReferenceBase::throw_invalid_dereference() const
	{
		assert(is_reference());
		std::get<SoftReference>(m_data).throw_invalid_dereference();
	}

	namespace exception
	{
		InvalidDereference::InvalidDereference(const SoftReference& reference)
			: ReferenceException{ std::format("Cannot use get() on reference {} because it is an indirect reference. Use dereference_single() instead.", reference) }
		{
		}

		InvalidReferenceToken::InvalidReferenceToken(std::string_view reference, std::string_view token, std::size_t pos, std::string_view explanation)
			: ReferenceException{ std::format("In {}: invalid '{}' found at position {}: {}", reference, token, pos, explanation) }
		{
		}

		ExpectedSingleResult::ExpectedSingleResult(const SoftReference& reference, std::size_t num_results)
			: ReferenceException{ std::format("Expected a single result dereferencing {}, but got {} results", reference, num_results) }
		{
		}
	}
}