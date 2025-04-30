This is of the JSON object specifications. [See here for an overview](../readme.md)

## Names

Many objects are identified by a `name` field. The name itself is a string, with some rules.

- This must be a string. (e.g. `"name": 42` is an error).
- This must not be empty (i.e. `"name": ""` is an error).
- This may only contain alphanumeric characters (i.e. `a`-`z`, `A` to `Z`, and `0` to `9`)
and the underscore (`_`) 	character (e.g. `"name": "joe-bloggs"` is an error, but `"name": joe_bloggs_42` is accepted).
(This restriction is that references can use special characters to do special functions. Check the section on references for examples.)
- Must be unique within their context. You cannot have two competitors with the same name in the same competition, nor two competition phases with the same name. In general, it is assumed any two objects with the same name represent the same thing.
- Names *are* case sensitive, so `"joe_bloggs"` and `"Joe_Bloggs"` are considered to be different.
- Names should not be the name you present to the user. If you do this you will create a localisation nightmare for yourself. `joe_bloggs_42` would likely appear as "Joe Bloggs" to the user, for example. Restricting spaces in the name field is one way to help prevent this, and it has some benefits when parsing too.

A simple named object may look like this:

```json
{ "name": "joe_bloggs_42" }
```

Names are used to identify: [Competitions](competition.md), [Competition Descriptors](descriptors/readme.md), and [Competitors](competitor.md).

[References](references.md) use these names (as well as the related [Tags](tags.md)) to search through the overall structures and find what they reference.
