#ifndef BRANCH_BOUND_H
#define BRANCH_BOUND_H

#include "../../utils/common.h"

/**
 * @brief Strategy selector for Branch and Bound search exploration.
 */
typedef enum {
    BB_STRATEGY_FIFO,      /**< Breadth-First Search using a FIFO queue */
    BB_STRATEGY_LIFO,      /**< Depth-First Search using a LIFO stack */
    BB_STRATEGY_LEAST_COST /**< Best-First Search using a min-priority queue on lower bound */
} BBStrategy;

/**
 * @brief Performance and solution result for Branch and Bound optimization.
 */
typedef struct {
    int* assignment;       /**< assignment[i] = venue/slot assigned to match i */
    int n;                 /**< Problem dimension */
    double best_cost;      /**< Minimum total assignment cost found */
    long nodes_generated;  /**< Number of state nodes generated */
    long nodes_explored;   /**< Number of state nodes dequeued and evaluated */
    long nodes_pruned;     /**< Number of state nodes pruned by bound check */
    BBStrategy strategy;   /**< Exploration strategy used */
} BBResult;

/**
 * @brief Solve optimal fixture-to-venue assignment using Branch and Bound.
 *
 * In sports scheduling context: Assigns N marquee fixtures to N available
 * venue-slot resources to minimize total travel + venue hosting costs.
 *
 * Compares FIFO, LIFO, and Least-Cost state space tree exploration under
 * identical lower-bound pruning functions.
 *
 * Lower bound calculation:
 * LB = current_cost + sum_{i = k}^{n-1} min_{j unassigned} { cost_matrix[i][j] }
 *
 * @param cost_matrix Row-major n x n cost table: cost_matrix[i * n + j] is the cost
 *                    of assigning match i to venue/slot j
 * @param n Dimension of assignment problem (1 <= n <= 20)
 * @param strategy BB_STRATEGY_FIFO, BB_STRATEGY_LIFO, or BB_STRATEGY_LEAST_COST
 * @return BBResult with optimal assignment and performance metrics.
 *         Must be freed with free_bb_result().
 */
BBResult solve_assignment_branch_bound(const double* cost_matrix, int n, BBStrategy strategy);

/**
 * @brief Print and compare Branch and Bound results.
 */
void print_bb_result(const BBResult* res);

/**
 * @brief Free resources allocated in BBResult.
 */
void free_bb_result(BBResult* res);

#endif /* BRANCH_BOUND_H */
