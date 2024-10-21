#include "encoding.hpp"
#include "encrypting.hpp"
#include "utility.hpp"
#include <openssl/aes.h>
#include <openssl/evp.h>

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

std::vector<uint8_t> encrypt_block(EVP_CIPHER_CTX *ctx, std::vector<uint8_t> block) {
    std::vector<uint8_t> ciphertext(BLOCKSIZE);
    int len = 0;

    EVP_EncryptUpdate(ctx, ciphertext.data(), &len, block.data(), block.size());
    ciphertext.resize(len);

    return ciphertext;
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

    if (!ctx) 
        std::runtime_error("Error Creating EVP Cipher Context.");

    if (EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL) != 1)
        std::runtime_error("Error Initalising Encryption Engine.");
    
    // Encrypt First Block
    std::vector<std::vector<uint8_t>> encrypted_blocks;
    std::vector<uint8_t> first_xored_block = cp::fixed_xor(blocks[0], iv_vector);
    std::vector<uint8_t> first_encrypted_block = encrypt_block(ctx, first_xored_block);
    encrypted_blocks.push_back(first_encrypted_block);
    
    // Encrypt Rest of the Blocks
    for (size_t i = 1; i < blocks.size(); i++) {
        std::vector<uint8_t> xored_block = cp::fixed_xor(blocks[i], encrypted_blocks[i - 1]);
        std::vector<uint8_t> encrypted_block = encrypt_block(ctx, xored_block);
        encrypted_blocks.push_back(encrypted_block);
    }

    for (size_t i = 0; i < encrypted_blocks.size(); i++) {
        print_array(encrypted_blocks[i]);
        std::cout << "\n";
    }

    // Debug Statements
    std::cout << "IV Size: " << iv_vector.size() << "\n\n";
    std::cout << "Block Size: " << blocks[0].size() << "\n\n";
    std::cout << "Key: " << key << "\n\n";

    std::cout << "IV as hex:\n";
    std::cout << cp::hex_encode(iv_vector);
    std::cout << "\n\n";

    std::cout << "First hex encoded Block:\n";
    std::cout << cp::hex_encode(blocks[0]);
    std::cout << "\n\n";

    std::cout <<"First block XORd with IV:\n";
    std::cout << cp::hex_encode(first_xored_block);
    std::cout << "\n\n";

    std::cout << "First block encrypted after XOR: \n";
    std::cout << cp::hex_encode(first_encrypted_block);
    std::cout << "\n\n";

    std::cout << "First block as byte: \n";
    print_array(blocks[0]);
    std::cout << "\n\n";

    std::cout << "First block XORd as bytes:\n";
    print_array(first_xored_block);
    std::cout << "\n\n";

    std::cout << " First XORd block encrypted as bytes:\n";
    print_array(first_encrypted_block);
    std::cout << "\n\n"; 

    EVP_CIPHER_CTX_free(ctx);
    return 0;
}
