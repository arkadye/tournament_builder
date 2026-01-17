### Navigation

___NAV-MAP___
       
# Resolve reference

`"event_type": "resolve_reference"`

This will take a target and attempt to resolve all references on that particular object.

> [!TIP]
> You will very rarely have to use this directly.

## Fields:

- **Required:** `target`: This is a reference to one or more competition objects. All references this competition, including its sub-phases, will attempt to be resolved. Failure to resolve them is not an error. If the reference resolves to many objects this event is applied to all of them. An array of references can also be passed in here as well and it will apply this event to everything.  
