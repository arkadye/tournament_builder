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
            RealCompetition* p_rc = competition.get_real_competition();
            if (p_rc == nullptr) return;
            RealCompetition& rc = *p_rc;
            for (Competition& phase : rc.phases)
            {
                set_entry_tags(phase);
            }

            for (std::size_t i=0u;i<rc.entry_list.size();++i)
            {
                Reference<Competitor>& competitor_ref = rc.entry_list[i];
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
            try
            {
                result.events = json_helper::get_array(event_array_opt.value(), &event::parse_event);
            }
            catch (tournament_builder::exception::TournamentBuilderException& ex)
            {
                ex.add_context("While parsing events");
                throw ex;
            }
        }

        return result;
    }

    void World::resolve_all_references()
    {
        try
        {
            std::vector<Name> location;
            competition.resolve_all_references(*this, location);
        }
        catch (tournament_builder::exception::TournamentBuilderException& ex)
        {
            ex.add_context("In the world's resolve_all_references step");
            throw ex;
        }
    }

    void World::execute_all_events()
    {
        using namespace event;
        resolve_all_references();
        for (std::size_t idx{ 0u };idx < events.size();++idx)
        {
            const EventHandle& ev = events[idx];
            try
            {
                ev->execute(*this);
            }
            catch (tournament_builder::exception::TournamentBuilderException& ex)
            {
                ex.add_context(std::format("Executing event index={} ('{}')", idx, ev->get_event_uid()));
            }
        }
        if (!events.empty())
        {
            resolve_all_references();
            events.clear();
        }
    }

    Name World::get_reference_key() const
    {
        // This should never be called.
        assert(false && "We should never be asking the world for its reference key");
        return Name{"@ROOT"};
    }

    std::shared_ptr<IReferencable> World::copy_ref(const ReferenceCopyOptions&) const
    {
        assert(false && "We should enver be asking the world to copy itself via a reference.");
        return std::shared_ptr<IReferencable>{};
    }

    std::vector<IReferencable*> World::get_all_next_locations()
    {
        return { &competition };
    }

    bool World::matches_token(const Token&) const
    {
        assert(false && "We should never be asking the world if it matches with a token.");
        return false;
    }
}
