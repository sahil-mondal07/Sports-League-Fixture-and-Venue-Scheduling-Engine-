# Testing Strategy

## Testing Approach

### 3-Layer Testing Pyramid

```
        /\
       /  \        E2E Tests (Scenarios)
      /____\       
     /      \      Integration Tests
    /________\     
   /          \    Unit Tests (Algorithms)
  /____________\   
```

---

## 1. Unit Tests (Algorithm Level)

### Goal
Verify each algorithm works correctly in isolation

### Strategy
- **Small-instance oracles:** Hand-calculated expected outputs
- **Edge cases:** Empty input, single element, maximum size
- **Invalid inputs:** NULL pointers, negative values, out-of-bounds

### Example: Merge Sort Test

```c
// tests/test_sorting.c
#include "../src/algorithms/sorting/merge_sort.h"
#include <assert.h>
#include <stdio.h>

void test_merge_sort_basic() {
    int arr[] = {5, 2, 8, 1, 9};
    int expected[] = {1, 2, 5, 8, 9};
    int n = 5;
    
    merge_sort(arr, 0, n-1);
    
    for (int i = 0; i < n; i++) {
        assert(arr[i] == expected[i]);
    }
    printf("✓ Merge sort basic test passed\n");
}

void test_merge_sort_empty() {
    int arr[] = {};
    merge_sort(arr, 0, -1);  // should not crash
    printf("✓ Merge sort empty test passed\n");
}

void test_merge_sort_single() {
    int arr[] = {42};
    merge_sort(arr, 0, 0);
    assert(arr[0] == 42);
    printf("✓ Merge sort single element test passed\n");
}

void test_merge_sort_already_sorted() {
    int arr[] = {1, 2, 3, 4, 5};
    int expected[] = {1, 2, 3, 4, 5};
    int n = 5;
    
    merge_sort(arr, 0, n-1);
    
    for (int i = 0; i < n; i++) {
        assert(arr[i] == expected[i]);
    }
    printf("✓ Merge sort already sorted test passed\n");
}

int main() {
    test_merge_sort_basic();
    test_merge_sort_empty();
    test_merge_sort_single();
    test_merge_sort_already_sorted();
    printf("\n✓ All merge sort tests passed!\n");
    return 0;
}
```

### Unit Test Coverage (All Algorithms)

**Sorting & Searching:**
- `test_merge_sort.c`
- `test_quick_sort.c`
- `test_binary_search.c`

**Graph Algorithms:**
- `test_dijkstra.c` - Use small 5-node graph with known shortest paths
- `test_prim.c` - Verify MST weight matches expected
- `test_kruskal.c` - Compare with Prim's result
- `test_bellman_ford.c` - Include negative edges, detect negative cycles
- `test_floyd_warshall.c` - Verify all-pairs distances

**DP Algorithms:**
- `test_knapsack.c` - Classic example: weights=[2,3,4], values=[3,4,5], capacity=5
- `test_lcs.c` - "AGGTAB" and "GXTXAYB" → "GTAB"
- `test_tsp.c` - 4-city example with known optimal tour

**Flow Algorithms:**
- `test_ford_fulkerson.c` - Simple flow network, verify max flow value
- `test_bipartite_matching.c` - 3 teams, 3 venues, verify matching count

---

## 2. Integration Tests (Component Level)

### Goal
Verify components work together correctly

### Strategy
- Test data flow between modules
- Verify scheduling engine uses algorithms correctly
- Check CSV loading → algorithm → output pipeline

### Example: Fixture Generation Integration Test

