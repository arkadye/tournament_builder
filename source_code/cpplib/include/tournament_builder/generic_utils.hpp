#pragma once

#include <string>
#include <string_view>
#include <sstream>
#include <ranges>
#include <format>
#include <vector>
#include <algorithm>
#include <cassert>

// No unique address implementation because MSVC handles it in an annoying manner.
#if __has_cpp_attribute(no_unique_address)
#ifdef _MSC_VER
#define NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#else
#define NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif
#else
#define NO_UNIQUE_ADDRESS
#endif

namespace tournament_builder::utils
{
	inline std::string join_tokens(const std::ranges::range auto& tokens, char delim)
	{
		bool is_first = true;
		std::ostringstream result;
		for (const auto& token : tokens)
		{
			if (is_first)
			{
				is_first = false;
			}
			else
			{
				result << delim;
			}
			result << std::format("{}", token);
		}
		return result.str();
	}

	template <typename T>
	inline void split_tokens(std::string_view input, char delim, std::output_iterator<T> auto target, std::vector<std::pair<char, char>> brackets = {})
	{
		std::string_view data_view = input;

		auto get_split_point = [delim, &brackets](std::string_view data)
			{
				assert(!data.empty());
				std::vector<std::pair<char,char>> open_brackets;
				for (std::size_t i = 0u; i < data.size(); ++i)
				{
					const char datum = data[i];

					// Check if we found a delimiter
					if (open_brackets.empty() && datum == delim)
					{
						return i;
					}

					// Check if we found a close bracket
					if (!open_brackets.empty() && datum == open_brackets.back().second)
					{
						// We found a close bracket.
						open_brackets.pop_back();
						continue;
					}
					
					// Check if we found an open bracket
					const auto open_bracket_it = std::ranges::find(brackets, datum, &std::pair<char, char>::first);
					if (open_bracket_it != end(brackets))
					{
						open_brackets.push_back(*open_bracket_it);
					}
				}

				// Unorthodox, but I'm going to immediately catch and rethrow within the same scope, pretty much.
				// So don't panic.
				if (!open_brackets.empty())
				{
					const auto [open, close] = open_brackets.back();
					throw std::format("Found open bracket '{}' with no matching '{}' to close", open, close);
				}
				return data.npos;
			};

		auto get_next_element = [&data_view, &get_split_point, &brackets]()
			{
				const std::size_t split_point = get_split_point(data_view);
				std::string_view result = data_view.substr(0, split_point);
				assert(!result.empty());
				if (split_point < data_view.size())
				{
					data_view = data_view.substr(split_point + 1);
				}
				else
				{
					data_view = "";
				}

				return result;
			};

		std::size_t num_elements_processed = 0u;
		try
		{
			while (!data_view.empty())
			{
				std::string_view element = get_next_element();
				const T output{ element };
				*target++ = output;
				++num_elements_processed;
			}
		}
		catch (const std::string& what)
		{
			throw exception::TournamentBuilderException{ std::format("Error at token {} while tokenizing '{}': {}", num_elements_processed, input, what) };
		}
	}

	template <typename T>
	std::vector<T>& combine_vectors(std::vector<T>& modified, std::vector<T> add_this)
	{
		std::ranges::move(std::move(add_this), std::back_inserter(modified));
		return modified;
	}

	template <typename T>
	void remove_duplicated(std::vector<T>& data)
	{
		std::ranges::sort(data);
		auto unique_result = std::unique(begin(data),end(data));
		data.erase(unique_result, end(data));
	}
}