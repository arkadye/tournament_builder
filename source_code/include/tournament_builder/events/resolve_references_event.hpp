#pragma once

#include "tournament_builder/events/event_base.hpp"
#include "tournament_builder/reference.hpp"

namespace tournament_builder
{
	class Competition;
}

namespace tournament_builder::event
{
	class ResolveAllReferences : public EventBase<ResolveAllReferences>
	{
	public:
		void execute(World& world) const override;
		Name get_event_uid() const override { return Name{ "resolve_all_references" }; }
		EventHandle parse(const nlohmann::json& input) const override;
	};

	class ResolveReference : public EventBase<ResolveReference>
	{
	public:
		void execute(World& world) const override;
		Name get_event_uid() const override { return Name{ "resolve_reference" }; }
		EventHandle parse(const nlohmann::json& input) const override;
		static void execute(World& world, const SoftReference& target);
		static void execute(World& world, ReferenceResult<Competition>& target);

		std::vector<SoftReference> target;
	};
}