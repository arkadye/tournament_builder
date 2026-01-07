#include "tournament_builder/descriptors/round_of_matches_descriptor.hpp"
#include "tournament_builder/json/json_helpers.hpp"

#include "nlohmann/json.hpp"

#include <string>
#include <utility>
#include <cassert>

namespace tournament_builder::descriptor
{
    std::optional<RealCompetition> RoundOfMatches::generate() const
    {
        RealCompetition result{ name };
       
        std::vector<Reference<Competitor>> competitors = entry_list.get_entries();
        add_outer_to_references(competitors);

        if (swap_left_and_right)
        {
            for (std::size_t idx = 0u; idx < competitors.size(); idx += 2)
            {
                if ((idx + 1) < competitors.size()) [[likely]]
                {
                    std::swap(competitors[idx + 0], competitors[idx + 1]);
                }
            }
        }

        if (competitors.size() % 2u) // We have an odd number of competitors.
        {
            competitors.emplace_back(Bye{});
        }

        assert(competitors.size() % 2u == 0u);

        result.phases.reserve(competitors.size() / 2);
        for (std::size_t idx = 0u; idx < competitors.size(); idx += 2u)
        {
            Reference<Competitor> left = std::move(competitors[idx + 0]);
            Reference<Competitor> right = std::move(competitors[idx + 1]);
            const bool lbye = Competitor::is_bye(left);
            const bool rbye = Competitor::is_bye(right);

            if (lbye && rbye)
            {
                continue;
            }
            if (!generate_explicit_byes && (lbye || rbye))
            {
                continue;
            }
            if (lbye) // && !rbye - or else we would have continue'd already
            {
                std::swap(left, right); // Move the bye to the right.
            }
            const auto game_idx = result.phases.size();
            const std::string game_name_str = std::format("game_{}", result.phases.size() + 1u);
            RealCompetition game{ game_name_str };
            game.entry_list.set({ left, right });

            result.phases.emplace_back(std::move(game));
        }

        return result;
    }

    DescriptorHandle RoundOfMatches::parse(std::shared_ptr<RoundOfMatches> prototype, const nlohmann::json& input) const
    {
        prototype->entry_list = EntryList::parse(input, "entry_list");
        prototype->swap_left_and_right = json_helper::get_bool_or(input, "swap_left_and_right", false);
        prototype->generate_explicit_byes = json_helper::get_bool_or(input, "generate_explicit_byes", true);

        return prototype;
    }
}