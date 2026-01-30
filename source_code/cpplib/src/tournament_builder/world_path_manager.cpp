#include "tournament_builder/world_path_manager.hpp"
#include "tournament_builder/world.hpp"

namespace tournament_builder
{
    WorldPathManager::WorldPathManager(World& owner, std::filesystem::path path)
        : m_owner{ &owner }
#ifndef NDEBUG
        , m_expected_top_on_destruction{ path }
#endif
    {
#ifndef NDEBUG
        assert(m_owner->m_num_current_file_managers >= 0);
        ++m_owner->m_num_current_file_managers;
#endif
        m_owner->push_current_file(std::move(path));
    }

    WorldPathManager::WorldPathManager(WorldPathManager&& other) noexcept
    {
        m_owner = other.m_owner;
        other.m_owner = nullptr;
#ifndef NDEBUG
        m_expected_top_on_destruction = std::move(other.m_expected_top_on_destruction);
#endif
    }

    WorldPathManager::~WorldPathManager()
    {
        if(m_owner == nullptr) return;
#ifndef NDEBUG
        assert(m_owner->peek_current_file() == m_expected_top_on_destruction);
        assert(m_owner->m_num_current_file_managers > 0);
        --m_owner->m_num_current_file_managers;
#endif
        m_owner->pop_current_file();
    }
}