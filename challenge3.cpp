#include "encoding.hpp"
#include "encrypting.hpp"
#include "utility.hpp"

int main(void) {
    std::string cipher_text = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";
    std::vector cipher_bytes = cp::hex_decode(cipher_text);
    result results = attack_single_byte_xor(cipher_bytes);

    std::cout << "Key: " << results.key << "\n\nPlain Text:\n";
    print_array(results.decrypted_bytes);
    std::cout << std::endl;

    return 0;
}
