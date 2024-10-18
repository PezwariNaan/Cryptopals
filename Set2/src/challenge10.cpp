#include "encoding.hpp"
#include "utility.hpp"
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <vector>

#define BLOCKSIZE 16

// Generate IV
std::vector<uint8_t> generate_iv(void) {
    std::vector<uint8_t> iv;

    for (char i = 0; i < 16; i++) {
        iv.push_back(0);
    }
    return iv;
} 

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
            for (size_t padding_needed = BLOCKSIZE - block.size(); padding_needed > 0; padding_needed--) {
                block.push_back(0);
            }
        }
        // Add block to block_vector;
        block_vector.push_back(block);
    }
    return block_vector;
}

// Encrypt block with AES 128 ECB using provided Key 
// std::vector<uint8_t> encrypt_block(const std::vector<uint8_t> key, std::vector<uint8_t> block) {
//     const uint8_t *raw_block = (uint8_t *)block.data();
//     const uint8_t *raw_key = (uint8_t *)key.data();

//     const int key_len = key.size() * 8;
//     if (key_len != 192) std::runtime_error("Key Length is not 192 bits");

//     AES_KEY aes_key;

// }

// XOR block 1 with block 2 - order does not matter (commutative property)

// Implement CBC

int main(void) {
    std::vector<uint8_t> read_file =  read_file_bytes("./Texts/challenge10_decoded");
    std::string key_string = "YELLOW SUBMARINE";

    std::vector<std::vector<uint8_t>> blocks = create_blocks(read_file);
    for (size_t i = 0; i < blocks.size(); i++) {
        //print_array(blocks[i]);
        std::cout << cp::hex_encode(blocks[i]);
        std::cout << "\n\n\n";
    }
    // std::vector<uint8_t> key(key_string.begin(), key_string.end());
    // std::vector<uint8_t> iv = generate_iv();
    // const uint8_t *raw_iv = (uint8_t *)iv.data();
    // const uint8_t *raw_key = (uint8_t *)key.data();

    // EVP_CIPHER_CTX *ctx;
    // int len;
    // int block_len;
    // int ciphertext_len;
    // uint8_t *cipher_text;
    // uint8_t *raw_block;


    // if (!(EVP_CIPHER_CTX_new())) 
    //     std::runtime_error("Error Creating EVP Cipher Context.");

    // if (EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, raw_key, raw_iv) != 1)
    //     std::runtime_error("Error Initalising Encryption Engine.");
    
    // if(EVP_EncryptUpdate(ctx, cipher_text, &len, raw_block, block_len))

    return 0;
}