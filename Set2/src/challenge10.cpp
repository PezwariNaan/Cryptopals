#include "encrypting.hpp"
#include "utility.hpp"
#include <cstddef>
#include <cstdint>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <stdexcept>
#include <sys/types.h>

#define BLOCKSIZE 16

// Get 16 byte blocks (with pkcs#7 if a block isn't long enough)
std::vector<std::vector<uint8_t>> create_blocks(std::vector<uint8_t> plaintext) {
    std::vector<std::vector<uint8_t>> block_vector; // All blocks
    for (size_t i = 0; i < plaintext.size(); i += BLOCKSIZE) { 
        std::vector<uint8_t> block; // Current block
        for (size_t j = i; j < i + BLOCKSIZE; j++) {
            block.push_back(plaintext[j]);
        }
        // Pad block if necessary
        if (block.size() < BLOCKSIZE) {
            int padding_needed = BLOCKSIZE - block.size();
            for (; padding_needed > 0; padding_needed--) {
                block.push_back(padding_needed);
            }
        }
        // Add block to block_vector;
        block_vector.push_back(block);
    }
    return block_vector;
}

std::vector<uint8_t> decrypt_block(EVP_CIPHER_CTX *ctx, std::vector<uint8_t> block) {
    std::vector<uint8_t> plaintext(BLOCKSIZE);
    int len = 0;

    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, block.data(), block.size()) != 1)
        throw std::runtime_error("Error during decryption.");
    
    plaintext.resize(len);

    return plaintext;
}

// Implement CBC

int main(void) {
    std::vector<uint8_t> read_file =  read_file_bytes("./Texts/challenge10_decoded");

    std::vector<std::vector<uint8_t>> blocks = create_blocks(read_file);

    const uint8_t key[BLOCKSIZE] = {'Y', 'E', 'L', 'L', 'O', 'W', ' ', 
                             'S', 'U', 'B', 'M', 'A', 'R', 'I', 'N', 'E'};

    std::vector<uint8_t> iv_vector = {0, 0, 0, 0,
                                      0, 0, 0, 0,
                                      0, 0, 0, 0, 
                                     0, 0, 0, 0};

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_set_padding(ctx, 0);

    if (!ctx) 
        std::runtime_error("Error Creating EVP Cipher Context.");

    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL) != 1)
        std::runtime_error("Error Initalising Encryption Engine.");
    
    std::vector<uint8_t> decrypted_text;
    std::vector<uint8_t> prev_block = iv_vector;
    for (size_t i = 0; i < blocks.size(); i++) {
        // Decrypt
        std::vector<uint8_t> decrypted_block = decrypt_block(ctx, blocks[i]);
        // Then Xor
        std::vector<uint8_t> plaintext_block = cp::fixed_xor(decrypted_block, prev_block);
        // Then appened 
        decrypted_text.insert(decrypted_text.end(), plaintext_block.begin(), plaintext_block.end());
        prev_block = blocks[i];
    }

    print_array(decrypted_text);

    EVP_CIPHER_CTX_free(ctx);
    return 0;
}
