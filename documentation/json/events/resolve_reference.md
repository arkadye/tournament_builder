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
        - [Resolve all references](resolve_all_references.md)
        - **Resolve reference** (You are here)
        - [Set finishing positions](set_finishing_positions.md)
      - [Names](..\naming_rules.md)
      - [References](..\references.md)
      - [Tags](..\tags.md)
      - [World object](..\world.md)
       
# Resolve reference

`"event_type": "resolve_reference"`

This will take a target and attempt to resolve all references on that particular object.

> [!TIP]
> You will very rarely have to use this directly.

## Fields:

- **Required:** `target`: This is a reference to one or more competition objects. All references this competition, including its sub-phases, will attempt to be resolved. Failure to resolve them is not an error. If the reference resolves to many objects this event is applied to all of them. An array of references can also be passed in here as well and it will apply this event to everything.  
