#include "encoding.hpp"
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <openssl.hpp>
#include <random>
#include <sys/types.h>
#include <utility.hpp>
#include <vector>

BYTES generate_key(void) {
    BYTES key;
    std::random_device rd;
    for (int i = 0; i < 16; i++) {
        key.push_back((rd() % 90) + 33); // Between 33 & 126
    }
    return key;
}

uint8_t keystream_guess(uint8_t plaintext_guess, uint8_t ciphertext_byte) {
    return plaintext_guess ^ ciphertext_byte;
}

int main(void) {
    // Variables
    std::vector<BYTES> lines = read_lines("Texts/challenge19.txt");
    std::vector<BYTES> decoded_lines;
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    long nonce = 0;
    BYTES key = generate_key();
    size_t blocksize = 16;
    BYTES guess;
    bool running = true;

    // Decode & encrypt each line
    for (size_t i = 0; i < lines.size(); i ++) {
        std::string lines_str(lines[i].begin(), lines[i].end());
        size_t len = lines_str.length();
        uint8_t *decoded_ptr = cp::base64_decode(lines_str, len, false);
        BYTES decoded(decoded_ptr, decoded_ptr + len);
        BYTES encrypted = openssl::aes_ctr(ctx, decoded, key, nonce, blocksize);
        decoded_lines.push_back(decoded);
    }

    while (running) {
        std::cout << "Guess: ";

        std::string input_line;
        std::getline(std::cin, input_line);

        if (input_line == ":q") {
            running = false;
            break;
        }
        
        BYTES guess(input_line.begin(), input_line.end());
        
        for (size_t i = 0; i < decoded_lines.size(); i++) {
            for (size_t j = 0; j < decoded_lines.size(); j++) {
                if (j < guess.size()) {
                    uint8_t decrypted = keystream_guess(guess[j], decoded_lines[i][j]);
                    std::cout << (std::isprint(decrypted) ? (char)decrypted : '.');
                } else {
                    std::cout << '?';
                }
            }
            std::cout << '\n';
        }
        
        std::cout << '\n';
    }
    
    return 0;
}
