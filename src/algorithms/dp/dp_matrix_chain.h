#ifndef DP_MATRIX_CHAIN_H
#define DP_MATRIX_CHAIN_H

#include "../../utils/common.h"

/**
 * @brief Represents a matrix in a chain with dimensions rows x cols.
 */
typedef struct {
    int rows;   /**< Number of rows */
    int cols;   /**< Number of columns */
} MatrixDim;

/**
 * @brief Solve the Matrix Chain Multiplication problem using dynamic programming.
 *
 * Finds the minimum number of scalar multiplications needed to multiply
 * a chain of matrices, and determines the optimal parenthesization order.
 *
 * In the sports scheduling context: This can optimize repeated matrix
 * operations on schedule data (e.g., combining rating projections,
 * computing combined statistics across matches, or transforming
 * venue-coordinate matrices).
 *
 * @param p Array of dimensions such that matrix i has dimensions p[i-1] x p[i].
 *          Size of array must be n+1 for n matrices.
 * @param n Number of matrices in the chain (p has n+1 elements).
 * @param out_selected_out Pre-allocated array of size n-1.
 *                         out_selected_out[k] stores the split point
 *                         (1 <= k < n) for the optimal parenthesization
 *                         at level k of the recursion tree.
 *                         If NULL, only the min cost is computed.
 * @return int Minimum number of scalar multiplications needed.
 *             Returns -1 on error (invalid dimensions, n <= 0).
 */
int matrix_chain_order(const int* p, int n, int* out_selected_out);

/**
 * @brief Print the optimal parenthesization of the matrix chain.
 *
 * Displays the optimal way to multiply the matrix chain using the
 * standard notation: (A1 × (A2 × A3)) etc.
 *
 * @param n Number of matrices in the chain.
 * @param s 2D table of split points (as returned by matrix_chain_order
 *          or computed internally). If NULL, a default split table
 *          based on n matrices with even splits is used.
 * @param name Base name for matrices (e.g., "A"); matrices will be
 *             referred to as name1, name2, ..., namen in the output.
 */
void matrix_chain_print_optimal_parens(int n, const int* s, const char* name);

#endif /* DP_MATRIX_CHAIN_H */