# Examples

These folders contain various examples to demonstrate how this works, use as a starting point / reference for your own usage, and as unit tests.

The `/in` folder contains inputs which produce a matching file in `/out`. The testing mode will check that these two files actually match.

To understand what the inputs mean, refer to [the input and output JSON documentation](../documentation/json/readme.md).

## Current examples

- `DEBUG_never_resolves.json`: This is a test-case for [descriptors](../documentation/json/descriptors/readme.md) that do not resolve to a [competition](../documentation/json/competition.md).
- `add_tags.json`: Demonstrates how the [add_tags event](../documentation/json/events/add_tags.md) works.
- `complex_elim.json`: Demonstrates a complicated [knockout bracket](../documentation/json/descriptors/knockout_bracket.md) works, including byes.
- `round_of_matches.json`: Demonstrates how the [round_of_matches](../documentation/json/descriptors/round_of_matches.md) works.
- `set_finishing_positions.json`: Demonstrates how the [set finishing positions event](../documentation/json/events/set_finishing_positions.md) works.
- `simple_elim.json`: Demonstrates how the [round_of_matches](../documentation/json/descriptors/round_of_matches.md) works with a simple example.
- `test_references.json`: Tests various more unusual cases for [references](../documentation/json/references.md).
- `test_multireferences.json`: Test more cases for references, particularly cases where they may resolve to more than one element.
- `test_tags.json`: Test cases for [tags](../documentation/json/tags.md) and using tags to match with references.
