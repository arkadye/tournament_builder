#include "tournament_builder/competitor.hpp"

#include "tournament_builder/json/json_helpers.hpp"

#include "nlohmann/json.hpp"

#include <algorithm>

namespace tournament_builder
{
	using nlohmann::json;
	void RealCompetitor::add_tag(Tag tag)
	{
		if (!tag.collides_with(m_tags))
		{
			m_tags.push_back(std::move(tag));
		}
	}

	RealCompetitor RealCompetitor::copy_ref() const
	{
		RealCompetitor result{ name };
		for (const Tag& tag : m_tags)
		{
			if (tag.copy_on_reference)
			{
				result.add_tag(tag);
			}
		}
		return result;
	}

	Competitor Competitor::copy_ref() const
	{
		struct Impl
		{
			Competitor operator()(Bye) const { return Bye{}; }
			Competitor operator()(const RealCompetitor& rc) const { return rc.copy_ref(); }
		};
		return std::visit(Impl{}, m_data);
	}

	void Competitor::add_tag(Tag tag)
	{
		if (RealCompetitor* prc = std::get_if<RealCompetitor>(&m_data))
		{
			prc->add_tag(std::move(tag));
		}
	}

	std::string_view Competitor::to_string() const
	{
		struct Impl
		{
			std::string_view operator()(const Bye&) const { return "[bye]"; }
			std::string_view operator()(const RealCompetitor& rc) const { return rc.name.to_string(); }
		};
		return std::visit(Impl{}, m_data);
	}

	bool Competitor::is_bye(const Reference<Competitor>& ref)
	{
		if (ref.is_reference())
		{
			return false;
		}
		return ref.get().is_bye();
	}

	bool Competitor::matches_token(Token token) const
	{
		struct Impl
		{
			Token t;
			bool operator()(const RealCompetitor& rc) const
			{
				if (rc.name == t) return true;

				const auto find_result = std::ranges::find(rc.get_tags(), Tag{t});
				return find_result != end(rc.get_tags());
			}
			bool operator()(const Bye& b) const {return false; }
		};
		return std::visit(Impl{ token }, m_data);
	}

	bool Competitor::matches_name(Name name) const
	{
		struct Impl
		{
			Name n;
			bool operator()(const RealCompetitor& rc) const
			{
				return rc.name == n;
			}
			bool operator()(const Bye& b) const { return false; }
		};
		return std::visit(Impl{ name }, m_data);
	}

	Competitor Competitor::parse(const json& input)
	{
		json_helper::validate_type(input, { json::value_t::object , json::value_t::null });
		if (input.is_null())
		{
			return Bye{};
		}

		return RealCompetitor::parse(input);
	}

	std::vector< Reference<Competitor>> Competitor::parse_entry_list(const json& input)
	{
		return json_helper::get_array(input, Reference<Competitor>::parse);
	}

	std::vector< Reference<Competitor>> Competitor::parse_entry_list(const json& input, std::string_view field_name)
	{
		json array_view = json_helper::get_array_object(input, field_name);
		return parse_entry_list(array_view);
	}

	RealCompetitor RealCompetitor::parse(const json& input)
	{
		json_helper::validate_type(input, { json::value_t::object });
		const Name name = Name::parse(input);
		RealCompetitor result{ name };
		if (input.contains("tags"))
		{
			std::vector<Tag> found_tags = json_helper::get_array(input["tags"], &Tag::parse_default_copy);
			for (Tag& tag : found_tags)
			{
				result.add_tag(std::move(tag));
			}
		}
		return result;
	}

	std::vector<RealCompetitor> RealCompetitor::parse_entry_list(const json& input)
	{
		return json_helper::get_array(input, RealCompetitor::parse);
	}

	std::vector<RealCompetitor> RealCompetitor::parse_entry_list(const json& input, std::string_view field_name)
	{
		json array_view = json_helper::get_array_object(input, field_name);
		return parse_entry_list(array_view);
	}
}