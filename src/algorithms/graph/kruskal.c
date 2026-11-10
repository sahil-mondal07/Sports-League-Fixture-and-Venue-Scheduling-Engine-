#include "kruskal.h"
#include <stdlib.h>
#include <stdio.h>

DisjointSet* disjoint_set_create(int n) {
    if (n <= 0) return NULL;
    DisjointSet* ds = (DisjointSet*)malloc(sizeof(DisjointSet));
    if (!ds) return NULL;

    ds->parent = (int*)malloc((size_t)n * sizeof(int));
    ds->rank = (int*)calloc((size_t)n, sizeof(int));
    if (!ds->parent || !ds->rank) {
        free(ds->parent);
        free(ds->rank);
        free(ds);
        return NULL;
    }

    ds->count = n;
    for (int i = 0; i < n; i++) {
        ds->parent[i] = i;
    }
    return ds;
}

int disjoint_set_find(DisjointSet* ds, int u) {
    if (!ds || u < 0 || u >= ds->count) return -1;
    if (ds->parent[u] != u) {
        ds->parent[u] = disjoint_set_find(ds, ds->parent[u]); /* Path compression */
    }
    return ds->parent[u];
}

int disjoint_set_union(DisjointSet* ds, int u, int v) {
    int root_u = disjoint_set_find(ds, u);
    int root_v = disjoint_set_find(ds, v);
    if (root_u == -1 || root_v == -1 || root_u == root_v) {
        return 0; /* Already in same component or invalid */
    }

    /* Union by rank */
    if (ds->rank[root_u] < ds->rank[root_v]) {
        ds->parent[root_u] = root_v;
    } else if (ds->rank[root_u] > ds->rank[root_v]) {
        ds->parent[root_v] = root_u;
    } else {
        ds->parent[root_v] = root_u;
        ds->rank[root_u]++;
    }
    return 1;
}

void disjoint_set_free(DisjointSet* ds) {
    if (ds) {
        free(ds->parent);
        free(ds->rank);
        free(ds);
    }
}

/* Edge comparator for qsort */
static int compare_kruskal_edges(const void* a, const void* b) {
    const KruskalEdge* edge_a = (const KruskalEdge*)a;
    const KruskalEdge* edge_b = (const KruskalEdge*)b;
    if (edge_a->weight < edge_b->weight) return -1;
    if (edge_a->weight > edge_b->weight) return 1;
    return 0;
}

KruskalResult kruskal_mst(const Graph* g) {
    KruskalResult res;
    res.mst_edges = NULL;
    res.mst_edge_count = 0;
    res.total_weight = 0.0;
    res.is_connected = 0;

    if (!g || g->num_vertices <= 0) {
        return res;
    }

    int V = g->num_vertices;
    if (V == 1) {
        res.is_connected = 1;
        return res;
    }

    /* Count total undirected edges.
       In undirected graph represented by adjacency list, an edge (u, v)
       with u < v is counted once. */
    int edge_capacity = 0;
    for (int u = 0; u < V; u++) {
        Edge* curr = g->adj_list[u];
        while (curr) {
            if (u < curr->dest) {
                edge_capacity++;
            }
            curr = curr->next;
        }
    }

    if (edge_capacity == 0) {
        return res;
    }

    KruskalEdge* all_edges = (KruskalEdge*)malloc((size_t)edge_capacity * sizeof(KruskalEdge));
    if (!all_edges) {
        return res;
    }

    int edge_count = 0;
    for (int u = 0; u < V; u++) {
        Edge* curr = g->adj_list[u];
        while (curr) {
            if (u < curr->dest) {
                all_edges[edge_count].src = u;
                all_edges[edge_count].dest = curr->dest;
                all_edges[edge_count].weight = curr->weight;
                edge_count++;
            }
            curr = curr->next;
        }
    }

    /* Sort edges ascending by weight: O(E log E) */
    qsort(all_edges, (size_t)edge_count, sizeof(KruskalEdge), compare_kruskal_edges);

    DisjointSet* ds = disjoint_set_create(V);
    if (!ds) {
        free(all_edges);
        return res;
    }

    res.mst_edges = (KruskalEdge*)malloc((size_t)(V - 1) * sizeof(KruskalEdge));
    if (!res.mst_edges) {
        free(all_edges);
        disjoint_set_free(ds);
        return res;
    }

    /* Greedily take smallest non-cycle edge */
    for (int i = 0; i < edge_count; i++) {
        if (disjoint_set_union(ds, all_edges[i].src, all_edges[i].dest)) {
            res.mst_edges[res.mst_edge_count++] = all_edges[i];
            res.total_weight += (double)all_edges[i].weight;
            if (res.mst_edge_count == V - 1) {
                break;
            }
        }
    }

    res.is_connected = (res.mst_edge_count == V - 1) ? 1 : 0;

    free(all_edges);
    disjoint_set_free(ds);
    return res;
}

void print_kruskal_result(const KruskalResult* result, const Venue* venues, int venue_count) {
    if (!result) return;

    printf("\n=== Kruskal Minimum Spanning Tree ===\n");
    printf("Total MST Weight: %.2f km\n", result->total_weight);
    printf("MST Edges: %d\n", result->mst_edge_count);
    printf("Graph Connected: %s\n", result->is_connected ? "Yes" : "No (Forest formed)");
    printf("%-25s <---> %-25s %-10s\n", "Venue A", "Venue B", "Distance");
    printf("-----------------------------------------------------------------\n");

    for (int i = 0; i < result->mst_edge_count; i++) {
        int u = result->mst_edges[i].src;
        int v = result->mst_edges[i].dest;
        const char* name_u = (venues && u < venue_count) ? venues[u].name : "Unknown";
        const char* name_v = (venues && v < venue_count) ? venues[v].name : "Unknown";
        printf("%-25s <---> %-25s %8.2f km\n", name_u, name_v, result->mst_edges[i].weight);
    }
}

void free_kruskal_result(KruskalResult* result) {
    if (result) {
        free(result->mst_edges);
        result->mst_edges = NULL;
        result->mst_edge_count = 0;
        result->total_weight = 0.0;
        result->is_connected = 0;
    }
}
