#include "utility.hpp"
#include "encoding.hpp"

int main(int argc, char *argv[]) {
    std::string filename = "Texts/challenge10.txt";

    std::vector<uint8_t> decoded = cp::py_base64decode(filename, *argv);
    std::cout.write(reinterpret_cast<char *>(decoded.data()), decoded.size());
    std::cout << std::endl;
    return 0;
}
