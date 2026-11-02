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
// RECURSIVE BINARY SEARCH (NO MALLOC - OPERATES DIRECTLY ON FIXTURES)
// ============================================================================

// Search for a fixture by round (recursive, no malloc)
// Recurses directly on the fixtures array
int binary_search_fixture_by_round_recursive(Fixture* fixtures, int left, int right, int target_round) {
    // Base case: not found
    if (left > right || !fixtures) {
        return -1;
    }

    int mid = left + (right - left) / 2;

    if (fixtures[mid].round == target_round) {
        return mid;  // Found
    } else if (fixtures[mid].round < target_round) {
        // Search right half
        return binary_search_fixture_by_round_recursive(fixtures, mid + 1, right, target_round);
    } else {
        // Search left half
        return binary_search_fixture_by_round_recursive(fixtures, left, mid - 1, target_round);
    }
}

// Search for a fixture by day (recursive, no malloc)
// Recurses directly on the fixtures array
int binary_search_fixture_by_day_recursive(Fixture* fixtures, int left, int right, int target_day) {
    // Base case: not found
    if (left > right || !fixtures) {
        return -1;
    }

    int mid = left + (right - left) / 2;

    if (fixtures[mid].day == target_day) {
        return mid;  // Found
    } else if (fixtures[mid].day < target_day) {
        // Search right half
        return binary_search_fixture_by_day_recursive(fixtures, mid + 1, right, target_day);
    } else {
        // Search left half
        return binary_search_fixture_by_day_recursive(fixtures, left, mid - 1, target_day);
    }
}

// ============================================================================
// FIRST/LAST OCCURRENCE VARIANTS (FOR DUPLICATES)
// ============================================================================

// Find first occurrence of target_round in sorted fixtures array
int binary_search_first_occurrence(Fixture* fixtures, int count, int target_round) {
    if (!fixtures || count <= 0) return -1;

    int left = 0, right = count - 1;
    int result = -1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (fixtures[mid].round == target_round) {
            result = mid;  // Found, but keep searching left for first occurrence
            right = mid - 1;
        } else if (fixtures[mid].round < target_round) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return result;
}

// Find last occurrence of target_round in sorted fixtures array
int binary_search_last_occurrence(Fixture* fixtures, int count, int target_round) {
    if (!fixtures || count <= 0) return -1;

    int left = 0, right = count - 1;
    int result = -1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (fixtures[mid].round == target_round) {
            result = mid;  // Found, but keep searching right for last occurrence
            left = mid + 1;
        } else if (fixtures[mid].round < target_round) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

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

    // Test 2: Search by Round (Recursive - no malloc)
    printf("[TEST 2] Recursive Search by Round (No Malloc)\n");
    int result2 = binary_search_fixture_by_round_recursive(fixtures, 0, count - 1, round_to_find);
    print_binary_search_result(fixtures, result2, round_to_find, "Round (Recursive)");

    // Test 3: First Occurrence
    printf("[TEST 3] First Occurrence of Round %d\n", round_to_find);
    int result3 = binary_search_first_occurrence(fixtures, count, round_to_find);
    print_binary_search_result(fixtures, result3, round_to_find, "First Occurrence");

    // Test 4: Last Occurrence
    printf("[TEST 4] Last Occurrence of Round %d\n", round_to_find);
    int result4 = binary_search_last_occurrence(fixtures, count, round_to_find);
    print_binary_search_result(fixtures, result4, round_to_find, "Last Occurrence");

    // Test 5: Not Found Case
    printf("[TEST 5] Search for Non-Existent Round\n");
    int result5 = binary_search_fixture_by_round_iterative(fixtures, count, 9999);
    print_binary_search_result(fixtures, result5, 9999, "Round (Not Found)");

    printf("========================================================\n");
    printf("All binary search tests completed\n");
    printf("========================================================\n\n");
}
