- [Back to top](../../../readme.md)
  - [Documentation home](..././readme.md)
    - [Input and output JSON](../readme.md)
      - [Events](readme.md)
        - [Resolve reference](resolve_reference.md)
        - [Resolve all references](resolve_all_reference.md)
        - [Add tags](add_tags.md)
        - [Set finishing positions](set_finishing_positions.md)

## Events

Events indicate things which happen which effect a competion or its competitors. When events are passed in the competition is parsed first, and then all the events are handled in order.

All events have the following field:

- **Required:** `event_type`: a unique string identifying what the event is.

### Resolve all references

`"event_type": "resolve_all_references"`

This event has no extra properties. The root level competition will attempt to resolve all references in it to actual objects. This event is done implicitly before any events are run and after all events are run.

### Resolve reference

`"event_type": "resolve_reference"`

Fields:

- **Required:** `target`: This is a reference to one or more competition objects. All references this competition, including its sub-phases, will attempt to be resolved. Failure to resolve them is not an error. If the reference resolves to many objects this event is applied to all of them. An array of references can also be passed in here as well and it will apply this event to everything.

### Add tags

`"event_type": "add_tags"`

Fields:

- **Required:** `target`: A reference, or array of references, to one or more competitor objects to have tags added to them.
- **Required:** `tags`: A tag, or array of tags, which will be added to all the competitors references.

A tag will only be added to a competitor once. If any attempt is made to add a duplicate tag to an element it silently fails. For special tags this includes ignoring the argument.

On each target this will attempt to resolve references before setting tags. If a target is an unresolved reference that target does not have tags set.

### Set finishing positions

`"event_type": "set_finishing_positions"`

Fields:

- **Required:** `target`: The competition having its finishing order set. This can only refer to a single competition.
- **Required:** `finish_order`: An array indicating the finishing order of the refereced competition.

This results in an error if the `target` cannot be resolved to a single competition or if any competitor in `entry_list` is an unresolved reference. This will implicitly do a `resolve_reference` on the target competition first.

The `finish_order` field tells the competition which competitor finished where. All competitors on the `entry_list` must appear in `finish_order`, and no competitor not in the `entry_list` may appear.

Typically this is done with a simple array of strings, representing the names. So if the `finish_order` is `[ "pears" , "oranges" , "lemons" , "apples" ]` then the competitor named `pears` will get the tag `$POS:1`, `oranges` will get the tag `$POS:2`, `lemons` will get the tag `$POS:3`, and `apples` will get `$POS:4`.

To set equal finishing positions, use a sub-array. If `oranges` and `lemons` finished equal-2nd, then the input `[ "pears" , [ "oranges" , "lemons" ] , "apples" ]` will give `oranges` and `lemons` the tag `$POS:2:3`. 

Note that it is possible to replicate this event with numerous `add_tags` events, but this method has far fewer reference lookups.
