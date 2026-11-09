#include <stdio.h>
#include <stdlib.h>
#include "../src/data/fixture.h"
#include "../src/algorithms/searching/binary_search.h"

static int test_edge_cases(void) {
    printf("TEST 1: Edge Cases (NULL, Empty, Single Element, Missing Target)\n");
    int all_pass = 1;

    /* 1. NULL array */
    if (binary_search_fixture_by_round_iterative(NULL, 10, 5) != -1) all_pass = 0;
    if (binary_search_fixture_by_day_iterative(NULL, 10, 5) != -1) all_pass = 0;
    if (binary_search_fixture_by_round_recursive(NULL, 0, 9, 5) != -1) all_pass = 0;
    if (binary_search_fixture_by_day_recursive(NULL, 0, 9, 5) != -1) all_pass = 0;
    if (binary_search_first_occurrence(NULL, 10, 5) != -1) all_pass = 0;
    if (binary_search_last_occurrence(NULL, 10, 5) != -1) all_pass = 0;

    /* 2. Empty array (count = 0, or left > right) */
    Fixture dummy[1];
    dummy[0].round = 5;
    dummy[0].day = 5;

    if (binary_search_fixture_by_round_iterative(dummy, 0, 5) != -1) all_pass = 0;
    if (binary_search_fixture_by_day_iterative(dummy, 0, 5) != -1) all_pass = 0;
    if (binary_search_fixture_by_round_recursive(dummy, 0, -1, 5) != -1) all_pass = 0;
    if (binary_search_fixture_by_day_recursive(dummy, 0, -1, 5) != -1) all_pass = 0;
    if (binary_search_first_occurrence(dummy, 0, 5) != -1) all_pass = 0;
    if (binary_search_last_occurrence(dummy, 0, 5) != -1) all_pass = 0;

    /* 3. Single element */
    Fixture single[1];
    single[0].round = 10;
    single[0].day = 20;

    if (binary_search_fixture_by_round_iterative(single, 1, 10) != 0) all_pass = 0;
    if (binary_search_fixture_by_round_iterative(single, 1, 99) != -1) all_pass = 0;
    if (binary_search_fixture_by_day_iterative(single, 1, 20) != 0) all_pass = 0;
    if (binary_search_fixture_by_day_iterative(single, 1, 99) != -1) all_pass = 0;

    if (binary_search_fixture_by_round_recursive(single, 0, 0, 10) != 0) all_pass = 0;
    if (binary_search_fixture_by_round_recursive(single, 0, 0, 99) != -1) all_pass = 0;
    if (binary_search_fixture_by_day_recursive(single, 0, 0, 20) != 0) all_pass = 0;
    if (binary_search_fixture_by_day_recursive(single, 0, 0, 99) != -1) all_pass = 0;

    if (binary_search_first_occurrence(single, 1, 10) != 0) all_pass = 0;
    if (binary_search_first_occurrence(single, 1, 99) != -1) all_pass = 0;
    if (binary_search_last_occurrence(single, 1, 10) != 0) all_pass = 0;
    if (binary_search_last_occurrence(single, 1, 99) != -1) all_pass = 0;

    printf("  Edge cases: %s\n", all_pass ? "PASS" : "FAIL");
    return all_pass;
}

