#include "encoding.hpp"
#include <openssl.hpp>
#include <utility.hpp>

BYTES generate_key(void) {
    BYTES key;
    std::random_device rd;
    for (int i = 0; i < 16; i++) {
        key.push_back((rd() % 90) + 33); // Between 33 & 126
    }
    return key;
}

int main(void) {
    // Variables
    std::vector<BYTES> lines = read_lines("Texts/challenge19.txt");
    std::vector<BYTES> encrytped_lines;
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    long nonce = 0;
    BYTES key = generate_key();
    size_t blocksize = 16;
    bool running = true;

    for (size_t i = 0; i < lines.size(); i++) {
        std::string line_str(lines[i].begin(), lines[i].end());
        size_t len = line_str.length();
        uint8_t *decoded_ptr = cp::base64_decode(line_str, len, false);
        BYTES decoded(decoded_ptr, decoded_ptr + len);
        BYTES encrytped = openssl::aes_ctr(ctx, decoded, key, nonce, blocksize);
        encrytped_lines.push_back(encrytped);
    }

    // Crib-dragging
    size_t min_len = SIZE_MAX;
    for (const auto &line : encrytped_lines) {
        if (line.size() < min_len) {
            min_len = line.size();
        }
    }

    std::cout << "\nGuess: ";
    
    while (running) {

        std::string input_line; 
        std::getline(std::cin, input_line);

        if (input_line == ":q") {
            running = false;
            break;
        }

        BYTES guess(input_line.begin(), input_line.end());

        for (size_t i = 1; i < encrytped_lines.size(); i++) {
            for (size_t j = 0; j < encrytped_lines[i].size(); j++) {
                uint8_t plaintext_byte = '?';
                if (j < guess.size()) {
                    uint8_t candidateK = encrytped_lines[0][j] ^ guess[j];
                    plaintext_byte = encrytped_lines[i][j] ^ candidateK;
                }
                std::cout << (std::isprint(plaintext_byte) ? (char)plaintext_byte : '.');
            }
            std::cout << '\n';
        }
        std::cout << "\nGuess: ";
    }
    return 0;
}
