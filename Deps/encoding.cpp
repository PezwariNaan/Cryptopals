#include "encoding.hpp"
#include <cstddef>
#include <cstdint>
#include <exception>
#include <sys/types.h>

#ifndef BASE64PAD
#define BASE64PAD '='
#endif 

DecodeError::DecodeError(const std::string &msg) : message_(msg) {}

const char*  DecodeError::what() const noexcept {
    return message_.c_str();
}

constexpr uint8_t b64de_table[256] = {
    255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,
    255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,
    255,255,255,255, 255,255,255,255, 255,255,255, 62, 255,255,255, 63,
    52 , 53, 54, 55,  56, 57, 58, 59,  60, 61,255,255, 255,  0,255,255,

    255,  0,  1,  2,   3,  4,  5,  6,   7,  8,  9, 10,  11, 12, 13, 14,
    15 , 16, 17, 18,  19, 20, 21, 22,  23, 24, 25,255, 255,255,255,255,
    255, 26, 27, 28,  29, 30, 31, 32,  33, 34, 35, 36,  37, 38, 39, 40,
    41 , 42, 43, 44,  45, 46, 47, 48,  49, 50, 51,255, 255,255,255,255,

    255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,
    255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,
    255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,
    255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,

    255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,
    255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,
    255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255,
    255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255};

std::string cp::base64_encode(const std::vector<uint8_t> &input) {
	std::string b64_string;
	int input_len = input.size();
	if (input_len <= 0) return "No Input";
	
	// Calculate length of base64 string and reserve memory
	int b64_length = std::floor((input_len + 2) / 3) * 4;
	b64_string.reserve(b64_length);

	// main convertion logic
	for (int i = 0; i < input_len; i += 3) {
		uint8_t byte1 = input[i];
		uint8_t byte2 = (i + 1 < input_len) ? input[i + 1] : 0;
		uint8_t byte3 = (i + 2 < input_len) ? input[i + 2] : 0;
		
		// Get first 6 bits
		b64_string.push_back(b64de_table[byte1 >> 2]);
		// Append previous 2 bytes to next 4
		// Use bitwise AND to get relevant bit§.
		// Use bitwise OR to create new 6 bit value
		b64_string.push_back(b64de_table[((byte1 & 0x03) << 4) | (byte2 >> 4)]);
		// Repeat previous step
		b64_string.push_back(b64de_table[((byte2 & 0x0F) << 2) | (byte3 >> 6)]);
		// get last 6 bits with bitwise AND
		b64_string.push_back(b64de_table[byte3 & 0x3F]);
	}

	int padding = input_len % 3;
	if (padding > 0) {
		b64_string[b64_length - 1] = '=';
		if (padding == 1) 
			b64_string[b64_length - 2] = '=';
	}
	
	return b64_string;
}

int b64_to_char(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}

uint8_t* cp::b64_decode(const std::string buffer, size_t &length, bool strict_mode) {
    std::string error_message;

    const uint8_t *ascii_data = (const uint8_t *)buffer.data();
    size_t ascii_len = length;
    bool padding_started = 0;

    size_t bin_len = ascii_len / 4 * 3; 
    uint8_t *bin_data = new (std::nothrow) uint8_t[bin_len + 1];
    if(!bin_data) {
        throw std::runtime_error("Failed to allocate memory for bin_data.");
    }
    uint8_t *bin_data_start = bin_data;
    bin_data[bin_len] = 0x0;

    uint8_t leftchar = 0;
    uint32_t quad_pos = 0;
    uint32_t pads = 0;

    if(strict_mode && (ascii_len > 0) && (*ascii_data == BASE64PAD)) {
        error_message = "Leading padding not allowed.";
        goto error_end;
    }

    size_t i;
    uint8_t this_ch;
    for(i = 0; i < ascii_len; ++i) {
        this_ch = ascii_data[i];

        if(this_ch == BASE64PAD) {
            padding_started = true;
            // If the current character is a padding character, the length
            // will be reduced by one to obtain the decoded true length.
            bin_len--;

            if(strict_mode && (!quad_pos)) {
                error_message = "Excess padding not allowed.";
                goto error_end;
            }

            if((quad_pos >= 2) && (quad_pos + (++pads) >= 4)) {

                if(strict_mode && ((i + 1) < ascii_len)) {
                    error_message = "Excess data after padding.";
                    goto error_end;
                }

                goto done;
            }

            continue;
        }

        this_ch = b64de_table[this_ch];
        if(this_ch == 255) {
            if(strict_mode) {
                error_message = "Only base64 data is allowed.";
                goto error_end;
            }
            continue;
        }

        if(strict_mode && padding_started) {
            error_message = "Discontinuous padding not allowed.";
            goto error_end;
        }

        pads = 0;

        switch(quad_pos) {
        case 0:
            quad_pos = 1;
            leftchar = this_ch;
            break;
        case 1:
            quad_pos = 2;
            *bin_data++ = (leftchar << 2) | (this_ch >> 4);
            leftchar = this_ch & 0xf;
            break;
        case 2:
            quad_pos = 3;
            *bin_data++ = (leftchar << 4) | (this_ch >> 2);
            leftchar = this_ch & 0x3;
            break;
        case 3:
            quad_pos = 0;
            *bin_data++ = (leftchar << 6) | (this_ch);
            leftchar = 0;
            break;
        }
    }

    if(quad_pos) {
        if(quad_pos == 1) {
            char tmpMsg[128]{};
            snprintf(tmpMsg, sizeof(tmpMsg),
                    "Invalid base64-encoded string: "
                    "number of data characters (%zd) cannot be 1 more "
                    "than a multiple of 4",
                    (bin_data - bin_data_start) / 3 * 4 + 1);
            error_message = tmpMsg;
            goto error_end;
        } else {
            error_message = "Incorrect padding.";
            goto error_end;
        }
        error_end:
        delete[] bin_data;
        throw std::runtime_error(error_message);
    }

    done:
    length = bin_data - bin_data_start;
    return bin_data_start;
}

std::vector<uint8_t> cp::hex_decode(const std::vector<uint8_t> &input) {
	std::vector<uint8_t> byte_array;

	for (size_t i = 0; i < input.size(); i+=2) {
		if (i + 1 >= input.size()) break;
		
		// 1 Byte = 2 hex characters; so that's what we get
		uint8_t first_nibble = input[i];
		uint8_t second_nibble = input[i + 1];

		std::string byte_string;
		byte_string.push_back(static_cast<char>(first_nibble));
		byte_string.push_back(static_cast<char>(second_nibble));
		
		// Convert the 2 characters into one byte
		uint8_t byte_value = static_cast<uint8_t> (
					std::stoi(byte_string, nullptr, 16)
				);
		byte_array.push_back(byte_value);
	}

	return byte_array;
}

std::string cp::hex_encode(const std::vector<uint8_t> &input) {
	std::stringstream hex_string;
	hex_string << std::hex;

	for (size_t i = 0; i < input.size(); i++) {
		hex_string << std::setw(2) << std::setfill('0') << static_cast<int>(input[i]);
	}
	
	return hex_string.str();
}
