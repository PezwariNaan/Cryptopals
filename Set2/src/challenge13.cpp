#include "utility.hpp"
#include "openssl.hpp"

#define MAP std::map<std::string, std::string>
#define BYTES std::vector<uint8_t>
// Parse Cookie
// A=B&C=D
void get_words(MAP *user_profile, size_t start, const std::string &cookie) {
	if (start >= cookie.length()) {return;}

	size_t pair_delim = cookie.find('=', start);
	if (pair_delim == std::string::npos) {return;};

	size_t pair_end = cookie.find('&', pair_delim);

	std::string key = cookie.substr(start, pair_delim - start);
	std::string value = (pair_end == std::string::npos) ? 
							cookie.substr(pair_delim + 1) :
							cookie.substr(pair_delim + 1, pair_end - pair_delim - 1);

	(*user_profile)[key] = value;
	if (pair_end != std::string::npos)
		get_words(user_profile, pair_end + 1, cookie); // Recursion yay!
}

MAP parse_cookie(std::string cookie) {
	MAP user_profile;
	size_t start = 0;
	get_words(&user_profile, start, cookie);
	return user_profile;
}

MAP profile_for(std::string email) {
	std::string role = "user";
	std::string id = "17";
	MAP profile;
	profile["role"] = role;
	profile["id"] = id;
	profile["email"] = email;

	return profile;
}

std::string encode_profile(MAP profile) {
	std::string encoded_profile;
	encoded_profile += "email=";
	encoded_profile += profile["email"];
	encoded_profile += '&';

	encoded_profile += "id=";
	encoded_profile += profile["id"];
	encoded_profile += '&';

	encoded_profile += "role=";
	encoded_profile += profile["role"];

	return encoded_profile;
}

std::vector<uint8_t> encrypt_profile(std::string profile_str, BYTES key) {
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	std::vector<uint8_t> profile_vec(profile_str.begin(), profile_str.end());
	std::vector<uint8_t> ciphertext = openssl::encrypt_ecb(ctx, profile_vec, key);

	return ciphertext;
}

std::vector<uint8_t> decrypt_ecb (std::vector<uint8_t> ciphertext, const std::vector<uint8_t> *key) {
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    if (EVP_DecryptInit(ctx, EVP_aes_128_ecb(), key->data(), NULL) != 1)
        throw std::runtime_error("Error Initalising Decryption Engine.");

    std::vector<uint8_t> plaintext(ciphertext.size() + EVP_CIPHER_CTX_block_size(ctx));
    int plaintext_len = 0;
    int len = 0;

    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size()) != 1)
        throw std::runtime_error("Error With ECB Decryption."); 
    plaintext_len = len;

    if (EVP_DecryptFinal(ctx, plaintext.data() + plaintext_len, &len) != 1)
        throw std::runtime_error("Error With ECB Decryption (Final Step).");
    plaintext_len += len;
    
    plaintext.resize(plaintext_len);
    
    return plaintext;
}

void print_user(MAP user) {
	auto iter = user.begin();
	while (iter != user.end()) {
		std::cout << iter->first << " : " << iter->second << '\n';
		iter++;
	}

	return;
}

std::string pkcs_padding(const std::string &input, size_t blocksize) {
	size_t pad = blocksize - ((input.size()) % blocksize);
	return input + std::string(pad, static_cast<char>(pad));
}

// email=hack@gmail.com&id=17&role=user
// email=aaaaaaaaaadmin&id=17&role=user% 
std::vector<uint8_t> cut_and_paste(BYTES key) {
	std::string user2_str = "admin";
	user2_str = pkcs_padding(user2_str, 16);
	MAP user1 = profile_for("hack@gmail.com");
	MAP user2 = profile_for("AAAAAAAAAA"+user2_str);
	std::string profile1 = encode_profile(user1);
	std::string profile2 = encode_profile(user2);
	BYTES encrytped1 = encrypt_profile(profile1, key);
	BYTES encrytped2 = encrypt_profile(profile2, key);
	encrytped1.resize(32);
	encrytped1.insert(encrytped1.end(), encrytped2.begin() + 16, encrytped2.begin() + 32);
	
	return encrytped1;
}

int main(void) {
	std::string key_str = "YELLOW SUBMARINE";
	std::vector<uint8_t> const key(key_str.begin(), key_str.end());
	std::vector<uint8_t> ciphertext = cut_and_paste(key);

	// MAP profile = profile_for("hello@gmail.com");
	// std::string encoded = encode_profile(profile);
	// std::vector<uint8_t> encoded_vec(encoded.begin(), encoded.end());
	// std::vector<uint8_t> ciphertext = openssl::encrypt_ecb(ctx, encoded_vec, &key);
	// print_array(ciphertext);
	// std::cout << '\n';
	std::vector plaintext = decrypt_ecb(ciphertext, &key);
	print_array(plaintext);
	std::cout << '\n';
	return 0;
}
