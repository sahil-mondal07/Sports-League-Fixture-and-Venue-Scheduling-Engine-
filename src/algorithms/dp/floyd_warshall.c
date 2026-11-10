#include "floyd_warshall.h"
#include <stdlib.h>
#include <stdio.h>

FloydWarshallResult floyd_warshall(const Graph* g) {
    FloydWarshallResult res;
    res.dist = NULL;
    res.next = NULL;
    res.num_vertices = 0;
    res.has_negative_cycle = 0;

    if (!g || g->num_vertices <= 0) {
        return res;
    }

    int V = g->num_vertices;
    res.num_vertices = V;

    /* Allocate V x V distance and next tables */
    res.dist = (double**)malloc((size_t)V * sizeof(double*));
    res.next = (int**)malloc((size_t)V * sizeof(int*));

    if (!res.dist || !res.next) {
        free(res.dist);
        free(res.next);
        res.dist = NULL;
        res.next = NULL;
        return res;
    }

    for (int i = 0; i < V; i++) {
        res.dist[i] = (double*)malloc((size_t)V * sizeof(double));
        res.next[i] = (int*)malloc((size_t)V * sizeof(int));
        if (!res.dist[i] || !res.next[i]) {
            for (int k = 0; k <= i; k++) {
                if (res.dist) free(res.dist[k]);
                if (res.next) free(res.next[k]);
            }
            free(res.dist);
            free(res.next);
            res.dist = NULL;
            res.next = NULL;
            return res;
        }
    }

    /* Initialize base distances */
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (i == j) {
                res.dist[i][j] = 0.0;
                res.next[i][j] = j;
            } else {
                res.dist[i][j] = FW_INFINITY;
                res.next[i][j] = -1;
            }
        }
    }

    /* Fill edge weights from adjacency list */
    for (int u = 0; u < V; u++) {
        Edge* curr = g->adj_list[u];
        while (curr) {
            int v = curr->dest;
            double w = (double)curr->weight;
            if (w < res.dist[u][v]) {
                res.dist[u][v] = w;
                res.next[u][v] = v;
            }
            curr = curr->next;
        }
    }

    /* Dynamic programming triple loop: O(V^3) */
    for (int k = 0; k < V; k++) {
        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                if (res.dist[i][k] < FW_INFINITY / 2.0 && res.dist[k][j] < FW_INFINITY / 2.0) {
                    double through_k = res.dist[i][k] + res.dist[k][j];
                    if (through_k < res.dist[i][j]) {
                        res.dist[i][j] = through_k;
                        res.next[i][j] = res.next[i][k];
                    }
                }
            }
        }
    }

    /* Negative cycle detection on main diagonal */
    for (int i = 0; i < V; i++) {
        if (res.dist[i][i] < 0.0) {
            res.has_negative_cycle = 1;
            break;
        }
    }

    return res;
}

int floyd_warshall_get_path(const FloydWarshallResult* res, int u, int v,
                           int* path_out, int* path_len) {
    if (!res || !path_out || !path_len ||
        u < 0 || u >= res->num_vertices ||
        v < 0 || v >= res->num_vertices) {
        return -1;
    }

    if (res->dist[u][v] >= FW_INFINITY / 2.0 || res->next[u][v] == -1) {
        *path_len = 0;
        return -1;
    }

    int count = 0;
    int curr = u;
    path_out[count++] = curr;

    while (curr != v) {
        curr = res->next[curr][v];
        if (curr == -1 || count >= res->num_vertices) {
            *path_len = 0;
            return -1;
        }
        path_out[count++] = curr;
    }

    *path_len = count;
    return 0;
}

void print_floyd_warshall_matrix(const FloydWarshallResult* res, const Venue* venues, int venue_count) {
    if (!res || !res->dist) return;

    int V = res->num_vertices;
    printf("\n=== Floyd-Warshall All-Pairs Shortest Distances (km) ===\n");
    if (res->has_negative_cycle) {
        printf("⚠️ Warning: Graph contains a negative cycle!\n");
    }

    /* Print header */
    printf("%-18s", "");
    for (int j = 0; j < V; j++) {
        const char* name = (venues && j < venue_count) ? venues[j].name : "V";
        char short_name[9];
        snprintf(short_name, sizeof(short_name), "%.8s", name);
        printf("%9s", short_name);
    }
    printf("\n");

    for (int i = 0; i < V; i++) {
        const char* name = (venues && i < venue_count) ? venues[i].name : "V";
        printf("%-18.17s", name);
        for (int j = 0; j < V; j++) {
            if (res->dist[i][j] >= FW_INFINITY / 2.0) {
                printf("%9s", "INF");
            } else {
                printf("%9.1f", res->dist[i][j]);
            }
        }
        printf("\n");
    }
}

void free_floyd_warshall_result(FloydWarshallResult* res) {
    if (res && res->dist && res->next) {
        int V = res->num_vertices;
        for (int i = 0; i < V; i++) {
            free(res->dist[i]);
            free(res->next[i]);
        }
        free(res->dist);
        free(res->next);
        res->dist = NULL;
        res->next = NULL;
        res->num_vertices = 0;
        res->has_negative_cycle = 0;
    }
}
