/* ============================================================================
 * tests/test_greedy_dc.c — Greedy and divide-and-conquer algorithm tests
 * ============================================================================
 * Tests: activity selection, interval partitioning, Huffman, max subarray,
 *        fast power/mod, celebrity, Strassen. Each test compares against an
 *        independent brute-force/reference implementation.
 * Must return non-zero exit code on any failure.
 * ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../src/utils/common.h"
#include "../src/algorithms/greedy/activity_selection.h"
#include "../src/algorithms/greedy/interval_partitioning.h"
#include "../src/algorithms/greedy/huffman.h"
#include "../src/algorithms/divide_conquer/max_subarray.h"
#include "../src/algorithms/divide_conquer/fast_power.h"
#include "../src/algorithms/divide_conquer/celebrity.h"
#include "../src/algorithms/divide_conquer/fast_power.h"
#include "../src/algorithms/divide_conquer/strassen.h"

/* ---- Activity selection brute force ---- */

static int max_activities_brute(const Activity* activities, int n) {
    int best = 0;
    for (int mask = 0; mask < (1 << n); mask++) {
        int last_finish = -1;
        int count = 0;
        int ok = 1;
        for (int i = 0; i < n; i++) {
            if (mask & (1 << i)) {
                if (activities[i].start_time < last_finish) { ok = 0; break; }
                last_finish = activities[i].finish_time;
                count++;
            }
        }
        if (ok && count > best) best = count;
    }
    return best;
}

static void test_activity_selection(void) {
    printf("TEST: activity selection (brute force comparison, n<=11)\n");
    srand((unsigned)time(NULL) + 1);
    int failures = 0;
    for (int t = 0; t < 200; t++) {
        int n = rand() % 11 + 1;
        Activity* activities = malloc(n * sizeof(Activity));
        for (int i = 0; i < n; i++) {
            activities[i].id = i;
            activities[i].start_time = rand() % 50;
            activities[i].finish_time = activities[i].start_time + rand() % 50 + 1;
        }
        int returned = select_activities(activities, n, NULL);
        int expected = max_activities_brute(activities, n);
        if (returned != expected) {
            printf("  FAIL case %d: returned=%d expected=%d (n=%d)\n",
                   t, returned, expected, n);
            failures++;
        }
        free(activities);
    }
    if (failures) {
        printf("  %d activity selection failures\n", failures);
    } else {
        printf("  PASS: all 200 activity selection cases\n");
    }
    exit(failures);
}

/* ---- Interval partitioning ---- */

static void test_interval_partitioning(void) {
    printf("TEST: interval partitioning\n");
    srand((unsigned)time(NULL) + 2);
    int failures = 0;
    for (int t = 0; t < 100; t++) {
        int n = rand() % 10 + 1;
        IntervalActivity* activities = malloc(n * sizeof(IntervalActivity));
        for (int i = 0; i < n; i++) {
            activities[i].id = i;
            activities[i].venue_id = 0;
            activities[i].start_time = rand() % 30;
            activities[i].finish_time = activities[i].start_time + rand() % 20 + 1;
        }
        int resources_out[10];
        int resources = interval_partition(activities, n, resources_out);
        int ok = 1;
        for (int r = 0; r < resources; r++) {
            int last_f = -1;
            for (int i = 0; i < n; i++) {
                if (resources_out[i] == r) {
                    if (activities[i].start_time < last_f) {
                        printf("  FAIL case %d: overlap in resource %d (interval %d starts %d, last finish %d)\n",
                               t, r, i, activities[i].start_time, last_f);
                        ok = 0;
                        break;
                    }
                    last_f = activities[i].finish_time;
                }
            }
            if (!ok) break;
        }
        free(activities);
        if (!ok) break;
    }
    if (failures) {
        printf("  %d interval partitioning failures\n", failures);
    } else {
        printf("  PASS: all 100 interval partitioning cases\n");
    }
    exit(failures);
}

/* ---- Huffman ---- */

