#include "tournament_builder/entry_list.hpp"
#include "tournament_builder/json/json_helpers.hpp"

namespace tournament_builder
{
	using nlohmann::json;
	EntryList EntryList::parse(const json& input)
	{
		EntryList result;
		result.entries = json_helper::get_array(input, Reference<Competitor>::parse);
		return result;
	}

	EntryList EntryList::parse(const json& input, std::string_view field_name)
	{
		try
		{
			json array_view = json_helper::get_array_object(input, field_name);
			return parse(array_view);
		}
		catch (exception::TournamentBuilderException& ex)
		{
			ex.add_context(std::format("In field '{}'", field_name));
			throw ex;
		}
	}
}