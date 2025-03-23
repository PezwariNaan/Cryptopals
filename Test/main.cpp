#include "encoding.hpp"
#include "utility.hpp"
#include "openssl.hpp"

#define BYTES std::vector<uint8_t>

int main(void) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    BYTES plaintext(64, 'a');
    BYTES key(16, 'A');

    BYTES ciphertext = openssl::encrypt_cbc(ctx, 16, plaintext, key, key);
    std::cout << cp::hex_encode(ciphertext) << std::endl;
    BYTES decrypted = openssl::decrypt_cbc(ctx, 16, ciphertext, key, key);
    std::string decrypted_str(decrypted.begin(), decrypted.end());
    std::cout << decrypted_str << std::endl;
    
    return 0;
}
