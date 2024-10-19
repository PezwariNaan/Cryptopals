#include "encoding.hpp"
#include "encrypting.hpp"
#include "utility.hpp"
#include <cstddef>
#include <openssl/aes.h>
#include <openssl/evp.h>


#define BLOCKSIZE 16

// Generate IV
std::vector<uint8_t> generate_iv(void) {
    std::vector<uint8_t> iv;

    for (char i = 0; i < 16; i++) {
        iv.push_back('0');
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

std::vector<uint8_t> encrypt_block(EVP_CIPHER_CTX *ctx, std::vector<uint8_t> block) {
    std::vector<uint8_t> ciphertext(BLOCKSIZE);
    int len = 0;

    EVP_EncryptUpdate(ctx, ciphertext.data(), &len, block.data(), block.size());

    return ciphertext;
}

// Implement CBC

int main(void) {
    std::vector<uint8_t> read_file =  read_file_bytes("./Texts/challenge10_decoded");
    std::string key_string = "YELLOW SUBMARINE";

    std::vector<std::vector<uint8_t>> blocks = create_blocks(read_file);

    std::vector<uint8_t> key_vector(key_string.begin(), key_string.end());
    const uint8_t *key = (uint8_t *)key_vector.data();
    std::vector<uint8_t> iv_vector = generate_iv();

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    if (!ctx) 
        std::runtime_error("Error Creating EVP Cipher Context.");

    if (EVP_EncryptInit(ctx, EVP_aes_128_ecb(), key, NULL) != 1)
        std::runtime_error("Error Initalising Encryption Engine.");
    
    // Encrypt First Block
    std::vector<std::vector<uint8_t>> encrypted_blocks;
    std::vector<uint8_t> first_xored_block = cp::fixed_xor(blocks[0], iv_vector);
    std::vector<uint8_t> first_encrypted_block = encrypt_block(ctx, first_xored_block);
    encrypted_blocks.push_back(first_encrypted_block);
    
    // Encrypt Rest of the Blocks
    for (size_t i = 1; i < blocks.size(); i++) {
        std::vector<uint8_t> xored_block = cp::fixed_xor(encrypted_blocks[i - 1], blocks[i]);
        std::vector<uint8_t> encrypted_block = encrypt_block(ctx, xored_block);
        encrypted_blocks.push_back(encrypted_block);
    }

    // for (size_t i = 0; i < encrypted_blocks.size(); i++) {
    //     print_array(encrypted_blocks[i]);
    //     std::cout << "\n";
    // }

    for (size_t i = 0; i < blocks.size(); i++) {
    std::cout << cp::hex_encode(blocks[i]);
    std::cout << "\n";
    }
    std::cout << "\n\n";

    // Debug Statements
    std::cout << "IV:\n";
    print_array(iv_vector);
    std::cout << "\n\n";

    std::cout << "First Hex Encoded Block:\n";
    std::cout << cp::hex_encode(blocks[0]);
    std::cout << "\n\n";

    std::cout <<"First block XORd with IV:\n";
    std::cout << cp::hex_encode(first_xored_block);
    std::cout << "\n\n";

    std::cout << "First block encrypted after XOR: \n";
    std::cout << cp::hex_encode(first_encrypted_block);
    std::cout << "\n\n";

    EVP_CIPHER_CTX_free(ctx);
    return 0;
}
