#pragma once

#include "tournament_builder/descriptors/descriptor_base.hpp"
#include "tournament_builder/reference.hpp"
#include "tournament_builder/competitor.hpp"
#include "tournament_builder/entry_list.hpp"

#include "nlohmann/json_fwd.hpp"

#include <vector>

namespace tournament_builder::descriptor
{
	class RoundOfMatches : public DescriptorBase<RoundOfMatches>
	{
	public:
		using DescriptorBase::DescriptorBase;
		Name get_descriptor_uid() const override { return Name{ "round_of_matches" }; }
		EntryList entry_list;
		bool swap_left_and_right = false;
		bool generate_explicit_byes = true;

		std::optional<RealCompetition> generate() const override;
	private:
		DescriptorHandle parse(std::shared_ptr<RoundOfMatches> prototype, const nlohmann::json& input, World& context) const override;
	};
}