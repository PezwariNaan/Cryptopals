#include "utility.hpp"
#include <chrono>
#include <thread>

// Generate Key
std::vector<uint8_t> generate_key(void) {
    std::vector<uint8_t> key;
    std::srand((unsigned) time(NULL));

    for (int i = 0; i < 16; i++) {
        uint8_t random_char = rand() % 128;
        key.push_back(random_char);
    }
    return key;
}

// Append + Prepend 5-10 bytes 
std::vector<uint8_t> padd_ciphertext(std::vector<uint8_t> ciphertext) {
    std::vector<uint8_t> padded_ciphertext;
    return padded_ciphertext;
}

// Encrypt Function With CBC
std::vector<uint8_t> encrypt_cbc(std::vector<uint8_t> key, std::vector<uint8_t> plaintext){
    std::vector<uint8_t> ciphertext;
    return ciphertext;
}

// Encrypt Fucntion With ECB
std::vector<uint8_t> encrypt_ecb(std::vector<uint8_t> key, std::vector<uint8_t> plaintext) {
    std::vector<uint8_t> ciphertext;
    return ciphertext;
}

int main(void) {
    std::vector<uint8_t> key1 = generate_key();
    print_array(key1);
    std::cout << "\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::vector<uint8_t> key2 = generate_key();
    print_array(key2);
    std::cout << "\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::vector<uint8_t> key3 = generate_key();
    print_array(key3);
    std::cout << "\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::vector<uint8_t> key4 = generate_key();
    print_array(key4);
    std::cout << "\n";

    return 0;
}