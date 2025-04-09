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
				for (const auto [open, close] : brackets)
				{
					if (data.front() != open) continue;
					const std::size_t result = data.find(close);
					if (result >= data.size())
					{
						// Unorthodox, but I'm going to immediately catch and rethrow within the same scope, pretty much.
						// So don't panic.
						throw std::format("Found open bracket '{}' with no matching '{}' to close", open, close);
					}
					return result + 1;
				}
				return data.find(delim);
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

				for (const auto [open, close] : brackets)
				{
					if (result.front() == open && result.back() == close)
					{
						result.remove_prefix(1);
						result.remove_suffix(1);
					}
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