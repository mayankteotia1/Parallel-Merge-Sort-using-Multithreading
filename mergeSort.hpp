#ifndef MERGESORT_H
#define MERGESORT_H

#include <vector>

// Classic single-threaded merge sort, used as the baseline for benchmarking
// ParallelMergeSort. Bottoms out at std::sort() below SEQUENTIAL_THRESHOLD so
// that the only difference between this and the parallel version is the
// threading itself -- not the underlying algorithm.
class MergeSort {
private:
    std::vector<int> *nums;
    std::vector<int> buffer; // scratch space, allocated once instead of per-merge

    void merge(int left, int mid, int right);

public:
    explicit MergeSort(std::vector<int> *nums);
    ~MergeSort();

    void sort();
    void recursiveSort(int left, int right);
};

#endif
