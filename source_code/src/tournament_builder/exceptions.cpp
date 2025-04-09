#include "tournament_builder/exceptions.hpp"
#include "tournament_builder/name.hpp"

namespace tournament_builder::exception
{
	void TournamentBuilderException::add_context(std::string_view new_msg)
	{
		msg = std::format("{}: {}", new_msg, msg);
	}

	void TournamentBuilderException::add_context(const tournament_builder::NamedElement& context)
	{
		add_context(std::format("In {}", context.name));
	}
}