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
