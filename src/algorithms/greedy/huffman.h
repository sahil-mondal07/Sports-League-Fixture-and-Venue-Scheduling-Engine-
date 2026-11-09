#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "../../utils/common.h"

/**
 * @brief A node in the Huffman tree.
 * Stores a fixture pattern ID and its frequency, plus child pointers.
 */
typedef struct HuffmanNode {
    int id;                         /**< Fixture pattern / activity ID */
    unsigned freq;                  /**< Frequency / weight (how often this pattern occurs) */
    struct HuffmanNode* left;     /**< Left child (lower frequency) */
    struct HuffmanNode* right;    /**< Right child (higher frequency) */
} HuffmanNode;

/**
 * @brief Create a new Huffman tree node.
 * @param id The fixture pattern ID.
 * @param freq The frequency / weight of this pattern.
 * @return Pointer to the newly created node (caller owns it).
 */
HuffmanNode* huffman_node_create(int id, unsigned freq);

/**
 * @brief Free an entire Huffman tree recursively.
 * @param node Root of the tree to free.
 */
void huffman_node_free(HuffmanNode* node);

/**
 * @brief Build a Huffman tree from an array of (id, frequency) pairs.
 *
 * @param patterns Array of {id, freq} pairs.
 * @param n Number of pattern entries.
 * @return Root node of the constructed Huffman tree.
 */
HuffmanNode* huffman_tree_build(const int* ids, const unsigned* freqs, int n);

/**
 * @brief Generate Huffman codes (bit sequences) from a built tree.
 *
 * @param root Root of the Huffman tree.
 * @param codes Output array of code strings (each allocated via malloc).
 *             Caller must free each string and the array.
 * @param num_codes Number of codes returned (matches the original n).
 */
void huffman_tree_generate_codes(HuffmanNode* root,
                                 char codes[][64], int* code_lens, int n);

/**
 * @brief Encode a sequence of fixture pattern IDs using the generated codes.
 *
 * @param ids Array of pattern IDs to encode.
 * @param n Number of IDs.
 * @param code_lens Array of code lengths (from huffman_tree_generate_codes).
 * @param codes Array of code strings (from huffman_tree_generate_codes).
 * @param out_len Output: total encoded bit length (in bits/bytes, implementation-defined).
 * @return char* Encoded bitstream (heap-allocated, caller frees). Laid out as
 *                 consecutive bits packed into bytes; the last byte may be partially filled.
 */
char* huffman_encode(const int* ids, int n, const int* code_lens,
                     const char codes[][64], int* out_len);

/**
 * @brief Decode a Huffman-encoded bitstream back to pattern IDs.
 *
 * @param root Root of the Huffman tree.
 * @param encoded Pointer to the encoded bitstream.
 * @param encoded_len Total number of bits (or bytes, implementation-defined) in the stream.
 * @param out_count Output: number of pattern IDs decoded.
 * @return int* Array of decoded pattern IDs (heap-allocated, caller frees).
 *             Returns NULL on error.
 */
int* huffman_decode(HuffmanNode* root, const char* encoded, int encoded_len,
                    int* out_count);

/**
 * @brief Free the encoded bitstream returned by huffman_encode.
 * @param bits The bitstream to free.
 */
void huffman_free_encoded(char* bits);

#endif /* HUFFMAN_H */