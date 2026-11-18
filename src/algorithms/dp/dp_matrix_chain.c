#include "dp_matrix_chain.h"
#include <stdlib.h>
#include <stdio.h>

/* ----------------------------------------------------------
 * Matrix Chain Multiplication DP implementation
 * ----------------------------------------------------------
 * We use the standard O(n^3) DP:
 * m[i][j] = minimum scalar multiplications needed to compute
 *           the product Ai × ... × Aj
 * s[i][j] = split point (k) that gives the optimal m[i][j]
 *
 * We implement this with 2D tables allocated as 1D arrays for
 * efficiency, but the interface uses clear 2D-like access macros.
 * ---------------------------------------------------------- */

#define MATRIX(m, i, j, cols) (m)[(i)*(cols)+(j)]

int matrix_chain_order(const int* p, int n, int* out_selected_out) {
    if (n <= 0 || !p) return -1;
    if (n == 1) {
        /* Only one matrix, no multiplications needed */
        if (out_selected_out) out_selected_out[0] = 0;
        return 0;
    }

    /* Tables: m[i][j] for 1 <= i <= j <= n-1 (1-indexed for convenience)
       We allocate (n+1) x (n+1) to make the 1-indexed math simpler.
       m has dimensions (n+1) x (n+1), s has dimensions (n+1) x (n+1). */
    int size = n + 1;
    int* m = (int*)calloc((size_t)(size * size), sizeof(int));
    int* s = (int*)calloc((size_t)(size * size), sizeof(int));

    if (!m || !s) {
        free(m);
        free(s);
        return -1;
    }

    /* m[i][j] will be filled for chain lengths L = 2 to n */
    /* i ranges from 1 to n-L+1, j = i+L-1 */
    /* We use 1-indexed for the algorithm but 0-indexed for the p array.
       Matrix i has dimensions p[i-1] x p[i] for i = 1..n */

    /* L = chain length */
    for (int L = 2; L <= n; L++) {
        /* i = start index (1-indexed) */
        for (int i = 1; i <= n - L + 1; i++) {
            int j = i + L - 1; /* j = end index (1-indexed) */

            /* Initialize m[i][j] to infinity */
            int best = 999999999;
            int best_k = i; /* default split */

            /* Try all split points k from i to j-1 */
            for (int k = i; k <= j - 1; k++) {
                /* Cost = m[i][k] + m[k+1][j] + p[i-1]*p[k]*p[j] */
                int q = MATRIX(m, i, k, size) + MATRIX(m, k + 1, j, size)
                        + p[i - 1] * p[k] * p[j];

                if (q < best) {
                    best = q;
                    best_k = k;
                }
            }

            MATRIX(m, i, j, size) = best;
            MATRIX(s, i, j, size) = best_k;
        }
    }

    /* Handle out_selected_out if provided.
     * The s table stores split points. We need to trace through
     * all subproblems to collect the full set of splits.
     * ---------------------------------------------------------- */
    if (out_selected_out) {
        /* We'll trace all splits recursively and store them.
           out_selected_out has room for n-1 split points (one per
           internal node in the binary tree of the chain). */
        int* splits = (int*)malloc((size_t)(n - 1) * sizeof(int));
        if (splits) {
            /* Trace all splits using a stack-based approach */
            /* We process subproblems (i, j) and collect s[i][j] */
            int split_count = 0;
            /* Stack of (i, j) pairs to process */
            int* stack_i = (int*)malloc((size_t)(n * n) * sizeof(int));
            int* stack_j = (int*)malloc((size_t)(n * n) * sizeof(int));
            if (stack_i && stack_j) {
                int si = 0; /* stack index */
                stack_i[si] = 1;
                stack_j[si] = n;
                si++;

                while (si > 0) {
                    si--;
                    int cur_i = stack_i[si];
                    int cur_j = stack_j[si];

                    if (cur_i < cur_j) {
                        int k = (int)MATRIX(s, cur_i, cur_j, size);
                        if (split_count < n - 1) {
                            splits[split_count++] = k;
                        }
                        /* Push right half first, then left half */
                        stack_i[si] = k + 1;
                        stack_j[si] = cur_j;
                        si++;
                        stack_i[si] = cur_i;
                        stack_j[si] = k;
                        si++;
                    }
                }
            }

            /* Copy splits to out_selected_out */
            int copy_count = split_count;
            if (copy_count > n - 1) copy_count = n - 1;
            for (int i = 0; i < copy_count; i++) {
                out_selected_out[i] = splits[i];
            }
            /* Fill remaining with 0 */
            for (int i = copy_count; i < n - 1; i++) {
                out_selected_out[i] = 0;
            }

            free(stack_i);
            free(stack_j);
            free(splits);
        } else {
            /* If malloc failed, fill with zeros */
            for (int i = 0; i < n - 1; i++) {
                out_selected_out[i] = 0;
            }
        }
    }

    int result = MATRIX(m, 1, n, size);

    free(m);
    free(s);

    /* Return the minimum cost: we computed it during DP */
    return result;
}
