#include "invoke_cli.h"

#include <filesystem>
#include <vector>
#include <algorithm>
#include <cassert>
#include <array>
#include <iostream>
#include <format>


#ifndef EXAMPLES_FOLDER_DEFAULT_LOCATION
#define EXAMPLES_FOLDER_DEFAULT_LOCATION "../../examples"
#endif

std::vector<char> make_c_str_vec(const auto& stringish);
std::pair<int, int> run_testcases_on_examples(auto& args_array, const std::filesystem::path& input_directory, const std::filesystem::path& output_directory);


// Set this to only run on files containing this pattern.
constexpr std::string_view PATTERN_TO_MATCH{ "json" };

// If true run as a test comparing against the output examples. Otherwise, write to them.
constexpr bool testing = true;

int main(int argc, char** argv)
{
	std::filesystem::path target{ EXAMPLES_FOLDER_DEFAULT_LOCATION };

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

	constexpr int num_args = 7;
	std::array<char*, num_args> args{};

	auto input_arg = make_c_str_vec("--input");
	auto output_arg = testing ? make_c_str_vec("--test-reference") : make_c_str_vec("--output");

	auto seed_arg = make_c_str_vec("--random-seed");
	auto seed_val = make_c_str_vec("42");

	args[0] = argv[0];
	args[1] = input_arg.data();
	args[3] = output_arg.data();
	args[5] = seed_arg.data();
	args[6] = seed_val.data();


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

	const auto [successes, failures] = run_testcases_on_examples(args, input_directory, output_directory);
	std::cout << std::format("Finished with {} successes and {} failures.\n", successes, failures);
	return 0;
}

std::vector<char> make_c_str_vec(const auto& stringish)
{
	std::vector<char> result;
	result.reserve(std::size(stringish) + 1);
	std::ranges::copy(stringish, std::back_inserter(result));
	result.push_back('\0');
	return result;
};

// Returns successes / failures
std::pair<int, int> run_testcases_on_examples(auto& args, const std::filesystem::path& input_directory, const std::filesystem::path& output_directory)
{
	using tournament_builder::tournament_builder_internal::invoke_cli;
	int successes = 0;
	int failures = 0;

	for (std::filesystem::path input_file : std::filesystem::directory_iterator{ input_directory })
	{
		std::filesystem::path filename = input_file.filename();
		if (std::filesystem::is_directory(input_file))
		{
			const std::filesystem::path in = input_directory / filename;
			const std::filesystem::path out = output_directory / filename;
			auto [subsuccesses, subfailures] = run_testcases_on_examples(args, in, out);
			successes += subsuccesses;
			failures += subfailures;
		}
		else
		{
			{
				const std::string filename_str = filename.generic_string();
				const std::string_view filename_sv{ filename_str };
				const std::size_t find_result = filename_sv.find(PATTERN_TO_MATCH);
				const bool found = find_result < filename_sv.size();
				if (!found)
				{
					std::cout << std::format("Skipping file '{}' - did not match pattern '{}'\n", filename_sv, PATTERN_TO_MATCH);
					continue;
				}
			}
			std::filesystem::path output_file = { output_directory / filename };

			auto infile_data = make_c_str_vec(input_file.generic_string());
			auto outfile_data = make_c_str_vec(output_file.generic_string());
			args[2] = infile_data.data();
			args[4] = outfile_data.data();
			std::cout << std::format("Processing {} and {} {}.\n", filename.generic_string(), testing ? "checking the result against" : "writing the result to", std::filesystem::absolute(output_file).generic_string());
			const bool success = (EXIT_SUCCESS == invoke_cli(static_cast<int>(args.size()), args.data()));
			success ? ++successes : ++failures;
		}
	}
	return { successes , failures };
}