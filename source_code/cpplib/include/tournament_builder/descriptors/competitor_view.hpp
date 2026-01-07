#pragma once

#include "tournament_builder/descriptors/descriptor_base.hpp"

#include "tournament_builder/competitor.hpp"
#include "tournament_builder/reference.hpp"

#include <vector>


namespace tournament_builder::descriptor
{
	class CompetitorView : public DescriptorBase<CompetitorView>
	{
		using DescriptorBase::DescriptorBase;
		Name get_descriptor_uid() const override { return Name{ "competitor_view" }; }
		DescriptorHandle parse(std::shared_ptr<CompetitorView> prototype, const nlohmann::json& input) const override;
		void write_to_json(nlohmann::json& target) const override;
		void resolve_contained_references(World& world, Location& location) override;

		EntryList entry_list;
	protected:
		std::optional<RealCompetition> generate() const override;
	};
}