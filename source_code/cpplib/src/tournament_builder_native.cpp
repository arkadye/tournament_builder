#include "tournament_builder_native.hpp"
#include "tournament_builder/exceptions.hpp"

#include "nlohmann/json.hpp"

#include <fstream>
#include <format>
#include <sstream>
#include <iostream>



namespace tournament_builder
{
    World make_tournament_world(std::string_view input, const ExtraArgs& extra_args)
    {
        if (input.ends_with(".json"))
        {
            std::filesystem::path filepath{ input };
            return make_tournament_world(filepath, extra_args);
        }

        std::istringstream in_stream{ std::string{input} };
        return make_tournament_world(in_stream, extra_args);
    }

    World make_tournament_world(const std::filesystem::path& input, const ExtraArgs& extra_args)
    {
        std::ifstream infile{ input };
        if (!infile.is_open())
        {
            throw exception::TournamentBuilderException{ std::format("Could not open input file '{}'", input.string()) };
        }
        try
        {
            return make_tournament_world(infile, extra_args);
        }
        catch (exception::TournamentBuilderException& ex)
        {
            ex.add_context(std::format("In file '{}'", input.string()));
            throw ex;
        }
    }

    World make_tournament_world(std::istream& input, const ExtraArgs& extra_args)
    {
        nlohmann::json in_json;
        try
        {
            input >> in_json;
        }
        catch (const nlohmann::json::exception& ex)
        {
            throw exception::TournamentBuilderException{ std::format("JSON parse exception: {}", ex.what()) };
        }
        return make_tournament_world(in_json, extra_args);
    }

    World make_tournament_world(const nlohmann::json& input, const ExtraArgs& extra_args)
    {
        using tournament_builder::World;
        const World parsed = World::parse(input);
        return make_tournament_world(parsed, extra_args);
    }

    World make_tournament_world(const World& input, const ExtraArgs& extra_args)
    {
        auto result = input;
        result.apply_extra_args(extra_args);
        result.execute_all_events();
        return result;
    }
}