#include "quick_sort.h"
#include <stdlib.h>

/* Swap helper for Fixture */
static void swap_fixture(Fixture* a, Fixture* b) {
    Fixture temp = *a;
    *a = *b;
    *b = temp;
}

/* Swap helper for Team */
static void swap_team(Team* a, Team* b) {
    Team temp = *a;
    *a = *b;
    *b = temp;
}

/* Quick sort for Fixture array (sorted ascending by round)
 * Uses median-of-three pivot, Dutch national flag (3-way) partition,
 * and recurses into the smaller side while looping on the larger side. */
void quick_sort_fixtures(Fixture* fixtures, int left, int right) {
    if (!fixtures) return;

    while (left < right) {
        int center = left + (right - left) / 2;

        /* Median-of-three pivot: sort left, center, right ascending */
        if (fixtures[left].round > fixtures[center].round) {
            swap_fixture(&fixtures[left], &fixtures[center]);
        }
        if (fixtures[center].round > fixtures[right].round) {
            swap_fixture(&fixtures[center], &fixtures[right]);
        }
        if (fixtures[left].round > fixtures[center].round) {
            swap_fixture(&fixtures[left], &fixtures[center]);
        }

        int pivot = fixtures[center].round;

        /* Three-way partition (Dutch National Flag) */
        int i = left;
        int j = right;
        int k = left;

        while (k <= j) {
            if (fixtures[k].round < pivot) {
                swap_fixture(&fixtures[i], &fixtures[k]);
                i++;
                k++;
            } else if (fixtures[k].round > pivot) {
                swap_fixture(&fixtures[k], &fixtures[j]);
                j--;
            } else {
                k++;
            }
        }

        /* Recurse into the smaller side, loop on the larger side */
        if ((i - 1 - left) < (right - (j + 1))) {
            if (left < i - 1) {
                quick_sort_fixtures(fixtures, left, i - 1);
            }
            left = j + 1;
        } else {
            if (j + 1 < right) {
                quick_sort_fixtures(fixtures, j + 1, right);
            }
            right = i - 1;
        }
    }
}

/* Quick sort for Team array (sorted descending by ranking, best first)
 * Uses median-of-three pivot, Dutch national flag (3-way) partition,
 * and recurses into the smaller side while looping on the larger side. */
void quick_sort_teams(Team* teams, int left, int right) {
    if (!teams) return;

    while (left < right) {
        int center = left + (right - left) / 2;

        /* Median-of-three pivot: sort left, center, right descending */
        if (teams[left].ranking < teams[center].ranking) {
            swap_team(&teams[left], &teams[center]);
        }
        if (teams[center].ranking < teams[right].ranking) {
            swap_team(&teams[center], &teams[right]);
        }
        if (teams[left].ranking < teams[center].ranking) {
            swap_team(&teams[left], &teams[center]);
        }

        int pivot = teams[center].ranking;

        /* Three-way partition (Dutch National Flag) for descending order */
        int i = left;
        int j = right;
        int k = left;

        while (k <= j) {
            if (teams[k].ranking > pivot) {
                swap_team(&teams[i], &teams[k]);
                i++;
                k++;
            } else if (teams[k].ranking < pivot) {
                swap_team(&teams[k], &teams[j]);
                j--;
            } else {
                k++;
            }
        }

        /* Recurse into the smaller side, loop on the larger side */
        if ((i - 1 - left) < (right - (j + 1))) {
            if (left < i - 1) {
                quick_sort_teams(teams, left, i - 1);
            }
            left = j + 1;
        } else {
            if (j + 1 < right) {
                quick_sort_teams(teams, j + 1, right);
            }
            right = i - 1;
        }
    }
}
