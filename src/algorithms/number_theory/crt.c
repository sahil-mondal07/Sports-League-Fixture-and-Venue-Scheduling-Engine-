#include "crt.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* Extended Euclidean Algorithm */
static long long extended_gcd(long long a, long long b, long long* x, long long* y) {
    if (a == 0) {
        *x = 0;
        *y = 1;
        return b;
    }
    long long x1, y1;
    long long gcd = extended_gcd(b % a, a, &x1, &y1);
    *x = y1 - (b / a) * x1;
    *y = x1;
    return gcd;
}

/* Modular multiplicative inverse */
static long long mod_inverse(long long a, long long m) {
    long long x, y;
    long long gcd = extended_gcd(a, m, &x, &y);
    if (gcd != 1) {
        return -1; /* inverse does not exist */
    }
    /* Ensure positive result */
    long long result = (x % m + m) % m;
    return result;
}

/* Chinese Remainder Theorem */
CRTResult chinese_remainder_theorem(const long long* remainders, const long long* moduli, int k) {
    CRTResult res;
    res.x = -1;
    res.mod = 0;
    res.success = 0;

    if (!remainders || !moduli || k <= 0) {
        return res;
    }

    /* Check for non-positive moduli */
    for (int i = 0; i < k; i++) {
        if (moduli[i] <= 0) return res;
    }

    /* Compute product of moduli with overflow check */
    long long prod = 1;
    for (int i = 0; i < k; i++) {
        if (__builtin_mul_overflow(prod, moduli[i], &prod)) {
            /* Overflow detected - return failure */
            res.success = 0;
            return res;
        }
    }

    /* Pairwise-coprime CRT accumulation; reduce each product modulo prod. */
    unsigned long long modulus = (unsigned long long)prod;
    unsigned long long x = 0;
    for (int i = 0; i < k; i++) {
        long long ni = prod / moduli[i];
        long long yi = mod_inverse(ni, moduli[i]);
        if (yi == -1) return res;
        unsigned long long a = (unsigned long long)((remainders[i] % prod + prod) % prod);
        unsigned long long b = (unsigned long long)ni;
        unsigned long long term = 0;
        /* Overflow-safe modular multiplication by repeated doubling. */
        while (b) {
            if (b & 1ULL) term = term >= modulus - a ? term - (modulus - a) : term + a;
            a = a >= modulus - a ? a - (modulus - a) : a + a;
            b >>= 1;
        }
        b = (unsigned long long)yi;
        a = term;
        term = 0;
        while (b) {
            if (b & 1ULL) term = term >= modulus - a ? term - (modulus - a) : term + a;
            a = a >= modulus - a ? a - (modulus - a) : a + a;
            b >>= 1;
        }
        x = x >= modulus - term ? x - (modulus - term) : x + term;
    }

    res.x = (x % prod + prod) % prod;
    res.mod = prod;
    res.success = 1;
    return res;
}

void print_crt_result(const CRTResult* res) {
    if (!res || !res->success) {
        printf("\n=== Chinese Remainder Theorem: No Solution / Invalid ===\n");
        return;
    }
    printf("\n=== Chinese Remainder Theorem Result ===\n");
    printf("Smallest Matchday Solution: x = %lld\n", res->x);
    printf("Periodic Cycle Modulus (LCM): %lld\n", res->mod);
}