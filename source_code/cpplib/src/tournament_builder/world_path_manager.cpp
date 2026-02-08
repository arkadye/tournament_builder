#include "tournament_builder/world_path_manager.hpp"
#include "tournament_builder/world.hpp"

namespace tournament_builder
{
    WorldPathManager::WorldPathManager(World& owner, std::filesystem::path path)
        : m_owner{ &owner }
        , m_path_to_restore{ owner.m_current_file }
    {
        assert(!path.empty());
#ifndef NDEBUG
        assert(m_owner->m_num_current_file_managers >= 0);
        ++m_owner->m_num_current_file_managers;
#endif
        owner.m_current_file = std::move(path);
    }

    WorldPathManager::WorldPathManager(WorldPathManager&& other) noexcept
        : m_owner{ other.m_owner }
        , m_path_to_restore{ std::move(other.m_path_to_restore) }
    {
        other.m_owner = nullptr;
    }

    WorldPathManager::~WorldPathManager()
    {
        if(m_owner == nullptr) return;
#ifndef NDEBUG
        assert(m_owner->m_num_current_file_managers > 0);
        --m_owner->m_num_current_file_managers;
#endif
        m_owner->m_current_file = std::move(m_path_to_restore);
    }
}