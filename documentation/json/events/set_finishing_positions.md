### Navigation

- [Back to top](../../../readme.md)
  - [Documentation home](..././readme.md)
    - [Input and output JSON](../readme.md)
      - [Events](readme.md)
        - Set finishing positions

# Set finishing positions

`"event_type": "set_finishing_positions"`

Set the finishing orders on a [competition](../competition.md).

## Fields:

- **Required:** `target`: The competition having its finishing order set. This can only refer to a single competition.
- **Required:** `finish_order`: An array indicating the finishing order of the refereced competition.

This results in an error if the `target` cannot be resolved to a single competition or if any competitor in `entry_list` is an unresolved reference. This will implicitly do a `resolve_reference` on the target competition first.

The `finish_order` field tells the competition which competitor finished where. All competitors on the `entry_list` must appear in `finish_order`, and no competitor not in the `entry_list` may appear.

Typically this is done with a simple array of strings, representing the names. So if the `finish_order` is `[ "pears" , "oranges" , "lemons" , "apples" ]` then the competitor named `pears` will get the tag `$POS:1`, `oranges` will get the tag `$POS:2`, `lemons` will get the tag `$POS:3`, and `apples` will get `$POS:4`.

To set equal finishing positions, use a sub-array. If `oranges` and `lemons` finished equal-2nd, then the input `[ "pears" , [ "oranges" , "lemons" ] , "apples" ]` will give `oranges` and `lemons` the tag `$POS:2:3`. 

Note that it is possible to replicate this event with numerous `add_tags` events, but this method has far fewer reference lookups.
