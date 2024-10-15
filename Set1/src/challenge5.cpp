#include "utility.hpp"
#include "encoding.hpp"
#include "encrypting.hpp"

int main(void) {
    std::string plain_text = "Burning 'em, if you ain't quick and nimble I go crazy when I hear a cymbal";
    std::string target = "0b3637272a2b2e63622c2e69692a23693a2a3c6324202d623d63343c2a26226324272765272a282b2f20430a652e2c652a3124333a653e2b2027630c692b20283165286326302e27282f";
    std::string key_text = "ICE";
    std::vector<uint8_t> key_bytes(key_text.begin(), key_text.end());
    std::vector<uint8_t> plain_bytes(plain_text.begin(), plain_text.end());

    std::vector<uint8_t> cipher_bytes = cp::repeating_key_xor(plain_bytes, key_bytes);
    std::string cipher_hex = cp::hex_encode(cipher_bytes);

    std::cout << "Hex: \n";
    std::cout << cipher_hex << "\n";
    std::cout << target << std::endl;

    return 0;
}
