#include "utility.hpp"
#include "encrypting.hpp"

int test(std::vector<uint8_t> first, std::vector<uint8_t> second) {
    int hamming_distance = cp::get_hamming_distance(first, second);
    
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

int main(void) {
    std::string first  = "this is a test";
    std::string second = "wokka wokka!!!";
    std::vector<uint8_t> first_bytes(first.begin(), first.end());
    std::vector<uint8_t> second_bytes(second.begin(), second.end());

    if (test(first_bytes, second_bytes) < 0) return 1;

    const std::string filename = "./Texts/challenge6_decoded.txt";
    std::vector<uint8_t> cipher_bytes = read_file_bytes(filename);

    auto [likely_keysize, likely_key, decrypted_bytes] = cp::attack_repeating_key_xor(cipher_bytes);

    std::cout << "Keysize: " << likely_keysize << "\n";
    std::cout << "Key: " ;
    print_array(likely_key);
    std::cout << "\n";
    std::cout << "Decrypted Bytes: \n";
    print_array(decrypted_bytes);
    std::cout << std::endl; 

    return 0;
}
