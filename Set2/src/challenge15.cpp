#include "utility.hpp"
#include <algorithm>

class PaddingError: public std::exception {
    private :
        std::string msg;

    public:
        explicit PaddingError(const std::string &message) : msg(message) {}

        const char* what() const noexcept override {
            return msg.c_str();
        }
};

void is_valid_pkcs7(BYTES input) {
    if (input.empty()) {
        throw PaddingError("No Input");
    }

    char expected_bytes = input.back();

    if (expected_bytes <= 0 || (size_t)expected_bytes > input.size()) {
        throw PaddingError("Invalid Padding Length");
    }

    size_t padding_start = input.size() - expected_bytes;

    if (!std::all_of(input.begin() + padding_start, input.end(), [expected_bytes](char c) {return c == expected_bytes;})) {
        throw PaddingError("Invalid Padding Bytes");
    }

}
int main(void) {
    // Test inputs
    std::string input_str1 = "ICE ICE BABY\x04\x04\x04\x04";
    std::string input_str2 = "ICE ICE BABY\x04\x04\x05\x04";
    std::string input_str3 = "ICE ICE BABY\x04\x04\x04";
    std::string input_str4 = "";

    // Convert strings to BYTES
    BYTES input(input_str1.begin(), input_str1.end());
    BYTES input2(input_str2.begin(), input_str2.end());
    BYTES input3(input_str3.begin(), input_str3.end());
    BYTES input4(input_str4.begin(), input_str4.end());

    // Check each input and handle the exception
    try {
        is_valid_pkcs7(input); 
        std::cout << "Input 1 passed padding check." << std::endl;
    } catch (PaddingError &e) {
        std::cout << "Input 1 failed padding check: " << e.what() << std::endl;
    }

    try {
        is_valid_pkcs7(input2);
        std::cout << "Input 2 passed padding check." << std::endl;
    } catch (PaddingError &e) {
        std::cout << "Input 2 failed padding check: " << e.what() << std::endl;
    }

    try {
        is_valid_pkcs7(input3); 
        std::cout << "Input 3 passed padding check." << std::endl;
    } catch (PaddingError &e) {
        std::cout << "Input 3 failed padding check: " << e.what() << std::endl;
    }

    try {
        is_valid_pkcs7(input4);
        std::cout << "Input 4 passed padding check." << std::endl;
    } catch (PaddingError &e) {
        std::cout << "Input 4 failed padding check: " << e.what() << std::endl;
    }

    return 0;
}
