#include "utility.hpp"
#include "openssl.hpp"
#include "encrypting.hpp"

std::vector<uint8_t> openssl::encrypt_ecb(EVP_CIPHER_CTX *ctx, const std::vector<uint8_t> plaintext, const std::vector<uint8_t> &key) {
    if (EVP_EncryptInit(ctx, EVP_aes_128_ecb(), key.data(), NULL) != 1)
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

std::vector<uint8_t> openssl::decrypt_ecb(EVP_CIPHER_CTX *ctx, const std::vector<uint8_t> ciphertext, const std::vector<uint8_t> &key) {
    if (EVP_DecryptInit(ctx, EVP_aes_128_ecb(), key.data(), NULL) != 1)
        throw std::runtime_error("Error Initalising Decryption Engine.");

    std::vector<uint8_t> plaintext(ciphertext.size() + EVP_CIPHER_CTX_block_size(ctx));
    int plaintext_len = 0;
    int len = 0;

    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size()) != 1)
        throw std::runtime_error("Error With ECB Decryption."); 
    plaintext_len = len;

    if (EVP_DecryptFinal(ctx, plaintext.data() + plaintext_len, &len) != 1)
        throw std::runtime_error("Error With ECB Decryption (Final Step).");
    plaintext_len += len;
    
    plaintext.resize(plaintext_len);
    
    return plaintext;
}

std::vector<uint8_t> openssl::encrypt_block(EVP_CIPHER_CTX *ctx, std::vector<uint8_t> block, const int blocksize) {
    std::vector<uint8_t> ciphertext(blocksize);
    int len = 0;

    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, block.data(), block.size()) != 1)
        throw std::runtime_error("Error During Encryption.");
    
    ciphertext.resize(len);

    return ciphertext;
}

std::vector<uint8_t> openssl::encrypt_cbc(EVP_CIPHER_CTX *ctx, int blocksize,const std::vector<uint8_t> plaintext, const std::vector<uint8_t> &key, const std::vector<uint8_t> iv) {
    EVP_CIPHER_CTX_set_padding(ctx, 0);

    if (EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key.data(), NULL) != 1)
        std::runtime_error("Error Initalising Encryption Engine.");

    std::vector<uint8_t> ciphertext;
    std::vector<std::vector<uint8_t>> blocks = create_blocks(plaintext, blocksize);

    std::vector<uint8_t> prev_block = iv;

    for (size_t i = 0; i < blocks.size(); i++) {
        // XOR
        std::vector<uint8_t> xored_block = cp::fixed_xor(prev_block, blocks[i]);
        // Then Encrypt
        std::vector<uint8_t> encrypted_block = encrypt_block(ctx, xored_block, blocksize);
        // Then Append
        ciphertext.insert(ciphertext.end(), encrypted_block.begin(), encrypted_block.end());
        prev_block = encrypted_block;
    }
    
    EVP_CIPHER_CTX_reset(ctx);
    return ciphertext;
}

std::vector<uint8_t> openssl::decrypt_block(EVP_CIPHER_CTX *ctx, std::vector<uint8_t> block, const int blocksize) {
    std::vector<uint8_t> plaintext(blocksize);
    int len = 0;

    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, block.data(), block.size()) != 1)
        throw std::runtime_error("Error during decryption.");
    
    plaintext.resize(len);

    return plaintext;
}

std::vector<uint8_t> openssl::decrypt_cbc(EVP_CIPHER_CTX *ctx, int blocksize,const std::vector<uint8_t> ciphertext, const std::vector<uint8_t> &key, const std::vector<uint8_t> iv) {
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
        std::vector<uint8_t> plaintext_block = cp::fixed_xor(decrypted_block, prev_block);
        // Then Appened 
        decrypted_text.insert(decrypted_text.end(), plaintext_block.begin(), plaintext_block.end());
        prev_block = blocks[i];
    }
    
    EVP_CIPHER_CTX_reset(ctx);

    return decrypted_text;
}
