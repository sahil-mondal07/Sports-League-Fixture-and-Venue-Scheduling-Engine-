#ifndef MAX_SUBARRAY_H
#define MAX_SUBARRAY_H

#include "../../utils/common.h"

/**
 * @brief Result structure for maximum subarray search.
 */
typedef struct {
    int left_idx;   /**< Starting index (inclusive) of the maximum subarray */
    int right_idx;  /**< Ending index (inclusive) of the maximum subarray */
    int sum;        /**< Sum of elements in array[left_idx..right_idx] */
} SubarrayResult;

/**
 * @brief Find the contiguous subarray with maximum sum using Divide and Conquer.
 *
 * Recurrence: T(n) = 2T(n/2) + O(n) => O(n log n) runtime, O(log n) stack space.
 *
 * In sports scheduling context: Identifies a team's most favorable consecutive
 * fixture run (e.g., home advantage differential, net momentum streak).
 *
 * @param arr Array of integers (values can be positive, negative, or zero)
 * @param low Starting index (inclusive, typically 0)
 * @param high Ending index (inclusive, typically count - 1)
 * @return SubarrayResult containing start index, end index, and maximum sum.
 *         If arr is NULL or low > high, returns indices -1 and sum 0.
 */
SubarrayResult max_subarray_dc(const int* arr, int low, int high);

/**
 * @brief Linear-time reference (Kadane's algorithm) for correctness checks and benchmarking.
 *
 * Runtime: O(n), Space: O(1).
 *
 * @param arr Array of integers
 * @param count Number of elements in array
 * @return SubarrayResult containing start index, end index, and maximum sum.
 */
SubarrayResult max_subarray_kadane(const int* arr, int count);

#endif /* MAX_SUBARRAY_H */
