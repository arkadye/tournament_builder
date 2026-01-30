#pragma once

#include <filesystem>

namespace tournament_builder
{
	class WorldPathManager
	{
		friend class World;
		World* m_owner = nullptr;
#ifndef NDEBUG
		std::filesystem::path m_expected_top_on_destruction;
#endif
		[[nodiscard]] WorldPathManager(World& owner, std::filesystem::path path);
	public:
		WorldPathManager() = default;
		WorldPathManager(const WorldPathManager&) = delete;
		WorldPathManager(WorldPathManager&& other) noexcept;
		~WorldPathManager();
	};
}