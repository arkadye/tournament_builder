#pragma once

#include <tuple>
#include <filesystem>
#include <bitset>
#include <optional>

namespace tournament_builder
{
	// A way to pass in extra arguments. Typically these will be command line
	// arguments that modify behaviour.
	class ExtraArgs
	{
	public:
		std::optional<std::filesystem::path> path = {};
		std::optional<uint64_t> random_seed;
		int json_indent = -1;
	};
}