#include "prim.h"
#include "../../data/graph.h"
#include <stdlib.h>
#include <limits.h>

/* Run Prim's algorithm on the given graph starting from source vertex.
   result must be freed with free_prim_result(). */
PrimResult prim_mst(Graph* g, int src) {
    PrimResult result;
    result.parent = (int*)malloc(g->num_vertices * sizeof(int));
    result.key = (double*)malloc(g->num_vertices * sizeof(double));
    result.mst_set = (int*)calloc(g->num_vertices, sizeof(int));
    result.mst_edges = (int*)malloc((g->num_vertices - 1) * 2 * sizeof(int));
    result.mst_edge_count = 0;
    result.total_weight = 0.0;

    /* Initialize keys and parent */
    for (int i = 0; i < g->num_vertices; i++) {
        result.key[i] = (double)INT_MAX;
        result.parent[i] = -1;
    }

    result.key[src] = 0.0;

    for (int count = 0; count < g->num_vertices; count++) {
        /* Pick minimum key vertex not yet in MST */
        int u = -1;
        double min_key = (double)INT_MAX;

        for (int v = 0; v < g->num_vertices; v++) {
            if (!result.mst_set[v] && result.key[v] < min_key) {
                min_key = result.key[v];
                u = v;
            }
        }

        if (u == -1) break; /* Graph is disconnected */

        result.mst_set[u] = 1;

        /* Add edge to MST (except for the first vertex) */
        if (result.parent[u] != -1 && result.mst_edge_count < g->num_vertices - 1) {
            result.mst_edges[result.mst_edge_count * 2] = result.parent[u];
            result.mst_edges[result.mst_edge_count * 2 + 1] = u;
            result.mst_edge_count++;
            result.total_weight += result.key[u];
        }

        /* Update key values of adjacent vertices */
        /* g->adj_list[u] is Edge* (head of linked list for vertex u) */
        Edge* edge_u = g->adj_list[u];
        while (edge_u != NULL) {
            int v = edge_u->dest;
            if (!result.mst_set[v] && edge_u->weight < result.key[v]) {
                result.parent[v] = u;
                result.key[v] = (double)edge_u->weight;
            }
            edge_u = edge_u->next;
        }
    }

    return result;
}

/* Print MST result using venue array for labels. */
void print_mst_result(PrimResult* result, const Venue* venues, int venue_count) {
    if (!result) return;

    printf("\n=== Prim's Minimum Spanning Tree ===\n");
    printf("Total MST Weight: %.2f km\n\n", result->total_weight);
    printf("MST Edges (Venue Connections):\n");

    for (int i = 0; i < result->mst_edge_count; i++) {
        int u = result->mst_edges[i * 2];
        int v = result->mst_edges[i * 2 + 1];
        printf("  [%d] %s (%s) -- [%d] %s (%s) [%.2f km]\n",
               u, venues[u].name, venues[u].city,
               v, venues[v].name, venues[v].city,
               result->key[v] > 0 ? (double)result->key[v] : 0.0);
    }

    printf("\nConnectivity: All %d venues connected via %d edges\n",
           venue_count, result->mst_edge_count);
}

/* Free Prim result memory. */
void free_prim_result(PrimResult* result) {
    if (!result) return;
    free(result->parent);
    free(result->key);
    free(result->mst_set);
    free(result->mst_edges);
}