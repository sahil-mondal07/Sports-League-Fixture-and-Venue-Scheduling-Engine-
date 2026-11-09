#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "../utils/common.h"
#include "../data/team.h"
#include <time.h>

// Benchmark result structure
typedef struct {
    char algorithm_name[64];
    int n;              // Number of elements
    double cpu_time;    // CPU time in seconds
    long memory_peak;   // Peak memory (bytes) - approximate
    int passes;         // Number of test runs
    int failures;       // Number of correctness failures
} BenchmarkResult;

// Run a single sort benchmark
BenchmarkResult benchmark_sort(void (*sort_func)(Team*, int, int),
                               const char* algorithm_name,
                               Team* teams, int n,
                               int run_count);

// Run comprehensive benchmark suite
int run_all_benchmarks(int n_small, int n_medium, int n_large,
                       int runs_per_test);

// Print benchmark results table
void print_benchmark_results(BenchmarkResult* results, int count);

#endif