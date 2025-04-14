#include "tournament_builder/events/event_base.hpp"

namespace tournament_builder::event
{
    EventHandle tournament_builder::event::parse_event(const nlohmann::json& input)
    {
        return internal_event::EventRegister::parse_event(input);
    }
}
