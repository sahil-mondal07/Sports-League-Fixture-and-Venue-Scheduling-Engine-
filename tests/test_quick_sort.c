#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../src/data/team.h"
#include "../src/data/fixture.h"
#include "../src/algorithms/sorting/quick_sort.h"

static int cmp_fixture_qsort(const void* a, const void* b) {
    const Fixture* fa = (const Fixture*)a;
    const Fixture* fb = (const Fixture*)b;
    return (fa->round > fb->round) - (fa->round < fb->round);
}

static int cmp_team_desc_qsort(const void* a, const void* b) {
    const Team* ta = (const Team*)a;
    const Team* tb = (const Team*)b;
    return (tb->ranking > ta->ranking) - (tb->ranking < ta->ranking);
}

static int test_fixtures_input(const char* label, const int* rounds, int n) {
    Fixture* quick_arr = (Fixture*)malloc(sizeof(Fixture) * n);
    Fixture* qsort_arr = (Fixture*)malloc(sizeof(Fixture) * n);
    for (int i = 0; i < n; i++) {
        quick_arr[i].id = i;
        quick_arr[i].round = rounds[i];
        qsort_arr[i].id = i;
        qsort_arr[i].round = rounds[i];
    }

    quick_sort_fixtures(quick_arr, 0, n - 1);
    qsort(qsort_arr, n, sizeof(Fixture), cmp_fixture_qsort);

    int pass = 1;
    for (int i = 0; i < n; i++) {
        if (quick_arr[i].round != qsort_arr[i].round) {
            pass = 0;
            break;
        }
    }

    printf("  Fixture [%s]: %s\n", label, pass ? "PASS" : "FAIL");
    free(quick_arr);
    free(qsort_arr);
    return pass;
}

static int test_teams_input(const char* label, const int* rankings, int n) {
    Team* quick_arr = (Team*)malloc(sizeof(Team) * n);
    Team* qsort_arr = (Team*)malloc(sizeof(Team) * n);
    for (int i = 0; i < n; i++) {
        quick_arr[i].id = i;
        quick_arr[i].ranking = rankings[i];
        qsort_arr[i].id = i;
        qsort_arr[i].ranking = rankings[i];
    }

    quick_sort_teams(quick_arr, 0, n - 1);
    qsort(qsort_arr, n, sizeof(Team), cmp_team_desc_qsort);

    int pass = 1;
    for (int i = 0; i < n; i++) {
        if (quick_arr[i].ranking != qsort_arr[i].ranking) {
            pass = 0;
            break;
        }
    }

    printf("  Team (descending) [%s]: %s\n", label, pass ? "PASS" : "FAIL");
    free(quick_arr);
    free(qsort_arr);
    return pass;
}

static int test_correctness_vs_qsort(void) {
    printf("TEST 1: Quick Sort vs QSort (random, sorted, reverse, all-equal)\n");
    int n = 1000;
    int* data_random = (int*)malloc(sizeof(int) * n);
    int* data_sorted = (int*)malloc(sizeof(int) * n);
    int* data_reverse = (int*)malloc(sizeof(int) * n);
    int* data_allequal = (int*)malloc(sizeof(int) * n);

    srand(12345);
    for (int i = 0; i < n; i++) {
        data_random[i] = rand() % 500;
        data_sorted[i] = i;
        data_reverse[i] = n - i;
        data_allequal[i] = 42;
    }

    int all_pass = 1;
    if (!test_fixtures_input("random", data_random, n)) all_pass = 0;
    if (!test_fixtures_input("sorted", data_sorted, n)) all_pass = 0;
    if (!test_fixtures_input("reverse", data_reverse, n)) all_pass = 0;
    if (!test_fixtures_input("all-equal", data_allequal, n)) all_pass = 0;

    if (!test_teams_input("random", data_random, n)) all_pass = 0;
    if (!test_teams_input("sorted", data_sorted, n)) all_pass = 0;
    if (!test_teams_input("reverse", data_reverse, n)) all_pass = 0;
    if (!test_teams_input("all-equal", data_allequal, n)) all_pass = 0;

    free(data_random);
    free(data_sorted);
    free(data_reverse);
    free(data_allequal);

    return all_pass;
}

static int test_100k_already_sorted(void) {
    printf("\nTEST 2: 100,000 Already-Sorted Fixtures Performance\n");
    int count = 100000;
    Fixture* f = (Fixture*)malloc(sizeof(Fixture) * count);
    if (!f) {
        printf("FAIL: Allocation failed\n");
        return 0;
    }

    for (int i = 0; i < count; i++) {
        f[i].id = i;
        f[i].round = i;
    }

    clock_t start = clock();
    quick_sort_fixtures(f, 0, count - 1);
    clock_t end = clock();

    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Sorted 100,000 already-sorted fixtures in %.4f s\n", elapsed);

    /* Verify order is preserved */
    int is_sorted = 1;
    for (int i = 1; i < count; i++) {
        if (f[i].round < f[i - 1].round) {
            is_sorted = 0;
            break;
        }
    }

    free(f);

    if (!is_sorted) {
        printf("FAIL: Array is not sorted\n");
        return 0;
    }

    if (elapsed >= 1.0) {
        printf("FAIL: Took %.4f s (must be under 1.0 s)\n", elapsed);
        return 0;
    }

    printf("PASS: 100,000 fixtures sorted in under 1.0 s\n");
    return 1;
}

int main(void) {
    printf("========================================\n");
    printf("QUICK SORT VERIFICATION TESTS\n");
    printf("========================================\n\n");

    int passes = 0, fails = 0;
    if (test_correctness_vs_qsort()) passes++; else fails++;
    if (test_100k_already_sorted()) passes++; else fails++;

    printf("\n========================================\n");
    printf("RESULTS: %d passed, %d failed\n", passes, fails);
    printf("========================================\n");

    return (fails > 0) ? 1 : 0;
}
