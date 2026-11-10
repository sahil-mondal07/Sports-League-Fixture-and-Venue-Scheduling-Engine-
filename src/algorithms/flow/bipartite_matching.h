#ifndef BIPARTITE_MATCHING_H
#define BIPARTITE_MATCHING_H

#include "../../utils/common.h"

#define MAX_BMP_NODES 50

/**
 * @brief Result structure for Maximum Bipartite Matching.
 */
typedef struct {
    int match_u[MAX_BMP_NODES]; /**< match_u[u] = venue matched to team u (-1 if unmatched) */
    int match_v[MAX_BMP_NODES]; /**< match_v[v] = team matched to venue v (-1 if unmatched) */
    int total_matches;          /**< Total number of valid pairs matched */
    int num_teams;
    int num_venues;
} BipartiteMatchResult;

/**
 * @brief Solve Maximum Bipartite Matching between teams (Set U) and venues (Set V).
 *
 * In sports scheduling context: Optimally assigns clubs to home venues
 * respecting preference compatibility constraints.
 *
 * @param adj_matrix n_teams x n_venues adjacency matrix where adj_matrix[t * n_venues + v] == 1
 *                   indicates team t can play at venue v.
 * @param num_teams Number of teams (Set U size)
 * @param num_venues Number of venues (Set V size)
 * @return BipartiteMatchResult containing matching pairs and total match count.
 */
BipartiteMatchResult solve_maximum_bipartite_matching(const int* adj_matrix, int num_teams, int num_venues);

/**
 * @brief Print bipartite matching results.
 */
void print_bipartite_match_result(const BipartiteMatchResult* res);

#endif /* BIPARTITE_MATCHING_H */
