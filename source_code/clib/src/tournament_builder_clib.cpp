#include "tournament_builder.h"

#include "tournament_builder.hpp"

#include <stdlib.h>

void tournament_builder_make_tournament_to_file(const char* input, const char* output_filename)
{
	tournament_builder::make_tournament_path(std::string_view{ input }, std::filesystem::path{ output_filename });
}

char* tournament_builder_make_tournament(const char* input)
{
	const std::string result_str = tournament_builder::make_tournament_str(std::string_view{ input });
	void* result_buffer = malloc((result_str.size() + 1) * sizeof(char));
	char* result = static_cast<char*>(result_buffer);
	if (result == nullptr)
	{
		return nullptr;
	}
	strcpy(result, result_str.c_str());
	return result;
}
