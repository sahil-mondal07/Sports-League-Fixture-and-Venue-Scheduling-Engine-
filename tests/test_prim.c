#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../src/data/graph.h"
#include "../src/algorithms/graph/prim.h"

/* =========================================================================
 * Kruskal Reference Implementation for Verification
 *===========================================================================*/

typedef struct {
    int u;
    int v;
    float weight;
} KruskalEdge;

static int compare_edges(const void* a, const void* b) {
    const KruskalEdge* ea = (const KruskalEdge*)a;
    const KruskalEdge* eb = (const KruskalEdge*)b;
    if (ea->weight < eb->weight) return -1;
    if (ea->weight > eb->weight) return 1;
    return 0;
}

static int find_parent(int* parent, int i) {
    if (parent[i] == i) return i;
    return parent[i] = find_parent(parent, parent[i]);
}

static void union_sets(int* parent, int* rank, int x, int y) {
    int root_x = find_parent(parent, x);
    int root_y = find_parent(parent, y);
    if (root_x != root_y) {
        if (rank[root_x] < rank[root_y]) {
            parent[root_x] = root_y;
        } else if (rank[root_x] > rank[root_y]) {
            parent[root_y] = root_x;
        } else {
            parent[root_y] = root_x;
            rank[root_x]++;
        }
    }
}

static double kruskal_mst(const Graph* g) {
    int v_count = g->num_vertices;
    int max_edges = 0;
    for (int i = 0; i < v_count; i++) {
        Edge* e = g->adj_list[i];
        while (e) {
            if (i < e->dest) max_edges++;
            e = e->next;
        }
    }

    KruskalEdge* edges = (KruskalEdge*)malloc(sizeof(KruskalEdge) * (max_edges > 0 ? max_edges : 1));
    int edge_idx = 0;
    for (int i = 0; i < v_count; i++) {
        Edge* e = g->adj_list[i];
        while (e) {
            if (i < e->dest) {
                edges[edge_idx].u = i;
                edges[edge_idx].v = e->dest;
                edges[edge_idx].weight = e->weight;
                edge_idx++;
            }
            e = e->next;
        }
    }

    qsort(edges, edge_idx, sizeof(KruskalEdge), compare_edges);

    int* parent = (int*)malloc(sizeof(int) * v_count);
    int* rank = (int*)calloc(v_count, sizeof(int));
    for (int i = 0; i < v_count; i++) parent[i] = i;

    double mst_weight = 0.0;
    int edges_count = 0;

    for (int i = 0; i < edge_idx && edges_count < v_count - 1; i++) {
        int root_u = find_parent(parent, edges[i].u);
        int root_v = find_parent(parent, edges[i].v);

        if (root_u != root_v) {
            union_sets(parent, rank, root_u, root_v);
            mst_weight += (double)edges[i].weight;
            edges_count++;
        }
    }

    free(parent);
    free(rank);
    free(edges);
    return mst_weight;
}

/* =========================================================================
 * Tests
 *===========================================================================*/

static int test_200_random_graphs(void) {
    printf("TEST 1: 200 Random Graphs with Fractional Weights vs Kruskal\n");
    srand(42);
    for (int t = 0; t < 200; t++) {
        int num_v = 5 + (rand() % 16); /* 5 to 20 vertices */
        Graph* g = create_graph(num_v);

        /* Guarantee connectivity with initial tree */
        for (int i = 1; i < num_v; i++) {
            int target = rand() % i;
            float w = (float)((rand() % 400 + 1) / 16.0);
            add_undirected_edge(g, i, target, w);
        }

        /* Add extra edges */
        int extra_edges = rand() % (num_v * 2);
        for (int e = 0; e < extra_edges; e++) {
            int u = rand() % num_v;
            int v = rand() % num_v;
            if (u != v) {
                float w = (float)((rand() % 400 + 1) / 16.0);
                add_undirected_edge(g, u, v, w);
            }
        }

        PrimResult prim_res = prim_mst(g, 0);
        double kruskal_w = kruskal_mst(g);

        if (fabs(prim_res.total_weight - kruskal_w) > 0.001) {
            printf("FAIL on graph %d: Prim=%.4f vs Kruskal=%.4f\n", t, prim_res.total_weight, kruskal_w);
            free_prim_result(&prim_res);
            free_graph(g);
            return 0;
        }

        free_prim_result(&prim_res);
        free_graph(g);
    }
    printf("PASS: All 200 random graphs matched Kruskal reference\n");
    return 1;
}

static int test_laliga_mst(void) {
    printf("\nTEST 2: LaLiga Venue Network MST\n");
    FILE* fp = fopen("data/football/laliga_distances.csv", "r");
    if (!fp) {
        printf("FAIL: Cannot open data/football/laliga_distances.csv\n");
        return 0;
    }

    Graph* g = create_graph(20);
    char line[256];
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        free_graph(g);
        return 0;
    }

    while (fgets(line, sizeof(line), fp)) {
        int u, v;
        float dist;
        if (sscanf(line, "%d,%d,%f", &u, &v, &dist) == 3) {
            if (u >= 0 && u < 20 && v >= 0 && v < 20) {
                add_undirected_edge(g, u, v, dist);
            }
        }
    }
    fclose(fp);

    PrimResult res = prim_mst(g, 0);
    printf("LaLiga MST total weight: %.2f km\n", res.total_weight);

    double expected = 3553.4;
    double diff = fabs(res.total_weight - expected);
    int pass = (diff <= 0.1);
    if (pass) {
        printf("PASS: LaLiga MST is %.2f km (expected %.1f +/- 0.1 km)\n", res.total_weight, expected);
    } else {
        printf("FAIL: Expected %.1f +/- 0.1 km, got %.2f km\n", expected, res.total_weight);
    }

    free_prim_result(&res);
    free_graph(g);
    return pass;
}

static int test_disconnected_graph(void) {
    printf("\nTEST 3: Disconnected Graph (No Crash)\n");
    Graph* g = create_graph(5);
    add_undirected_edge(g, 0, 1, 10.0f);
    add_undirected_edge(g, 1, 2, 20.0f);
    /* Vertices 3 and 4 are isolated */

    PrimResult res = prim_mst(g, 0);
    printf("Disconnected graph processed: %d edges in component, weight %.2f\n",
           res.mst_edge_count, res.total_weight);

    free_prim_result(&res);
    free_graph(g);
    printf("PASS: Disconnected graph handled without crashing\n");
    return 1;
}

int main(void) {
    printf("========================================\n");
    printf("PRIM MINIMUM SPANNING TREE TESTS\n");
    printf("========================================\n\n");

    int passes = 0, fails = 0;
    if (test_200_random_graphs()) passes++; else fails++;
    if (test_laliga_mst()) passes++; else fails++;
    if (test_disconnected_graph()) passes++; else fails++;

    printf("\n========================================\n");
    printf("RESULTS: %d passed, %d failed\n", passes, fails);
    printf("========================================\n");

    return (fails > 0) ? 1 : 0;
}
