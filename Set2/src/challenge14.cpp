#include "utility.hpp"

#define BYTES std::vector<uint8_t>
#define BASE64PAD '='

class DecodeError : public std::exception {
    private: 
       std::string message_;
    
    public: 
        DecodeError(std::string msg) : message_(msg) {}
        const char* what() const noexcept override {
            return message_.c_str();
        }
};

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

int b64_to_char(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}

uint8_t* b64_decode(const std::string buffer, size_t &length, bool strict_mode) {
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

int main(void) {
    try {
        std::string encoded_string = read_file("Texts/64test.txt");
        size_t length = encoded_string.length();
        uint8_t *decoded_bytes = b64_decode(encoded_string, length, false);
        BYTES byte_vec(decoded_bytes, decoded_bytes + length);
        print_array(byte_vec);
        return 0;

    } catch(const DecodeError &e) {
        std::cout << "Caught Exception: " << e.what() << std::endl;
        return -1;
    }
}
