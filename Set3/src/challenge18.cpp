#include <cstring>
#include <encoding.hpp>
#include <utility.hpp>
#include <openssl.hpp>

#define BLOCKSIZE (size_t)16

BYTES aes_ctr(BYTES input, BYTES key, long nonce) {
    BYTES plaintext;
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    size_t num_blocks = (input.size() + BLOCKSIZE - 1) / BLOCKSIZE;

    for (size_t i = 0; i < num_blocks; i++) {
        BYTES counter_block(BLOCKSIZE, 0);
        memcpy(&counter_block[0], &nonce, 8);
        uint64_t counter = i;
        memcpy(&counter_block[8], &counter, 8);

        BYTES keystream = openssl::encrypt_ecb(ctx, counter_block, key);
        size_t offset = i * BLOCKSIZE;
        size_t len = std::min(BLOCKSIZE, input.size() - offset);
        BYTES cipher_block(input.begin() + offset, input.begin() + offset + len);
        
        for (size_t j = 0; j < len; j++) {
            plaintext.push_back(cipher_block[j] ^ keystream[j]);
        }
    }

    return plaintext;
}

int main(void) {
    std::string cipher_encoded = "L77na/nrFsKvynd6HzOoG7GHTLXsTVu9qvY/2syLXzhPweyyMTJULu/6/kXX0KSvoOLSFQ==";
    size_t len = cipher_encoded.length();

    uint8_t *cipher_decoded = cp::base64_decode(cipher_encoded, len, false);
    BYTES ciphertext(cipher_decoded, cipher_decoded + len);

    std::string key_str = "YELLOW SUBMARINE";
    BYTES key(key_str.begin(), key_str.end());
    long nonce = 0;

    BYTES plaintext = aes_ctr(ciphertext, key, nonce);

    print_array(plaintext);
    std::cout << std::endl;
    return 0;
}
