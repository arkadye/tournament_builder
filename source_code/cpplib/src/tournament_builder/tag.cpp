#include "tournament_builder/tag.hpp"
#include "tournament_builder/exceptions.hpp"
#include "tournament_builder/ireferencable.hpp"

#include "tournament_builder/generic_utils.hpp"
#include "tournament_builder/json/json_helpers.hpp"

#include "nlohmann/json.hpp"

#include <algorithm>


namespace tournament_builder
{
    namespace internal_tag
    {
        constexpr char SPECIAL_TAG_INDICATOR = '$';
        constexpr char TAG_DELIMINATOR = '-';
        constexpr char ARG_DELIMINATOR = ':';

        constexpr std::string_view POS = "$POS";
        constexpr std::string_view ENTRY = "$ENTRY";

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
            return invalid;
        }

        bool are_normal_tags_equal(TokenIterator left_start, TokenIterator left_finish, TokenIterator right_start, TokenIterator right_finish)
        {
            const bool left_finished = (left_start == left_finish);
            const bool right_finished = (right_start == right_finish);
            if (left_finished && right_finished)
            {
                return true;
            }

            if (left_finished || right_finished)
            {
                return false;
            }

            const Token left = *left_start;
            const Token right = *right_start;

            if (left != right)
            {
                return false;
            }
            return are_normal_tags_equal(std::next(left_start), left_finish, std::next(right_start), right_finish);
        }

        bool are_solo_tags_equal(Token left, Token right, Tag::TagType type)
        {
            using enum Tag::TagType;
            switch (type)
            {
            case entry:
            case pos:
                break;
            // These types are unsupported.
            case normal:
            case invalid:
                assert(false);
                break;
            }

            auto throw_error = [](Token tok, std::string_view explanation)
                {
                    throw exception::InvalidTagToken{ tok.to_string(), tok.to_string(), 0u, explanation };
                };

            auto get_as_num = [&throw_error](Token tok, std::variant<int64_t, std::string_view> parse_result)
                {
                    if (std::string_view* svp = std::get_if<std::string_view>(&parse_result))
                    {
                        throw_error(tok, std::format("All arguments must be a positive number. Got {} which is not a number", *svp));
                    }
                    assert(std::holds_alternative<int64_t>(parse_result));
                    const int64_t result = std::get<int64_t>(parse_result);
                    if (result <= 0)
                    {
                        throw_error(tok, std::format("All arguments must be a positive number. Got {} which is not positive.", result));
                    }
                    return result;
                };

            auto get_token_args = [&throw_error, &get_as_num](Token tok)
                {
                    std::pair<int64_t, int64_t> result{ 0,0 };
                    const auto args = Token::get_args(tok, ARG_DELIMINATOR);
                    if (args.empty() || args.size() > 2u)
                    {
                        throw_error(tok, std::format("Must have either 1 or 2 positive arguments. (E.g. '{0}{1}3' or {0}{1}2{1}4. Got {2}", tok, ARG_DELIMINATOR, args.size()));
                    }

                    result.first = get_as_num(tok, args.front());
                    result.second = get_as_num(tok, args.back());
                    if (result.second < result.first)
                    {
                        throw_error(tok, std::format("The second argument must be greater or equal to the first argument. Got {} and {}", result.first, result.second));
                    }
                    return result;
                };

            const auto [left_min,left_max] = get_token_args(left);
            const auto [right_min, right_max] = get_token_args(right);

            if (left_min > right_max) return false;
            if (left_max < right_min) return false;
            return true;
        }

        bool are_single_tags_equal(const TokenVector& left, const TokenVector& right)
        {
            assert(left.size() == 1u);
            assert(right.size() == 1u);

            const Token l = left.front();
            const Token r = right.front();

            const Tag::TagType lt = get_type(l);
            const Tag::TagType rt = get_type(r);

            auto is_solo_type = [](Tag::TagType stt)
                {
                    using enum Tag::TagType;
                    switch (stt)
                    {
                    case pos:
                    case entry:
                        return true;
                    case invalid:
                    case normal:
                        break;
                    }
                    return false;
                };

            if (is_solo_type(lt) && (lt == rt))
            {
                return are_solo_tags_equal(l, r, lt);
            }

            return are_normal_tags_equal(begin(left), end(left), begin(right), end(right));
        }

        bool are_tags_equal(const TokenVector& left, const TokenVector& right)
        {
            if (left.size() == 1u && right.size() == 1u)
            {
                return are_single_tags_equal(left, right);
            }
            return are_normal_tags_equal(begin(left), end(left), begin(right), end(right));
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
        utils::split_tokens<Token>(input, internal_tag::TAG_DELIMINATOR, std::back_inserter(m_data));
        copy_on_reference = copy_on_reference_arg;
    }

    bool Tag::operator==(const Tag& other) const noexcept
    {
        return internal_tag::are_tags_equal(this->m_data, other.m_data);
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