#include <string_view>
#include <filesystem>
#include <iostream>

#include "tournament_builder/extra_args.hpp"

#include "nlohmann/json.hpp"

// Separate the versions needing the full nlohmann/json header the user does not have to include it if they don't need or want it.

namespace tournament_builder
{
	class World;
}

namespace tournament_builder
{
	/*
		Params:
			input: If the input ends in '.json', interpret as a file path. Otherwise interpret as JSON.
		Return: An nlohmann JSON object
	*/
	nlohmann::json make_tournament_json(std::string_view input, const ExtraArgs& extra_args);

	/*
		Params:
			input: A path to a JSON file.
		Return: An nlohmann JSON object
	*/
	nlohmann::json make_tournament_json(const std::filesystem::path& input, const ExtraArgs& extra_args);

	/*
		Params:
			input: A path to a JSON file.
		Return: An nlohmann JSON object
	*/
	nlohmann::json make_tournament_json(std::istream& input, const ExtraArgs& extra_args);

	/*
		Params:
			input: An nlohmann::JSON oject
		Return: An nlohmann JSON object
	*/
	nlohmann::json make_tournament_json(const nlohmann::json& input, const ExtraArgs& extra_args);

	/*
		Params:
			input: A tournament_builder::World object describing a tourmanent.
		Return: An nlohmann JSON object
	*/
	nlohmann::json make_tournament_json(const World& input, const ExtraArgs& extra_args);
}