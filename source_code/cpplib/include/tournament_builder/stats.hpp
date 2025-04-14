#pragma once

#include "tournament_builder/name.hpp"

#include <vector>
#include <string>

namespace tournament_builder
{
	class BasicStat : public NamedElement
	{
		int64_t value = 0;
	};

	class DerivedStat : public NamedElement
	{
		std::string calculation;
	};
}