#pragma once

#include "nlohmann/json.hpp"

#include <initializer_list>
#include <format>
#include <optional>
#include <string>
#include <sstream>

namespace tournament_builder::json_helper
{
	using nlohmann::json;
	void validate_type(const json& object, std::initializer_list<json::value_t> allowed_types);

	[[noreturn]] void throw_invalid_types_exception(const json& object, std::initializer_list<json::value_t> allowed_types);

	inline void validate_number(const nlohmann::json& object) { validate_type(object, { nlohmann::json::value_t::number_float , nlohmann::json::value_t::number_integer, nlohmann::json::value_t::number_unsigned }); }
	inline void validate_type(const nlohmann::json& object, nlohmann::json::value_t allowed_type) { validate_type(object, { allowed_type }); }

	std::optional<nlohmann::json> get_optional_any(const nlohmann::json& object, std::string_view field);
	nlohmann::json get_any_or(const nlohmann::json& object, std::string_view field, const nlohmann::json& fallback);
	nlohmann::json get_any(const nlohmann::json& object, std::string_view field);

	std::optional<std::string> get_optional_string(const nlohmann::json& object, std::string_view field);
	std::string get_string_or(const nlohmann::json& object, std::string_view field, std::string fallback);
	std::string get_string(const nlohmann::json& object, std::string_view field);

	std::optional<nlohmann::json> get_optional_object(const nlohmann::json& object, std::string_view field);
	nlohmann::json get_object_or(const nlohmann::json& object, std::string_view field, const nlohmann::json& fallback);
	nlohmann::json get_object(const nlohmann::json& object, std::string_view field);

	std::optional<nlohmann::json> get_optional_array_object(const nlohmann::json& object, std::string_view field);
	nlohmann::json get_array_object_or(const nlohmann::json& object, std::string_view field, const nlohmann::json& fallback);
	nlohmann::json get_array_object(const nlohmann::json& object, std::string_view field);

	std::optional<bool> get_optional_bool(const nlohmann::json& object, std::string_view field);
	bool get_bool_or(const nlohmann::json& object, std::string_view field, bool fallback);
	bool get_bool(const nlohmann::json& object, std::string_view field);

	std::optional<int> get_optional_int(const nlohmann::json& object, std::string_view field);
	int get_int_or(const nlohmann::json& object, std::string_view field, int fallback);
	int get_int(const nlohmann::json& object, std::string_view field);

	std::optional<uint64_t> get_optional_uint64(const nlohmann::json& object, std::string_view field);
	uint64_t get_uint64_or(const nlohmann::json& object, std::string_view field, uint64_t fallback);
	uint64_t get_uint64(const nlohmann::json& object, std::string_view field);

	// Returns a vector
	template <typename Parser> requires std::invocable<Parser, const nlohmann::json&>
	inline std::vector<std::invoke_result_t<Parser, const nlohmann::json&>> get_array(const nlohmann::json& json_array, const Parser& parser)
	{
		using nlohmann::json;
		using ArrayType = std::invoke_result_t<Parser, const nlohmann::json&>;
		validate_type(json_array, json::value_t::array);
		std::vector<ArrayType> result;
		const std::size_t num_elements = json_array.size();
		result.reserve(num_elements);
		std::ranges::transform(json_array, std::back_inserter(result), parser);
		return result;
	}

	// Returns a vector
	template <typename Parser> requires std::invocable<Parser, const nlohmann::json&>
	inline std::vector<std::invoke_result_t<Parser, const nlohmann::json&>> get_array_maybe_from_single_item(const nlohmann::json& input, std::string_view field_name, const Parser& parser)
	{
		using nlohmann::json;
		using ArrayType = std::invoke_result_t<Parser, const nlohmann::json&>;

		std::vector<ArrayType> result;

		if (!input.contains(field_name))
		{
			return result;
		}

		const json& json_array = input[field_name];

		if (json_array.type() == json::value_t::array)
		{
			result = get_array(json_array, parser);
		}
		else
		{
			result.emplace_back(parser(json_array));
		}
		return result;
	}
};

template <>
class std::formatter<nlohmann::json>
{
public:
	template <class PrsContext>
	constexpr auto parse(const PrsContext& prsContext)
	{
		return prsContext.begin();
	}

	auto format(nlohmann::json json_object, std::format_context& ctx) const
	{
		std::ostringstream oss; 
		oss << json_object;
		return std::format_to(ctx.out(), "{}", oss.str());
	}
};