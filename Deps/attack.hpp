#ifndef ATTACK_H
#define ATTACK_H

#include "utility.hpp"
#include "openssl.hpp"

namespace attack {
    std::vector<uint8_t> encrypt_plaintext(const std::vector<uint8_t> my_string, const std::vector<uint8_t> plaintext);
    int get_blocksize(const std::vector<uint8_t> plaintext);
    std::string detect_ecb(const std::vector<uint8_t> plaintext, const int blocksize);
    std::vector<uint8_t> attack_ecb(std::vector<uint8_t> plaintext, int blocksize);
}

#endif