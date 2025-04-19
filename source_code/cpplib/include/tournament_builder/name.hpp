#pragma once

#include "tournament_builder/token.hpp"
#include "tournament_builder/exceptions.hpp"

#include <cstdint>
#include <string_view>
#include <format>
#include <iosfwd>

#include "nlohmann/json_fwd.hpp"

// Name acts much like a string, but has some limitations. It can be checked for uniqueness, and easily compared for equality.

namespace tournament_builder
{
	class Name : public Token
	{
	public:
		// Parse the name out of a string object OR a named object, whichever is passed in.
		static Name parse(const nlohmann::json& input);
		explicit Name(std::string_view init) : Token{ init } { validate_name(init); }
		explicit Name(Token t) : Token{ t } { validate_name(t.to_string()); }
	private:
		static void validate_name(std::string_view name);
	};

	class NamedElement
	{
	public:
		Name name;
		template <typename StringIsh>
		NamedElement(const StringIsh& init) : name{ init } {}
		bool matches_name(Name a_name) const { return name == a_name; }
		auto operator<=>(const NamedElement& other) const noexcept = default;
		std::string_view to_string() const { return name.to_string(); }
	};

	namespace exception
	{
		class DuplicateName : public TournamentBuilderException
		{
		public:
			DuplicateName(Name name, std::string_view field_name, std::size_t idx_0, std::size_t idx_1)
				: TournamentBuilderException{ std::format("'{}' contains duplicate of '{}' at positions {}[{}]", name.to_string(), field_name, idx_0, idx_1)}
			{}
			DuplicateName(Name name, std::string_view field_name_a, std::size_t idx_a, std::string_view field_name_b, std::size_t idx_b) : TournamentBuilderException("Please hold...")
				//: tournament_builder::MakerException{ std::format("'{}' contains duplicate of '{}' at {}[{}] and {}[{}]", name, field_name_a, idx_a, field_name_b, idx_b) }
			{}
		};
	}
}

template <>
struct std::formatter<tournament_builder::Name>
{
	template <class PrsContext>
	constexpr auto parse(const PrsContext& prsContext)
	{
		return prsContext.begin();
	}

	auto format(tournament_builder::Name name, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "{}", tournament_builder::Token{ name });
	}
};