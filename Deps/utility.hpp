#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
#include <iostream>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <map>
#include <string>

// Declarations
void print_array(std::vector<uint8_t> array);
std::string read_file(const std::string &file_name);
std::map<int, std::string> read_lines(const std::string filename);
//-------------------------------------------

// Definitions
void print_array(std::vector<uint8_t> array) {
    for (size_t i = 0;i  < array.size(); i++)
        std::cout << array[i];
    return;
}

std::string read_file(const std::string &file_name){
    std::ifstream file(file_name);
    if (!file.is_open()) {
        std::cerr << "Error Opening " << file_name << std::endl;
    }

    std::stringstream buffer;
    
    buffer << file.rdbuf();
    return buffer.str();
}

std::map<int, std::string> read_lines(const std::string file_name) {
    std::ifstream file(file_name);
    if (!file.is_open()) {
        std::cerr << "Error Opening " <<file_name << std::endl;
    }
    std::map<int, std::string> lines; // line_number, line_content
    int line_number = 1;
    std::string line_content;

    while(std::getline(file, line_content)) {
        lines.insert({line_number, line_content});
        line_number++;
    }

    return lines;
}
//-------------------------------------------

#endif // UTILITY_H