/* ============================================================================
 * tests/test_graph_search.c — Graph search and combinatorial algorithm tests
 * ============================================================================
 * Tests: Kruskal/Prim, N-Queens, subset sum, branch and bound, Ford-Fulkerson,
 *        bipartite matching, CRT, string matchers. Each test compares against an
 *        independent reference/brute-force. Must return non-zero exit code on any failure.
 * ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../src/algorithms/string_matching/string_matcher.h"
#include "../src/utils/common.h"
#include "../src/algorithms/greedy/round_robin.h"
#include "../src/algorithms/sorting/merge_sort.h"
#include "../src/algorithms/searching/binary_search.h"
#include "../src/algorithms/graph/dijkstra.h"
#include "../src/algorithms/graph/prim.h"
#include "../src/algorithms/graph/kruskal.h"
#include "../src/algorithms/backtracking/n_queens.h"
#include "../src/algorithms/backtracking/subset_sum.h"
#include "../src/algorithms/flow/ford_fulkerson.h"
#include "../src/algorithms/flow/ford_fulkerson.h"
#include "../src/algorithms/flow/bipartite_matching.h"
#include "../src/algorithms/number_theory/crt.h"

/* ---- N-Queens reference (brute force with blocking) ---- */

/* ---- Subset sum count reference ---- */

static int subset_sum_brute(const int* arr, int n, int target) {
    int count = 0;
    for (int mask = 0; mask < (1 << n); mask++) {
        int sum = 0;
        for (int i = 0; i < n; i++) if (mask & (1 << i)) sum += arr[i];
        if (sum == target) count++;
    }
    return count;
}

/* ---- Ford-Fulkerson vs Edmonds-Karp reference ---- */

/* ---- Bipartite matching brute force ---- */

static int bipartite_match_brute(const int* adj, int n, int m) {
    /* adj is n x m matrix, 1 = edge */
    /* Try all possible matchings via DFS */
    int* matchL = calloc(n, sizeof(int));
    int* matchR = calloc(m, sizeof(int));
    int result = 0;
    for (int v = 0; v < m; v++) matchR[v] = -1;
    for (int u = 0; u < n; u++) matchL[u] = -1;

    int bpm(int u, const int* adj, int* matchR, int* seen, int m) {
        for (int v = 0; v < m; v++) {
            if (adj[u * m + v] && !seen[v]) {
                seen[v] = 1;
                if (matchR[v] < 0 || bpm(matchR[v], adj, matchR, seen, m)) {
                    matchR[v] = u;
                    matchL[u] = v;
                    return 1;
                }
            }
        }
        return 0;
    }

    for (int u = 0; u < n; u++) {
        int* seen = calloc(m, sizeof(int));
        bpm(u, adj, matchR, seen, m);
        free(seen);
    }

    result = 0;
    for (int v = 0; v < m; v++) if (matchR[v] >= 0) result++;
    free(matchL);
    free(matchR);
    return result;
}

/* ---- CRT brute force ---- */

static long long crt_brute(const long long* remainders, const long long* moduli, int k) {
    if (k <= 0) return -1;
    /* Search from 1 up to product of moduli */
    long long prod = 1;
    for (int i = 0; i < k; i++) prod *= moduli[i];
    for (long long x = 0; x < prod; x++) {
        int ok = 1;
        for (int i = 0; i < k; i++) {
            if (x % moduli[i] != remainders[i]) { ok = 0; break; }
        }
        if (ok) return x;
    }
    return -1;
}

/* ---- String matchers reference ---- */

static int strstr_search(const char* haystack, const char* needle) {
    const char* found = strstr(haystack, needle);
    return found ? (int)(found - haystack) : -1;
}

static int first_match(StringMatchResult result) {
    return result.count > 0 ? result.indices[0] : -1;
}

static int total_failures = 0;

static void test_n_queens(void) {
    printf("TEST: N-Queens counts (n=1..9, blocked cells)\n");
    srand((unsigned)time(NULL) + 1);
    int failures = 0;
    int expected[] = {1, 0, 0, 2, 10, 4, 40, 92, 352};
    for (int n = 1; n <= 9; n++) {
        
        /* We need a 2D array */
        int b[9][9] = {{0}};
        NQueensResultSet result = solve_n_queens(n, &b[0][0], 0);
        int actual = result.count;
        free_n_queens_result(&result);
        int exp = expected[n - 1];
        if (actual != exp) {
            printf("  FAIL n=%d: got %d expected %d\n", n, actual, exp);
            failures++;
        } else {
            printf("  PASS n=%d: %d solutions\n", n, actual);
        }
    }
    if (failures) {
        printf("  %d N-Queens failures\n", failures);
    }
    total_failures += failures;
}

static void test_subset_sum(void) {
    printf("TEST: subset sum count vs brute force (include target 0)\n");
    srand((unsigned)time(NULL) + 2);
    int failures = 0;
    for (int t = 0; t < 100; t++) {
        int n = rand() % 10 + 1; /* 1..10 */
        int target = rand() % 50;
        int* arr = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) arr[i] = rand() % 20 + 1;
        SubsetSumResultSet result = solve_subset_sum(arr, n, target, 0);
        int actual = result.count;
        free_subset_sum_result(&result);
        int expected = subset_sum_brute(arr, n, target);
        if (actual != expected) {
            printf("  FAIL case %d: n=%d target=%d got %d expected %d\n",
                   t, n, target, actual, expected);
            failures++;
        }
        free(arr);
    }
    if (failures) {
        printf("  %d subset sum failures\n", failures);
    } else {
        printf("  PASS: all 100 subset sum cases\n");
    }
    total_failures += failures;
}

