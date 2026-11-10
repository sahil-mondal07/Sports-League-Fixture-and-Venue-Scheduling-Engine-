#ifndef BELLMAN_FORD_H
#define BELLMAN_FORD_H

#include "../../utils/common.h"
#include "../../data/graph.h"
#include "../../data/venue.h"

#define BF_INFINITY 1e30

/**
 * @brief Result structure for Bellman-Ford algorithm.
 */
typedef struct {
    double* dist;               /**< dist[v] = shortest distance from src to v */
    int* predecessor;          /**< predecessor[v] = parent of v in shortest path tree */
    int num_vertices;          /**< Number of vertices in graph */
    int source;                /**< Starting vertex index */
    int has_negative_cycle;    /**< 1 if reachable negative cycle detected, 0 otherwise */
    int* cycle_vertices;       /**< Array of vertex IDs forming negative cycle, or NULL */
    int cycle_len;             /**< Length of cycle_vertices array */
} BellmanFordResult;

/**
 * @brief Run the Bellman-Ford algorithm from a source vertex.
 *
 * Computes single-source shortest paths on graphs with arbitrary (including
 * negative) edge weights via dynamic programming relaxations:
 * d^(k)[v] = min(d^(k-1)[v], min_{(u,v)} { d^(k-1)[u] + w(u,v) })
 *
 * In sports scheduling context: Calculates team travel costs with home
 * subsidies, rest discounts (negative weights), and detects negative-cost
 * scheduling loops (arbitrage/unsustainable fixture sequences).
 *
 * @param g Pointer to Graph (from src/data/graph.h)
 * @param src Source vertex index (0 <= src < g->num_vertices)
 * @return BellmanFordResult containing distances, paths, and negative cycle status.
 *         Must be freed with free_bellman_ford_result().
 */
BellmanFordResult bellman_ford(const Graph* g, int src);

/**
 * @brief Reconstruct the shortest path from source to destination.
 *
 * @param res Pointer to BellmanFordResult
 * @param dest Destination vertex index
 * @param path_out Pre-allocated array of size at least res->num_vertices
 * @param path_len Output: number of vertices in reconstructed path
 * @return int 0 on success, -1 if no path exists or destination is unreachable.
 */
int bellman_ford_get_path(const BellmanFordResult* res, int dest, int* path_out, int* path_len);

/**
 * @brief Print Bellman-Ford shortest paths and any negative cycle report.
 */
void print_bellman_ford_result(const BellmanFordResult* res, const Venue* venues, int venue_count);

/**
 * @brief Free resources allocated in BellmanFordResult.
 */
void free_bellman_ford_result(BellmanFordResult* res);

#endif /* BELLMAN_FORD_H */
