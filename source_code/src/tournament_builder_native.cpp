#include "tournament_builder_native.hpp"
#include "tournament_builder/exceptions.hpp"

#include "nlohmann/json.hpp"

#include <fstream>
#include <format>
#include <sstream>
#include <iostream>



namespace tournament_builder
{
    tournament_builder::World make_tournament_world(std::string_view input)
    {
        if (input.ends_with(".json"))
        {
            std::filesystem::path filepath{ input };
            return make_tournament_world(filepath);
        }

        std::istringstream in_stream{ std::string{input} };
        return make_tournament_world(in_stream);
    }

    tournament_builder::World make_tournament_world(const std::filesystem::path& input)
    {
        std::ifstream infile{ input };
        if (!infile.is_open())
        {
            throw tournament_builder::exception::TournamentBuilderException{ std::format("Could not open input file '{}'", input.string()) };
        }
        return make_tournament_world(infile);
    }

    tournament_builder::World make_tournament_world(std::istream& input)
    {
        nlohmann::json in_json;
        try
        {
            input >> in_json;
        }
        catch (const nlohmann::json::exception& ex)
        {
            throw tournament_builder::exception::TournamentBuilderException{ std::format("JSON parse exception: {}", ex.what()) };
        }
        return make_tournament_world(in_json);
    }

    tournament_builder::World make_tournament_world(const nlohmann::json& input)
    {
        using tournament_builder::World;
        const World parsed = World::parse(input);
        return make_tournament_world(parsed);
    }

    tournament_builder::World make_tournament_world(const tournament_builder::World& input)
    {
        auto result = input;
        result.execute_all_events();
        return result;
    }
}