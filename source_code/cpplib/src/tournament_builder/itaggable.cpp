#include "tournament_builder/itaggable.hpp"
#include "tournament_builder/token.hpp"
#include "tournament_builder/json/json_helpers.hpp"

#include <algorithm>

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

    bool ITaggable::has_tag_matching_token(const Token& token) const
    {
        auto tag_matches_token = [&token](const Tag& tag)
            {
                return tag == token;
            };

        return std::ranges::any_of(get_tags(), tag_matches_token);
    }

    void TaggableMixin::add_tags_from_json(const nlohmann::json& input)
    {
        if (input.contains("tags"))
        {
            m_tags = json_helper::get_array(input["tags"], &Tag::parse_default_copy);
        }
    }
}