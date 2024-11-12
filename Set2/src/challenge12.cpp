#include "openssl.hpp"
#include "utility.hpp"
#include "encoding.hpp"
// --------------------
#include <cstdint>
#include <iostream>
#include <openssl/evp.h>
#include <sys/types.h>
#include <vector>

// Generate Key
std::vector<uint8_t> generate_key(void) {
    std::vector<uint8_t> key;

    for (int i = 0; i < 16; i++) {
        uint8_t random_char = rand() % 128;
        key.push_back(random_char);
    }
    return key;
}

std::vector<uint8_t> encrypt_plaintext(const std::vector<uint8_t> my_string, const std::vector<uint8_t> plaintext) {
    std::vector<uint8_t> ciphertext;
    std::vector<uint8_t> complete_plaintext(my_string.begin(), my_string.end());
    complete_plaintext.insert(complete_plaintext.end(), plaintext.begin(), plaintext.end());

    std::vector<uint8_t> key(16, 'A');

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    ciphertext = openssl::encrypt_ecb(ctx, complete_plaintext, key); // What we are attempting to decrypt

    EVP_CIPHER_CTX_free(ctx);

    return ciphertext;
}

int get_blocksize(const std::vector<uint8_t> plaintext) {
    int blocksize = 0;
    int padding = 0;
    std::vector<uint8_t> my_string = {'0'}; // This is what we will use to pad the start of each ciphertext
    std::vector<uint8_t> ciphertext = encrypt_plaintext(my_string, plaintext);
    std::vector<uint8_t> padded_ciphertext;

    do {
        padded_ciphertext = encrypt_plaintext(my_string, plaintext);
        my_string.insert(my_string.end(), '0');
    } while (ciphertext.size() == padded_ciphertext.size());
    blocksize = padded_ciphertext.size() - ciphertext.size();
    padding = blocksize - my_string.size() - 1;
    
    return blocksize;
}

std::string detect_ecb(const std::vector<uint8_t> plaintext, const int blocksize) {
    std::vector<uint8_t> padding(blocksize * 3, '0'); // Add enough bytes to ensure repeating block if using ECB

    std::vector<uint8_t> ciphertext = encrypt_plaintext(padding, plaintext);
    std::string mode = openssl::ecb_cbc_oracle(ciphertext);
    return mode;
}

std::vector<uint8_t> attack_ecb(std::vector<uint8_t> plaintext, int blocksize) {
    std::vector<uint8_t> result;
    for (size_t byte_position = 0; byte_position < plaintext.size(); byte_position++) {
        int padding_size = blocksize - (result.size() % blocksize) - 1;
        std::vector<uint8_t> padding(padding_size, '0');

        std::vector<uint8_t> ciphertext = encrypt_plaintext(padding, plaintext);
        std::vector<uint8_t> target_block(ciphertext.begin(), ciphertext.begin() + blocksize);

        bool byte_found = false;
        for (uint8_t guess = 0; guess < 255; guess++) {

            std::vector<uint8_t> attempt = padding;
            attempt.insert(attempt.end(), result.begin(), result.end());
            attempt.push_back(guess);

            std::vector<uint8_t> attempt_ciphertext = encrypt_plaintext(attempt, plaintext);
            std::vector<uint8_t> attempt_block(attempt_ciphertext.begin(), attempt_ciphertext.begin() + blocksize);

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

int main(int argc, char *argv[]) {
    std::string filename = "Texts/challenge12.txt";
    std::vector<uint8_t> plaintext = cp::py_base64decode(filename, *argv);

    // Get block size
    int blocksize = get_blocksize(plaintext);

    // Detect ECB
    std::string mode = detect_ecb(plaintext, blocksize);

    if (mode != "ECB") {
        std::cerr << "AES Encryption is not ECB." << std::endl;
        return 1;
    }

    std::vector<uint8_t> guess = attack_ecb(plaintext, blocksize);
    print_array(guess);

    return 0;
}
