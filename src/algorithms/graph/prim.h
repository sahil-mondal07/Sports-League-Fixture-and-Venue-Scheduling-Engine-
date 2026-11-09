#ifndef PRIM_H
#define PRIM_H

#include "../../utils/common.h"
#include "../../data/graph.h"
#include "../../data/venue.h"

/* Prim's MST result structure - uses the shared Graph from src/data/graph.h */
typedef struct {
    int* parent;                  // parent[v] = previous vertex in MST path
    double* key;                  // key[v] = minimum edge weight to connect v to MST (double for fractional weights)
    int* mst_set;                 // mst_set[v] = true if v is in MST
    double total_weight;          // Total MST weight
    int* mst_edges;               // Array of edges in MST (stores u,v pairs: u0,v0,u1,v1,...)
    int mst_edge_count;           // Number of edges in MST
} PrimResult;

/* Run Prim's algorithm on the given graph starting from source vertex.
   result must be freed with free_prim_result(). */
PrimResult prim_mst(Graph* g, int src);

/* Print MST result using venue array for labels. */
void print_mst_result(PrimResult* result, const Venue* venues, int venue_count);

/* Free Prim result memory. */
void free_prim_result(PrimResult* result);

#endif /* PRIM_H */