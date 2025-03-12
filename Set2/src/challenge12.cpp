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

int main() {
    // Instanciate Cipher
    GetHacked server;
    server.generate_key();
    
    size_t blocksize = get_blocksize(server);
    bool is_ecb = detect_ecb(blocksize, server); // In this instance it is True
    if (!is_ecb) {
        std::cerr << "No ECB encypted ciphertext found" << std::endl;
        return -1;
    }

    // Build Dictionary 

    std::map<BYTES, char> ecb_dictionary;
    for (size_t n = 1; n < 2; n++) {
        std::string padding(blocksize - 1, 'a');

        for (int i = 0; i < 256; i++) {
    
            std::string test_block = padding; 
            test_block.insert(test_block.end(), n, i);

            BYTES ciphertext = server.challenge12_oracle(test_block);
            BYTES ecb_dictionary_key(ciphertext.begin(), ciphertext.begin() + blocksize);
    
            ecb_dictionary.insert({ecb_dictionary_key, i});
        }
    }

    // Test Dictionary
    std::string test_padding(15, 'a');

    BYTES test_ciphertext = server.challenge12_oracle(test_padding);
    BYTES test_block(test_ciphertext.begin(), test_ciphertext.begin() + blocksize);

    auto iter = ecb_dictionary.find(test_block);
    if (iter != ecb_dictionary.end()) {
        std::cout << "First Character: " << iter->second << '\n';
        return 0;
    }
        

    // auto iter = ecb_dictionary.begin();
    // while (iter != ecb_dictionary.end()) {
    //     std::cout << iter->second << " :::: ";
    //     std::cout << cp::hex_encode((iter->first)) << '\n';
    //     iter++;
    // }

    return 0;
}
