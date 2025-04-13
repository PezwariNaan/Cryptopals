#include <cstring>
#include <encoding.hpp>
#include <utility.hpp>
#include <openssl.hpp>

#define BLOCKSIZE (size_t)16

int main(void) {
    std::string cipher_encoded = "L77na/nrFsKvynd6HzOoG7GHTLXsTVu9qvY/2syLXzhPweyyMTJULu/6/kXX0KSvoOLSFQ==";
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    size_t len = cipher_encoded.length();

    uint8_t *cipher_decoded = cp::base64_decode(cipher_encoded, len, false);
    BYTES ciphertext(cipher_decoded, cipher_decoded + len);

    std::string key_str = "YELLOW SUBMARINE";
    BYTES key(key_str.begin(), key_str.end());
    long nonce = 0;

    BYTES plaintext = openssl::aes_ctr(ctx, ciphertext, key, nonce, BLOCKSIZE);

    print_array(plaintext);
    std::cout << std::endl;
    return 0;
}
