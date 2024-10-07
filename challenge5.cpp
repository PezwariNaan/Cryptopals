#include "utility.hpp"
#include "encoding.hpp"

int main(void) {
    std::string filename = "./Text/challenge5.txt";
    std::map<int, std::string> plain_text = read_lines(filename);
    
    auto current_element = plain_text.begin();
    auto end = plain_text.end();

    for (; current_element != end; current_element++) {
        std::cout << current_element->second << "\n";
    }


    return 0;
}