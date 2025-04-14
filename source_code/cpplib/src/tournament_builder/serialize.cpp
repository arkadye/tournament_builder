#include "tournament_builder/serialize.hpp"
#include "tournament_builder/name.hpp"
#include "tournament_builder/competitor.hpp"
#include "tournament_builder/competition.hpp"
#include "tournament_builder/tag.hpp"
#include "tournament_builder/world.hpp"

#include "nlohmann/json.hpp"

namespace tournament_builder
{
	using nlohmann::json;
	namespace internal_serialize
	{
		json to_json_array(const auto& range)
		{
			json result;
			for (const auto& elem : range)
			{
				result.push_back(::tournament_builder::to_json(elem));
			}
			return result;
		}

		json to_json_ne(const NamedElement& ne)
		{
			return json{ {"name" , ne.name.to_string() } };
		}
	}

	json to_json(const NamedElement& nt)
	{
		return internal_serialize::to_json_ne(nt);
	}

	json to_json(const RealCompetitor& competitor)
	{
		using namespace internal_serialize;
		json result = to_json_ne(competitor);

		const std::vector<Tag>& tags = competitor.get_tags();
		std::vector<Tag> tags_copy;

		// We don't want the entry tags to be copied - lots of noise and they don't play well with being on the input.
		// Ideally we want outputs to be directly useable as an input again without the user having to strip out these tags.
		std::ranges::copy_if(tags, std::back_inserter(tags_copy), [](const Tag& t) {return t.get_type() != Tag::TagType::entry; });
		if (!tags_copy.empty())
		{
			result["tags"] = to_json_array(tags_copy);
		}
		return result;
	}

	json to_json(const Competitor& comp)
	{
		struct Serializer
		{
			json operator()(const RealCompetitor& comp) const
			{
				return to_json(comp);
			}
			json operator()(const Bye& bye) const
			{
				return nullptr;
			}
		};
		return std::visit(Serializer{}, comp.data());
	}

	json to_json(const Competition& comp)
	{
		using namespace internal_serialize;
		json result = to_json_ne(comp);
		if (!comp.entry_list.empty())
		{
			result["entry_list"] = to_json_array(comp.entry_list);
		}
		if (!comp.phases.empty())
		{
			result["phases"] = to_json_array(comp.phases);
		}
		return result;
	}

	json to_json(const SoftReference& sr)
	{
		return sr.to_string();
	}

	nlohmann::json to_json(const Tag& tag)
	{
		if (tag.copy_on_reference)
		{
			return tag.to_string();
		}
		json result;
		result["tag"] = tag.to_string();
		result["copy_on_ref"] = tag.copy_on_reference;
		return result;
	}

	nlohmann::json to_json(const World& world)
	{
		nlohmann::json result;
		if (!world.error_messages.empty()) [[unlikely]]
		{
			result["errors"] = world.error_messages;
			return result;
		}
		result["competition"] = to_json(world.competition);
		return result;
	}
}