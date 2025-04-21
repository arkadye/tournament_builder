#include "invoke_cli.h"

#include <filesystem>
#include <vector>
#include <algorithm>
#include <cassert>
#include <array>
#include <iostream>
#include <format>

// Set this to only run on files containing this pattern.
constexpr std::string_view PATTERN_TO_MATCH{ ".json" };

// If true run as a test comparing against the output examples. Otherwise, write to them.
constexpr bool testing = true;

int main(int argc, char** argv)
{
	using tournament_builder::tournament_builder_internal::invoke_cli;

	std::filesystem::path target{ "../../examples" };
	if (argc > 1)
	{
		target = argv[1];
	}

	if (!std::filesystem::exists(target))
	{
		std::cerr << std::format("Cannot run dbg. Path {} does not exist.\n", std::filesystem::absolute(target).generic_string());
		return 1;
	}

	if (!std::filesystem::is_directory(target))
	{
		std::cerr << std::format("Cannot run dbg. {} is not a path.\n", std::filesystem::absolute(target).generic_string());
		return 1;
	}

	constexpr int num_args = 5;
	std::array<char*, num_args> args{};

	auto make_c_str_vec = [](const auto& stringish) -> std::vector<char>
		{
			std::vector<char> result;
			result.reserve(std::size(stringish) + 1);
			std::ranges::copy(stringish, std::back_inserter(result));
			result.push_back('\0');
			return result;
		};

	auto input_arg = make_c_str_vec("--input");
	auto output_arg = testing ? make_c_str_vec("--test-reference") : make_c_str_vec("--output");

	args[0] = argv[0];
	args[1] = input_arg.data();
	args[3] = output_arg.data();

	const std::filesystem::path input_directory = target / "in";

	if (!std::filesystem::exists(input_directory))
	{
		std::cerr << std::format("Cannot run dbg. Path {} does not exist.\n", std::filesystem::absolute(input_directory).generic_string());
		return 1;
	}

	if (!std::filesystem::is_directory(input_directory))
	{
		std::cerr << std::format("Cannot run dbg. {} is not a path.\n", std::filesystem::absolute(input_directory).generic_string());
		return 1;
	}

	const std::filesystem::path output_directory = target / "out";
	std::filesystem::create_directories(output_directory);


	for (std::filesystem::path input_file : std::filesystem::directory_iterator{ input_directory })
	{
		std::filesystem::path filename = input_file.filename();
		{
			const std::string filename_str = filename.generic_string();
			const std::string_view filename_sv{ filename_str };
			const std::size_t find_result = filename_sv.find(PATTERN_TO_MATCH);
			const bool found = find_result < filename_sv.size();
			if (!found)
			{
				std::cout << std::format("Skipping file '{}' - did not match pattern '{}'\n", filename_sv, PATTERN_TO_MATCH);
			}
		}
		std::filesystem::path output_file = { target / "out" / filename };
		
		auto infile_data = make_c_str_vec(input_file.generic_string());
		auto outfile_data = make_c_str_vec(output_file.generic_string());
		args[2] = infile_data.data();
		args[4] = outfile_data.data();
		std::cout << std::format("Processing {} and {} {}.\n", filename.generic_string(), testing ? "checking the result against" : "writing the result to", std::filesystem::absolute(output_file).generic_string());
		invoke_cli(num_args, args.data());
	}
	return 0;
}