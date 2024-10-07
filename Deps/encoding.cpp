#include "encoding.hpp"

std::string cp::base64_encode(const std::vector<uint8_t> &input) {
	const std::string b64_lookup_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	std::string b64_string;
	int input_len = input.size();
	if (input_len <= 0) return "No Input";
	
	// Calculate length of base64 string and reserve memory
	int b64_length = std::floor((input_len + 2) / 3) * 4;
	b64_string.reserve(b64_length);

	// main converstion logic
	for (int i = 0; i < input_len; i += 3) {
		uint8_t byte1 = input[i];
		uint8_t byte2 = (i + 1 < input_len) ? input[i + 1] : 0;
		uint8_t byte3 = (i + 2 < input_len) ? input[i + 2] : 0;
		
		// Get first 6 bit
		b64_string.push_back(b64_lookup_table[byte1 >> 2]);
		// Append previous 2 bytes to next 4
		// Use bitwise AND to get relevant bit§.
		// Use bitwise OR to create new 6 bit value
		b64_string.push_back(b64_lookup_table[((byte1 & 0x03) << 4) | (byte2 >> 4)]);
		// Repeat previous step
		b64_string.push_back(b64_lookup_table[((byte2 & 0x0F) << 2) | (byte3 >> 6)]);
		// get last 6 bits with bitwise AND
		b64_string.push_back(b64_lookup_table[byte3 & 0x3F]);
	}

	int padding = input_len % 3;
	if (padding > 0) {
		b64_string[b64_length - 1] = '=';
		if (padding == 1) 
			b64_string[b64_length - 2] = '=';
	}
	
	return b64_string;
}

std::vector<uint8_t> cp::base64_decode(const std::string &input) {
	const std::string b64_lookup_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	std::vector<uint8_t> byte_array;
	int input_len = input.length();
	
	for (int i = 0; i < input_len; i += 4) {
		// Handle padding '='
		int first_char_index = b64_lookup_table.find(input[i]);
		int second_char_index = b64_lookup_table.find(input[i + 1]);
		int third_char_index = (input[i + 2] == '=') ? 0 : b64_lookup_table.find(input[i + 2]);
		int fourth_char_index = (input[i + 3] == '=') ? 0 : b64_lookup_table.find(input[i + 3]);
		
		// Decode
		uint8_t byte1 = (first_char_index << 2) | (second_char_index >> 4);
		byte_array.push_back(byte1);
		
		if (input[i + 2] != '=') {
			uint8_t byte2 = ((second_char_index & 0x0F) << 4) | (third_char_index >> 2);
			byte_array.push_back(byte2);
		}
		
		if (input[i + 3] != '=') {
			uint8_t byte3 = ((third_char_index & 0x03) << 6) | fourth_char_index;
			byte_array.push_back(byte3);
		}
	}
	
	return byte_array;
}

std::vector<uint8_t> cp::hex_decode(const std::string &input) {
	std::vector<uint8_t> byte_array;
	for (size_t i = 0; i < input.length(); i+=2) {
		// 1 Byte = 2 hex characters; so that's what we get
		std::string byte_string = input.substr(i, 2);
		
		// Convert the 2 characters into one byte
		uint8_t byte_value = static_cast<uint8_t> (
					std::stoi(byte_string, nullptr, 16)
				);
		byte_array.push_back(byte_value);
	}
	return byte_array;
}

std::string cp::hex_encode(const std::vector<uint8_t> &input) {
	std::stringstream hex_string;
	hex_string << std::hex;

	for (size_t i = 0; i < input.size(); i++) {
		hex_string << std::setw(2) << std::setfill('0') << static_cast<int>(input[i]);
	}
	
	return hex_string.str();
}
