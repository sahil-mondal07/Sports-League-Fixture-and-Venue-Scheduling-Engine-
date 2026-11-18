#include "dp_knapsack.h"
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------
 * 0/1 Knapsack with 2-D DP table and proper item selection traceback.
 * ------------------------------------------------------------ */

int knapsack_01(int capacity, const KnapsackItem* items, int n, int* selected_out) {
    /* Validate inputs */
    if (capacity <= 0 || items == NULL || n <= 0) {
        if (selected_out) memset(selected_out, 0, (size_t)n * sizeof(int));
        return 0;
    }

    /* Reject any item with negative weight.
       Items with weight 0 and value > 0 are handled separately. */
    for (int i = 0; i < n; i++) {
        if (items[i].weight < 0) {
            /* Reject item with negative weight - treat as unavailable */
            /* Reset selected_out to all zeros and return 0 */
            if (selected_out) memset(selected_out, 0, (size_t)n * sizeof(int));
            return 0;
        }
    }

    /* Build 2-D DP table: t[i][w] = max value using items 0..i-1 with capacity w */
    int* t = (int*)calloc((size_t)(n + 1) * (capacity + 1), sizeof(int));
    if (!t) return 0;

    /* Fill the DP table */
    for (int i = 1; i <= n; i++) {
        const KnapsackItem* item = &items[i - 1];
        int weight_i = item->weight;
        int value_i = item->value;

        for (int w = 0; w <= capacity; w++) {
            /* Option 1: don't include item i-1 */
            int val1 = t[(i - 1) * (capacity + 1) + w];

            /* Option 2: include item i-1, if it fits */
            int val2 = 0;
            if (weight_i <= w && weight_i >= 0) {
                val2 = t[(i - 1) * (capacity + 1) + (w - weight_i)] + value_i;
            } else if (weight_i == 0 && value_i > 0) {
                /* Weight 0, value > 0: always include, add value regardless of w */
                val2 = t[(i - 1) * (capacity + 1) + w] + value_i;
            }

            t[i * (capacity + 1) + w] = val1 > val2 ? val1 : val2;
        }
    }

    /* Save the result before freeing */
    int result = n > 0 && capacity >= 0 ? t[n * (capacity + 1) + capacity] : 0;

    /* Traceback: determine which items were selected.
       Start from i=n, w=capacity. If t[i][w] != t[i-1][w], item i-1 is selected. */
    /* Reset selected_out to all 0s first */
    if (selected_out) memset(selected_out, 0, (size_t)n * sizeof(int));

    int w = capacity;
    for (int i = n; i > 0 && w >= 0; i--) {
        int val_without = t[(i - 1) * (capacity + 1) + w];
        int val_with = t[i * (capacity + 1) + w];

        if (val_with > val_without) {
            /* Item i-1 was selected */
            if (selected_out) selected_out[i - 1] = 1;
            /* Move w back by the weight of item i-1 */
            const KnapsackItem* item = &items[i - 1];
            w -= item->weight;
            if (w < 0) w = 0; /* Clamp to avoid negative */
        }
        /* else: item i-1 was not selected, w stays the same */
    }

    free(t);

    return result;
}

/*------------------------------------------------------------*/