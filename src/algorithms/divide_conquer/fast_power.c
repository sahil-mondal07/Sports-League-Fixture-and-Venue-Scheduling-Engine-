#include "fast_power.h"
#include <stdlib.h>
#include <string.h>

double fast_power(double base, int exp) {
    if (exp == 0) {
        return 1.0;
    }
    if (exp < 0) {
        if (base == 0.0) return 0.0;
        return 1.0 / fast_power(base, -exp);
    }

    /* Divide and conquer: b^e = (b^(e/2))^2 * (b if e is odd else 1) */
    double half = fast_power(base, exp / 2);
    if (exp % 2 == 0) {
        return half * half;
    } else {
        return half * half * base;
    }
}

long long fast_power_mod(long long base, long long exp, long long mod) {
    if (mod <= 0 || exp < 0) {
        return -1;
    }
    if (mod == 1) {
        return 0;
    }

    long long result = 1;
    long long current = base % mod;
    if (current < 0) {
        current += mod;
    }

    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * current) % mod;
        }
        current = (current * current) % mod;
        exp /= 2;
    }

    return result;
}

void matrix_multiply_square(const double* A, const double* B, double* C, int dim) {
    if (!A || !B || !C || dim <= 0) return;

    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            double sum = 0.0;
            for (int k = 0; k < dim; k++) {
                sum += A[i * dim + k] * B[k * dim + j];
            }
            C[i * dim + j] = sum;
        }
    }
}

int matrix_power(const double* M, int dim, int exp, double* result) {
    if (!M || !result || dim <= 0 || exp < 0) {
        return -1;
    }

    size_t sz = (size_t)dim * (size_t)dim * sizeof(double);

    /* Initialize result as Identity matrix I */
    memset(result, 0, sz);
    for (int i = 0; i < dim; i++) {
        result[i * dim + i] = 1.0;
    }

    if (exp == 0) {
        return 0;
    }

    /* Allocate temporary scratch buffers */
    double* current_base = (double*)malloc(sz);
    double* temp = (double*)malloc(sz);

    if (!current_base || !temp) {
        free(current_base);
        free(temp);
        return -1;
    }

    memcpy(current_base, M, sz);
    int current_exp = exp;

    while (current_exp > 0) {
        if (current_exp % 2 == 1) {
            matrix_multiply_square(result, current_base, temp, dim);
            memcpy(result, temp, sz);
        }
        if (current_exp > 1) {
            matrix_multiply_square(current_base, current_base, temp, dim);
            memcpy(current_base, temp, sz);
        }
        current_exp /= 2;
    }

    free(current_base);
    free(temp);
    return 0;
}
