#include "tournament_builder/world.hpp"

#include "tournament_builder/extra_args.hpp"

#include <string_view>
#include <filesystem>

// Isolate out the version with the full tournament builder types so the user does not need to include it.

namespace tournament_builder
{
	/*
		Params:
			input: If the input ends in '.json', interpret as a file path. Otherwise interpret as JSON.
		Return: A tournament_builder::World type representing the outcome
	*/
	World make_tournament_world(std::string_view input, const ExtraArgs& extra_args);

	/*
		Params:
			input: A path to a JSON file.
		Return: A tournament_builder::World type representing the outcome
	*/
	World make_tournament_world(const std::filesystem::path& input, const ExtraArgs& extra_args);

	/*
		Params:
			input: An input stream containing JSON
		Return: A tournament_builder::World type representing the outcome
	*/
	World make_tournament_world(std::istream& input, const ExtraArgs& extra_args);

	/*
		Params:
			input: An nlohmann::JSON oject
		Return: A tournament_builder::World type representing the outcome
	*/
	World make_tournament_world(const nlohmann::json& input, const ExtraArgs& extra_args);

	/*
		Params:
			input: A tournament_builder::World object describing a tourmanent.
		Return: A tournament_builder::World type representing the outcome
	*/
	World make_tournament_world(const World& input, const ExtraArgs& extra_args);
}