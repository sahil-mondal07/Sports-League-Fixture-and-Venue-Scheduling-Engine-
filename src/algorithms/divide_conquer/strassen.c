#include "strassen.h"
#include <stdlib.h>
#include <string.h>

#define STRASSEN_THRESHOLD 32

void matrix_multiply_classical(const double* A, const double* B, double* C, int n) {
    if (!A || !B || !C || n <= 0) return;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += A[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = sum;
        }
    }
}

/* Helper matrix arithmetic: C = A + B */
static void matrix_add(const double* A, const double* B, double* C, int n) {
    int total = n * n;
    for (int i = 0; i < total; i++) {
        C[i] = A[i] + B[i];
    }
}

/* Helper matrix arithmetic: C = A - B */
static void matrix_sub(const double* A, const double* B, double* C, int n) {
    int total = n * n;
    for (int i = 0; i < total; i++) {
        C[i] = A[i] - B[i];
    }
}

/* Recursive internal Strassen function for n = power of 2 */
static int strassen_recursive(const double* A, const double* B, double* C, int n) {
    if (n <= STRASSEN_THRESHOLD) {
        matrix_multiply_classical(A, B, C, n);
        return 0;
    }

    int half = n / 2;
    size_t half_sz = (size_t)half * (size_t)half * sizeof(double);

    /* Allocate 4 quadrants for A and B */
    double* A11 = (double*)malloc(half_sz);
    double* A12 = (double*)malloc(half_sz);
    double* A21 = (double*)malloc(half_sz);
    double* A22 = (double*)malloc(half_sz);

    double* B11 = (double*)malloc(half_sz);
    double* B12 = (double*)malloc(half_sz);
    double* B21 = (double*)malloc(half_sz);
    double* B22 = (double*)malloc(half_sz);

    /* Allocate scratch matrices for additions/subtractions */
    double* S1 = (double*)malloc(half_sz);
    double* S2 = (double*)malloc(half_sz);

    /* Allocate 7 multiplication products */
    double* M1 = (double*)malloc(half_sz);
    double* M2 = (double*)malloc(half_sz);
    double* M3 = (double*)malloc(half_sz);
    double* M4 = (double*)malloc(half_sz);
    double* M5 = (double*)malloc(half_sz);
    double* M6 = (double*)malloc(half_sz);
    double* M7 = (double*)malloc(half_sz);

    if (!A11 || !A12 || !A21 || !A22 || !B11 || !B12 || !B21 || !B22 ||
        !S1 || !S2 || !M1 || !M2 || !M3 || !M4 || !M5 || !M6 || !M7) {
        free(A11); free(A12); free(A21); free(A22);
        free(B11); free(B12); free(B21); free(B22);
        free(S1); free(S2);
        free(M1); free(M2); free(M3); free(M4); free(M5); free(M6); free(M7);
        return -1;
    }

    /* Split A and B into 4 quadrants */
    for (int i = 0; i < half; i++) {
        for (int j = 0; j < half; j++) {
            A11[i * half + j] = A[i * n + j];
            A12[i * half + j] = A[i * n + (j + half)];
            A21[i * half + j] = A[(i + half) * n + j];
            A22[i * half + j] = A[(i + half) * n + (j + half)];

            B11[i * half + j] = B[i * n + j];
            B12[i * half + j] = B[i * n + (j + half)];
            B21[i * half + j] = B[(i + half) * n + j];
            B22[i * half + j] = B[(i + half) * n + (j + half)];
        }
    }

    /* M1 = (A11 + A22) * (B11 + B22) */
    matrix_add(A11, A22, S1, half);
    matrix_add(B11, B22, S2, half);
    strassen_recursive(S1, S2, M1, half);

    /* M2 = (A21 + A22) * B11 */
    matrix_add(A21, A22, S1, half);
    strassen_recursive(S1, B11, M2, half);

    /* M3 = A11 * (B12 - B22) */
    matrix_sub(B12, B22, S2, half);
    strassen_recursive(A11, S2, M3, half);

    /* M4 = A22 * (B21 - B11) */
    matrix_sub(B21, B11, S2, half);
    strassen_recursive(A22, S2, M4, half);

    /* M5 = (A11 + A12) * B22 */
    matrix_add(A11, A12, S1, half);
    strassen_recursive(S1, B22, M5, half);

    /* M6 = (A21 - A11) * (B11 + B12) */
    matrix_sub(A21, A11, S1, half);
    matrix_add(B11, B12, S2, half);
    strassen_recursive(S1, S2, M6, half);

    /* M7 = (A12 - A22) * (B21 + B22) */
    matrix_sub(A12, A22, S1, half);
    matrix_add(B21, B22, S2, half);
    strassen_recursive(S1, S2, M7, half);

    /* Combine into C:
       C11 = M1 + M4 - M5 + M7
       C12 = M3 + M5
       C21 = M2 + M4
       C22 = M1 - M2 + M3 + M6 */
    for (int i = 0; i < half; i++) {
        for (int j = 0; j < half; j++) {
            int idx = i * half + j;
            double m1 = M1[idx];
            double m2 = M2[idx];
            double m3 = M3[idx];
            double m4 = M4[idx];
            double m5 = M5[idx];
            double m6 = M6[idx];
            double m7 = M7[idx];

            C[i * n + j]                         = m1 + m4 - m5 + m7;
            C[i * n + (j + half)]                = m3 + m5;
            C[(i + half) * n + j]                = m2 + m4;
            C[(i + half) * n + (j + half)]       = m1 - m2 + m3 + m6;
        }
    }

    free(A11); free(A12); free(A21); free(A22);
    free(B11); free(B12); free(B21); free(B22);
    free(S1); free(S2);
    free(M1); free(M2); free(M3); free(M4); free(M5); free(M6); free(M7);
    return 0;
}

/* Helper to find next power of 2 >= n */
static int next_power_of_two(int n) {
    int p = 1;
    while (p < n) {
        p <<= 1;
    }
    return p;
}

int matrix_multiply_strassen(const double* A, const double* B, double* C, int n) {
    if (!A || !B || !C || n <= 0) {
        return -1;
    }

    /* For small dimensions, classical algorithm is faster and avoids allocations */
    if (n <= STRASSEN_THRESHOLD) {
        matrix_multiply_classical(A, B, C, n);
        return 0;
    }

    int p2 = next_power_of_two(n);

    /* If already power of 2, run directly */
    if (p2 == n) {
        return strassen_recursive(A, B, C, n);
    }

    /* Otherwise, pad with zeros to p2 x p2 */
    double* padA = (double*)calloc((size_t)p2 * (size_t)p2, sizeof(double));
    double* padB = (double*)calloc((size_t)p2 * (size_t)p2, sizeof(double));
    double* padC = (double*)calloc((size_t)p2 * (size_t)p2, sizeof(double));

    if (!padA || !padB || !padC) {
        free(padA);
        free(padB);
        free(padC);
        return -1;
    }

    /* Copy input matrices into top-left of padded matrices */
    for (int i = 0; i < n; i++) {
        memcpy(&padA[i * p2], &A[i * n], (size_t)n * sizeof(double));
        memcpy(&padB[i * p2], &B[i * n], (size_t)n * sizeof(double));
    }

    int status = strassen_recursive(padA, padB, padC, p2);

    if (status == 0) {
        /* Extract result from top-left back into C */
        for (int i = 0; i < n; i++) {
            memcpy(&C[i * n], &padC[i * p2], (size_t)n * sizeof(double));
        }
    }

    free(padA);
    free(padB);
    free(padC);
    return status;
}
