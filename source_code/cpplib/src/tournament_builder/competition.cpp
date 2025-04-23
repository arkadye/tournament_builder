#include "tournament_builder/competition.hpp"
#include "tournament_builder/descriptors/descriptor_base.hpp"
#include "tournament_builder/competitor.hpp"

#include "tournament_builder/json/json_helpers.hpp"

#include "nlohmann/json.hpp"

#include <algorithm>

namespace tournament_builder
{
	using nlohmann::json;
	bool Competition::has_finalized_entry_list(bool resursive) const
	{
		const bool contents_result = resursive && std::ranges::all_of(phases, [](const Competition& inner) {return inner.has_finalized_entry_list(true); });
		return contents_result && std::ranges::all_of(entry_list, &Reference<Competitor>::is_resolved);
	}

	bool Competition::resolve_all_references_impl(World& context, std::vector<Name>& location)
	{
		// In general, working outside-in will work better.
#ifndef NDEBUG
		auto location_copy = location;
#endif
		location.push_back(name);
		bool result = false;
		for (std::size_t i = 0u;i<entry_list.size();++i)
		{
			Reference<Competitor>& competitor_ref = entry_list[i];
			if (competitor_ref.is_reference())
			{
				result = competitor_ref.resolve(context, location) || result; // We DO NOT want to short circuit this.
				if (competitor_ref.is_resolved())
				{
					Competitor& competitor = competitor_ref.get();
					competitor.add_tag(Tag{ std::format("$ENTRY:{}", i + 1), false });
				}
			}
		}

		for (Competition& phase : phases)
		{
			result = phase.resolve_all_references_impl(context, location) || result;
		}

		location.pop_back();
#ifndef NDEBUG
		assert(location_copy == location);
#endif

		return result;
	}

	bool Competition::resolve_all_references(World& context, std::vector<Name>& location)
	{
		// Keep trying until we fail to make a change
		while (resolve_all_references_impl(context, location));
		return has_finalized_entry_list(true);
	}

	Competition Competition::parse(const json& input)
	{
		using namespace descriptor;
		json_helper::validate_type(input, json::value_t::object);

		// Try to see if we can get a structure out of this.
		if (DescriptorHandle descriptor = parse_descriptor(input))
		{
			return descriptor->generate_wrapper();
		}

		// It's not a descriptor, so parse it properly.
		const Name name = Name::parse(input);
		Competition result{ name };
		if (input.contains("phases"))
		{
			result.phases = json_helper::get_array(input["phases"], &Competition::parse);
		}
		if (input.contains("entry_list"))
		{
			result.entry_list = Competitor::parse_entry_list(input["entry_list"]);
		}
		result.add_tags_from_json(input);
		return result;
	}

	std::shared_ptr<IReferencable> Competition::copy_ref(const ReferenceCopyOptions&) const
	{
		Competition* copy = new Competition{ *this };
		IReferencable* icopy = copy;
		return std::shared_ptr<IReferencable>(icopy);
	}

	std::vector<IReferencable*> Competition::get_next_locations()
	{
		std::vector<IReferencable*> result;
		result.reserve(entry_list.size() + phases.size());

		std::ranges::transform(entry_list | std::views::filter(&Reference<Competitor>::is_resolved),
			std::back_inserter(result),
			[](Reference<Competitor>& ref_comp) -> IReferencable*
			{
				return &ref_comp.get();
			});
		std::ranges::transform(phases, std::back_inserter(result), [](Competition& comp) {return &comp; });
		return result;
	}
}