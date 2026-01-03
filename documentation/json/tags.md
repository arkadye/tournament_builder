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

Negative indices can be used to count from the end. So to capture the entry who finished 4th from bottom, use `$POS:-4`. To get the bottom three use `$POS:-3:-1`.

A zero index does not exist. Trying to get `$POS:0` will always fail, but will not report an error.

To set these tags use the `"set_finishing_order"` event.

Unlike other special tags, these tags will appear on the output.

This tag is unique. It cannot appear as part of a chain. So `example.$POS:1.oops` is an error, as is `$POS:1.example`.

If this tag is given a negative index (e.g. `$POS:-1`) it counts from the back. So `$POS:-1` will be the competitor in *last* place, `$POS:-2` the competitor 2nd to last, and so on. The bottom three could be referenced using `$POS-3:-1`, and first-and-last would be $POS:-1:1`.

[^1]: Note to programmers: this is 1-indexed because actual league tables are also 1-indexed.

### Special Tag `$ENTRY`

This corresponds to a competitor in a competition. Whichever competitor is listed first in the entry list implicity has the tag `$ENTRY:1`, and the numbering works exactly like the `$POS` numbering.

This tag is unique. It cannot appear as part of a chain. So `example.$ENTRY:1.oops` is an error, as is `$ENTRY:1.example`.

If this tag is given a negative index (e.g. `$ENTRY:-1`) it counts from the back. So `$ENTRY:-1` will be the *last* competitor in the entry list, `$ENTRY:-2` the competitor 2nd to last, and so on. The last three could be referenced using `$ENTRY-3:-1`, and first-and-last would be `$ENTRY:-1:1`.

This tag is omitted from the output.

### Special Tag tokens `$ANY` and `$GLOB`

These special tokens are wildcards. For example: `Example.$ANY` will match with `Example.Fred`, `Example.Shaggy`, `Example.Scooby`, etc...

An `$ANY` tag may be given an argument like so: `$ANY:3`. This example requires `$ANY` to match with three tokens, and is equivalent to `$ANY.$ANY.$ANY`.

An `$ANY` tag can be given two arguments to match with a variable number of tags. So `X.$ANY:0:2.Y` is equivalent to `X.Y` and `X.$ANY.Y` and `X.$ANY.$ANY.Y`.

`$ANY:5` (for example) is equivalent to `$ANY:5:5`. And `$ANY` (without an argument) is equivalent to `$ANY:1` and `$ANY:1:1`.

The `$GLOB` tag acts similarly, but with different defaults. `$GLOB` with no arguments is equivalent to `$ANY:0:2147483647` [^2]. By default this will match with any number of tokens.

With a single argument, this sets a maximum. So `$GLOB:10` is equivalent to `$ANY:0:10`.

`$GLOB` can also be given multiple arguments (`$GLOB:3:5`, for example). In this case it behaves exactly the same as an `$ANY` tag with the same arguments.

Wildcard tags do not consider `$POS` or `$ENTRY` tags when matching.

[^2]: 2147483647 is 2^31  - 1 - the largest number can than be stored in a signed 32-bit integer.
