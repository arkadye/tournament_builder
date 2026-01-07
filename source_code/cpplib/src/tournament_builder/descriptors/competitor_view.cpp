#include "tournament_builder/descriptors/competitor_view.hpp"

#include "tournament_builder/json/json_helpers.hpp"
#include "tournament_builder/serialize.hpp"
#include "tournament_builder/exceptions.hpp"

#include "nlohmann/json.hpp"

namespace tournament_builder::descriptor
{
    DescriptorHandle CompetitorView::parse(std::shared_ptr<CompetitorView> prototype, const nlohmann::json& input) const
    {
        prototype->entry_list = EntryList::parse(input, "entry_list");
        return prototype;
    }

    void CompetitorView::write_to_json(nlohmann::json& target) const
    {
        add_name_and_descriptor_type_to_json(target);
        target["entry_list"] = to_json(entry_list);
    }

    void CompetitorView::resolve_contained_references(World& world, Location& location)
    {
        auto loc_push = LocationPusher{ location, name };
        entry_list.unpack_entry_list(world, location);
    }

    std::optional<RealCompetition> tournament_builder::descriptor::CompetitorView::generate() const
    {
        std::optional<RealCompetition> result{};
        if (entry_list.is_resolved())
        {
            result = RealCompetition{ name };
            result->entry_list = std::move(entry_list);
        }
        return result;
    }
}
