# Tournament Maker

Build tournaments and do basic operations on them with this tool.

## Features

This tool accepts a competition description in JSON. This can be arbitrarily complex, within the limits of the machine you are running on. From this it will generate a list of matches and associated league tables, with logic for chaining the various matches and stages together.

At some point the ability to run Swiss formats might be added.

## Who is this for?

Primarily targetted at indie devs, though there is nothing to stop AAA studios using this if they really want, this tool is to aid you building sports or sports-adjacent games. I've known a few who build certain tournament structures into the game, and then turn down "can we have a format where..." requests because would be hard to incorporate other formats.

The idea here is that this can either be used as an offline tool to help build new formats to respond to those requests, or - even easier - your game could integrate this library and bundle the tournament descriptors in a free format, parsing them, saving the results, and using them as needed. It would then be possible for modders to create their own tournament descriptors to run within the game, and all you would need to do would be to point them to the appropriate file(s) to edit, and to this documentation.

In principle it could also be used by people running actual sports tournaments as an aid and that is a use-case that is supported. So if you have a feature request for this purpose do feel free to use it. In many ways the use of JSON was motivated by this use-case, since it is relatively easy for webservers to injest.

## Quickstart guide

### Building from source

Fork this repo and integrate the source code in your favourite manner. The CMakeLists.txt file in `source_code/static_library` should integrate the source code into your project.

The C++ interface is currently not documented here, although the headers are hopefully comprehensible.

There is also a `tournament_builder_cli_dbg` project which automatically finds the files in the example folder and runs targetting them. This is convenient for things like Visual Studio. If you wish to work from Visual Studio and its like, I suggest making this your default startup project.

### Unity Engine integration

There are two DLLs required: `tournament_maker_clib.dll` and `tournament_maker_mono.dll`.

In the "Assets" panel in Unity, right-click and select "Import New Asset..." Select both those DLLs and import them.

The C# interface is now available in Unity under the `TournamentBuilder.TournamentBuilder` namespace.

More detailed documentation to come.

### As a command line tool.

The command line tool will be made available in the Releases section. It can be invoked with the command `tourmanet_builder_cli --input [input file] --output [output file]`. Leave `--input` blank to read JSON from stdin (i.e. you can pipe it in) and leave `--output` blank to write the results to stdout (or pipe it out). More in-depth documentation to come.

### Input

A World JSON object is the input. See the JSON documentation below.

### Output

A different World JSON object is the output. This means the outputs can be modified as used again as inputs.

If an error (or errors) occur, this will instead output a JSON object with just a single field: `"errors"`, with an array of error messages.

## Compatibility
 
This is designed to be a standalone command line tool or as a C++ library, but a C interface is also planned allowing bindings with any language that supports them.
 
The C bindings can be used to interface with:
 
- Lua
- Python
- C#

Between these it can be used with the following engines:

