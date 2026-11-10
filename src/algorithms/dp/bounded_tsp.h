#ifndef BOUNDED_TSP_H
#define BOUNDED_TSP_H

#include "../../utils/common.h"
#include "../../data/venue.h"

#define TSP_INFINITY 1e30
#define TSP_MAX_EXACT_VERTICES 20

/**
 * @brief Result structure for Bounded TSP solver.
 */
typedef struct {
    int* tour;             /**< Sequence of venue IDs in tour (size num_stops + 1, starts & ends at start_node) */
    int tour_len;          /**< Number of vertices in tour path (num_stops + 1) */
    double total_cost;     /**< Total travel distance/cost of the closed tour */
    int is_exact;          /**< 1 if exact Held-Karp DP solution was computed, 0 if heuristic fallback was used */
} TSPResult;

/**
 * @brief Solve the Traveling Salesman Problem using Held-Karp Dynamic Programming.
 *
 * DP Recurrence (Held-Karp):
 * C(S, j) = min_{i in S \ {j}} { C(S \ {j}, i) + dist(i, j) }
 * where S is a subset of vertices represented as a bitmask, and j is the last visited vertex.
 *
 * In sports scheduling context: Optimizes referee delegation tours, broadcast crew
 * inspection routes, or multi-venue inspection circuits visiting all tournament venues.
 *
 * For n <= 20, computes the EXACT optimal tour in O(n^2 * 2^n) time.
 * For n > 20, uses a bounded Greedy Nearest Neighbor + 2-Opt local search.
 *
 * @param dist_matrix Row-major n x n distance matrix: dist_matrix[i * n + j]
 * @param n Number of venues/stops (1 <= n <= MAX_VENUES)
 * @param start_node Starting and ending venue ID (0 <= start_node < n)
 * @return TSPResult containing optimal/bounded tour and cost.
 *         Must be freed with free_tsp_result().
 */
TSPResult solve_bounded_tsp(const double* dist_matrix, int n, int start_node);

/**
 * @brief Print the TSP tour and total distance.
 */
void print_tsp_result(const TSPResult* res, const Venue* venues, int venue_count);

/**
 * @brief Free resources allocated in TSPResult.
 */
void free_tsp_result(TSPResult* res);

#endif /* BOUNDED_TSP_H */
