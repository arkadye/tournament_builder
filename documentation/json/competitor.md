### Navigation

- [Tournament Maker](../../README.md)
  - [Tournament Builder Documentation](../readme.md)
    - [Input and Output JSON](readme.md)
      - [Competition](competition.md)
      - [Competition Descriptors](descriptors/readme.md)
      - **Competitor** (You are here)
      - [Entry List](entry_list.md)
      - [Error messages](error_messages.md)
      - [Events](events/readme.md)
      - [Names](naming_rules.md)
      - [References](references.md)
      - [Tags](tags.md)
      - [World object](world.md)

# Competitor

This object describes a competitor into the competition.

Fields:

- **Required:** `name`: the [name](naming_rules.md) of the competitor.
- *Optional:* `tags`: an array of [tag objects](tags.md).
- *Optional:* `user_data`: arbitrary JSON which will be carried on this element as it gets copied.
