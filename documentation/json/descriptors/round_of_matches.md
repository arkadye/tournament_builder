### Navigation

- [Tournament Maker](../../../README.md)
  - [Tournament Builder Documentation](../../readme.md)
    - [Input and Output JSON](../readme.md)
      - [Competition](../competition.md)
      - [Competition Descriptors](readme.md)
        - [Competitor View](competitor_view.md)
        - [DEBUG: Never resolves](DEBUG_never_resolves.md)
        - [Knockout-bracket](knockout_bracket.md)
        - [Round Robin](round_robin.md)
        - **Round of Matches** (You are here)
      - [Competitor](../competitor.md)
      - [Entry List](../entry_list.md)
      - [Error messages](../error_messages.md)
      - [Events](../events/readme.md)
      - [Names](../naming_rules.md)
      - [References](../references.md)
      - [Tags](../tags.md)
      - [World object](../world.md)

# Round of Matches

This descriptor accepts an ordered array of [competitors](../competitor.md) called `entry_list`. It generates a [competition](../competition.md) where each entry plays (at most) one match. The first team in the array plays the second; the third team plays the fourth; and so on. If there is a spare team at the end they are considered to have a bye.

This is not really intended to by used as a complete competition, but as an intermediate step or building block for larger competitions. Examples might be each round of a knock-out competition creating a Round of Matches, or each match-day in a league format, and these then get resolved to build the overall competition.

## Input fields

- `"descriptor_type": "round_of_matches"`.
- **Required:** `entry_list`: an array of Competitors. This also accepts JSON `null`s, which create byes.
- *Optional:* `swap_left_and_right`: a bool (default: `false`) which makes second vs first, fourth vs third, etc... instead of first vs second, third v fourth, etc... Mainly used for Round Robin formats.
- *Optional:* `generate_explicit_byes`: a bool (default: `true`) which creates Competition events where one entry is a bye. Setting this to `false` will omit them. Any matches where both Competitors would be a Bye are omitted regardless of what this is set to. This may be useful if you wish to have your program display a list of matches including listings like "Joe Bloggs United - BYE".

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

## Output

A Competition with one match per team in the `contents` field, and an empty `entry_list`. The individual games are named `game_0`, `game_1`, and so-on.

Example input:
	
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
