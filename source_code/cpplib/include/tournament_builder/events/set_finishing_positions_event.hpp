#pragma once

#include "tournament_builder/events/event_base.hpp"
#include "tournament_builder/reference.hpp"

#include <vector>
#include <variant>

namespace tournament_builder
{
	class World;
	class SoftReference;
}

namespace tournament_builder::event
{
	class SetFinishingPositions : public EventBase<SetFinishingPositions>
	{
	public:
		using EqualPlacedGroup = std::vector<Name>;
		using PlacementIndicator = std::variant<EqualPlacedGroup, Name>;

		SoftReference target;
		std::vector<PlacementIndicator> finish_order;

		Name get_event_uid() const override { return Name{ "set_finishing_positions" }; }
		EventHandle parse(const nlohmann::json& input) const override;
		void execute(World& world) const;
		static void execute(World& world, const SoftReference& target, const std::vector<PlacementIndicator>& finish_order);
	};
}