#include "utility.hpp"

int main(void) {
    const std::string filename = "./Texts/challenge8.txt";
    std::map<int, std::vector<uint8_t>> file_lines = read_lines(filename);
    auto iter = file_lines.begin();
    while (iter != file_lines.end()) {
        std::cout << iter->first << ") ";
        print_array(iter->second);
        std::cout << "\n\n";
        iter++;
    }

    return 0;
}