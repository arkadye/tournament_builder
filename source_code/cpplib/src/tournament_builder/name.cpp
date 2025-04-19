#include "tournament_builder/name.hpp"
#include "tournament_builder/exceptions.hpp"

#include "tournament_builder/json/json_helpers.hpp"

#include "nlohmann/json.hpp"

#include <cassert>
#include <vector>
#include <algorithm>
#include <string_view>
#include <ranges>
#include <optional>

namespace tournament_builder
{
	using nlohmann::json;
	void Name::validate_name(std::string_view name)
	{
		if (name.empty())
		{
			throw exception::InvalidArgument("'name' cannot be empty.");
		}

		auto is_invalid_char = [](char c)
			{
				if (std::isalnum(c)) return false;
				return c != '_';
			};
		const auto illegal_char_it = std::ranges::find_if(name, is_invalid_char);
		if (illegal_char_it != end(name))
		{
			const auto illegal_char_pos = std::distance(begin(name), illegal_char_it);
			throw exception::IllegalName(name, illegal_char_pos, *illegal_char_it);
		}
	}

	Name Name::parse(const json& input)
	{
		json_helper::validate_type(input, { json::value_t::string , json::value_t::object });
		if (input.is_string())
		{
			const std::string result = input.get<std::string>();
			return Name{ result };
		}

		assert(input.is_object());
		const std::string result = json_helper::get_string(input, "name");
		return Name{ result };
	}
}