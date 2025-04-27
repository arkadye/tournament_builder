#pragma once

#include "tournament_builder/descriptors/descriptor_base.hpp"
#include "tournament_builder/reference.hpp"

namespace tournament_builder::descriptor
{
	// A very simple bracket: 1 plays 2; 3 plays 4; 5 plays 6; 7 plays 8; etc... then in the next round winner of 1&2 plays winner of 3&4; winner of 5&6 plays winner of 7&8 and so on.
	// Must have a power of 2 number of entries, but does accept and handle byes correctly. So more complex knockout structures can be created that way.
	class KnockoutBracket : public DescriptorBase<KnockoutBracket>
	{
	public:
		std::vector<Reference<Competitor>> entry_list;

		using DescriptorBase::DescriptorBase;
		Name get_descriptor_uid() const override { return Name{ "knockout_bracket" }; }
	private:
		std::optional<RealCompetition> generate() const override;
		DescriptorHandle parse(const nlohmann::json& input) const override;
	};
}