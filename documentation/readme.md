# Tournament Builder Documentation

The documentation for this tool is here:

- [Tournament Maker](..\README.md)
  - **Tournament Builder Documentation** (You are here)
    - [Input and Output JSON](json\readme.md)
      - [Competition](json\competition.md)
      - [Competition Descriptors](json\descriptors\readme.md)
        - [Competitor View](json\descriptors\competitor_view.md)
        - [DEBUG: Never resolves](json\descriptors\DEBUG_never_resolves.md)
        - [Knockout-bracket](json\descriptors\knockout_bracket.md)
        - [Round Robin](json\descriptors\round_robin.md)
        - [Round of Matches](json\descriptors\round_of_matches.md)
      - [Competitor](json\competitor.md)
      - [Entry List](json\entry_list.md)
      - [Error messages](json\error_messages.md)
      - [Events](json\events\readme.md)
        - [Add tags](json\events\add_tags.md)
        - [Resolve all references](json\events\resolve_all_references.md)
        - [Resolve reference](json\events\resolve_reference.md)
        - [Set finishing positions](json\events\set_finishing_positions.md)
      - [Names](json\naming_rules.md)
      - [References](json\references.md)
      - [Tags](json\tags.md)
      - [World object](json\world.md)
       
The main headers are:

- [**Input and output JSON:**](json/readme.md) This covers the JSON files that are used as input and output to tournament_builder, whether passed programmatically via one the APIs or to the command-line interface.
- **Command-line interface:** Details on how to use the commmand line interface. (Coming soon.)
- **C++ Library:** How to build and use the C++ library version. (Coming eventually.)
- **C interface:** How to build and use the C interface. (Coming soon.)
- **C#/mono interface:** How to build and use the C# interface, and integrate with the Unity game engine. (Coming soon.)
