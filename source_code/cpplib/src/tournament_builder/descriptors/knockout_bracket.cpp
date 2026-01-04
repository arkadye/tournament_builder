#include "tournament_builder/descriptors/knockout_bracket.hpp"
#include "tournament_builder/descriptors/round_of_matches_descriptor.hpp"

#include "tournament_builder/exceptions.hpp"

#include <cassert>

namespace tournament_builder::descriptor
{
	std::optional<RealCompetition> KnockoutBracket::generate() const
	{
		// Check for a power of 2 number of entries
		if (std::popcount(entry_list.size()) > 1u)
		{
			throw exception::InvalidArgument{ "knockout_bracket must have a power-of-two number of entries (i.e. 2, 4, 8, 16, ...)" };
		}

		int round_num = 0;

		RealCompetition result{ name };

		auto next_round_entries = entry_list;

		// We putting these into sub rounds
		add_outer_to_references(next_round_entries);

		while (next_round_entries.size() >= 2u)
		{
			RoundOfMatches round_desc{ std::format("round_{}", ++round_num) };
			round_desc.generate_explicit_byes = false;
			round_desc.entry_list = std::move(next_round_entries);

			next_round_entries = decltype(next_round_entries){};
			next_round_entries.reserve(round_desc.entry_list.size() / 2u);

			RealCompetition round = round_desc.generate().value() ;
			const std::string_view round_name = round.name.to_string();

			auto round_it = begin(round.phases);

			for (std::size_t idx = 0u; idx < round_desc.entry_list.size(); idx += 2u)
			{
				assert((idx + 1u) < round_desc.entry_list.size());
				const Reference<Competitor>& a = round_desc.entry_list[idx + 0u];
				const Reference<Competitor>& b = round_desc.entry_list[idx + 1u];

				const bool a_bye = Competitor::is_bye(a);
				const bool b_bye = Competitor::is_bye(b);
				if (!a_bye && !b_bye) // Use an actual match result. It *should* be the match pointed to by round_it.
				{
					assert(round_it != end(round.phases));
					next_round_entries.emplace_back(std::format("@OUTER.{}.{}.$POS:1", round_name, round_it->get_reference_key()));
					++round_it;
				}
				else if (a_bye != b_bye)
				{
					const Reference<Competitor>& non_bye = a_bye ? b : a;
					next_round_entries.emplace_back(non_bye);
				}
				else if (a_bye && b_bye) // Both entries are a bye.
				{
					next_round_entries.emplace_back(Competitor{ Bye{} });
				}
				else
				{
					assert(false); // Unreachable
				}
			}

			// We should have used all the matches.
			assert(round_it == end(round.phases));

			result.phases.emplace_back(std::move(round));
		}
		return result;
	}

	DescriptorHandle KnockoutBracket::parse(const nlohmann::json& input) const
	{
		verify_input(input);
		const Name name = Name::parse(input);
		auto result = std::make_shared<KnockoutBracket>(name);
		result->entry_list = EntryList::parse(input, "entry_list");
		return  result;
	}
}