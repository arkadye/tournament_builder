
#include <filesystem>
#include <iostream>
#include <fstream>

#include "cxxopts.hpp"

#include "tournament_builder.hpp"
#include "tournament_builder/random.hpp"

/* This has a dependancy on tournament_builder_static_lib which must be built and linked against me. */

namespace tournament_builder::tournament_builder_internal
{
	int invoke_cli(int argc, char** argv)
	{
		constexpr bool FORCE_RUN_TESTS = true;

		cxxopts::Options options_config{ "Tournament Builder (alpha)", "A tool to convert JSON tournament descriptors to a JSON representing the actual tournament,\n\tor for resolving events at that tournament." };
		options_config.add_options()
			("i,input", "Input file - JSON tournament descriptor. If not set, read from stdin.", cxxopts::value<std::filesystem::path>())
			("o,output", "Output file - JSON tournament OR an error. If not set, write to stdout", cxxopts::value<std::filesystem::path>())
			("r,random-seed", "Any random elements use this specific random seed", cxxopts::value<uint64_t>())
			("h,help", "Display help");

		auto options = options_config.parse(argc, argv);

		if (options.contains("help"))
		{
			std::cout << options_config.help() << '\n';
			return 0;
		}

		if (auto seed_opt = options["random-seed"].as_optional<uint64_t>())
		{
			tournament_builder::random::initialize_engine(*seed_opt);
		}

		struct SeedPrinter
		{
			~SeedPrinter()
			{
				std::cout << std::format("Random seed used: {}\n", tournament_builder::random::get_seed());
			}
		};
		SeedPrinter sp;

		std::ifstream input;
		std::ofstream output;

		if (const auto infile_opt = options["input"].as_optional<std::filesystem::path>())
		{
			const auto& infile = *infile_opt;
			input.open(infile);
			if (!input.is_open())
			{
				std::cerr << std::format("Cannot open input file {}.", std::filesystem::absolute(infile).string());
				return 1;
			}
		}

		if (const auto outfile_opt = options["output"].as_optional<std::filesystem::path>())
		{
			const auto& outfile = *outfile_opt;
			output.open(outfile);
			if (!output.is_open())
			{
				std::cerr << std::format("Cannot open output file {}.", std::filesystem::absolute(outfile).string());
				return 1;
			}
		}

		tournament_builder::make_tournament_ostream(input.is_open() ? input : std::cin, output.is_open() ? output : std::cout);

		return 0;
	}
}