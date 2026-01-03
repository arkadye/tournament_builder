### Navigation

- [Back to top](../../readme.md)
  - [Documentation home](../readme.md)
    - [Input and output JSON](readme.md)
      - References

# References

Wherever something [named](naming_rules.md) (i.e. it has a `name` field) or [tagged](tags.md) (i.e. it supports a `tags` field) is referred to, it may also be referred to via a reference.

A reference is identified by its "ref" field which gets interpreted within the program. In its most basic form it is a list of Names, separated by dots (`.`), for example: if a field expecting a Competitor object gets `"championship.group_0.joe_bloggs"` would look for a Competition named `championship`, look in that for a competition named `group_0`, and look in that for a Competitor named `joe_bloggs`. If it cannot find one it will remain unresolved, and the output will contain the original reference instead of a competitor.

## Fields:

- **Required:** `ref`: the reference text.
- *Optional:* `text_replace`: a dictionary of string-to-string conversions to apply to the dereferenced object. Primarily used to customise generic structures.

## Reference Text

This is the text that defines how the reference works.

For example:

```json
{
	"ref": "anytown_utd.joe_bloggs"
}
```

In a situation where something may accept a reference (e.g. a competition entry list accepts Competitors, or references to Competitors) a string will be interpreted as a reference. That means there is no difference between:

```json
{
	"name": "example",
	"entry_list": [ "@ROOT.anytown_utd.joe_bloggs" ]
}
```

and:

```json
{
    "name": "example",
	"entry_list": [
		{
			"ref": "@ROOT.anytown_utd.joe_bloggs"
		}
	]
}
```
	
By default a reference will start in its current location, so consider this structure:

```json
{
  "name": "grandparent",
  "phases": [
	{
	  "name": "parent",
	  "phases": [
		{
		  "name": "child"
		}
	  ]
	},
	{ "ref": "parent.child" }
  ]
}
```
	
In `grandparent` the reference to `{ "ref": "parent.child" }` it would resolve to `{ "name": "child" }`.

By default the name only gets copied over, although additional fields can be used to transfer additional things as part of the resolution process.

There are also a number of special tokens that can be used to unlock more powerful functionality. The special tokens all begin with an `@` and are in upper-case. They are also case-sensitive.

In some situations (e.g. matching with tags which can be duplicated, or using special reference elements like `@ANY`) a reference can match with multiple items. In some circumstances this is fine and valid, and those circumstances will raise this. If it is in a situation where only a single result is expected a reference matching multiple things will act as though it matched with nothing. There is no error in this situation, since it is assumed that the reference will resolve correctly in the future.

### Special tokens

There are some special tokens in references that do special things in the lookup. All special tokens start with the character `@`.

#### `@HERE`

The `@HERE` token is replaced by a reference to the current location. Since references always start from the current location by default the references: `{ "ref": "@HERE.anytown_utd.joe_bloggs" }` and `{ "ref": "anytown_utd.joe_bloggs" }` are equivalent.

