#include "string_matcher.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

StringMatchResult string_match_naive(const char* text, const char* pattern) {
    StringMatchResult res;
    res.count = 0;
    if (!text || !pattern) return res;

    int n = (int)strlen(text);
    int m = (int)strlen(pattern);
    if (m == 0 || m > n) return res;

    for (int i = 0; i <= n - m; i++) {
        int j = 0;
        while (j < m && text[i + j] == pattern[j]) {
            j++;
        }
        if (j == m) {
            if (res.count < MAX_MATCH_RESULTS) {
                res.indices[res.count++] = i;
            }
        }
    }
    return res;
}

StringMatchResult string_match_rabin_karp(const char* text, const char* pattern) {
    StringMatchResult res;
    res.count = 0;
    if (!text || !pattern) return res;

    int n = (int)strlen(text);
    int m = (int)strlen(pattern);
    if (m == 0 || m > n) return res;

    long long d = 256; /* alphabet size */
    long long q = 101; /* prime number */
    long long p_hash = 0;
    long long t_hash = 0;
    long long h = 1;

    for (int i = 0; i < m - 1; i++) {
        h = (h * d) % q;
    }

    for (int i = 0; i < m; i++) {
        p_hash = (d * p_hash + (unsigned char)pattern[i]) % q;
        t_hash = (d * t_hash + (unsigned char)text[i]) % q;
    }

    for (int i = 0; i <= n - m; i++) {
        if (p_hash == t_hash) {
            int match = 1;
            for (int j = 0; j < m; j++) {
                if (text[i + j] != pattern[j]) {
                    match = 0;
                    break;
                }
            }
            if (match) {
                if (res.count < MAX_MATCH_RESULTS) {
                    res.indices[res.count++] = i;
                }
            }
        }

        if (i < n - m) {
            t_hash = (d * (t_hash - (unsigned char)text[i] * h) + (unsigned char)text[i + m]) % q;
            if (t_hash < 0) {
                t_hash += q;
            }
        }
    }

    return res;
}

/* KMP Prefix function (lps array) */
static void compute_lps(const char* pattern, int m, int* lps) {
    int len = 0;
    lps[0] = 0;
    int i = 1;
    while (i < m) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

StringMatchResult string_match_kmp(const char* text, const char* pattern) {
    StringMatchResult res;
    res.count = 0;
    if (!text || !pattern) return res;

    int n = (int)strlen(text);
    int m = (int)strlen(pattern);
    if (m == 0 || m > n) return res;

    int* lps = (int*)malloc((size_t)m * sizeof(int));
    if (!lps) return res;

    compute_lps(pattern, m, lps);

    int i = 0, j = 0;
    while (i < n) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }
        if (j == m) {
            if (res.count < MAX_MATCH_RESULTS) {
                res.indices[res.count++] = i - j;
            }
            j = lps[j - 1];
        } else if (i < n && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }

    free(lps);
    return res;
}

StringMatchResult string_match_fa(const char* text, const char* pattern) {
    /* Fallback wrapper to KMP for finite automaton matcher */
    return string_match_kmp(text, pattern);
}

void print_string_match_result(const StringMatchResult* res, const char* pattern) {
    if (!res) return;
    printf("\n=== String Matching Results for [%s] ===\n", pattern);
    printf("Matches Found: %d\n", res->count);
    for (int i = 0; i < res->count; i++) {
        printf("  Match at index: %d\n", res->indices[i]);
    }
}
