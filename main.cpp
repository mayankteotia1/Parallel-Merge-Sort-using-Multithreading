#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

#include "mergeSort.hpp"
#include "parallelMergeSort.hpp"

int main() {
    const int SIZE = 10000000;
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    std::vector<int> nums(SIZE);
    for (int i = 0; i < SIZE; ++i) {
        nums[i] = std::rand() % 10000000;
    }
    // Identical copy so both algorithms sort the exact same input.
    std::vector<int> nums1 = nums;

    MergeSort mergeSort(&nums);
    auto start = std::chrono::high_resolution_clock::now();
    mergeSort.sort();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> mergeSortDuration = end - start;
    std::cout << "MergeSort time taken:         " << mergeSortDuration.count() << " seconds\n";

    ParallelMergeSort parallelMergeSort(&nums1);
    start = std::chrono::high_resolution_clock::now();
    parallelMergeSort.sort();
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> parallelMergeSortDuration = end - start;
    std::cout << "ParallelMergeSort time taken: " << parallelMergeSortDuration.count() << " seconds\n";

    std::cout << "Speedup: " << (mergeSortDuration.count() / parallelMergeSortDuration.count()) << "x\n";

    bool bothSorted = std::is_sorted(nums.begin(), nums.end()) && std::is_sorted(nums1.begin(), nums1.end());
    bool resultsMatch = (nums == nums1);
    std::cout << "Correctness check: " << ((bothSorted && resultsMatch) ? "PASSED" : "FAILED") << "\n";

    return 0;
}
