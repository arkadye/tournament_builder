#include "tournament_builder/ireferencable.hpp"
#include "tournament_builder/itaggable.hpp"
#include "tournament_builder/name.hpp"

namespace tournament_builder
{
    bool IReferencable::matches_token(const Token& token) const
    {
        if (const auto* as_named = dynamic_cast<const NamedElement*>(this))
        {
            if (as_named->name == token)
            {
                return true;
            }
        }

        if (const auto* as_taggable = dynamic_cast<const ITaggable*>(this))
        {
            if (as_taggable->has_tag_matching_token(token))
            {
                return true;
            }
        }
        return false;
    }
}