#ifndef CELEBRITY_H
#define CELEBRITY_H

#include "../../utils/common.h"

/**
 * @brief Function pointer type for "knows" or "plays_at" query:
 * Returns 1 if candidate a has played at / interacts with b, 0 otherwise.
 */
typedef int (*InteractionQueryFunc)(int a, int b, const void* context);

/**
 * @brief Find the "Celebrity" team or venue using Divide and Conquer.
 *
 * In sports scheduling context: Identifies a universal hub venue where all
 * teams must travel, or a central reference team played by everyone.
 * A celebrity C satisfies:
 * 1. For all i != C, interacts(i, C) == 1 (everyone plays at / knows C)
 * 2. For all j != C, interacts(C, j) == 0 (C doesn't play at / know anyone else)
 *
 * Divide and Conquer recurrence: T(n) = T(n-1) + O(1) => O(n) queries.
 *
 * @param n Total number of candidates (0 to n - 1)
 * @param query Callback function answering interacts(a, b, context)
 * @param context User data passed to query (e.g. 2D interaction matrix)
 * @return int Index of celebrity (0 <= id < n), or -1 if no celebrity exists.
 */
int find_celebrity_dc(int n, InteractionQueryFunc query, const void* context);

/**
 * @brief Helper for matrix-based celebrity lookup.
 *
 * @param matrix Row-major n x n adjacency/interaction matrix: matrix[i * n + j]
 * @param n Number of entities
 * @return int Index of celebrity, or -1 if none exists.
 */
int find_celebrity_matrix(const int* matrix, int n);

#endif /* CELEBRITY_H */
