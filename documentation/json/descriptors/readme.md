### Navigation

- [Back to top](../../../readme.md)
  - [Documentation home](../../readme.md)
    - [Input and output JSON](../readme.md)
      - [Competition Descriptor object](readme.md)
        - [Round of matches](round_of_matches.md)
        - [Round-robin](round_robin.md)
        - [Simple elimination](simple_elimination.md)

# Competition Descriptors

Descriptors are objects that may be used in place of a Competition object. They describe how to create phase of a competition and generate a Competition structure from it.

## Fields

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
