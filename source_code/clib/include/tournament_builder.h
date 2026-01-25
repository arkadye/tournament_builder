#pragma once

/*
	This header file allows tournament_builder to be invoked from C. I am not particularly expecting
	anyone writing C to use this directly, but LUA and Python bindings to C interfaces are easier with a C interface.
*/

#include "tournament_builder/clib_export.h"

#if __cplusplus
#define EXTERN_C_DECL extern "C"
#else
#define EXTERN_C_DECL
#endif

typedef struct
{

} TournamentBuilderExtraArgs;


/*
	Params:
		input: Either a JSON string or a filename containing JSON representing a World.
		output_filename: The file to write the output to.
		extra_args: Extra arguments which affect how this element is used.
*/
EXTERN_C_DECL TOURNAMENT_BUILDER_CLIB_EXPORT void tournament_builder_make_tournament_to_file(const char* input, const char* output_filename);
EXTERN_C_DECL TOURNAMENT_BUILDER_CLIB_EXPORT void tournament_builder_make_tournament_to_file_ext(const char* input, const char* output_filename, const TournamentBuilderExtraArgs* extra_args);

/*
	Params:
		input: Either a JSON string or a filename containing JSON representing a World.
		extra_args: Extra arguments which affect how this element is used.
	Returns:
		A char* array containing a JSON object representing a World. This must have free() called on it by the user.
*/
EXTERN_C_DECL TOURNAMENT_BUILDER_CLIB_EXPORT char* tournament_builder_make_tournament(const char* input);
EXTERN_C_DECL TOURNAMENT_BUILDER_CLIB_EXPORT char* tournament_builder_make_tournament_ext(const char* input, const TournamentBuilderExtraArgs* extra_args);

#undef EXTERN_C_DECL