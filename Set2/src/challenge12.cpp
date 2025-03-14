#include "utility.hpp"
#include "openssl.hpp"
#include "encoding.hpp"

class GetHacked {
    private:
        BYTES key;
        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        std::string encoded_text = read_file("Texts/challenge12.txt");

    public:
        void generate_key(void) {
            srand(std::time(0));
            for (int i = 0; i < 16; i++) {
                // char random_char = (random() % 90) + 33; // Always generate an ASCII character
                // key.push_back(random_char);
                key.push_back('A');
            }
        }

        BYTES challenge12_oracle(std::string my_string) {
            size_t length = encoded_text.length();
            uint8_t *decoded_bytes_ptr = cp::base64_decode(encoded_text, length, false);

            BYTES unknown_bytes(decoded_bytes_ptr, decoded_bytes_ptr + length); // This works because the length variable is updated with length of the byte array by the decode function 
            BYTES my_bytes(my_string.begin(), my_string.end());

            BYTES total_bytes;
            total_bytes.insert(total_bytes.begin(), my_bytes.begin(), my_bytes.end());
            total_bytes.insert(total_bytes.end(), unknown_bytes.begin(), unknown_bytes.end());

            BYTES ciphertext = openssl::encrypt_ecb(ctx, total_bytes, key);
            return ciphertext;
        }
};

size_t get_blocksize(GetHacked hackable) {
    std::string tester;
    BYTES ciphertext = hackable.challenge12_oracle(tester);
    size_t initial_length = ciphertext.size();

    for (int i = 1; i < 2048; i++) {
        tester += i;
        BYTES new_ciphertext = hackable.challenge12_oracle(tester);
        size_t new_length = new_ciphertext.size();

        if (new_length != initial_length) {
            return new_length - initial_length;
        }
    }

    return -1;
}

bool detect_ecb(size_t blocksize, GetHacked hackable) {
    std::string tester;
    tester.append(blocksize * 4, 'a');
    BYTES ciphertext = hackable.challenge12_oracle(tester);
    BYTES first_block(ciphertext.begin(), ciphertext.begin() + blocksize);
    BYTES second_block(ciphertext.begin() + blocksize, ciphertext.begin() + (blocksize * 2));

    if (first_block == second_block) {
        return true;
    } else {
        return false;
    }
}

BYTES attack_ecb(GetHacked server, size_t blocksize) {
    BYTES deciphered = {};
    BYTES hidden_text = server.challenge12_oracle("");
    // Build Dictionary 
    std::map<BYTES, char> ecb_dictionary;
    
    do {
        size_t padding_size = blocksize  - (deciphered.size() % blocksize) - 1;
        BYTES padding(padding_size, 'a');
        size_t block_index = deciphered.size() / blocksize;

        for (int i = 0; i < 256; i++) {
            std::string test_block(padding.begin(), padding.end()); 
            test_block.insert(test_block.end(), deciphered.begin(), deciphered.end());
            test_block.push_back(i);

            BYTES ciphertext = server.challenge12_oracle(test_block);
            BYTES ecb_dictionary_key(ciphertext.begin() + (blocksize * block_index),
                                ciphertext.begin() + (blocksize * (block_index + 1)));

            ecb_dictionary.insert({ecb_dictionary_key, i});
        }

        // Search for Dictionary Match
        std::string test_padding(padding_size, 'a');

        BYTES test_ciphertext = server.challenge12_oracle(test_padding);
        BYTES test_block(test_ciphertext.begin() + (blocksize * block_index), 
                        test_ciphertext.begin() + (blocksize * (block_index + 1)));

        auto iter = ecb_dictionary.find(test_block);

        if (iter != ecb_dictionary.end()) {
            deciphered.push_back(iter->second);
        } else {
            break;
        }

    } while (deciphered.size() < hidden_text.size());

    return deciphered;
}

int main() {
    // Instanciate Cipher
    GetHacked server;
    server.generate_key();
    
    size_t blocksize = get_blocksize(server);
    bool is_ecb = detect_ecb(blocksize, server); // In this instance it is True
    if (!is_ecb) {
        std::cerr << "No ECB Encypted Ciphertext Found" << std::endl;
        return -1;
    }

    BYTES answer_vec = attack_ecb(server, blocksize);
    std::string answer(answer_vec.begin(), answer_vec.end());
    std::cout << answer;
    
    return 0;
}
