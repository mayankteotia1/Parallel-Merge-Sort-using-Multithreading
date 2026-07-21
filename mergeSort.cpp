#include "mergeSort.hpp"
#include <algorithm>

namespace {
    constexpr int SEQUENTIAL_THRESHOLD = 5000;
}

MergeSort::MergeSort(std::vector<int> *nums)
    : nums(nums), buffer(nums->size())
{
}

MergeSort::~MergeSort() {}

void MergeSort::merge(int left, int mid, int right) {
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

void MergeSort::recursiveSort(int left, int right) {
    if (left >= right) {
        return;
    }

    if (right - left < SEQUENTIAL_THRESHOLD) {
        std::sort(nums->begin() + left, nums->begin() + right + 1);
        return;
    }

    int mid = left + (right - left) / 2;

    recursiveSort(left, mid);
    recursiveSort(mid + 1, right);
    merge(left, mid, right);
}

void MergeSort::sort() {
    if (nums->empty()) {
        return;
    }
    recursiveSort(0, static_cast<int>(nums->size()) - 1);
}
