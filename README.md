# aoc2020
Advent Of Code 2020

## Implementation

C++11 with the occasional C++17 trivial feature like initialized class
members using vectors in the class definition.

## Learnings

Here I'm listing anything I discover along the way that would ideally
result in refactors or simplifications in previous solutions.

- `std::ifstream` has a `getline` function. I've used `get` with individual
  characters for the first few days.
- Need to check that I've used `match[1].str()` everywhere necessary. Otherwise
  strange things appear to happen.
- Need to check whether input stream is open with `ifs.fail()` as the cwd when
  run from within VSCode seems to be the workspace rather than the current file
  directory.
- I've been extremely inconsistent in naming, in particular starting with a capital
  or not. Should figure out my linting story.
- When deleting values from a data structure we're iterating through, do the
  dance of moving the iterator properly over the deleted value and onto the next
  one rather than breaking out of the loop and starting iteration again.
