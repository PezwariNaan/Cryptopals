#include "encrypting.hpp"

// Encryption
std::vector<uint8_t> cp::fixed_xor(std::vector<uint8_t> &start, std::vector<uint8_t> &key) {
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

int cp::character_lookup(std::vector<uint8_t> &byte_array, uint8_t &key) {
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

std::map<int, uint8_t> cp::get_scores(std::vector<uint8_t> &byte_array) {
    std::map<int, uint8_t> key_score_map; // <Score, Key>

    // Score every key against the decoded cipher text
    for (uint8_t key = 0; key < 255; key++) {
        int score = cp::character_lookup(byte_array, key);
        key_score_map.insert({score, key});
    }

    return key_score_map;
}

std::vector<uint8_t> cp::single_byte_xor(std::vector<uint8_t> &byte_array, uint8_t &key) {
    std::vector<uint8_t> result;
    for (size_t i = 0; i < byte_array.size(); i++)
        result.push_back(byte_array[i] ^ key);
    return result;
}

result cp::attack_single_byte_xor(std::vector<uint8_t> &cipher_bytes) {
    result results;
    std::map<int, uint8_t> key_score_map = get_scores(cipher_bytes);
    
    // Get the highest scoring key
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

std::vector<uint8_t> cp::repeating_key_xor(std::vector<uint8_t> &plain_bytes, std::vector<uint8_t> &key_bytes) {
    std::vector<uint8_t> cipher_bytes;

    for (int i = 0; i < plain_bytes.size(); i++) {
        uint8_t xored_byte = plain_bytes[i] ^ key_bytes[i % key_bytes.size()];
        cipher_bytes.push_back(xored_byte);
    }

    return cipher_bytes;
}


// Attack Repeating Key XOR

std::map<float, int> cp::get_keysize(const int MAX_KEYSIZE, std::vector<uint8_t> cipher_bytes) {
    std::map<float, int> keysize_scores;
    int likely_keysize = 2;
    // Get normalized edit distance between blocks of KEYSIZE length
    for (; likely_keysize < MAX_KEYSIZE; likely_keysize++) {
        float keysize_score = 0;
        std::vector<std::vector<uint8_t>> block_vector;

        // Gather blocks of size 'likely_keysize'
        for (int i = 0; i < cipher_bytes.size() / likely_keysize; i++) {  // Use the as many blocks as possible
            std::vector<uint8_t> block(cipher_bytes.begin() + i * likely_keysize, 
                                    cipher_bytes.begin() + (i + 1) * likely_keysize);
            
            block_vector.push_back(block);
        }

        // Calculate pairwise Hamming distances
        int comparisons = 0;
        for (size_t i = 0; i < block_vector.size(); i++) {
            for (size_t j = i + 1; j < block_vector.size(); j++) {
                int hamming_distance = cp::get_hamming_distance(block_vector[i], block_vector[j]);
                keysize_score += hamming_distance / static_cast<float>(likely_keysize);  // Normalize by likely_keysize
                comparisons++;
            }
        }

        // Average the score over the number of comparisons
        keysize_score /= comparisons;

        // Store the score for this likely_keysize
        keysize_scores.insert({keysize_score, likely_keysize});
    }
        //std::cout << "Keysize: " << likely_keysize << " Score: " << keysize_score << std::endl;
    return keysize_scores;
}

std::map<int, std::vector<uint8_t>> cp::make_blocks(int keysize, std::vector<uint8_t> &cipher_bytes) {
    std::map<int, std::vector<uint8_t>> keysize_blocks;
    int count = 1;
    
    // Transpose and propulate 'keysize_blocks' with 'block'
    for (int j = 0; j < keysize; j++) {
        std::vector<uint8_t> block;
        for (size_t i = j; i < cipher_bytes.size(); i += keysize) {
            block.push_back(cipher_bytes[i]);
        }
        keysize_blocks.insert({count, block});
        count++;
    }
    
    return keysize_blocks;
}

std::tuple<int, std::vector<uint8_t>, std::vector<uint8_t>> cp::attack_repeating_key_xor(std::vector<uint8_t> &cipher_bytes) {
    int keysize = 2;
    const int MAX_KEYSIZE = 40;
    std::map<float, int> keysize_scores = cp::get_keysize(MAX_KEYSIZE, cipher_bytes); // score, likely_keysize
    
    // Select likely_keysize with lowest edit distance 
    int likely_keysize = keysize_scores.begin()->second;

    // Break cipher_bytes into likely_keysize length blocks 
    std::map<int, std::vector<uint8_t>> keysize_blocks = cp::make_blocks(likely_keysize, cipher_bytes); // index, block

    auto element = keysize_blocks.begin();
    auto end = keysize_blocks.end();
    
    // Run attack_single_byte_xor on each transposed block 
    std::vector<result> result_vector;
    for (;element != end; element++) {
        result results = cp::attack_single_byte_xor(element->second);
        result_vector.push_back(results);
    }

    std::vector<uint8_t> likely_key;
    for (size_t i = 0; i < result_vector.size(); i++) {
        likely_key.push_back(result_vector[i].key);
    }

    std::vector<uint8_t> decrypted_bytes = cp::repeating_key_xor(cipher_bytes, likely_key);

    std::tuple<int, std::vector<uint8_t>, std::vector<uint8_t>> results(likely_keysize, likely_key, decrypted_bytes);

    //return likely_keysize likely_key decrypted_bytes
    return results;

}

int cp::get_hamming_distance(std::vector<uint8_t> &first, std::vector<uint8_t> &second) {

    if (first.size() != second.size()) {
        std::cout << "Cannot find hamming distance. Strings are different lengths" << std::endl;
        return -1;
    }

    int score = 0;
    for (size_t i = 0; i < first.size(); i++) {
        uint8_t xored_byte = first[i] ^ second[i];
        while (xored_byte > 0) {
            score += xored_byte & 1;
            xored_byte >>= 1;
        }
    }

    return score;
}
