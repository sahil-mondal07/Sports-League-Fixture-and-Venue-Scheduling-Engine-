#ifndef DP_LCS_H
#define DP_LCS_H

#include "../../utils/common.h"

/**
 * @brief Represents a position in the LCS tracking.
 * Used internally for traceback to reconstruct the actual subsequence.
 */
typedef struct {
    int i;   /**< Index in the first sequence (1-based) */
    int j;   /**< Index in the second sequence (1-based) */
} LCSPosition;

/**
 * @brief Compute the Longest Common Subsequence (LCS) between two sequences.
 *
 * Finds the longest subsequence common to both input sequences.
 * A subsequence is obtained by deleting some elements (possibly none)
 * from the original sequence, preserving the order of the remaining elements.
 *
 * In the sports scheduling context: LCS can compare two schedules or
 * sequences of fixture events to find common patterns, shared time
 * slots, overlapping venue usage, or matching team interaction sequences
 * across different rounds or tournament structures.
 *
 * @param seq1 First sequence (array of integers or IDs). The array
 *             should have at least m elements; values are compared
 *             for equality.
 * @param m Number of elements in seq1.
 * @param seq2 Second sequence (array of integers or IDs). The array
 *             should have at least n elements.
 * @param n Number of elements in seq2.
 * @param out_length Output: length of the longest common subsequence.
 *                   Returns 0 if there is no common element.
 * @param out_selected_out Pre-allocated array of size min(m,n) to store
 *                         the indices of elements in seq1 that form the
 *                         LCS (their positions in seq1). May be NULL if
 *                         only the length is needed.
 * @return int Length of the LCS. Returns 0 if no common subsequence exists,
 *             or -1 on invalid input (m <= 0 or n <= 0 or NULL seq arrays).
 */
int lcs_length(const int* seq1, int m, const int* seq2, int n,
               int* out_length, int* out_selected_out);

/**
 * @brief Print the LCS solution in human-readable form.
 *
 * Displays both input sequences and the longest common subsequence found,
 * showing which elements match and their positions.
 *
 * @param seq1 First sequence array
 * @param m Number of elements in seq1
 * @param seq2 Second sequence array
 * @param n Number of elements in seq2
 * @param selected Array of positions in seq1 that form the LCS
 *               (output from lcs_length, or the caller's reconstruction).
 * @param selected_len Length of the selected array (should match the LCS length)
 */
void lcs_print_solution(const int* seq1, int m, const int* seq2, int n,
                        const int* selected, int selected_len);

#endif /* DP_LCS_H */