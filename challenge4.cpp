#include "utility.hpp"
#include "encrypting.hpp"
#include "encoding.hpp"
#include <memory>

int main(void) {
    const std::string file_name = "./Texts/challenge4.txt";
    std::map<int, std::string> cipher_text = read_lines(file_name);

    auto current_element = cipher_text.begin();
    auto end = cipher_text.end();

    std::unique_ptr<result> final_result = std::make_unique<result>();
    int final_result_line_number;

    for (; current_element != end; current_element++) {
        std::vector<uint8_t> cipher_bytes = cp::hex_decode(current_element->second);
        std::unique_ptr<result> results = std::make_unique<result>();
        *results = cp::attack_single_byte_xor(cipher_bytes);

        if (results->score > final_result->score) {
            final_result = std::move(results);
            final_result_line_number = current_element->first;
        }
    }

    std::cout << "Key: " << final_result->key << "\n\n";
    std::cout << "Line Number: " << final_result_line_number << "\n\n";
    std::cout << "Plain Text:\n";
    print_array(final_result->decrypted_bytes);
    std::cout << std::endl;

    return 0;
}
