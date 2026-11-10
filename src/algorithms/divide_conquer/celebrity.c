#include "celebrity.h"
#include <stdlib.h>

/**
 * @brief Divide and conquer helper to narrow down to a single candidate.
 * Eliminates one element per recursive step: T(n) = T(n-1) + O(1) => O(n).
 */
static int find_candidate_dc(int n, InteractionQueryFunc query, const void* context) {
    if (n <= 0) {
        return -1;
    }
    if (n == 1) {
        return 0;
    }

    /* Recursively find candidate among the first n - 1 people */
    int candidate = find_candidate_dc(n - 1, query, context);
    if (candidate == -1) {
        return n - 1;
    }

    /* Compare candidate with the n-th person (index n - 1) */
    if (query(candidate, n - 1, context)) {
        /* candidate knows (n - 1), so candidate cannot be celebrity;
           (n - 1) might be */
        return n - 1;
    } else {
        /* candidate does NOT know (n - 1), so (n - 1) cannot be celebrity;
           candidate might still be */
        return candidate;
    }
}

int find_celebrity_dc(int n, InteractionQueryFunc query, const void* context) {
    if (n <= 0 || !query) {
        return -1;
    }
    if (n == 1) {
        return 0;
    }

    /* Phase 1: Divide and conquer reduction to find sole potential candidate */
    int candidate = find_candidate_dc(n, query, context);
    if (candidate < 0 || candidate >= n) {
        return -1;
    }

    /* Phase 2: Verification pass (O(n)) */
    for (int i = 0; i < n; i++) {
        if (i != candidate) {
            /* Candidate must not know i */
            if (query(candidate, i, context)) {
                return -1;
            }
            /* Everyone else must know candidate */
            if (!query(i, candidate, context)) {
                return -1;
            }
        }
    }

    return candidate;
}

/* Internal context wrapper for matrix queries */
typedef struct {
    const int* matrix;
    int n;
} MatrixContext;

static int matrix_query_func(int a, int b, const void* context) {
    const MatrixContext* ctx = (const MatrixContext*)context;
    return ctx->matrix[a * ctx->n + b];
}

int find_celebrity_matrix(const int* matrix, int n) {
    if (!matrix || n <= 0) {
        return -1;
    }
    MatrixContext ctx;
    ctx.matrix = matrix;
    ctx.n = n;
    return find_celebrity_dc(n, matrix_query_func, &ctx);
}
