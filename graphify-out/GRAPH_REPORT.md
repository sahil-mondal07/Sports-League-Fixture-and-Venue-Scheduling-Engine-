# Graph Report - PBL  (2026-10-20)

## Corpus Check
- 6 files · ~5,297 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 63 nodes · 90 edges · 5 communities
- Extraction: 78% EXTRACTED · 22% INFERRED · 0% AMBIGUOUS · INFERRED: 20 edges (avg confidence: 0.8)
- Token cost: 0 input · 0 output

## Community Hubs (Navigation)
- Project area 1
- Project area 2
- Project area 3
- Project area 4
- Project area 5

## God Nodes (most connected - your core abstractions)
1. `ALGORITHM_MAPPING` - 18 edges
2. `IMPLEMENTATION_TIMELINE` - 12 edges
3. `ARCHITECTURE` - 11 edges
4. `TESTING_STRATEGY` - 11 edges
5. `DATA_STRUCTURES` - 10 edges
6. `PROJECT_OVERVIEW` - 8 edges
7. `Scheduling Engine` - 7 edges
8. `Travel Optimization` - 7 edges
9. `Fairness Validation` - 4 edges
10. `Dijkstra` - 4 edges

## Surprising Connections (you probably didn't know these)
- `Scheduling Engine` --drives--> `Travel Optimization`  [INFERRED]
  PROJECT_OVERVIEW.md → ALGORITHM_MAPPING.md
- `Scheduling Engine` --drives--> `Venue Allocation`  [INFERRED]
  PROJECT_OVERVIEW.md → ALGORITHM_MAPPING.md
- `Scheduling Engine` --drives--> `Fixture Generation`  [INFERRED]
  PROJECT_OVERVIEW.md → ALGORITHM_MAPPING.md
- `Benchmark Mode` --measured_by--> `Benchmark Tests`  [INFERRED]
  PROJECT_OVERVIEW.md → TESTING_STRATEGY.md
- `Fairness Validation` --validates--> `Schedule`  [INFERRED]
  PROJECT_OVERVIEW.md → DATA_STRUCTURES.md

## Communities (5 total, 0 thin omitted)

### Community 0 - "Project area 1"
Cohesion: 0.14
Nodes (15): Algorithm Library, Benchmark and Testing, CLI Interface, Conflict Resolver, Data Layer, Data Structures, Fairness Validator, Fixture Generator (+7 more)

### Community 1 - "Project area 2"
Cohesion: 0.17
Nodes (15): Fairness Validation, Fixture, Fixture Generation, Kruskal, MST Result, Prim, Priority Queue, Schedule (+7 more)

### Community 2 - "Project area 3"
Cohesion: 0.21
Nodes (14): Activity Selection, Bellman-Ford, Binary Search, Bipartite Matching, Bounded TSP, Broadcast Capacity, Chinese Remainder Theorem, Dijkstra (+6 more)

### Community 3 - "Project area 4"
Cohesion: 0.20
Nodes (10): Backtracking, Branch and Bound, Divide and Conquer, Dynamic Programming, Ford-Fulkerson, Foundation, Graph Algorithms, Greedy Algorithms (+2 more)

### Community 4 - "Project area 5"
Cohesion: 0.28
Nodes (9): Benchmark Mode, Benchmark Tests, End-to-End Tests, Integration Tests, Merge Sort, Scenario Testing, Testing and Documentation, Unit Tests (+1 more)

## Knowledge Gaps
- **27 isolated node(s):** `Football Scheduling`, `Data Structures`, `Travel Cost`, `Data Layer`, `Utilities` (+22 more)
  These have ≤1 connection - possible missing edges or undocumented components.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Scheduling Engine` connect `Project area 2` to `Project area 1`, `Project area 3`?**
  _High betweenness centrality (0.412) - this node is a cross-community bridge._
- **Why does `ALGORITHM_MAPPING` connect `Project area 3` to `Project area 2`, `Project area 4`, `Project area 5`?**
  _High betweenness centrality (0.324) - this node is a cross-community bridge._
- **Why does `ARCHITECTURE` connect `Project area 1` to `Project area 2`?**
  _High betweenness centrality (0.261) - this node is a cross-community bridge._
- **What connects `Football Scheduling`, `Data Structures`, `Travel Cost` to the rest of the system?**
  _27 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `Project area 1` be split into smaller, more focused modules?**
  _Cohesion score 0.14285714285714285 - nodes in this community are weakly interconnected._