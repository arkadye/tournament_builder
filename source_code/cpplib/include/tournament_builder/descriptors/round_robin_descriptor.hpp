#pragma once

#include "tournament_builder/descriptors/descriptor_base.hpp"
#include "tournament_builder/reference.hpp"

#include "nlohmann/json_fwd.hpp"

#include <vector>

class tournament_builder::Competitor;

namespace tournament_builder::descriptor
{
	class RoundRobin : public DescriptorBase<RoundRobin>
	{
	public:
		using DescriptorBase::DescriptorBase;
		Name get_descriptor_uid() const override { return Name{ "round_robin" }; }
		std::vector<Reference<Competitor>> entry_list;
		int num_times_to_play_each_opponent = 1;
		bool alternate_left_and_right = false;
		bool shuffle_entries = false;
		bool generate_byes = false;

		Competition generate() const override;
		DescriptorHandle parse(const nlohmann::json& input) const override;
	};
}