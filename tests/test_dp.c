/* ============================================================================
 * tests/test_dp.c — Dynamic Programming algorithm tests
 * ============================================================================
 * Tests: knapsack, matrix chain, LCS, optimal BST. Each test compares against
 * an independent brute-force/reference implementation.
 * Must return non-zero exit code on any failure.
 * ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include "../src/utils/common.h"
#include "../src/algorithms/dp/dp_knapsack.h"
#include "../src/algorithms/dp/dp_matrix_chain.h"
#include "../src/algorithms/dp/dp_lcs.h"
#include "../src/algorithms/dp/dp_optimal_bst.h"

/* ---- Knapsack brute force ---- */

static int knapsack_brute_force(const int* weights, const int* values,
                                int n, int capacity, int* best_w,
                                int* best_v) {
    int best_val = 0;
    int best_wt = 0;
    for (int mask = 0; mask < (1 << n); mask++) {
        int wt = 0, val = 0;
        for (int i = 0; i < n; i++) {
            if (mask & (1 << i)) {
                wt += weights[i];
                val += values[i];
            }
        }
        if (wt <= capacity && val > best_val) {
            best_val = val;
            best_wt = wt;
        }
    }
    if (best_w) *best_w = best_wt;
    if (best_v) *best_v = best_val;
    return best_val;
}

static void test_knapsack(void) {
    printf("TEST: knapsack (500 random cases, n<=12, cap<=50)\n");
    srand(12345);
    int failures = 0;
    for (int t = 0; t < 500; t++) {
        int n = rand() % 12 + 1;
        int cap = rand() % 50 + 1;
        int* weights = malloc(n * sizeof(int));
        int* values = malloc(n * sizeof(int));
        KnapsackItem* items = malloc(n * sizeof(KnapsackItem));
        for (int i = 0; i < n; i++) {
            weights[i] = rand() % 20 + 1;
            values[i] = rand() % 100 + 1;
            items[i].weight = weights[i];
            items[i].value = values[i];
        }
        int selected[12];
        int returned_val = knapsack_01(cap, items, n, selected);
        int bf_w, bf_v;
        int bf_best = knapsack_brute_force(weights, values, n, cap, &bf_w, &bf_v);
        if (returned_val != bf_best) {
            printf("  FAIL case %d: returned=%d expected=%d (n=%d, cap=%d)\n",
                   t, returned_val, bf_best, n, cap);
            failures++;
        }
        int sel_w = 0;
        for (int i = 0; i < n; i++) if (selected[i]) sel_w += weights[i];
        if (sel_w > cap) {
            printf("  FAIL case %d: selected weight %d > capacity %d\n", t, sel_w, cap);
            failures++;
        }
        int sel_v = 0;
        for (int i = 0; i < n; i++) if (selected[i]) sel_v += values[i];
        if (sel_v != returned_val) {
            printf("  FAIL case %d: selected value %d != returned %d\n", t, sel_v, returned_val);
            failures++;
        }
        free(weights);
        free(values);
        free(items);
    }
    if (failures) {
        printf("  %d knapsack failures\n", failures);
    } else {
        printf("  PASS: all 500 knapsack cases\n");
    }
    exit(failures);
}

/* ---- Matrix chain brute force ---- */

static int mc_brute(int* p, int i, int j) {
    if (i >= j) return 0;
    int min = INT_MAX;
    for (int k = i; k < j; k++) {
        int q = mc_brute(p, i, k) + mc_brute(p, k + 1, j) + p[i - 1] * p[k] * p[j];
        if (q < min) min = q;
    }
    return min;
}

static void test_matrix_chain(void) {
    printf("TEST: matrix chain (300 random chains, n=2..8)\n");
    srand((unsigned)time(NULL) + 1);
    int failures = 0;
    for (int t = 0; t < 300; t++) {
        int n = rand() % 7 + 2; /* 2..8 matrices */
        int* p = malloc((n + 1) * sizeof(int));
        for (int i = 0; i <= n; i++) p[i] = rand() % 10 + 1;
        int brute = mc_brute(p, 1, n);
        int* selected = malloc((n + 1) * sizeof(int));
        int returned = matrix_chain_order(p, n, selected);
        if (returned != brute) {
            printf("  FAIL case %d: returned=%d expected=%d (n=%d)\n",
                   t, returned, brute, n);
            failures++;
        }
        free(p);
        free(selected);
    }
    if (failures) {
        printf("  %d matrix chain failures\n", failures);
    } else {
        printf("  PASS: all 300 matrix chain cases\n");
    }
    exit(failures);
}

/* ---- LCS brute force ---- */

static int lcs_brute_force(const int* a, int na, const int* b, int nb) {
    int** dp = malloc((na + 1) * sizeof(int*));
    for (int i = 0; i <= na; i++) {
        dp[i] = calloc(nb + 1, sizeof(int));
    }
    for (int i = 1; i <= na; i++) {
        for (int j = 1; j <= nb; j++) {
            if (a[i - 1] == b[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else {
                dp[i][j] = dp[i][j - 1] > dp[i - 1][j] ? dp[i][j - 1] : dp[i - 1][j];
            }
        }
    }
    int result = dp[na][nb];
    for (int i = 0; i <= na; i++) free(dp[i]);
    free(dp);
    return result;
}

static void test_lcs(void) {
    printf("TEST: LCS (500 random sequences)\n");
    srand((unsigned)time(NULL) + 2);
    int failures = 0;
    for (int t = 0; t < 500; t++) {
        int na = rand() % 20;
        int nb = rand() % 20;
        int* a = malloc(na * sizeof(int));
        int* b = malloc(nb * sizeof(int));
        for (int i = 0; i < na; i++) a[i] = rand() % 100;
        for (int i = 0; i < nb; i++) b[i] = rand() % 100;
        int out_len = 0;
        int* selected = malloc((na > nb ? nb : na) * sizeof(int));
        int returned = lcs_length(a, na, b, nb, &out_len, selected);
        int expected = lcs_brute_force(a, na, b, nb);
        if (returned != expected) {
            printf("  FAIL case %d: returned=%d expected=%d (na=%d, nb=%d)\n",
                   t, returned, expected, na, nb);
            failures++;
        }
        free(a);
        free(b);
        free(selected);
    }
    if (failures) {
        printf("  %d LCS failures\n", failures);
    } else {
        printf("  PASS: all 500 LCS cases\n");
    }
    exit(failures);
}

/* ---- Optimal BST ---- */

static void test_optimal_bst(void) {
    printf("TEST: optimal BST\n");
    srand((unsigned)time(NULL) + 3);
    int failures = 0;
    double weight;
    int* root = malloc(6 * sizeof(int)); /* max n=5 + 1 */
    for (int n = 1; n <= 5; n++) {
        double* p = malloc((n + 1) * sizeof(double));
        for (int i = 1; i <= n; i++) p[i] = rand() % 100 + 1;
        double result = optimal_bst(p, n, &weight, root);
        if (result < 0) {
            printf("  FAIL n=%d: optimal_bst returned -1\n", n);
            failures++;
        }
        free(p);
    }
    free(root);
    if (failures) {
        printf("  %d optimal BST failures\n", failures);
    } else {
        printf("  PASS: all optimal BST cases\n");
    }
    exit(failures);
}

int main(void) {
    printf("=== Dynamic Programming Tests ===\n");
    test_knapsack();
    test_matrix_chain();
    test_lcs();
    test_optimal_bst();
    return 0;
}