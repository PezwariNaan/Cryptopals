#include "utility.hpp"
#include <openssl/evp.h>
#include <random>
#include "openssl.hpp"

struct cipher {
    BYTES ciphertext;
    BYTES iv;
};

class Hackable {
    private:
        BYTES _key;
        BYTES _iv;
        std::vector<std::vector<uint8_t>> texts = read_lines("Texts/challenge17.txt");

        BYTES generate_random_bytes(void) {
            BYTES result;
            std::random_device rd;
            while (result.size() < 16)
                result.push_back((rd() % 70)+ 32);
            return result;
        }

    public:
        Hackable() {
            _iv = generate_random_bytes();
            _key = generate_random_bytes();
        }

        cipher encrypt_string(void) {
            cipher response;
            EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();\
            int blocksize = 16;

            BYTES random_text = texts[rand() % texts.size()];
            pkcs7_padding(random_text);
            response.ciphertext = openssl::encrypt_cbc(ctx, blocksize, random_text, _key, _iv);
            response.iv = _iv;

            return response;
        }

    bool decrypt_string(cipher input) {
        int blocksize = 16;
        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        BYTES plaintext = openssl::decrypt_cbc(ctx, blocksize, input.ciphertext, _key, input.iv);
        is_valid_pkcs7(plaintext);

        return true;
    }
};

int main(void) {
    Hackable server;
    cipher response = server.encrypt_string();
    std::cout << server.decrypt_string(response) << std::endl;

    return 0;
}
