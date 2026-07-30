#pragma once

/*
	This header file allows tournament_builder to be invoked from C. I am not particularly expecting
	anyone writing C to use this directly, but LUA and Python bindings to C interfaces are easier with a C interface.
*/

#if defined (_WIN32)
	#if defined IS_EXPORTING_TOURNAMENT_BUILDER_CLIB
		#define TOURNAMENT_BUILDER_CLIB_EXPORT __declspec(dllexport)
	#else
		#define TOURNAMENT_BUILDER_CLIB_EXPORT __declspec(dllimport)
	#endif
#else
	#define TOURNAMENT_BUILDER_CLIB_EXPORT
#endif

#if __cplusplus
extern "C"
{
#endif

	typedef struct
	{
		int json_indent;
		long long int random_seed;
	} TournamentBuilderExtraArgs;

	TournamentBuilderExtraArgs tournament_builder_make_extra_args();

	/*
		Params:
			input: Either a JSON string or a filename containing JSON representing a World.
			output_filename: The file to write the output to.
			extra_args: Extra arguments which affect how this element is used.
	*/
	TOURNAMENT_BUILDER_CLIB_EXPORT void tournament_builder_make_tournament_to_file(const char* input, const char* output_filename);
	TOURNAMENT_BUILDER_CLIB_EXPORT void tournament_builder_make_tournament_to_file_ext(const char* input, const char* output_filename, const TournamentBuilderExtraArgs* extra_args);

	/*
		Params:
			input: Either a JSON string or a filename containing JSON representing a World.
			extra_args: Extra arguments which affect how this element is used.
		Returns:
			A char* array containing a JSON object representing a World. This must have free() called on it by the user.
	*/
	TOURNAMENT_BUILDER_CLIB_EXPORT char* tournament_builder_make_tournament(const char* input);
	TOURNAMENT_BUILDER_CLIB_EXPORT char* tournament_builder_make_tournament_ext(const char* input, const TournamentBuilderExtraArgs* extra_args);

#if __cplusplus
}
#endif