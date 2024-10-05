#include <vector>

std::vector<uint8_t> fixed_xor(std::vector<uint8_t> start, std::vector<uint8_t> key) {
    std::vector<uint8_t> result;

    if (key.size() != start.size()) {
        std::cerr << "Text and key are not matching lengths";
        return result;
    }

    for (size_t i = 0; i < start.size(); i++) 
        result.push_back(start[i] ^ key[i]);

    return result;
}
