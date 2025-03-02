#ifndef ENCODING_H
#define ENCODING_H

#include <exception>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <cstdint>
#include <cmath>

class DecodeError : public std::exception {
        private: 
            std::string message_;

        public:
            explicit DecodeError(const std::string &msg);
            const char* what() const noexcept override;
};

namespace cp {
    // Base64 Functions
    uint8_t* b64_decode(const std::string buffer, size_t &length, bool strict_mode);
    std::string base64_encode(const std::vector<uint8_t> &input);


    // Hex Functions
    std::vector<uint8_t> hex_decode(const std::vector<uint8_t> &input);
    std::string hex_encode(const std::vector<uint8_t> &input);
}

#endif // ENCODING_H
