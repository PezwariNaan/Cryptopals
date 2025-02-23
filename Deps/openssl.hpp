#ifndef OPENSSL_H
#define OPENSSL_H
#include "utility.hpp"
#include <openssl/evp.h>

namespace openssl {
    std::vector<uint8_t> encrypt_ecb(EVP_CIPHER_CTX *ctx, const std::vector<uint8_t> plaintext, const std::vector<uint8_t> *key);
    std::vector<uint8_t> encrypt_cbc(EVP_CIPHER_CTX *ctx, std::vector<uint8_t> plaintext, const std::vector<uint8_t> iv, const std::vector<uint8_t> *key);
}

#endif // OPENSSL_H
