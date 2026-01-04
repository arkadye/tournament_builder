#pragma once

#include "tournament_builder/reference.hpp"
#include "tournament_builder/competitor.hpp"

#include <vector>

namespace tournament_builder
{
	class EntryList
	{
	public:
		std::vector<Reference<Competitor>> entries;
		int32_t min_entries = 0;
		int32_t max_entries = 0;

		using value_type = decltype(entries)::value_type;
		std::size_t size() const noexcept { return entries.size(); }
		Reference<Competitor>& operator[](std::size_t idx) { return entries[idx]; }
		const Reference<Competitor>& operator[](std::size_t idx) const { return entries[idx]; }
		template <typename...Args>
		void emplace_back(Args&&...args) { entries.emplace_back(std::forward<Args>(args)...); }
		void push_back(Reference<Competitor>&& in) { entries.push_back(std::move(in)); }
		void push_back(const Reference<Competitor>& in) { entries.push_back(in); }
		void reserve(std::size_t num) { entries.reserve(num); }

		auto begin() { return entries.begin(); }
		auto end() { return entries.end(); }
		auto begin() const { return entries.cbegin(); }
		auto end() const { return entries.cend(); }
		auto cbegin() const { return entries.cbegin(); }
		auto cend() const { return entries.cend(); }

		static EntryList parse(const nlohmann::json& input);
		static EntryList parse(const nlohmann::json& input, std::string_view field_name);
	};
}

inline auto begin(const tournament_builder::EntryList& in) { return in.begin(); }
inline auto end(const tournament_builder::EntryList& in) { return in.end(); }
inline auto begin(tournament_builder::EntryList& in) { return in.begin(); }
inline auto end(tournament_builder::EntryList& in) { return in.end(); }
inline auto cbegin(const tournament_builder::EntryList& in) { return in.cbegin(); }
inline auto cend(const tournament_builder::EntryList& in) { return in.cend(); }