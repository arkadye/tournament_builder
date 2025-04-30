### Navigation

- [Back to top](../../../readme.md)
  - [Documentation home](..././readme.md)
    - [Input and output JSON](../readme.md)
      - Events
        - [Resolve reference](resolve_reference.md)
        - [Resolve all references](resolve_all_references.md)
        - [Add tags](add_tags.md)
        - [Set finishing positions](set_finishing_positions.md)

# Events

Events indicate things which happen which effect a [competion](../competition.md) or its [competitors](../competitor.md). When events are passed in the competition is parsed first, and then all the events are handled in order.

All events have the following field:

- **Required:** `event_type`: a unique string identifying what the event is.

## Event types

- [**Resolve reference:**](resolve_reference.md) takes a reference to a competition and attempts to resolve it.
- [**Resolve all references:**](resolve_all_references.md) tries to resolve all references in the [world](../world.md).
- [**Add tags:**](add_tags.md) adds [tags](../tags.md) to the targetted object.
- [**Set finishing positions:**](set_finishing_positions.md) sets the finishing order of entrants in a competition object.
