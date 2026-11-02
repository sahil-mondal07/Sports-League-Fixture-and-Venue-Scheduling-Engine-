#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "../../data/graph.h"
#include "../../utils/common.h"

#define DIJKSTRA_INF 1e9f

typedef struct {
    int num_vertices;
    int source;
    float* dist;        /* Array of shortest distances from source to each vertex */
    int* parent;        /* Array of predecessor vertices for path reconstruction */
} DijkstraResult;

/**
 * Runs Dijkstra's algorithm from a single source vertex on an adjacency-list Graph.
 * Edge weights must be non-negative.
 * Returns dynamically allocated DijkstraResult, or NULL on failure.
 * Caller must free via free_dijkstra_result().
 */
DijkstraResult* dijkstra_shortest_path(const Graph* g, int source);

/**
 * Reconstructs the shortest path from source to target.
 * path_out must have space for up to result->num_vertices ints.
 * Returns the number of vertices on the path (including source and target),
 * or 0 if unreachable / invalid.
 */
int dijkstra_get_path(const DijkstraResult* res, int target, int* path_out);

/**
 * Prints the shortest paths and distances from the source to all vertices.
 */
void print_dijkstra_result(const DijkstraResult* res);

/**
 * Frees memory allocated for a DijkstraResult.
 */
void free_dijkstra_result(DijkstraResult* res);

#endif /* DIJKSTRA_H */
