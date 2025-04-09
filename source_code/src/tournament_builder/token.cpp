#include "tournament_builder/token.hpp"
#include "tournament_builder/exceptions.hpp"

#include "tournament_builder/generic_utils.hpp"
#include "tournament_builder/json/json_helpers.hpp"

#include "nlohmann/json.hpp"

#include <cassert>
#include <vector>
#include <algorithm>
#include <string_view>
#include <ranges>
#include <optional>
#include <charconv>

namespace tournament_builder
{
	Token Token::parse(const nlohmann::json& input)
	{
		using nlohmann::json;
		json_helper::validate_type(input, { json::value_t::string , json::value_t::object });
		if (input.is_string())
		{
			const std::string result = input.get<std::string>();
			return Token{ result };
		}

		assert(input.is_object());
		const std::string result = json_helper::get_string(input, "token");
		return Token{ result };
	}

	std::vector<std::variant<int64_t, std::string_view>> Token::get_args(std::string_view token_str, char delim)
	{
		std::vector<std::variant<int64_t, std::string_view>> result;

		const auto num_delims = std::ranges::count(token_str, delim);
		if (num_delims == 0u)
		{
			return result;
		}
		
		// Eat the first bit which is just the identifying bit of the token.
		const auto first_delim = token_str.find(delim);
		assert(first_delim < token_str.size());
		token_str.remove_prefix(first_delim + 1);

		result.reserve(num_delims);

		utils::split_tokens<std::string_view>(token_str, delim, std::back_inserter(result));

		for (auto& result_elem : result)
		{
			assert(std::holds_alternative<std::string_view>(result_elem));
			const std::string_view elem = std::get<std::string_view>(result_elem);
			int64_t num{};
			const auto result = std::from_chars(elem.data(), elem.data() + elem.size(), num);
			if (result.ec == std::errc{})
			{
				result_elem = num;
			}
		}
		return result;
		
	}
}