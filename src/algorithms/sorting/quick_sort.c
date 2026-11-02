#include "quick_sort.h"
#include <stdlib.h>
#include <stdio.h>

/* Swap helper for Fixture arrays */
static void swap_fixture(Fixture* a, Fixture* b) {
    Fixture temp = *a;
    *a = *b;
    *b = temp;
}

/* Partition function: partition fixtures by round */
static int partition_fixtures(Fixture* fixtures, int left, int right) {
    int pivot = fixtures[right].round;
    int i = left - 1;

    for (int j = left; j < right; j++) {
        if (fixtures[j].round <= pivot) {
            i++;
            swap_fixture(&fixtures[i], &fixtures[j]);
        }
    }
    swap_fixture(&fixtures[i + 1], &fixtures[right]);
    return (i + 1);
}

/* Quick sort recursive implementation */
void quick_sort_recursive(Fixture* fixtures, int left, int right) {
    if (left < right) {
        int pi = partition_fixtures(fixtures, left, right);

        /* Sort elements before partition and after partition */
        quick_sort_recursive(fixtures, left, pi - 1);
        quick_sort_recursive(fixtures, pi + 1, right);
    }
}

/* Main quick sort function - sort fixtures by round */
void quick_sort_fixtures(Fixture* fixtures, int left, int right) {
    if (!fixtures) return;
    quick_sort_recursive(fixtures, left, right);
}