### Navigation

- [Tournament Maker](../../README.md)
  - [Tournament Builder Documentation](../readme.md)
    - [Input and Output JSON](readme.md)
      - [Competition](competition.md)
      - [Competition Descriptors](descriptors/readme.md)
      - [Competitor](competitor.md)
      - **Entry List** (You are here)
      - [Error messages](error_messages.md)
      - [Events](events/readme.md)
      - [Names](naming_rules.md)
      - [References](references.md)
      - [Tags](tags.md)
      - [World object](world.md)

# Entry List

This object represents the list of  [competitors](competitor.md) or [references to competitors](references.md) taking part in a [competition](competition.md).

## Fields:

- **Required:** `entries`: an ordered array of either competitors or references to competitors. A bare string is intepreted as a reference.
- *Optional:*: `num_entries`: the number of entries expected. If unspecified this is set to the length of the `entries` field, assuming one entry per entry there. However, it is possible to use this to indicate more entries (e.g. in situations where a reference may capture many entries) or fewer entries (e.g. in situations where not all references are expected to complete).
- *Optional:* `min_entries`: where a variable number of entries are expected, set the minimum number. Must be less than or equal to `max_entries`. If `num_entries` is also set this will raise an error. If `max_entries` is set, but this is omitted it will default to `0`.
- *Optional:* `max_entries`: where a variable number of entries are expected, set the maximum number. Must be greater than or equal to `min_entries`. If `num_entries` is also set this will raise an error. If `min_entries` is set, but this is omitted it will default to `2147483648`.

A short entry list may be presented. An array of competitors (or references to competitors) will be interpreted as if that array is the `entries` field with the optional fields set to their default values.

The optional values can be combined with the features of references limiting how many values a single reference may capture.

## Examples

On a Competition the field for an entry list is called `entry_list`.

The simple way to represent this is as follows:

```json
{
    "name": "simple_example",
    "entry_list": [{"name": "entry_a"} , {"name": "entry_b"}, "entry_c_reference"]
}
```

This uses the feature of interpreting an array as an entry list with only the `entries` field set. It is equivalent to:

```json
{
    "name": "simple_example",
    "entry_list": {
        "entries": [{"name": "entry_a"} , {"name": "entry_b"}, "entry_c_reference"]
    }
}
```

This is equivalent to:

```json
{
    "name": "simple_example",
    "entry_list": {
        "entries": [{"name": "entry_a"} , {"name": "entry_b"}, "entry_c_reference"],
        "num_entries": 3
    }
}
```

And also equivalent to:

```json
{
    "name": "simple_example",
    "entry_list": {
        "entries": [{"name": "entry_a"} , {"name": "entry_b"}, "entry_c_reference"],
        "min_entries": 3
        "max_entries": 3
    }
}
```