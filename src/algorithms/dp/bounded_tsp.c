#include "bounded_tsp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Nearest-Neighbor + 2-Opt heuristic for n > TSP_MAX_EXACT_VERTICES */
static TSPResult solve_tsp_heuristic(const double* dist, int n, int start_node) {
    TSPResult res;
    res.tour = (int*)malloc((size_t)(n + 1) * sizeof(int));
    res.tour_len = n + 1;
    res.total_cost = 0.0;
    res.is_exact = 0;

    if (!res.tour) return res;

    int* visited = (int*)calloc((size_t)n, sizeof(int));
    if (!visited) {
        free(res.tour);
        res.tour = NULL;
        return res;
    }

    res.tour[0] = start_node;
    visited[start_node] = 1;
    int curr = start_node;

    for (int step = 1; step < n; step++) {
        int best_next = -1;
        double best_dist = TSP_INFINITY;
        for (int v = 0; v < n; v++) {
            if (!visited[v] && dist[curr * n + v] < best_dist) {
                best_dist = dist[curr * n + v];
                best_next = v;
            }
        }
        if (best_next == -1) {
            /* Fallback to any unvisited */
            for (int v = 0; v < n; v++) {
                if (!visited[v]) { best_next = v; break; }
            }
        }
        visited[best_next] = 1;
        res.tour[step] = best_next;
        res.total_cost += dist[curr * n + best_next];
        curr = best_next;
    }

    res.tour[n] = start_node;
    res.total_cost += dist[curr * n + start_node];
    free(visited);

    /* 2-Opt local search improvement passes */
    int improved = 1;
    int max_passes = 50;
    while (improved && max_passes-- > 0) {
        improved = 0;
        for (int i = 1; i < n - 1; i++) {
            for (int j = i + 1; j < n; j++) {
                int a = res.tour[i - 1];
                int b = res.tour[i];
                int c = res.tour[j];
                int d = res.tour[j + 1];

                double current_d = dist[a * n + b] + dist[c * n + d];
                double new_d = dist[a * n + c] + dist[b * n + d];

                if (new_d < current_d - 1e-6) {
                    /* Reverse segment [i..j] */
                    int left = i, right = j;
                    while (left < right) {
                        int tmp = res.tour[left];
                        res.tour[left] = res.tour[right];
                        res.tour[right] = tmp;
                        left++;
                        right--;
                    }
                    res.total_cost += (new_d - current_d);
                    improved = 1;
                }
            }
        }
    }

    return res;
}

