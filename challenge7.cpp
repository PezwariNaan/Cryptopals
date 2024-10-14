#include <iostream>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <ostream>
#include <sys/types.h>
#include "utility.hpp"
#include "encoding.hpp"

int main(void) {
    //EVP_CIPH_ECB_MODE
    std::vector<uint8_t> cipher_bytes = read_file_bytes("./Texts/challenge7.txt");
    std::vector<uint8_t> cipher_bytes_decoded = cp::base64_decode(cipher_bytes);
    std::vector<uint8_t> decrypted_text(cipher_bytes_decoded.size() + AES_BLOCK_SIZE);

    uint8_t *raw_cipher_bytes = cipher_bytes_decoded.data();
    int raw_bytes_size = cipher_bytes_decoded.size();
    uint8_t *key = (uint8_t *)"YELLOW SUBMARINE";

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER *cipher = EVP_aes_128_ecb();
    EVP_CIPHER_CTX_set_padding(ctx, 0);

    if (EVP_DecryptInit_ex(ctx, cipher, nullptr, key, nullptr) != 1) {
        std::cerr << "EVP_DecryptInit_ex failed :(" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return 1; 
    }

    int len = 0;
    int plaintext_len = 0;

    if (EVP_DecryptUpdate(ctx, decrypted_text.data(), &len, raw_cipher_bytes, raw_bytes_size) != 1) {
        std::cerr << "DecryptUpdate Failed :(" << std::endl;
        return 1;
    }
    plaintext_len += len;

    if (EVP_DecryptFinal(ctx, decrypted_text.data() + len, &len) != 1) {
        std::cerr << "EVP_DecryptFinal failed :(" << std::endl;
        return 1;
    }
    plaintext_len += len;

    decrypted_text.resize(plaintext_len);
    EVP_CIPHER_CTX_free(ctx);

    print_array(decrypted_text);
    std::cout << std::endl;
    return 0;
}
