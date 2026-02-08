### Navigation

- [Tournament Maker](../../../README.md)
  - [Tournament Builder Documentation](../../readme.md)
    - [Input and Output JSON](../readme.md)
      - [Competition](../competition.md)
      - [Competition Descriptors](../descriptors/readme.md)
      - [Competitor](../competitor.md)
      - [Entry List](../entry_list.md)
      - [Error messages](../error_messages.md)
      - **Events** (You are here)
        - [Add tags](add_tags.md)
        - [Resolve all references](resolve_all_references.md)
        - [Resolve reference](resolve_reference.md)
        - [Set finishing positions](set_finishing_positions.md)
      - [Names](../naming_rules.md)
      - [References](../references.md)
      - [Tags](../tags.md)
      - [World object](../world.md)

# Events

Events indicate things which happen which effect a [competion](../competition.md) or its [competitors](../competitor.md). When events are passed in the competition is parsed first, and then all the events are handled in order.

All events have the following field:

- **Required:** `event_type`: a unique string identifying what the event is.

## Event types

- [**Resolve reference:**](resolve_reference.md) takes a reference to a competition and attempts to resolve it.
- [**Resolve all references:**](resolve_all_references.md) tries to resolve all references in the [world](../world.md).
- [**Add tags:**](add_tags.md) adds [tags](../tags.md) to the targetted object.
- [**Set finishing positions:**](set_finishing_positions.md) sets the finishing order of entrants in a competition object.
