#include "tournament_builder/tag_and_reference_helpers.hpp"

#include "tournament_builder/token.hpp"
#include "tournament_builder/exceptions.hpp"

#include <format>
#include <string>
#include <cassert>
#include <algorithm>

namespace tournament_builder::helpers
{
	GetSpecialTagArgsResult get_special_tag_args(const tournament_builder::Token& token, SpecialTagType type, char arg_delim)
	{
		auto throw_error = [&token](std::string_view explanation)
			{
				throw exception::TournamentBuilderException{ std::format("Error parsing args in token '{}': {}", token, explanation) };
			};

		enum class SingleArgBehaviour
		{
			set_min,
			set_both,
			set_max
		};
		bool allow_negative_min = false;
		bool allow_zero_min = false;
		bool allow_negative_max = false;
		bool allow_zero_max = false;
		SingleArgBehaviour single_arg_behaviour = SingleArgBehaviour::set_both;
		GetSpecialTagArgsResult result;

		switch (type)
		{
		case SpecialTagType::any:
			result.min = 1;
			result.max = 1;
			allow_zero_min = true;
			break;
		case SpecialTagType::glob:
			result.min = 0;
			result.max = std::numeric_limits<int64_t>::max();
			allow_zero_min = true;
			single_arg_behaviour = SingleArgBehaviour::set_max;
			break;
		case SpecialTagType::entry:
		case SpecialTagType::pos:
			result.min = 0;
			result.max = 0;
			allow_negative_min = true;
			allow_negative_max = true;
			break;
		}

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
			for (std::size_t arg_idx = 0u; arg_idx < any_args.size(); ++arg_idx)
			{
				if (const std::string_view* str_arg = std::get_if<std::string_view>(&any_args[arg_idx])) [[unlikely]]
				{
					throw_error(std::format("Argument to '{}' must be positive number. Arg {} is '{}', which is not a number", result.prefix, arg_idx, *str_arg));
				}
			}

			switch (any_args.size())
			{
			default:
				throw_error(std::format("Token has {} arguments after the '{}'. Can only have 0, 1, or 2 arguments.", any_args.size(), arg_delim));
				break;
			case 0u:
				// Nothing to be done.
				break;
			case 1u:
			{
				const int64_t arg_val = std::get<int64_t>(any_args.front());
				switch (single_arg_behaviour)
				{
				case SingleArgBehaviour::set_min:
					result.min = arg_val;
					break;
				case SingleArgBehaviour::set_max:
					result.max = arg_val;
					break;
				case SingleArgBehaviour::set_both:
					result.min = result.max = arg_val;
					break;
				}
				break;
			}
			case 2u:
				result.min = std::get<int64_t>(any_args.front());
				result.max = std::get<int64_t>(any_args.back());
				break;
			}

			if (result.min > result.max) [[unlikely]]
			{
				throw_error(std::format("Min argument to '{}' must not be greater than the max argument. Got {} and {}.", result.prefix, result.min, result.max));
			}

			if (!allow_negative_min && result.min < 0) [[unlikely]]
			{
				throw_error(std::format("Min argument to '{}' must not be a negative number. Got '{}', which is less than 0", result.prefix, result.min));
			}

			if (!allow_zero_min && result.min == 0) [[unlikely]]
			{
				throw_error(std::format("Min argument to '{}' must not be zero. Got '{}'.", result.prefix, result.min));
			}

			if (!allow_negative_max && result.max < 0) [[unlikely]]
			{
				throw_error(std::format("Max argument to '{}' must not be a negative number. Got '{}', which is less than 0", result.prefix, result.min));
			}

			if (!allow_zero_max && result.max == 0) [[unlikely]]
			{
				throw_error(std::format("Max argument to '{}' must not be zero. Got '{}'.", result.prefix, result.min));
			}
		}
		return result;
	}
}