#include "utility.hpp"
#include <cstdint>

struct answer {
    std::vector<uint8_t> plaintext;
    uint16_t seed;
};

std::vector<uint8_t> mt19937_stream_cipher(uint16_t seed, const std::vector<uint8_t> &data) {
    std::vector<uint8_t> ciphertext;
    cp::MT19937 mt(seed);

    uint32_t buffer = 0;
    int remaining = 0;

    for (uint8_t byte : data) {
        if (remaining == 0) {
            buffer = mt();
            remaining = 4;
        }

        uint8_t keystream_byte = buffer & 0xFF;
        buffer >>= 8;

        remaining--;

        ciphertext.push_back(byte ^ keystream_byte);
    }

    return ciphertext;
}

answer brute_force(std::vector<uint8_t> ciphertext) {
    answer result;
    std::vector<uint8_t> check(16, 'A');
    std::vector<uint8_t> guess;
    uint16_t seed;

    for (int i = 0; i <= 65535; i++) {
        seed = i;
        guess = mt19937_stream_cipher(seed, ciphertext);

        if (
            guess.size() >= 16 &&
            std::equal(guess.end() - 16, guess.end(), check.begin())
            ) {
            result.plaintext = guess;
            result.seed = seed;
        }    
    }

    return result;
}

int main(void) {
    std::string plaintext = "Hello World!";
    plaintext.append(16, 'A');
    std::vector<uint8_t> plainbytes(plaintext.begin(), plaintext.end());

    uint16_t seed = time(NULL);
    std::vector<uint8_t> ciphertext = mt19937_stream_cipher(seed, plainbytes);

    answer result = brute_force(ciphertext);

    print_array(result.plaintext);
    std::cout << '\n' << result.seed << std::endl;


    return 0;
}