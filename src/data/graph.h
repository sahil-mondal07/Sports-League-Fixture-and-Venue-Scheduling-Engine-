#ifndef GRAPH_H
#define GRAPH_H

#include "../utils/common.h"

typedef struct Edge {
    int dest;
    float weight;
    struct Edge* next;
} Edge;

typedef struct {
    int num_vertices;
    Edge** adj_list;
} Graph;

typedef struct {
    int size;
    float** distances;
} DistanceMatrix;

Graph* create_graph(int vertices);

void add_edge(Graph* g, int src, int dest, float weight);

void add_undirected_edge(Graph* g, int u, int v, float weight);

void print_graph(const Graph* g);

void free_graph(Graph* g);

DistanceMatrix* create_distance_matrix(int size);

void set_distance(DistanceMatrix* dm, int from, int to, float dist);

float get_distance(DistanceMatrix* dm, int from, int to);

void free_distance_matrix(DistanceMatrix* dm);

#endif
