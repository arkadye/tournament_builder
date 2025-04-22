#include "tournament_builder/competitor.hpp"

#include "tournament_builder/json/json_helpers.hpp"

#include "nlohmann/json.hpp"

#include <algorithm>

namespace tournament_builder
{
	using nlohmann::json;

	RealCompetitor RealCompetitor::copy_ref(const ReferenceCopyOptions& rco) const
	{
		RealCompetitor result{ name };
		result.take_tags_via_reference(*this, rco);
		return result;
	}

	const std::vector<Tag>& Competitor::get_tags() const
	{
		assert(std::holds_alternative<RealCompetitor>(m_data) && "get_tags can only be called on a non-bye competitor");
		return std::get<RealCompetitor>(m_data).get_tags();
	}

	void Competitor::add_tag(Tag tag)
	{
		if (RealCompetitor* prc = std::get_if<RealCompetitor>(&m_data))
		{
			prc->add_tag(std::move(tag));
		}
	}

	void Competitor::take_tags_via_reference(const ITaggable& other, const ReferenceCopyOptions& options)
	{
		if (RealCompetitor* prc = std::get_if<RealCompetitor>(&m_data))
		{
			prc->take_tags_via_reference(other, options);
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

	bool Competitor::matches_token(const Token& token) const
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

	Name Competitor::get_reference_key() const
	{
		struct Impl
		{
			Name operator()(Bye) const
			{
				assert(false && "We should never be asking a bye for its name");
				return Name{ "ERROR_NAME" };
			}
			Name operator()(const RealCompetitor& rc) const
			{
				return rc.name;
			}
		};
		return std::visit(Impl{}, m_data);
	}

	std::shared_ptr<IReferencable> Competitor::copy_ref(const ReferenceCopyOptions& copy_options) const
	{
		struct Impl
		{
			const ReferenceCopyOptions& rco;
			std::shared_ptr<IReferencable> operator()(Bye) const
			{
				Competitor* copy = new Competitor{ Bye{} };
				return std::shared_ptr<IReferencable>{copy};
			}
			std::shared_ptr<IReferencable> operator()(const RealCompetitor& rc) const
			{
				Competitor* copy = new Competitor{ rc.copy_ref(rco) };
				return std::shared_ptr<IReferencable>{copy};
			}
		};
		return std::visit(Impl{ copy_options }, m_data);
	}

	std::vector<IReferencable*> Competitor::get_next_locations()
	{
		struct Impl
		{
			std::vector<IReferencable*> operator()(Bye) const
			{
				assert(false && "We should never be asking a bye next locations");
				return {};
			}
			std::vector<IReferencable*> operator()(RealCompetitor& rc)
			{
				return {};
			}
		};
		return std::visit(Impl{}, m_data);
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