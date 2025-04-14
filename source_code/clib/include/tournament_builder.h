#pragma once

/*
	This header file allows tournament_builder to be invoked from C. I am not particularly expecting
	anyone writing C to use this directly, but LUA and Python bindings to C interfaces are easier with a C interface.
*/

/*
	To compile for DLL export, set define TOURNAMENT_BUILDER_ENABLE_EXPORT_TO_DLL to 1.
*/

#ifdef TOURNAMENT_BUILDER_ENABLE_EXPORT_TO_DLL
#define TOURNAMENT_BUILDER_API __declspec(dllexport)
#else
#define TOURNAMENT_BUILDER_API
#endif

#ifdef __cplusplus
extern "C"
{
#endif
	/*
		Params:
			input: Either a JSON string or a filename containing JSON representing a World.
			output_filename: The file to write the output to.
	*/
	TOURNAMENT_BUILDER_API void tournament_builder_make_tournament_to_file(const char* input, const char* output_filename);

	/*
		Params:
			input: Either a JSON string or a filename containing JSON representing a World.
		Returns:
			A char* array containing a JSON object representing a World. This must have free() called on it by the user.
	*/
	TOURNAMENT_BUILDER_API char* tournament_builder_make_tournament(const char* input);
#ifdef __cplusplus
}
#endif

#undef TOURNAMENT_BUILDER_EXPORT