### Navigation

- [Tournament Maker](../../../README.md)
  - [Tournament Builder Documentation](../../readme.md)
    - [Input and Output JSON](../readme.md)
      - [Competition](../competition.md)
      - [Competition Descriptors](readme.md)
        - [Competitor View](competitor_view.md)
        - **DEBUG: Never resolves** (You are here)
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

# DEBUG: Never resolves

> [!CAUTION]
> Do not use this descriptor outside of testing environments.

This descriptor has no additional fields. It will never resolve. When it outputs itself, it adds a `"comment":` field explaining its purpose.