However, a `@HERE` token in the middle of a reference (e.g. `{ "ref": "anytown_utd.@HERE.joe_bloggs" }` it resets the parsing and anything before the `@HERE` token is discarded.

This token can either be used for clarity, or to simplify generators which generate references on the fly.

#### `@ROOT`

The `@ROOT` token may only appear at the very front of a reference. Instead of the reference being local they start at the very outer Competition structure.

For example:

```json
{
  "name": "grandparent",
  "phases": [
	{
	  "name": "parent",
	  "phases": [
		{
		  "name": "child"
		},
		{ "ref": "@ROOT.grandparent.aunt" }
	  ]
	},
	{
		"name": "aunt"
	}
  ]
}
```
	
In this `child` can reference its aunt using `{ "ref:" "@ROOT.grandparent" }`. 

A `@ROOT` token appearing mid-way through a reference will restart at the root again, discarding anything that came before it. This behaviour is typically more useful when scripting where references may be concatcenated.

#### `@OUTER`

The `@OUTER` token moves one level outwards. For example:

```json
{
  "name": "grandparent",
  "phases": [
	{
	  "name": "parent",
	  "phases": [
		{
		  "name": "child"
		}
	  ]
	}
  ]
}
```
	
In this, `child` could reference `parent` with `@OUTER`. Trying to use `@OUTER` from the top level (e.g. if `grandparent` tries to reference `@OUTER` in the example above) is an error.

It is possible to chain `@OUTER` (i.e. `@OUTER.@OUTER` to go up two levels), or to mix them in. `foo.bar.@OUTER` is equivalent to `foo`, for example.

Multiple `@OUTER`s chained together can be expressed using the shorthand `@OUTER:n` where `n` is the number of times to chain it. So `@OUTER:3` is equivalent to `@OUTER.@OUTER.@OUTER`. `n` must be a positive integer.

#### `@ANY` and `@GLOB`

The `@ANY` token matches with any (or all) items. This may allow a reference to match with multiple items.

Consider:

```json
{
  "name": "grandparent",
  "phases": [
	{
	  "name": "parent_A",
	  "phases": [
		{
		  "name": "child_X"
		},
		{
		  "name": "child_Y"
		},
		{
		  "name": "child_Z"
		},
	  ]
	},
	{
	  "name": "parent_B",
	  "phases": [
		{
		  "name": "child_X"
		},
		{
		  "name": "child_Y"
		},
		{
		  "name": "child_Z"
		},
	  ]
	},
	{
	  "name": "parent_C",
	  "phases": [
		{
		  "name": "child_X"
		},
		{
		  "name": "child_Y"
		},
		{
		  "name": "child_Z"
		},
	  ]
	}
  ]
}
```
	
The reference `grandparent.@ANY.childX` will resolve to `[grandparent.parent_A.childX , grandparent.parent_B.childX , grandparent.parent_C.childX ]`.

As a shorthand for chaining multiple `@ANY` references, a single number can be added to the end. `@ANY:2` is equivalent to `@ANY.@ANY`, for example. `@ANY` and `@ANY:1` are the same as well.

This shorthand can also be used to chain variable numbers of `@ANY` references by giving two arguments. So `@ANY:1:3` is the equivalent chaining together `@ANY`, `@ANY.@ANY` and `@ANY.@ANY.@ANY` and getting all the results. In this case the first argument can be 0, so `@ANY:0:2` is valid. Using a `@ANY` with a single argument is effectively the equivalent of having the same argument in both spaces. For example: `@ANY:2` is equivalent to `@ANY:2:2`; and `@ANY` is equivalent to `@ANY:1` and `@ANY:1:1`.

The `@GLOB` tag acts similarly, but defaults to matching any number of elements. `GLOB` is equivalent to `@ANY:0:2147483647`. A single argument can be provided to set the maximum, so `@GLOB:10` is equivalent to `@ANY:0:10`. When there are two arguments `@GLOB` and `@ANY` are equivalent.

#### `@STORE`

The world has several areas set aside for storing generic objects. The `@STORE` tag will go and look in those places, just like a regular tag. For example, in this World object:

```json
{
	"competition":{},
	"competitor_archetypes": [],
	"competition_archetypes": []
}
```

The `@STORE` token is context-aware. In a context where a Competitor is expected it will look in `competitor_archetypes`; for Competitions it will look in `competition_archetypes`, and so on for any new types which are added.

The examples in the **Text Replace** section show examples of this, since that field is primarily for use with the store.

## Text Replace

The optional `text_replace` field allows a find-and-replace to be performed on a dereferenced object before it is used. The main purpose of this is to customise a generic structure. This example is a use-case for a Competitor:

```json
{
    "competition": {
		"name": "example",
		"entry_list": [
			{
				"ref": "@STORE.ARCHETYPE_NAME",
				"text_replace": {
					"ARCHETYPE_NAME": "entry_a",
					"ARCHETYPE_IDENTIFIER": "id_foo"
				}
			},
			{
				"ref": "@STORE.ARCHETYPE_NAME",
				"text_replace": {
					"ARCHETYPE_NAME": "entry_b",
					"ARCHETYPE_IDENTIFIER": "id_bar"
				}
			}
		]
	},
	"competitor_archetypes": [
		{
			"name": "ARCHETYPE_NAME",
			"tags": ["lots" , "of" , "tags", "which", "I" , "do" , "not", "want" , "to", "repeat", "every", "time", "ARCHETYPE_IDENTIFIER"]
		}
	]
}
```

This is equivalent to:

```json
{
    "competition": {
		"name": "example",
		"entry_list": [
			{
			"name": "entry_a",
			"tags": ["lots" , "of" , "tags", "which", "I" , "do" , "not", "want" , "to", "repeat", "every", "time", "id_foo"]
			},
			{
			"name": "entry_b",
			"tags": ["lots" , "of" , "tags", "which", "I" , "do" , "not", "want" , "to", "repeat", "every", "time", "id_bar"]
			}
		]
	}
}
```

While a short example does not appear much simplier, this does mean that if the tags ever change only [one location needs to change to reflect that](https://en.wikipedia.org/wiki/Single_source_of_truth). This becomes even more useful if this archetype is referenced in many places, though such an example is a bit unwieldy for documentation. *[TODO: If an example uses this, add a link to it here.]*

The same principle is also applicable to sections of tournament. For example, pool play[^1] could be created like this:

```json
{
	"competition": {
		"name": "example",
		"phases": [
			{
				"ref": "@STORE.ROUND_ROBIN_GROUP",
				"text_replace"{
					"ROUND_ROBIN_GROUP": "group_a",
					"POOL_ID": "pool_a"
				}
			},
			{
				"ref": "@STORE.ROUND_ROBIN_GROUP",
				"text_replace"{
					"ROUND_ROBIN_GROUP": "group_b",
					"POOL_ID": "pool_b"
				}
			},
			{
				"ref": "@STORE.ROUND_ROBIN_GROUP",
				"text_replace"{
					"ROUND_ROBIN_GROUP": "group_c",
					"POOL_ID": "pool_c"
				}
			},
			{
				"ref": "@STORE.ROUND_ROBIN_GROUP",
				"text_replace"{
					"ROUND_ROBIN_GROUP": "group_d",
					"POOL_ID": "pool_d"
				}
			}
		]
	},
	"competition_archetypes": [
		{
			"descriptor_type": "round_robin",
			"name": "ROUND_ROBIN_GROUP",
			"entry_list": [ "@OUTER.pool_tag.POOL_ID" ],
			"num_times_to_play_each_opponent": 1,
			"alternate_left_and_right": true,
			"generate_byes": false
		}
	]
}
```

This will create four pools, each with parameters copied from the archetype.

[^1]: A descriptor specifically for pool play is planned which will make this example redundant in practice. But the principle can be expanded to more complex structures you create.