```c
// tests/test_scheduling.c
#include "../src/scheduling/fixture_generator.h"
#include "../src/data/loader.h"
#include <assert.h>

void test_fixture_generation_small() {
    // Load 4 teams
    Team* teams = load_teams("data/test/small_teams.csv", &num_teams);
    assert(num_teams == 4);
    
    // Generate round-robin fixtures
    Schedule* schedule = generate_round_robin(teams, num_teams);
    
    // 4 teams → 4*(4-1)/2 = 6 matches per round
    // Home & away → 2 rounds → 12 fixtures total
    assert(schedule->fixture_count == 12);
    
    // Verify each team plays every other team exactly once per round
    int team_match_count[4] = {0};
    for (int i = 0; i < schedule->fixture_count; i++) {
        team_match_count[schedule->fixtures[i].home_team_id]++;
        team_match_count[schedule->fixtures[i].away_team_id]++;
    }
    
    for (int i = 0; i < 4; i++) {
        assert(team_match_count[i] == 6);  // Each team plays 6 matches
    }
    
    printf("✓ Fixture generation integration test passed\n");
}

void test_venue_allocation() {
    // Load fixtures and venues
    Schedule* schedule = load_schedule("data/test/fixtures.csv");
    Venue* venues = load_venues("data/test/venues.csv", &num_venues);
    
    // Allocate venues using activity selection
    allocate_venues(schedule, venues, num_venues);
    
    // Verify no venue conflicts (no two matches at same venue, same time)
    for (int i = 0; i < schedule->fixture_count; i++) {
        for (int j = i + 1; j < schedule->fixture_count; j++) {
            if (schedule->fixtures[i].venue_id == schedule->fixtures[j].venue_id) {
                // Same venue → must have different time slots
                assert(schedule->fixtures[i].time_slot != schedule->fixtures[j].time_slot ||
                       schedule->fixtures[i].day != schedule->fixtures[j].day);
            }
        }
    }
    
    printf("✓ Venue allocation integration test passed\n");
}
```

### Integration Test Coverage

- **Data Loading:** CSV → Structures
- **Scheduling Pipeline:** Teams → Fixtures → Venue Assignment → Travel Optimization
- **Algorithm Composition:** Dijkstra result → Travel Optimizer → Schedule
- **Export Pipeline:** Schedule → CSV/JSON

---

## 3. End-to-End Tests (Scenario Level)

### Goal
Verify complete system behavior on realistic datasets

### Scenario 1: Normal Operation (20 Teams, 38 Rounds)

```bash
# tests/run_scenario_normal.sh
./build/league_scheduler \
    --teams data/laliga_teams.csv \
    --venues data/laliga_venues.csv \
    --distances data/laliga_distances.csv \
    --output results/scenario1.csv \
    --algorithm dijkstra
```

**Validation:**
- All 380 fixtures generated (20 teams × 19 opponents × 2)
- No team plays twice on same day
- Travel cost < threshold
- Fairness score > 0.8

### Scenario 2: Capacity Shock

```bash
./build/league_scheduler \
    --teams data/laliga_teams.csv \
    --venues data/reduced_venues.csv \
    --capacity-limit 50000 \
    --output results/scenario2.csv \
    --algorithm ford-fulkerson
```

**Validation:**
- Maximum flow constraints respected
- Some matches rescheduled or venues changed
- Report shows capacity utilization

### Scenario 3: Large Data Stress (100 Teams)

```bash
./build/league_scheduler \
    --teams data/large_teams.csv \
    --venues data/large_venues.csv \
    --output results/scenario3.csv \
    --benchmark
```

**Validation:**
- Completes within reasonable time (<5 minutes)
- Memory usage < 1GB
- Benchmark report shows algorithm comparison

---

## 4. Benchmark Tests

### Goal
Compare algorithmic strategies on same input

### Benchmark Framework

```c
// src/benchmark/benchmark_runner.h
typedef struct {
    char algorithm_name[50];
    double execution_time_ms;
    long memory_bytes;
    float solution_quality;  // problem-specific metric
    int is_correct;          // passed validation?
} BenchmarkResult;

void benchmark_shortest_path(Graph* g, int source, BenchmarkResult* results);
void benchmark_sorting(int* arr, int n, BenchmarkResult* results);
void print_benchmark_report(BenchmarkResult* results, int count);
```

