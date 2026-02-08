### Navigation

- [Tournament Maker](../../../README.md)
  - [Tournament Builder Documentation](../../readme.md)
    - [Input and Output JSON](../readme.md)
      - [Competition](../competition.md)
      - [Competition Descriptors](readme.md)
        - [Competitor View](competitor_view.md)
        - [DEBUG: Never resolves](DEBUG_never_resolves.md)
        - [Knockout-bracket](knockout_bracket.md)
        - **Round Robin** (You are here)
        - [Round of Matches](round_of_matches.md)
      - [Competitor](../competitor.md)
      - [Entry List](../entry_list.md)
      - [Error messages](../error_messages.md)
      - [Events](../events/readme.md)
      - [Names](../naming_rules.md)
      - [References](../references.md)
      - [Tags](../tags.md)
      - [World object](../world.md)
       
# Round Robin

This generates a round-robin competition where each entry plays every other entry. Matches are organised into rounds, named `round_0`, `round_1`, and so-on, with each entry having one match per round, generated using the Round Of Matches descriptor. If there are an odd number of teams one team will get a bye in each round.

## Input

- `"descriptor_type": "round_robin"`.
- **Required:** `entry_list`: an array of Competitors. This does not accept `null` objects.
- *Optional:* `alternate_left_and_right`: a bool (default: `false`) which will try to get entries to alternate whether they are listed first or second on the outputs. This may be relevant for home-and-away formats (like football) or where this is first-turn advantage (like chess).
- *Optional:* `generate_byes`: a bool (default `false`) which tells the generator whether or not to produce a spare match vs a `null` opponent (i.e.: a bye) for the spare team each round if there are an odd number of entries. If there are an even number of entries this does nothing.
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

## Output

The example input produces the output:

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
