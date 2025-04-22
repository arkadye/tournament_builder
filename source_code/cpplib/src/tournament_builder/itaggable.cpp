#include "tournament_builder/itaggable.hpp"

namespace tournament_builder
{
    void TaggableMixin::add_tag(Tag tag)
    {
        if (!tag.collides_with(m_tags))
        {
            m_tags.push_back(std::move(tag));
        }
    }
    void TaggableMixin::take_tags_via_reference(const ITaggable& other, const ReferenceCopyOptions& options)
    {
        const std::vector<Tag>& original_tags = other.get_tags();
        m_tags = Tag::copy_tags_on_ref(original_tags, options);
    }
}