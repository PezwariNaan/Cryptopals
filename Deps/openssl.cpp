#include "utility.hpp"
#include "openssl.hpp"
#include "encrypting.hpp"

std::vector<uint8_t> openssl::encrypt_ecb(EVP_CIPHER_CTX *ctx, const std::vector<uint8_t> plaintext, const std::vector<uint8_t> *key) {
    if (EVP_EncryptInit(ctx, EVP_aes_128_ecb(), key->data(), NULL) != 1)
        throw std::runtime_error("Error Initalising Encryption Engine.");

    std::vector<uint8_t> ciphertext(plaintext.size() + EVP_CIPHER_CTX_block_size(ctx));
    int ciphertext_len = 0;
    int len = 0;

    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), plaintext.size()) != 1)
        throw std::runtime_error("Error With ECB Encryption."); 
    ciphertext_len = len;

    if (EVP_EncryptFinal(ctx, ciphertext.data() + ciphertext_len, &len) != 1)
        throw std::runtime_error("Error With ECB Encryption (Final Step).");
    ciphertext_len += len;
    
    ciphertext.resize(ciphertext_len);
    
    return ciphertext;
}

std::vector<uint8_t> openssl::encrypt_cbc(EVP_CIPHER_CTX *ctx, std::vector<uint8_t> plaintext, const std::vector<uint8_t> iv, const std::vector<uint8_t> *key) {
    std::vector<uint8_t> ciphertext;
    std::vector<std::vector<uint8_t>> blocks = create_blocks(plaintext);
    std::vector<uint8_t> xored_blocks;

    // Insert iv as first block 
    blocks.insert(blocks.begin(), iv);

    for (size_t i = 1; i < blocks.size() - 1; i++) {
        std::vector<uint8_t> xored_block = cp::fixed_xor(blocks[i - 1], blocks[i]);
        xored_blocks.insert(xored_blocks.end(), xored_block.begin(), xored_block.end());
        std::vector<uint8_t> encrypted_block = openssl::encrypt_ecb(ctx, xored_block, key);
        ciphertext.insert(ciphertext.end(), encrypted_block.begin(), encrypted_block.end());
    }
    
    return ciphertext;
}

std::string openssl::ecb_cbc_oracle(std::vector<uint8_t> ciphertext) {
    // Detect ECB
    std::vector<std::vector<uint8_t>> blocks = create_blocks(ciphertext);
    for (size_t i = 0; i < blocks.size(); i++) {
        for (size_t j = i + 1; j < blocks.size(); j++) {
            if (blocks[i] == blocks[j]) {
                return "ECB";
            }
        }
    }
    // Else CBC
    return "CBC";
}
