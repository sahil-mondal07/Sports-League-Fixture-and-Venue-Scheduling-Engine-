#include "bipartite_matching.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* DFS augmenting path helper for bipartite matching */
static int dfs_bpm(int u, int num_venues, const int* adj_matrix,
                   int* match_v, int* visited) {
    for (int v = 0; v < num_venues; v++) {
        if (adj_matrix[u * num_venues + v] && !visited[v]) {
            visited[v] = 1;

            if (match_v[v] < 0 || dfs_bpm(match_v[v], num_venues, adj_matrix, match_v, visited)) {
                match_v[v] = u;
                return 1;
            }
        }
    }
    return 0;
}

BipartiteMatchResult solve_maximum_bipartite_matching(const int* adj_matrix, int num_teams, int num_venues) {
    BipartiteMatchResult res;
    for (int i = 0; i < MAX_BMP_NODES; i++) {
        res.match_u[i] = -1;
        res.match_v[i] = -1;
    }
    res.total_matches = 0;
    res.num_teams = num_teams;
    res.num_venues = num_venues;

    if (!adj_matrix || num_teams <= 0 || num_venues <= 0 ||
        num_teams > MAX_BMP_NODES || num_venues > MAX_BMP_NODES) {
        return res;
    }

    int match_v[MAX_BMP_NODES];
    for (int i = 0; i < MAX_BMP_NODES; i++) {
        match_v[i] = -1;
    }

    int total = 0;
    int* visited = (int*)malloc((size_t)num_venues * sizeof(int));
    if (!visited) return res;

    for (int u = 0; u < num_teams; u++) {
        memset(visited, 0, (size_t)num_venues * sizeof(int));
        if (dfs_bpm(u, num_venues, adj_matrix, match_v, visited)) {
            total++;
        }
    }

    res.total_matches = total;
    for (int v = 0; v < num_venues; v++) {
        res.match_v[v] = match_v[v];
        if (match_v[v] != -1) {
            res.match_u[match_v[v]] = v;
        }
    }

    free(visited);
    return res;
}

void print_bipartite_match_result(const BipartiteMatchResult* res) {
    if (!res) return;
    printf("\n=== Maximum Bipartite Matching Result ===\n");
    printf("Total Matched Teams: %d / %d\n", res->total_matches, res->num_teams);
    for (int u = 0; u < res->num_teams; u++) {
        if (res->match_u[u] != -1) {
            printf("  Team %2d -> Venue %2d\n", u, res->match_u[u]);
        } else {
            printf("  Team %2d -> Unmatched\n", u);
        }
    }
}
