#include "tournament_builder/events/add_tags_event.hpp"
#include "tournament_builder/json/json_helpers.hpp"
#include "tournament_builder/reference.hpp"
#include "tournament_builder/tag.hpp"
#include "tournament_builder/world.hpp"
#include "tournament_builder/generic_utils.hpp"
#include "tournament_builder/competitor.hpp"

namespace tournament_builder::event
{
    EventHandle AddTags::parse(const nlohmann::json& input) const
    {
        verify_input(input);

        AddTags* result = new AddTags;
        result->target = json_helper::get_array_maybe_from_single_item(input, "target", SoftReference::parse);
        result->tags = json_helper::get_array_maybe_from_single_item(input, "tags", Tag::parse_default_no_copy);

        return EventHandle{ result };
    }

    void AddTags::execute(World& world) const
    {
        execute(world, target, tags);
    }

    void AddTags::execute(World& world, const std::vector<SoftReference>& target_refs, const std::vector<Tag>& tags)
    {
        std::vector<ReferenceResult<Competitor>> targets;
        for (const SoftReference& sf : target_refs)
        {
            std::vector<ReferenceResult<Competitor>> sub_targets = world.dereference<Competitor>(sf);
            utils::combine_vectors(targets, std::move(sub_targets));
        }
        utils::remove_duplicated(targets);

        for (ReferenceResult<Competitor>& competitor_ref : targets)
        {
            if (competitor_ref.get() != nullptr)
            {
                for (const Tag& tag : tags)
                {
                    competitor_ref.get()->add_tag(tag);
                }
            }
        }
    }
}