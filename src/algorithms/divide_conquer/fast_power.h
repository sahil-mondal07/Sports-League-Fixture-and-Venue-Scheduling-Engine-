#ifndef FAST_POWER_H
#define FAST_POWER_H

#include "../../utils/common.h"

/**
 * @brief Fast scalar exponentiation: base^exp using Divide and Conquer.
 *
 * Recurrence: T(n) = T(n/2) + O(1) => O(log exp) operations.
 *
 * In sports scheduling context: Projects compounding team rating growth,
 * momentum multipliers, or ranking coefficient adjustments over n rounds.
 *
 * @param base Base value
 * @param exp Exponent (non-negative integer)
 * @return double Result of base^exp. If exp < 0, returns 1.0 / fast_power(base, -exp).
 */
double fast_power(double base, int exp);

/**
 * @brief Fast modular exponentiation: (base^exp) % mod.
 *
 * Useful for hash generation, permutation seed indexing, and CRT scheduling.
 *
 * @param base Non-negative integer base
 * @param exp Non-negative integer exponent
 * @param mod Positive modulus
 * @return long long (base^exp) % mod, or -1 on invalid input (mod <= 0 or exp < 0).
 */
long long fast_power_mod(long long base, long long exp, long long mod);

/**
 * @brief Multiply two square matrices of dimension dim x dim: C = A * B.
 *
 * Helper for matrix exponentiation.
 */
void matrix_multiply_square(const double* A, const double* B, double* C, int dim);

/**
 * @brief Fast matrix exponentiation: Result = M^exp for a dim x dim matrix.
 *
 * Uses repeated squaring in O(dim^3 * log exp).
 *
 * In sports scheduling context: Computes state transition probabilities
 * (e.g. Markov chain of team venue occupancy or matchup odds) after exp rounds.
 *
 * @param M Input matrix of size dim x dim (row-major: M[i * dim + j])
 * @param dim Matrix dimension (number of rows/cols)
 * @param exp Exponent (exp >= 0)
 * @param result Pre-allocated output matrix of size dim x dim
 * @return int 0 on success, -1 on invalid parameters.
 */
int matrix_power(const double* M, int dim, int exp, double* result);

#endif /* FAST_POWER_H */
