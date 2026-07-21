# Parallel Merge Sort

A C++17 merge sort with a multithreaded variant that parallelizes both the **divide** step and the **merge** step — most parallel merge sort implementations only parallelize the divide step and merge sequentially, which caps the achievable speedup since merging is O(n) work at every level of recursion.

## Features

- **Parallel merge**: two sorted ranges are merged concurrently using binary-search partitioning, not just parallel splitting.
- **Bounded thread spawning**: thread creation is capped by a depth limit from `std::thread::hardware_concurrency()`, instead of spawning threads at every recursion level (which would create thousands of OS threads on large inputs).
- **Fair baseline**: the sequential and parallel versions share the same base case (`std::sort` below a threshold) and the same buffer-reuse strategy, so the only variable in the benchmark is threading.

## Design

| | Sequential (`MergeSort`) | Parallel (`ParallelMergeSort`) |
|---|---|---|
| Divide | Recursive, single-threaded | Recursive, split across threads (depth-capped) |
| Merge | Sequential, reused scratch buffer | Binary-search partition, split across threads (depth-capped) |
| Base case | `std::sort` below threshold | `std::sort` below threshold |

Note: thread creation here is depth-limited spawning, not a persistent thread pool — worker threads aren't reused across tasks.

## Correctness

Verified against `std::sort` with 3,000 randomized trials (small arrays, heavy duplicates, lowered threshold to force the parallel-merge path) plus edge cases: empty, single-element, all-duplicates, sorted, reverse-sorted, exact-threshold-size.

## Build & Run

```bash
git clone https://github.com/mayankteotia1/Parallel-Merge-Sort-using-Multithreading.git
cd Parallel-Merge-Sort-using-Multithreading
g++ -std=c++17 -O2 -pthread main.cpp mergeSort.cpp parallelMergeSort.cpp -o parallel_sort
./parallel_sort
```

Or with CMake:

```bash
cmake -B build -S .
cmake --build build
./build/parallel_sort
```

`main.cpp` sorts 10M random integers with both implementations and prints timing, speedup, and a correctness check. Speedup depends on core count — report it alongside any number you cite (e.g. "2.8x on 4 cores").

## Possible extensions

- Real thread pool instead of depth-limited spawning
- Per-phase threshold tuning (divide vs. merge)
- Generic template over iterators/comparators instead of `std::vector<int>`
