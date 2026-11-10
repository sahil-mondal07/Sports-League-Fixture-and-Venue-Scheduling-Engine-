#include "crt.h"
#include <stdio.h>

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
    return (x % m + m) % m;
}

CRTResult chinese_remainder_theorem(const long long* remainders, const long long* moduli, int k) {
    CRTResult res;
    res.x = -1;
    res.mod = 0;
    res.success = 0;

    if (!remainders || !moduli || k <= 0) {
        return res;
    }

    long long prod = 1;
    for (int i = 0; i < k; i++) {
        if (moduli[i] <= 0) return res;
        prod *= moduli[i];
    }

    long long x = 0;
    for (int i = 0; i < k; i++) {
        long long ni = prod / moduli[i];
        long long yi = mod_inverse(ni, moduli[i]);
        if (yi == -1) {
            return res; /* moduli are not coprime */
        }
        x = (x + remainders[i] * ni * yi) % prod;
    }

    res.x = (x + prod) % prod;
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
