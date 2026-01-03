#pragma once

#include "tournament_builder/reference.hpp"
#include "tournament_builder/competitor.hpp"

#include <vector>

namespace tournament_builder
{
	class EntryList
	{
	public:
		std::vector<Reference<Competitor>> entries;
		int32_t min_entries = 0;
		int32_t max_entries = 0;
	};
}