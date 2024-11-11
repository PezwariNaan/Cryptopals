#include "openssl.hpp"
#include "utility.hpp"
#include "encoding.hpp"
// --------------------
#include <cstdint>
#include <openssl/evp.h>
#include <sys/types.h>
#include <vector>

int main(int argc, char *argv[]) {
    std::string filename = "Texts/challenge12.txt";
    std::vector<uint8_t> plaintext = cp::py_base64decode(filename, *argv);
    std::vector<uint8_t> padded_plaintext = plaintext; // Variable that plaintext will be copied into before padding
    std::vector<uint8_t> key = {'A'};
    std::vector<uint8_t> padding;
    size_t blocksize = 0;
    size_t unpadded_ciphertext_size = 0;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    std::vector<uint8_t> ciphertext = openssl::encrypt_ecb(ctx, plaintext, key); // What we are attempting to decrypt
    
    // TODO:

    //      Pad & Encrypt plaintext 
    std::vector<uint8_t> padded_ciphertext = openssl::encrypt_ecb(ctx, padded_plaintext, key);

    int padding_size = 0;
    do {
        padding_size++;
        padding.assign(padding_size, 'B');
        padded_plaintext = plaintext;
        padded_plaintext.insert(padded_plaintext.begin(), padding.begin(), padding.end());
        padded_ciphertext = openssl::encrypt_ecb(ctx, padded_plaintext, key);
    } while (ciphertext.size() == padded_ciphertext.size());

    blocksize = padded_ciphertext.size() - ciphertext.size();
    unpadded_ciphertext_size = ciphertext.size() - (blocksize - padding_size);
    int existing_padding = blocksize - padding_size - 1;

    //      Detect ECB mode
    //      Detect block size
    //      Detect unpadded ciphertext size (ciphertext.size() - (padding.size() + blocksize))
    //      Pad ciphertext blocksize - 1
    //      Test final byte == ciphertext guess 
    //      Repeat for next byte 
    //      Print result

    EVP_CIPHER_CTX_free(ctx);
    return 0;
}
