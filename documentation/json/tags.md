### Navigation

- [Back to top](../../readme.md)
  - [Documentation home](../readme.md)
    - [Input and output JSON](readme.md)
      - Tags

# Tags

Things with tags or attributes have a field named `tags`, which holds an array of tag objects. Tags are similar to names and generally follow the same rules. However, a tag may consist of multiple tokens, separated by `.` (e.g. `"part_1.part_2.part_3"`). Furthermore, there are some special tokens, identified by starting with a `$` symbol, which have special meanings, such as `$ANY` which will match with everything.

References can match with something with a name or with a tag.

To differentiate a tag within a reference, a tag with `.`s in it must have `[square brackets]` around it. E.g.: `@ROOT.part_1.[MULTI.PART.TAG].part_2`.

## Fields

- **Required:** `tag`: a string of the actual tag text
- *Optional:* `copy_on_ref`: a boolean which tells the reference system whether this tag should be copied when resolving a reference. This defaults to `true` when set as part of the object's JSON setup, and `false` when added as the result of an event.

Alternatively (and similar to references) a string can be used in place of a tag. This means there is no difference between:

```json
{
	"name": "example",
	"tags": [
		{
			"tag": "example_tag_1",
			"copy_on_ref": true
		} 
	]
}
```
	
and

```json
{
	"name": "example",
	"tags": [ "example_tag_1" ]
}
```

In the latter case `copy_on_ref` is assumed to be `true`.

## Tags and references

Where a reference refers to something with tags it will also match with a name. For example:

```json
{
	"name": "example",
	"phases": [
		{
			"name": "round_0"
			"entry_list":
			[
				{
					"name": "tea",
					"tags": [ "beverages" ]
				},
				{
					"name": "lemon",
					"tags": [ "fruit" ]
				}
			]
		},
		{
				"name": "round_1",
				"entry_list": [ { "ref": "$OUTER.round_0.beverages" } ]
		},
	]
}
```
	
In the above the tag will pick up the "tea" entity, because the tag "beverages" matches. So when references are resolved it will become this:

```json
{
	"name": "example",
	"phases": [
		{
			"name": "round_0"
			"entry_list": [
				{
					"name": "tea",
					"tags": [ "beverages" ]
				},
				{
					"name": "lemon",
					"tags": [ "fruit" ]
				}
			]
		},
		{
				"name": "round_1",
				"entry_list": [
					{
						"name": "tea",
						"tags": [ "beverages" ]
					}
				]
		},
	]
}
```

On each object tags are unique. Any attempt to add a duplicate tag will do nothing, but it is not an error.

## Special Tags

There are various tags with special meanings. They all start with the character `$`.

### Special Tag `$POS`

When a competition the finishing order of all the entries are set. Whichever competitor is placed first gets the tag `$POS:1`, 2nd gets the tag `$POS:2`, and so on. [^1]

If a group of competitors finished equal in a position they get a tag indicating the range of positions. So if four competitors finished equal third they would all get the tag `$POS:3:6`. This will match with references `$POS:3`, `$POS:4`, `$POS:5`, and `$POS:6`, or any references looking for a range of those positions (e.g. a reference to `$POS:6:8` will pick up all those competitors as well).

To set these tags use the `"set_finishing_order"` event.

Unlike other special tags, these tags will appear on the output.

This tag is unique. It cannot appear as part of a chain. So `example.$POS:1.oops` is an error, as is `$POS:1.example`.

[^1]: Note to programmers: this is 1-indexed because actual league tables are also 1-indexed.

### Special Tag `$ENTRY`

This corresponds to a competitor in a competition. Whichever competitor is listed first in the entry list implicity has the tag `$ENTRY:1`, and the numbering works exactly like the `$POS` numbering.

This tag is unique. It cannot appear as part of a chain. So `example.$ENTRY:1.oops` is an error, as is `$ENTRY:1.example`.

This tag is omitted from the output.
