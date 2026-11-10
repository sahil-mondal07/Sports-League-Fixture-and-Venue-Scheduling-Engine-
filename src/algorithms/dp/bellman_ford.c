#include "bellman_ford.h"
#include <stdlib.h>
#include <stdio.h>

/* Internal directed edge structure extracted from graph */
typedef struct {
    int src;
    int dest;
    double weight;
} DirectedEdge;

BellmanFordResult bellman_ford(const Graph* g, int src) {
    BellmanFordResult res;
    res.dist = NULL;
    res.predecessor = NULL;
    res.num_vertices = 0;
    res.source = src;
    res.has_negative_cycle = 0;
    res.cycle_vertices = NULL;
    res.cycle_len = 0;

    if (!g || g->num_vertices <= 0 || src < 0 || src >= g->num_vertices) {
        return res;
    }

    int V = g->num_vertices;
    res.num_vertices = V;

    /* Extract all directed edges from the adjacency list */
    int edge_capacity = 0;
    for (int u = 0; u < V; u++) {
        Edge* e = g->adj_list[u];
        while (e) {
            edge_capacity++;
            e = e->next;
        }
    }

    DirectedEdge* edges = NULL;
    if (edge_capacity > 0) {
        edges = (DirectedEdge*)malloc((size_t)edge_capacity * sizeof(DirectedEdge));
        if (!edges) {
            return res;
        }
    }

    int edge_count = 0;
    for (int u = 0; u < V; u++) {
        Edge* e = g->adj_list[u];
        while (e) {
            edges[edge_count].src = u;
            edges[edge_count].dest = e->dest;
            edges[edge_count].weight = (double)e->weight;
            edge_count++;
            e = e->next;
        }
    }

    /* Allocate distance and predecessor arrays */
    res.dist = (double*)malloc((size_t)V * sizeof(double));
    res.predecessor = (int*)malloc((size_t)V * sizeof(int));

    if (!res.dist || !res.predecessor) {
        free(edges);
        free(res.dist);
        free(res.predecessor);
        res.dist = NULL;
        res.predecessor = NULL;
        return res;
    }

    /* Step 1: Initialize distances from source */
    for (int i = 0; i < V; i++) {
        res.dist[i] = BF_INFINITY;
        res.predecessor[i] = -1;
    }
    res.dist[src] = 0.0;

    /* Step 2: Relax all edges |V| - 1 times */
    for (int i = 1; i <= V - 1; i++) {
        int updated = 0;
        for (int j = 0; j < edge_count; j++) {
            int u = edges[j].src;
            int v = edges[j].dest;
            double w = edges[j].weight;

            if (res.dist[u] < BF_INFINITY / 2.0 && res.dist[u] + w < res.dist[v]) {
                res.dist[v] = res.dist[u] + w;
                res.predecessor[v] = u;
                updated = 1;
            }
        }
        /* Early exit if no relaxation occurred */
        if (!updated) {
            break;
        }
    }

    /* Step 3: Check for negative-weight cycles */
    int cycle_start = -1;
    for (int j = 0; j < edge_count; j++) {
        int u = edges[j].src;
        int v = edges[j].dest;
        double w = edges[j].weight;

        if (res.dist[u] < BF_INFINITY / 2.0 && res.dist[u] + w < res.dist[v]) {
            res.has_negative_cycle = 1;
            cycle_start = v;
            break;
        }
    }

    /* Step 4: If negative cycle detected, trace the cycle vertices */
    if (res.has_negative_cycle && cycle_start != -1) {
        /* Walk back V times to guarantee reaching a node inside the cycle */
        int curr = cycle_start;
        for (int i = 0; i < V; i++) {
            if (curr != -1 && res.predecessor[curr] != -1) {
                curr = res.predecessor[curr];
            }
        }

        /* Now trace the cycle starting from curr until we encounter curr again */
        int* temp_cycle = (int*)malloc((size_t)(V + 1) * sizeof(int));
        if (temp_cycle) {
            int node = curr;
            int count = 0;
            do {
                temp_cycle[count++] = node;
                node = res.predecessor[node];
            } while (node != curr && count < V && node != -1);
            temp_cycle[count++] = curr; /* close the loop */

            res.cycle_vertices = (int*)malloc((size_t)count * sizeof(int));
            if (res.cycle_vertices) {
                for (int i = 0; i < count; i++) {
                    res.cycle_vertices[i] = temp_cycle[count - 1 - i]; /* reverse to natural direction */
                }
                res.cycle_len = count;
            }
            free(temp_cycle);
        }
    }

    free(edges);
    return res;
}

int bellman_ford_get_path(const BellmanFordResult* res, int dest, int* path_out, int* path_len) {
    if (!res || !path_out || !path_len || dest < 0 || dest >= res->num_vertices) {
        return -1;
    }
    if (res->has_negative_cycle) {
        return -1; /* Shortest path undefined in presence of negative cycle */
    }
    if (res->dist[dest] >= BF_INFINITY / 2.0) {
        *path_len = 0;
        return -1; /* Unreachable */
    }

    int temp[MAX_VENUES];
    int count = 0;
    int curr = dest;

    while (curr != -1 && count < MAX_VENUES) {
        temp[count++] = curr;
        if (curr == res->source) break;
        curr = res->predecessor[curr];
    }

    if (count == 0 || temp[count - 1] != res->source) {
        *path_len = 0;
        return -1;
    }

    /* Reverse to get path from source to dest */
    for (int i = 0; i < count; i++) {
        path_out[i] = temp[count - 1 - i];
    }
    *path_len = count;
    return 0;
}

void print_bellman_ford_result(const BellmanFordResult* res, const Venue* venues, int venue_count) {
    if (!res) return;

    const char* src_name = (venues && res->source < venue_count) ? venues[res->source].name : "Unknown";
    printf("\n=== Bellman-Ford Shortest Paths from Source [%s] ===\n", src_name);

    if (res->has_negative_cycle) {
        printf("⚠️ WARNING: Reachable negative-weight cycle detected!\n");
        if (res->cycle_vertices && res->cycle_len > 0) {
            printf("Negative Cycle Path: ");
            for (int i = 0; i < res->cycle_len; i++) {
                int v = res->cycle_vertices[i];
                const char* name = (venues && v < venue_count) ? venues[v].name : "Node";
                printf("%s%s", name, (i < res->cycle_len - 1) ? " -> " : "\n");
            }
        }
        return;
    }

    printf("%-5s %-25s %-12s %-15s\n", "ID", "Venue Name", "Net Cost", "Predecessor");
    printf("-----------------------------------------------------------------\n");

    for (int i = 0; i < res->num_vertices; i++) {
        const char* name = (venues && i < venue_count) ? venues[i].name : "Venue";
        const char* pred_name = (res->predecessor[i] != -1 && venues && res->predecessor[i] < venue_count)
                                ? venues[res->predecessor[i]].name : "None";

        if (res->dist[i] >= BF_INFINITY / 2.0) {
            printf("%-5d %-25s %-12s %-15s\n", i, name, "Unreachable", "-");
        } else {
            printf("%-5d %-25s %12.2f %-15s\n", i, name, res->dist[i], pred_name);
        }
    }
}

void free_bellman_ford_result(BellmanFordResult* res) {
    if (res) {
        free(res->dist);
        free(res->predecessor);
        free(res->cycle_vertices);
        res->dist = NULL;
        res->predecessor = NULL;
        res->cycle_vertices = NULL;
        res->num_vertices = 0;
        res->cycle_len = 0;
        res->has_negative_cycle = 0;
    }
}
