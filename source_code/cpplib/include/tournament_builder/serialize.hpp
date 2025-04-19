#pragma once

#include "tournament_builder/reference.hpp"

#include "nlohmann/json.hpp"


// These are not member functions because I don't want every serializable type to
// have to include the full JSON header.

namespace tournament_builder
{
	class NamedElement;
	class Competitor;
	class Competition;
	class RealCompetitor;
	class Tag;
	class World;
}

namespace tournament_builder
{
	nlohmann::json to_json(const NamedElement& type);
	nlohmann::json to_json(const RealCompetitor& competitor);
	nlohmann::json to_json(const Competitor& competitor);
	nlohmann::json to_json(const Competition& comp);
	nlohmann::json to_json(const SoftReference& sr);
	nlohmann::json to_json(const Tag& tag);
	nlohmann::json to_json(const World& world);

	template <typename T> nlohmann::json to_json(const Reference<T>& reference);

	// TEMPLATE IMPLEMENTATIONS BELOW HERE
	template <typename T>
	inline nlohmann::json to_json(const tournament_builder::Reference<T>& reference)
	{
		struct Impl
		{
			nlohmann::json operator()(const T& elem) const
			{
				return to_json(elem);
			}
			nlohmann::json operator()(const SoftReference& elem)
			{
				return to_json(elem);
			}
		};
		return std::visit(Impl{}, reference.data());
	}
}