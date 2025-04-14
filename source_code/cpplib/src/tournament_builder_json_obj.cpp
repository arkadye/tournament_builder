#include "tournament_builder_json_obj.hpp"
#include "tournament_builder_native.hpp"

#include "tournament_builder/serialize.hpp"
#include "tournament_builder/exceptions.hpp"

namespace tournament_builder
{
	namespace internal_tournament_builder_native
	{
		template <typename T>
		nlohmann::json wrap(T&& arg)
		{
			nlohmann::json result;
			try
			{
				const tournament_builder::World world = make_tournament_world(std::forward<T>(arg));
				result = tournament_builder::to_json(world);
			}
			catch (const tournament_builder::exception::TournamentBuilderException& ex)
			{
				result["error"] = ex.what();
			}
			return result;
		}
	}

	nlohmann::json make_tournament_json(std::string_view input) { return internal_tournament_builder_native::wrap(input); }
	nlohmann::json make_tournament_json(const std::filesystem::path& input) { return internal_tournament_builder_native::wrap(input); }
	nlohmann::json make_tournament_json(std::istream& input) { return internal_tournament_builder_native::wrap(input); }
	nlohmann::json make_tournament_json(const nlohmann::json& input) { return internal_tournament_builder_native::wrap(input); }
	nlohmann::json make_tournament_json(const tournament_builder::World& input) { return internal_tournament_builder_native::wrap(input); }
}