#include "max_subarray.h"
#include <limits.h>

/**
 * @brief Find the maximum subarray crossing the midpoint mid.
 * Runs in O(high - low + 1) = O(len) time.
 */
static SubarrayResult max_crossing_subarray(const int* arr, int low, int mid, int high) {
    SubarrayResult res;
    res.left_idx = mid;
    res.right_idx = mid + 1;
    res.sum = 0;

    int left_sum = INT_MIN;
    int current_sum = 0;
    int max_left = mid;

    for (int i = mid; i >= low; i--) {
        current_sum += arr[i];
        if (current_sum > left_sum) {
            left_sum = current_sum;
            max_left = i;
        }
    }

    int right_sum = INT_MIN;
    current_sum = 0;
    int max_right = mid + 1;

    for (int j = mid + 1; j <= high; j++) {
        current_sum += arr[j];
        if (current_sum > right_sum) {
            right_sum = current_sum;
            max_right = j;
        }
    }

    res.left_idx = max_left;
    res.right_idx = max_right;
    res.sum = left_sum + right_sum;
    return res;
}

SubarrayResult max_subarray_dc(const int* arr, int low, int high) {
    SubarrayResult invalid_res = {-1, -1, 0};
    if (!arr || low > high) {
        return invalid_res;
    }

    /* Base case: single element */
    if (low == high) {
        SubarrayResult base_res;
        base_res.left_idx = low;
        base_res.right_idx = high;
        base_res.sum = arr[low];
        return base_res;
    }

    int mid = low + (high - low) / 2;

    /* Conquer left and right subproblems */
    SubarrayResult left_res = max_subarray_dc(arr, low, mid);
    SubarrayResult right_res = max_subarray_dc(arr, mid + 1, high);
    SubarrayResult cross_res = max_crossing_subarray(arr, low, mid, high);

    /* Combine: return the maximum of the three */
    if (left_res.sum >= right_res.sum && left_res.sum >= cross_res.sum) {
        return left_res;
    } else if (right_res.sum >= left_res.sum && right_res.sum >= cross_res.sum) {
        return right_res;
    } else {
        return cross_res;
    }
}

SubarrayResult max_subarray_kadane(const int* arr, int count) {
    SubarrayResult invalid_res = {-1, -1, 0};
    if (!arr || count <= 0) {
        return invalid_res;
    }

    int max_so_far = arr[0];
    int max_ending_here = arr[0];
    int start = 0;
    int end = 0;
    int s = 0;

    for (int i = 1; i < count; i++) {
        if (arr[i] > max_ending_here + arr[i]) {
            max_ending_here = arr[i];
            s = i;
        } else {
            max_ending_here += arr[i];
        }

        if (max_ending_here > max_so_far) {
            max_so_far = max_ending_here;
            start = s;
            end = i;
        }
    }

    SubarrayResult res;
    res.left_idx = start;
    res.right_idx = end;
    res.sum = max_so_far;
    return res;
}
