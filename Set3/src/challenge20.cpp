#include <cctype>
#include <cstddef>
#include <cstdint>
#include <utility.hpp>
#include <openssl.hpp>
#include <encoding.hpp>

BYTES generate_key(void) {
    BYTES key;
    std::random_device rd;
    for (int i = 0; i < 16; i++) {
        key.push_back((rd() % 90) + 33); // Between 33 & 126
    }
    return key;
}

BYTES concat_texts(unsigned int len, std::vector<BYTES> ciphertexts) {
    BYTES concated_texts; 

    for (auto i : ciphertexts) {
        for (size_t j = 0; j < len; j++) {
            concated_texts.push_back(i[j]);
        }
    }
    return concated_texts;
}

BYTES get_keystream(unsigned int len, BYTES &concated_text) {
    BYTES keystream;

    for (unsigned int i = 0; i < len; i++) {
        int best_score = INT_MIN;
        uint8_t best_k = 0;

        for (int k = 0; k < 256; k++) {
            int total_score = 0;

            for (size_t base = 0; base + i < concated_text.size(); base += len) {
                uint8_t pt_guess = concated_text[base + i] ^ (uint8_t)k;

                if (isalpha(pt_guess)) total_score += 2;
                else if (pt_guess == ' ') total_score += 1;
                else if (isprint(pt_guess)) total_score += 0;
                else total_score -= 1;
            }

            if (total_score > best_score) {
                best_score = total_score;
                best_k = k;
            }
        }

        keystream.push_back(best_k);
    }
    return keystream;
}

int main(void) {

    std::vector<BYTES> lines = read_lines("Texts/challenge20.txt");
    std::vector<BYTES> encrypted_lines;
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    long nonce = 0;
    BYTES key = generate_key();
    size_t blocksize = 16;

    for (size_t i = 0; i < lines.size(); i++) {
        std::string line_str(lines[i].begin(), lines[i].end());
        size_t len = line_str.length();
        uint8_t *decoded_ptr = cp::base64_decode(line_str, len, false);
        BYTES decoded(decoded_ptr, decoded_ptr + len);
        BYTES encrypted = openssl::aes_ctr(ctx, decoded, key, nonce, blocksize);
        encrypted_lines.push_back(encrypted);
    }

    size_t shortest_len = INT_MAX;
    for (auto i : encrypted_lines) {
        shortest_len = i.size() < shortest_len ? i.size() : shortest_len;
    }

    // Truncate and concatinate ciphertexts
    BYTES concated_text = concat_texts(shortest_len, encrypted_lines);
    
    BYTES keystream = get_keystream(shortest_len, concated_text);

    for (auto i : encrypted_lines) {
        for (size_t j = 0; j < shortest_len; j++) {
            std::cout << static_cast<char>((i[j] ^ keystream[j]));

        }
        std::cout << '\n';
    }

    return 0;
}
