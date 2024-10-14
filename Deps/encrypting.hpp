#ifndef ENCRYPTING_H
#define ENCRYPTING_H

#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <cstdint>
#include <memory>

struct result {
    std::vector<uint8_t> decrypted_bytes;
    uint8_t key;
    int score = 0;
};

namespace cp {
    std::vector<uint8_t> fixed_xor(const std::vector<uint8_t> &start, const std::vector<uint8_t> &key);
    std::vector<uint8_t> repeating_key_xor(const std::vector<uint8_t> &plain_bytes, const std::vector<uint8_t> &key_bytes);

    // Attack Single Byte XOR 
    int character_lookup(std::vector<uint8_t> &byte_array, const uint8_t &key);
    std::map<int, uint8_t> get_scores(std::vector<uint8_t> &byte_array);
    std::vector<uint8_t> single_byte_xor(std::vector<uint8_t> &byte_array, const  uint8_t &key);
    result attack_single_byte_xor(std::vector<uint8_t> &cipher_bytes);
    // -----------------------

    // Attack Repeating Key Xor
    int get_hamming_distance(std::vector<const uint8_t> &first, const std::vector<uint8_t> &second);
    std::map<float, int> get_keysize(const int MAX_KEYSIZE, std::vector<uint8_t> cipher_bytes);
    std::map<int, std::vector<uint8_t>> make_blocks(int keysize, std::vector<uint8_t> &cipher_bytes);
    std::tuple<int, std::vector<uint8_t>, std::vector<uint8_t>> attack_repeating_key_xor(std::vector<uint8_t> &cipher_bytes);
    // ------------------------
}

#endif // ENCRYPTING_H
