#ifndef UTILITY_H
#define UTILITY_H

// Includes all of header needed for h4ck1ng
#include <algorithm>
#include <cstddef>
#include <sys/types.h>
#include <vector>
#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

#define BYTES std::vector<uint8_t>
// Declarations
void print_array(std::vector<uint8_t> array);
const std::string read_file(const std::string &file_name);
const std::vector<std::vector<uint8_t>> read_lines(const std::string filename);
const std::vector<uint8_t> read_file_bytes(const std::string &file_name);
std::vector<std::vector<uint8_t>> create_blocks(const std::vector<uint8_t> plaintext);
void is_valid_pkcs7(BYTES input);
inline void pkcs7_padding(BYTES &input);
namespace cp {
    class MT19937 {
        private:
            static constexpr int n = 624; 
            static constexpr int m = 397; // Middle Word
            static constexpr int w = 32;  // Word Length
            static constexpr int r = 31;  // Left shift
            static constexpr unsigned long a  = 0x9908b0dfUL; // Twist Coefficient 
            static constexpr uint32_t u  = 11;                // Tempering Bit Shift
            static constexpr uint32_t s  = 7;                 // Tempering Bit Shift 
            static constexpr int t = 15;                      // Tempering Bit Shift
            static constexpr int l = 18;                      // Tempering Bit Shift
            static constexpr int b = 0x9d2c5680UL;            // Tempering Bit Mask
            static constexpr int c = 0xefc60000UL;            // Tempering Bit Mask
            static constexpr unsigned long f = 1812433253UL;  // Initialisation Value

            static constexpr unsigned long UMASK = (0xffffffffUL << r);  // Upper Bit Mask
            static constexpr uint32_t LMASK = (0xffffffffUL >> (w - r)); // Lower Bit Mask 

            uint32_t state_array[n];
            int state_index = n + 1;
            
            void initalise( uint32_t seed);

            void twist();
        
        public:
            MT19937(uint32_t seed);
            void set_state(std::vector<uint32_t> &state);

            uint32_t operator() ();
    };
    
    inline void MT19937::initalise( uint32_t seed) {
        state_array[0] = seed;

        for (int i = 1; i < n; ++i) {
            seed = f * (seed ^ (seed >> (w - 2))) + i;
            state_array[i] = seed;
        }
        state_index = 0;
    }

    inline void MT19937::twist() {
        for (int i = 0; i < n; ++i) {
            int j = (i + 1) % n;
            uint32_t x = (state_array[i] & UMASK )| (state_array[j] & LMASK);
            uint32_t xA = x >> 1;
            if (x & 0x1) xA ^= a;
            int k = (i + m) % n;
            state_array[i] = state_array[k] ^ xA;
        }
        state_index = 0;
    }

    inline void MT19937::set_state(std::vector<uint32_t> &state) {
        for (int i = 0; i < 624; i++) {
            state_array[i] = state[i];
        }

        state_index = 624;
    }

    inline MT19937::MT19937(uint32_t seed) {
        initalise(seed);
    }

    inline uint32_t MT19937::operator() () {
        if (state_index >= n) twist();

        uint32_t x = state_array[state_index++];
        x ^= (x >> u);
        x ^= (x << s) & b;
        x ^= (x << t) & c;
        x ^= (x >> l);
        return x;
    }
}
//-------------------------------------------

// Definitions
inline void print_array(std::vector<uint8_t> array) {
    for (size_t i = 0;i  < array.size(); i++)
        std::cout << array[i];
    return;
}

inline const std::string read_file(const std::string &file_name){
    std::ifstream file(file_name);
    if (!file.is_open()) {
        std::cerr << "Error Opening " << file_name << std::endl;
    }

    std::stringstream buffer;
    
    buffer << file.rdbuf();
    return buffer.str();
}

inline const std::vector<std::vector<uint8_t>> read_lines(const std::string file_name) {
    std::ifstream file(file_name);
    if (!file.is_open()) {
        std::cerr << "Error Opening " <<file_name << std::endl;
    }
    std::vector<std::vector<uint8_t>> lines; 
    std::string line_content;

    while(std::getline(file, line_content)) {
        std::vector<uint8_t> line_content_bytes(line_content.begin(), line_content.end());
        lines.push_back(line_content_bytes);
    }

    return lines;
}

inline const std::vector<uint8_t> read_file_bytes(const std::string &file_name) {
    // Open file in binary mode
    std::ifstream file(file_name, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error opening " << file_name << std::endl;
        return {};
    }

    // Move the file cursor to the end to get the file size
    file.seekg(0, std::ios::end);
    std::streamsize file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Create a vector to store the raw bytes
    std::vector<uint8_t> buffer(file_size);

    // Read file contents into the vector
    if (!file.read(reinterpret_cast<char*>(buffer.data()), file_size)) {
        std::cerr << "Error reading " << file_name << std::endl;
        return {};
    }

    return buffer;
}

inline std::vector<std::vector<uint8_t>> create_blocks(const std::vector<uint8_t> plaintext, const int blocksize) {
    std::vector<std::vector<uint8_t>> block_vector; // All blocks
    size_t i = 0;
    for (; i + blocksize <= plaintext.size(); i += blocksize)
        block_vector.emplace_back(plaintext.begin() + i, plaintext.begin() + i + blocksize);

    // Pad block if necessary
    if (i < plaintext.size()) {
        std::vector<uint8_t> block(plaintext.begin() + i, plaintext.end());
        size_t padding_needed = blocksize - block.size();
        block.insert(block.end(), padding_needed, padding_needed);

        // Add block to block_vector;
        block_vector.push_back(block);
    }

    return block_vector;

}
#ifndef PADDING_ERROR_HPP
#define PADDING_ERROR_HPP

class PaddingError: public std::exception {
    private :
        std::string msg;

    public:
        explicit PaddingError(const std::string &message) : msg(message) {}

        const char* what() const noexcept override {
            return msg.c_str();
        }
};

#endif // PADDING_ERROR_HPP

inline void is_valid_pkcs7(BYTES input) {
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

inline void pkcs7_padding(BYTES &input) {
    int blocksize = 16;
    int padding = blocksize - (input.size() % blocksize);
    input.insert(input.end(), padding, padding);
    
    return;
}
//-------------------------------------------

#endif // UTILITY_H
