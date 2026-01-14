#include "tournament_builder/descriptors/round_robin_descriptor.hpp"
#include "tournament_builder/descriptors/round_of_matches_descriptor.hpp"
#include "tournament_builder/random.hpp"
#include "tournament_builder/exceptions.hpp"
#include "tournament_builder/json/json_helpers.hpp"

#include "nlohmann/json.hpp"

#include <cassert>
#include <numeric>
#include <algorithm>

namespace tournament_builder
{
	std::optional<RealCompetition> descriptor::RoundRobin::generate() const
	{
		if (num_times_to_play_each_opponent <= 0)
		{
			exception::InvalidArgument error{ std::format("Round-robin argument 'num_times_to_play_each_opponent' was {}. Must be 1 or more, or not set.", num_times_to_play_each_opponent) };
			error.add_context(*this);
			throw error;
		}

		// We need the size to be set first.
		if (!entry_list.has_fixed_length())
		{
			return std::nullopt;
		}

		for (std::size_t idx = 0u; idx < entry_list.size(); ++idx)
		{
			if (Competitor::is_bye(entry_list[idx]))
			{
				exception::InvalidArgument error{ std::format("Round-robin entry list must not contain byes ('null' values). Bye found at position {}", idx) };
				error.add_context(*this);
				throw error;
			}
		}

		RealCompetition result{ name };
		result.entry_list = entry_list;

		// We cannot rely on the actual list having all references resolved, so create one out of references to the round-robin's entry list.
		std::vector<Reference<Competitor>> entries;
		for (int i = 0; i < entry_list.get_min_entries(); ++i)
		{
			
			entries.emplace_back(std::format("@OUTER.$ENTRY:{}", i + 1));
		}

		if (entries.size() == 0u) return result;

		// If we have an odd number, add a bye to the end.
		if (entries.size() % 2u) // is odd
		{
			entries.emplace_back(Bye{});
		}

		RoundOfMatches round{ "round_0" };
		round.generate_explicit_byes = generate_byes;
		round.entry_list.set(std::move(entries));

		const std::size_t rounds_per_repetition = round.entry_list.size() - 1;
		const std::size_t total_rounds = rounds_per_repetition * num_times_to_play_each_opponent;
		result.phases.reserve(total_rounds);

		{
			// Generate the first round from scratch
			for (std::size_t i = 0u; i < rounds_per_repetition; ++i)
			{
				auto round_opt = round.generate_wrapper();
				assert(round_opt.has_value());
				result.phases.emplace_back(Competition{ std::move(round_opt.value()) });
				if (i < rounds_per_repetition - 1) [[likely]]
				{
					// Advance data to next round.
					round.name = Name{ std::format("round_{}", result.phases.size() + 1u) };
					auto& entries = round.entry_list.get_entries();
					std::rotate(begin(entries) + 1, begin(entries) + 2, end(entries));
					round.swap_left_and_right = alternate_left_and_right && !round.swap_left_and_right;
				}
			}
		}

		// Generate subsquent founds from the already generated data.
		for (int rep = 1; rep < num_times_to_play_each_opponent; ++rep)
		{
			const bool invert_left_and_right = alternate_left_and_right && (rep % 2);
			for (std::size_t idx = 0u; idx < rounds_per_repetition; ++idx)
			{
				assert(result.phases[idx].is_resolved());
				Competition comp_round = result.phases[idx].get();
				RealCompetition* p_round = comp_round.get_real_competition();
				assert(p_round != nullptr);
				RealCompetition& round = *p_round;
				round.name = Name{ std::format("round_{}", result.phases.size() + 1) };
				if (invert_left_and_right) [[unlikely]]
				{
					for (Reference<Competition>& comp_match_ref : round.phases)
					{
						assert(comp_match_ref.is_resolved());
						Competition& comp_match = comp_match_ref.get();
						RealCompetition* p_match = comp_match.get_real_competition();
						assert(p_match != nullptr);
						RealCompetition& match = *p_match;
						 assert(match.entry_list.size() == 1u || match.entry_list.size() == 2u);
						if (match.entry_list.size() == 2u) [[likely]]
						{
							std::swap(match.entry_list[0], match.entry_list[1]);
						}
					}
				}
				result.phases.emplace_back(Competition{ std::move(round) });
			}
		}

		return result;
	}

	descriptor::DescriptorHandle descriptor::RoundRobin::parse(std::shared_ptr<RoundRobin> prototype, const nlohmann::json& input) const
	{
		prototype->entry_list = EntryList::parse(input, "entry_list");
		prototype->num_times_to_play_each_opponent = json_helper::get_int_or(input, "num_times_to_play_each_opponent", 1);
		prototype->alternate_left_and_right = json_helper::get_bool_or(input, "alternate_left_and_right", false);
		prototype->generate_byes = json_helper::get_bool_or(input, "generate_byes", false);
		prototype->shuffle_entries = json_helper::get_bool_or(input,"shuffle_entries", false);

		return prototype;
	}
}