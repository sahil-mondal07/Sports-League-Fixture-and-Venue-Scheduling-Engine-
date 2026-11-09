#ifndef DP_OPTIMAL_BST_H
#define DP_OPTIMAL_BST_H

#include "../../utils/common.h"

/**
 * @brief Solve the Optimal Binary Search Tree problem using dynamic programming.
 *
 * Given n keys with associated search probabilities, find the BST structure
 * that minimizes the expected search cost (average number of comparisons).
 *
 * In the sports scheduling context: Optimal BST can optimize fixture lookups,
 * round queries, or venue searches when access frequency varies across
 * different parts of the schedule.
 *
 * @param p Array of size n+1 where p[i] is the search probability for key i
 *          (1-indexed). p[0] is typically 0 or unused.
 * @param n Number of keys (the p array has n+1 elements).
 * @param out_weight Output: sum of all probabilities (total weight).
 * @param out_root Output: pre-allocated array of size n+1 where out_root[k]
 *                 stores the root of the optimal BST subtree containing
 *                 keys 1..k. May be NULL if only the expected cost is needed.
 * @return double Minimum expected search cost. Returns -1 on error.
 */
double optimal_bst(const double* p, int n, double* out_weight, int* out_root);

#endif /* DP_OPTIMAL_BST_H */