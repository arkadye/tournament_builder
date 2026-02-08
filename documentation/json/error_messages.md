### Navigation

- [Tournament Maker](../../README.md)
  - [Tournament Builder Documentation](../readme.md)
    - [Input and Output JSON](readme.md)
      - [Competition](competition.md)
      - [Competition Descriptors](descriptors/readme.md)
      - [Competitor](competitor.md)
      - [Entry List](entry_list.md)
      - **Error messages** (You are here)
      - [Events](events/readme.md)
      - [Names](naming_rules.md)
      - [References](references.md)
      - [Tags](tags.md)
      - [World object](world.md)

# Error messages

This is an object which gets returned instead of a [world object](world.md) if an error occurs.

It cannot be used as an input.

Presently, the first error will cause processing to be abandoned and a single error message immediately returned, although that may change.

## Fields:

- **Errors:** An array of strings. Each element is a string with an error message. This message should give enough information to work out what went wrong and fix it.

  Example:

  ```json
  {
     "errors:" [ "Oh no, somthing went wrong." , "And something else." ] 
  }
  ```
