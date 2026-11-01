# Implementation Timeline

**Start Date:** October 20, 2026  
**Due Date:** End of November 2026  
**Total Time:** ~6 weeks

---

## Week 1: Foundation (Oct 20-26)

### Goals
- Set up project structure
- Implement core data structures
- Basic file I/O and CSV loading

### Deliverables
- [ ] Complete directory structure created
- [ ] `team.h/c`, `venue.h/c`, `fixture.h/c` implemented
- [ ] `graph.h/c` for venue connectivity
- [ ] `loader.h/c` for reading CSV files
- [ ] Sample data files (teams.csv, venues.csv, distances.csv)
- [ ] Makefile for compilation
- [ ] Basic `main.c` with menu skeleton

### Priority Algorithms (Start Learning)
- Merge Sort
- Binary Search
- Dijkstra

---

## Week 2: Sorting & Graph Basics (Oct 27 - Nov 2)

### Goals
- Complete sorting/searching algorithms
- Implement basic graph algorithms
- Start scheduling engine foundation

### Deliverables
- [ ] `merge_sort.c`, `quick_sort.c`, `binary_search.c` - tested
- [ ] `dijkstra.c`, `prim.c`, `kruskal.c` - tested
- [ ] `travel_optimizer.c` using Dijkstra
- [ ] Unit tests for sorting and graph algorithms
- [ ] Benchmark runner skeleton

### Demo Checkpoint
- Show sorted teams, shortest path between venues

---

## Week 3: Greedy & Dynamic Programming (Nov 3-9)

### Goals
- Implement greedy algorithms
- Start DP algorithms
- Build venue allocator

### Deliverables
- [ ] `activity_selection.c`, `interval_partition.c`
- [ ] `huffman.c` for compression
- [ ] `knapsack.c`, `bellman_ford.c`, `floyd_warshall.c`
- [ ] `venue_allocator.c` using activity selection
- [ ] Integration tests for scheduling components

### Demo Checkpoint
- Schedule non-overlapping matches
- Show all-pairs shortest paths

---

## Week 4: Divide-Conquer & Flow (Nov 10-16)

### Goals
- Complete divide-conquer algorithms
- Implement network flow
- Build fixture generator

### Deliverables
- [ ] `max_subarray.c`, `fast_power.c`, `strassen.c`
- [ ] `ford_fulkerson.c`, `bipartite_matching.c`
- [ ] `fixture_generator.c` core logic
- [ ] `conflict_resolver.c`
- [ ] CSV export functionality

### Demo Checkpoint
- Generate complete fixture list
- Show capacity allocation

---

## Week 5: Backtracking, B&B, Strings (Nov 17-23)

### Goals
- Implement remaining algorithms
- Complete scheduling engine
- Full system integration

### Deliverables
- [ ] `nqueens.c`, `subset_sum.c`
- [ ] `scheduling_bb.c` (FIFO, LIFO, Least-Cost)
- [ ] `tsp.c`, `lcs.c`, `optimal_bst.c`, `matrix_chain.c`
- [ ] All string matching algorithms
- [ ] `chinese_remainder.c`
- [ ] `fairness_validator.c`
- [ ] Complete CLI menu system
- [ ] All 3 demonstration scenarios working

### Demo Checkpoint
- Run all scenarios
- Compare 2+ algorithms on same input

---

## Week 6: Testing, Documentation & Polish (Nov 24-30)

### Goals
- Comprehensive testing
- Complete documentation
- Prepare demonstration
- Buffer for fixes

### Deliverables
- [ ] Complete test suite (all algorithms tested)
- [ ] Benchmark results on small/medium/large datasets
- [ ] USER_GUIDE.md completed
- [ ] DESIGN_DOC.md completed
- [ ] COMPLEXITY_REPORT.md with Big-O analysis
- [ ] Demonstration script polished
- [ ] Code cleanup and comments
- [ ] Git repository cleaned, tagged for release
- [ ] Practice presentation

### Final Deliverables Checklist
- [ ] Working C/C++ application
- [ ] All mandatory algorithms implemented
- [ ] 3 realistic scenarios demonstrable
- [ ] Benchmark comparisons ready
- [ ] Full documentation package
- [ ] Clean Git history with release tag

---

## Daily Work Pattern

### Learning Phase (First 3 weeks)
- **Morning:** Understand 1 new algorithm (theory + pseudocode)
- **Afternoon:** Implement algorithm in C
- **Evening:** Write tests, verify correctness

### Integration Phase (Weeks 4-5)
- **Morning:** Connect algorithms to scheduling engine
- **Afternoon:** Build scenarios and test integration
- **Evening:** Debug, benchmark, document

### Polish Phase (Week 6)
- **Daily:** Test → Fix → Document → Practice demo

---

## Risk Mitigation

### If Behind Schedule
1. **Priority 1 Algorithms:** Dijkstra, Merge Sort, Ford-Fulkerson, Activity Selection
2. **Priority 2 Algorithms:** Floyd-Warshall, Knapsack, Bipartite Matching
3. **Priority 3 Algorithms:** String matching, CRT, Strassen

### Red Flags
- Week 2 and no working graph algorithms → Need help
- Week 4 and fixture generation not working → Simplify constraints
- Week 5 and test coverage <50% → Cut lower-priority algorithms

---

**Next Action:** Create `src/` directory and start with data structures (Team, Venue, Fixture).
