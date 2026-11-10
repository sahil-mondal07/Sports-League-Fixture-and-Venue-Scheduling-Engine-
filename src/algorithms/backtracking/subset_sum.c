#include "subset_sum.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SUBSET_SUM_INIT_CAP 16

/* Internal item representation to keep track of original indices */
typedef struct {
    int val;
    int original_idx;
} ItemMeta;

/* Compare descending for faster pruning */
static int compare_items_desc(const void* a, const void* b) {
    const ItemMeta* ia = (const ItemMeta*)a;
    const ItemMeta* ib = (const ItemMeta*)b;
    return ib->val - ia->val;
}

static void backtrack_subset_sum(int idx, int n, int current_sum, int target,
                                 const ItemMeta* items, const int* suffix_sums,
                                 int* chosen_indices, int chosen_count,
                                 int first_only, SubsetSumResultSet* res) {
    if (first_only && res->count > 0) {
        return;
    }

    if (current_sum == target) {
        if (res->count >= res->capacity) {
            int new_cap = res->capacity * 2;
            SubsetSumSolution* new_arr = (SubsetSumSolution*)realloc(res->solutions,
                                                                     (size_t)new_cap * sizeof(SubsetSumSolution));
            if (!new_arr) return;
            res->solutions = new_arr;
            res->capacity = new_cap;
        }

        SubsetSumSolution sol;
        sol.count = chosen_count;
        sol.sum = target;
        sol.indices = (int*)malloc((size_t)chosen_count * sizeof(int));
        sol.values = (int*)malloc((size_t)chosen_count * sizeof(int));

        if (sol.indices && sol.values) {
            for (int i = 0; i < chosen_count; i++) {
                int item_idx = chosen_indices[i];
                sol.indices[i] = items[item_idx].original_idx;
                sol.values[i] = items[item_idx].val;
            }
            res->solutions[res->count++] = sol;
        } else {
            free(sol.indices);
            free(sol.values);
        }
        return;
    }

    if (idx >= n) {
        return;
    }

    /* Prune if even taking all remaining items cannot reach target */
    if (current_sum + suffix_sums[idx] < target) {
        return;
    }

    for (int i = idx; i < n; i++) {
        /* Prune if adding this item exceeds target */
        if (current_sum + items[i].val > target) {
            continue;
        }

        chosen_indices[chosen_count] = i;
        backtrack_subset_sum(i + 1, n, current_sum + items[i].val, target,
                             items, suffix_sums, chosen_indices, chosen_count + 1,
                             first_only, res);

        if (first_only && res->count > 0) {
            return;
        }
    }
}

SubsetSumResultSet solve_subset_sum(const int* values, int n, int target, int first_only) {
    SubsetSumResultSet res;
    res.solutions = NULL;
    res.count = 0;
    res.capacity = 0;

    if (!values || n <= 0 || target <= 0) {
        return res;
    }

    res.capacity = SUBSET_SUM_INIT_CAP;
    res.solutions = (SubsetSumSolution*)malloc((size_t)res.capacity * sizeof(SubsetSumSolution));
    if (!res.solutions) {
        res.capacity = 0;
        return res;
    }

    ItemMeta* items = (ItemMeta*)malloc((size_t)n * sizeof(ItemMeta));
    int* suffix_sums = (int*)malloc((size_t)n * sizeof(int));
    int* chosen_indices = (int*)malloc((size_t)n * sizeof(int));

    if (!items || !suffix_sums || !chosen_indices) {
        free(items);
        free(suffix_sums);
        free(chosen_indices);
        free(res.solutions);
        res.solutions = NULL;
        res.capacity = 0;
        return res;
    }

    for (int i = 0; i < n; i++) {
        items[i].val = values[i];
        items[i].original_idx = i;
    }

    /* Sort items descending */
    qsort(items, (size_t)n, sizeof(ItemMeta), compare_items_desc);

    /* Compute suffix sums for bounding */
    suffix_sums[n - 1] = items[n - 1].val;
    for (int i = n - 2; i >= 0; i--) {
        suffix_sums[i] = suffix_sums[i + 1] + items[i].val;
    }

    backtrack_subset_sum(0, n, 0, target, items, suffix_sums,
                         chosen_indices, 0, first_only, &res);

    free(items);
    free(suffix_sums);
    free(chosen_indices);
    return res;
}

void print_subset_sum_result(const SubsetSumResultSet* res) {
    if (!res) return;

    printf("\n=== Subset Sum Solutions (Count: %d) ===\n", res->count);
    for (int i = 0; i < res->count; i++) {
        const SubsetSumSolution* sol = &res->solutions[i];
        printf("  Solution %2d (Sum = %d): { ", i + 1, sol->sum);
        for (int j = 0; j < sol->count; j++) {
            printf("%d (idx %d)%s", sol->values[j], sol->indices[j],
                   (j < sol->count - 1) ? ", " : " ");
        }
        printf("}\n");
    }
}

void free_subset_sum_result(SubsetSumResultSet* res) {
    if (res && res->solutions) {
        for (int i = 0; i < res->count; i++) {
            free(res->solutions[i].indices);
            free(res->solutions[i].values);
        }
        free(res->solutions);
        res->solutions = NULL;
        res->count = 0;
        res->capacity = 0;
    }
}
