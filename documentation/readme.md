# Tournament Builder Documentation

The documentation for this tool is here:

- [Top level](../README.md)
  - Documentation home
    - [Input and output JSON](json/readme.md)
      - [Names](json/naming_rules.md)
      - [Tags](json/tags.md)
      - [References](json/references.md)
      - [World object](json/world.md)
      - [Competition object](json/competition.md)
      - [Competitor object](json/competitor.md)
      - [Entry List](json/entry_list.md)
      - [Competition Descriptor object](json/descriptors/readme.md)
	    - [Competitor view](json/descriptors/competitor_view.md)
        - [Round of matches](json/descriptors/round_of_matches.md)
        - [Round-robin](json/descriptors/round_robin.md)
        - [Knockout bracket](json/descriptors/knockout_bracket.md)
		- [DEBUG: Never resolves](json/descriptors/DEBUG_never_resolves.md)
      - [Events](json/events/readme.md)
        - [Resolve reference](json/events/resolve_reference.md)
        - [Resolve all references](json/events/resolve_all_reference.md)
        - [Add tags](json/events/add_tags.md)
        - [Set finishing positions](json/events/set_finishing_positions.md)
       
The main headers are:

- [**Input and output JSON:**](json/readme.md) This covers the JSON files that are used as input and output to tournament_builder, whether passed programmatically via one the APIs or to the command-line interface.
- **Command-line interface:** Details on how to use the commmand line interface. (Coming soon.)
- **C++ Library:** How to build and use the C++ library version. (Coming eventually.)
- **C interface:** How to build and use the C interface. (Coming soon.)
- **C#/mono interface:** How to build and use the C# interface, and integrate with the Unity game engine. (Coming soon.)
