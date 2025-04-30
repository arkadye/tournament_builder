### Navigation

- [Back to top](../../readme.md)
  - [Documentation home](../readme.md)
    - [Input and output JSON](readme.md)
      - Error messages

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
