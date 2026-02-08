### Navigation

- [Tournament Maker](../../../README.md)
  - [Tournament Builder Documentation](../../readme.md)
    - [Input and Output JSON](../readme.md)
      - [Competition](../competition.md)
      - **Competition Descriptors** (You are here)
        - [Competitor View](competitor_view.md)
        - [DEBUG: Never resolves](DEBUG_never_resolves.md)
        - [Knockout-bracket](knockout_bracket.md)
        - [Round Robin](round_robin.md)
        - [Round of Matches](round_of_matches.md)
      - [Competitor](../competitor.md)
      - [Entry List](../entry_list.md)
      - [Error messages](../error_messages.md)
      - [Events](../events/readme.md)
      - [Names](../naming_rules.md)
      - [References](../references.md)
      - [Tags](../tags.md)
      - [World object](../world.md)

# Competition Descriptors

Descriptors are objects that may be used in place of a [competition object](../competition.md). They describe how to create phase of a competition and generate a competition structure from it.

## Fields

All descriptors have the following fields in addition to the ones specific to this descriptor:

- **Required:** `descriptor_type`: a string identifying the type of the descriptor. The parser uses this to know which type of descriptor it is building. This *is* case-sensitive. See the documentation on each descriptor type to see what values are valid here.
- **Required**: `name`: the name given to the resulting competition.
- *Optional:* `tags`: Tags applied to the competition once it is built.

All descriptors are identified by a field named `descriptor_type` which tells the parser which type of descriptor to create. 


## Types

- [**Round of matches:**](round_of_matches.md) (`round_of_matches`) converts a list of [competitors](../competitor.md) into a single match each.
- [**Round-robin:**](round_robin.md) (`round_robin`) creates an everyone-plays-everyone round-robin, grouped into rounds.
- [**Knockout bracket:**](knockout_bracket.md) (`round_robin`) creates a single-elmination knockout competition from a list of entries.

These types are provided for debugging or testing, and are not intended to be used in any real situations.

- [**Never resolves:**](DEBUG_never_resolves.md) `DEBUG_never_resolves`: Test whether functionality allowing descriptors to defer resolving works correctly.
