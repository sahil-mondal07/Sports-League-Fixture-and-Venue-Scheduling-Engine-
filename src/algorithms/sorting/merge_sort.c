#include "merge_sort.h"

void merge_teams(Team* teams, int left, int mid, int right) {
    int left_size = mid - left + 1;
    int right_size = right - mid;

    // Create temporary arrays
    Team* left_arr = (Team*)malloc(sizeof(Team) * left_size);
    Team* right_arr = (Team*)malloc(sizeof(Team) * right_size);

    if (!left_arr || !right_arr) {
        printf("Memory allocation failed in merge_teams\n");
        return;
    }

    // Copy data to temporary arrays
    for (int i = 0; i < left_size; i++)
        left_arr[i] = teams[left + i];

    for (int i = 0; i < right_size; i++)
        right_arr[i] = teams[mid + 1 + i];

    // Merge the temporary arrays back
    int i = 0, j = 0, k = left;

    while (i < left_size && j < right_size) {
        // Sort descending: higher ranking (better team) comes first
        if (left_arr[i].ranking >= right_arr[j].ranking) {
            teams[k++] = left_arr[i++];
        } else {
            teams[k++] = right_arr[j++];
        }
    }

    // Copy remaining elements from left array
    while (i < left_size) {
        teams[k++] = left_arr[i++];
    }

    // Copy remaining elements from right array
    while (j < right_size) {
        teams[k++] = right_arr[j++];
    }

    free(left_arr);
    free(right_arr);
}

void merge_sort_teams(Team* teams, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        // Sort left half
        merge_sort_teams(teams, left, mid);

        // Sort right half
        merge_sort_teams(teams, mid + 1, right);

        // Merge sorted halves
        merge_teams(teams, left, mid, right);
    }
}

void print_sorted_teams(Team* teams, int count) {
    if (!teams || count == 0) {
        printf("\nNo teams to display.\n");
        return;
    }

    printf("\n=== Teams Sorted by Ranking (Best to Worst) ===\n");
    printf("%-5s %-20s %-20s %-10s\n", "Rank", "Team Name", "Stadium", "Points");
    printf("-----------------------------------------------------\n");

    for (int i = 0; i < count; i++) {
        printf("%-5d %-20s %-20s %-10d\n",
               teams[i].ranking, teams[i].name, teams[i].home_stadium, teams[i].points);
    }
}