TSPResult solve_bounded_tsp(const double* dist_matrix, int n, int start_node) {
    TSPResult res;
    res.tour = NULL;
    res.tour_len = 0;
    res.total_cost = TSP_INFINITY;
    res.is_exact = 0;

    if (!dist_matrix || n <= 0 || start_node < 0 || start_node >= n) {
        return res;
    }

    /* Single vertex trivial tour */
    if (n == 1) {
        res.tour = (int*)malloc(2 * sizeof(int));
        if (res.tour) {
            res.tour[0] = start_node;
            res.tour[1] = start_node;
            res.tour_len = 2;
            res.total_cost = 0.0;
            res.is_exact = 1;
        }
        return res;
    }

    /* Two vertices trivial tour */
    if (n == 2) {
        res.tour = (int*)malloc(3 * sizeof(int));
        if (res.tour) {
            int other = (start_node == 0) ? 1 : 0;
            res.tour[0] = start_node;
            res.tour[1] = other;
            res.tour[2] = start_node;
            res.tour_len = 3;
            res.total_cost = dist_matrix[start_node * 2 + other] + dist_matrix[other * 2 + start_node];
            res.is_exact = 1;
        }
        return res;
    }

    /* If problem size exceeds exact threshold, use 2-opt heuristic */
    if (n > TSP_MAX_EXACT_VERTICES) {
        return solve_tsp_heuristic(dist_matrix, n, start_node);
    }

    /* Exact Held-Karp DP:
       mask has 1 << n states.
       dp[mask * n + u] = min cost to visit subset `mask` ending at vertex `u`.
       parent[mask * n + u] = previous vertex `v` for path traceback. */
    size_t num_masks = (size_t)1 << n;
    size_t table_size = num_masks * (size_t)n;

    double* dp = (double*)malloc(table_size * sizeof(double));
    int* parent = (int*)malloc(table_size * sizeof(int));

    if (!dp || !parent) {
        free(dp);
        free(parent);
        return solve_tsp_heuristic(dist_matrix, n, start_node);
    }

    for (size_t i = 0; i < table_size; i++) {
        dp[i] = TSP_INFINITY;
        parent[i] = -1;
    }

    /* Base case: only start_node visited */
    size_t start_mask = (size_t)1 << start_node;
    dp[start_mask * (size_t)n + (size_t)start_node] = 0.0;

    /* Iterate through all bitmasks with increasing popcounts */
    for (size_t mask = 1; mask < num_masks; mask++) {
        /* Only consider masks that include the start_node */
        if (!(mask & start_mask)) continue;

        for (int u = 0; u < n; u++) {
            if (!(mask & ((size_t)1 << u))) continue;

            double curr_cost = dp[mask * (size_t)n + (size_t)u];
            if (curr_cost >= TSP_INFINITY / 2.0) continue;

            /* Extend path to an unvisited vertex v */
            for (int v = 0; v < n; v++) {
                if (mask & ((size_t)1 << v)) continue;

                size_t next_mask = mask | ((size_t)1 << v);
                double edge_cost = dist_matrix[u * n + v];
                double new_cost = curr_cost + edge_cost;

                size_t next_idx = next_mask * (size_t)n + (size_t)v;
                if (new_cost < dp[next_idx]) {
                    dp[next_idx] = new_cost;
                    parent[next_idx] = u;
                }
            }
        }
    }

    /* Find optimal closing edge back to start_node */
    size_t full_mask = num_masks - 1;
    double best_tour_cost = TSP_INFINITY;
    int last_node = -1;

    for (int u = 0; u < n; u++) {
        if (u == start_node) continue;
        double path_cost = dp[full_mask * (size_t)n + (size_t)u];
        double return_cost = dist_matrix[u * n + start_node];
        if (path_cost < TSP_INFINITY / 2.0 && path_cost + return_cost < best_tour_cost) {
            best_tour_cost = path_cost + return_cost;
            last_node = u;
        }
    }

    if (last_node == -1) {
        free(dp);
        free(parent);
        return solve_tsp_heuristic(dist_matrix, n, start_node);
    }

    /* Trace back the optimal tour */
    res.tour = (int*)malloc((size_t)(n + 1) * sizeof(int));
    if (!res.tour) {
        free(dp);
        free(parent);
        return res;
    }

    res.tour_len = n + 1;
    res.total_cost = best_tour_cost;
    res.is_exact = 1;

    res.tour[n] = start_node;
    int curr_node = last_node;
    size_t curr_mask = full_mask;

    for (int step = n - 1; step >= 1; step--) {
        res.tour[step] = curr_node;
        int prev_node = parent[curr_mask * (size_t)n + (size_t)curr_node];
        curr_mask ^= ((size_t)1 << curr_node);
        curr_node = prev_node;
    }
    res.tour[0] = start_node;

    free(dp);
    free(parent);
    return res;
}

void print_tsp_result(const TSPResult* res, const Venue* venues, int venue_count) {
    if (!res || !res->tour || res->tour_len <= 0) return;

    printf("\n=== Bounded TSP Tour Result (%s) ===\n",
           res->is_exact ? "Held-Karp Exact DP" : "Nearest-Neighbor + 2-Opt Heuristic");
    printf("Total Tour Distance: %.2f km\n", res->total_cost);
    printf("Tour Stops (%d venues closed circuit):\n", res->tour_len - 1);

    for (int i = 0; i < res->tour_len; i++) {
        int v = res->tour[i];
        const char* name = (venues && v < venue_count) ? venues[v].name : "Venue";
        printf("  %2d. [%-25s]%s", i + 1, name, (i < res->tour_len - 1) ? "\n" : " (Circuit Closed)\n");
    }
}

void free_tsp_result(TSPResult* res) {
    if (res) {
        free(res->tour);
        res->tour = NULL;
        res->tour_len = 0;
        res->total_cost = TSP_INFINITY;
        res->is_exact = 0;
    }
}
