### Navigation

- [Back to top](../../../readme.md)
  - [Documentation home](..././readme.md)
    - [Input and output JSON](../readme.md)
      - [Events](readme.md)
        - Add tags
       
# Add tags

`"event_type": "add_tags"`

Adds [tags](../tags.md) to taggable objects.

## Fields:

- **Required:** `target`: A reference, or array of references, to one or more [competitor objects](../competitor.md) or [competition objects](../competition.md) to have tags added to them.
- **Required:** `tags`: A tag, or array of tags, which will be added to all the competitors references.

A tag will only be added to a competitor once. If any attempt is made to add a duplicate tag to an element it silently fails. For special tags this includes ignoring the argument. If the target already has a tag `$POS:2` then adding `$POS:3` will do nothing.

On each target this will attempt to resolve references before setting tags. If a target is an unresolved reference that target does not have tags set.
