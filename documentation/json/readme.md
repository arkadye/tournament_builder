### Navigation

- [Back to top](../../readme.md)
  - [Documentation home](../readme.md)
    - Input and output JSON
      - [Names](naming_rules.md)
      - [Tags](tags.md)
      - [References](references.md)
      - [World object](world.md)
      - [Competition object](competition.md)
      - [Entry List](entry_list.md)
      - [Competitor object](competitor.md)
      - [Competition Descriptor object](descriptors/readme.md)
	    - [Competitor view](descriptors/competitor_view.md)
        - [Round of matches](descriptors/round_of_matches.md)
        - [Round-robin](descriptors/round_robin.md)
        - [Knockout bracket](descriptors/knockout_bracket.md)
        - [DEBUG: Never resolves](descriptors/DEBUG_never_resolves.md)
      - [Events](events/readme.md)
        - [Resolve reference](events/resolve_reference.md)
        - [Resolve all references](events/resolve_all_reference.md)
        - [Add tags](events/add_tags.md)
        - [Set finishing positions](events/set_finishing_positions.md)
      - [Error messages](error_messages.md)

# Input and Output JSON

The main way to use this tool is inputting JSON objects and parsing the output.

The input can contain competition structures, including descriptors - which describe a structure to build (e.g. "round-robin" which generates an all-play-all round-robin structure) as well as events (e.g. setting the result of a particular phase or match). This allow you to express your competition in simple terms and have the full list of games/matches/races/whatever... be generated.

Alternatively this can be used to track progress. If a match has reference looking for the winner of a previous match, events can be used to set the winner of the previous match and have the next round update automatically.

The output will contain a fully detailed structure which you can parse and use. The outputs use the same format as the inputs, so you can add events to the output and feed it back into the input to update it.

When interpreting the JSON inputs, any unrecognised fields will be stripped out. Some types have a `user_data` field for adding arbitrary information which is expected to carry through to the output.

## Concepts

- [**Names:**](naming_rules.md) Many types are identified by a `"name":` field.
- [**Tags:**](tags.md) Many types have an optional `"tags":` field, allowing that instance to be tagged and found via tags instead of the name.
- [**References**](references.md) References can be used in place of many types to tell the system to copy the data from elsewhere in the structure, if it is available. For example: the entries to round 2 of an elimination bracket.

## Types

- [**World:**](world.md) The top-level type used as input and output.
- [**Competition:**](competition.md) This type describes a competition
- [**Competitor:**](competitor.md) This describes a competitor in the competition.
- [**Events:**](events/readme.md) Events are things which act on and possibly mutate a Competition.
- [**Error messages:**](error_messages.md) This type gets output instead of a world object if an error occured.
