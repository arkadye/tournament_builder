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
	class RealCompetition;
	class ITaggable;
	class RealCompetitor;
	class Tag;
	class World;
	namespace descriptor::internal_descriptor
	{
		class DescriptorBaseImpl;
	}
}

namespace tournament_builder
{
	template <typename T> nlohmann::json to_json(const T& serializable);
	template <typename T> nlohmann::json to_json(const Reference<T>& reference);
	template <typename T> nlohmann::json to_json(const std::vector<T>& array);

	namespace internal_serialize
	{
		nlohmann::json to_json_internal(const NamedElement& type);
		nlohmann::json to_json_internal(const RealCompetitor& competitor);
		nlohmann::json to_json_internal(const Competitor& competitor);
		nlohmann::json to_json_internal(const RealCompetition& comp);
		nlohmann::json to_json_internal(const Competition& comp);
		nlohmann::json to_json_internal(const descriptor::internal_descriptor::DescriptorBaseImpl& desc);
		nlohmann::json to_json_internal(const SoftReference& sr);
		nlohmann::json to_json_internal(const Tag& tag);
		nlohmann::json to_json_internal(const World& world);
		nlohmann::json try_add_tags(const ITaggable* maybe_taggable, nlohmann::json input);
	}

	// TEMPLATE IMPLEMENTATIONS BELOW HERE

	template <typename T>
	inline nlohmann::json to_json(const T& serializable)
	{
		nlohmann::json result = internal_serialize::to_json_internal(serializable);
		if constexpr (std::is_polymorphic_v<T>)
		{
			result = internal_serialize::try_add_tags(dynamic_cast<const ITaggable*>(&serializable), std::move(result));
		}
		return result;
	}

	template <typename T>
	inline nlohmann::json to_json(const tournament_builder::Reference<T>& reference)
	{
		struct Impl
		{
			nlohmann::json operator()(const std::shared_ptr<IReferencable>& up_referencable) const
			{
				IReferencable* p_referencable = up_referencable.get();
				T* p_elem = p_referencable->get_as<T>();
				assert(p_elem != nullptr);
				return to_json(*p_elem);
			}
			nlohmann::json operator()(const SoftReference& elem)
			{
				return to_json(elem);
			}
		};
		return std::visit(Impl{}, reference.data());
	}

	template <typename T>
	inline nlohmann::json to_json(const std::vector<T>& range)
	{
		nlohmann::json result;
		for (const T& elem : range)
		{
			result.push_back(to_json(elem));
		}
		return result;
	}
}