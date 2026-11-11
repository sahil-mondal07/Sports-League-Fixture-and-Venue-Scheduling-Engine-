# Bug Fix Summary - Project 39 Sports League Scheduling Engine

## Fixed 7 Buggy Modules

### 1. dp_knapsack.c - Knapsack (3 issues fixed)
- **Heap-use-after-free**: `return dp[capacity]` executed after `free(dp)` - moved return to before free
- **Wrong selected-items output**: Traceback logic rewritten using `choice[]` array to correctly trace which items were selected in the optimal solution
- **Wrong return value in small-capacity cases**: Now returns the saved result before freeing memory

### 2. dp_matrix_chain.c - Matrix Chain (2 issues fixed)
- **Incomplete selected_out output**: Now traces ALL split points from the s table using stack-based recursion, storing n-1 split points instead of just the top-level one
- **Print function**: Fixed `matrix_chain_print_optimal_parens` to correctly iterate through all splits using the stack, printing full parenthesization
- **Use-after-free on return**: Fixed by saving result before freeing m array

### 3. huffman.c - Huffman Coding (1 issue fixed)
- **code_lens[] never set**: `huffman_tree_generate_codes` now computes actual code lengths from the generated codes array after traversal
- Round-trip encode/decode now works correctly with proper code lengths

### 4. interval_partitioning.c - Interval Partitioning (1 issue fixed)
- **Wrong greedy strategy**: Changed sort from finish-time to start-time (optimal greedy algorithm)
- Was using O(n²) bubble sort, now uses the same pattern (minor optimization)
- **Result**: Will use minimal resources (optimal greedy) instead of near-optimal but suboptimal

### 5. dp_lcs.c - Longest Common Subsequence (1 issue fixed)
- **Returns -1 for empty sequences**: Changed to return 0 (correct LCS length for empty sequences)
- Header documentation already states "returns 0", now matches the contract

### 6. string_matcher.h - String Matching (1 issue fixed)
- **Silently caps results at 100**: Changed `MAX_MATCH_RESULTS` from 100 to 10000
- Large-data stress tests will now report all 500+ matches correctly

### 7. crt.c - Chinese Remainder Theorem (1 issue fixed)
- **Signed-integer overflow UB**: Now uses `__int128` for product computation to avoid overflow with large moduli
- Previously used `long long` multiplication which overflows with moduli > ~10^4

### 8. subset_sum.c - Subset Sum (1 issue fixed)
- **Returns 0 solutions for target 0**: Now returns the empty set as a valid solution (1 solution with count=0)
- Added documentation convention note

### 9. .gitattributes (1 issue fixed)
- **Windows line endings**: Created `.gitattributes` with `* text=auto eol=lf` to prevent git from showing whole files as modified

## Demonstration Scenarios Ready

All three mandatory demonstration scenarios from the project syllabus are now supported:

1. **Scenario 1: Normal Operation** - 20 LaLiga teams, Dijkstra vs Bellman-Ford travel comparison
2. **Scenario 2: Capacity Shock** - Reduced stadium capacities, Ford-Fulkerson max flow & knapsack broadcast selection
3. **Scenario 3: Large Data Stress** - Floyd-Warshall all-pairs, Quick Sort vs Merge Sort benchmarks (now reports full match results)

## Build/Run Commands

```bash
make              # Build the project
make run          # Run the scheduler CLI
make test         # Run all tests
make clean        # Clean build artifacts
```

## Files Modified (9 files)
- `src/algorithms/dp/dp_knapsack.c` - Fixed use-after-free + traceback
- `src/algorithms/dp/dp_matrix_chain.c` - Fix split points + print function + use-after-free
- `src/algorithms/greedy/huffman.c` - Generate proper code lengths
- `src/algorithms/greedy/interval_partitioning.c` - Sort by start time (optimal greedy)
- `src/algorithms/dp/dp_lcs.c` - Return 0 for empty sequences
- `src/algorithms/string_matching/string_matcher.h` - Increase MAX_MATCH_RESULTS to 10000
- `src/algorithms/number_theory/crt.c` - Use __int128 to prevent overflow
- `src/algorithms/backtracking/subset_sum.c` - Return empty set for target 0
- `.gitattributes` - New file for line ending normalization