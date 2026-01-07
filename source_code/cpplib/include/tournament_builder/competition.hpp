#pragma once

#include "tournament_builder/ireferencable.hpp"
#include "tournament_builder/itaggable.hpp"

#include "tournament_builder/competitor.hpp"
#include "tournament_builder/entry_list.hpp"
#include "tournament_builder/name.hpp"
#include "tournament_builder/reference.hpp"
#include "tournament_builder/descriptors/descriptor_handle.hpp"

#include "nlohmann/json_fwd.hpp"

#include <vector>
#include <variant>

namespace tournament_builder
{
	class World;
	class Competition;

	// This can be a single match/race or a tournament_builder phase.
	class RealCompetition : public NamedElement , public TaggableMixin
	{
	public:
		using NamedElement::NamedElement;
		virtual ~RealCompetition() = default;
		EntryList entry_list;
		std::vector<Competition> phases;

		//  Returns true if a RealCompetition and all the entry list references have been resolved and the same is true for all phases.
		bool has_resolved_all_references() const;

		// Returns true if a RealCompetition and all the entry list references have been resolved.
		bool has_finalized_entry_list() const;

		// Returns true if it successfully resolves all the references.
		bool resolve_all_references(World& context, Location& location);

		static RealCompetition parse(const nlohmann::json& input);

		// IReferencable
		// This should return the reference key for this element. Normally the same as get_name().
		Name get_reference_key() const { return name; }

		// Copy this as through a reference.
		std::shared_ptr<IReferencable> copy_ref(const ReferenceCopyOptions&) const;

		// Get a list of all the possible inner targets for use when parsing a reference.
		std::vector<IReferencable*> get_all_next_locations();
		std::vector<IReferencable*> get_next_locations(const Token& token);

	private:
		friend class Competition;
		// Returns true if it makes any changes
		bool resolve_all_references_impl(World& context, Location& location);
		std::vector<IReferencable*> get_next_locations_entry_tag(const Token& tag);
		std::vector<IReferencable*> get_next_locations_pos_tags(const Token& tag);
	};

	class Competition : public IReferencable , public ITaggable
	{
	public:
		explicit Competition(descriptor::DescriptorHandle desc) : m_data{ std::move(desc) } {}
		explicit Competition(RealCompetition competition) : m_data{ std::move(competition) } {}
		virtual ~Competition() = default;

		//  Returns ture if a RealCompetition and all the entry list references have been resolved and the same is true for all phases.
		bool has_resolved_all_references() const;

		// Returns ture if a RealCompetition and all the entry list references have been resolved.
		bool has_finalized_entry_list() const;

		// If working with a descriptor, tries to resolve the descriptor.
		// If successful, or already a RealCompetition, resolves as many references as it can on the competition.
		// Returns true if it successfully resolves all the references.
		bool resolve_all_references(World& context, Location& location);

		static Competition parse(const nlohmann::json& input);

		// IReferencable
		Name get_reference_key() const override;
		std::shared_ptr<IReferencable> copy_ref(const ReferenceCopyOptions&) const override;
		std::vector<IReferencable*> get_next_locations(const Token& token) override;
		std::vector<IReferencable*> get_all_next_locations() override;

		// ITaggable
		const std::vector<Tag>& get_tags() const override;
		void add_tag(Tag tag) override;
		void take_tags_via_reference(const ITaggable& other, const ReferenceCopyOptions&) override;

		RealCompetition* get_real_competition();
		const RealCompetition* get_real_competition() const;

		const std::variant<descriptor::DescriptorHandle, RealCompetition>& get_data() const { return m_data; }
	private:
		friend class RealCompetition;
		std::variant<descriptor::DescriptorHandle, RealCompetition> m_data;

		// Returns true if it makes any changes
		bool resolve_all_references_impl(World& context, Location& location);
	};
}