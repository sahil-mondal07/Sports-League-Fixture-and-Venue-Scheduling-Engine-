#ifndef BINARY_SEARCH_H
#define BINARY_SEARCH_H

#include "../../data/fixture.h"
#include "../../utils/common.h"

// ============================================================================
// BINARY SEARCH - Iterative & Recursive Variants
// ============================================================================
// Used to find fixtures by round, day, or other sorted criteria
// Time Complexity: O(log n)
// Space Complexity: O(1) iterative, O(log n) recursive (call stack)
// ============================================================================

// ============ ITERATIVE BINARY SEARCH ============

// Search for a fixture by round (assumes fixtures sorted by round)
// Returns index if found, -1 if not found
int binary_search_fixture_by_round_iterative(Fixture* fixtures, int count, int target_round);

// Search for a fixture by day (assumes fixtures sorted by day)
// Returns index if found, -1 if not found
int binary_search_fixture_by_day_iterative(Fixture* fixtures, int count, int target_day);

// Generic iterative binary search on sorted integer array
// Returns index if found, -1 if not found
int binary_search_iterative(int* arr, int count, int target);

// ============ RECURSIVE BINARY SEARCH ============

// Search for a fixture by round (recursive wrapper)
int binary_search_fixture_by_round_recursive(Fixture* fixtures, int count, int target_round);

// Search for a fixture by day (recursive wrapper)
int binary_search_fixture_by_day_recursive(Fixture* fixtures, int count, int target_day);

// Generic recursive binary search helper
// left, right = search bounds (inclusive)
// Returns index if found, -1 if not found
int binary_search_recursive_helper(int* arr, int left, int right, int target);

// ============ UTILITY FUNCTIONS ============

// Print search result
void print_binary_search_result(Fixture* fixtures, int result, int target, const char* search_type);

// Demo function showing all binary search variants
void demo_binary_search(Fixture* fixtures, int count);

#endif
