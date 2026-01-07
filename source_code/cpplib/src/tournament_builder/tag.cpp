#include "tournament_builder/tag.hpp"
#include "tournament_builder/exceptions.hpp"
#include "tournament_builder/ireferencable.hpp"

#include "tournament_builder/generic_utils.hpp"
#include "tournament_builder/json/json_helpers.hpp"
#include "tournament_builder/tag_and_reference_helpers.hpp"

#include "nlohmann/json.hpp"

#include <algorithm>

#include <iostream>

namespace tournament_builder
{
    namespace internal_tag
    {
        constexpr char SPECIAL_TAG_INDICATOR = '$';
        constexpr char TAG_DELIMINATOR = '.';
        constexpr char ARG_DELIMINATOR = ':';

        constexpr std::string_view POS = "$POS";
        constexpr std::string_view ENTRY = "$ENTRY";
        constexpr std::string_view ANY = "$ANY";
        constexpr std::string_view GLOB = "$GLOB";

        using TokenVector = std::vector<Token>;
        using TokenIterator = TokenVector::const_iterator;

        Tag::TagType get_type(Token t)
        {
            using enum Tag::TagType;
            const std::string_view s = t.to_string();
            if (s.empty()) return normal;
            if (s[0] != internal_tag::SPECIAL_TAG_INDICATOR) return normal;
            if (s.starts_with(POS)) return pos;
            if (s.starts_with(ENTRY)) return entry;
            if (s.starts_with(ANY)) return any;
            if (s.starts_with(GLOB)) return glob;
            return invalid;
        }

        bool is_wildcard(const Token& t)
        {
            const Tag::TagType type = get_type(t);
            switch (type)
            {
            case Tag::TagType::any:
            case Tag::TagType::glob:
                return true;
            default:
                break;
            }
            return false;
        }

        bool is_solo_type(Tag::TagType stt)
        {
            using enum Tag::TagType;
            switch (stt)
            {
            case pos:
            case entry:
                return true;
            case invalid:
            case normal:
            case any:
            case glob:
                break;
            }
            return false;
        }

        bool are_normal_tags_equal(const Tag& left_context, TokenIterator left_start, TokenIterator left_current, TokenIterator left_finish, const Tag& right_context, TokenIterator right_start, TokenIterator right_current, TokenIterator right_finish);

        bool are_wildcard_tags_equal(const Tag& left_context, TokenIterator left_start, TokenIterator left_current_wc, TokenIterator left_finish, const Tag& right_context, TokenIterator right_start, TokenIterator right_current, TokenIterator right_finish)
        {
            assert(is_wildcard(*left_current_wc));
            const Tag::TagType tag_type = get_type(*left_current_wc);
            helpers::SpecialTagType wildcard_type{};
            switch (tag_type)
            {
            case Tag::TagType::any:
                wildcard_type = helpers::SpecialTagType::any;
                break;
            case Tag::TagType::glob:
                wildcard_type = helpers::SpecialTagType::glob;
                break;
            default:
                assert(false && "Invalid tag_type");
                break;
            }

            try
            {
                helpers::GetSpecialTagArgsResult wildcard_args = helpers::get_special_tag_args(*left_current_wc, wildcard_type, ARG_DELIMINATOR);
                const auto next_left = std::next(left_current_wc);
                for (int64_t lvl{ 0 }; lvl <= wildcard_args.max; ++lvl, ++right_current)
                {
                    if (right_current == right_finish) return (lvl >= wildcard_args.min) && (next_left == left_finish);

                    // If the right-hand-side is a solo-type tag, we cannot match with it.
                    const Tag::TagType right_type = get_type(*right_current);
                    if (is_solo_type(right_type)) return false;

                    if (lvl < wildcard_args.min) continue;

                    const bool candidate = are_normal_tags_equal(left_context, left_start, next_left, left_finish, right_context, right_start, right_current, right_finish);
                    if (candidate)
                    {
                        return true;
                    }
                }
            }
            catch(const exception::TournamentBuilderException& ex)
            {
                throw exception::InvalidTagToken{ left_context.to_string(), left_current_wc->to_string(), static_cast<std::size_t>(std::distance(left_start, left_current_wc)), ex.what() };
            }
            return false;
        }

