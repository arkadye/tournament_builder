### Navigation

- [Back to top](../../../readme.md)
  - [Documentation home](..././readme.md)
    - [Input and output JSON](../readme.md)
      - [Events](readme.md)
        - Resolve all references

# Resolve all references

`"event_type": "resolve_all_references"`

>[!TIP]
> You probably do not need to ever use this event directly.

This event has no extra properties. The root level competition will attempt to resolve all references in it to actual objects. This event is done implicitly before any events are run and after all events are run.
