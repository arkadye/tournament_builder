#pragma once

#include "tournament_builder/name.hpp"
#include "tournament_builder/reference.hpp"
#include "tournament_builder/competitor.hpp"

#include "nlohmann/json_fwd.hpp"

#include <vector>
#include <iosfwd>

namespace tournament_builder
{
	class World;

	// This can be a single match/race or a tournament_builder phase.
	class Competition : public NamedElement
	{
	public:
		std::vector<Reference<Competitor>> entry_list;
		std::vector<Competition> phases;

		// Returns true if all the entry list are actual Competitor types and not refrences.
		bool has_finalized_entry_list(bool recursive) const;

		// Returns true if it successfully resolves all the references.
		bool resolve_all_references(World& context, std::vector<Name>& location);

		static Competition parse(const nlohmann::json& input);

		Competition copy_ref() const { return *this; }

	private:
		// Returns true if it makes any changes
		bool resolve_all_references_impl(World& context, std::vector<Name>& location);
	};
}