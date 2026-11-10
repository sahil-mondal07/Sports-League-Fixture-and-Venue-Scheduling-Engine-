#ifndef KRUSKAL_H
#define KRUSKAL_H

#include "../../utils/common.h"
#include "../../data/graph.h"
#include "../../data/venue.h"

/**
 * @brief Representation of an undirected weighted edge for Kruskal's algorithm.
 */
typedef struct {
    int src;
    int dest;
    float weight;
} KruskalEdge;

/**
 * @brief Disjoint Set (Union-Find) with path compression and union by rank.
 */
typedef struct {
    int* parent;
    int* rank;
    int count;
} DisjointSet;

/**
 * @brief Kruskal MST result structure.
 */
typedef struct {
    KruskalEdge* mst_edges; /**< Array of edges included in MST */
    int mst_edge_count;     /**< Number of edges in MST (V - 1 for connected graph) */
    double total_weight;    /**< Total weight of the MST */
    int is_connected;       /**< 1 if a spanning tree was formed, 0 if disconnected */
} KruskalResult;

/**
 * @brief Create a disjoint-set (union-find) structure for n elements (0..n-1).
 */
DisjointSet* disjoint_set_create(int n);

/**
 * @brief Find the set representative of element u (with path compression).
 */
int disjoint_set_find(DisjointSet* ds, int u);

/**
 * @brief Union the sets containing u and v (by rank).
 * @return int 1 if sets were merged, 0 if u and v were already in the same set.
 */
int disjoint_set_union(DisjointSet* ds, int u, int v);

/**
 * @brief Free the disjoint-set structure.
 */
void disjoint_set_free(DisjointSet* ds);

/**
 * @brief Compute the Minimum Spanning Tree of a graph using Kruskal's algorithm.
 *
 * Sorts all unique undirected edges by weight (O(E log E)) and greedily
 * adds edges that do not form a cycle using Disjoint Set Union-Find.
 *
 * In sports scheduling context: Constructs the minimum-cost travel/telecom
 * backbone connecting all tournament venues.
 *
 * @param g Pointer to Graph (from src/data/graph.h)
 * @return KruskalResult containing the MST edges, total weight, and connectivity.
 *         Must be freed with free_kruskal_result().
 */
KruskalResult kruskal_mst(const Graph* g);

/**
 * @brief Print Kruskal MST result in human-readable form.
 */
void print_kruskal_result(const KruskalResult* result, const Venue* venues, int venue_count);

/**
 * @brief Free memory allocated in KruskalResult.
 */
void free_kruskal_result(KruskalResult* result);

#endif /* KRUSKAL_H */
