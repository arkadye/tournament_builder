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
	Competition descriptor::RoundRobin::generate() const
	{
		if (num_times_to_play_each_opponent <= 0)
		{
			exception::InvalidArgument error{ std::format("Round-robin argument 'num_times_to_play_each_opponent' must be 1 or more.") };
			error.add_context(*this);
			throw error;
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

		Competition result{ name };
		if (entry_list.size() == 0u) return result;

		RoundOfMatches round{ "round_0" };
		round.generate_explicit_byes = generate_byes;
		round.entry_list = entry_list;

		// We're putting these into another sub-competition.
		add_outer_to_references(round.entry_list);

		if (round.entry_list.size() % 2u) // is odd
		{
			round.entry_list.emplace_back(Bye{});
		}

		const std::size_t rounds_per_repetition = round.entry_list.size() - 1;
		const std::size_t total_rounds = rounds_per_repetition * num_times_to_play_each_opponent;
		std::ranges::copy(entry_list, std::back_inserter(result.entry_list));
		result.phases.reserve(total_rounds);

		{
			if (shuffle_entries)
			{
				random::shuffle(round.entry_list);
			}

			// Generate the first round from scratch
			for (std::size_t i = 0u; i < rounds_per_repetition; ++i)
			{
				result.phases.emplace_back(round.generate());
				if (i < rounds_per_repetition - 1) [[likely]]
				{
					// Advance data to next round.
					round.name = Name{ std::format("round_{}", result.phases.size() + 1u) };
					std::rotate(begin(round.entry_list) + 1, begin(round.entry_list) + 2, end(round.entry_list));
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
				Competition round = result.phases[idx];
				round.name = Name{ std::format("round_{}", result.phases.size() + 1) };
				if (invert_left_and_right) [[unlikely]]
				{
					for (Competition& match : round.phases)
					{
						assert(match.entry_list.size() == 1u || match.entry_list.size() == 2u);
						if (match.entry_list.size() == 2u) [[likely]]
						{
							std::swap(match.entry_list[0], match.entry_list[1]);
						}
					}
				}
				result.phases.emplace_back(std::move(round));
			}
		}

		return result;
	}

	descriptor::DescriptorHandle descriptor::RoundRobin::parse(const nlohmann::json& input) const
	{
		const Name name = Name::parse(input);
		RoundRobin* result = new RoundRobin{ name };

		result->entry_list = Competitor::parse_entry_list(input, "entry_list");
		result->num_times_to_play_each_opponent = json_helper::get_int_or(input, "num_times_to_play_each_opponent", 1);
		result->alternate_left_and_right = json_helper::get_bool_or(input, "alternate_left_and_right", false);
		result->generate_byes = json_helper::get_bool_or(input, "generate_byes", false);
		result->shuffle_entries = json_helper::get_bool_or(input,"shuffle_entries", false);

		return DescriptorHandle{result};
	}
}