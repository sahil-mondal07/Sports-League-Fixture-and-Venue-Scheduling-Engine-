#include "huffman.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ------------------------------------------------------------
 * Min-heap implementation for building the Huffman tree.
 * We keep the heap sorted by frequency (ascending).
 * ------------------------------------------------------------ */

#define HEAP_INIT_CAP 16

typedef struct {
    HuffmanNode** data;
    int size;
    int capacity;
} MinHeap;

static MinHeap* heap_create(int cap) {
    MinHeap* h = (MinHeap*)malloc(sizeof(MinHeap));
    if (!h) return NULL;
    h->data = (HuffmanNode**)malloc(sizeof(HuffmanNode*) * cap);
    if (!h->data) { free(h); return NULL; }
    h->size = 0;
    h->capacity = cap;
    return h;
}

static void heap_destroy(MinHeap* h) {
    if (h) {
        free(h->data);
        free(h);
    }
}

/* Insert a node maintaining min-heap property (by freq ascending) */
static void heap_insert(MinHeap* h, HuffmanNode* node) {
    if (h->size >= h->capacity) {
        h->capacity *= 2;
        h->data = (HuffmanNode**)realloc(h->data, sizeof(HuffmanNode*) * h->capacity);
    }
    int i = h->size++;
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (node->freq >= h->data[parent]->freq) break;
        h->data[i] = h->data[parent];
        i = parent;
    }
    h->data[i] = node;
}

/* Extract the minimum-freq node (root) */
static HuffmanNode* heap_extract_min(MinHeap* h) {
    if (h->size == 0) return NULL;
    HuffmanNode* min = h->data[0];
    HuffmanNode* last = h->data[--h->size];
    int i = 0, child;
    while (i * 2 + 1 < h->size) {
        child = i * 2 + 1;
        if (child + 1 < h->size && h->data[child + 1]->freq < h->data[child]->freq)
            child++;
        if (last->freq <= h->data[child]->freq) break;
        h->data[i] = h->data[child];
        i = child;
    }
    h->data[i] = last;
    return min;
}

/* ------------------------------------------------------------
 * Huffman node utility
 * ------------------------------------------------------------ */

