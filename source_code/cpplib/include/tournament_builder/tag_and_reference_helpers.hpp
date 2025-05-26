#pragma once

#include <vector>
#include <cstdint>
#include <string_view>

namespace tournament_builder
{
	class Token;

	namespace helpers
	{
		struct GetWildcardArgsResult
		{
			std::string_view prefix;
			int64_t min{ 0 };
			int64_t max{ 0 };
		};

		enum class WildcardType
		{
			any, glob
		};

		GetWildcardArgsResult get_wildcard_args(const tournament_builder::Token& token, WildcardType type, char arg_delim);
	}
}