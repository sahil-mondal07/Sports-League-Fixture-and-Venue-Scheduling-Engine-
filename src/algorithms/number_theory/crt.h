#ifndef CRT_H
#define CRT_H

#include "../../utils/common.h"

/**
 * @brief Result structure for Chinese Remainder Theorem.
 */
typedef struct {
    long long x;       /**< Smallest non-negative solution */
    long long mod;     /**< Product of moduli (LCM if pairwise coprime) */
    int success;       /**< 1 if solution exists, 0 otherwise */
} CRTResult;

/**
 * @brief Solve system of linear congruences using Chinese Remainder Theorem.
 *
 * x = a_i (mod m_i) for i = 0 .. k-1.
 *
 * In sports scheduling context: Resolves periodic constraints where venues,
 * broadcast slots, and rest cycles repeat on modular intervals.
 *
 * @param remainders Array of remainders (a_i)
 * @param moduli Array of pairwise coprime moduli (m_i)
 * @param k Number of congruences
 * @return CRTResult containing smallest non-negative solution and combined modulus.
 */
CRTResult chinese_remainder_theorem(const long long* remainders, const long long* moduli, int k);

/**
 * @brief Print CRT result.
 */
void print_crt_result(const CRTResult* res);

#endif /* CRT_H */
