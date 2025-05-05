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
		DescriptorHandle parse(const nlohmann::json& input) const override;
		void write_to_json(nlohmann::json& target) const override;
		void resolve_contained_references(World& world, std::vector<Name>& location);

		static std::optional<std::vector<Reference<Competitor>>> unpack_entry_list(World& world, std::vector<Name>& location, const std::vector<Reference<Competitor>> in_list, int32_t min_entries, int32_t max_entries);

		std::vector<Reference<Competitor>> entry_list;
		int32_t min_entries = 0u;
		int32_t max_entries = 0u;
	protected:
		std::optional<RealCompetition> generate() const override;
	};
}