HuffmanNode* huffman_node_create(int id, unsigned freq) {
    HuffmanNode* node = (HuffmanNode*)malloc(sizeof(HuffmanNode));
    if (!node) return NULL;
    node->id = id;
    node->freq = freq;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void huffman_node_free(HuffmanNode* node) {
    if (!node) return;
    huffman_node_free(node->left);
    huffman_node_free(node->right);
    free(node);
}

/* ------------------------------------------------------------
 * Build the Huffman tree from frequency pairs.
 * ------------------------------------------------------------ */

HuffmanNode* huffman_tree_build(const int* ids, const unsigned* freqs, int n) {
    if (n <= 0) return NULL;
    if (n == 1) {
        return huffman_node_create(ids[0], freqs[0]);
    }

    MinHeap* heap = heap_create(HEAP_INIT_CAP);
    if (!heap) return NULL;

    /* Insert all leaf nodes */
    for (int i = 0; i < n; i++) {
        HuffmanNode* node = huffman_node_create(ids[i], freqs[i]);
        if (!node) { heap_destroy(heap); return NULL; }
        heap_insert(heap, node);
    }

    /* Repeatedly extract two smallest, create parent, insert back */
    while (heap->size > 1) {
        HuffmanNode* left = heap_extract_min(heap);
        HuffmanNode* right = heap_extract_min(heap);
        if (!left || !right) { huffman_node_free(left); huffman_node_free(right); break; }

        HuffmanNode* parent = huffman_node_create(-1, left->freq + right->freq);
        if (!parent) { huffman_node_free(left); huffman_node_free(right); break; }
        parent->left = left;
        parent->right = right;
        heap_insert(heap, parent);
    }

    HuffmanNode* root = heap_extract_min(heap);
    heap_destroy(heap);
    return root;
}

/* ------------------------------------------------------------
 * Generate Huffman codes from the built tree.
 *
 * We traverse the tree: going left appends bit '0', going right appends bit '1'.
 * Codes are stored as null-terminated strings in the output array.
 * ------------------------------------------------------------ */

static void codes_gen_traverse(HuffmanNode* node, int depth,
                               const char* prefix, char codes[][64], int* code_lens, int n) {
    if (!node) return;

    /* Build the current prefix code string */
    char code_str[65];  /* max 64 chars + null terminator */
    if (depth > 0) {
        memcpy(code_str, prefix, depth);
        code_str[depth] = '\0';
    } else {
        code_str[0] = '\0';
    }

    if (node->id >= 0) {
        /* Leaf node — record the code for this ID */
        if (node->id < n) {
            strncpy(codes[node->id], code_str, 64);
        }
        return;
    }

    /* Internal node — propagate prefix extended with '0' or '1' */
    char left_prefix[65];
    char right_prefix[65];

    if (depth > 0) {
        memcpy(left_prefix, prefix, depth);
        left_prefix[depth] = '0';
        left_prefix[depth + 1] = '\0';

        memcpy(right_prefix, prefix, depth);
        right_prefix[depth] = '1';
        right_prefix[depth + 1] = '\0';
    } else {
        left_prefix[0] = '0';
        left_prefix[1] = '\0';
        right_prefix[0] = '1';
        right_prefix[1] = '\0';
    }

    codes_gen_traverse(node->left, depth + 1, left_prefix, codes, code_lens, n);
    codes_gen_traverse(node->right, depth + 1, right_prefix, codes, code_lens, n);
}

void huffman_tree_generate_codes(HuffmanNode* root,
                                 char codes[][64], int* code_lens, int n) {
    if (!root || n <= 0) return;

    char empty_prefix[1] = { '\0' };
    memset(code_lens, 0, sizeof(int) * n);

    codes_gen_traverse(root, 0, empty_prefix, codes, code_lens, n);
}

/* ------------------------------------------------------------
 * Encode an array of pattern IDs using the generated codes.
 * ------------------------------------------------------------ */

char* huffman_encode(const int* ids, int n, const int* code_lens,
                     const char codes[][64], int* out_len) {
    if (!ids || n <= 0 || !code_lens || !codes || !out_len) return NULL;

    /* Compute total encoded bit count */
    int total_bits = 0;
    for (int i = 0; i < n; i++) {
        int cid = ids[i];
        if (cid < 0 || cid >= n) continue;
        if (code_lens[cid] <= 0) continue;
        total_bits += code_lens[cid];
    }

    if (total_bits == 0) { *out_len = 0; return NULL; }

    /* Pack bits into bytes (ceiling) */
    int total_bytes = (total_bits + 7) / 8;
    char* bits = (char*)calloc(total_bytes, sizeof(char));
    if (!bits) { *out_len = 0; return NULL; }

    /* Accumulate bits sequentially into the byte array */
    int byte_pos = 0;
    int bit_idx = 0;  /* position within current byte (0..7) */

    for (int i = 0; i < n; i++) {
        int cid = ids[i];
        if (cid < 0 || cid >= n) continue;
        int cl = code_lens[cid];
        if (cl <= 0) continue;
        const char* code = codes[cid];

        for (int b = 0; b < cl; b++) {
            if (code[b] == '1') {
                bits[byte_pos] |= (1 << (7 - bit_idx));  /* MSB-first within byte */
            }
            bit_idx++;
            if (bit_idx == 8) {
                bit_idx = 0;
                byte_pos++;
            }
        }
    }

    *out_len = total_bits;
    return bits;
}

/* ------------------------------------------------------------
 * Decode a Huffman-encoded bitstream.
 * ------------------------------------------------------------ */

int* huffman_decode(HuffmanNode* root, const char* encoded, int encoded_len,
                    int* out_count) {
    if (!root || !encoded || encoded_len <= 0 || !out_count) return NULL;

    /* encoded_len here is the number of bits; but our bitstream is stored in bytes.
       We'll interpret the encoded byte array as containing encoded_len bits total. */
    int total_bytes = (encoded_len + 7) / 8;

    int* result = (int*)malloc(sizeof(int) * encoded_len);
    if (!result) { *out_count = 0; return NULL; }

    int count = 0;
    int byte_pos = 0;
    int bit_pos = 0;  /* 0..7 position within current byte */

    HuffmanNode* current = root;
    while (count < encoded_len && byte_pos < total_bytes) {
        /* Get the current bit */
        if (bit_pos >= 8) {
            bit_pos = 0;
            byte_pos++;
        }
        if (byte_pos >= total_bytes) break;

        char bit = (encoded[byte_pos] >> (7 - bit_pos)) & 1 ? '1' : '0';
        bit_pos++;

        if (bit == '0') {
            current = current->left;
        } else {
            current = current->right;
        }

        if (!current) break;

        if (current->id >= 0) {
            /* Leaf node — record this ID */
            result[count++] = current->id;
            current = root;  /* reset for next code */
        }
    }

    *out_count = count;
    return result;
}

void huffman_free_encoded(char* bits) {
    if (bits) free(bits);
}