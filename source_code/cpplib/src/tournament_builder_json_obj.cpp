#include "tournament_builder_json_obj.hpp"
#include "tournament_builder_native.hpp"

#include "tournament_builder/serialize.hpp"
#include "tournament_builder/exceptions.hpp"

namespace tournament_builder
{
	namespace internal_tournament_builder_native
	{
		template <typename T>
		nlohmann::json wrap(T&& arg, const ExtraArgs& extra_args)
		{
			nlohmann::json result;
			try
			{
				const World world = make_tournament_world(std::forward<T>(arg), extra_args);
				result = to_json(world);
			}
			catch (const exception::TournamentBuilderException& ex)
			{
				result["errors"] = { ex.what() };
			}
			return result;
		}
	}

	nlohmann::json make_tournament_json(std::string_view input, const ExtraArgs& extra_args) { return internal_tournament_builder_native::wrap(input, extra_args); }
	nlohmann::json make_tournament_json(const std::filesystem::path& input, const ExtraArgs& extra_args) { return internal_tournament_builder_native::wrap(input, extra_args); }
	nlohmann::json make_tournament_json(std::istream& input, const ExtraArgs& extra_args) { return internal_tournament_builder_native::wrap(input, extra_args); }
	nlohmann::json make_tournament_json(const nlohmann::json& input, const ExtraArgs& extra_args) { return internal_tournament_builder_native::wrap(input, extra_args); }
	nlohmann::json make_tournament_json(const tournament_builder::World& input, const ExtraArgs& extra_args) { return internal_tournament_builder_native::wrap(input, extra_args); }
}