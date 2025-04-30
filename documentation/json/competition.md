### Navigation

- [Back to top](../../readme.md)
  - [Documentation home](../readme.md)
    - [Input and output JSON](readme.md)
      - Competition object

# Competition

This can stand for a phase of a bigger tournament, a single match/race/etc..., or the entire competition itself.

Generally, anything can use a competition object can also have a [competition descriptor](descriptors/readme.md) given to it instead. Descriptors are known competition structures that represent much more complex competitions.

## Fields:

- **Required:** `name`: the [name](naming_rules.md) of the Competition.
- *Optional*: `entry_list`: this is an ordered list of [competitors](competitor.md) or [references to competitors](references.md).
- *Optional*: `phases`: this is an ordered list of competitions or [competition descriptors](descriptors/readme.md).
- *Optional*: `tags`: an array of [tags](tags.md).

A Competition **must** have one, or the other, or both of `entry_list` and `contents`. (It doesn't make much sense, otherwise.)

### `entry_list`

These are the Competitors taking place in this Competition. It is not mandatory as it is unnecessary for situations where the `contents` field describes what is happening: a single round in a knock-out competition, for example. Some situations, like a round-robin league, would have both an `entry_list` and `contents`. A Competition with no `entry_list` cannot be queried for who finished in what position, nor have its finishing order set.

### `phases`

In a multi-phase competition each phase, or round, is probably its own entry here.

A competition with no `phases` field is probably a single match/game/race (or whatever else makes sense in your system). Alternatively it is a "view" - consider for example, the table of 3rd place teams at UEFA Euro 2024 where the best four teams who finished 3rd in their group advanced to the round-of-16.
