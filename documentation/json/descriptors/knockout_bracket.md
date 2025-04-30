- [Back to top](../../../readme.md)
  - [Documentation home](../../readme.md)
    - [Input and output JSON](../readme.md)
      - [Competition Descriptor object](readme.md)
        - Knockout bracket
       
# Knockout-bracket

This takes an entry list and produces a single-elimination knockout bracket.

## Fields

- `descriptor_type`: `"knockout_bracket"`.
- **Required:** `entry_list`: an array of [competitors](../competitor.md). Use `null` objects to create byes. This must have a power-of-two number of entries, including the byes.

In the first round Entry 1 will play Entry 2, entry 3 will play entry 4, and so on. Then in the second round the winner of 1&2 will play the winner of 3&4, and so on. Rounds are generated until there is a winner.

## Input

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

## Output

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
