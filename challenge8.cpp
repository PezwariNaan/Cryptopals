#include "encoding.hpp"
#include "utility.hpp"
#include <openssl/safestack.h>
#include <ostream>
#include <sys/types.h>

#define BLOCKSIZE 16

int main(void) {
    const std::string filename = "./Texts/challenge8.txt";
    std::map<int, std::vector<uint8_t>> file_lines = read_lines(filename);
    
    auto iter = file_lines.begin();
    int line_number = 0;
    std::vector<uint8_t> encrypted_with_ecb;

    while (iter != file_lines.end()) {
        // Break-up everyline into BLOCKSIZE chunks. 
        // Compare every chunk and if there is a match; 
        // It is likely encrypted with AES_ECB.
        std::map<std::vector<uint8_t>, int> lookup_table;
        std::vector<uint8_t> decoded_line = cp::hex_decode(iter->second); // Decode from hex to bytes

        for (int i = 0; i < decoded_line.size(); i += BLOCKSIZE) {
            std::vector<uint8_t> block;
            for (int j = i; j < i + BLOCKSIZE; j++) {
                block.push_back(decoded_line[j]);
            }
            // Check lookup for block 
            // If present: ECB Cipher
            if (lookup_table.find(block) != lookup_table.end()) {
                std::cout << "Line Number: " << line_number << "\n";
                print_array(iter->second);
                std::cout << std::endl;
                return 0;
            }
            // If not present: insert into lookup table
            lookup_table.insert({block, 1});
        }
        iter++;
        line_number++;
    }
    // Else no match found
    std::cout << "No ECB Cipher Found" << std::endl;
    return 1;
}
