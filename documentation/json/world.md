### Navigation

- [Tournament Maker](..\..\README.md)
  - [Tournament Builder Documentation](..\readme.md)
    - [Input and Output JSON](readme.md)
      - [Competition](competition.md)
      - [Competition Descriptors](descriptors\readme.md)
      - [Competitor](competitor.md)
      - [Entry List](entry_list.md)
      - [Error messages](error_messages.md)
      - [Events](events\readme.md)
      - [Names](naming_rules.md)
      - [References](references.md)
      - [Tags](tags.md)
      - **World object** (You are here)

# World object

This is the unnamed very outer object that holds everything within it. One of these is taken as the input and a modified one returned as the output.

For examples of what a world object looks like, [you may refer to the examples folder](../../examples/readme.md).

## Fields:

- **Required:** `competition`: the [competition object](competition.md) or the [competition descriptor](descriptors/readme.md) to be worked with.
- *Optional:* `events`: an array of [event objects](events/readme.md) to apply to `competition`.
- *Optional:* `templates`: a container of templates to use. A [reference](references.md) may use the `@TEMPLATE` element to find generic objects from here and customise them by doing a find-and-replace on the strings.
- *Optional:* `preserve_templates`: a Boolean. If `True` (default) the `templates` section will be written to the output. If `False` the output will not include the `templates` field. If no templates are set this has no effect.

## Templates

There are no hard rules about the structure here. Consider:

```json
{
    "competition": {
        "name": "example"
    },
    "templates": {
        "foo": {
            "bar": {
                "baz": {
                    "name": "#GENERIC_COMPETITOR",
                    "tags": ["lorem", "ipsum"]
                }
            },
            "bop": { "name": "#GENERIC_COMPETITOR" }
        },
        "final_match": {
            "name": "#FINAL_NAME",
            "entry_list": ["@OUTER.#SEMIS_NAME.match_0.$POS:1","@OUTER.SEMIS_NAME.match_1.$POS:1"]
        },
        "flip": [
            {"name": "steve"},
            {"name": "david"},
            {"name": "nicko"},
            {"name": "jannick"},
            {"name": "bruce"},
            {"name": "adrian"}
        ]
    }
}
```

 For a reference like: `@TEMPLATE.foo.bar.baz` would find the field `foo`, and in there would find `bar`, and in there find `baz`, and would resolve to something like:

```json
{
    "name": "#GENERIC_COMPETITOR",
    "tags": ["lorem", "ipsum"]
}
```

This can be read as a competitor. There is no `phases` nor `entry_list` field, so it would cause an error if it were attempted to read this were a competition is expected.

Notice that the name `#GENERIC_COMPETITOR` has a character that is not allowed in a name ('#'). To solve this use the `text_replace` part of a reference to do a find-and-replace. You can use this to replace the `#GENERIC_COMPETITOR` text with something chosen *in situ*. There is no requirement for any text here to include the '#' to force that to happen, but it is recommended to avoid errors.

An array can be indexed into by using a number: `@TEMPLATE.flip.5` would get item index `5` in the `flip` array, which is `{"name": "adrian"}`. In this case there is no error if no find-and-replace keeps the original `name`, because `adrian` is a valid name.

Be aware that elements in the `templates` section that are never reference are never referenced will not cause an error, even if they are not valid TourmentBuilder ojects, as long as they are valid JSON.

Additionally, planed for release.

- [Planned for release] *Optional:* `stats`: an array of stat types you may wish to use.
- [Planned for release] *Optional:* `resolvers`: an array of competition resolvers you may wish to use. Resolvers may be used to tell a Competition how to interact with stats.
- [Planned for release] *Optional:* `random_seed`: a number between `0` and `18446744073709551615` used as the random seed for any structures that have random elements. Setting this will force "random" functionality to always produce the same output. This is generally used for reproducability. It it is not set on the input, and random elements are used, the random seed generated will be put on the output.