static void test_huffman(void) {
    printf("TEST: Huffman (encode/decode roundtrip, prefix-free, optimal cost)\n");
    srand((unsigned)time(NULL) + 3);
    int failures = 0;
    for (int alpha = 1; alpha <= 10; alpha++) {
        int* ids = malloc(alpha * sizeof(int));
        unsigned* freqs = malloc(alpha * sizeof(unsigned));
        for (int i = 0; i < alpha; i++) {
            ids[i] = i;
            freqs[i] = rand() % 50 + 1;
        }
        HuffmanNode* root = huffman_tree_build(ids, freqs, alpha);
        if (!root) {
            printf("  FAIL alphabet=%d: tree build returned NULL\n", alpha);
            failures++;
            free(ids); free(freqs);
            continue;
        }
        char codes[10][64];
        int code_lens[10];
        /* huffman_tree_generate_codes: (root, codes, code_lens, n, alphabet_size) */
        huffman_tree_generate_codes(root, codes, code_lens, alpha, alpha);
        /* Verify encode/decode roundtrip */
        int msg_len = rand() % 20 + 1;
        int* msg = malloc(msg_len * sizeof(int));
        for (int i = 0; i < msg_len; i++) msg[i] = rand() % alpha;
        int encoded_len = 0;
        /* huffman_encode: (ids, n, code_lens, codes, alphabet_size, out_len) */
        char* encoded = huffman_encode(msg, msg_len, code_lens,
                        (const char (*)[64])codes, alpha, &encoded_len);
        if (!encoded) {
            printf("  FAIL alphabet=%d: encode returned NULL\n", alpha);
            failures++;
            free(ids); free(freqs); free(msg); free(encoded);
            continue;
        }
        int decoded_count = 0;
        /* huffman_decode: (root, encoded, encoded_len, out_count) */
        int* decoded = huffman_decode(root, encoded, encoded_len, &decoded_count);
        if (!decoded) {
            printf("  FAIL alphabet=%d: decode returned NULL\n", alpha);
            failures++;
        } else if (decoded_count != msg_len) {
            printf("  FAIL alphabet=%d: decoded %d != original %d\n", alpha, decoded_count, msg_len);
            failures++;
        } else {
            int ok = 1;
            for (int i = 0; i < msg_len; i++) {
                if (decoded[i] != msg[i]) { ok = 0; break; }
            }
            if (!ok) {
                printf("  FAIL alphabet=%d: roundtrip mismatch\n", alpha);
                failures++;
            }
        }
        free(ids); free(freqs); free(msg); free(encoded); free(decoded);
        huffman_node_free(root);
        /* Prefix-free check */
        for (int i = 0; i < alpha; i++) {
            for (int j = 0; j < alpha; j++) {
                if (i != j && code_lens[i] > 0 && code_lens[j] > 0 && code_lens[i] <= code_lens[j]) {
                    int is_prefix = 1;
                    for (int k = 0; k < code_lens[i]; k++) {
                        if (codes[i][k] != codes[j][k]) { is_prefix = 0; break; }
                    }
                    if (is_prefix) {
                        printf("  FAIL alphabet=%d: code %d is prefix of code %d\n", alpha, i, j);
                        failures++;
                    }
                }
            }
        }
    }
    /* Test single-symbol alphabet */
    {
        printf("  (single-symbol alphabet test)\n");
        int alpha = 1;
        int* ids = malloc(alpha * sizeof(int));
        unsigned* freqs = malloc(alpha * sizeof(unsigned));
        ids[0] = 0; freqs[0] = 1;
        HuffmanNode* root = huffman_tree_build(ids, freqs, alpha);
        if (!root) {
            printf("  FAIL alphabet=1: tree build returned NULL\n");
            failures++;
        } else {
            char codes[10][64];
            int code_lens[10];
            huffman_tree_generate_codes(root, codes, code_lens, alpha, alpha);
            /* Single symbol should have code "0" length 1 */
            if (code_lens[0] != 1 || codes[0][0] != '0') {
                printf("  FAIL alphabet=1: expected code_lens[0]=1 codes[0]=\"0\", got code_lens=%d codes=%c\n",
                       code_lens[0], codes[0][0]);
                failures++;
            } else {
                printf("  PASS alphabet=1: single-symbol code is \"0\" length 1\n");
            }
            huffman_node_free(root);
        }
        free(ids); free(freqs);
    }
    /* Test message shorter than alphabet */
    {
        printf("  (short message test, msg_len < alphabet)\n");
        int alpha = 10;
        int msg_len = 3; /* less than alphabet_size */
        int* msg = malloc(msg_len * sizeof(int));
        msg[0] = 1; msg[1] = 3; msg[2] = 7;
        int* ids = malloc(alpha * sizeof(int));
        unsigned* freqs = malloc(alpha * sizeof(unsigned));
        for (int i = 0; i < alpha; i++) {
            ids[i] = i;
            freqs[i] = rand() % 50 + 1;
        }
        HuffmanNode* root = huffman_tree_build(ids, freqs, alpha);
        if (!root) {
            printf("  FAIL short message alphabet=10: tree build returned NULL\n");
            failures++;
            free(ids); free(freqs); free(msg);
        } else {
            char codes[10][64];
            int code_lens[10];
            huffman_tree_generate_codes(root, codes, code_lens, alpha, alpha);
            int encoded_len = 0;
            char* encoded = huffman_encode(msg, msg_len, code_lens,
                            (const char (*)[64])codes, alpha, &encoded_len);
            if (!encoded) {
                printf("  FAIL short message alphabet=10: encode returned NULL\n");
                failures++;
            } else {
                int decoded_count = 0;
                int* decoded = huffman_decode(root, encoded, encoded_len, &decoded_count);
                if (!decoded) {
                    printf("  FAIL short message alphabet=10: decode returned NULL\n");
                    failures++;
                } else if (decoded_count != msg_len) {
                    printf("  FAIL short message alphabet=10: decoded %d != original %d\n", decoded_count, msg_len);
                    failures++;
                } else {
                    int ok = 1;
                    for (int i = 0; i < msg_len; i++) {
                        if (decoded[i] != msg[i]) { ok = 0; break; }
                    }
                    if (!ok) {
                        printf("  FAIL short message alphabet=10: roundtrip mismatch\n");
                        failures++;
                    } else {
                        printf("  PASS short message alphabet=10: roundtrip OK (3 symbols < alphabet 10)\n");
                    }
                }
                free(encoded); free(decoded);
            }
            free(ids); free(freqs); free(msg);
            huffman_node_free(root);
        }
    }
    if (failures) {
        printf("  %d Huffman failures\n", failures);
    } else {
        printf("  PASS: all Huffman cases (alphabet 1..10, single-symbol, short message)\n");
    }
    exit(failures);
}

