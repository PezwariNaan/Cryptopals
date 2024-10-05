#include <vector>
#include <iostream>

void print_array(std::vector<uint8_t> array);

void print_array(std::vector<uint8_t> array) {
    for (size_t i = 0;i  < array.size(); i++)
        std::cout << array[i];
    return;
}