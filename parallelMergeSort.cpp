#include "parallelMergeSort.hpp"
#include <algorithm>
#include <cmath>

namespace {
    constexpr int SEQUENTIAL_THRESHOLD = 5000;
}

ParallelMergeSort::ParallelMergeSort(std::vector<int> *nums)
    : nums(nums), buffer(nums->size())
{
    maxThreads = std::max(1u, std::thread::hardware_concurrency());
}

ParallelMergeSort::~ParallelMergeSort() {}

void ParallelMergeSort::sequentialMerge(int left, int mid, int right) {
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {
        buffer[k++] = ((*nums)[i] <= (*nums)[j]) ? (*nums)[i++] : (*nums)[j++];
    }
    while (i <= mid) {
        buffer[k++] = (*nums)[i++];
    }
    while (j <= right) {
        buffer[k++] = (*nums)[j++];
    }

    for (int x = left; x <= right; x++) {
        (*nums)[x] = buffer[x];
    }
}

// Classic parallel-merge partition: pick the midpoint of the larger range,
// binary-search its value in the smaller range, then recurse on the two
// resulting (independent, order-preserving) halves in parallel.
void ParallelMergeSort::parallelMerge(int l1, int r1, int l2, int r2, int outStart, unsigned int depth) {
    int n1 = r1 - l1 + 1;
    int n2 = r2 - l2 + 1;

    if (n1 <= 0 && n2 <= 0) {
        return;
    }
    if (n1 <= 0) {
        std::copy(nums->begin() + l2, nums->begin() + r2 + 1, buffer.begin() + outStart);
        return;
    }
    if (n2 <= 0) {
        std::copy(nums->begin() + l1, nums->begin() + r1 + 1, buffer.begin() + outStart);
        return;
    }

    if (depth == 0 || (n1 + n2) < SEQUENTIAL_THRESHOLD) {
        // Reuse the plain merge, but it writes/reads via `nums`, so stage
        // this sub-range through nums first when it isn't already contiguous
        // starting at `left == l1`. In practice l1 <= l2 always here, and
        // outStart == l1 for the outermost call, so a direct in-place merge
        // works; for nested calls we merge into buffer directly instead.
        int i = l1, j = l2, k = outStart;
        while (i <= r1 && j <= r2) {
            buffer[k++] = ((*nums)[i] <= (*nums)[j]) ? (*nums)[i++] : (*nums)[j++];
        }
        while (i <= r1) buffer[k++] = (*nums)[i++];
        while (j <= r2) buffer[k++] = (*nums)[j++];
        return;
    }

    // Always split the larger of the two ranges so recursion depth stays
    // logarithmic regardless of how skewed the input is.
    if (n1 < n2) {
        parallelMerge(l2, r2, l1, r1, outStart, depth);
        return;
    }

    int mid1 = l1 + n1 / 2;
    int splitValue = (*nums)[mid1];

    // First index in [l2, r2] whose value is strictly greater than splitValue.
    int mid2 = static_cast<int>(
        std::upper_bound(nums->begin() + l2, nums->begin() + r2 + 1, splitValue) - nums->begin()
    );

    int leftCount = (mid1 - l1) + (mid2 - l2);

    std::thread worker([this, l1, mid1, l2, mid2, outStart, depth] {
        parallelMerge(l1, mid1 - 1, l2, mid2 - 1, outStart, depth - 1);
    });
    parallelMerge(mid1, r1, mid2, r2, outStart + leftCount, depth - 1);
    worker.join();
}

void ParallelMergeSort::sequentialSort(int left, int right) {
    if (left >= right) {
        return;
    }
    if (right - left < SEQUENTIAL_THRESHOLD) {
        std::sort(nums->begin() + left, nums->begin() + right + 1);
        return;
    }

    int mid = left + (right - left) / 2;
    sequentialSort(left, mid);
    sequentialSort(mid + 1, right);
    sequentialMerge(left, mid, right);
}

void ParallelMergeSort::parallelSort(int left, int right, unsigned int depth) {
    if (left >= right) {
        return;
    }

    if (depth == 0 || right - left < SEQUENTIAL_THRESHOLD) {
        sequentialSort(left, right);
        return;
    }

    int mid = left + (right - left) / 2;

    std::thread worker([this, left, mid, depth] {
        parallelSort(left, mid, depth - 1);
    });
    parallelSort(mid + 1, right, depth - 1);
    worker.join();

    // Merge the two now-sorted halves, in parallel when the merge itself is
    // big enough to be worth splitting.
    parallelMerge(left, mid, mid + 1, right, left, depth);
    std::copy(buffer.begin() + left, buffer.begin() + right + 1, nums->begin() + left);
}

void ParallelMergeSort::sort() {
    if (nums->empty()) {
        return;
    }

    unsigned int depth = static_cast<unsigned int>(std::log2(maxThreads)) + 1;
    parallelSort(0, static_cast<int>(nums->size()) - 1, depth);
}
