#include "encoding.hpp"
#include <iostream>

int main(void) {
	std::string start_text = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";
    std::string target_text = "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t";

	std::vector<uint8_t> bytes = cp::hex_decode(start_text);
	std::string end_text = cp::base64_encode(bytes);
    std::cout << "Target: " << target_text << "\n";
	std::cout << "Result: " << end_text << "\n";

    return 0;
}
