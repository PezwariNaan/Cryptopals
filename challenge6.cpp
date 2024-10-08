#include "utility.hpp"

int get_hamming_distace(std::vector<uint8_t> &first, std::vector<uint8_t> &second) {

    if (first.size() != second.size()) {
        std::cout << "Cannot find hamming distance. Strings are different lengths" << std::endl;
        return -1;
    }

    int score = 0;
    for (size_t i = 0; i < first.size(); i++) {
        uint8_t xored_byte = first[i] ^ second[i];
        while (xored_byte > 0) {
            score += xored_byte & 1;
            xored_byte >>= 1;
        }
    }

    return score;
}

int main(void) {
    std::string first  = "this is a test";
    std::string second = "wokka wokka!!!";

    std::vector<uint8_t> first_bytes(first.begin(), first.end());
    std::vector<uint8_t> second_bytes(second.begin(), second.end());

    int hamming_distance = get_hamming_distace(first_bytes, second_bytes);
    std::cout << hamming_distance << std::endl;

    return 0;
}
