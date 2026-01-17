### Navigation

___NAV-MAP___
       
# Competitor View

This takes an entry list with a variable size and displays it. This is used for situations where there are no sub-phases, and is also very useful for test cases.

## Input

- `"descriptor_type": "competitor_view"`.
- **Required:** `entry_list`: an array of Competitors, typically references. References may refer to multiple competitors at once.
- *Optional:* `expected_num_entries`: how many entries are expected. This may not be used in conjunction with `min_entries` or `max_entries`.
- *Optional:* `min_entries`: the minimum number of entries expected. This may not be used in conjunction with `expected_num_entries`.
- *Optional:* `max_entries`: the maximum number of entries expected. This may not be used in conjunction with `expected_num_entries`.

### Default values

If none of `expected_num_entries`, `min_entries`, or `max_entries` are set, then `expected_num_entries` is assumed to be the number of elements in `entry_list`.

Setting `expected_num_entries` is equivalent to setting `min_entries` and `max_entries` to the same number.

If `min_entries` is set and `max_entries` is not, `max_entries` is defaulted to 2147483647.

If `max_entries` is set and `min_entries` is not, `min_entries` is defaulted to 1.

### Resolving multiple entries

When generating, if any reference is unresolved this will not generate.

If the same competitor is found twice, it will only appear in the output list once.