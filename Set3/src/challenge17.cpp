#include "utility.hpp"

int main(void) {
    std::vector<std::vector<uint8_t>> texts = read_lines("Texts/challenge17.txt");

    auto iter = texts.begin();

    while (iter != texts.end()) {
        print_array(*iter); 
        std::cout << '\n';
        iter++;
    }

    return 0;
}
