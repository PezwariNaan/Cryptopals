#ifndef OPENSSL_H
#define OPENSSL_H
#include "utility.hpp"
#include <openssl/evp.h>

namespace openssl {
    std::vector<uint8_t> encrypt_ecb(EVP_CIPHER_CTX *ctx, const std::vector<uint8_t> plaintext, const std::vector<uint8_t> &key);
    std::vector<uint8_t> decrypt_ecb(EVP_CIPHER_CTX *ctx, const std::vector<uint8_t> ciphertext , const std::vector<uint8_t> &key);

    std::vector<uint8_t> encrypt_block(EVP_CIPHER_CTX *ctx, std::vector<uint8_t> block, const int blocksize);
    std::vector<uint8_t> encrypt_cbc(EVP_CIPHER_CTX *ctx, int blocksize,const std::vector<uint8_t> plaintext, const std::vector<uint8_t> &key, const std::vector<uint8_t> iv);

    std::vector<uint8_t> decrypt_block(EVP_CIPHER_CTX *ctx, std::vector<uint8_t> block, const int blocksize);
    std::vector<uint8_t> decrypt_cbc(EVP_CIPHER_CTX *ctx, int blocksize,const std::vector<uint8_t> ciphertext, const std::vector<uint8_t> &key, const std::vector<uint8_t> iv);
}

#endif // OPENSSL_H
