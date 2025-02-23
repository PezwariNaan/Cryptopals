#include "utility.hpp"
#include "openssl.hpp"

std::vector<uint8_t> encrypt_plaintext(const std::vector<uint8_t> my_string, const std::vector<uint8_t> plaintext) {
    std::vector<uint8_t> ciphertext;
    std::vector<uint8_t> complete_plaintext(my_string.begin(), my_string.end());
    complete_plaintext.insert(complete_plaintext.end(), plaintext.begin(), plaintext.end());

    std::vector<uint8_t> key(16, 'A');

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    ciphertext = openssl::encrypt_ecb(ctx, complete_plaintext, &key); // What we are attempting to decrypt

    EVP_CIPHER_CTX_free(ctx);

    return ciphertext;
}

int get_blocksize(const std::vector<uint8_t> plaintext) {
    int blocksize = 0;
    std::vector<uint8_t> my_string = {'0'}; // This is what we will use to pad the start of each ciphertext
    std::vector<uint8_t> ciphertext = encrypt_plaintext(my_string, plaintext);
    std::vector<uint8_t> padded_ciphertext;

    do {
        padded_ciphertext = encrypt_plaintext(my_string, plaintext);
        my_string.insert(my_string.end(), '0');
    } while (ciphertext.size() == padded_ciphertext.size());
    blocksize = padded_ciphertext.size() - ciphertext.size();
    
    return blocksize;
}

std::string ecb_cbc_oracle(std::vector<uint8_t> ciphertext, int blocksize) {
    // Detect ECB
    std::vector<std::vector<uint8_t>> blocks = create_blocks(ciphertext, blocksize);
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

std::string detect_ecb(const std::vector<uint8_t> plaintext, const int blocksize) {
    std::vector<uint8_t> padding(blocksize * 3, '0'); // Add enough bytes to ensure repeating block if using ECB

    std::vector<uint8_t> ciphertext = encrypt_plaintext(padding, plaintext);
    std::string mode = ecb_cbc_oracle(ciphertext, blocksize);
    return mode;
}

std::vector<uint8_t> attack_ecb(std::vector<uint8_t> plaintext, int blocksize) {
    std::vector<uint8_t> result;
    while (result.size() < plaintext.size()) { // Little bit of a cheat - should ideally hide plaintext from the rest of the program & use the stripped ciphertext. 
        int padding_size = blocksize - (result.size() % blocksize) - 1;
        std::vector<uint8_t> padding(padding_size, '0');

        std::vector<uint8_t> ciphertext = encrypt_plaintext(padding, plaintext);

        // Once there are blocksize number of elements in results - the block_index will increment
        int block_index = result.size()/ blocksize;
        std::vector<uint8_t> target_block(ciphertext.begin() + (block_index * blocksize), 
                                        ciphertext.begin() + (block_index + 1) * blocksize);

        // Loop through all possible values in the ASCII table for whichever byte we are attempting to decrypt. 
        bool byte_found = false;
        for (uint8_t guess = 0; guess < 255; guess++) {

            std::vector<uint8_t> attempt = padding;
            attempt.insert(attempt.end(), result.begin(), result.end());
            attempt.push_back(guess);

            std::vector<uint8_t> attempt_ciphertext = encrypt_plaintext(attempt, plaintext);
            std::vector<uint8_t> attempt_block(attempt_ciphertext.begin() + (block_index * blocksize), 
                                                attempt_ciphertext.begin() + (block_index + 1) * blocksize);
        // If there is a match append to results vector.
            if (attempt_block == target_block) {
                result.push_back(guess);
                byte_found = true;
                break;
            }
        }

        if (!byte_found) {
            std::cerr << "No byte found during decryption. Attack failed." << std::endl;
            break;
        }
    }

    return result;
}
