### Navigation

- [Tournament Maker](..\..\..\README.md)
  - [Tournament Builder Documentation](..\..\readme.md)
    - [Input and Output JSON](..\readme.md)
      - [Competition](..\competition.md)
      - [Competition Descriptors](..\descriptors\readme.md)
      - [Competitor](..\competitor.md)
      - [Entry List](..\entry_list.md)
      - [Error messages](..\error_messages.md)
      - [Events](readme.md)
        - [Add tags](add_tags.md)
        - **Resolve all references** (You are here)
        - [Resolve reference](resolve_reference.md)
        - [Set finishing positions](set_finishing_positions.md)
      - [Names](..\naming_rules.md)
      - [References](..\references.md)
      - [Tags](..\tags.md)
      - [World object](..\world.md)

# Resolve all references

`"event_type": "resolve_all_references"`

>[!TIP]
> You probably do not need to ever use this event directly.

This event has no extra properties. The root level competition will attempt to resolve all references in it to actual objects. This event is done implicitly before any events are run and after all events are run.
