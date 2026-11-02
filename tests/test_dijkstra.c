#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../src/data/graph.h"
#include "../src/algorithms/graph/dijkstra.h"

static int test_dijkstra_simple() {
    printf("TEST 1: Simple 4-node weighted graph\n");
    /*
       (0) --1.0--> (1) --2.0--> (3)
        |                         ^
        +----------4.0------------+
       Shortest path 0->3 is 0->1->3 with weight 3.0
    */
    Graph* g = create_graph(4);
    add_edge(g, 0, 1, 1.0f);
    add_edge(g, 1, 3, 2.0f);
    add_edge(g, 0, 3, 4.0f);
    add_edge(g, 1, 2, 5.0f);

    DijkstraResult* res = dijkstra_shortest_path(g, 0);
    if (!res) {
        printf("FAIL: Dijkstra returned NULL\n");
        free_graph(g);
        return 0;
    }

    if (fabs(res->dist[3] - 3.0f) > 0.001f) {
        printf("FAIL: Expected dist[3] = 3.0, got %.2f\n", res->dist[3]);
        free_dijkstra_result(res);
        free_graph(g);
        return 0;
    }

    int path[4];
    int len = dijkstra_get_path(res, 3, path);
    if (len != 3 || path[0] != 0 || path[1] != 1 || path[2] != 3) {
        printf("FAIL: Path reconstruction incorrect\n");
        free_dijkstra_result(res);
        free_graph(g);
        return 0;
    }

    printf("PASS: Shortest path 0->3 is 3.0 (0 -> 1 -> 3)\n");
    free_dijkstra_result(res);
    free_graph(g);
    return 1;
}

static int test_dijkstra_unreachable() {
    printf("\nTEST 2: Disconnected vertex\n");
    Graph* g = create_graph(3);
    add_edge(g, 0, 1, 2.5f);
    /* Vertex 2 has no edges */

    DijkstraResult* res = dijkstra_shortest_path(g, 0);
    if (!res) {
        printf("FAIL: NULL result\n");
        free_graph(g);
        return 0;
    }

    if (res->dist[2] < DIJKSTRA_INF - 1.0f) {
        printf("FAIL: Expected vertex 2 to be INF\n");
        free_dijkstra_result(res);
        free_graph(g);
        return 0;
    }

    int path[3];
    int len = dijkstra_get_path(res, 2, path);
    if (len != 0) {
        printf("FAIL: Unreachable vertex should return path len 0\n");
        free_dijkstra_result(res);
        free_graph(g);
        return 0;
    }

    printf("PASS: Vertex 2 detected as unreachable\n");
    free_dijkstra_result(res);
    free_graph(g);
    return 1;
}

static int test_dijkstra_venues_graph() {
    printf("\nTEST 3: Realistic venue network (5 venues)\n");
    /*
       0: Camp Nou (Barcelona)
       1: Santiago Bernabeu (Madrid)
       2: Civitas Metropolitano (Madrid)
       3: Mestalla (Valencia)
       4: Ramon Sanchez Pizjuan (Seville)
    */
    Graph* g = create_graph(5);
    add_undirected_edge(g, 1, 2, 7.7f);     /* Bernabeu <-> Metropolitano */
    add_undirected_edge(g, 0, 3, 298.4f);   /* Barcelona <-> Valencia */
    add_undirected_edge(g, 1, 3, 302.1f);   /* Madrid <-> Valencia */
    add_undirected_edge(g, 1, 4, 394.2f);   /* Madrid <-> Seville */
    add_undirected_edge(g, 0, 1, 499.0f);   /* Direct Barcelona <-> Madrid: 499km */
    /* Barcelona -> Valencia (298.4) -> Madrid (302.1) = 600.5km vs direct 499km */

    DijkstraResult* res = dijkstra_shortest_path(g, 0);
    if (!res) {
        printf("FAIL: NULL result\n");
        free_graph(g);
        return 0;
    }

    /* Direct Barcelona to Bernabeu is 499.0 */
    if (fabs(res->dist[1] - 499.0f) > 0.1f) {
        printf("FAIL: Expected dist 499.0, got %.2f\n", res->dist[1]);
        free_dijkstra_result(res);
        free_graph(g);
        return 0;
    }

    /* Barcelona to Metropolitano is Barcelona -> Bernabeu -> Metropolitano = 499 + 7.7 = 506.7 */
    float expected_metro = 499.0f + 7.7f;
    if (fabs(res->dist[2] - expected_metro) > 0.1f) {
        printf("FAIL: Expected Barcelona->Metropolitano = %.1f, got %.2f\n", expected_metro, res->dist[2]);
        free_dijkstra_result(res);
        free_graph(g);
        return 0;
    }

    int path[5];
    int len = dijkstra_get_path(res, 2, path);
    if (len != 3 || path[0] != 0 || path[1] != 1 || path[2] != 2) {
        printf("FAIL: Path to Metropolitano should be 0 -> 1 -> 2\n");
        free_dijkstra_result(res);
        free_graph(g);
        return 0;
    }

    printf("PASS: Correct shortest route Barcelona -> Bernabeu -> Metropolitano (%.1f km)\n", expected_metro);
    free_dijkstra_result(res);
    free_graph(g);
    return 1;
}

int main() {
    int passes = 0, fails = 0;
    printf("========================================\n");
    printf("DIJKSTRA SHORTEST PATH TESTS\n");
    printf("========================================\n\n");

    if (test_dijkstra_simple()) passes++; else fails++;
    if (test_dijkstra_unreachable()) passes++; else fails++;
    if (test_dijkstra_venues_graph()) passes++; else fails++;

    printf("\n========================================\n");
    printf("RESULTS: %d passed, %d failed\n", passes, fails);
    printf("========================================\n");
    return (fails > 0) ? 1 : 0;
}
