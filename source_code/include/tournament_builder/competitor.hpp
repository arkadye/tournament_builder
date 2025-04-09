#pragma once

#include "name.hpp"
#include "reference.hpp"
#include "tag.hpp"

#include "nlohmann/json_fwd.hpp"

#include <variant>
#include <vector>
#include <string_view>

namespace tournament_builder
{
	class RealCompetitor : public NamedElement
	{
	public:
		using NamedElement::NamedElement;
		const std::vector<Tag>& get_tags() const { return m_tags; }
		void add_tag(Tag tag);
		RealCompetitor copy_ref() const;
		static RealCompetitor parse(const nlohmann::json& input);
		static std::vector<RealCompetitor> parse_entry_list(const nlohmann::json& input);
		static std::vector<RealCompetitor> parse_entry_list(const nlohmann::json& input, std::string_view field_name);
	private:
		std::vector<Tag> m_tags;
	};

	class Bye {};

	class Competitor
	{
	public:
		using underlying_t = std::variant<RealCompetitor, Bye>;
	private:
		underlying_t m_data;
	public:
		Competitor(RealCompetitor rc) : m_data{ std::move(rc) } {}
		Competitor(Bye b) noexcept : m_data{ b } {}
		Competitor(const Competitor&) = default;
		Competitor(Competitor&&) noexcept = default;
		Competitor& operator=(const Competitor&) = default;
		Competitor& operator=(Competitor&&) noexcept = default;
		underlying_t& data() noexcept { return m_data; }
		const underlying_t& data() const noexcept { return m_data; }
		bool is_bye() const noexcept { return std::holds_alternative<Bye>(m_data); }
		Competitor copy_ref() const;
		void add_tag(Tag tag);

		std::string_view to_string() const;

		static bool is_bye(const Reference<Competitor>& ref);

		// This is true if the token provided is the name, or matches any tag when interpreted as one.
		bool matches_token(Token token) const;

		// Returns true if the name matches this one.
		bool matches_name(Name name) const;

		static Competitor parse(const nlohmann::json& input);
		static std::vector<Reference<Competitor>> parse_entry_list(const nlohmann::json& input);
		static std::vector<Reference<Competitor>> parse_entry_list(const nlohmann::json& input, std::string_view field_name);
	};
}