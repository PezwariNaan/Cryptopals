#include "utility.hpp"

#define BYTES std::vector<uint8_t>

class DecodeError : public std::exception {
    private: 
       std::string message_;
    
    public: 
        DecodeError(std::string msg) : message_(msg) {}
        const char* what() const noexcept override {
            return message_.c_str();
        }
};

int b64_to_char(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}

BYTES b64_decode(const std::string loaded_file) {
    BYTES vec_loaded_file(loaded_file.begin(), loaded_file.end());

    BYTES vec_decoded;

    // Get first 4 characters (24 bits)
    // Convert base64 value to 6 bit value
    for (size_t i = 0; i < vec_loaded_file.size(); i++) {
        vec_loaded_file[i] =  b64_to_char(vec_loaded_file[i]);
    }

    // Go 4 bytes at a time & Concat them into one variable
    for (size_t i = 0; i < vec_loaded_file.size() ; i += 4) {
        BYTES block(vec_loaded_file.begin() + i, vec_loaded_file.begin() + (i + 4));
        uint32_t combined = (block[0] << 18 | block[1] << 12 | block[2] << 6 | block[3]);

        uint8_t first_byte  = combined >> 16;
        uint8_t second_byte = combined >> 8;
        uint8_t third_byte  = combined;
        
        vec_decoded.push_back(first_byte);
        vec_decoded.push_back(second_byte);
        vec_decoded.push_back(third_byte);
    }

    return vec_decoded;
}

int main(void) {
    try {
        std::string encoded_string = read_file("Texts/64test.txt");
        BYTES decoded_bytes = b64_decode(encoded_string);
        print_array(decoded_bytes);
        std::cout << std::endl;
        
        return 0;

    } catch(const DecodeError &e) {
        std::cout << "Caught Exception: " << e.what() << std::endl;
        return -1;
    }
}
