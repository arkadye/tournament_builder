
#include <filesystem>
#include <iostream>
#include <fstream>

#include "cxxopts.hpp"

#include "tournament_builder.hpp"
#include "tournament_builder/random.hpp"

#include <variant>

/* This has a dependancy on tournament_builder_static_lib which must be built and linked against me. */

namespace tournament_builder::tournament_builder_internal
{
	int invoke_cli(int argc, char** argv)
	{
		using Path = std::filesystem::path;

		cxxopts::Options options_config{ "Tournament Builder (pre-alpha)", "A tool to convert JSON tournament descriptors to a JSON representing the actual tournament,\n\tor for resolving events at that tournament." };
		options_config.add_options()
			("i,input", "Input file - JSON tournament descriptor. If not set, read from stdin.", cxxopts::value<Path>())
			("o,output", "Output file - JSON tournament OR an error. If not set, write to stdout.", cxxopts::value<Path>())
			("t,test-reference", "Test reference - if set the output is not written. Instead it is compared to this file and reports whether or not they are equivalent.", cxxopts::value<Path>())
			("r,random-seed", "Any random elements use this specific random seed. This overrides a random seed set in data.", cxxopts::value<uint64_t>())
			("h,help", "Display help");

		auto options = options_config.parse(argc, argv);

		if (options.contains("help"))
		{
			std::cout << options_config.help() << '\n';
			return EXIT_SUCCESS;
		}

		if (auto seed_opt = options["random-seed"].as_optional<uint64_t>())
		{
			random::initialize_engine(*seed_opt); 
		}

		struct SeedPrinter
		{
			~SeedPrinter()
			{
				std::cout << std::format("Random seed used: {}\n", random::get_seed());
			}
		};
		SeedPrinter sp;

		std::variant<Path, std::istream*> input{ &std::cin };
		std::variant<Path, std::ostream*> output{ &std::cout };
		
		if (const auto infile_opt = options["input"].as_optional<std::filesystem::path>())
		{
			input = *infile_opt;

			if (const auto testref_opt = options["test-reference"].as_optional<std::filesystem::path>())
			{
				const auto& reference = *testref_opt;
				const bool passes = tournament_builder::test_input_file(*infile_opt, reference);
				return passes ? EXIT_SUCCESS : EXIT_FAILURE;
			}
		}

		if (const auto outfile_opt = options["output"].as_optional<std::filesystem::path>())
		{
			output = *outfile_opt;
		}

		ExtraArgs extras; // Populate this as needed from the command line.

		// This method makes the compiler check that I remembered all combinations.
		struct Picker
		{
			const ExtraArgs& ea;
			void operator()(const Path& in, const Path& out) { return make_tournament_path(in, out, ea); }
			void operator()(const Path& in, std::ostream* out) { return make_tournament_ostream(in, *out, ea); }
			void operator()(std::istream* in, const Path& out) { return make_tournament_path(*in, out, ea); }
			void operator()(std::istream* in, std::ostream* out) { return make_tournament_ostream(*in, *out, ea); }
		};

		std::visit(Picker{ extras }, input, output);

		return EXIT_SUCCESS;
	}
}