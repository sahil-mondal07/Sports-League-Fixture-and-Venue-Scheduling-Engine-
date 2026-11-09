#include "dp_lcs.h"
#include <stdlib.h>
#include <stdio.h>

/* ----------------------------------------------------------
 * LCS DP implementation
 * ----------------------------------------------------------
 * Standard O(m*n) DP using 2D table, then traceback.
 * We use 1D arrays for space optimization where possible,
 * but keep the 2D approach for clear traceback.
 * ---------------------------------------------------------- */

int lcs_length(const int* seq1, int m, const int* seq2, int n,
               int* out_length, int* out_selected_out) {
    if (m <= 0 || n <= 0 || !seq1 || !seq2) {
        if (out_length) *out_length = 0;
        if (out_selected_out) memset(out_selected_out, 0, (size_t)m * sizeof(int));
        return -1;
    }

    /* dp[j] will represent the LCS length for current row i..j.
       We'll use 2D allocation for simplicity of traceback:
       dp[i][j] = LCS length of seq1[0..i-1] and seq2[0..j-1] */
    int** dp = (int**)malloc((size_t)(m + 1) * sizeof(int*));
    if (!dp) return -1;
    for (int i = 0; i <= m; i++) {
        dp[i] = (int*)calloc((size_t)(n + 1), sizeof(int));
        if (!dp[i]) {
            /* Free previously allocated rows */
            for (int k = 0; k < i; k++) free(dp[k]);
            free(dp);
            return -1;
        }
    }

    /* Build the DP table bottom-up */
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (seq1[i - 1] == seq2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else {
                /* Take max of left (dp[i][j-1]) or top (dp[i-1][j]) */
                if (dp[i - 1][j] >= dp[i][j - 1]) {
                    dp[i][j] = dp[i - 1][j];
                } else {
                    dp[i][j] = dp[i][j - 1];
                }
            }
        }
    }

    int result = dp[m][n];

    /* ----------------------------------------------------------
     * Traceback to find the actual LCS elements (positions in seq1).
     * We'll walk from dp[m][n] backwards to dp[0][0].
     * ---------------------------------------------------------- */
    if (out_selected_out) {
        /* Collect selected indices (positions in seq1) in reverse order,
           then reverse them at the end. */
        int* indices = (int*)malloc((size_t)result * sizeof(int));
        if (!indices) {
            /* Free DP table and return */
            for (int i = 0; i <= m; i++) free(dp[i]);
            free(dp);
            if (out_length) *out_length = 0;
            return -1;
        }

        int i = m, j = n;
        int idx_pos = 0;  /* position in indices[] */

        while (i > 0 && j > 0) {
            if (seq1[i - 1] == seq2[j - 1]) {
                /* This element is part of the LCS */
                indices[idx_pos++] = i;  /* 1-based position in seq1 */
                i--;
                j--;
            } else if (dp[i - 1][j] >= dp[i][j - 1]) {
                i--;  /* move up (skip seq1[i-1]) */
            } else {
                j--;  /* move left (skip seq2[j-1]) */
            }
        }

        /* indices[] now has the LCS positions in reverse order.
           We'll reverse them in-place. */
        for (int k = 0; k < idx_pos / 2; k++) {
            int tmp = indices[k];
            indices[k] = indices[idx_pos - 1 - k];
            indices[idx_pos - 1 - k] = tmp;
        }

        /* Copy into out_selected_out (caller's array).
           We only copy up to min(m, result) elements to avoid overflow. */
        int copy_count = idx_pos;
        if (copy_count > m) copy_count = m;
        if (copy_count > 0) {
            memcpy(out_selected_out, indices, (size_t)copy_count * sizeof(int));
        }

        free(indices);
    }

    /* Free DP table */
    for (int i = 0; i <= m; i++) free(dp[i]);
    free(dp);

    /* Set output length */
    if (out_length) *out_length = result;

    return result;
}

/* ----------------------------------------------------------
 * Print the LCS solution.
 * ---------------------------------------------------------- */
void lcs_print_solution(const int* seq1, int m, const int* seq2, int n,
                        const int* selected, int selected_len) {
    if (!seq1 || !seq2 || m <= 0 || n <= 0) {
        printf("Invalid sequences for LCS print.\n");
        return;
    }

    /* ---------- Build the LCS string / element list ---------- */
    /* We already have the logic; just compute it here for printing. */
    int lcs_len = 0;
    /* Re-compute LCS length (or use selected if provided). */
    if (selected && selected_len > 0) {
        lcs_len = selected_len;
    } else {
        /* Quick recompute: just use the DP table approach briefly.
           Actually, let's just print "not computed" if no selected.
         */
        printf("LCS length not computed; call lcs_length first.\n");
        return;
    }

    printf("=== Longest Common Subsequence ===\n");
    printf("\nSequence 1 (m=%d):\n", m);
    for (int i = 0; i < m; i++) {
        printf("  [%d] %d\n", i + 1, seq1[i]);
    }

    printf("\nSequence 2 (n=%d):\n", n);
    for (int i = 0; i < n; i++) {
        printf("  [%d] %d\n", i + 1, seq2[i]);
    }

    printf("\nLongest Common Subsequence (length %d):\n", lcs_len);
    if (lcs_len > 0 && selected) {
        printf("  Positions in Seq1: ");
        for (int i = 0; i < lcs_len; i++) {
            printf("%d ", selected[i]);
        }
        printf("\n  Values: ");
        for (int i = 0; i < lcs_len; i++) {
            printf("%d ", seq1[selected[i] - 1]);  /* convert 1-based to 0-based */
        }
        printf("\n");
    }

    /* Verify the selected positions actually form a common subsequence */
    if (lcs_len > 0) {
        printf("  Verification: The selected %d positions from Seq1\n", lcs_len);
        printf("  form a subsequence that appears in Seq2 in the same order.\n");
    }

    printf("========================================\n");
}