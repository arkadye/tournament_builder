#include "tournament_builder/reference.hpp"
#include "tournament_builder/exceptions.hpp"
#include "tournament_builder/world.hpp"
#include "tournament_builder/competition.hpp"
#include "tournament_builder/competitor.hpp"

#include "tournament_builder/generic_utils.hpp"
#include "tournament_builder/json/json_helpers.hpp"
#include "tournament_builder/tag_and_reference_helpers.hpp"

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
		constexpr std::string_view TEMPLATE_STORE{ "@TEMPLATE" };
		constexpr std::string_view ANY_PREFIX { "@ANY" };
		constexpr std::string_view GLOB_PREFIX { "@GLOB" };
		constexpr std::string_view OUTER_PREFIX{ "@OUTER" };

		using TokenVector = std::vector<Token>;
		using TokenIterator = TokenVector::const_iterator;

		int32_t ReferenceBase::get_min_results() const noexcept
		{
			struct Impl
			{
				int32_t operator()(const SoftReference& sr) const { return sr.get_min_results(); }
				int32_t operator()(const std::shared_ptr<IReferencable>& ir) const { return 1; }
			};
			return std::visit(Impl{}, m_data);
		}

		int32_t ReferenceBase::get_max_results() const noexcept
		{
			struct Impl
			{
				int32_t operator()(const SoftReference& sr) const { return sr.get_max_results(); }
				int32_t operator()(const std::shared_ptr<IReferencable>& ir) const { return 1; }
			};
			return std::visit(Impl{}, m_data);
		}

		bool ReferenceBase::has_fixed_size() const noexcept
		{
			struct Impl
			{
				bool operator()(const SoftReference& sr) const { return sr.has_fixed_size(); }
				bool operator()(const std::shared_ptr<IReferencable>& ir) const { return true; }
			};
			return std::visit(Impl{}, m_data);
		}

		bool ReferenceBase::has_single_item() const noexcept
		{
			struct Impl
			{
				bool operator()(const SoftReference& sr) const { return sr.has_single_item(); }
				bool operator()(const std::shared_ptr<IReferencable>& ir) const { return true; }
			};
			return std::visit(Impl{}, m_data);
		}

		std::string ReferenceBase::to_string() const
		{
			struct Impl
			{
				std::string operator()(const std::shared_ptr<IReferencable>& ir) const
				{
					return std::format("name='{}'", ir->get_reference_key());

				}
				std::string operator()(const SoftReference& sr) const { return std::format("{}", sr.to_string()); }
			};
			return std::visit(Impl{}, m_data);
		}

		std::vector<IReferencable*> get_next_locations(IReferencable* target, const Token& token)
		{
			assert(target != nullptr);
			std::vector<IReferencable*> next_locations = target->get_next_locations(token);
			return next_locations;
		}

		enum class SpecialRefType : std::int8_t
		{
			normal,
			here,
			root,
			template_store,
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
			if (s == TEMPLATE_STORE) return template_store;
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
						throw_error(std::format("Token '{}' has multiple arguments ({}) after the ':'. Can only have 1 argument or fewer.",current_token, outer_args.size()));
					}

					if (const std::string_view* str_arg = std::get_if<std::string_view>(&outer_args.front())) [[unlikely]]
					{
						throw_error(std::format("Argument to '{}' must be positive number. Got '{}', which is not a number", OUTER_PREFIX, *str_arg));
					}

					assert(std::holds_alternative<int64_t>(outer_args.front()));
					num_levels = std::get<int64_t>(outer_args.front());

					if (num_levels <= 0) [[unlikely]]
					{
						throw_error(std::format("Argument to '{}' must be positive number. Got '{}', which is less than 1", OUTER_PREFIX, num_levels));
					}
				}

				if (num_levels >= std::ssize(current_working_location))
				{
					throw_error(std::format("{} element in reference would try to get outside the World object.", current_token));
				}

				auto new_end_it = end(current_working_location) - num_levels;
				current_working_location.erase(new_end_it, end(current_working_location));

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
					for (IReferencable* next_location : location->get_all_next_locations())
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
				helpers::SpecialTagType wildcard_type{};
				switch (any_or_glob)
				{
				case SpecialRefType::any_prefix:
					wildcard_type = helpers::SpecialTagType::any;
					break;
				case SpecialRefType::glob_prefix:
					wildcard_type = helpers::SpecialTagType::glob;
					break;
				default:
					assert(false && "SpecialRefType was not any or glob types.");
				}

				try
				{
					const helpers::GetSpecialTagArgsResult get_args_result = helpers::get_special_tag_args(current_token, wildcard_type, ARGUMENT_DIVIDER);
					return dereference_any_tag(get_args_result.min, get_args_result.max, context, start, current, last, current_working_location);
				}
				catch (const exception::TournamentBuilderException& ex)
				{
					throw exception::InvalidReferenceToken{ context.to_string(), current_token, static_cast<std::size_t>(std::distance(start, current)), ex.what() };
				}

			}

			nlohmann::json make_text_replacements(nlohmann::json object, const std::vector<SoftReference::Replacement>& replacements);

			nlohmann::json make_text_replacements_iterable(nlohmann::json object, const std::vector<SoftReference::Replacement>& replacements)
			{
				assert(object.is_object() || object.is_array());
				for (auto& elem : object)
				{
					elem = make_text_replacements(elem, replacements);
				}
				return object;
			}

			nlohmann::json make_text_replacements_str(nlohmann::json object, const std::vector<SoftReference::Replacement>& replacements)
			{
				assert(object.is_string());
				std::ostringstream oss;
				bool found_replacement = false;
				std::string data = object;
				std::size_t search_start = 0u;

				while (true)
				{
					// Can I do this with ranges or an algorithm? Probably. How? I don't know.
					auto find_pos = std::string::npos;
					auto find_it = end(replacements);
					for (auto it = begin(replacements); it != end(replacements); ++it)
					{
						const auto pos = data.find(it->first, search_start);
						if (pos < find_pos)
						{
							find_pos = pos;
							find_it = it;
						}
						if (pos == 0u)
						{
							break;
						}
					}

					if (find_it == end(replacements))
					{
						break;
					}

					data.replace(find_pos, find_it->first.size(), find_it->second);
					search_start = find_pos + find_it->second.size();
					found_replacement = true;
				}

				if (found_replacement)
				{
					object = data;
				}
				return object;
			}

			nlohmann::json make_text_replacements(nlohmann::json object, const std::vector<SoftReference::Replacement>& replacements)
			{
				if (object.is_object() || object.is_array())
				{
					return make_text_replacements_iterable(std::move(object), replacements);
				}
				else if (object.is_string())
				{
					return make_text_replacements_str(std::move(object), replacements);
				}
				return object;
			}

			nlohmann::json get_deferred_json_impl(const nlohmann::json& store, TokenIterator start, TokenIterator current, TokenIterator last, const std::vector<SoftReference::Replacement>& replacements)
			{
				json_helper::validate_type(store, { nlohmann::json::value_t::array, nlohmann::json::value_t::object });
				if (current == last)
				{
					if (replacements.empty())
					{
						return store;
					}
					else
					{
						return make_text_replacements(store, replacements);
					}
				}
				nlohmann::json next = [token = current->to_string(), &store]()
					{
						if (store.is_object())
						{
							return json_helper::get_any(store, token);
						}
						else if (store.is_array())
						{
							uint64_t val{};
							const char* data_start = token.data();
							const char* data_end = data_start + token.size();
							const std::from_chars_result result = std::from_chars(data_start, data_end + token.size(), val);
							if (result.ec != std::errc{} || result.ptr != data_end)
							{
								throw exception::InvalidArgument(std::format("Expected array index (i.e. positive integer) but instead got '{}'", token));
							}

							if (val >= store.size())
							{
								throw exception::InvalidArgument(std::format("Cannot access item {} of an array with {} elements", val, store.size()));
							}
							return store[val];
						}
						assert(false);
						return store;
					}();

				try
				{
					return get_deferred_json_impl(next, start, current + 1, last, replacements);
				}
				catch (exception::TournamentBuilderException& ex)
				{
					ex.add_context(std::format("In part '{}'", *current));
					throw ex;
				}
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

				IReferencable* r = current_working_location.back();
				Location l;
				if (current_working_location.size() >= 2u)
				{
					const std::size_t location_size = current_working_location.size() - 2; // Drop the current location and the world at the beginning.
					l.reserve(location_size);
					std::ranges::transform(current_working_location | std::views::drop(1) | std::views::take(location_size), std::back_inserter(l), [](IReferencable* ref) {return ref->get_reference_key(); });
				}
				return { ReferenceResultBase{ r, std::move(l) } };
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
			case SpecialRefType::template_store:
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

		IReferencable* ReferenceResultBase::get_ptr() const noexcept
		{
			struct Impl
			{
				IReferencable* operator()(IReferencable* in) const noexcept
				{
					return in;
				}
				IReferencable* operator()(const std::shared_ptr<IReferencable>& in) const noexcept
				{
					return in.get();
				}
			};
			return std::visit(Impl{}, m_result);
		}

		std::string ReferenceResultBase::to_string() const
		{
			std::ostringstream oss;
			oss << "[";
			bool first_iteration = true;
			for (const Name& n : m_location)
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
			oss << ']' << (get_ptr() != nullptr ? get_ptr()->get_reference_key().to_string() : std::string_view{"!NULL!"});
			return oss.str();
		}

		ReferenceCopyOptions ReferenceBase::get_copy_opts() const
		{
			if (const auto* sr = std::get_if<SoftReference>(&m_data))
			{
				return sr->get_copy_opts();
			}
			return ReferenceCopyOptions{};
		}
	}

	std::unique_ptr<nlohmann::json> SoftReference::get_deferred_json(const World& context) const
	{
		assert(!m_elements.empty());
		if (!context.template_store)
		{
			exception::InvalidDereference ex(*this);
			ex.add_context("Tried to derefence @TEMPLATE reference, but no 'templates' field exists on the top level object");
			throw ex;
		}
		try
		{
			return std::make_unique<nlohmann::json>(internal_reference::tag_processing::get_deferred_json_impl(*context.template_store.get(), begin(m_elements), begin(m_elements) + 1, end(m_elements), m_replacements));
		}
		catch (exception::TournamentBuilderException& ex)
		{
			ex.add_context("In template store at 'templates'");
			throw ex;
		}
	}

	LocationPusher::LocationPusher(Location& in_loc, const Name& name)
		: location{ in_loc }
