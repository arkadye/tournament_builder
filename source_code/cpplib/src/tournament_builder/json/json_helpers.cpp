#include "tournament_builder/json/json_helpers.hpp"
#include "tournament_builder/exceptions.hpp"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace tournament_builder::exception
{
	using nlohmann::json;
	namespace internal_exception
	{
		const char* type_name(json::value_t type) noexcept
		{
			switch (type)
			{
			case json::value_t::null:
				return "null";
			case json::value_t::object:
				return "object";
			case json::value_t::array:
				return "array";
			case json::value_t::string:
				return "string";
			case json::value_t::boolean:
				return "boolean";
			case json::value_t::binary:
				return "binary";
			case json::value_t::discarded:
				return "discarded";
			case json::value_t::number_integer:
				return "integer";
			case json::value_t::number_unsigned:
				return "unsigned";
			case json::value_t::number_float:
				return "float";
			}
			return "ERROR";
		}

		std::string make_allowed_types_string(const std::vector<std::uint8_t>& values)
		{
			static_assert(std::is_same_v<std::underlying_type_t<json::value_t>, uint8_t>);
			std::string result;
			for (std::uint8_t val_as_int : values)
			{
				const auto val = static_cast<json::value_t>(val_as_int);
				if (result.empty())
				{
					result = std::format("{}", type_name(val));
				}
				else
				{
					result = std::format("{} , {}", result, type_name(val));
				}
			}
			if (result.empty()) return "[]";
			return std::format("[ {} ]", result);
		}
	}

	JsonParseInvalidType::JsonParseInvalidType(const json& object, const std::vector<std::uint8_t>& allowed_types)
		: JsonParseException{ object, std::format("Invalid type: '{}' but expected one of {}", object.type_name(), internal_exception::make_allowed_types_string(allowed_types)) }
	{}

	JsonParseException::JsonParseException(const json& object, std::string_view message)
		: TournamentBuilderException{ std::format("Error in JSON '{}': {}", object, message)}
	{}

	JsonInvalidDescriptor::JsonInvalidDescriptor(const json& object, std::string_view type)
		: JsonParseException{ object, std::format("Field 'descriptor_type' is '{}', which is not a valid descriptor.", type)}
	{
	}

	JsonInvalidEvent::JsonInvalidEvent(const json& object, std::string_view type)
		: JsonParseException{ object, std::format("Field 'event_type' is '{}', which is not a valid event.", type) }
	{
	}
}

namespace tournament_builder::json_helper::internal_json_helper
{
	using nlohmann::json;
	const json* get_optional_field(const json& object, std::string_view field_name)
	{
		if (!object.is_object())
		{
			return nullptr;
		}

		if (!object.contains(field_name))
		{
			return nullptr;
		}

		return &object[field_name];
	}

	// Range version
	bool is_one_of(const json& object, const auto& allowed_types)
	{
		const json::value_t type = object.type();
		return (std::ranges::find(allowed_types, type) != end(allowed_types));
	}

	// Template version
	template <json::value_t AllowedType, json::value_t...MoreAllowedTypes>
	bool is_one_of(const json& object)
	{
		const json::value_t type = object.type();
		if (type == AllowedType)
		{
			return true;
		}
		if constexpr (sizeof...(MoreAllowedTypes) > 0)
		{
			return is_one_of<MoreAllowedTypes...>(object);
		}
		return false;
	}

	template <typename T, json::value_t...AllowedTypes>
	class get_helper_basic
	{
	public:
		static std::optional<T> get_optional(const json& object, std::string_view field)
		{
			const json* result = internal_json_helper::get_optional_field(object, field);
			if (result == nullptr)
			{
				return std::nullopt;
			}

			if (!is_one_of<AllowedTypes...>(*result))
			{
				return std::nullopt;
			}

			return result->get<T>();
		}

		static T get_or(const json& object, std::string_view field, const T& alternative)
		{
			std::optional<T> result = get_optional(object, field);
			return result.value_or(alternative);
		}

		static T get(const json& object, std::string_view field)
		{
			std::optional<T> result = get_optional(object, field);
			if (!result.has_value())
			{
				tournament_builder::exception::JsonParseInvalidType exception(object, { static_cast<std::uint8_t>(AllowedTypes)...});
				exception.add_context(std::format("Field '{}' was not found or had the wrong type", field));
				throw exception;
			}
			return result.value();
		}
	};

