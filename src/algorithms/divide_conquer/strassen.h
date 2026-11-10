#ifndef STRASSEN_H
#define STRASSEN_H

#include "../../utils/common.h"

/**
 * @brief Classical O(n^3) matrix multiplication: C = A * B.
 *
 * @param A Row-major matrix of size n x n
 * @param B Row-major matrix of size n x n
 * @param C Output row-major matrix of size n x n (pre-allocated)
 * @param n Dimension of matrices
 */
void matrix_multiply_classical(const double* A, const double* B, double* C, int n);

/**
 * @brief Strassen's matrix multiplication using Divide and Conquer.
 *
 * Recurrence: T(n) = 7T(n/2) + O(n^2) => O(n^log2(7)) ≈ O(n^2.807).
 *
 * In sports scheduling context: Optimizes large-scale predictive calculations,
 * multi-team stat projection matrices, and distance/connectivity powers.
 *
 * Automatically handles dimensions that are not powers of two by padding
 * to the nearest power of 2, and uses a cutoff threshold to revert to
 * classical multiplication for smaller blocks.
 *
 * @param A Row-major matrix of size n x n
 * @param B Row-major matrix of size n x n
 * @param C Output row-major matrix of size n x n (pre-allocated)
 * @param n Dimension of input matrices
 * @return int 0 on success, -1 on invalid arguments or allocation failure.
 */
int matrix_multiply_strassen(const double* A, const double* B, double* C, int n);

#endif /* STRASSEN_H */
