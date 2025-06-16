#include <cstdint>
#include <utility.hpp>

uint32_t unshift_right_xor(uint32_t y, uint8_t shift) {
    uint32_t result = 0;
    for (uint8_t i = 0; i < 32; i++) {
        uint32_t part = (y ^ (result >> shift) & (1 << i));
        result |= part;
    }
    
    return result;
}

uint32_t unshift_left_xor_mask(uint32_t y, uint8_t shift, uint32_t mask) {
    uint32_t result = 0;
    for (uint8_t i = 0; i < 32; i++) {
        uint32_t bit = 1 << (32 - i);
        uint32_t part = (y ^ ((result << shift) & mask)) & bit;
        result |= part;
    }

    return result;
}

int main(void) {
    std::cout << "Hello World!\n";
    return 0;
}