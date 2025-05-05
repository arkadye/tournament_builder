#include "tournament_builder/descriptors/competitor_view.hpp"

#include "tournament_builder/json/json_helpers.hpp"
#include "tournament_builder/serialize.hpp"
#include "tournament_builder/exceptions.hpp"

#include "nlohmann/json.hpp"

namespace tournament_builder::descriptor
{
    DescriptorHandle CompetitorView::parse(const nlohmann::json& input) const
    {
        verify_input(input);
        Name name = Name::parse(input);
        auto result = std::make_shared<CompetitorView>(name);
        result->entry_list = Competitor::parse_entry_list(input, "entry_list");

        std::optional<int32_t> expected_num_opt = json_helper::get_optional_int(input, "expected_num_entries");
        std::optional<int32_t> min_entries_opt = json_helper::get_optional_int(input, "min_entries");
        std::optional<int32_t> max_entries_opt = json_helper::get_optional_int(input, "max_entries");

        if (expected_num_opt.has_value())
        {
            if (min_entries_opt.has_value() || max_entries_opt.has_value())
            {
                throw exception::InvalidArgument{ "Descriptor type 'entry_list' cannot have 'expected_num_entries' with 'min_entries' or 'max_entries'" };
            }

            result->min_entries = result->max_entries = expected_num_opt.has_value();
        }
        else if (min_entries_opt.has_value() || max_entries_opt.has_value())
        {
            result->min_entries = min_entries_opt.value_or(0);
            result->max_entries = max_entries_opt.value_or(std::numeric_limits<int32_t>::max());
        }
        else
        {
            result->min_entries = result->max_entries = std::ssize(result->entry_list);
        }
        return result;
    }

    void CompetitorView::write_to_json(nlohmann::json& target) const
    {
        add_name_and_descriptor_type_to_json(target);
        target["entry_list"] = to_json(entry_list);
        const auto entry_list_size = std::ssize(entry_list);
        if (min_entries == max_entries)
        {
            const auto expected_entries = min_entries;
            if (entry_list_size != expected_entries)
            {
                target["expected_num_entries"] = expected_entries;
            }
        }
        else
        {
            target["min_entries"] = min_entries;
            target["max_entries"] = max_entries;
        }
    }

    void CompetitorView::resolve_contained_references(World& world, std::vector<Name>& location)
    {
        auto unpack_result = unpack_entry_list(world, location, entry_list, min_entries, max_entries);
        if (unpack_result.has_value())
        {
            entry_list = std::move(*unpack_result);
        }
    }
    std::optional<std::vector<Reference<Competitor>>> CompetitorView::unpack_entry_list(World& world, std::vector<Name>& location, const std::vector<Reference<Competitor>> in_list, int32_t min_entries, int32_t max_entries)
    {
        std::vector<Reference<Competitor>> entries;
        entries.reserve(max_entries);

        for (const Reference<Competitor>& rc : in_list)
        {
            assert(std::ssize(entries) <= max_entries);
            if (rc.is_resolved())
            {
                if (std::ssize(entries) >= max_entries) return std::nullopt;
                entries.push_back(rc);
                continue;
            }

            const std::vector<ReferenceResult<Competitor>> deref_result = rc.dereference_multi(world, location);

            for (const ReferenceResult<Competitor>& rrc : deref_result)
            {
                Competitor* result = rrc.get();
                assert(result != nullptr);
                const Name name = result->get_reference_key();
                auto matches_name = [&name](const Reference<Competitor>& elem)
                    {
                        if (elem.is_reference()) return false;
                        const Competitor& other = elem.get();
                        return other.matches_name(name);
                    };

                if (std::ranges::any_of(entries, matches_name) || std::ranges::any_of(in_list, matches_name)) continue;
                if (std::ssize(entries) >= max_entries) return std::nullopt;
                entries.emplace_back(std::move(*result));
            }
        }

        if (std::ssize(entries) < min_entries) return std::nullopt;
     
        return entries;
    }

    std::optional<RealCompetition> tournament_builder::descriptor::CompetitorView::generate() const
    {
        std::optional<RealCompetition> result{};
        if (std::ranges::all_of(entry_list, &Reference<Competitor>::is_resolved))
        {
            result = RealCompetition{ name };
            result->entry_list = std::move(entry_list);
        }
        return result;
    }
}
