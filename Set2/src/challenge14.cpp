#include "utility.hpp"
#include "encoding.hpp"

int main(void) {
    try {
        std::string encoded_string = read_file("Texts/64test.txt");
        size_t length = encoded_string.length();
        uint8_t *decoded_bytes = cp::b64_decode(encoded_string, length, false);
        BYTES byte_vec(decoded_bytes, decoded_bytes + length);
        print_array(byte_vec);
        return 0;

    } catch(const DecodeError &e) {
        std::cout << "Caught Exception: " << e.what() << std::endl;
        return -1;
    }
}
