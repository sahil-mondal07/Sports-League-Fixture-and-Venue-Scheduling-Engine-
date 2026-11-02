#include "dijkstra.h"
#include <stdlib.h>
#include <stdio.h>

/* Min-Heap Priority Queue item for Dijkstra */
typedef struct {
    int vertex;
    float dist;
} HeapNode;

typedef struct {
    HeapNode* nodes;
    int* pos;       /* Position of vertex in heap for decrease-key operations */
    int size;
    int capacity;
} MinHeap;

static MinHeap* create_min_heap(int capacity) {
    MinHeap* h = (MinHeap*)malloc(sizeof(MinHeap));
    if (!h) return NULL;

    h->nodes = (HeapNode*)malloc(sizeof(HeapNode) * capacity);
    h->pos = (int*)malloc(sizeof(int) * capacity);
    if (!h->nodes || !h->pos) {
        if (h->nodes) free(h->nodes);
        if (h->pos) free(h->pos);
        free(h);
        return NULL;
    }

    h->size = 0;
    h->capacity = capacity;
    for (int i = 0; i < capacity; i++) {
        h->pos[i] = -1;
    }
    return h;
}

static void free_min_heap(MinHeap* h) {
    if (!h) return;
    if (h->nodes) free(h->nodes);
    if (h->pos) free(h->pos);
    free(h);
}

static void heap_swap(MinHeap* h, int i, int j) {
    HeapNode temp = h->nodes[i];
    h->nodes[i] = h->nodes[j];
    h->nodes[j] = temp;

    h->pos[h->nodes[i].vertex] = i;
    h->pos[h->nodes[j].vertex] = j;
}

static void min_heapify(MinHeap* h, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < h->size && h->nodes[left].dist < h->nodes[smallest].dist) {
        smallest = left;
    }
    if (right < h->size && h->nodes[right].dist < h->nodes[smallest].dist) {
        smallest = right;
    }

    if (smallest != idx) {
        heap_swap(h, idx, smallest);
        min_heapify(h, smallest);
    }
}

static void decrease_key(MinHeap* h, int vertex, float dist) {
    int i = h->pos[vertex];
    h->nodes[i].dist = dist;

    while (i > 0 && h->nodes[i].dist < h->nodes[(i - 1) / 2].dist) {
        heap_swap(h, i, (i - 1) / 2);
        i = (i - 1) / 2;
    }
}

static HeapNode extract_min(MinHeap* h) {
    HeapNode root = h->nodes[0];
    HeapNode last = h->nodes[h->size - 1];

    h->nodes[0] = last;
    h->pos[root.vertex] = -1;
    h->pos[last.vertex] = 0;

    h->size--;
    if (h->size > 0) {
        min_heapify(h, 0);
    }
    return root;
}

static int is_in_min_heap(const MinHeap* h, int vertex) {
    return h->pos[vertex] >= 0;
}

DijkstraResult* dijkstra_shortest_path(const Graph* g, int source) {
    if (!g || source < 0 || source >= g->num_vertices) {
        return NULL;
    }

    int n = g->num_vertices;

    DijkstraResult* res = (DijkstraResult*)malloc(sizeof(DijkstraResult));
    if (!res) return NULL;

    res->num_vertices = n;
    res->source = source;
    res->dist = (float*)malloc(sizeof(float) * n);
    res->parent = (int*)malloc(sizeof(int) * n);
    if (!res->dist || !res->parent) {
        free_dijkstra_result(res);
        return NULL;
    }

    MinHeap* heap = create_min_heap(n);
    if (!heap) {
        free_dijkstra_result(res);
        return NULL;
    }

    for (int v = 0; v < n; v++) {
        res->dist[v] = DIJKSTRA_INF;
        res->parent[v] = -1;
        heap->nodes[v].vertex = v;
        heap->nodes[v].dist = DIJKSTRA_INF;
        heap->pos[v] = v;
    }

    res->dist[source] = 0.0f;
    heap->nodes[source].dist = 0.0f;
    heap->size = n;
    decrease_key(heap, source, 0.0f);

    while (heap->size > 0) {
        HeapNode min_node = extract_min(heap);
        int u = min_node.vertex;

        if (min_node.dist >= DIJKSTRA_INF) {
            break; /* Remaining vertices unreachable */
        }

        Edge* edge = g->adj_list[u];
        while (edge) {
            int v = edge->dest;
            float weight = edge->weight;

            if (is_in_min_heap(heap, v) &&
                res->dist[u] + weight < res->dist[v]) {
                res->dist[v] = res->dist[u] + weight;
                res->parent[v] = u;
                decrease_key(heap, v, res->dist[v]);
            }
            edge = edge->next;
        }
    }

    free_min_heap(heap);
    return res;
}

int dijkstra_get_path(const DijkstraResult* res, int target, int* path_out) {
    if (!res || !path_out || target < 0 || target >= res->num_vertices) {
        return 0;
    }
    if (res->dist[target] >= DIJKSTRA_INF) {
        return 0; /* Target is unreachable */
    }

    /* Traverse from target backwards to source */
    int temp[res->num_vertices];
    int count = 0;
    int curr = target;

    while (curr != -1) {
        temp[count++] = curr;
        if (curr == res->source) break;
        curr = res->parent[curr];
    }

    /* Reverse into path_out */
    for (int i = 0; i < count; i++) {
        path_out[i] = temp[count - 1 - i];
    }
    return count;
}

void print_dijkstra_result(const DijkstraResult* res) {
    if (!res) return;

    printf("\n=== Dijkstra Shortest Paths from Source Vertex %d ===\n", res->source);
    printf("%-8s %-12s %s\n", "Target", "Distance", "Path");
    printf("-----------------------------------------\n");

    int* path = (int*)malloc(sizeof(int) * res->num_vertices);
    if (!path) return;

    for (int v = 0; v < res->num_vertices; v++) {
        printf("%-8d ", v);
        if (res->dist[v] >= DIJKSTRA_INF) {
            printf("%-12s Unreachable\n", "INF");
        } else {
            printf("%-12.2f ", res->dist[v]);
            int path_len = dijkstra_get_path(res, v, path);
            for (int i = 0; i < path_len; i++) {
                printf("%d%s", path[i], (i == path_len - 1) ? "" : " -> ");
            }
            printf("\n");
        }
    }
    free(path);
    printf("=========================================\n\n");
}

void free_dijkstra_result(DijkstraResult* res) {
    if (!res) return;
    if (res->dist) free(res->dist);
    if (res->parent) free(res->parent);
    free(res);
}
