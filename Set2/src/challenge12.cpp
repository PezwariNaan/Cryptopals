#include "utility.hpp"
#include "openssl.hpp"
#include "encoding.hpp"

BYTES key;

void generate_key(void) {
    srand(std::time(0));
    for (int i = 0; i < 16; i++) {
        char random_char = (random() % 90) + 33;
        key.push_back(random_char);
    }
}

BYTES challenge12_oracle(std::string my_string) {
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

size_t get_blocksize(void) {
    std::string tester;
    BYTES ciphertext = challenge12_oracle(tester);
    size_t initial_length = ciphertext.size();

    for (int i = 1; i < 2048; i++) {
        tester += i;
        BYTES new_ciphertext = challenge12_oracle(tester);
        size_t new_length = new_ciphertext.size();

        if (new_length != initial_length) {
            return new_length - initial_length;
        }
    }

    return -1;
}

bool detect_ecb() {
    size_t blocksize = get_blocksize();
    std::string tester;
    tester.append(blocksize * 4, 'a');
    BYTES ciphertext = challenge12_oracle(tester);
    BYTES first_block(ciphertext.begin(), ciphertext.begin() + blocksize);
    BYTES second_block(ciphertext.begin() + blocksize, ciphertext.begin() + (blocksize * 2));

    if (first_block == second_block) {
        return true;
    } else {
        return false;
    }
}

int main() {
    generate_key();
    std::string my_string = "Hello World!\n";
    BYTES ciphertext = challenge12_oracle(my_string);
    bool is_ecb = detect_ecb();
    return 0;
}
