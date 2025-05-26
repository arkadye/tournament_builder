#include "tournament_builder/tag_and_reference_helpers.hpp"

#include "tournament_builder/token.hpp"
#include "tournament_builder/exceptions.hpp"

#include <format>
#include <string>
#include <cassert>
#include <algorithm>

namespace tournament_builder::helpers
{
	GetWildcardArgsResult get_wildcard_args(const tournament_builder::Token& token, WildcardType type, char arg_delim)
	{
		auto throw_error = [&token](std::string_view explanation)
			{
				throw exception::TournamentBuilderException{ std::format("Error parsing args in token '{}': {}", token, explanation) };
			};
		GetWildcardArgsResult result;
		const bool is_any = (type == WildcardType::any);
		result.min = is_any ? 1 : 0;
		result.max = is_any ? 1 : std::numeric_limits<int64_t>::max();
		{
			std::string_view tok_str = token.to_string();
			const auto first_delim_location = tok_str.find(arg_delim);
			if (first_delim_location >= tok_str.size())
			{
				result.prefix = tok_str;
				return result;
			}
			result.prefix = tok_str.substr(0, first_delim_location);
		}
		const auto any_args = Token::get_args(token, arg_delim);

		if (!any_args.empty())
		{
			if (any_args.size() > 2u) [[unlikely]]
			{
				throw_error(std::format("Token has {} arguments after the ':'. Can only have 0, 1, or 2 arguments.", any_args.size()));
			}

			if (const std::string_view* str_arg = std::get_if<std::string_view>(&any_args.front())) [[unlikely]]
			{
				throw_error(std::format("Argument to '{}' must be positive number. Got '{}', which is not a number", result.prefix, *str_arg));
			}

			if (is_any || (any_args.size() == 2u))
			{
				assert(std::holds_alternative<int64_t>(any_args.front()));
				result.min = std::get<int64_t>(any_args.front());

				assert(std::holds_alternative<int64_t>(any_args.back()));
				result.max = std::get<int64_t>(any_args.back());
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
					result.max = std::get<int64_t>(any_args.front());
					break;
				default:
					assert(false);
				}
			}

			if (result.min < 0) [[unlikely]]
			{
				throw_error(std::format("Min levels argument to '{}' must not be a negative number. Got '{}', which is less than 0", result.prefix, result.min));
			}

			if (result.max <= 0) [[unlikely]]
			{
				throw_error(std::format("Max levels argument to '{}' must be positive number. Got '{}', which is less than 1", result.prefix, result.max));
			}

			if (result.min > result.max) [[unlikely]]
			{
				throw_error(std::format("Min levels ({}) is greater than max levels ({}).", result.min, result.max));
			}
		}
		return result;
	}
}