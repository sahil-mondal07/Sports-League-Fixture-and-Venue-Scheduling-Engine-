#include "graph.h"

Graph* create_graph(int vertices) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    if (!graph) return NULL;

    graph->num_vertices = vertices;
    graph->adj_list = (Edge**)malloc(vertices * sizeof(Edge*));
    if (!graph->adj_list) {
        free(graph);
        return NULL;
    }

    for (int i = 0; i < vertices; i++) {
        graph->adj_list[i] = NULL;
    }

    return graph;
}

void add_edge(Graph* g, int src, int dest, float weight) {
    if (!g || src < 0 || src >= g->num_vertices || dest < 0 || dest >= g->num_vertices) {
        return;
    }

    Edge* new_edge = (Edge*)malloc(sizeof(Edge));
    if (!new_edge) return;

    new_edge->dest = dest;
    new_edge->weight = weight;
    new_edge->next = g->adj_list[src];
    g->adj_list[src] = new_edge;
}

void add_undirected_edge(Graph* g, int u, int v, float weight) {
    add_edge(g, u, v, weight);
    add_edge(g, v, u, weight);
}

void print_graph(const Graph* g) {
    if (!g) return;

    printf("\n=== Graph (Adjacency List) ===\n");
    for (int i = 0; i < g->num_vertices; i++) {
        printf("Vertex %d:", i);
        Edge* current = g->adj_list[i];
        while (current) {
            printf(" -> %d (%.2f)", current->dest, current->weight);
            current = current->next;
        }
        printf("\n");
    }
}

void free_graph(Graph* g) {
    if (!g) return;

    for (int i = 0; i < g->num_vertices; i++) {
        Edge* current = g->adj_list[i];
        while (current) {
            Edge* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(g->adj_list);
    free(g);
}

DistanceMatrix* create_distance_matrix(int size) {
    DistanceMatrix* dm = (DistanceMatrix*)malloc(sizeof(DistanceMatrix));
    if (!dm) return NULL;

    dm->size = size;
    dm->distances = (float**)malloc(size * sizeof(float*));
    if (!dm->distances) {
        free(dm);
        return NULL;
    }

    for (int i = 0; i < size; i++) {
        dm->distances[i] = (float*)malloc(size * sizeof(float));
        if (!dm->distances[i]) {
            for (int j = 0; j < i; j++) {
                free(dm->distances[j]);
            }
            free(dm->distances);
            free(dm);
            return NULL;
        }
        for (int j = 0; j < size; j++) {
            dm->distances[i][j] = (i == j) ? 0.0 : INFINITY;
        }
    }

    return dm;
}

void set_distance(DistanceMatrix* dm, int from, int to, float dist) {
    if (!dm || from < 0 || from >= dm->size || to < 0 || to >= dm->size) {
        return;
    }
    dm->distances[from][to] = dist;
}

float get_distance(DistanceMatrix* dm, int from, int to) {
    if (!dm || from < 0 || from >= dm->size || to < 0 || to >= dm->size) {
        return INFINITY;
    }
    return dm->distances[from][to];
}

void free_distance_matrix(DistanceMatrix* dm) {
    if (!dm) return;

    for (int i = 0; i < dm->size; i++) {
        free(dm->distances[i]);
    }
    free(dm->distances);
    free(dm);
}
