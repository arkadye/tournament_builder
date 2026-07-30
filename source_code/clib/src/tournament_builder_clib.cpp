#include "tournament_builder.h"
//#include "tournament_builder.hpp"

#include <iostream>
#include <format>

TournamentBuilderExtraArgs tournament_builder_make_extra_args()
{
	TournamentBuilderExtraArgs result;
	result.json_indent = -1;
	result.random_seed = -1;
	return result;
}

void tournament_builder_make_tournament_to_file(const char* input, const char* output_filename) { tournament_builder_make_tournament_to_file_ext(input, output_filename, nullptr); }
void tournament_builder_make_tournament_to_file_ext(const char* input, const char* output_filename, const TournamentBuilderExtraArgs* extra_args)
{
	std::cout << std::format("Called tournament_builder_make_tournament_to_file with args input={} output={} and extras {}\n", input, output_filename, extra_args ? "set" : "not set");
}

char* tournament_builder_make_tournament(const char* input) { return tournament_builder_make_tournament_ext(input, nullptr); }
char* tournament_builder_make_tournament_ext(const char* input, const TournamentBuilderExtraArgs* extra_args)
{
	std::cout << std::format("Called tournament_builder_make_tournament with args input={} and extras {}\n", input, extra_args ? "set" : "not set");
	static char dummy[] = "Hello World!";
	return dummy;
}