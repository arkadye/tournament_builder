#include "tournament_builder/entry_list.hpp"
#include "tournament_builder/json/json_helpers.hpp"
#include "tournament_builder/exceptions.hpp"

#include <algorithm>

namespace tournament_builder
{
	using nlohmann::json;
	EntryList EntryList::parse_array(const json& input, World& context)
	{
        json_helper::validate_type(input, json::value_t::array);
		EntryList result;
        auto parse_competitor = [&context](const json& competitor)
            {
                return Reference<Competitor>::parse(competitor, context);
            };
		auto data = json_helper::get_array(input, parse_competitor);
        result.set(std::move(data));
		return result;
	}

    EntryList EntryList::parse_object(const json& input, World& context)
    {
        json_helper::validate_type(input, json::value_t::object);
        json entries_field = json_helper::get_array_object(input, "entries");
        EntryList result = parse_array(entries_field, context);
        auto num_entries_field = json_helper::get_optional_int(input, "num_entries");
        auto min_entries_field = json_helper::get_optional_int(input, "min_entries");
        auto max_entries_field = json_helper::get_optional_int(input, "max_entries");

        if (num_entries_field.has_value())
        {
            if (min_entries_field.has_value() || max_entries_field.has_value())
            {
                throw exception::InvalidArgument{ "Entry list cannot have 'num_entries' set when either 'min_entries' or 'max_entries' are set." };
            }
            result.m_min_entries = result.m_max_entries = *num_entries_field;
        }
        else if (min_entries_field.has_value() || max_entries_field.has_value())
        {
            result.m_min_entries = min_entries_field.value_or(1);
            result.m_max_entries = max_entries_field.value_or(std::numeric_limits<decltype(result.m_max_entries)>::max());
            if (result.m_min_entries > result.m_max_entries)
            {
                throw exception::InvalidArgument{ std::format("An entry list cannot have min_entries({}) greater than max_entries({})", result.m_min_entries, result.m_max_entries) };
            }
        }

        return result;
    }

    EntryList EntryList::parse(const json& input, World& context)
    {
        json_helper::validate_type(input, { json::value_t::array , json::value_t::object });
        if (input.is_array())
        {
            return parse_array(input, context);
        }
        else if (input.is_object())
        {
            return parse_object(input, context);
        }
        else
        {
            assert(false);
        }
        return {};
    }

    EntryList EntryList::parse(const json& input, std::string_view field_name, World& context)
    {
		try
		{
			json field_view = json_helper::get_any(input, field_name);
			return parse(field_view, context);
		}
		catch (exception::TournamentBuilderException& ex)
		{
			ex.add_context(std::format("In field '{}'", field_name));
			throw ex;
		}
	}

    bool EntryList::has_fixed_length() const noexcept
    {
        return m_min_entries == m_max_entries || std::ranges::all_of(m_entries, &Reference<Competitor>::has_fixed_size);
    }

    bool EntryList::is_resolved() const noexcept
    {
        return std::ranges::all_of(m_entries, &Reference<Competitor>::is_resolved);
    }

    std::optional<EntryList> EntryList::unpack_entry_list(World& world, const Location& location, const EntryList& in, bool* pChangeMade)
    {
        if (pChangeMade)
        {
            *pChangeMade = false;
        }

        EntryList result = in;
        result.m_entries.clear();

        bool localChangeMade = false;

        for (const Reference<Competitor>& rc : in.m_entries)
        {
            assert(std::ssize(result) <= in.m_max_entries);
            if (rc.is_resolved())
            {
                result.m_entries.push_back(rc);
                continue;
            }

            const std::vector<ReferenceResult<Competitor>> deref_result = rc.dereference_multi(world, location);

            if (deref_result.empty() && rc.get_min_results() > 0)
            {
                result.m_entries.push_back(rc);
                continue;
            }
            
            for (const ReferenceResult<Competitor>& rrc : deref_result)
            {
                Competitor* competitor = rrc.get();
                assert(competitor != nullptr);
                const Name name = competitor->get_reference_key();
                auto matches_name = [&name](const Reference<Competitor>& elem)
                    {
                        if (elem.is_reference()) return false;
                        const Competitor& other = elem.get();
                        return other.matches_name(name);
                    };

                if (std::ranges::any_of(result.m_entries, matches_name) || std::ranges::any_of(in.m_entries, matches_name)) continue;


                const ReferenceCopyOptions copy_opts = rc.get_copy_opts();
                result.m_entries.emplace_back(competitor->copy_ref(copy_opts));
            }
            localChangeMade = true;
        }

        const auto smallest_possible_size = std::transform_reduce(begin(result.m_entries), end(result.m_entries), 0, std::plus<int32_t>{}, [](const Reference<Competitor>& rc) {return rc.get_min_results(); });
        const auto largest_possible_size = std::transform_reduce(begin(result.m_entries), end(result.m_entries), 0, std::plus<int32_t>{}, [](const Reference<Competitor>& rc) {return rc.get_max_results(); });

        if (largest_possible_size < in.get_min_entries() || smallest_possible_size > in.get_max_entries())
        {
            return std::nullopt;
        }

#ifndef NDEBUG
        auto rc_str = [](const Reference<Competitor>& rc)
            {
                if (rc.is_reference()) return rc.to_string();
                const Competitor& comp = rc.get();
                return std::string{ comp.to_string() };
            };
        auto old_view = std::ranges::views::transform(in.m_entries, rc_str);
        auto new_view = std::ranges::views::transform(result.m_entries, rc_str);

        const bool are_equal = std::ranges::equal(in.m_entries, result.m_entries, {}, rc_str, rc_str);
        const bool has_changed = !are_equal;
        assert(localChangeMade == has_changed);
#endif

        if (pChangeMade)
        {
            *pChangeMade = localChangeMade;
        }
        return result;
    }

    void EntryList::unpack_entry_list(World& world, const Location& location, bool* pChangeMade)
    {
        auto new_entries = unpack_entry_list(world, location, *this, pChangeMade);
        if (new_entries.has_value())
        {
            *this = std::move(*new_entries);
        }
    }

    void EntryList::set(std::vector<Reference<Competitor>> new_elements, int32_t new_min, int32_t new_max)
    {
        assert(new_min <= new_max);
        m_original_entries = m_entries = std::move(new_elements);
        m_min_entries = new_min;
        m_max_entries = new_max;
    }
}