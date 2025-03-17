#include "utility.hpp"
#include "openssl.hpp"
#include "encoding.hpp"
#include <cstdlib>
#include <openssl/asn1.h>

struct Info {
    size_t blocksize;
    size_t cipher_length;
    size_t prefix_block;
    size_t prefix_length;
    size_t padding_length;
};

class GetHacked {
    private:
        const BYTES key;
        const std::string prefix;
        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        std::string encoded_text = read_file("Texts/challenge12.txt");

        void seed() {
            static bool seeded = false;
            if (!seeded) {
                srand(std::time(0));
                seeded = true;
            }
            return;
        }

        const BYTES generate_key() {
            seed();
            BYTES _key;
            for (int i = 0; i < 16; i++) {
                char random_char = (random() % 90) + 33; // Always generate an ASCII character
                _key.push_back(random_char);
            }
            return _key;
        }

        const std::string generate_prefix(void) {
            seed();
            std::string _prefix;
            int prefix_length = random() % 80;
            for (int i = 0; i < prefix_length; i++) {
                char random_char = (random() % 90) + 33;
                _prefix.push_back(random_char);
            }
            return _prefix;
        }

    public:
        GetHacked() : key(generate_key()), prefix(generate_prefix()) {}

        BYTES challenge12_oracle(std::string my_string) {
            // Unknown string - load & decode
            size_t length = encoded_text.length();
            uint8_t *decoded_bytes_ptr = cp::base64_decode(encoded_text, length, false);

            // Convert string to std::vector<uint8_t>
            BYTES unknown_bytes(decoded_bytes_ptr, decoded_bytes_ptr + length); // This works because the length variable is updated with length of the byte array by the decode function 
            BYTES my_bytes(my_string.begin(), my_string.end());

            // Concatinate my_bytes & unknown bytes
            BYTES total_bytes;
            total_bytes.insert(total_bytes.begin(), prefix.begin(), prefix.end());
            total_bytes.insert(total_bytes.end(), my_bytes.begin(), my_bytes.end());
            total_bytes.insert(total_bytes.end(), unknown_bytes.begin(), unknown_bytes.end());

            // Encrypt
            BYTES ciphertext = openssl::encrypt_ecb(ctx, total_bytes, key);
            return ciphertext;
        }
};

// Increment what we pass to the block by 1 everytime. 
// When the total ciphertext length changes -> we have found the padding size
// When I add a character the block that it is contained in while change, as well as 
// Every other block after it. If I change the CHARACTER at that point, I will know 
// Which block it is in. Repeat that process until 2 blocks have changed and I will know 
// Be able to calculate long the prefix is 
Info get_block_info(GetHacked hackable) {
    Info info;

    std::string test_string = "";
    BYTES original_ciphertext = hackable.challenge12_oracle(test_string);

    test_string.push_back('A');
    BYTES manipulated_ciphertext = hackable.challenge12_oracle(test_string);

    int i = 0, j = 0;
    while (original_ciphertext[i] == manipulated_ciphertext[i]) {
        i++;
    }
    info.prefix_block = i; // Multiple of 'blocksize'
                           // This is the block where our input starts

    // This will get the block size
    for (; j < 2048; j++) {
        if (manipulated_ciphertext.size() == original_ciphertext.size()) {
            test_string.push_back('A');
            manipulated_ciphertext = hackable.challenge12_oracle(test_string);
        } else {
            break;
        }
    }

    info.blocksize = manipulated_ciphertext.size() - original_ciphertext.size();
    info.padding_length = j;

    // Once we have the blocksize size & the start of the block our input is in
    // We can add & compare characters

    size_t start = info.prefix_block + info.blocksize;
    std::string prefix_test_string = "A";
    std::string prefix_test_string2 = "B";
    int k = 1;

    BYTES prefix_test_ciphertext = hackable.challenge12_oracle(prefix_test_string);
    BYTES prefix_test_ciphertext2 = hackable.challenge12_oracle(prefix_test_string2);

    while (prefix_test_ciphertext[start] == prefix_test_ciphertext2[start]) {
        k++;        
        prefix_test_string.push_back('A');
        prefix_test_string2.push_back('B');
        prefix_test_ciphertext = hackable.challenge12_oracle(prefix_test_string);
        prefix_test_ciphertext2 = hackable.challenge12_oracle(prefix_test_string2);
    }

    info.prefix_length = info.prefix_block + (info.blocksize - (k - 1)); // k is the number of bytes it takes to edit THE NEXT BLOCK
                                                                         // Which is the number it takes to fill the previous block + 1
    info.cipher_length = original_ciphertext.size() - (info.padding_length  + info.prefix_length);

    std::cout << "Prefix Length: " << info.prefix_length << '\n';
    std::cout << "Text Length: " << info.cipher_length << '\n';
    std::cout << "Ciphertext Length " << original_ciphertext.size() << '\n';
    std::cout << "Padding Length: " << info.padding_length << "\n\n";

    if (info.blocksize == 0) { // If we never get the blocksize we're sad
        info.blocksize      = -1;
        info.cipher_length  = -1;
        info.prefix_block   = -1;
        info.padding_length = -1;
        info.prefix_length  = -1;
    }

    return info;
}

