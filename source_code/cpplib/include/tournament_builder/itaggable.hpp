#pragma once

#include "tournament_builder/tag.hpp"

#include "nlohmann/json_fwd.hpp"

#include <vector>

namespace tournament_builder
{
	class Token;
	class ReferenceCopyOptions;
	class ITaggable
	{
	public:
		virtual const std::vector<Tag>& get_tags() const = 0;
		virtual void add_tag(Tag tag) = 0;
		virtual void take_tags_via_reference(const ITaggable& other, const ReferenceCopyOptions&) = 0;
		bool has_tag_matching_token(const Token& token) const;
	};

	class TaggableMixin : public ITaggable
	{
	public:
		const std::vector<Tag>& get_tags() const override { return m_tags; }
		void add_tag(Tag tag) override;
		void take_tags_via_reference(const ITaggable& other, const ReferenceCopyOptions&) override;
		virtual void add_tags_from_json(const nlohmann::json& input);
	private:
		std::vector<Tag> m_tags;
	};
}