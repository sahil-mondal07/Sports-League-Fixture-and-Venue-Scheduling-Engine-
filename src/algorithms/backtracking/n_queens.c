#include "n_queens.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define N_QUEENS_INIT_CAPACITY 16

/* Internal backtracking helper */
static void backtrack_n_queens(int row, int n, int* current_cols,
                              unsigned int col_mask,
                              unsigned int diag1_mask,
                              unsigned int diag2_mask,
                              const int* blocked,
                              int first_only,
                              NQueensResultSet* res) {
    /* If first_only requested and already found one, stop immediately */
    if (first_only && res->count > 0) {
        return;
    }

    /* Base case: all N matches successfully placed without conflict */
    if (row == n) {
        if (res->count >= res->capacity) {
            int new_cap = res->capacity * 2;
            NQueensSolution* new_arr = (NQueensSolution*)realloc(res->solutions,
                                                                 (size_t)new_cap * sizeof(NQueensSolution));
            if (!new_arr) return;
            res->solutions = new_arr;
            res->capacity = new_cap;
        }

        NQueensSolution sol;
        sol.n = n;
        sol.positions = (int*)malloc((size_t)n * sizeof(int));
        if (sol.positions) {
            memcpy(sol.positions, current_cols, (size_t)n * sizeof(int));
            res->solutions[res->count++] = sol;
        }
        return;
    }

    /* Try placing in every column c at the current row */
    for (int col = 0; col < n; col++) {
        /* Check if position is blocked by external constraint */
        if (blocked && blocked[row * n + col]) {
            continue;
        }

        unsigned int c_bit = 1u << col;
        unsigned int d1_bit = 1u << (row - col + n - 1); /* Major diagonal */
        unsigned int d2_bit = 1u << (row + col);         /* Minor diagonal */

        /* Prune branch if column or diagonals are attacked */
        if ((col_mask & c_bit) || (diag1_mask & d1_bit) || (diag2_mask & d2_bit)) {
            continue;
        }

        /* Place queen */
        current_cols[row] = col;

        /* Recurse to next row with updated bitmasks */
        backtrack_n_queens(row + 1, n, current_cols,
                           col_mask | c_bit,
                           diag1_mask | d1_bit,
                           diag2_mask | d2_bit,
                           blocked,
                           first_only,
                           res);

        if (first_only && res->count > 0) {
            return;
        }

        /* Backtrack: unplacement handled implicitly as row overwrites current_cols[row] */
    }
}

NQueensResultSet solve_n_queens(int n, const int* blocked, int first_only) {
    NQueensResultSet res;
    res.solutions = NULL;
    res.count = 0;
    res.capacity = 0;

    if (n <= 0 || n > 32) {
        return res;
    }

    res.capacity = N_QUEENS_INIT_CAPACITY;
    res.solutions = (NQueensSolution*)malloc((size_t)res.capacity * sizeof(NQueensSolution));
    if (!res.solutions) {
        res.capacity = 0;
        return res;
    }

    int* current_cols = (int*)malloc((size_t)n * sizeof(int));
    if (!current_cols) {
        free(res.solutions);
        res.solutions = NULL;
        res.capacity = 0;
        return res;
    }

    backtrack_n_queens(0, n, current_cols, 0, 0, 0, blocked, first_only, &res);

    free(current_cols);
    return res;
}

void print_n_queens_solution(const NQueensSolution* sol) {
    if (!sol || !sol->positions || sol->n <= 0) return;

    int n = sol->n;
    printf("\n--- Schedule Grid (%d x %d) ---\n", n, n);
    printf("Slot \\ Venue: ");
    for (int c = 0; c < n; c++) printf("%2d ", c);
    printf("\n");

    for (int r = 0; r < n; r++) {
        printf("Slot %2d:      ", r);
        for (int c = 0; c < n; c++) {
            if (sol->positions[r] == c) {
                printf(" Q ");
            } else {
                printf(" . ");
            }
        }
        printf("\n");
    }
}

void free_n_queens_result(NQueensResultSet* result) {
    if (result && result->solutions) {
        for (int i = 0; i < result->count; i++) {
            free(result->solutions[i].positions);
        }
        free(result->solutions);
        result->solutions = NULL;
        result->count = 0;
        result->capacity = 0;
    }
}
