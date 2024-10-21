#ifndef ENCODING_H
#define ENCODING_H

#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <cstdint>
#include <cmath>


// Base64 Functions
namespace cp {
    std::vector<uint8_t> base64_decode(const std::vector<uint8_t> &input);
    std::string base64_encode(const std::vector<uint8_t> &input);

    // Hex Functions
    std::vector<uint8_t> hex_decode(const std::vector<uint8_t> &input);
    std::string hex_encode(const std::vector<uint8_t> &input);
}

#endif // ENCODING_H
