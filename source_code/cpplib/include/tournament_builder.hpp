#pragma once

#include <string>
#include <string_view>
#include <filesystem>
#include <iosfwd>

#include "nlohmann/json_fwd.hpp"

/* An interface using basic types, avoiding pulling in other heavy headers unnecessarily. */

namespace tournament_builder
{
	class World;
}

namespace tournament_builder
{
	/*
		Params:
			input: If the input ends in '.json', interpret as a file path. Otherwise interpret as JSON.
		Return: A JSON object as a std::string
	*/
	std::string make_tournament_str(std::string_view input);

	/*
		Params:
			input: A path to a JSON file.
		Return: The processed JSON as a std::string
	*/
	std::string make_tournament_str(const std::filesystem::path& input);

	/*
	Params:
		input: An input stream containing JSON
	Return: The processed JSON as a std::string
*/
	std::string make_tournament_str(std::istream& input);

	/*
		Params:
			input: An nlohmann::JSON oject
		Return: A JSON object as a std::string
	*/
	std::string make_tournament_str(const nlohmann::json& input);

	/*
		Params:
			input: A tournament_builder::World object describing a tourmanent.
		Return: A JSON object as a std::string
	*/
	std::string make_tournament_str(const tournament_builder::World& input);

	/*
		Params:
			input: If the input ends in '.json', interpret as a file path. Otherwise interpret as JSON.
			output: A path where the result will be written.
	*/
	void make_tournament_path(std::string_view input, const std::filesystem::path& output);

	/*
		Params:
			input:  A path to a JSON file.
			output: A path where the result will be written.
	*/
	void make_tournament_path(const std::filesystem::path& input, const std::filesystem::path& output);

	/*
		Params:
			input:  An input stream containing JSON
			output: A path where the result will be written.
	*/
	void make_tournament_path(std::istream& input, const std::filesystem::path& output);

	/*
		Params:
			input: An nlohmann::JSON oject
			output: A path where the result will be written.
	*/
	void make_tournament_path(const nlohmann::json& input, const std::filesystem::path& output);

	/*
		Params:
			input: A tournament_builder::World object describing a tourmanent.
			output: A path where the result will be written.
	*/
	void make_tournament_path(const tournament_builder::World& input, const std::filesystem::path& output);

	/*
		Params:
			input: If the input ends in '.json', interpret as a file path. Otherwise interpret as JSON.
			output: An output stream where the result will be written.
	*/
	void make_tournament_ostream(std::string_view input, std::ostream& output);

	/*
		Params:
			input: A path to a JSON file.
			output: An output stream where the result will be written.
	*/
	void make_tournament_ostream(const std::filesystem::path& input, std::ostream& output);

	/*
		Params:
			input: An input stream containing JSON
			output: An output stream where the result will be written.
	*/
	void make_tournament_ostream(std::istream& input, std::ostream& output);

	/*
		Params:
			input: An nlohmann::JSON oject
			output: An output stream where the result will be written.
	*/
	void make_tournament_ostream(const nlohmann::json& input, std::ostream& output);

	/*
		Params:
			input: A tournament_builder::World object describing a tourmanent.
			output: An output stream where the result will be written.
	*/
	void make_tournament_ostream(const tournament_builder::World& input, std::ostream& output);
}