#include "encoding.hpp"
#include <iostream>

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

void print_array(std::vector<uint8_t> array) {
    for (size_t i = 0;i  < array.size(); i++)
        std::cout << array[i];
}

int main(void) {
    std::string start  = "1c0111001f010100061a024b53535009181c";
    std::string key    = "686974207468652062756c6c277320657965";
    std::string target = "746865206b696420646f6e277420706c6179";

    std::vector<uint8_t> start_bytes = cp::hex_decode(start);
    std::vector<uint8_t> key_bytes   = cp::hex_decode(key);

    std::vector<uint8_t> result_bytes = fixed_xor(start_bytes, key_bytes);
    std::string result_hex = cp::hex_encode(result_bytes);
    
    if (result_hex != target) {
        std::cout << "Results don't match :(";
        return 1;
    }

    std::cout << "Match :)\n";
    std::cout << "Target: " << target << "\n";
    std::cout << "Result: " << result_hex << std::endl;
    return 0;
}