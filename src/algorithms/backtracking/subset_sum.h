#ifndef SUBSET_SUM_H
#define SUBSET_SUM_H

#include "../../utils/common.h"

/**
 * @brief Representation of a subset summing to the target.
 */
typedef struct {
    int* indices;      /**< Indices of original items included in the subset */
    int* values;       /**< Values of items included */
    int count;         /**< Number of items in this subset */
    int sum;           /**< Total sum (equals target) */
} SubsetSumSolution;

/**
 * @brief Collection of all matching subsets.
 */
typedef struct {
    SubsetSumSolution* solutions; /**< Array of matching solutions */
    int count;                    /**< Number of matching subsets */
    int capacity;                 /**< Capacity of solutions array */
} SubsetSumResultSet;

/**
 * @brief Solve Subset Sum problem using Backtracking with branch pruning.
 *
 * In sports scheduling context: Groups teams into balanced divisions with
 * identical target cumulative strength, or selects exact-fit match packages
 * for TV programming blocks.
 *
 * Pruning rules:
 * 1. current_sum + item > target => skip item
 * 2. current_sum + suffix_sum < target => prune entire branch
 *
 * @param values Array of positive integers (item values/weights)
 * @param n Number of items
 * @param target Desired sum
 * @param first_only If 1, returns upon finding the first valid subset; if 0, finds all.
 * @return SubsetSumResultSet containing all matching subsets.
 *         Must be freed with free_subset_sum_result().
 */
SubsetSumResultSet solve_subset_sum(const int* values, int n, int target, int first_only);

/**
 * @brief Print subset sum results.
 */
void print_subset_sum_result(const SubsetSumResultSet* res);

/**
 * @brief Free resources in SubsetSumResultSet.
 */
void free_subset_sum_result(SubsetSumResultSet* res);

#endif /* SUBSET_SUM_H */
