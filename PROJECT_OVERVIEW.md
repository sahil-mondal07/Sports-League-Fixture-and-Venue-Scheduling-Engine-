# Sports League Fixture and Venue Scheduling Engine

**Project 39 - DAA End Semester PBL**  
**Duration:** 8 weeks | **Due:** End of November 2026  
**Language:** C/C++ | **Evaluation:** Hackathon-style (50% internal marks)

## Domain: Generic Sports League Scheduling (Any Sport)

### Core Problem
Generate fair fixtures for any sport league with minimized travel costs and scheduling conflicts, subject to explicit constraints and validation rules. Works for football, basketball, volleyball, hockey, tennis, etc.

### System Components

#### 1. Algorithm Library (Mandatory Implementations)

**Sorting & Searching**
- Merge Sort - sorting teams by rank, venues by capacity
- Quick Sort - sorting fixtures by date/time
- Binary Search (iterative & recursive) - finding available time slots

**Divide & Conquer**
- Maximum Subarray - analyzing consecutive home/away game sequences
- Fast Powering - rating calculations
- Celebrity Discovery - finding key teams/venues
- Strassen Matrix Multiplication - match outcome predictions

**Greedy Algorithms**
- Activity Selection - scheduling non-overlapping matches
- Interval Partitioning - assigning time slots to minimize conflicts
- Huffman Coding - compressing fixture/result data
- Dijkstra - shortest travel path between venues
- Prim & Kruskal - minimum spanning tree for venue connectivity

**Dynamic Programming**
- 0/1 Knapsack - selecting matches within broadcast capacity
- Matrix Chain Multiplication - optimizing multi-stage computation
- LCS - comparing fixture patterns
- Optimal BST - efficient team/venue lookup
- Bellman-Ford - negative-weight travel costs
- Floyd-Warshall - all-pairs shortest paths between venues
- Bounded TSP - referee/crew travel optimization

**Backtracking**
- N-Queens variant - scheduling constraints
- Subset Problems - team grouping with constraints

**Branch & Bound**
- FIFO, LIFO, Least-Cost - exact scheduling decisions

**Network Flow**
- Ford-Fulkerson - maximum capacity allocation (broadcast slots, venue capacity)
- Maximum Bipartite Matching - team-to-venue assignments

**String Matching**
- Naive, Rabin-Karp, Finite-Automaton, KMP - team name/event sequence matching

**Number Theory**
- Chinese Remainder Theorem - scheduling with periodic constraints

#### 2. Data Structures
- Teams (ID, name, stadium, location coordinates)
- Venues (ID, name, capacity, location, availability)
- Fixtures (round, home team, away team, venue, date/time)
- Travel distances (weighted graph between venues)
- Broadcast slots (capacity-constrained resource)

#### 3. Core Scheduling Engine
- Fixture Generator
- Venue Allocator
- Travel Cost Optimizer
- Conflict Resolver
- Fairness Validator

#### 4. Interface & Output
- Menu-driven CLI
- Scenario runner (normal, constraint shock, large-data stress)
- Benchmark mode (compare algorithms)
- CSV/JSON export
- Fairness & cost reports

#### 5. Testing & Validation
- Unit tests for each algorithm
- Integration tests for scheduling workflows
- Edge cases & invalid inputs
- Small-instance oracles
- Regression data

### Deliverables
1. Portable C/C++ application
2. Reusable algorithm library with consistent interfaces
3. Makefile/CMake build system
4. Sample datasets (small, medium, large, adversarial)
5. User guide & design documentation
6. Complexity analysis report
7. Demonstration script
8. Git repository with release tag

### Demonstration Requirements
- 3 realistic scenarios
- Compare 2+ algorithmic strategies on same input
- Explain correctness, time, memory, solution quality differences

---
**Timeline:** ~6 weeks remaining (started Oct 20, 2026)
