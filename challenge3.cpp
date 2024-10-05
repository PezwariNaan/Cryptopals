#include "encoding.hpp"
#include "encrypting.hpp"
#include "utility.hpp"
#include <algorithm>
#include <map>

int character_lookup(std::vector<uint8_t> byte_array, int key) {
    const std::string common_string = "ETAOIN SHRDLU";
    std::vector<uint8_t> common_characters(common_string.begin(), common_string.end());
    int score = 0;

    for (size_t i = 0; i < byte_array.size(); i++) {
        uint8_t xored_byte = byte_array[i] ^ key;
        // Score++ if xored_byte is in common_characters
        if (std::isalpha(xored_byte)) {
            uint8_t xored_byte_upper = std::toupper(xored_byte);

            if (std::find(common_characters.begin(), common_characters.end(), xored_byte_upper) \
            != common_characters.end()) {
                score++;
           }
           
        } else if (xored_byte == ' ')
            score++;
    }

    return score;
}

std::map<int, uint8_t> attack_single_byte_xor(std::vector<uint8_t> byte_array) {

    std::map<int, uint8_t> key_score_map; // <Score, Key>

    for (int key = 0; key < 255; key++) {
        int score = character_lookup(byte_array, key);
        key_score_map.insert({score, key});
    }

    return key_score_map;
}

std::vector<uint8_t> single_byte_xor(std::vector<uint8_t> byte_array, uint8_t key) {
    std::vector<uint8_t> result;
    for (size_t i = 0; i < byte_array.size(); i++)
        result.push_back(byte_array[i] ^ key);
    return result;
}

int main(void) {
    std::string cipher_text = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";
    std::vector cipher_bytes = cp::hex_decode(cipher_text);
    std::map<int, uint8_t> key_score_map = attack_single_byte_xor(cipher_bytes);
    
    auto iter = key_score_map.begin();
    auto end = key_score_map.end(); 
    auto last_element = std::prev(end);
    std::vector<uint8_t> plain_bytes = single_byte_xor(cipher_bytes, last_element->second);

    std::cout <<"Results:\n--------------------------\n";
    while (iter++ != key_score_map.end()) {
        std::cout << "Key     : " << iter->second << '\n';
        std::cout << "Key_Int : " << static_cast<int>(iter->second) << '\n';
        std::cout << "Score   : " << iter->first << '\n';
    }
    
    std::cout << "\n\n\nKey: " << (last_element->second) << '\n';
    std::cout << "Plaintext:\n";
    print_array(plain_bytes);
    std::cout << std::endl;
    

    return 0;
}