        bool are_normal_tags_equal(const Tag& left_context, TokenIterator left_start, TokenIterator left_current, TokenIterator left_finish, const Tag& right_context, TokenIterator right_start, TokenIterator right_current, TokenIterator right_finish)
        {
            while (true)
            {
                const bool left_finished = (left_current == left_finish);
                const bool right_finished = (right_current == right_finish);
                if (left_finished && right_finished)
                {
                    return true;
                }

                if (!left_finished && is_wildcard(*left_current))
                {
                    const bool candidate = are_wildcard_tags_equal(left_context, left_start, left_current, left_finish, right_context, right_start, right_current, right_finish);
                    if (candidate) return true;
                }

                if (!right_finished && is_wildcard(*right_current))
                {
                    const bool candidate = are_wildcard_tags_equal(right_context, right_start, right_current, right_finish, left_context, left_start, left_current, left_finish);
                    if (candidate) return true;
                }

                if (left_finished || right_finished)
                {
                    return false;
                }

                const Token left = *left_current;
                const Token right = *right_current;

                if (left != right)
                {
                    return false;
                }
                ++left_current;
                ++right_current;
            }
        }

        bool are_tags_equal(const Tag& left_context, const TokenVector& left, const Tag& right_context, const TokenVector& right)
        {
            auto bl = begin(left);
            auto el = end(left);
            auto br = begin(right);
            auto er = end(right);
            return are_normal_tags_equal(left_context,bl, bl, el, right_context, br, br, er);
        }
    }

    Tag::TagType Tag::get_type() const
    {
        using enum Tag::TagType;
        if (m_data.size() > 1u) return normal;
        if (m_data.empty()) return invalid;
        return internal_tag::get_type(m_data.front());
    }

    Tag Tag::parse(const nlohmann::json& input, bool default_copy_of_ref)
    {
        if (input.is_string())
        {
            return Tag{ input.get<std::string>() , default_copy_of_ref };
        }

        const std::string tag_str = json_helper::get_string(input, "tag");
        const bool copy_on_ref = json_helper::get_bool_or(input, "copy_on_ref", default_copy_of_ref);
        return Tag{ tag_str, copy_on_ref };
    }

    Tag::Tag(std::string_view input, bool copy_on_reference_arg)
    {
        if (input.starts_with('[') && input.ends_with(']'))
        {
            input.remove_prefix(1);
            input.remove_suffix(1);
        }
        utils::split_tokens<Token>(input, internal_tag::TAG_DELIMINATOR, std::back_inserter(m_data));
        copy_on_reference = copy_on_reference_arg;
    }

    bool Tag::operator==(const Tag& other) const noexcept
    {
        const bool result = internal_tag::are_tags_equal(*this, this->m_data, other, other.m_data);
        return result;
    }

    std::string tournament_builder::Tag::to_string() const
    {
        return utils::join_tokens(m_data, internal_tag::TAG_DELIMINATOR);
    }

    bool Tag::collides_with(const Tag& other) const
    {
        using enum TagType;

        const TagType my_type = this->get_type();
        const TagType other_type = other.get_type();

        // If the types are different they cannot collide.
        if (my_type != other_type) return false;

        const bool are_special = my_type != TagType::normal;

        // If they're both the same type of special, they do collide even if they're not equal.
        if (are_special)
        {
            return true;
        }

        // Now we simply check equality.
        return *this == other;       
    }

    bool Tag::collides_with(const std::vector<Tag>& others) const
    {
        return std::ranges::any_of(others, [&tag = *this](const Tag& other) {return tag.collides_with(other); });
    }

    std::vector<Tag> Tag::copy_tags_on_ref(std::vector<Tag> in, const ReferenceCopyOptions& options)
    {
        auto should_copy_tag = [&options](const Tag& tag)
            {
                return tag.copy_on_reference;
            };

        auto should_erase_tag = [&should_copy_tag](const Tag& tag)
            {
                return !should_copy_tag(tag);
            };

        auto erase_result = std::ranges::remove_if(in, should_erase_tag);
        in.erase(begin(erase_result), end(erase_result));
        return in;
    }

    namespace exception
    {
        InvalidTagToken::InvalidTagToken(std::string_view tag, std::string_view token, std::size_t pos, std::string_view explanation)
            : TagException{std::format("In tag '{}' at token `{}` in position {}: {}", tag, token, pos, explanation)}
        {
        }
    }
}