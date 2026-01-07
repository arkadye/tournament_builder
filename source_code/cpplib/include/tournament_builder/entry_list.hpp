#pragma once

#include "tournament_builder/reference.hpp"
#include "tournament_builder/competitor.hpp"

#include <vector>
#include <optional>
#include <ranges>

namespace tournament_builder
{
	class EntryList
	{
		std::vector<Reference<Competitor>> m_original_entries;	// Keep the original entry list always, so we have something to recalculate from if required.
		std::vector<Reference<Competitor>> m_entries;			// The latest entry list, as resolved as we can get it
		int32_t m_min_entries = 0;
		int32_t m_max_entries = 0;
	public:
		EntryList() = default;
		explicit EntryList(std::vector<Reference<Competitor>> new_elements) { set(std::move(new_elements)); }
		bool has_fixed_length() const noexcept;
		int32_t get_min_entries() const noexcept { return m_min_entries; }
		int32_t get_max_entries() const noexcept { return m_max_entries; }
		const std::vector<Reference<Competitor>>& get_entries() const noexcept { return m_entries; }
		std::vector<Reference<Competitor>>& get_entries() noexcept { return m_entries; }
		bool is_resolved() const noexcept;

		void set(std::vector<Reference<Competitor>> new_elements, int32_t new_min, int32_t new_max);
		void set(std::vector<Reference<Competitor>> new_elements, int32_t num_elements) { set(std::move(new_elements), num_elements, num_elements); }
		void set(std::vector<Reference<Competitor>> new_elements) { set(std::move(new_elements), std::ssize(new_elements)); }

		static std::optional<EntryList> unpack_entry_list(World& world, const Location& location, const EntryList& in, bool* pChangeMade = nullptr);
		void unpack_entry_list(World& world, const Location& location, bool* pChangeMade = nullptr);
		void repack_entry_list() { m_entries = m_original_entries; }

		using value_type = decltype(m_entries)::value_type;
		std::size_t size() const noexcept { return m_entries.size(); }
		Reference<Competitor>& operator[](std::size_t idx) { return m_entries[idx]; }
		const Reference<Competitor>& operator[](std::size_t idx) const { return m_entries[idx]; }
		
		bool empty() const noexcept { return m_entries.empty(); }

		static EntryList parse(const nlohmann::json& input);
		static EntryList parse(const nlohmann::json& input, std::string_view field_name);
	private:
		static EntryList parse_array(const nlohmann::json& input);
		static EntryList parse_object(const nlohmann::json& input);
	};
}