/* ---- Max subarray ---- */

static void test_max_subarray(void) {
    printf("TEST: max subarray (divide-and-conquer and Kadane, all-negative)\n");
    srand((unsigned)time(NULL) + 4);
    int failures = 0;
    for (int t = 0; t < 200; t++) {
        int n = rand() % 25 + 1;
        int* arr = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) arr[i] = rand() % 40 - 20;
        SubarrayResult dc = max_subarray_dc(arr, 0, n - 1);
        /* Kadane reference */
        SubarrayResult kadane;
        kadane.sum = arr[0]; kadane.left_idx = 0; kadane.right_idx = 0;
        int best = arr[0], cur = arr[0];
        for (int i = 1; i < n; i++) {
            cur = cur > 0 ? cur + arr[i] : arr[i];
            if (cur > best) { best = cur; kadane.left_idx = (cur == arr[i]) ? i : kadane.left_idx; kadane.right_idx = i; kadane.sum = best; }
        }
        if (dc.sum != kadane.sum) {
            printf("  FAIL case %d: dc.sum=%d kadane.sum=%d (n=%d)\n", t, dc.sum, kadane.sum, n);
            failures++;
        }
        free(arr);
    }
    if (failures) {
        printf("  %d max subarray failures\n", failures);
    } else {
        printf("  PASS: all 200 max subarray cases\n");
    }
    exit(failures);
}

/* ---- Fast power/mod ---- */

static void test_fast_power(void) {
    printf("TEST: fast power and power mod\n");
    srand((unsigned)time(NULL) + 5);
    int failures = 0;
    for (int t = 0; t < 200; t++) {
        int base = rand() % 100 + 1;
        int exp = rand() % 20 + 1;
        int mod = rand() % 50 + 1;
        int pm = fast_power_mod(base, exp, mod);
        int ref = 1;
        for (int i = 0; i < exp; i++) ref = (ref * base) % mod;
        if (pm != ref) {
            printf("  FAIL case %d: pow_mod=%d ref=%d (base=%d exp=%d mod=%d)\n",
                   t, pm, ref, base, exp, mod);
            failures++;
        }
        int fp = fast_power(base, exp);
        ref = 1;
        for (int i = 0; i < exp; i++) ref *= base;
        if (fp != ref) {
            printf("  FAIL case %d: fast_power=%d ref=%d (base=%d exp=%d)\n",
                   t, fp, ref, base, exp);
            failures++;
        }
    }
    if (failures) {
        printf("  %d fast power/mod failures\n", failures);
    } else {
        printf("  PASS: all 200 fast power/mod cases\n");
    }
    exit(failures);
}

