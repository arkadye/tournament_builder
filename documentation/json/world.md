### Navigation

- [Back to top](../../readme.md)
  - [Documentation home](../readme.md)
    - [Input and output JSON](readme.md)
      - World object

# World object

This is the unnamed very outer object that holds everything within it. One of these is taken as the input and a modified one returned as the output.

For examples of what a world object looks like, [you may refer to the examples folder](../../examples/readme.md).

## Fields:

- **Required:** `competition`: the [Competition](competition.md) object to be worked with.
- *Optional:* `events`: an array of [Event objects](events/readme.md) to apply to `competition`.

Additionally, planned for Beta:

- [Planned for BETA] *Optional:* `competition_archetypes`: an array of competition archetypes you may wish to use.
- [Planned for BETA] *Optional:* `competitors`: an array of competitor achetypes you may wish to use.

Additionally, planed for release.

- [Planned for release] *Optional:* `stats`: an array of stat types you may wish to use.
- [Planned for release] *Optional:* `resolvers`: an array of competition resolvers you may wish to use. Resolvers may be used to tell a Competition how to interact with stats.
- [Planned for release] *Optional:* `random_seed`: a number between `0` and `18446744073709551615` used as the random seed for any structures that have random elements. Setting this will force "random" functionality to always produce the same output. This is generally used for reproducability. It it is not set on the input, and random elements are used, the random seed generated will be put on the output.
