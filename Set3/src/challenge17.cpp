#include "utility.hpp"
#include <cstdlib>
#include <ctime>
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

        void is_seeded() {
            static bool seeded = false;
            if (!seeded) {
                srand(std::time(0));
                seeded = true;
            }
            return;
        }

        BYTES generate_random_bytes(void) {
            is_seeded();
            BYTES result;
            while (result.size() < 16)
                result.push_back((rand() % 70)+ 32);
            return result;
        }

    public:
        Hackable() {
            _iv = generate_random_bytes();
            _key = generate_random_bytes();
        }

        cipher encrypt_string(void) {
            cipher response;
            BYTES random_text = texts[rand() % texts.size()];
            pkcs7_padding(random_text);
            response.ciphertext = random_text;
            response.iv = _iv;
            
            return response;
        }
};

// Function One
// Select A String Randomly 
// Pad it
// Encrypt it

// Function Two
// Accept ciphertext
// Decrypt it 
// Check padding
// return valid padding ? true : false

int main(void) {
    Hackable server;
    cipher response = server.encrypt_string();
    try {
        is_valid_pkcs7(response.ciphertext);    
    } catch (PaddingError &e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
