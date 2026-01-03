#include "tournament_builder/competitor.hpp"
#include "tournament_builder/tag_and_reference_helpers.hpp"

#include "tournament_builder/json/json_helpers.hpp"


#include "nlohmann/json.hpp"

#include <algorithm>

namespace tournament_builder
{
	using nlohmann::json;

	RealCompetitor RealCompetitor::copy_ref(const ReferenceCopyOptions& rco) const
	{
		RealCompetitor result = *this;
		result.take_tags_via_reference(*this, rco);
		result.finishing_position = std::nullopt;
		return result;
	}

	void RealCompetitor::add_tags_from_json(const nlohmann::json& input)
	{
		TaggableMixin::add_tags_from_json(input);
		auto position_tag_it = std::ranges::find(m_tags, Tag::TagType::pos, &Tag::get_type);
		if (position_tag_it != end(m_tags))
		{
			using namespace helpers;
			const Tag& position_tag = *position_tag_it;

			GetSpecialTagArgsResult tag_args = get_special_tag_args(Token{ position_tag.to_string() }, SpecialTagType::pos, ':');
			const int min = static_cast<int>(tag_args.min);
			const int max = static_cast<int>(tag_args.max);
			finishing_position = std::pair{ min,max };

			m_tags.erase(position_tag_it);
		}
	}

	const std::vector<Tag>& Competitor::get_tags() const
	{
		if (const RealCompetitor* prc = std::get_if<RealCompetitor>(&m_data))
		{
			return prc->get_tags();
		}
		static const std::vector<Tag> dummy;
		return dummy;
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

	std::optional<std::pair<int, int>> Competitor::get_finishing_positions() const
	{
		if (const RealCompetitor* prc = std::get_if<RealCompetitor>(&m_data))
		{
			return prc->finishing_position;
		}
		return std::nullopt;
	}

	void Competitor::set_finish_positions(std::pair<int, int> positions)
	{
		assert(std::holds_alternative<RealCompetitor>(m_data));
		RealCompetitor& rc = std::get<RealCompetitor>(m_data);
		rc.finishing_position = std::move(positions);
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

	std::vector<Reference<Competitor>> Competitor::parse_entry_list(const json& input)
	{
		return json_helper::get_array(input, Reference<Competitor>::parse);
	}

	std::vector<Reference<Competitor>> Competitor::parse_entry_list(const json& input, std::string_view field_name)
	{
		try
		{
			json array_view = json_helper::get_array_object(input, field_name);
			return parse_entry_list(array_view);
		}
		catch (exception::TournamentBuilderException& ex)
		{
			ex.add_context(std::format("In field '{}'", field_name));
			throw ex;
		}
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

	std::vector<IReferencable*> Competitor::get_all_next_locations()
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
		result.add_tags_from_json(input);
		result.user_data = json_helper::get_optional_object(input, "user_data");
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