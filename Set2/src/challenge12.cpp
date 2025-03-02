#include "utility.hpp"
#include "openssl.hpp"
#include "encoding.hpp"

BYTES key;

void generate_key(void) {
    srand(std::time(0));
    for (int i = 0; i < 16; i++) {
        char random_char = random() % 127;
        key.push_back(random_char);
    }
}

BYTES challenge12_oracle(std::string my_string, BYTES &key) {
    std::string encoded_text = read_file("Texts/challenge12.txt");
    size_t length = encoded_text.length();
    uint8_t *decoded_bytes_ptr = cp::base64_decode(encoded_text, length, false);
    
    BYTES unknown_bytes(decoded_bytes_ptr, decoded_bytes_ptr + length); // This works because the length variable is updated with length of the byte array by the decode function 
    BYTES my_bytes(my_string.begin(), my_string.end());

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    BYTES total_bytes;
    total_bytes.insert(total_bytes.begin(), my_bytes.begin(), my_bytes.end());
    total_bytes.insert(total_bytes.end(), unknown_bytes.begin(), unknown_bytes.end());

    BYTES ciphertext = openssl::encrypt_ecb(ctx, total_bytes, &key);
    return ciphertext;
}

int main() {
    generate_key();
    std::string my_string = "Hello Worldsdsdfsdfsdfdssdfsdfsdfsdsdsdsdsdfsddf123123 :)";
    BYTES ciphertext = challenge12_oracle(my_string, key);

    std::cout << ciphertext.size() << std::endl;
    return 0;
}
