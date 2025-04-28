#include "tournament_builder/serialize.hpp"
#include "tournament_builder/name.hpp"
#include "tournament_builder/competitor.hpp"
#include "tournament_builder/competition.hpp"
#include "tournament_builder/tag.hpp"
#include "tournament_builder/world.hpp"
#include "tournament_builder/descriptors/descriptor_base_internal.hpp"

#include "nlohmann/json.hpp"

namespace tournament_builder
{
	namespace internal_serialize
	{
		using nlohmann::json;
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

		json to_json_internal(const NamedElement& nt)
		{
			return internal_serialize::to_json_ne(nt);
		}

		json to_json_internal(const RealCompetitor& competitor)
		{
			using namespace internal_serialize;
			json result = to_json_ne(competitor);
			if (competitor.user_data.has_value())
			{
				result["user_data"] = competitor.user_data.value();
			}
			return result;
		}

		json to_json_internal(const Competitor& comp)
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

		json to_json_internal(const RealCompetition& comp)
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

		nlohmann::json to_json_internal(const Competition& comp)
		{
			struct Impl
			{
				nlohmann::json operator()(const descriptor::DescriptorHandle& dh) const
				{
					return to_json(*dh.get());
				}
				nlohmann::json operator()(const RealCompetition& rc) const
				{
					return to_json(rc);
				}
			};
			return std::visit(Impl{}, comp.get_data());
		}

		nlohmann::json to_json_internal(const descriptor::internal_descriptor::DescriptorBaseImpl& desc)
		{
			nlohmann::json result;
			desc.write_to_json(result);
			return result;
		}

		json to_json_internal(const SoftReference& sr)
		{
			return sr.to_string();
		}

		nlohmann::json to_json_internal(const Tag& tag)
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

		nlohmann::json to_json_internal(const World& world)
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

		nlohmann::json try_add_tags(const ITaggable* maybe_taggable, nlohmann::json input)
		{
			// Not everything can be converted to a taggable, so we expect this to be null sometimes.
			if (maybe_taggable == nullptr) return input;

			// Check to see if we did it already.
			if (input.contains("tags")) return input;

			// We don't want the entry tags to be copied - lots of noise and they don't play well with being on the input.
			// Ideally we want outputs to be directly useable as an input again without the user having to strip out these tags.
			std::vector<Tag> tags_copy;
			tags_copy.reserve(maybe_taggable->get_tags().size());
			std::ranges::copy_if(maybe_taggable->get_tags(), std::back_inserter(tags_copy), [](const Tag& t) {return t.get_type() != Tag::TagType::entry; });
			if (!tags_copy.empty())
			{
				input["tags"] = to_json_array(tags_copy);
			}
			return input;
		}
	}
}