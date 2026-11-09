#include "benchmark.h"
#include "../data/team.h"
#include "../algorithms/sorting/merge_sort.h"
#include "../algorithms/sorting/quick_sort.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
BenchmarkResult benchmark_sort(void (*sort_func)(Team*, int, int),
                               const char* algorithm_name,
                               Team* teams, int n,
                               int run_count) {
    BenchmarkResult result;
    strncpy(result.algorithm_name, algorithm_name, sizeof(result.algorithm_name) - 1);
    result.algorithm_name[sizeof(result.algorithm_name) - 1] = '\0';
    result.n = n;
    result.passes = 0;
    result.failures = 0;
    result.cpu_time = 0.0;
    result.memory_peak = 0;

    if (n <= 0 || !teams || !sort_func) {
        result.cpu_time = -1.0;
        return result;
    }

    /* Create a copy of teams to sort */
    Team* teams_copy = (Team*)malloc(sizeof(Team) * n);
    if (!teams_copy) {
        result.cpu_time = -1.0;
        return result;
    }
    memcpy(teams_copy, teams, sizeof(Team) * n);

    /* Warm-up run to avoid cold start effects */
    sort_func(teams_copy, n, sizeof(Team));

    /* Actual benchmark */
    clock_t start = clock();
    for (int r = 0; r < run_count; r++) {
        memcpy(teams_copy, teams, sizeof(Team) * n);
        sort_func(teams_copy, n, sizeof(Team));
        /* Verify sort is correct (check non-decreasing order by ranking) */
        int valid = 1;
        for (int i = 1; i < n; i++) {
            if (teams_copy[i - 1].ranking > teams_copy[i].ranking) {
                valid = 0;
                break;
            }
        }
        if (valid) result.passes++;
        else result.failures++;
    }
    clock_t end = clock();

    result.cpu_time = (double)(end - start) / CLOCKS_PER_SEC / run_count;
    /* Approximate memory peak: sizeof(Team) * n bytes */
    size_t peak = sizeof(Team) * n;
    if (peak > (size_t)result.memory_peak) result.memory_peak = peak;

    free(teams_copy);
    return result;
}

/* Run comprehensive benchmark suite */
int run_all_benchmarks(int n_small, int n_medium, int n_large,
                       int runs_per_test) {
    /* Seed random number generator */
    srand((unsigned int)time(NULL));

    int total_tests = 0;
    int passed_tests = 0;

    /* Small test: n_small teams */
    {
        Team* teams = (Team*)malloc(sizeof(Team) * n_small);
        if (!teams) return 0;
        for (int i = 0; i < n_small; i++) {
            teams[i].id = i;
            snprintf(teams[i].name, MAX_NAME_LENGTH, "Team %d", i + 1);
            teams[i].ranking = (rand() % 20) + 1;  /* 1-20 */
            teams[i].points = 0;
            snprintf(teams[i].home_stadium, MAX_NAME_LENGTH, "Stadium %d", i + 1);
        }

        /* Merge Sort */
        {
            BenchmarkResult r = benchmark_sort(merge_sort_teams, "Merge Sort",
                                               teams, n_small, runs_per_test);
            total_tests++;
            if (r.failures == 0) passed_tests++;
            printf("Merge Sort (n=%d): %.3f s, %d passes, %d failures\n",
                   n_small, r.cpu_time, r.passes, r.failures);
        }

        /* Quick Sort */
        {
            BenchmarkResult r = benchmark_sort(quick_sort_teams, "Quick Sort",
                                               teams, n_small, runs_per_test);
            total_tests++;
            if (r.failures == 0) passed_tests++;
            printf("Quick Sort (n=%d): %.3f s, %d passes, %d failures\n",
                   n_small, r.cpu_time, r.passes, r.failures);
        }

        free(teams);
    }

    /* Medium test: n_medium teams */
    {
        Team* teams = (Team*)malloc(sizeof(Team) * n_medium);
        if (!teams) return passed_tests;
        for (int i = 0; i < n_medium; i++) {
            teams[i].id = i;
            snprintf(teams[i].name, MAX_NAME_LENGTH, "Team %d", i + 1);
            teams[i].ranking = (rand() % 20) + 1;  /* 1-20 */
            teams[i].points = 0;
            snprintf(teams[i].home_stadium, MAX_NAME_LENGTH, "Stadium %d", i + 1);
        }

        /* Merge Sort */
        {
            BenchmarkResult r = benchmark_sort(merge_sort_teams, "Merge Sort",
                                               teams, n_medium, runs_per_test);
            total_tests++;
            if (r.failures == 0) passed_tests++;
            printf("Merge Sort (n=%d): %.3f s, %d passes, %d failures\n",
                   n_medium, r.cpu_time, r.passes, r.failures);
        }

        /* Quick Sort */
        {
            BenchmarkResult r = benchmark_sort(quick_sort_teams, "Quick Sort",
                                               teams, n_medium, runs_per_test);
            total_tests++;
            if (r.failures == 0) passed_tests++;
            printf("Quick Sort (n=%d): %.3f s, %d passes, %d failures\n",
                   n_medium, r.cpu_time, r.passes, r.failures);
        }

        free(teams);
    }

    /* Large test: n_large teams */
    {
        Team* teams = (Team*)malloc(sizeof(Team) * n_large);
        if (!teams) return passed_tests;
        for (int i = 0; i < n_large; i++) {
            teams[i].id = i;
            snprintf(teams[i].name, MAX_NAME_LENGTH, "Team %d", i + 1);
            teams[i].ranking = (rand() % 20) + 1;  /* 1-20 */
            teams[i].points = 0;
            snprintf(teams[i].home_stadium, MAX_NAME_LENGTH, "Stadium %d", i + 1);
        }

        /* Merge Sort */
        {
            BenchmarkResult r = benchmark_sort(merge_sort_teams, "Merge Sort",
                                               teams, n_large, runs_per_test);
            total_tests++;
            if (r.failures == 0) passed_tests++;
            printf("Merge Sort (n=%d): %.3f s, %d passes, %d failures\n",
                   n_large, r.cpu_time, r.passes, r.failures);
        }

        /* Quick Sort */
        {
            BenchmarkResult r = benchmark_sort(quick_sort_teams, "Quick Sort",
                                               teams, n_large, runs_per_test);
            total_tests++;
            if (r.failures == 0) passed_tests++;
            printf("Quick Sort (n=%d): %.3f s, %d passes, %d failures\n",
                   n_large, r.cpu_time, r.passes, r.failures);
        }

        free(teams);
    }

    printf("\n%d/%d tests passed\n", passed_tests, total_tests);
    return passed_tests;
}

/* Print benchmark results table */
void print_benchmark_results(BenchmarkResult* results, int count) {
    printf("\n=== Benchmark Results Table ===\n");
    printf("%-20s %-8s %-12s %-12s %-12s\n",
           "Algorithm", "N", "CPU Time(s)", "Passes", "Failures");
    printf("-------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-20s %-8d %-12.4f %-12d %-12d\n",
               results[i].algorithm_name, results[i].n,
               results[i].cpu_time, results[i].passes, results[i].failures);
    }
    printf("=======================================================\n");
}