static int test_first_last_occurrence(void) {
    printf("\nTEST 2: First and Last Occurrence (Duplicates)\n");
    int rounds[] = {1, 2, 2, 2, 3, 4, 4, 5};
    int n = sizeof(rounds) / sizeof(rounds[0]);

    Fixture* f = (Fixture*)malloc(sizeof(Fixture) * n);
    for (int i = 0; i < n; i++) {
        f[i].id = i;
        f[i].round = rounds[i];
        f[i].day = rounds[i];
    }

    int all_pass = 1;

    /* Target 2: first = 1, last = 3 */
    if (binary_search_first_occurrence(f, n, 2) != 1) {
        printf("FAIL: first_occurrence for 2 expected 1, got %d\n", binary_search_first_occurrence(f, n, 2));
        all_pass = 0;
    }
    if (binary_search_last_occurrence(f, n, 2) != 3) {
        printf("FAIL: last_occurrence for 2 expected 3, got %d\n", binary_search_last_occurrence(f, n, 2));
        all_pass = 0;
    }

    /* Target 4: first = 5, last = 6 */
    if (binary_search_first_occurrence(f, n, 4) != 5) {
        printf("FAIL: first_occurrence for 4 expected 5, got %d\n", binary_search_first_occurrence(f, n, 4));
        all_pass = 0;
    }
    if (binary_search_last_occurrence(f, n, 4) != 6) {
        printf("FAIL: last_occurrence for 4 expected 6, got %d\n", binary_search_last_occurrence(f, n, 4));
        all_pass = 0;
    }

    /* Missing target: 99 */
    if (binary_search_first_occurrence(f, n, 99) != -1) all_pass = 0;
    if (binary_search_last_occurrence(f, n, 99) != -1) all_pass = 0;

    free(f);
    printf("  First/last occurrence: %s\n", all_pass ? "PASS" : "FAIL");
    return all_pass;
}

static int test_1_million_elements(void) {
    printf("\nTEST 3: 1,000,000 Elements Search\n");
    int n = 1000000;
    Fixture* f = (Fixture*)malloc(sizeof(Fixture) * n);
    if (!f) {
        printf("FAIL: Memory allocation failed for 1,000,000 fixtures\n");
        return 0;
    }

    for (int i = 0; i < n; i++) {
        f[i].id = i;
        f[i].round = i * 2; /* 0, 2, 4, ..., 1999998 */
        f[i].day = i * 2;
    }

    int all_pass = 1;
    int target_idx = 250000;
    int target_val = target_idx * 2;

    /* Iterative round */
    int idx = binary_search_fixture_by_round_iterative(f, n, target_val);
    if (idx != target_idx) {
        printf("FAIL: round_iterative expected %d, got %d\n", target_idx, idx);
        all_pass = 0;
    }

    /* Iterative day */
    idx = binary_search_fixture_by_day_iterative(f, n, target_val);
    if (idx != target_idx) {
        printf("FAIL: day_iterative expected %d, got %d\n", target_idx, idx);
        all_pass = 0;
    }

    /* Recursive round */
    idx = binary_search_fixture_by_round_recursive(f, 0, n - 1, target_val);
    if (idx != target_idx) {
        printf("FAIL: round_recursive expected %d, got %d\n", target_idx, idx);
        all_pass = 0;
    }

    /* Recursive day */
    idx = binary_search_fixture_by_day_recursive(f, 0, n - 1, target_val);
    if (idx != target_idx) {
        printf("FAIL: day_recursive expected %d, got %d\n", target_idx, idx);
        all_pass = 0;
    }

    /* Missing targets (odd numbers) */
    if (binary_search_fixture_by_round_iterative(f, n, target_val + 1) != -1) all_pass = 0;
    if (binary_search_fixture_by_day_iterative(f, n, target_val + 1) != -1) all_pass = 0;
    if (binary_search_fixture_by_round_recursive(f, 0, n - 1, target_val + 1) != -1) all_pass = 0;
    if (binary_search_fixture_by_day_recursive(f, 0, n - 1, target_val + 1) != -1) all_pass = 0;

    free(f);
    printf("  1,000,000 elements search: %s\n", all_pass ? "PASS" : "FAIL");
    return all_pass;
}

int main(void) {
    printf("========================================\n");
    printf("BINARY SEARCH VERIFICATION TESTS\n");
    printf("========================================\n\n");

    int passes = 0, fails = 0;
    if (test_edge_cases()) passes++; else fails++;
    if (test_first_last_occurrence()) passes++; else fails++;
    if (test_1_million_elements()) passes++; else fails++;

    printf("\n========================================\n");
    printf("RESULTS: %d passed, %d failed\n", passes, fails);
    printf("========================================\n");

    return (fails > 0) ? 1 : 0;
}
