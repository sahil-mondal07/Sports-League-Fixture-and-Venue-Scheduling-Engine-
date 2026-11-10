#ifndef STRING_MATCHER_H
#define STRING_MATCHER_H

#include "../../utils/common.h"

#define MAX_MATCH_RESULTS 100

/**
 * @brief Result structure for string matching positions.
 */
typedef struct {
    int indices[MAX_MATCH_RESULTS]; /**< Starting indices in text where pattern occurs */
    int count;                      /**< Total occurrences found */
} StringMatchResult;

/**
 * @brief Naive string matching algorithm.
 * Runtime: O((n - m + 1) * m).
 */
StringMatchResult string_match_naive(const char* text, const char* pattern);

/**
 * @brief Rabin-Karp string matching with rolling hash.
 * Runtime: O(n + m) average, O(n * m) worst case.
 */
StringMatchResult string_match_rabin_karp(const char* text, const char* pattern);

/**
 * @brief Knuth-Morris-Pratt (KMP) string matching algorithm.
 * Runtime: O(n + m).
 *
 * In sports scheduling context: Efficiently searches long tournament schedule
 * logs for team names, venue IDs, or derby fixtures.
 */
StringMatchResult string_match_kmp(const char* text, const char* pattern);

/**
 * @brief Finite Automaton string matching algorithm.
 * Runtime: O(m * |Sigma| + n).
 */
StringMatchResult string_match_fa(const char* text, const char* pattern);

/**
 * @brief Print string matching results.
 */
void print_string_match_result(const StringMatchResult* res, const char* pattern);

#endif /* STRING_MATCHER_H */
