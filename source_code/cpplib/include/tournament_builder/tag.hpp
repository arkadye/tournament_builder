#pragma once

#include "tournament_builder/token.hpp"

#include "nlohmann/json_fwd.hpp"

#include <vector>

namespace tournament_builder
{
	class ReferenceCopyOptions;
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
			any,
			glob,
			invalid
		};

		TagType get_type() const;
		bool copy_on_reference = false;
		explicit Tag(std::string_view input, bool copy_on_reference_arg = false);
		bool operator==(const Tag& other) const noexcept;
		bool operator==(std::string_view other) const { return operator==(Tag{ other }); }
		bool operator==(Token other) const { return operator==(other.to_string()); }
		std::string to_string() const;
		static Tag parse_default_copy(const nlohmann::json& in) { return parse(in, true); }
		static Tag parse_default_no_copy(const nlohmann::json& in) { return parse(in, false); }
		bool collides_with(const Tag& other) const;
		bool collides_with(const std::vector<Tag>& others) const;
		static std::vector<Tag> copy_tags_on_ref(std::vector<Tag> in, const ReferenceCopyOptions& options);
	};
}