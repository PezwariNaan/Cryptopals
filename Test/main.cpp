#include <cstdint>
#include <iostream>
#include <vector>
#include <openssl/evp.h>

#define BYTES std::vector<uint8_t>

std::vector<uint8_t> fixed_xor(const std::vector<uint8_t> &x, const std::vector<uint8_t> &y) {
    std::vector<uint8_t> result;

    if (x.size() != y.size()) {
        std::cerr << "Text and key are not matching lengths";
        return result;
    }

    for (size_t i = 0; i < x.size(); i++) 
        result.push_back(x[i] ^ y[i]);

    return result;
}

std::vector<std::vector<uint8_t>> create_blocks(const std::vector<uint8_t> input, const int blocksize) {
    std::vector<std::vector<uint8_t>> block_vector; // All blocks
    size_t i = 0;
    for (; i + blocksize <= input.size(); i += blocksize)
        block_vector.emplace_back(input.begin() + i, input.begin() + i + blocksize);

    // Pad block if necessary
    if (i < input.size()) {
        std::vector<uint8_t> block(input.begin() + i, input.end());
        size_t padding_needed = blocksize - block.size();
        block.insert(block.end(), padding_needed, padding_needed);

        // Add block to block_vector;
        block_vector.push_back(block);
    }

    return block_vector;
}

std::vector<uint8_t> encrypt_block(EVP_CIPHER_CTX *ctx, std::vector<uint8_t> block, const int blocksize) {
    std::vector<uint8_t> ciphertext(blocksize);
    int len = 0;

    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, block.data(), block.size()) != 1)
        throw std::runtime_error("Error during Encryption.");
    
    ciphertext.resize(len);

    return ciphertext;
}

std::vector<uint8_t> encrypt_cbc(EVP_CIPHER_CTX *ctx, int blocksize,const std::vector<uint8_t> plaintext, const std::vector<uint8_t> &key, const std::vector<uint8_t> iv) {
    EVP_CIPHER_CTX_set_padding(ctx, 0);

    if (EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key.data(), NULL) != 1)
        std::runtime_error("Error Initalising Encryption Engine.");

    BYTES ciphertext;
    std::vector<std::vector<uint8_t>> blocks = create_blocks(plaintext, blocksize);

    std::vector<uint8_t> prev_block = iv;

    for (size_t i = 0; i < blocks.size(); i++) {
        // XOR
        std::vector<uint8_t> xored_block = fixed_xor(prev_block, blocks[i]);
        // Then Encrypt
        std::vector<uint8_t> encrypted_block = encrypt_block(ctx, xored_block, blocksize);
        // Then Append
        ciphertext.insert(ciphertext.end(), encrypted_block.begin(), encrypted_block.end());
        prev_block = encrypted_block;
    }
    
    EVP_CIPHER_CTX_reset(ctx);
    return ciphertext;
}

std::vector<uint8_t> decrypt_block(EVP_CIPHER_CTX *ctx, std::vector<uint8_t> block, const int blocksize) {
    std::vector<uint8_t> plaintext(blocksize);
    int len = 0;

    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, block.data(), block.size()) != 1)
        throw std::runtime_error("Error during decryption.");
    
    plaintext.resize(len);

    return plaintext;
}

std::vector<uint8_t> decrypt_cbc(EVP_CIPHER_CTX *ctx, int blocksize,const std::vector<uint8_t> ciphertext, const std::vector<uint8_t> &key, const std::vector<uint8_t> iv) {
    EVP_CIPHER_CTX_set_padding(ctx, 0);

    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key.data(), NULL) != 1)
        std::runtime_error("Error Initalising Encryption Engine.");

    std::vector<std::vector<uint8_t>> blocks = create_blocks(ciphertext, blocksize);

    std::vector<uint8_t> decrypted_text;
    std::vector<uint8_t> prev_block = iv;

    for (size_t i = 0; i < blocks.size(); i++) {
        // Decrypt
        std::vector<uint8_t> decrypted_block = decrypt_block(ctx, blocks[i], blocksize);
        // Then XOR
        std::vector<uint8_t> plaintext_block = fixed_xor(decrypted_block, prev_block);
        // Then Appened 
        decrypted_text.insert(decrypted_text.end(), plaintext_block.begin(), plaintext_block.end());
        prev_block = blocks[i];
    }
    
    EVP_CIPHER_CTX_reset(ctx);

    return decrypted_text;
}

int main(void) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    BYTES plaintext(7, 'a');
    BYTES key(16, 'A');

    BYTES ciphertext = encrypt_cbc(ctx, 16, plaintext, key, key);
    BYTES decrypted = decrypt_cbc(ctx, 16, ciphertext, key, key);
    std::string decrypted_str(decrypted.begin(), decrypted.end());
    std::cout << decrypted_str << std::endl;
    
    return 0;
}
