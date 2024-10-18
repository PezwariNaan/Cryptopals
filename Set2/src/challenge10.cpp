#include "utility.hpp"

// Generate IV
std::vector<uint8_t> generate_iv(void) {
    std::vector<uint8_t> iv;

    for (char i = 0; i < 16; i++) {
        iv.push_back(0);
    }

    return iv;
} 
// Encrypt Block 

// XOR Block 

// Implement CBC

int main(void) {

    std::vector<uint8_t> read_file =  read_file_bytes("./Texts/challenge10_decoded");
    print_array(read_file);
    std::cout << std::endl;
    return 0;
}