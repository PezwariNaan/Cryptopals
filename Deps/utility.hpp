#ifndef UTILITY_H
#define UTILITY_H

#include <sys/types.h>
#include <vector>
#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <map>
#include <string>

// Declarations
void print_array(std::vector<uint8_t> array);
const std::string read_file(const std::string &file_name);
const std::map<int, std::vector<uint8_t>> read_lines(const std::string filename);
const std::vector<uint8_t> read_file_bytes(const std::string &file_name);
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

inline const std::map<int, std::vector<uint8_t>> read_lines(const std::string file_name) {
    std::ifstream file(file_name);
    if (!file.is_open()) {
        std::cerr << "Error Opening " <<file_name << std::endl;
    }
    std::map<int, std::vector<uint8_t>> lines; // line_number, line_content
    int line_number = 1;
    std::string line_content;

    while(std::getline(file, line_content)) {
        std::vector<uint8_t> line_content_bytes(line_content.begin(), line_content.end());
        lines.insert({line_number, line_content_bytes});
        line_number++;
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
//-------------------------------------------

#endif // UTILITY_H