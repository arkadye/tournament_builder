#include "tournament_builder/events/set_finishing_positions_event.hpp"
#include "tournament_builder/competition.hpp"
#include "tournament_builder/world.hpp"

#include "tournament_builder/json/json_helpers.hpp"

#include <algorithm>

namespace tournament_builder::event
{

	EventHandle SetFinishingPositions::parse(const nlohmann::json& input) const
	{
		using nlohmann::json;
		SetFinishingPositions* result = new SetFinishingPositions;
		const json& ref = json_helper::get_any(input, "target");
		result->target = SoftReference::parse(ref);

		const json& finish_array = json_helper::get_array_object(input, "finish_order");
		for (const json& element : finish_array)
		{
			if (element.type() == json::value_t::string)
			{
				result->finish_order.emplace_back(Name::parse(element));
			}
			else if (element.type() == json::value_t::array)
			{
				EqualPlacedGroup group;
				group.reserve(element.size());
				std::ranges::transform(element, std::back_inserter(group), Name::parse);
				result->finish_order.emplace_back(std::move(group));
			}
			else
			{
				json_helper::throw_invalid_types_exception(element, { json::value_t::string, json::value_t::array });
			}
		}
		return EventHandle{ result };
	}

	void SetFinishingPositions::execute(World& world) const
	{
		execute(world, target, finish_order);
	}

	void SetFinishingPositions::execute(World& world, const SoftReference& target_ref, const std::vector<PlacementIndicator>& finish_order)
	{
		auto target_ref_result_vector = world.dereference<Competition>(target_ref);
		if (!target_ref_result_vector.size() == 1u)
		{
			throw exception::ExpectedSingleResult{ target_ref, target_ref_result_vector.size() };
		}

		ReferenceResult<Competition>& target_ref_result = target_ref_result_vector.front();
		assert(target_ref_result.get() != nullptr);

		Competition& competition_target = *target_ref_result.get();;

		if (!competition_target.resolve_all_references(world, target_ref_result.get_location()))
		{
			throw_execution_exception("Not all references in the target could be resolved.");
		}

		RealCompetition* p_target = competition_target.get_real_competition();
		assert(p_target != nullptr);
		RealCompetition& target = *p_target;

		struct Placement
		{
			Name name;
			std::string tag_str;
		};

		std::vector<Placement> placements;
		placements.reserve(target.entry_list.size());

		// Set up placements
		{
			std::size_t place = 1u;
			struct MakePlacements
			{
				std::size_t& place;
				std::vector<Placement>& placements;
				void operator()(Name name) const
				{
					placements.emplace_back(name, std::format("$POS:{}", place++));
				}
				void operator()(const EqualPlacedGroup& epg) const
				{
					const std::size_t last_place = place + epg.size() - 1;
					for (Name name : epg)
					{
						placements.emplace_back(name, std::format("$POS:{}:{}", place, last_place));
					}

					place = last_place + 1;
				}
			};

			MakePlacements make_placements{ place, placements };
			for (const PlacementIndicator& pi : finish_order)
			{
				std::visit(make_placements, pi);
			}
		}

		const auto placement_matches_competitor = [](const Placement& placement, const Reference<Competitor>& comp_ref)
			{
				const Competitor& competitor = comp_ref.get();
				return competitor.matches_name(placement.name);
			};

		for (const Placement& placement : placements)
		{
			auto matches_competitor = [&placement, &placement_matches_competitor](const Reference<Competitor>& comp_ref)
				{
					return placement_matches_competitor(placement, comp_ref);
				};
			const auto find_result = std::ranges::find_if(target.entry_list, matches_competitor);
			if (find_result == end(target.entry_list))
			{
				throw_execution_exception(std::format("Could not find {} in entry list. All elements must be in the entry list.", placement.name));
			}
		}

		for (Reference<Competitor>& entry_ref : target.entry_list)
		{
			Competitor& entry = entry_ref.get();
			if (entry.is_bye()) continue;

			auto matches_placement = [&entry_ref, &placement_matches_competitor](const Placement& placement)
				{
					return placement_matches_competitor(placement, entry_ref);
				};

			const auto find_result = std::ranges::find_if(placements, matches_placement);
			if (find_result != end(placements))
			{
				entry.add_tag(Tag{ find_result->tag_str , false });
			}
			else
			{
				throw_execution_exception(std::format("Entry {} was not placed. All entries which are not byes must be given a finishing position.", entry.to_string()));
			}
		}
	}
}