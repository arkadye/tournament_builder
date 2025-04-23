#pragma once

#include "tournament_builder/name.hpp"
#include "tournament_builder/reference.hpp"
#include "tournament_builder/itaggable.hpp"
#include "tournament_builder/ireferencable.hpp"

#include "nlohmann/json.hpp"

#include <variant>
#include <vector>
#include <string_view>
#include <optional>

namespace tournament_builder
{
	class RealCompetitor : public NamedElement , public TaggableMixin
	{
	public:
		using NamedElement::NamedElement;
		RealCompetitor copy_ref(const ReferenceCopyOptions&) const;
		static RealCompetitor parse(const nlohmann::json& input);
		static std::vector<RealCompetitor> parse_entry_list(const nlohmann::json& input);
		static std::vector<RealCompetitor> parse_entry_list(const nlohmann::json& input, std::string_view field_name);

		std::optional<nlohmann::json> user_data;
	};

	class Bye {};

	class Competitor : public IReferencable , public ITaggable
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

		std::string_view to_string() const;

		static bool is_bye(const Reference<Competitor>& ref);

		// Returns true if the name matches this one.
		bool matches_name(Name name) const;

		static Competitor parse(const nlohmann::json& input);
		static std::vector<Reference<Competitor>> parse_entry_list(const nlohmann::json& input);
		static std::vector<Reference<Competitor>> parse_entry_list(const nlohmann::json& input, std::string_view field_name);

		// IReferencable
		Name get_reference_key() const override;
		std::shared_ptr<IReferencable> copy_ref(const ReferenceCopyOptions&) const override;
		std::vector<IReferencable*> get_next_locations() override;
		bool matches_token(const Token&) const override;

		// ITaggable
		const std::vector<Tag>& get_tags() const override;
		void add_tag(Tag tag) override;
		void take_tags_via_reference(const ITaggable& other, const ReferenceCopyOptions&) override;
	};
}