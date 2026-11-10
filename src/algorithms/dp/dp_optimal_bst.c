#include "dp_optimal_bst.h"
#include <stdlib.h>
#include <stdio.h>

double optimal_bst(const double* p, int n, double* out_weight, int* out_root) {
    if (n <= 0 || !p) {
        if (out_weight) *out_weight = 0.0;
        if (out_root) {
            /* Fill with 0s */
            for (int k = 0; k <= n; k++) out_root[k] = 0;
        }
        return -1.0;
    }

    /* Validate probabilities are non-negative */
    for (int i = 1; i <= n; i++) {
        if (p[i] < 0.0) {
            if (out_weight) *out_weight = 0.0;
            if (out_root) {
                for (int k = 0; k <= n; k++) out_root[k] = 0;
            }
            return -1.0;
        }
    }

    /* Compute w[i][j] on the fly using prefix sums.
       w[i][j] = sum_{k=i}^{j} p[k] */

    /* Allocate DP tables: e[i][j] for 1 <= i <= j <= n, plus e[i][i-1] */
    /* We'll allocate (n+2) x (n+2) to handle e[i][i-1] and 1-indexing conveniently.
       Index i ranges from 1..n, j from 0..n (where j < i means empty subtree). */
    double** e = (double**)malloc((size_t)(n + 2) * sizeof(double*));
    int** root = (int**)malloc((size_t)(n + 2) * sizeof(int*));
    if (!e || !root) {
        free(e); free(root);
        if (out_weight) *out_weight = 0.0;
        if (out_root) {
            for (int k = 0; k <= n; k++) out_root[k] = 0;
        }
        return -1.0;
    }

    for (int i = 0; i <= n + 1; i++) {
        e[i] = (double*)calloc((size_t)(n + 2), sizeof(double));
        root[i] = (int*)calloc((size_t)(n + 2), sizeof(int));
        if (!e[i] || !root[i]) {
            /* Free all so far */
            for (int k = 0; k <= i; k++) {
                free(e[k]);
                free(root[k]);
            }
            free(e); free(root);
            if (out_weight) *out_weight = 0.0;
            if (out_root) {
                for (int k = 0; k <= n; k++) out_root[k] = 0;
            }
            return -1.0;
        }
    }

    /* Initialize: e[i][i-1] = 0 for all i (empty subtree) */
    /* Already 0 from calloc, but be explicit */
    for (int i = 1; i <= n + 1; i++) {
        e[i][i - 1] = 0.0;
    }

    /* Build e[1..n][1..n] and root[1..n][1..n] */
    /* Chain length L = 1 means single key */
    for (int L = 1; L <= n; L++) {
        for (int i = 1; i <= n - L + 1; i++) {
            int j = i + L - 1;

            /* Compute w[i][j] = sum_{k=i}^{j} p[k] */
            double wij = 0.0;
            for (int k = i; k <= j; k++) {
                wij += p[k];
            }

            /* Initialize with r = i */
            double best = 1e100;
            int best_r = i;

            /* Try all possible roots r from i to j */
            for (int r = i; r <= j; r++) {
                /* Cost = e[i][r-1] + e[r+1][j] + wij */
                double left = (r - 1 >= i) ? e[i][r - 1] : 0.0;
                double right = (r + 1 <= j) ? e[r + 1][j] : 0.0;
                double cost = left + right + wij;

                if (cost < best) {
                    best = cost;
                    best_r = r;
                }
            }

            e[i][j] = best;
            root[i][j] = best_r;
        }
    }

    /* Compute total weight */
    double total_weight = 0.0;
    for (int k = 1; k <= n; k++) {
        total_weight += p[k];
    }
    if (out_weight) *out_weight = total_weight;

    if (out_root) {
        /* out_root[k] = root of optimal BST containing keys 1..k */
        out_root[0] = 0;
        for (int k = 1; k <= n; k++) {
            out_root[k] = root[1][k];
        }
    }

    double result_cost = e[1][n];

    /* Free tables */
    for (int i = 0; i <= n + 1; i++) {
        free(e[i]);
        free(root[i]);
    }
    free(e);
    free(root);

    return result_cost;
}