#ifndef NDEBUG
		, original_location{ in_loc }
#endif
	{
		location.push_back(name);
	}

	LocationPusher::~LocationPusher()
	{
#ifndef NDEBUG
		assert(!location.empty());
#endif
		location.pop_back();
#ifndef NDEBUG
		assert(original_location == location);
#endif
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

	bool SoftReference::uses_deferred_resolve() const noexcept
	{
		using enum internal_reference::SpecialRefType;
		if (m_elements.empty()) return false;
		const auto type = internal_reference::get_type(m_elements.front());
		switch (type)
		{
		case template_store:
			return true;
		default:
			break;
		}
		return false;
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
			SoftReference result{ data.value() };
			try {
				if (!result.m_elements.empty() && result.m_elements.front() == internal_reference::TEMPLATE_STORE)
				{
					if (auto replacements_opt = json_helper::get_optional_object(input, "text_replace"))
					{
						nlohmann::json replacements = std::move(replacements_opt).value();
						result.m_replacements.reserve(replacements.size());
						for (const auto& elem : replacements.items())
						{
							std::string key = elem.key();
							const nlohmann::json& value_json = replacements[key];
							if (!value_json.is_string())
							{
								throw exception::TournamentBuilderException{ std::format("In JSON object '{}': In reference to '{}': field 'text_replace': entry '{}' must be a string.",
									input, data.value(), key) };
							}
							std::string value = value_json;
							result.m_replacements.emplace_back(std::string(key), std::move(value));
						}
					}
					return result;
				}

				auto expected_results_opt = json_helper::get_optional_int(input, "num_results");
				auto min_results_opt = json_helper::get_optional_int(input, "min_results");
				auto max_results_opt = json_helper::get_optional_int(input, "min_results");
				if (expected_results_opt.has_value())
				{
					if (min_results_opt.has_value() || max_results_opt.has_value())
					{
						throw exception::ReferenceParseException{ "If `num_results` is set, 'min_results' and 'max_results' must both be left off." };
					}
					result.m_min_results = result.m_max_results = expected_results_opt.value();
				}
				else if (min_results_opt.has_value() || max_results_opt.has_value())
				{
					result.m_min_results = min_results_opt.value_or(1);
					result.m_max_results = max_results_opt.value_or(std::numeric_limits<decltype(result.m_max_results)>::max());
					if (result.m_min_results > result.m_max_results)
					{
						throw exception::ReferenceParseException{ std::format("'min_results' ({}) must not be greater than max results ({})", result.m_min_results, result.m_max_results) };
					}
				}

				return result;
			}
			catch (exception::TournamentBuilderException& ex)
			{
				ex.add_context(std::format("While parsing reference to '{}'", data.value()));
				throw ex;
			}
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
			: ReferenceException{ std::format("In '{}': invalid '{}' found at position {}: {}", reference, token, pos, explanation) }
		{
		}

		ExpectedSingleResult::ExpectedSingleResult(const SoftReference& reference, std::size_t num_results)
			: ReferenceException{ std::format("Expected a single result dereferencing '{}', but got {} results", reference, num_results) }
		{
		}
	}
}