bool detect_ecb(size_t blocksize, GetHacked hackable, Info info) {
    std::string tester;
    size_t offset = info.prefix_block + blocksize;

    tester.append(info.prefix_length, 'a');
    tester.append(blocksize * 4, 'a');

    BYTES ciphertext = hackable.challenge12_oracle(tester);
    BYTES first_block(ciphertext.begin() + offset, ciphertext.begin() + blocksize + offset);
    BYTES second_block(ciphertext.begin() + blocksize + offset, ciphertext.begin() + (blocksize * 2) + offset);

    if (first_block == second_block) {
        return true;
    } else {
        return false;
    }
}

BYTES attack_ecb(GetHacked server, Info info) {
    BYTES deciphered = {};
    BYTES hidden_text = server.challenge12_oracle("");

    size_t dictionary_offset = info.prefix_block + info.blocksize;
    size_t prefix_offset = info.blocksize - (info.prefix_length - info.prefix_block);

    // Build Dictionary 
    std::map<BYTES, char> ecb_dictionary;
    
    do {
        size_t padding_size = (info.blocksize  - (deciphered.size() % info.blocksize) - 1) + prefix_offset;
        BYTES padding(padding_size, 'a');
        size_t block_index = deciphered.size() / info.blocksize;

        for (int i = 0; i < 256; i++) {
            std::string dict_block(padding.begin(), padding.end()); 
            dict_block.insert(dict_block.end(), deciphered.begin(), deciphered.end());
            dict_block.push_back(i);

            BYTES ciphertext = server.challenge12_oracle(dict_block);
            BYTES ecb_dictionary_key(ciphertext.begin() + (info.blocksize * block_index) + dictionary_offset,
                                ciphertext.begin() + (info.blocksize * (block_index + 1)) + dictionary_offset);

            ecb_dictionary.insert({ecb_dictionary_key, i});
        }

        // Search for Dictionary Match
        std::string test_padding(padding_size, 'a');

        BYTES test_ciphertext = server.challenge12_oracle(test_padding);
        BYTES test_block(test_ciphertext.begin() + (info.blocksize * block_index) + dictionary_offset, 
                        test_ciphertext.begin() + (info.blocksize * (block_index + 1)) + dictionary_offset);

        auto iter = ecb_dictionary.find(test_block);

        if (iter != ecb_dictionary.end()) {
            deciphered.push_back(iter->second);
        } else {
            break;
        }

    } while (deciphered.size() < info.cipher_length);

    return deciphered;
}

int main() {
    // Instanciate Cipher
    GetHacked server;

    Info info = get_block_info(server);

    bool is_ecb = detect_ecb(info.blocksize, server, info);
    if (!is_ecb) {
        std::cerr << "No ECB Encypted Ciphertext Found" << std::endl;
        return -1;
    }

    BYTES answer_vec = attack_ecb(server, info);
    std::string answer(answer_vec.begin(), answer_vec.end());
    std::cout << answer;

    return 0;
}
