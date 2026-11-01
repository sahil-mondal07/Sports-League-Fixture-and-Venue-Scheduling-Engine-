#include "binary_search.h"
#include <stdio.h>
#include <string.h>

// ============================================================================
// ITERATIVE BINARY SEARCH IMPLEMENTATIONS
// ============================================================================

// Search for a fixture by round (iterative)
int binary_search_fixture_by_round_iterative(Fixture* fixtures, int count, int target_round) {
    if (!fixtures || count <= 0) return -1;

    int left = 0, right = count - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;  // Avoid overflow

        if (fixtures[mid].round == target_round) {
            return mid;  // Found
        } else if (fixtures[mid].round < target_round) {
            left = mid + 1;  // Search right half
        } else {
            right = mid - 1;  // Search left half
        }
    }

    return -1;  // Not found
}

// Search for a fixture by day (iterative)
int binary_search_fixture_by_day_iterative(Fixture* fixtures, int count, int target_day) {
    if (!fixtures || count <= 0) return -1;

    int left = 0, right = count - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (fixtures[mid].day == target_day) {
            return mid;  // Found
        } else if (fixtures[mid].day < target_day) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return -1;
}

// Generic iterative binary search on sorted integer array
int binary_search_iterative(int* arr, int count, int target) {
    if (!arr || count <= 0) return -1;

    int left = 0, right = count - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (arr[mid] == target) {
            return mid;
        } else if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return -1;
}

// ============================================================================
// RECURSIVE BINARY SEARCH IMPLEMENTATIONS
// ============================================================================

// Helper for recursive binary search
int binary_search_recursive_helper(int* arr, int left, int right, int target) {
    if (left > right) {
        return -1;  // Not found
    }

    int mid = left + (right - left) / 2;

    if (arr[mid] == target) {
        return mid;  // Found
    } else if (arr[mid] < target) {
        return binary_search_recursive_helper(arr, mid + 1, right, target);
    } else {
        return binary_search_recursive_helper(arr, left, mid - 1, target);
    }
}

// Search for a fixture by round (recursive wrapper)
int binary_search_fixture_by_round_recursive(Fixture* fixtures, int count, int target_round) {
    if (!fixtures || count <= 0) return -1;

    // Convert fixture rounds to array for recursive search
    int* rounds = (int*)malloc(count * sizeof(int));
    for (int i = 0; i < count; i++) {
        rounds[i] = fixtures[i].round;
    }

    int result = binary_search_recursive_helper(rounds, 0, count - 1, target_round);
    free(rounds);

    return result;
}

// Search for a fixture by day (recursive wrapper)
int binary_search_fixture_by_day_recursive(Fixture* fixtures, int count, int target_day) {
    if (!fixtures || count <= 0) return -1;

    // Convert fixture days to array for recursive search
    int* days = (int*)malloc(count * sizeof(int));
    for (int i = 0; i < count; i++) {
        days[i] = fixtures[i].day;
    }

    int result = binary_search_recursive_helper(days, 0, count - 1, target_day);
    free(days);

    return result;
}

// ============================================================================
// UTILITY & DEMO FUNCTIONS
// ============================================================================

void print_binary_search_result(Fixture* fixtures, int result, int target, const char* search_type) {
    printf("\n--- Binary Search Result ---\n");
    printf("Search Type: %s\n", search_type);
    printf("Target: %d\n", target);

    if (result == -1) {
        printf("STATUS: NOT FOUND\n");
    } else {
        printf("STATUS: FOUND at index %d\n", result);
        printf("Fixture Details:\n");
        printf("  Round: %d\n", fixtures[result].round);
        printf("  Day: %d\n", fixtures[result].day);
        printf("  Home Team ID: %d\n", fixtures[result].home_team_id);
        printf("  Away Team ID: %d\n", fixtures[result].away_team_id);
        printf("  Venue ID: %d\n", fixtures[result].venue_id);
    }
    printf("\n");
}

void demo_binary_search(Fixture* fixtures, int count) {
    if (!fixtures || count <= 0) {
        printf("ERROR: No fixtures to search\n");
        return;
    }

    printf("\n");
    printf("========================================================\n");
    printf("BINARY SEARCH DEMONSTRATION\n");
    printf("========================================================\n");
    printf("Total fixtures: %d\n\n", count);

    // Test 1: Search by Round (Iterative)
    printf("[TEST 1] Iterative Search by Round\n");
    int round_to_find = fixtures[count / 2].round;
    int result1 = binary_search_fixture_by_round_iterative(fixtures, count, round_to_find);
    print_binary_search_result(fixtures, result1, round_to_find, "Round (Iterative)");

    // Test 2: Search by Round (Recursive)
    printf("[TEST 2] Recursive Search by Round\n");
    int result2 = binary_search_fixture_by_round_recursive(fixtures, count, round_to_find);
    print_binary_search_result(fixtures, result2, round_to_find, "Round (Recursive)");

    // Test 3: Search by Day (Iterative)
    printf("[TEST 3] Iterative Search by Day\n");
    int day_to_find = fixtures[count / 3].day;
    int result3 = binary_search_fixture_by_day_iterative(fixtures, count, day_to_find);
    print_binary_search_result(fixtures, result3, day_to_find, "Day (Iterative)");

    // Test 4: Search by Day (Recursive)
    printf("[TEST 4] Recursive Search by Day\n");
    int result4 = binary_search_fixture_by_day_recursive(fixtures, count, day_to_find);
    print_binary_search_result(fixtures, result4, day_to_find, "Day (Recursive)");

    // Test 5: Not Found Case
    printf("[TEST 5] Search for Non-Existent Round\n");
    int result5 = binary_search_fixture_by_round_iterative(fixtures, count, 9999);
    print_binary_search_result(fixtures, result5, 9999, "Round (Not Found)");

    printf("========================================================\n");
    printf("All binary search tests completed\n");
    printf("========================================================\n\n");
}