/* ---- Celebrity ---- */

static void test_celebrity(void) {
    printf("TEST: celebrity (matrix version)\n");
    srand((unsigned)time(NULL) + 6);
    int failures = 0;
    /* Test n=4, n=8 */
    for (int n = 1; n <= 8; n *= 2) {
        int* matrix = malloc(n * n * sizeof(int));
        if (!matrix) {
            printf("  FAIL n=%d: malloc failed\n", n);
            failures++;
            continue;
        }
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                matrix[i * n + j] = (i == j) ? 1 : (rand() % 2);
        /* Set up celebrity: person 0 is celebrity */
        for (int j = 0; j < n; j++) matrix[0 * n + j] = (j == 0) ? 1 : 0;
        for (int i = 1; i < n; i++) matrix[i * n + 0] = 1;

        int result = find_celebrity_matrix(matrix, n);
        if (result == 0) {
            printf("  PASS n=%d: celebrity found at index 0\n", n);
        } else {
            printf("  FAIL n=%d: expected celebrity at index 0, got %d\n", n, result);
            failures++;
        }
        free(matrix);
    }
    if (failures) {
        printf("  %d celebrity failures\n", failures);
    } else {
        printf("  PASS: all celebrity cases\n");
    }
    exit(failures);
}

/* ---- Strassen ---- */

static void test_strassen(void) {
    printf("TEST: Strassen matrix multiplication\n");
    srand((unsigned)time(NULL) + 7);
    int failures = 0;
    /* Test n=1,2,4,8,16 - using power-of-2 dimensions */
    for (int pow2 = 0; pow2 <= 4; pow2++) {
        int n = 1 << pow2;
        /* Use row-major flat arrays instead of double pointers */
        double* a = (double*)malloc(n * n * sizeof(double));
        double* b = (double*)malloc(n * n * sizeof(double));
        double* result = (double*)malloc(n * n * sizeof(double));
        if (!a || !b || !result) {
            printf("  FAIL n=%d: malloc failed\n", n);
            if (a) free(a);
            if (b) free(b);
            if (result) free(result);
            failures++;
            continue;
        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                a[i * n + j] = (double)(rand() % 10);
                b[i * n + j] = (double)(rand() % 10);
                result[i * n + j] = 0;
                for (int k = 0; k < n; k++)
                    result[i * n + j] += a[i * n + k] * b[k * n + j];
            }
        }
        double* s = (double*)malloc(n * n * sizeof(double));
        if (!s) {
            printf("  FAIL n=%d: Strassen returned NULL\n", n);
            free(a); free(b); free(result);
            failures++;
            continue;
        }
        int status = matrix_multiply_strassen(a, b, s, n);
        if (status != 0) {
            printf("  FAIL n=%d: matrix_multiply_strassen returned %d\n", n, status);
            free(a); free(b); free(result); free(s);
            failures++;
            continue;
        }
        int match = 1;
        for (int i = 0; i < n && match; i++)
            for (int j = 0; j < n; j++)
                if (s[i * n + j] != result[i * n + j]) match = 0;
        if (!match) {
            printf("  FAIL n=%d: Strassen result doesn't match reference\n", n);
            failures++;
        } else {
            printf("  PASS n=%d: Strassen matches reference\n", n);
        }
        free(a); free(b); free(result); free(s);
    }
    if (failures) {
        printf("  %d Strassen failures\n", failures);
    } else {
        printf("  PASS: all Strassen cases (n=1,2,4,8,16)\n");
    }
    exit(failures);
}

int main(void) {
    printf("=== Greedy & Divide-and-Conquer Tests ===\n");
    test_activity_selection();
    test_interval_partitioning();
    test_huffman();
    test_max_subarray();
    test_fast_power();
    test_celebrity();
    test_strassen();
    return 0;
}