	using string_getter = get_helper_basic<std::string, json::value_t::string>;
	using object_getter = get_helper_basic<json, json::value_t::object>;
	using any_getter = get_helper_basic<json, json::value_t::array, json::value_t::boolean, json::value_t::null, json::value_t::number_float, json::value_t::number_integer, json::value_t::number_unsigned, json::value_t::object, json::value_t::string>;
	using array_object_getter = get_helper_basic<json, json::value_t::array>;
	using bool_getter = get_helper_basic<json, json::value_t::boolean>;
	using int_getter = get_helper_basic<int, json::value_t::number_integer, json::value_t::number_unsigned>;
	using uint64_getter = get_helper_basic<uint64_t, json::value_t::number_integer, json::value_t::number_unsigned>;
}

namespace tournament_builder::json_helper
{
	using nlohmann::json;
	void validate_type(const json& object, std::initializer_list<json::value_t> allowed_types)
	{
		if (internal_json_helper::is_one_of(object, allowed_types))
		{
			// All is good.
			return;
		}

		throw_invalid_types_exception(object, allowed_types);
	}

	void json_helper::throw_invalid_types_exception(const json& object, std::initializer_list<json::value_t> allowed_types)
	{
		std::vector<std::uint8_t> type_arg;
		type_arg.reserve(allowed_types.size());
		std::ranges::transform(allowed_types, std::back_inserter(type_arg), [](json::value_t jv) { return static_cast<std::uint8_t>(jv); });
		throw exception::JsonParseInvalidType(object, type_arg);
	}

	std::optional<json> get_optional_any(const json& object, std::string_view field)
	{
		return internal_json_helper::any_getter::get_optional(object, field);
	}

	json get_any_or(const json& object, std::string_view field, const json& fallback)
	{
		return internal_json_helper::any_getter::get_or(object, field, fallback);
	}

	json get_any(const json& object, std::string_view field)
	{
		return internal_json_helper::any_getter::get(object, field);
	}

	std::optional<std::string> get_optional_string(const json& object, std::string_view field)
	{
		return internal_json_helper::string_getter::get_optional(object, field);
	}

	std::string get_string_or(const json& object, std::string_view field, std::string fallback)
	{
		return internal_json_helper::string_getter::get_or(object, field, fallback);
	}

	std::string get_string(const json& object, std::string_view field)
	{
		return internal_json_helper::string_getter::get(object, field);
	}

	std::optional<json> get_optional_object(const json& object, std::string_view field)
	{
		return internal_json_helper::object_getter::get_optional(object, field);
	}

	json get_object_or(const json& object, std::string_view field, const json& fallback)
	{
		return internal_json_helper::object_getter::get_or(object, field, fallback);
	}

	json get_object(const json& object, std::string_view field)
	{
		return internal_json_helper::object_getter::get(object, field);
	}

	std::optional<json> get_optional_array_object(const json& object, std::string_view field)
	{
		return internal_json_helper::array_object_getter::get_optional(object, field);
	}

	json get_array_object_or(const json& object, std::string_view field, const json& fallback)
	{
		return internal_json_helper::array_object_getter::get_or(object, field, fallback);
	}

	json get_array_object(const json& object, std::string_view field)
	{
		return internal_json_helper::array_object_getter::get(object, field);
	}

	std::optional<bool> get_optional_bool(const json& object, std::string_view field)
	{
		return internal_json_helper::bool_getter::get_optional(object, field);
	}

	bool get_bool_or(const json& object, std::string_view field, bool fallback)
	{
		return internal_json_helper::bool_getter::get_or(object, field, fallback);
	}

	bool get_bool(const json& object, std::string_view field)
	{
		return internal_json_helper::bool_getter::get(object, field);
	}

	std::optional<int> get_optional_int(const json& object, std::string_view field)
	{
		return internal_json_helper::int_getter::get_optional(object, field);
	}

	int get_int_or(const json& object, std::string_view field, int fallback)
	{
		return internal_json_helper::int_getter::get_or(object, field, fallback);
	}

	int get_int(const json& object, std::string_view field)
	{
		return internal_json_helper::int_getter::get(object, field);
	}

	std::optional<uint64_t> get_optional_uint64(const json& object, std::string_view field)
	{
		return internal_json_helper::uint64_getter::get_optional(object, field);
	}

	uint64_t get_uint64_or(const json& object, std::string_view field, uint64_t fallback)
	{
		return internal_json_helper::uint64_getter::get_or(object, field, fallback);
	}

	uint64_t get_uint64(const json& object, std::string_view field)
	{
		return internal_json_helper::uint64_getter::get(object, field);
	}
}