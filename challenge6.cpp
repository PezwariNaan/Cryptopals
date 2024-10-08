#include "utility.hpp"
#include "encoding.hpp"
#include "encrypting.hpp"

int get_hamming_distance(std::vector<uint8_t> &first, std::vector<uint8_t> &second) {

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

int test(std::vector<uint8_t> first, std::vector<uint8_t> second) {
    int hamming_distance = get_hamming_distance(first, second);
    
    switch(hamming_distance) {
        case 37:
            std::cout << "Hamming Distance Check Passed.\n";
            break;
        case -1:
            return -1;
            break;
        default:
            std::cout << "Incorrect Hamming Distance\n";
            std::cout << "Result: " << hamming_distance << "\n";
            std::cout << "Goal : " << 37 << std::endl;
            return -1;
    }

    return 0;
}

std::map<float, int> get_keysize(const int MAX_KEYSIZE, std::vector<uint8_t> cipher_bytes) {
    std::map<float, int> keysize_scores;
    int keysize = 2;
    // Get normalized edit distance between blocks of KEYSIZE length
    for (; keysize < MAX_KEYSIZE; keysize++) {
        float keysize_score = 0;
        std::vector<std::vector<uint8_t>> block_vector;

        // Gather blocks of size 'keysize'
        for (int i = 0; i < cipher_bytes.size() / keysize; i++) {  // Use the as many blocks as possible
            std::vector<uint8_t> block(cipher_bytes.begin() + i * keysize, 
                                    cipher_bytes.begin() + (i + 1) * keysize);
            
            block_vector.push_back(block);
        }

        // Calculate pairwise Hamming distances
        int comparisons = 0;
        for (size_t i = 0; i < block_vector.size(); i++) {
            for (size_t j = i + 1; j < block_vector.size(); j++) {
                int hamming_distance = get_hamming_distance(block_vector[i], block_vector[j]);
                keysize_score += hamming_distance / static_cast<float>(keysize);  // Normalize by keysize
                comparisons++;
            }
        }

        // Average the score over the number of comparisons
        keysize_score /= comparisons;

        // Store the score for this keysize
        keysize_scores.insert({keysize_score, keysize});
    }
        //std::cout << "Keysize: " << keysize << " Score: " << keysize_score << std::endl;
    return keysize_scores;
}

int main(void) {
    std::string first  = "this is a test";
    std::string second = "wokka wokka!!!";
    std::vector<uint8_t> first_bytes(first.begin(), first.end());
    std::vector<uint8_t> second_bytes(second.begin(), second.end());

    if (test(first_bytes, second_bytes) < 0) return 1;

    std::string filename = "./Texts/challenge6.txt";
    std::string cipher_text = read_file(filename);
    std::vector<uint8_t> cipher_bytes = cp::base64_decode(cipher_text);

    int keysize = 2;
    const int MAX_KEYSIZE = 40;
    std::map<float, int> keysize_scores = get_keysize(MAX_KEYSIZE, cipher_bytes); // score, keysize
    
    // Select keysize with lowest edit distance 
    int likely_keysize = keysize_scores.begin()->second;
    std::cout << "Keysize: " << likely_keysize << "\n";

    // Break cipher_bytes into keysize length blocks 
    std::map<int, std::vector<uint8_t>> keysize_blocks;
    int count = 1;
    
    // Transpose the blocks as they are populated 
    for (int j = 0; j < likely_keysize; j++) {
        std::vector<uint8_t> block;
        for (size_t i = j; i < cipher_bytes.size(); i += likely_keysize) {
            block.push_back(cipher_bytes[i]);
        }
        keysize_blocks.insert({count, block});
        count++;
    }

    auto element = keysize_blocks.begin();
    auto end = keysize_blocks.end();
    
    // Run attack_single_byte_xor on each transposed block 
    std::vector<result> result_vector;
    for (;element != end; element++) {
        result results = cp::attack_single_byte_xor(element->second);
        result_vector.push_back(results);
    }

    for (size_t i = 0; i < result_vector.size(); i++) {
        std::cout << result_vector[i].key;
    }

    std::cout << std::endl; 

    return 0;
}
