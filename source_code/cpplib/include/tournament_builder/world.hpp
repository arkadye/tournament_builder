#pragma once

#include "tournament_builder/competition.hpp"
#include "tournament_builder/reference.hpp"
#include "tournament_builder/events/event_base.hpp"
#include "tournament_builder/extra_args.hpp"
#include "tournament_builder/world_path_manager.hpp"

#include "nlohmann/json_fwd.hpp"

#include <optional>
#include <vector>
#include <string>
#include <cstdint>
#include <random>

namespace tournament_builder
{
	class World : public IReferencable
	{
	public:
		World() = default;
		World(const World& other);
		World& operator=(const World& other);

		World(World&&) noexcept = default;
		World& operator=(World&& other) noexcept = default;

		Competition competition;
		std::vector<event::EventHandle> events;
		std::unique_ptr<nlohmann::json> template_store;
		bool preserve_templates = true;
		
		std::vector<std::string> error_messages;
		static World parse(const nlohmann::json& input, const ExtraArgs& extra_args);

		void resolve_all_references();
		void execute_all_events();

		template <typename T>
		std::vector<ReferenceResult<T>> dereference(const SoftReference& soft_reference);

		// IReferencable
		Name get_reference_key() const final;
		std::shared_ptr<IReferencable> copy_ref(const ReferenceCopyOptions&) const final;
		std::vector<IReferencable*> get_all_next_locations() final;
		bool matches_token(const Token&) const final;

		void apply_extra_args(const ExtraArgs& args);

		const std::filesystem::path& get_base_file() const { return m_base_file; }
		const std::filesystem::path& get_current_file() const;

		[[nodiscard]] WorldPathManager add_temp_current_file(std::filesystem::path path);

	private:
		friend class WorldPathManager;
		std::filesystem::path m_current_file{};
		std::filesystem::path m_base_file{};
#ifndef NDEBUG
		int m_num_current_file_managers = 0;
#endif
	};

	template <typename T>
	std::vector<ReferenceResult<T>> World::dereference(const SoftReference& soft_reference)
	{
		Reference<T> ref{ soft_reference };
		return ref.dereference_multi(*this, {});
	}
}