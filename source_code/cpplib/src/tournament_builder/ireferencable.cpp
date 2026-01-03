#include "tournament_builder/ireferencable.hpp"
#include "tournament_builder/itaggable.hpp"
#include "tournament_builder/name.hpp"

#include <algorithm>
namespace tournament_builder
{
    bool IReferencable::matches_token(const Token& token) const
    {
        if (get_reference_key() == token)
        {
            return true;
        }

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

    std::vector<IReferencable*> IReferencable::get_next_locations(const Token& token)
    {
        auto result = get_all_next_locations();
        auto remove_result = std::ranges::remove_if(result, [&token](IReferencable* ir) {return !ir->matches_token(token); });
        result.erase(begin(remove_result), end(remove_result));
        return result;
    }
}