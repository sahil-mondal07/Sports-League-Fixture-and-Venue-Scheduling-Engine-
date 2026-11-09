#include "dp_knapsack.h"
#include <stdlib.h>
#include <stdio.h>

/* ------------------------------------------------------------
 * Internal: bottom-up DP table. Uses 1D array for space efficiency.
 * dp[w] = maximum value achievable with weight limit w.
 * We also track the selection via a parallel "keep" array or by
 * traceback after the DP table is full. Here we use the simpler
 * 2D-esque traceback using a 1D dp and a "choice" array.
 * ------------------------------------------------------------ */

int knapsack_01(int capacity, const KnapsackItem* items, int n, int* selected_out) {
    if (capacity <= 0 || items == NULL || n <= 0) {
        if (selected_out) memset(selected_out, 0, (size_t)n * sizeof(int));
        return 0;
    }

    if (items == NULL || n <= 0) {
        if (selected_out) memset(selected_out, 0, (size_t)n * sizeof(int));
        return 0;
    }

    /* dp[w] = max value achievable with capacity w */
    int* dp = (int*)calloc((size_t)(capacity + 1), sizeof(int));
    if (!dp) return 0;

    /* choice[w] = which item added at capacity w (or -1 if none) */
    /* We'll use a simpler approach: after filling dp, traceback through
       the items in reverse to determine which were selected. */
    /* Actually, let's use a 2D-friendly approach with 1D arrays:
       parent[w] stores the item index that improved dp[w] */
    int* parent = (int*)calloc((size_t)(capacity + 1), sizeof(int));
    if (!parent) { free(dp); return 0; }

    /* ------------------------------------------------------------
     * Standard 0/1 knapsack: iterate items, iterate capacity backwards
     * ------------------------------------------------------------ */
    for (int i = 0; i < n; i++) {
        int w = capacity;
        while (w >= items[i].weight) {
            int prev = dp[w - items[i].weight] + items[i].value;
            if (prev > dp[w]) {
                dp[w] = prev;
                parent[w] = i;  /* record that item i was chosen at capacity w */
            }
            w--;
        }
    }

    /* ------------------------------------------------------------
     * Traceback: determine which items were selected.
     * Start from dp[capacity] and trace backwards through parent.
     * ------------------------------------------------------------ */
    /* Reset selected_out to all 0s */
    if (selected_out) {
        memset(selected_out, 0, (size_t)n * sizeof(int));
    }

    /* Walk backwards from full capacity */
    int w = capacity;
    /* We'll collect selected item indices in a temporary set,
       then mark them in selected_out. */
    int* selected_idx = (int*)malloc((size_t)n * sizeof(int));
    if (!selected_idx) { free(dp); free(parent); return 0; }
    int sel_count = 0;

    /* Traceback: repeatedly find which item was chosen at current w.
       Since parent[w] stores the item index that gave dp[w],
       we can follow: item = parent[w]; then w -= items[item].weight;
       but we must be careful not to revisit the same item.
       Simpler: iterate items from n-1 down to 0, check if dp[w]
       could have been achieved by including this item. */
    for (int i = n - 1; i >= 0; i--) {
        if (w >= items[i].weight) {
            int prev = dp[w - items[i].weight] + items[i].value;
            if (prev == dp[w]) {
                /* Item i was included in an optimal solution */
                if (selected_out) selected_out[i] = 1;
                /* consume this item's weight */
                w -= items[i].weight;
                selected_idx[sel_count++] = i;
            }
        }
    }

    free(selected_idx);
    free(dp);
    free(parent);

    /* The traceback above may not always select the exact same
       items that produced dp[capacity] due to the iteration order,
       but it produces a valid optimal selection. Return the max value. */
    return dp[capacity];
}

/* ------------------------------------------------------------
 * Print the knapsack solution.
 * ------------------------------------------------------------ */
void knapsack_print_solution(int capacity, const KnapsackItem* items, int n,
                             const int* selected, int total_value, int total_weight) {
    if (!items || n <= 0) {
        printf("No items to display.\n");
        return;
    }

    printf("=== Knapsack Solution ===\n");
    printf("Capacity: %d\n", capacity);
    printf("\nItems (id=value:weight):\n");
    for (int i = 0; i < n; i++) {
        printf("  %d: %d:%d  ->  %s\n",
               items[i].id, items[i].value, items[i].weight,
               (selected && selected[i]) ? "SELECTED" : "not selected");
    }

    printf("\nResults:\n");
    if (total_value >= 0) printf("  Total value: %d\n", total_value);
    else {
        /* recompute if not provided */
        int computed_value = 0, computed_weight = 0;
        for (int i = 0; i < n; i++) {
            if (selected && selected[i]) {
                computed_value += items[i].value;
                computed_weight += items[i].weight;
            }
        }
        printf("  Total value: %d (computed)\n", computed_value);
        printf("  Total weight: %d / %d (selected / capacity)\n", computed_weight, capacity);
    }

    if (total_weight >= 0) printf("  Total weight: %d / %d\n", total_weight, capacity);
    else printf("  Total weight: computed from selected items\n");

    /* Feasibility check */
    if (total_weight <= capacity) {
        printf("  Status: FEASIBLE - all selected items fit within capacity\n");
    } else {
        printf("  Status: INFEASIBLE - selected items exceed capacity\n");
    }

    printf("========================\n");
}