### Example Benchmark: Shortest Path Algorithms

```c
void benchmark_shortest_path(Graph* g, int source, BenchmarkResult* results) {
    // Benchmark Dijkstra
    clock_t start = clock();
    ShortestPathResult* dijkstra_result = dijkstra(g, source);
    clock_t end = clock();
    
    results[0].execution_time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
    strcpy(results[0].algorithm_name, "Dijkstra");
    results[0].is_correct = validate_shortest_paths(dijkstra_result);
    
    // Benchmark Bellman-Ford
    start = clock();
    ShortestPathResult* bf_result = bellman_ford(g, source);
    end = clock();
    
    results[1].execution_time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
    strcpy(results[1].algorithm_name, "Bellman-Ford");
    results[1].is_correct = validate_shortest_paths(bf_result);
    
    // Compare results
    int results_match = 1;
    for (int i = 0; i < g->num_vertices; i++) {
        if (fabs(dijkstra_result->distances[i] - bf_result->distances[i]) > 0.01) {
            results_match = 0;
            break;
        }
    }
    
    printf("\n=== Shortest Path Benchmark ===\n");
    printf("Algorithm       | Time (ms) | Correct | Match\n");
    printf("----------------|-----------|---------|-------\n");
    printf("Dijkstra        | %8.2f  | %s     | -\n", 
           results[0].execution_time_ms, 
           results[0].is_correct ? "✓" : "✗");
    printf("Bellman-Ford    | %8.2f  | %s     | %s\n", 
           results[1].execution_time_ms,
           results[1].is_correct ? "✓" : "✗",
           results_match ? "✓" : "✗");
}
```

---

## 5. Test Data Sets

### Small (for unit/integration tests)
- **4 teams**, 2 venues, 12 fixtures
- Hand-verifiable results
- Files: `data/test/small_*.csv`

### Medium (for validation)
- **20 teams** (realistic LaLiga size), 10 venues, 380 fixtures
- Files: `data/laliga_*.csv`

### Large (for stress testing)
- **100 teams**, 50 venues, 9900 fixtures
- Generated programmatically
- Files: `data/large_*.csv`

### Adversarial (for edge cases)
- Disconnected graph (some venues unreachable)
- Zero-capacity venues
- Conflicting constraints
- Files: `data/adversarial_*.csv`

---

## 6. Test Automation

### Build & Run All Tests

```bash
# tests/run_tests.sh
#!/bin/bash

echo "Building project..."
make clean && make all

echo "Running unit tests..."
./build/test_sorting
./build/test_graphs
./build/test_dp
./build/test_flow

echo "Running integration tests..."
./build/test_scheduling
./build/test_data_loading

echo "Running scenarios..."
./tests/run_scenario_normal.sh
./tests/run_scenario_capacity.sh
./tests/run_scenario_large.sh

echo "Running benchmarks..."
./build/benchmark_runner

echo "✓ All tests completed!"
```

### Makefile Test Targets

```makefile
test: test_unit test_integration

test_unit:
	./build/test_sorting
	./build/test_graphs
	./build/test_dp

test_integration:
	./build/test_scheduling

test_scenarios:
	./tests/run_scenario_normal.sh
	./tests/run_scenario_capacity.sh
	./tests/run_scenario_large.sh

benchmark:
	./build/benchmark_runner
```

---

## Testing Checklist (Week 6)

- [ ] All unit tests passing (one per algorithm)
- [ ] Integration tests passing (data → scheduling → output)
- [ ] 3 scenarios run successfully
- [ ] Benchmark comparison for ≥2 algorithms completed
- [ ] Test coverage report generated
- [ ] Edge cases documented
- [ ] Adversarial inputs handled gracefully
- [ ] Memory leaks checked with valgrind (if available)

---

**Planning Complete!** Ready to start coding.

**Tokens Remaining:** 163091
