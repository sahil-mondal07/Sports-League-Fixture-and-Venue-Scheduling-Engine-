#include "ford_fulkerson.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

FlowNetwork* flow_network_create(int num_vertices) {
    if (num_vertices <= 0 || num_vertices > MAX_FLOW_VERTICES) return NULL;
    FlowNetwork* net = (FlowNetwork*)malloc(sizeof(FlowNetwork));
    if (!net) return NULL;

    net->num_vertices = num_vertices;
    memset(net->capacity, 0, sizeof(net->capacity));
    memset(net->flow, 0, sizeof(net->flow));
    return net;
}

void flow_network_add_edge(FlowNetwork* net, int u, int v, double capacity) {
    if (!net || u < 0 || u >= net->num_vertices || v < 0 || v >= net->num_vertices) return;
    net->capacity[u][v] += capacity; /* allow parallel edges by summing */
}

void flow_network_free(FlowNetwork* net) {
    free(net);
}

/* BFS to find augmenting path in residual graph (Edmonds-Karp) */
static int bfs_augmenting_path(int V, const double capacity[MAX_FLOW_VERTICES][MAX_FLOW_VERTICES],
                               const double flow[MAX_FLOW_VERTICES][MAX_FLOW_VERTICES],
                               int s, int t, int* parent) {
    int* queue = (int*)malloc((size_t)V * sizeof(int));
    int* visited = (int*)calloc((size_t)V, sizeof(int));
    if (!queue || !visited) {
        free(queue);
        free(visited);
        return 0;
    }

    int head = 0, tail = 0;
    queue[tail++] = s;
    visited[s] = 1;
    parent[s] = -1;

    int found = 0;
    while (head < tail) {
        int u = queue[head++];
        if (u == t) {
            found = 1;
            break;
        }

        for (int v = 0; v < V; v++) {
            /* Residual capacity = capacity[u][v] - flow[u][v] */
            if (!visited[v] && capacity[u][v] - flow[u][v] > 1e-9) {
                visited[v] = 1;
                parent[v] = u;
                queue[tail++] = v;
            }
        }
    }

    free(queue);
    free(visited);
    return found;
}

FlowResult ford_fulkerson(const FlowNetwork* net, int source, int sink) {
    FlowResult res;
    res.max_flow_value = 0.0;
    memset(res.flow, 0, sizeof(res.flow));
    res.num_vertices = 0;
    res.source = source;
    res.sink = sink;

    if (!net || source < 0 || source >= net->num_vertices ||
        sink < 0 || sink >= net->num_vertices || source == sink) {
        return res;
    }

    int V = net->num_vertices;
    res.num_vertices = V;

    /* Copy capacity matrix to local residual flow tracking */
    double cap[MAX_FLOW_VERTICES][MAX_FLOW_VERTICES];
    double f[MAX_FLOW_VERTICES][MAX_FLOW_VERTICES];
    memcpy(cap, net->capacity, sizeof(cap));
    memset(f, 0, sizeof(f));

    int* parent = (int*)malloc((size_t)V * sizeof(int));
    if (!parent) return res;

    double max_flow = 0.0;

    /* Augment flow while path exists from source to sink in residual graph */
    while (bfs_augmenting_path(V, cap, f, source, sink, parent)) {
        /* Find bottleneck capacity along the augmenting path */
        double path_flow = 1e30;
        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            double residual = cap[u][v] - f[u][v];
            if (residual < path_flow) {
                path_flow = residual;
            }
        }

        /* Update flow along path and reverse edges */
        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            f[u][v] += path_flow;
            f[v][u] -= path_flow; /* reverse residual flow */
        }

        max_flow += path_flow;
    }

    res.max_flow_value = max_flow;
    memcpy(res.flow, f, sizeof(f));

    free(parent);
    return res;
}

void print_flow_result(const FlowResult* res) {
    if (!res) return;
    printf("\n=== Ford-Fulkerson Maximum Flow Result ===\n");
    printf("Source: %d, Sink: %d, Max Flow Throughput: %.2f\n",
           res->source, res->sink, res->max_flow_value);
    printf("Active Flow Edges (Flow > 0):\n");
    for (int u = 0; u < res->num_vertices; u++) {
        for (int v = 0; v < res->num_vertices; v++) {
            if (res->flow[u][v] > 1e-9) {
                printf("  Node %2d -> Node %2d : Flow = %.2f\n", u, v, res->flow[u][v]);
            }
        }
    }
}
