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


/*
	Params:
		input: Either a JSON string or a filename containing JSON representing a World.
		output_filename: The file to write the output to.
*/
EXTERN_C_DECL TOURNAMENT_BUILDER_CLIB_EXPORT void tournament_builder_make_tournament_to_file(const char* input, const char* output_filename);

/*
	Params:
		input: Either a JSON string or a filename containing JSON representing a World.
	Returns:
		A char* array containing a JSON object representing a World. This must have free() called on it by the user.
*/
EXTERN_C_DECL TOURNAMENT_BUILDER_CLIB_EXPORT char* tournament_builder_make_tournament(const char* input);

#undef EXTERN_C_DECL