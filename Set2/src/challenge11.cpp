#include "utility.hpp"
#include <chrono>
#include <cstdint>
#include <openssl/evp.h>
#include <stdexcept>
#include <thread>

// Generate Key
std::vector<uint8_t> generate_key(void) {
    std::vector<uint8_t> key;
    std::srand((unsigned) time(NULL));

    for (int i = 0; i < 16; i++) {
        uint8_t random_char = rand() % 128;
        key.push_back(random_char);
    }
    return key;
}

// Append + Prepend 5-10 bytes 
std::vector<uint8_t> padd_ciphertext(std::vector<uint8_t> ciphertext) {
    std::vector<uint8_t> padded_ciphertext;
    return padded_ciphertext;
}

// Encrypt Function With CBC
std::vector<uint8_t> encrypt_cbc(EVP_CIPHER_CTX *ctx, std::vector<uint8_t> key, std::vector<uint8_t> plaintext){
    std::vector<uint8_t> ciphertext;

    return ciphertext;
}

// Encrypt Fucntion With ECB
std::vector<uint8_t> encrypt_ecb(EVP_CIPHER_CTX *ctx, std::vector<uint8_t> plaintext) {
    if (EVP_EncryptInit(ctx, EVP_aes_128_ecb(), generate_key().data(), NULL) != 1)
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

int main(void) {
    EVP_CIPHER_CTX *ctx =  EVP_CIPHER_CTX_new();
    std::string plaintext_string = "Hello Darkness My Old Friend";
    std::vector<uint8_t> plaintext(plaintext_string.begin(), plaintext_string.end());
    

    return 0;
}