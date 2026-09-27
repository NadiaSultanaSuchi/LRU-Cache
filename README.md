# LRU Cache (C++)

## Data structures used and why
- `list<tuple<string, int, time_point, bool>>` — keeps key-value pairs in order, along with each item's expiry info. Front = most recently used, back = least recently used. A list is used because inserting/removing items from any position is O(1).
- `unordered_map<string, list<...>::iterator>` — maps each key directly to its position in the list, so lookups are instant instead of scanning the whole list.

## How LRU ordering is maintained
On every `get` or `put`, the accessed item is removed from its current position and pushed to the front of the list, marking it as most recently used. When capacity is exceeded, the item at the back of the list (least recently used) is removed.

## Time complexity
- `get`: O(1) average
- `put`: O(1) average

## Space complexity
O(capacity) — both the list and map hold at most `capacity` items.

## How to run
g++ -std=c++17 main.cpp -o test -pthread
./test

## Bonus: TTL/expiration support
`put(key, value, ttlSeconds)` lets a key expire after a set number of seconds. Expiration is lazy — checked only when `get()` is called, rather than using a background thread. This keeps the design simple with no extra overhead, but as a trade-off, an expired key can still sit in memory until it's next accessed.
