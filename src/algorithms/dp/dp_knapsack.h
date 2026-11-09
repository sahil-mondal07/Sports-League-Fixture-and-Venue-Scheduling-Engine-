#ifndef DP_KNAPSACK_H
#define DP_KNAPSACK_H

#include "../../utils/common.h"

/**
 * @brief Represents an item with value and weight for knapsack optimization.
 * In the sports scheduling context: value = team ranking/points, weight = broadcast slot cost.
 */
typedef struct {
    int id;                /**< Item identifier (e.g., match ID) */
    int value;             /**< Value of the item (e.g., team ranking / points) */
    int weight;            /**< Weight/cost of the item (e.g., broadcast slot duration) */
} KnapsackItem;

/**
 * @brief Solve the 0/1 Knapsack problem using dynamic programming.
 *
 * Determines the maximum-value subset of items that fit within the given
 * broadcast slot capacity. Each item is either taken or left (0/1 choice).
 *
 * @param capacity Maximum broadcast slot capacity (total available slots)
 * @param items    Array of n items, each with a value and weight
 * @param n        Number of items available for selection
 * @param selected_out Pre-allocated array of size n: selected_out[i] = 1 if item i is included,
 *                     0 if excluded. May be NULL if only the max value is needed.
 * @return int Maximum total value achievable within the capacity.
 *             Returns 0 if capacity <= 0 or items == NULL or n <= 0.
 *             Caller must check: if all items have weight > capacity,
 *             the result will be 0 (no items selected).
 */
int knapsack_01(int capacity, const KnapsackItem* items, int n, int* selected_out);

/**
 * @brief Print the knapsack solution details in human-readable form.
 *
 * @param capacity Knapsack capacity (total broadcast slots available)
 * @param items    Array of n items
 * @param n        Number of items
 * @param selected Array indicating which items are selected (1 = selected, 0 = not selected).
 *                 May be NULL if only the summary is needed.
 * @param total_value Output: total value of the selected items. May be NULL.
 * @param total_weight Output: total weight of the selected items. May be NULL.
 */
void knapsack_print_solution(int capacity, const KnapsackItem* items, int n,
                             const int* selected, int total_value, int total_weight);

#endif /* DP_KNAPSACK_H */