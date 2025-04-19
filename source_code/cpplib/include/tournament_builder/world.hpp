#pragma once

#include "tournament_builder/competition.hpp"
#include "tournament_builder/reference.hpp"
#include "tournament_builder/events/event_base.hpp"

#include "nlohmann/json_fwd.hpp"

#include <optional>
#include <vector>
#include <string>
#include <cstdint>
#include <random>

namespace tournament_builder
{
	class World
	{
	public:
		World(Competition comp) : competition{ std::move(comp) } {}
		Competition competition;
		std::vector<event::EventHandle> events;
		
		std::vector<std::string> error_messages;
		static World parse(const nlohmann::json& input);

		void resolve_all_references();
		void execute_all_events();

		template <Referencable T>
		std::vector<ReferenceResult<T>> dereference(const SoftReference& soft_reference);
	};

	template <Referencable T>
	std::vector<ReferenceResult<T>> World::dereference(const SoftReference& soft_reference)
	{
		Reference<T> ref{ soft_reference };
		return ref.dereference_multi(*this, {});
	}
}