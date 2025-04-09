#pragma once

#include "tournament_builder/events/event_base.hpp"
#include "tournament_builder/tag.hpp"

namespace tournament_builder
{
	class SoftReference;
	class World;
}

namespace tournament_builder::event
{
	class AddTags : public EventBase<AddTags>
	{
	public:
		EventHandle parse(const nlohmann::json& input) const override;
		void execute(World& world) const;
		static void execute(World& world, const std::vector<SoftReference>& target, const std::vector<Tag>& tag);
		Name get_event_uid() const override { return Name{ "add_tags" }; }

		std::vector<SoftReference> target;
		std::vector<Tag> tags;
	};
}