#include "utility.hpp"
#include "encoding.hpp"
#include <cstdint>
#include <stdexcept>
#include <sys/types.h>

// Implement PKCS#7 Padding
#define MAX_PADDING 16

std::vector<uint8_t> add_pkcs7_padding(uint8_t specified_padding, std::vector<uint8_t> &my_vector) {
        std::vector<uint8_t> my_vec_with_padding = my_vector;
        for (uint8_t i = 0; i < specified_padding; i++) {
        if (specified_padding > MAX_PADDING) {
            throw std::runtime_error("Specified Padding Cannot Exceed 16.");
        }

        my_vec_with_padding.push_back(specified_padding);
    }
    return my_vec_with_padding;
}

int main(void) {
    std::string no_padding_string = "Hello There!";
    std::vector<uint8_t> my_vec_no_padding(no_padding_string.begin(), no_padding_string.end());

    std::vector<uint8_t> my_vec_with_padding = add_pkcs7_padding(2, my_vec_no_padding);

    std::string hex_no_padding = cp::hex_encode(my_vec_no_padding);
    std::string hex_with_padding = cp::hex_encode(my_vec_with_padding);

    std::cout << "Before Padding:\n";
    print_array(my_vec_no_padding);
    std::cout << "\nAfter Padding:\n";
    print_array(my_vec_with_padding);
    std::cout << "\n\n\n";

    std::cout << "Hex: \n";
    std::cout << hex_no_padding << "\n";
    std::cout << hex_with_padding << std::endl;
    return 0;
}
