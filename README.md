# Parallel Merge Sort

A C++17 implementation of merge sort with a multithreaded variant, built to
measure and demonstrate the actual performance gain from parallelizing both
the **divide** and **merge** steps of the algorithm — not just the divide
step, which is the more common (and easier) place to stop.

## Why this project exists

Most "parallel merge sort" examples only parallelize the recursive splitting
and still merge sequentially, which caps the achievable speedup since merging
is O(n) work done at every level of the recursion. This implementation also
parallelizes the merge itself, using the classic binary-search partitioning
technique: to merge two sorted ranges in parallel, split the larger range at
its midpoint, binary-search that value's position in the smaller range, and
merge the two resulting (independent, order-preserving) halves concurrently.

## Design

- **`MergeSort`** — a single-threaded baseline. Recurses down to
  `SEQUENTIAL_THRESHOLD` elements, then finishes with `std::sort`, and reuses
  one scratch buffer across all merges instead of allocating on every call.
- **`ParallelMergeSort`** — same base case and same scratch-buffer strategy
  as `MergeSort`, so the only variable between the two benchmarks is
  threading, not algorithmic differences. It parallelizes:
  - the **divide/recursion** (sorting the two halves on separate threads), and
  - the **merge** itself (splitting the merge into two independent halves
    via binary search and merging each on a separate thread).
- **Thread budget** — thread creation is capped by a depth limit derived
  from `std::thread::hardware_concurrency()`, rather than spawning two new
  threads at every level of recursion all the way down to the threshold
  (which, for 10M elements, would create thousands of OS threads and make
  the parallel version *slower* than the sequential one).

This is a from-scratch depth-limited spawn strategy, not a persistent thread
pool — worth knowing if you're asked about it, since "thread pool" implies
reused worker threads across tasks, which this does not do.

## Correctness

Verified with:
- 3,000 randomized trials (small arrays, 0–400 elements, heavy duplicate
  values) against `std::sort` as ground truth, with the threshold lowered to
  force the parallel-merge partition logic to run.
- Edge cases: empty input, single element, all-duplicate arrays, already
  sorted, reverse sorted, and arrays exactly at the threshold boundary.

## Building

```bash
git clone <repo-url>
cd parallelMergeSort
cmake -B build -S .
cmake --build build
./build/parallel_sort
```

Or without CMake:

```bash
g++ -std=c++17 -O2 -pthread main.cpp mergeSort.cpp parallelMergeSort.cpp -o parallel_sort
./parallel_sort
```

## Benchmarking notes

The included `main.cpp` sorts 10,000,000 random integers with both
implementations on identical input and prints the measured speedup, plus a
correctness check comparing both results against each other and against
`std::is_sorted`. Actual speedup depends heavily on the number of physical
cores available — report the machine and core count alongside any numbers
you cite (e.g., "N.Nx on an M-core / N-thread machine"), rather than a single
unqualified figure.

## Possible extensions

- Replace the depth-limited spawn strategy with a real thread pool that
  reuses worker threads across the divide and merge phases, avoiding
  repeated thread creation/destruction overhead entirely.
- Tune `SEQUENTIAL_THRESHOLD` per phase (divide vs. merge) rather than
  sharing one constant, since the optimal cutover point differs between them.
- Extend to a generic, comparator- and iterator-based template rather than
  a hardcoded `std::vector<int>`.
