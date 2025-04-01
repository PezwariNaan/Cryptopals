#include "utility.hpp"

int main(void) {
    std::map<int, std::vector<uint8_t>> texts = read_lines("Texts/challenge17.txt");

    auto iter = texts.begin();

    while (iter != texts.end()) {
        print_array(iter->second); 
        std::cout << '\n';
        iter++;
    }

    return 0;
}
