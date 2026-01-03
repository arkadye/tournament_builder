#pragma once

#include <vector>
#include <cstdint>
#include <string_view>

namespace tournament_builder
{
	class Token;

	namespace helpers
	{
		struct GetSpecialTagArgsResult
		{
			std::string_view prefix;
			int64_t min{ 0 };
			int64_t max{ 0 };
		};

		enum class SpecialTagType
		{
			any, glob, entry, pos
		};

		GetSpecialTagArgsResult get_special_tag_args(const tournament_builder::Token& token, SpecialTagType type, char arg_delim);
	}
}