#include "utility.hpp"
#include <random>
#include "openssl.hpp"
#include "encoding.hpp"

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
};

// Function One
// Select A String Randomly 
// Pad it
// Encrypt it
// Return ciphertext and iv

// Function Two
// Accept ciphertext
// Decrypt it 
// Check padding
// return valid padding ? true : false

int main(void) {
    Hackable server;
    cipher response = server.encrypt_string();
    std::cout << cp::hex_encode(response.ciphertext) << std::endl;
    std::cout << '\n';
    std::cout << cp::hex_encode(response.iv) << std::endl;

    return 0;
}
