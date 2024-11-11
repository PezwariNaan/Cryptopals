#include "utility.hpp"
#include "encoding.hpp"
#include <openssl/evp.h>

std::vector<uint8_t> encrypt_ecb(EVP_CIPHER_CTX *ctx, const std::vector<uint8_t> plaintext, const std::vector<uint8_t> key) {
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

int main(int argc, char *argv[]) {
    std::string filename = "Texts/challenge12.txt";

    std::vector<uint8_t> decoded = cp::py_base64decode(filename, *argv);
    
    return 0;
}
