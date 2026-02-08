#include "tournament_builder.h"
#include "tournament_builder.hpp"

#include <stdlib.h>

namespace tournament_builder
{
	namespace clib_internal
	{
		ExtraArgs make_extra_args(const TournamentBuilderExtraArgs* in)
		{
			return ExtraArgs{};
		}
	}
}

TournamentBuilderExtraArgs tournament_builder_make_extra_args()
{
	TournamentBuilderExtraArgs result;
	result.json_indent = -1;
	return result;
}

void tournament_builder_make_tournament_to_file(const char* input, const char* output_filename) { tournament_builder_make_tournament_to_file_ext(input, output_filename, nullptr); }
void tournament_builder_make_tournament_to_file_ext(const char* input, const char* output_filename, const TournamentBuilderExtraArgs* extra_args)
{
	tournament_builder::make_tournament_path(std::string_view{ input }, std::filesystem::path{ output_filename }, tournament_builder::clib_internal::make_extra_args(extra_args));
}

char* tournament_builder_make_tournament(const char* input) { return tournament_builder_make_tournament_ext(input, nullptr); }
char* tournament_builder_make_tournament_ext(const char* input, const TournamentBuilderExtraArgs* extra_args)
{
	const std::string result_str = tournament_builder::make_tournament_str(std::string_view{ input }, tournament_builder::clib_internal::make_extra_args(extra_args));
	const std::size_t result_buffer_size = (result_str.size() + 1) * sizeof(char);
	void* result_buffer = malloc(result_buffer_size);
	char* result = static_cast<char*>(result_buffer);
	if (result == nullptr)
	{
		return nullptr;
	}
	strcpy_s(result, result_buffer_size, result_str.c_str());
	return result;
}