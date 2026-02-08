#pragma once

#include <filesystem>

namespace tournament_builder
{
	class WorldPathManager
	{
		friend class World;
		World* m_owner = nullptr;
		std::filesystem::path m_path_to_restore;
		[[nodiscard]] WorldPathManager(World& owner, std::filesystem::path path);
	public:
		WorldPathManager() = default;
		WorldPathManager(const WorldPathManager&) = delete;
		WorldPathManager(WorldPathManager&& other) noexcept;
		~WorldPathManager();
	};
}