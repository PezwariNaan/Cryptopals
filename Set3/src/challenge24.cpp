#include "utility.hpp"
#include <chrono>

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

uint32_t check_token(uint32_t token, time_t start, time_t end) {
    for (time_t t = start; t <= end; t++) {
        cp::MT19937 mt((uint32_t) t);
        uint32_t candidate = mt();
        
        if (candidate == token) 
            return t;
    }

    return 0;
}

int main(void) {
    std::string plaintext = "Hello World!";
    plaintext.append(16, 'A');
    std::vector<uint8_t> plainbytes(plaintext.begin(), plaintext.end());

    time_t seed = time(NULL);
    std::vector<uint8_t> ciphertext = mt19937_stream_cipher((uint16_t) seed, plainbytes);

    // Part 1 - Recover 16bit seed.
    answer result = brute_force(ciphertext);
    std:: cout << "Plaintext: ";
    print_array(result.plaintext);
    std::cout << "\nSeed: " << result.seed << "\n\n";

    // Part 2 - Check if token is a product of MT19937 seeded with current time. 
    cp::MT19937 mt(seed);
    uint32_t reset_token = mt(); 

    auto end = std::chrono::system_clock::now();
    auto start = end - std::chrono::minutes(4);
    time_t end_time = std::chrono::system_clock::to_time_t(end);
    time_t start_time = std::chrono::system_clock::to_time_t(start);

    uint32_t cracked = check_token(reset_token, start_time, end_time);

    std::cout << "Token " << reset_token << " Seeded With: " << cracked << std::endl;
    return 0;
}