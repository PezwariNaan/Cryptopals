#include "utility.hpp"
#include "encrypting.hpp"
#include <openssl/evp.h>

#define BLOCKSIZE 16

// (DONE) Detect which cipher was used. 

// Generate Key
std::vector<uint8_t> generate_key(void) {
    std::vector<uint8_t> key;

    for (int i = 0; i < 16; i++) {
        uint8_t random_char = rand() % 128;
        key.push_back(random_char);
    }
    return key;
}

// Append + Prepend 5-10 bytes 
std::vector<uint8_t> pad_plaintext(const std::vector<uint8_t> &plaintext) {
    std::vector<uint8_t> padded_plaintext = plaintext;

    uint8_t padding_added_start = (rand() % 5) + 6;
    uint8_t padding_added_end   = (rand() % 5) + 6;

    padded_plaintext.insert(padded_plaintext.end(), padding_added_end, padding_added_end);
    padded_plaintext.insert(padded_plaintext.begin(), padding_added_start, padding_added_start);

    return padded_plaintext;
}

// Encrypt With ECB
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

// Encrypt With CBC
std::vector<uint8_t> encrypt_cbc(EVP_CIPHER_CTX *ctx, std::vector<uint8_t> plaintext) {
    std::vector<uint8_t> ciphertext;
    std::vector<std::vector<uint8_t>> blocks = create_blocks(plaintext);
    std::vector<uint8_t> xored_blocks;

    // Insert iv as first block 
    std::vector<uint8_t >iv = generate_key();
    blocks.insert(blocks.begin(), iv);

    for (size_t i = 1; i < blocks.size() - 1; i++) {
        std::vector<uint8_t> xored_block = cp::fixed_xor(blocks[i - 1], blocks[i]);
        xored_blocks.insert(xored_blocks.end(), xored_block.begin(), xored_block.end());
        std::vector<uint8_t> encrypted_block = encrypt_ecb(ctx, xored_block);
        ciphertext.insert(ciphertext.end(), encrypted_block.begin(), encrypted_block.end());
    }
    
    return ciphertext;
}

// Randomly Choose Between CBC & ECB
std::vector<uint8_t> generate_ciphertext(EVP_CIPHER_CTX *ctx, std::vector<uint8_t> plaintext) {
    std::srand((unsigned) time(NULL));
    uint8_t option = rand() % 2;

    switch (option) {
        case 0:
            std::cout << "ECB\n";
            return encrypt_ecb(ctx, plaintext);
        case 1:
            std::cout << "CBC\n";
            return encrypt_cbc(ctx, plaintext);
        default:
            throw std::runtime_error("Error Genorating Ciphertext.");
    }
}

std::string ecb_cbc_oracle(std::vector<uint8_t> ciphertext) {
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

int main(void) {
    std::srand((unsigned) time(NULL));

    EVP_CIPHER_CTX *ctx =  EVP_CIPHER_CTX_new();
    std::string plaintext_string = "Sixteen Repeats Sixteen Repeats Sixteen Repeats ";
    std::vector<uint8_t> plaintext(plaintext_string.begin(), plaintext_string.end());
    
    std::vector<uint8_t> padded_plaintext = pad_plaintext(plaintext);

    std::vector<uint8_t> ciphertext = generate_ciphertext(ctx, padded_plaintext);
    std::string guess = ecb_cbc_oracle(ciphertext);
    std::cout << guess << "\n";

    return 0;
}
