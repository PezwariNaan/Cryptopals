#include "encoding.hpp"
#include "utility.hpp"

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

int main(void) {
    std::string plaintext = "Hello World!";
    plaintext.append(16, 'A');
    std::vector<uint8_t> plainbytes(plaintext.begin(), plaintext.end());

    uint16_t seed = time(NULL);
    std::vector<uint8_t> ciphertext = mt19937_stream_cipher(seed, plainbytes);

    return 0;
}