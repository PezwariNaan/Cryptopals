#include <iostream>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include "utility.hpp"
#include "encoding.hpp"

int main(void) {
    //EVP_CIPH_ECB_MODE
    std::vector<uint8_t> cipher_bytes = read_file_bytes("./Texts/challenge7.txt");
    std::vector<uint8_t> cipher_decoded = cp::base64_decode(cipher_bytes);

    print_array(cipher_decoded);
    std::cout << std::endl;
    return 0;
}
