#ifndef N_QUEENS_H
#define N_QUEENS_H

#include "../../utils/common.h"

/**
 * @brief Solution structure containing valid match/queen placements.
 */
typedef struct {
    int* positions;    /**< positions[row] = column index (venue/matchday) of the queen */
    int n;             /**< Board dimension (N x N) */
} NQueensSolution;

/**
 * @brief Container for all solutions found during exhaustive backtracking.
 */
typedef struct {
    NQueensSolution* solutions; /**< Array of valid solutions */
    int count;                  /**< Total number of solutions found */
    int capacity;               /**< Allocated capacity of solutions array */
} NQueensResultSet;

/**
 * @brief Solve N-Queens variant with optional venue/slot reservation blocks.
 *
 * In sports scheduling context: Schedules N marquee matches across N time slots
 * and N venues such that no two matches share a slot (row), venue (col), or
 * overlapping broadcast/travel buffer (diagonal conflict), while respecting
 * unavailable/blackout dates.
 *
 * Backtracking: T(n) = n! with heavy branch pruning.
 *
 * @param n Board size (number of slots/venues, 1 <= n <= 32)
 * @param blocked Optional n x n matrix where blocked[r * n + c] == 1 indicates
 *                that venue c is unavailable at time slot r. Pass NULL if no blocks.
 * @param first_only If 1, stops after finding the first valid schedule; if 0, finds all.
 * @return NQueensResultSet containing all discovered valid configurations.
 *         Must be freed with free_n_queens_result().
 */
NQueensResultSet solve_n_queens(int n, const int* blocked, int first_only);

/**
 * @brief Print an N-Queens solution grid with labels.
 */
void print_n_queens_solution(const NQueensSolution* sol);

/**
 * @brief Free resources in NQueensResultSet.
 */
void free_n_queens_result(NQueensResultSet* result);

#endif /* N_QUEENS_H */