- Unreal Engine (as a C++ library)
- Unity (as a C# assembly)
- GameMaker (as a DLL)
- PyGame (via Python bindings)

Conversion to an Unreal Plugin, or a GameMaker extension, or a Unity library is on the to-do list at some point. If you do this for your own use and are happy to share do feel free to [open a pull request](https://github.com/arkadye/tournament_builder/pulls) and I'll see what I can do.

Data is stored as JSON, which is a reasonably human-readable format.

## Usage

There are, broadly, two (maybe three, depending on how you count) ways to use this.

- An offline tool to simplify the work of generating tournament structures for you. You can then
injest the created structures as you build your program.

- An inline tool where your game stores actual tournament layouts and parses them when needed.
This way your users can then modify the game files to create their own structures, using
the offline version of this tool to help.

- An integrated tool, where this is embedded into your game allowing the players to build
structures within the game.

## Roadmap

### Current state: Alpha

This is the current state. Not everything planned is in, but minimal possibly useful functionality exists. At this point, share within small(-ish) programming communities only, but it is not ready for non-programmer focussed indie-devs yet.

[See everything planned for Alpha here.](https://github.com/arkadye/tournament_builder/milestone/1)

### Beta

This is the point to start sharing in indie-dev circles, since it is now useable enough to integrate with Unity and possibly GameMaker, even if it is not feature complete.

[See everything planned for Beta here.](https://github.com/arkadye/tournament_builder/milestone/2)


### Release

[See everything planned for Release here.](https://github.com/arkadye/tournament_builder/milestone/3)

### Post-release

These are things I don't plan to work on, but will accept pull requests for.

[See everything which would currently be 'Post-release' here.](https://github.com/arkadye/tournament_builder/milestone/4)


## Contributing

The easiest way to contribute is to click on the ["Issues" tab](https://github.com/arkadye/tournament_builder/issues), and either comment on an issue you see that is similar or start your own.

Use these for bug reports, feature requests, no matter how major or minor they seem.

If you want to contribute by writing code [make a fork](https://github.com/arkadye/tournament_builder/fork), and do your thing and make a pull request. If you would like to contribute but are not sure what to do there is a list of ideas above and there may be some things in "Issues" which you think you could solve.

## Attribution

Under the terms of the license there is no need to attribute or acknowledge this work. However, if you use it I would much appreciate it if you could let me know so I can maintain a list of projects that have used this tool.

Furthermore, while I acknowledge there is no requirement to do this (and I will never know if you don't), if you create any new Descriptors, Events, or other functionality for your project - no matter how obvious or niche or major or minor - it would be a courtesy to push those additions back to the main brach via a pull request.

# JSON Documentation

## Names

Most objects are identified by a `name` field.

- This must be a string. (e.g. `"name": 42` is an error).
- This must not be empty (i.e. `"name": ""` is an error).
- This may only contain alphanumeric characters (i.e. `a`-`z`, `A` to `Z`, and `0` to `9`)
and the underscore (`_`) 	character (e.g. `"name": "joe-bloggs"` is an error, but `"name": joe_bloggs_42` is accepted).
(This restriction is that references can use special characters to do special functions. Check the section on references for examples.)
- Must be unique within their context. You cannot have two competitors with the same name in the same competition, nor two competition phases with the same name. In general, it is assumed any two objects with the same name represent the same thing.
- Names *are* case sensitive, so `"joe_bloggs"` and `"Joe_Bloggs"` are considered to be different.
- Names should not be the name you present to the user. If you do this you will create a localisation nightmare for yourself. `joe_bloggs_42` would likely appear as "Joe Bloggs" to the user, for example. Restricting spaces in the name field is one way to help prevent this, and it has some benefits when parsing too.


## References

Wherever something named (i.e., it has a `name` field) is referred to, it may also be referred to via a reference.

A reference is identified by its "ref" field which gets interpreted within the program. In its most basic form it is a list of Names, separated by dots (`.`), for example: if a field expecting a Competitor object gets `"championship.group_0.joe_bloggs"` would look for a Competition named `championship`, look in that for a competition named `group_0`, and look in that for a Competitor named `joe_bloggs`. If it cannot find one it will remain unresolved, and the output will contain the original reference instead of a competitor.

Fields:

- **Required:** `ref`: the reference text.

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

### `@HERE`

The `@HERE` token is replaced by a reference to the current location. Since references always start from the current location by default the references: `{ "ref": "@HERE.anytown_utd.joe_bloggs" }` and `{ "ref": "anytown_utd.joe_bloggs" }` are equivalent.

However, a `@HERE` token in the middle of a reference (e.g. `{ "ref": "anytown_utd.@HERE.joe_bloggs" }` it resets the parsing and anything before the `@HERE` token is discarded.

This token can either be used for clarity, or to simplify generators which generate references on the fly.

### `@ROOT`

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

### `@OUTER`

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

### `@ANY` and `@GLOB`

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

## Tags

Things with tags or attributes have a field named `tags`, which holds an array of tag objects. Tags are similar to names and generally follow the same rules. However, a tag may consist of multiple tokens, separated by `.` (e.g. `"part_1.part_2.part_3"`). Furthermore, there are some special tokens, identified by starting with a `$` symbol, which have special meanings, such as `$ANY` which will match with everything.

To differentiate a tag within a reference, a tag with `.`s in it must have `[square brackets]` around it. E.g.: `@ROOT.part_1.[MULTI.PART.TAG].part_2`.

A tag is an object with the fields:

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

### Tags and references

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

### Special Tag `$POS`

When a competition the finishing order of all the entries are set. Whichever competitor is placed first gets the tag `$POS:1`, 2nd gets the tag `$POS:2`, and so on. [Note to programmers: this is 1-indexed because actual league tables are also 1-indexed.]

If a group of competitors finished equal in a position they get a tag indicating the range of positions. So if four competitors finished equal third they would all get the tag `$POS:3:6`. This will match with references `$POS:3`, `$POS:4`, `$POS:5`, and `$POS:6`, or any references looking for a range of those positions (e.g. a reference to `$POS:6:8` will pick up all those competitors as well).

To set these tags use the `"set_finishing_order"` event.

Unlike other special tags, these tags will appear on the output.

This tag is unique. It cannot appear as part of a chain. So `example.$POS:1.oops` is an error, as is `$POS:1.example`.

### Special Tag `$ENTRY`

This corresponds to a competitor in a competition. Whichever competitor is listed first in the entry list implicity has the tag `$ENTRY:1`, and the numbering works exactly like the `$POS` numbering.

This tag is unique. It cannot appear as part of a chain. So `example.$ENTRY:1.oops` is an error, as is `$ENTRY:1.example`.

This tag is omitted from the output.

## World object

This is the unnamed very outer object that holds everything within it.

Fields:

- **Required:** `competition`: the Competition object to be worked with.
- *Optional:* `events`: an array of Event objects to apply to `competition`.
- [Planned for BETA] *Optional:* `competition_archetypes`: an array of competition archetypes you may wish to use.
- [Planned for BETA] *Optional:* `competitors`: an array of competitor achetypes you may wish to use.
- [Planned for release] *Optional:* `stats`: an array of stat types you may wish to use.
- [Planned for release] *Optional:* `resolvers`: an array of competition resolvers you may wish to use.
- [Planned for release] *Optional:* `random_seed`: a number between `0` and `18446744073709551615` used as the random seed for any structures that have random elements. Setting this will force "random" functionality to always produce the same output. This is generally used for reproducability. It it is not set on the input, and random elements are used, the random seed generated will be put on the output.

## Competition

This can stand for a phase of a bigger tournament, a single match/race/etc..., or the entire competition itself.

Fields:

- **Required:** `name`: the name of the Competition.
- *Optional*: `entry_list`: this is an ordered list of Competitors.
- *Optional*: `contents`: this is an ordered list of Competitions.
- *Optional*: `tags`: an array of tags.

A Competition **must** have one, or the other, or both of `entry_list` and `contents`. (It doesn't make much sense, otherwise.)

### Entry list

These are the Competitors taking place in this Competition. It is not mandatory as it is unnecessary for situations where the `contents` field describes what is happening: a single round in a knock-out competition, for example. Some situations, like a round-robin league, would have both an `entry_list` and `contents`. A Competition with no `entry_list` cannot be queried for who finished in what position.

### Contents

In a multi-phase competition each phase, or round, is probably its own entry here.

A competition with no `contents` field is probably a single match/game/race (or whatever else makes sense in your system). Alternatively it is a "view" - consider for example, the table of 3rd place teams at UEFA Euro 2024 where the best four teams who finished 3rd in their group advanced to the round-of-16.

## Competitor

This object describes a competitor into the competition.

Fields:

- **Required:** `name`: the name of the competitor.
- *Optional:* `tags`: an array of tag objects.
- *Optional:* `user_data`: arbitrary JSON which will be carried on this element as it gets copied.

## Descriptors

Descriptors are objects that may be used in place of a Competition object. They describe how to create phase of a competition and generate a Competition structure from it.

All descriptors have the following fields in addition to the ones specific to this descriptor:

- **Required:** `descriptor_type`: a string identifying the type of the descriptor. The parser uses this to know which type of descriptor it is building. This *is* case-sensitive. See the documentation on each descriptor type to see what values are valid here.
- **Required**: `name`: the name given to the resulting competition.
- *Optional:* `tags`: Tags applied to the competition once it is built.

All descriptors are identified by a field named `descriptor_type` which tells the parser which type of descriptor to create. 

### Round of Matches

This accepts an ordered array of Competitors called `entry_list`. It generates a Competition where each entry plays (at most) one match. The first team in the array plays the second; the third team plays the fourth; and so on. If there is a spare team at the end they are considered to have a bye.

This is not really intended to by used as a complete competition, but as an intermediate step or building block for larger competitions. Examples might be each round of a knock-out competition creating a Round of Matches, or each match-day in a league format.

Input Fields:

- `"descriptor_type": "round_of_matches"`.
- **Required:** `entry_list`: an array of Competitors. This also accepts JSON `null`s, which create byes.
- *Optional:* `swap_left_and_right`: a bool (default: `false`) which makes second vs first, fourth vs third, etc... instead of first vs second, third v fourth, etc... Mainly used for Round Robin formats.
- *Optional:* `generate_explicit_byes`: a bool default: `true`) which creates Competition events where one entry is a bye. Setting this to `false` will omit them. Any matches where both Competitors would be a Bye are omitted regardless of what this is set to. This may be useful if you wish to have your program display a list of matches including listings like "Joe Bloggs United - BYE".

Output:

A Competition with one match per team in the `contents` field, and an empty `entry_list`. The individual games are named `game_0`, `game_1`, and so-on.

Example input:

```json
{
	"descriptor_type": "round_of_matches",
	"name": "example_round_of_matches",
	"entry_list": [
		{ "name": "apples" },
		{ "name": "oranges" },
		null,
		{ "name": "cabbages" },
		{ "name": "lemons" },
		{ "name": "pears" },
		{ "name": "tomatoes" }
	],
	"swap_left_and_right": false,
	"generate_explicit_byes": true
}
```
	
would output:

```json
{
  "phases": [
	{
	  "entry_list": [
		{ "name": "oranges" },
		{ "name": "apples" }
	  ],
	  "name": "game_1"
	},
	{
	  "entry_list": [
		{ "name": "cabbages" },
		null
	  ],
	  "name": "game_2"
	},
	{
	  "entry_list": [
		{ "name": "pears" },
		{ "name": "lemons" }
	  ],
	  "name": "game_3"
	},
	{
	  "entry_list": [
		{ "name": "tomatoes" },
		null
	  ],
	  "name": "game_4"
	}
  ],
  "name": "example_round_of_matches"
}
```

### Round Robin

This generates a round-robin competition where each entry plays every other entry. Matches are organised into rounds, named `round_0`, `round_1`, and so-on, with each entry having one match per round, generated using the Round Of Matches descriptor. If there are an odd number of teams one team will get a bye in each round.

- `"descriptor_type": "round_robin"`.
- **Required:** `entry_list`: an array of Competitors. This does not accept `null` objects.
- *Optional:* `alternate_left_and_right`: a bool (default: `false`) which will try to get entries to alternate whether they are listed first or second on the outputs. This may be relevant for home-and-away formats (like football) or where this is first-turn advantage (like chess).
- *Optional:* `generate_byes`: a bool (default `false`) which tells the generator whether or not to produce a spare match vs a `null` opponent (i.e.: a bye) for the spare team each round if there are an odd number of rounds. If there are an even number of entries this does nothing.
- *Optional:* `num_times_to_play_each_opponent`: an integer of 1 or greater (default: `1`). If this is 1 each entry will play each other entry once. If it's 2, twice, etc... If not set it defaults to 1. When set greater than 1 the schedule will repeat (but with left and right inverted in each repeat if `alternate_left_and_right` is `true`).
- *Optional:* `shuffle_entries`: a bool (default `false`) which, if true, will cause the entry list to be shuffled, meaning opponents will appear in a random order. Note that this shuffle is *not* repeated between phases, so if `num_times_to_play_each_opponent` is set greater than `1` the schedule will still repeat after playing each opponent.

Example input:

```json
{
  "descriptor_type": "round_robin",
  "name": "example_round_robin",
  "entry_list": [
	{ "name": "apples" },
	{ "name": "oranges" },
	{ "name": "lemons" },
	{ "name": "pears" },
	{ "name": "melons" }
  ],
  "num_times_to_play_each_opponent": 2,
  "alternate_left_and_right": true,
  "generate_byes": false,
  "shuffle_entries": false
}
```
	
Produces output:

```json
{
  "phases": [
	{
	  "phases": [
		{
		  "entry_list": [
			{ "name": "lemons" },
			{ "name": "pears" }
		  ],
		  "name": "game_1"
		},
		{
		  "entry_list": [
			{ "name": "apples" },
			{ "name": "melons" }
		  ],
		  "name": "game_2"
		}
	  ],
	  "name": "round_1"
	},
	{
	  "phases": [
		{
		  "entry_list": [
			{ "name": "apples" },
			{ "name": "lemons" }
		  ],
		  "name": "game_1"
		},
		{
		  "entry_list": [
			{ "name": "pears" },
			{ "name": "oranges" }
		  ],
		  "name": "game_2"
		}
	  ],
	  "name": "round_2"
	},
	{
	  "phases": [
		{
		  "entry_list": [
			{ "name": "lemons" },
			{ "name": "melons" }
		  ],
		  "name": "game_1"
		},
		{
		  "entry_list": [
			{ "name": "pears" },
			{ "name": "apples" }
		  ],
		  "name": "game_2"
		}
	  ],
	  "name": "round_3"
	},
	{
	  "phases": [
		{
		  "entry_list": [
			{ "name": "pears" },
			{ "name": "oranges" }
		  ],
		  "name": "game_1"
		},
		{
		  "entry_list": [
			{ "name": "melons" },
			{ "name": "apples" }
		  ],
		  "name": "game_2"
		}
	  ],
	  "name": "round_4"
	},
	{
	  "phases": [
		{
		  "entry_list": [
			{ "name": "lemons" },
			{ "name": "oranges" }
		  ],
		  "name": "game_1"
		},
		{
		  "entry_list": [
			{ "name": "pears" },
			{ "name": "apples" }
		  ],
		  "name": "game_2"
		}
	  ],
	  "name": "round_5"
	},
	{
	  "phases": [
		{
		  "entry_list": [
			{ "name": "pears" },
			{ "name": "lemons" }
		  ],
		  "name": "game_1"
		},
		{
		  "entry_list": [
			{ "name": "melons" },
			{ "name": "apples" }
		  ],
		  "name": "game_2"
		}
	  ],
	  "name": "round_6"
	},
	{
	  "phases": [
		{
		  "entry_list": [
			{ "name": "lemons" },
			{ "name": "apples" }
		  ],
		  "name": "game_1"
		},
		{
		  "entry_list": [
			{ "name": "oranges" },
			{ "name": "pears" }
		  ],
		  "name": "game_2"
		}
	  ],
	  "name": "round_7"
	},
	{
	  "phases": [
		{
		  "entry_list": [
			{ "name": "melons" },
			{ "name": "lemons" }
		  ],
		  "name": "game_1"
		},
		{
		  "entry_list": [
			{ "name": "apples" },
			{ "name": "pears" }
		  ],
		  "name": "game_2"
		}
	  ],
	  "name": "round_8"
	},
	{
	  "phases": [
		{
		  "entry_list": [
			{ "name": "oranges" },
			{ "name": "pears" }
		  ],
		  "name": "game_1"
		},
		{
		  "entry_list": [
			{ "name": "apples" },
			{ "name": "melons" }
		  ],
		  "name": "game_2"
		}
	  ],
	  "name": "round_9"
	},
	{
	  "phases": [
		{
		  "entry_list": [
			{ "name": "oranges" },
			{ "name": "lemons" }
		  ],
		  "name": "game_1"
		},
		{
		  "entry_list": [
			{ "name": "apples" },
			{ "name": "pears" }
		  ],
		  "name": "game_2"
		}
	  ],
	  "name": "round_10"
	}
  ],
  "entry_list": [
	{ "name": "apples" },
	{ "name": "oranges" },
	{ "name": "lemons" },
	{ "name": "pears" },
	{ "name": "melons" }
  ],
  "name": "example_round_robin"
}
```

### Knockout-bracket

- `descriptor_type`: `"knockout_bracket".
- **Required:** `entry_list`: an array of Competitors. Use `null` objects to create byes. This must have a power-of-two number of entries, including the byes.

In the first round Entry 1 will play Entry 2, entry 3 will play entry 4, and so on. Then in the second round the winner of 1&2 will play the winner of 3&4, and so on. Rounds are generated until there is a winner.

Here's an example with byes:

```json
{
  "competition": {
	"descriptor_type": "knockout_bracket",
	"name": "bracket_with_byes",
	"comment": "An animal show-down. Naturally lions and tigers get a bye to round 2, and penguins to round 3.",
	"entry_list": [
	  { "name": "penguin" },
	  null,
	  null,
	  null,
	  { "name": "goldfish" },
	  { "name": "sparrow" },
	  { "name": "cow" },
	  { "name": "snake" },
	  { "name": "lion" },
	  null,
	  { "name": "pigeon" },
	  { "name": "dog" },
	  { "name": "tiger" },
	  null,
	  { "name": "rat" },
	  { "name": "cat" }
	]
  }
}
```

In this animal show-down lions and tigers get a bye to round 2, and penguins to round 3. The output looks like this:

```json
{
  "competition": {
	"name": "bracket_with_byes",
	"phases": [
	  {
		"name": "round_1",
		"phases": [
		  {
			"entry_list": [
			  { "name": "goldfish" },
			  { "name": "sparrow" }
			],
			"name": "game_1"
		  },
		  {
			"entry_list": [
			  { "name": "cow" },
			  { "name": "snake" }
			],
			"name": "game_2"
		  },
		  {
			"entry_list": [
			  { "name": "pigeon" },
			  { "name": "dog" }
			],
			"name": "game_3"
		  },
		  {
			"entry_list": [
			  { "name": "rat" },
			  { "name": "cat" }
			],
			"name": "game_4"
		  }
		]
	  },
	  {
		"name": "round_2",
		"phases": [
		  {
			"entry_list": [ "@OUTER.@OUTER.round_1.game_1.$POS:1", "@OUTER.@OUTER.round_1.game_2.$POS:1" ],
			"name": "game_1"
		  },
		  {
			"entry_list": [
			  { "name": "lion" },
			  "@OUTER.@OUTER.round_1.game_3.$POS:1"
			],
			"name": "game_2"
		  },
		  {
			"entry_list": [
			  { "name": "tiger" },
			  "@OUTER.@OUTER.round_1.game_4.$POS:1"
			],
			"name": "game_3"
		  }
		]
	  },
	  {
		"name": "round_3",
		"phases": [
		  {
			"entry_list": [
			  { "name": "penguin" },
			  "@OUTER.@OUTER.round_2.game_1.$POS:1"
			],
			"name": "game_1"
		  },
		  {
			"entry_list": [ "@OUTER.@OUTER.round_2.game_2.$POS:1", "@OUTER.@OUTER.round_2.game_3.$POS:1" ],
			"name": "game_2"
		  }
		]
	  },
	  {
		"name": "round_4",
		"phases": [
		  {
			"entry_list": [ "@OUTER.@OUTER.round_3.game_1.$POS:1", "@OUTER.@OUTER.round_3.game_2.$POS:1" ],
			"name": "game_1"
		  }
		]
	  }
	]
  }
}
```

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
