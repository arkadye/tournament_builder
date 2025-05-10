#include "tournament_builder/random.hpp"

#include <random>
#include <cassert>

namespace tournament_builder::random
{
	namespace internal_random
	{
		using EngineType = std::mt19937_64;

		uint64_t seed_used = EngineType::default_seed;
		bool seeded = false;

		void initialize_engine()
		{
			using tournament_builder::random::initialize_engine;
			std::random_device rd{};
			constexpr uint64_t rd_max = rd.max();
			static_assert(rd_max >= std::numeric_limits<uint32_t>::max());
			if constexpr (rd_max >= std::numeric_limits<uint64_t>::max())
			{
				initialize_engine(rd());
			}
			else // if constexpr (rd_max >= static_cast<uint64_t>(std::numeric_limits<uint32_t>::max()))
			{
				auto get_32_bit_result = [&rd]() { return static_cast<uint64_t>(rd() && 0xFFFFFFFF); };
				const uint64_t upper_seed = get_32_bit_result();
				const uint64_t lower_seed = get_32_bit_result();
				const uint64_t seed = (upper_seed << 32) + lower_seed;
				initialize_engine(seed);
			}
		}

		EngineType& get_engine_base()
		{
			static EngineType engine;
			return engine;
		}

		EngineType& get_engine()
		{
			if (!seeded)
			{
				initialize_engine();
			}
			return get_engine_base();
		}
	}

	void initialize_engine(uint64_t seed)
	{
		using namespace internal_random;
		seeded = true;
		get_engine_base().seed(seed);
		internal_random::seed_used = seed;
	}

	std::size_t get_idx(std::size_t min, std::size_t max)
	{
		assert(min < max);
		std::uniform_int_distribution<std::size_t> distro{ min, max - 1 };
		const std::size_t result = distro(internal_random::get_engine());
		return result;
	}

	bool was_seeded()
	{
		return internal_random::seeded;
	}

	uint64_t get_seed()
	{
		return internal_random::seed_used;
	}
}