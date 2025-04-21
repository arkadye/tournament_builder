#include "tournament_builder/events/resolve_references_event.hpp"
#include "tournament_builder/world.hpp"

#include "tournament_builder/json/json_helpers.hpp"

#include <cassert>

namespace tournament_builder
{

	void event::ResolveAllReferences::execute(World& world) const
	{
		world.resolve_all_references();
	}

	event::EventHandle event::ResolveAllReferences::parse(const nlohmann::json& input) const
	{
		verify_input(input);
		return EventHandle{ new ResolveAllReferences };
	}

	event::EventHandle event::ResolveReference::parse(const nlohmann::json& input) const
	{
		verify_input(input);
		ResolveReference* result = new ResolveReference;
		result->target = json_helper::get_array_maybe_from_single_item(input, "target", SoftReference::parse);
		return EventHandle{ result };
	}

	void tournament_builder::event::ResolveReference::execute(World& world) const
	{
		std::vector<ReferenceResult<Competition>> all_references;
		for (const SoftReference& ref : target)
		{
			utils::combine_vectors(all_references, world.dereference<Competition>(ref));
		}
		utils::remove_duplicated(all_references);
	}

	void event::ResolveReference::execute(World& world, const SoftReference& target)
	{
		auto reference_results = world.dereference<Competition>(target);
		for (auto& ref_result : reference_results)
		{
			execute(world, ref_result);
		}
	}

	void event::ResolveReference::execute(World& world, ReferenceResult<Competition>& target)
	{
		assert(target.get() != nullptr);
		target.get()->resolve_all_references(world, target.get_location());
	}
}