#pragma once

#include "tournament_builder/token.hpp"

#include "nlohmann/json_fwd.hpp"

#include <vector>

namespace tournament_builder
{
	class Tag
	{
		std::vector<Token> m_data;
		static Tag parse(const nlohmann::json&, bool default_copy_on_ref);
	public:
		enum class TagType : std::int8_t
		{
			normal,
			pos,
			entry,
			invalid
		};

		TagType get_type() const;
		bool copy_on_reference = false;
		explicit Tag(std::string_view input, bool copy_on_reference_arg = false);
		bool operator==(const Tag& other) const noexcept;
		bool operator==(std::string_view other) const { return operator==(Tag{ other }); }
		bool operator==(Token other) const { return operator==(std::string{ other.to_string()}); }
		std::string to_string() const;
		static Tag parse_default_copy(const nlohmann::json& in) { return parse(in, true); }
		static Tag parse_default_no_copy(const nlohmann::json& in) { return parse(in, false); }
		bool collides_with(const Tag& other) const;
		bool collides_with(const std::vector<Tag>& others) const;
	};
}