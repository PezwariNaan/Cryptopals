#include "utility.hpp"
#include "openssl.hpp"
#include "encoding.hpp"

const std::string key_str = "YELLOW SUBMARINE";
BYTES key(key_str.begin(), key_str.end());

int main() {
    std::string encoded_text = read_file("Texts/challenge12.txt");
    size_t length = encoded_text.length();
    uint8_t *decoded_bytes_ptr = cp::base64_decode(encoded_text, length, false);
    BYTES decoded_bytes(decoded_bytes_ptr, decoded_bytes_ptr + length); // This works because the length variable is updated with length of the byte array by the decode function 
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    BYTES ciphertext = openssl::encrypt_ecb(ctx, decoded_bytes, &key);
    print_array(ciphertext);
    std::cout << '\n';

    return 0;
}