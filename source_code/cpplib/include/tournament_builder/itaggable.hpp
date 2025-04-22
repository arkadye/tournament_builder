#pragma once

#include "tournament_builder/tag.hpp"

#include <vector>

namespace tournament_builder
{
	class Tag;
	class ReferenceCopyOptions;
	class ITaggable
	{
	public:
		virtual const std::vector<Tag>& get_tags() const = 0;
		virtual void add_tag(Tag tag) = 0;
		virtual void take_tags_via_reference(const ITaggable& other, const ReferenceCopyOptions&) = 0;
	};

	class TaggableMixin : public ITaggable
	{
	public:
		const std::vector<Tag>& get_tags() const override { return m_tags; }
		void add_tag(Tag tag) override;
		void take_tags_via_reference(const ITaggable& other, const ReferenceCopyOptions&) override;
	private:
		std::vector<Tag> m_tags;
	};
}