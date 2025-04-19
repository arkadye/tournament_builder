#include "tournament_builder/world.hpp"
#include "tournament_builder/competition.hpp"
#include "tournament_builder/events/event_base.hpp"

#include "tournament_builder/events/resolve_references_event.hpp"

#include "tournament_builder/json/json_helpers.hpp"

namespace tournament_builder
{
    namespace internal_world
    {
        void set_entry_tags(Competition& competition)
        {
            for (Competition& phase : competition.phases)
            {
                set_entry_tags(phase);
            }

            for (std::size_t i=0u;i<competition.entry_list.size();++i)
            {
                Reference<Competitor>& competitor_ref = competition.entry_list[i];
                if (competitor_ref.is_resolved())
                {
                    Competitor& competitor = competitor_ref.get();
                    competitor.add_tag(Tag{ std::format("$ENTRY:{}",i + 1) });
                }
            }
        }
    }

    World World::parse(const nlohmann::json& input)
    {
        Competition competition = Competition::parse(json_helper::get_object(input,"competition"));
        internal_world::set_entry_tags(competition);
        World result{ std::move(competition) };
        if (const auto& event_array_opt = json_helper::get_optional_array_object(input, "events"))
        {
            result.events = json_helper::get_array(event_array_opt.value(), &event::parse_event);
        }

        return result;
    }

    void World::resolve_all_references()
    {
        std::vector<Name> location;
        competition.resolve_all_references(*this, location);
    }

    void World::execute_all_events()
    {
        using namespace event;
        resolve_all_references();
        for (const EventHandle& ev : events)
        {
            ev->execute(*this);
        }
        if (!events.empty())
        {
            resolve_all_references();
            events.clear();
        }
    }
}
