#include "encrypting.hpp"

// Encryption
std::vector<uint8_t> cp::fixed_xor(std::vector<uint8_t> start, std::vector<uint8_t> key) {
    std::vector<uint8_t> result;

    if (key.size() != start.size()) {
        std::cerr << "Text and key are not matching lengths";
        return result;
    }

    for (size_t i = 0; i < start.size(); i++) 
        result.push_back(start[i] ^ key[i]);

    return result;
}

// --------------------------------------------------------------------
// These blocks of code are responsible for breaking XOR 'vignere' ciphers

int cp::character_lookup(std::vector<uint8_t> byte_array, int key) {
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

std::map<int, uint8_t> cp::get_scores(std::vector<uint8_t> byte_array) {

    std::map<int, uint8_t> key_score_map; // <Score, Key>

    // Score every key against the decoded cipher text
    for (int key = 0; key < 255; key++) {
        int score = cp::character_lookup(byte_array, key);
        key_score_map.insert({score, key});
    }

    return key_score_map;
}

std::vector<uint8_t> cp::single_byte_xor(std::vector<uint8_t> byte_array, uint8_t key) {
    std::vector<uint8_t> result;
    for (size_t i = 0; i < byte_array.size(); i++)
        result.push_back(byte_array[i] ^ key);
    return result;
}

result cp::attack_single_byte_xor(std::vector<uint8_t> cipher_bytes) {
    result results;
    std::map<int, uint8_t> key_score_map = get_scores(cipher_bytes);
    
    auto iter = key_score_map.begin();
    auto end = key_score_map.end(); 
    auto key = std::prev(end)->second;
    auto score = std::prev(end) ->first;
    
    results.decrypted_bytes = cp::single_byte_xor(cipher_bytes, key);
    results.key = key;
    results.score = score;
    
    return results;
}
// ----------------------------------------------------------------------
