#ifndef FLOYD_WARSHALL_H
#define FLOYD_WARSHALL_H

#include "../../utils/common.h"
#include "../../data/graph.h"
#include "../../data/venue.h"

#define FW_INFINITY 1e30

/**
 * @brief Result structure for all-pairs shortest paths via Floyd-Warshall.
 */
typedef struct {
    double** dist;         /**< dist[u][v] = length of shortest path from u to v */
    int** next;            /**< next[u][v] = intermediate vertex on path u -> v for reconstruction */
    int num_vertices;      /**< Total vertices */
    int has_negative_cycle;/**< 1 if negative cycle detected on diagonal, 0 otherwise */
} FloydWarshallResult;

/**
 * @brief Run Floyd-Warshall all-pairs shortest path algorithm on a graph.
 *
 * DP Recurrence:
 * d^(k)[i][j] = min( d^(k-1)[i][j], d^(k-1)[i][k] + d^(k-1)[k][j] )
 *
 * In sports scheduling context: Precomputes all-pairs travel distances between
 * every venue in the league to allow O(1) fixture travel evaluations.
 *
 * @param g Pointer to Graph (from src/data/graph.h)
 * @return FloydWarshallResult containing V x V distance and next-hop tables.
 *         Must be freed with free_floyd_warshall_result().
 */
FloydWarshallResult floyd_warshall(const Graph* g);

/**
 * @brief Reconstruct the sequence of vertices from u to v.
 *
 * @param res Pointer to FloydWarshallResult
 * @param u Starting venue/vertex
 * @param v Destination venue/vertex
 * @param path_out Pre-allocated array of size at least res->num_vertices
 * @param path_len Output: number of vertices in reconstructed path
 * @return int 0 on success, -1 if no path exists or invalid input.
 */
int floyd_warshall_get_path(const FloydWarshallResult* res, int u, int v,
                           int* path_out, int* path_len);

/**
 * @brief Print the all-pairs distance matrix.
 */
void print_floyd_warshall_matrix(const FloydWarshallResult* res, const Venue* venues, int venue_count);

/**
 * @brief Free memory allocated in FloydWarshallResult.
 */
void free_floyd_warshall_result(FloydWarshallResult* res);

#endif /* FLOYD_WARSHALL_H */
