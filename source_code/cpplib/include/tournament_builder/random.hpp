#pragma once

// We use this to provide a simple interface to <random>, with machine-independant pseudorandomness assuming the same seed.

#include <cstdint>
#include <cstddef>
#include <iterator>
#include <utility>

namespace tournament_builder::random
{
	void initialize_engine(uint64_t seed);

	// Get whichever seed was used for the random engine in order to seed the RNG.
	uint64_t get_seed();

	// Get a random index in the range [min,max).
	std::size_t get_idx(std::size_t min, std::size_t max);

	// Get a random index in the range [0,max).
	inline std::size_t get_idx(std::size_t max) { return get_idx(0u, max); }

	// Implements an in-place Fisher-Yates shuffle on the provided range
	template <std::random_access_iterator It>
	inline void shuffle(It first, It last)
	{
		const std::size_t size = static_cast<std::size_t>(std::distance(first, last));
		for (std::size_t idx = 0u; idx < size; ++idx)
		{
			const std::size_t swap_target = get_idx(idx, size);
			if (swap_target != idx) [[likely]]
			{
				std::swap(*(first + idx), *(first + swap_target));
			}
		}
	}

	template <class Container>
	inline void shuffle(Container& container)
	{
		return shuffle(begin(container), end(container));
	}
}