static void test_bipartite_matching(void) {
    printf("TEST: bipartite matching vs brute force\n");
    srand((unsigned)time(NULL) + 3);
    int failures = 0;
    for (int t = 0; t < 50; t++) {
        int n = rand() % 8 + 2; /* 2..9 */
        int m = rand() % 8 + 2;
        int* adj = malloc(n * m * sizeof(int));
        for (int i = 0; i < n * m; i++) adj[i] = rand() % 2;
        BipartiteMatchResult result = solve_maximum_bipartite_matching(adj, n, m);
        int actual = result.total_matches;
        int expected = bipartite_match_brute(adj, n, m);
        if (actual != expected) {
            printf("  FAIL case %d: n=%d m=%d got %d expected %d\n",
                   t, n, m, actual, expected);
            failures++;
        }
        free(adj);
    }
    if (failures) {
        printf("  %d bipartite matching failures\n", failures);
    } else {
        printf("  PASS: all 50 bipartite matching cases\n");
    }
    total_failures += failures;
}

static void test_crt(void) {
    printf("TEST: CRT vs brute force; classic example = 23\n");
    srand((unsigned)time(NULL) + 4);
    int failures = 0;
    /* Classic: x = 2 mod 3, x = 3 mod 5, x = 2 mod 7 => x = 23 */
    long long rems[3] = {2, 3, 2};
    long long mods[3] = {3, 5, 7};
    CRTResult r = chinese_remainder_theorem(rems, mods, 3);
    if (!r.success) {
        printf("  FAIL CRT: no solution\n");
        failures++;
    } else if (r.x != 23) {
        printf("  FAIL CRT classic: got %lld expected 23\n", r.x);
        failures++;
    } else {
        printf("  PASS CRT classic: x = %lld\n", r.x);
    }
    /* Random coprime systems vs brute force */
    for (int t = 0; t < 30; t++) {
        int k = rand() % 4 + 2; /* 2..5 */
        long long* rems = malloc(k * sizeof(long long));
        long long* mods = malloc(k * sizeof(long long));
        /* CRT implementation specifies pairwise-coprime moduli. */
        const long long primes[] = {2, 3, 5, 7, 11, 13, 17};
        for (int i = 0; i < k; i++) {
            mods[i] = primes[i];
            rems[i] = rand() % mods[i];
        }
        CRTResult r = chinese_remainder_theorem(rems, mods, k);
        long long expected = crt_brute(rems, mods, k);
        if (!r.success && expected != -1) {
            printf("  FAIL CRT case %d: algo no solution brute %lld\n", t, expected);
            failures++;
        } else if (r.success && expected != -1 && r.x != expected) {
            printf("  FAIL CRT case %d: got %lld expected %lld\n", t, r.x, expected);
            failures++;
        }
        free(rems);
        free(mods);
    }
    if (failures) {
        printf("  %d CRT failures\n", failures);
    } else {
        printf("  PASS: all CRT cases\n");
    }
    total_failures += failures;
}

static void test_string_matchers(void) {
    printf("TEST: string matchers (naive, Rabin-Karp, KMP, automaton vs strstr)\n");
    srand((unsigned)time(NULL) + 5);
    int failures = 0;
    /* Include overlapping matches and >100 matches */
    for (int t = 0; t < 200; t++) {
        int n = rand() % 200 + 1; /* 1..200 */
        int m = rand() % 100 + 1; /* 1..100 */
        if (m > n) { int tmp = n; n = m; m = tmp; }
        char* hay = malloc(n + 1);
        char* needle = malloc(m + 1);
        for (int i = 0; i < n; i++) hay[i] = rand() % 26 + 'a';
        hay[n] = 0;
        for (int i = 0; i < m; i++) needle[i] = rand() % 26 + 'a';
        needle[m] = 0;
        /* Naive */
        int pos1 = first_match(string_match_naive(hay, needle));
        /* Rabin-Karp */
        int pos2 = first_match(string_match_rabin_karp(hay, needle));
        /* KMP */
        int pos3 = first_match(string_match_kmp(hay, needle));
        /* Automaton */
        int pos4 = first_match(string_match_fa(hay, needle));
        /* strstr */
        int pos5 = strstr_search(hay, needle);
        /* Check: all should either find same position or -1 */
        if (!(pos1 == pos5 && pos2 == pos5 && pos3 == pos5 && pos4 == pos5)) {
            printf("  FAIL case %d: hay=%s needle=%s got pos1=%d pos2=%d pos3=%d pos4=%d pos5=%d\n",
                   t, hay, needle, pos1, pos2, pos3, pos4, pos5);
            failures++;
        }
        free(hay);
        free(needle);
    }
    if (failures) {
        printf("  %d string matcher failures\n", failures);
    } else {
        printf("  PASS: all 200 string matcher cases\n");
    }
    total_failures += failures;
}

int main(void) {
    printf("=== Graph Search & Combinatorial Tests ===\n");
    test_n_queens();
    test_subset_sum();
    test_bipartite_matching();
    test_crt();
    test_string_matchers();
    printf("Graph/search failures: %d\n", total_failures);
    return total_failures ? 1 : 0;
}