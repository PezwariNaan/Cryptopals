#include "utility.hpp"
#include <cstdint>
#include <random>
#include "openssl.hpp"
#include "encoding.hpp"

struct cipher {
    BYTES ciphertext;
    BYTES iv;
};

class Hackable {
    private:
        BYTES _key;
        BYTES _iv;
        std::vector<std::vector<uint8_t>> texts = read_lines("Texts/challenge17.txt");

        BYTES generate_random_bytes(void) {
            BYTES result;
            std::random_device rd;
            while (result.size() < 16)
                result.push_back((rd() % 70)+ 32);
            return result;
        }

    public:
        Hackable() {
            _iv = generate_random_bytes();
            _key = generate_random_bytes();
        }

        cipher encrypt_string(void) {
            std::random_device rd;
            cipher response;
            EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();\
            int blocksize = 16;

            BYTES random_text = texts[rd() % texts.size()];
            std::string random_text_str(random_text.begin(), random_text.end());
            size_t len = random_text_str.length();

            uint8_t *random_text_decoded = cp::base64_decode(random_text_str, len, false);
            BYTES random_text_vec(random_text_decoded, random_text_decoded + len);
            pkcs7_padding(random_text_vec);
            response.ciphertext = openssl::encrypt_cbc(ctx, blocksize, random_text_vec, _key, _iv);
            response.iv = _iv;

            return response;
        }

		bool decrypt_string(cipher input) {
			int blocksize = 16;
			EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
			BYTES plaintext = openssl::decrypt_cbc(ctx, blocksize, input.ciphertext, _key, input.iv);
			is_valid_pkcs7(plaintext);

			return true;
		}
};

BYTES padding_oracle_attack(const cipher response, Hackable server) {
    int blocksize  = 16; 
    cipher modified = response;
    BYTES ciphertext = response.ciphertext;
    BYTES plaintext;

    // Start with the first block 
    for (size_t i = 0; i < ciphertext.size(); i += 16) {
        BYTES block(ciphertext.begin() + i, ciphertext.begin() + blocksize + i);
        BYTES previous_block = (i == 0)
            ? response.iv
            : BYTES(ciphertext.begin() + i - blocksize, ciphertext.begin() + i);

        modified.ciphertext = block;
        modified.iv = previous_block;
        
        BYTES block_plaintext(16, 0);
        BYTES intermediate(16, 0); 

        for (int pos = 15; pos >= 0; pos--) {
            int pad_value = blocksize - pos;
            
            for (int i = 15; i > pos; i--) {
                modified.iv[i] = intermediate[i] ^ pad_value;
            }
            
            for (int guess = 0; guess < 256; guess++) {
                modified.iv[pos] = guess;

                try {
                    server.decrypt_string(modified);
                    intermediate[pos] = guess ^ pad_value;
                    block_plaintext[pos] = intermediate[pos] ^ previous_block[pos];
                    break;
                } catch (...) {}
            }
        }

        plaintext.insert(plaintext.end(), block_plaintext.begin(), block_plaintext.end());
    }
    print_array(plaintext);
    std::cout << std::endl;

    return plaintext;
}

int main(void) {
    Hackable server;
    cipher response = server.encrypt_string();
    
    padding_oracle_attack(response, server);

    return 0;
}
