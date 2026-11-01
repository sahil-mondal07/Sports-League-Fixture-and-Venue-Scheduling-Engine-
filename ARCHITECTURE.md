# Project Architecture

## High-Level System Design

```
┌─────────────────────────────────────────────────────────────────┐
│                         CLI INTERFACE                            │
│  (Menu System, Scenario Runner, Benchmark Mode, Reports)        │
└────────────────────┬────────────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────────────┐
│                   SCHEDULING ENGINE                              │
│  • Fixture Generator    • Conflict Resolver                     │
│  • Venue Allocator      • Fairness Validator                    │
│  • Travel Optimizer     • Cost Calculator                       │
└────────────────────┬────────────────────────────────────────────┘
                     │
        ┌────────────┼────────────┐
        │            │            │
┌───────▼──────┐ ┌──▼─────────┐ ┌▼────────────────┐
│   DATA       │ │ ALGORITHM  │ │   UTILITIES     │
│   LAYER      │ │  LIBRARY   │ │                 │
└──────────────┘ └────────────┘ └─────────────────┘
```

## Directory Structure

```
PBL/
├── src/
│   ├── main.c                          # Entry point, CLI menu
│   ├── data/
│   │   ├── team.h/c                    # Team structure & operations
│   │   ├── venue.h/c                   # Venue structure & operations
│   │   ├── fixture.h/c                 # Fixture structure & operations
│   │   ├── graph.h/c                   # Graph for venue connectivity
│   │   └── loader.h/c                  # Load data from CSV/files
│   │
│   ├── algorithms/
│   │   ├── sorting/
│   │   │   ├── merge_sort.h/c
│   │   │   ├── quick_sort.h/c
│   │   │   └── binary_search.h/c
│   │   │
│   │   ├── divide_conquer/
│   │   │   ├── max_subarray.h/c
│   │   │   ├── fast_power.h/c
│   │   │   ├── celebrity.h/c
│   │   │   └── strassen.h/c
│   │   │
│   │   ├── greedy/
│   │   │   ├── activity_selection.h/c
│   │   │   ├── interval_partition.h/c
│   │   │   ├── huffman.h/c
│   │   │   ├── dijkstra.h/c
│   │   │   ├── prim.h/c
│   │   │   └── kruskal.h/c
│   │   │
│   │   ├── dynamic_programming/
│   │   │   ├── knapsack.h/c
│   │   │   ├── matrix_chain.h/c
│   │   │   ├── lcs.h/c
│   │   │   ├── optimal_bst.h/c
│   │   │   ├── bellman_ford.h/c
│   │   │   ├── floyd_warshall.h/c
│   │   │   └── tsp.h/c
│   │   │
│   │   ├── backtracking/
│   │   │   ├── nqueens.h/c
│   │   │   └── subset_sum.h/c
│   │   │
│   │   ├── branch_bound/
│   │   │   └── scheduling_bb.h/c
│   │   │
│   │   ├── flow/
│   │   │   ├── ford_fulkerson.h/c
│   │   │   └── bipartite_matching.h/c
│   │   │
│   │   └── string_matching/
│   │       ├── naive.h/c
│   │       ├── rabin_karp.h/c
│   │       ├── kmp.h/c
│   │       └── finite_automaton.h/c
│   │
│   ├── scheduling/
│   │   ├── fixture_generator.h/c       # Core fixture generation logic
│   │   ├── venue_allocator.h/c         # Assign venues to matches
│   │   ├── travel_optimizer.h/c        # Minimize travel costs
│   │   ├── conflict_resolver.h/c       # Handle scheduling conflicts
│   │   └── fairness_validator.h/c      # Validate fairness metrics
│   │
│   ├── benchmark/
│   │   ├── timer.h/c                   # Time measurement utilities
│   │   ├── memory_tracker.h/c          # Memory usage tracking
│   │   └── benchmark_runner.h/c        # Compare algorithms
│   │
│   └── utils/
│       ├── common.h                    # Common definitions/macros
│       ├── file_io.h/c                 # File reading/writing
│       └── chinese_remainder.h/c       # CRT utility
│
├── data/
│   ├── teams.csv                       # Team data
│   ├── venues.csv                      # Venue data
│   ├── distances.csv                   # Travel distances matrix
│   └── constraints.csv                 # Scheduling constraints
│
├── tests/
│   ├── test_sorting.c
│   ├── test_graphs.c
│   ├── test_scheduling.c
│   └── run_tests.sh
│
├── docs/
│   ├── USER_GUIDE.md
│   ├── DESIGN_DOC.md
│   ├── COMPLEXITY_REPORT.md
│   └── ALGORITHM_MAPPING.md
│
├── scripts/
│   ├── demo.sh                         # Demonstration script
│   └── generate_data.py                # Generate test datasets
│
├── Makefile
├── CMakeLists.txt
├── README.md
└── PROJECT_OVERVIEW.md
```

## Module Responsibilities

### Data Layer
**What it does:** Defines core data structures (teams, venues, fixtures) and provides operations to create, modify, and access them.

**Why separate:** Clean separation between data representation and algorithm logic. Easy to modify data structures without touching algorithms.

### Algorithm Library
**What it does:** Self-contained implementations of all required DAA algorithms. Each algorithm is independent and reusable.

**Why separate:** Modularity. Each algorithm can be tested, benchmarked, and understood independently. You "own" each algorithm implementation.

### Scheduling Engine
**What it does:** Uses algorithms from the library to solve actual scheduling problems. This is where algorithms meet the real-world problem.

**Why separate:** Business logic layer. Translates football scheduling requirements into algorithm inputs/outputs.

### CLI Interface
**What it does:** User interaction point. Menu system for selecting scenarios, running benchmarks, viewing reports.

**Why separate:** User experience layer. Can be replaced with GUI later without touching core logic.

### Benchmark & Testing
**What it does:** Validates correctness and measures performance. Compares algorithmic strategies.

**Why separate:** Quality assurance. Proves your implementation works and shows you understand complexity analysis.

---

**Next Step:** Run `/graphify` to create interactive visualization of this architecture.
