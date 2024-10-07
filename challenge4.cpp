#include "utility.hpp"
#include "encrypting.hpp"
#include "encoding.hpp"


int main(void) {
    const std::string file_name = "./Texts/challenge4.txt";
    std::map<int, std::string> cipher_text = read_lines(file_name);

    auto start = cipher_text.begin();
    auto end = cipher_text.end();

    for (;start != end; start++) {
        std::cout << start->second << "\n\n";
    }    

    return 0;
}
