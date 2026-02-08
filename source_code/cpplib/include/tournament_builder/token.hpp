#pragma once

#include <cstdint>
#include <string_view>
#include <format>
#include <iosfwd>
#include <vector>
#include <variant>

#include "nlohmann/json_fwd.hpp"

// Token acts much like a string, but has some limitations. It can be checked for uniqueness, and easily compared for equality.

namespace tournament_builder
{
	class Token
	{
	public:
		auto operator<=>(const Token&) const noexcept = default;
		auto operator<=>(std::string_view other) const noexcept { return to_string() <=> other; }
		explicit Token(std::string_view token) : m_data{ token } {}
		std::string_view to_string() const noexcept { return m_data; }
		operator std::string_view() const noexcept { return to_string(); }

		// Parse the name out of a string object OR a named object, whichever is passed in.
		static Token parse(const nlohmann::json& input);

		// A helper to split a token with arguments up into the token and its args.
		static std::vector<std::variant<int64_t, std::string_view>> get_args(std::string_view token_str, char delim);
		static std::vector<std::variant<int64_t, std::string_view>> get_args(const Token& token, char delim) { return get_args(token.to_string(), delim); }

	private:
		std::string m_data;
	};
}

template <>
struct std::formatter<tournament_builder::Token>
{
	template <class PrsContext>
	constexpr auto parse(const PrsContext& prsContext)
	{
		return prsContext.begin();
	}

	auto format(tournament_builder::Token token, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "{}", token.to_string());
	}
};