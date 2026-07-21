#ifndef PARALLELMERGESORT_H
#define PARALLELMERGESORT_H

#include <vector>
#include <thread>

// Parallel merge sort. Both the divide step (sorting the two halves) and the
// combine step (merging them) run in parallel, each capped by a depth budget
// derived from std::thread::hardware_concurrency() so the number of OS
// threads created stays roughly bounded instead of growing exponentially
// with the input size.
class ParallelMergeSort {
private:
    std::vector<int> *nums;
    std::vector<int> buffer; // scratch space, allocated once instead of per-merge
    unsigned int maxThreads;

    void sequentialSort(int left, int right);
    void parallelSort(int left, int right, unsigned int depth);

    void sequentialMerge(int left, int mid, int right);
    // Merges nums[l1..r1] and nums[l2..r2] (either range may be empty) into
    // buffer starting at outStart, splitting the work across threads via a
    // binary-search partition when both ranges are large enough.
    void parallelMerge(int l1, int r1, int l2, int r2, int outStart, unsigned int depth);

public:
    explicit ParallelMergeSort(std::vector<int> *nums);
    ~ParallelMergeSort();

    void sort();
};

#endif
