#include "../Deps/openssl.hpp"
#include "utility.hpp"
#include <cstdint>
#include <exception>

#define MAP std::map<std::string, std::vector<uint8_t>>
#define CALLBACK std::bind(&Profile::create, &hacker_profile, std::placeholders::_1)

class invalid_cookie_error : public std::exception {
    public:
        invalid_cookie_error(const std::string &message) : message_(message) {}
        const char* what() const throw() {return message_.c_str();}

    private:
        std::string message_;
};

class invalid_email_error : public std::exception {
	public:
		invalid_email_error(const std::string &message) : message_(message) {}
		const char* what() const throw() {return message_.c_str();}

	private:
		std::string message_;
};

class decryption_error : public std::exception {
	public:
		decryption_error(const std::string &message) : message_(message)	 {}
		const char* what() const throw() {return message_.c_str();}
	
	private:
		std::string message_;
};

class Profile {
	public:
		std::vector<uint8_t> email;
		std::vector<uint8_t> role;

		Profile() {
			init_profile();
		}

		std::map<std::string, std::vector<uint8_t>> create(std::string cookie) {
			parse_cookie(cookie);
			std::vector<uint8_t> temp_email = email;
			std::vector<uint8_t> temp_role = role;

			for (size_t i = 0; i < email.size(); i++) {
				if (email[i] == '&' || email[i] == '=')
					throw invalid_email_error("Invalid Character in Email");
			}

			for (size_t i = 0; i < role.size(); i++) {
				if (role[i] == '&' || role[i] == '=')
					throw invalid_email_error("Invalid Character In role");
			}
			
			static int id = 1;
			
			id++;
			
			temp_email = openssl::encrypt_ecb(ctx, email, &key);
			temp_role = openssl::encrypt_ecb(ctx, role, &key);
			id = id;
			std::map<std::string, std::vector<uint8_t>> profile_details{{"Email", temp_email}, {"Role", temp_role}};

			return profile_details;
		}
	
	private:
		EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
		inline static std::vector<uint8_t> key;
		int id;

		void init_profile() {
			srand(time(0));

			// Generate Random Key 
			const char lookup_table[] = "abcdefghijlkmnopABCDEFGHIJKLMNOP";
			char key_size = 16;
			char index = 0;

			while (index < key_size) {
				uint8_t random_index = rand() % (sizeof(lookup_table) - 1);
				key.push_back(lookup_table[random_index]);
				index++;
			}

			return;
		}

		void parse_cookie(std::string cookie) {
			// Cookie Structure: email=foo&role=bar
			// Skip '&' & '='
			email.clear();
			role.clear();
			size_t index = 0;
			bool passed_equals = false;
			bool got_email = false;

			if (cookie.length() > 1 ) {
				while (index < cookie.length()) {
					if (cookie[index] == '&') {
						passed_equals = false;
						got_email = true;
						index++;
					}

					if(passed_equals && !got_email) {
						email.push_back(cookie[index]);
					}
					
					if (passed_equals && got_email) {
						role.push_back(cookie[index]);
					}

					if (cookie[index] == '=') passed_equals = true;
					index++;
				}

			} else {
				throw invalid_cookie_error("No Cookie Provided");
			}
			
			return;
		}
};

void print_user_details(MAP user_details) {
	auto iter = user_details.begin();

	while (iter != user_details.end()) {
		std::cout << iter->first << '\n';
		print_array(iter->second);
		std::cout << '\n';
		iter++;
	}

	return;
}
// Code Breaking Part (Finally Lol!)
// Modify this function to get the blocksize that encrypt_cookie uses (16/32/64 etc...) 
// This function should be made universal and added to attack.cpp, then re-compiled to attack.o 
// and added to libcryptopals.a xoxo

int get_blocksize(const std::string plaintext, std::function<MAP(std::string)> encryption_function) {
    static int padding = 1;
	MAP base_object = encryption_function(plaintext);
	int base_length = base_object.begin()->second.size();

	while (true) {
		std::string padded_plaintext = plaintext;
		padded_plaintext.insert(6, padding, '0');
		MAP ciphertext_object = encryption_function(padded_plaintext);
		int current_length = ciphertext_object.begin()->second.size();
		if (current_length != base_length){
			return current_length - base_length;
		}

		padding++;
	}

}

std::string ecb_cbc_oracle(std::vector<uint8_t> ciphertext, int blocksize) {
    // Detect ECB
    std::vector<std::vector<uint8_t>> blocks = create_blocks(ciphertext, blocksize);
    for (size_t i = 0; i < blocks.size(); i++) {
        for (size_t j = i + 1; j < blocks.size(); j++) {
            if (blocks[i] == blocks[j]) {
                return "ECB";
            }
        }
    }
    // Else CBC
    return "CBC";
}

std::vector<uint8_t> attack_ecb(std::vector<uint8_t> ciphertext, int blocksize, std::function<MAP(std::string)> encryption_function) {
	int ciphertext_length = ciphertext.size();
	int total_blocks = ciphertext_length / blocksize; // Ciphertext must be some multiple of *blocksize*
	int block_index = 0; 
	std::vector<uint8_t> decrypted_bytes = {};

for (;block_index < total_blocks; block_index++ ) {

		for (int byte_index = 0; byte_index < blocksize; byte_index++) {
			int padding = blocksize - ((decrypted_bytes.size() + 1) % blocksize);
			bool match = false;

			for (int i = 0; i < 255; i++) {

				// Create a block of blocksize * character we want to test
				std::string test_block(blocksize, i);
				MAP encrypted_testblock_object = encryption_function(test_block);

				// Pad original ciphertext & testblock
				std::vector<uint8_t> padded_ciphertext = ciphertext;
				std::vector<uint8_t> padded_testblock = encrypted_testblock_object.begin() ->second;
				padded_ciphertext.insert(padded_ciphertext.begin(), padding, '0');
				padded_testblock.insert(padded_testblock.begin(), padding, '0');
				
				// Encrypt them again
				std::string padded_ciphertext_str(padded_ciphertext.begin(), padded_ciphertext.end());
				std::string padded_testblock_str(padded_testblock.begin(), padded_testblock.end());

				MAP encrypted_padded_ciphertext = encryption_function(padded_ciphertext_str);
				MAP encrypted_padded_testblock = encryption_function(padded_testblock_str);

				// If they match we know which character has been encrypted
				if (encrypted_padded_ciphertext.begin()->second == encrypted_padded_testblock.begin()->second) {
					decrypted_bytes.push_back(i);
					match = true;
					break;
				}
			}
			if (!match) {
				throw decryption_error("Could Not Match Byte");
			}
		}
	}

	return decrypted_bytes;
}

int main(void) {
    try {
		std::string hacker_cookie = "email=hacker@hacker.hacked&role=user";
		int blocksize = 0;

		// A while loop is not necessary; however better simulates the client, server relationship 
		// Each call is a new profile
		while (blocksize == 0) {
			Profile my_profile;
			blocksize = get_blocksize(hacker_cookie, std::bind(&Profile::create, &my_profile, std::placeholders::_1));
		}

		// Padd the plaintext with 3 * blocksize '0's to ensure that we have repeating blocks if it is ECB
		std::string padded_hacker_cookie = "email=&hacker@hacker.hackedrole=user";
		padded_hacker_cookie.insert(6, blocksize * 3, '0');
		Profile hacker_profile;
		MAP hacker_details = hacker_profile.create(padded_hacker_cookie);
		std::vector<uint8_t> hacker_ciphertext = hacker_details.begin()->second;
		std::string mode = ecb_cbc_oracle(hacker_details.begin()->second, blocksize);
		
		if (mode == "ECB") {
			std::vector<uint8_t> decrypted_bytes = attack_ecb(hacker_ciphertext, blocksize, CALLBACK);
			print_array(decrypted_bytes);
			std::cout << '\n';
			
			return 0;

		} else {
			std::cerr << "Not ECB, Can't Attack\n";
			return 1;
		}
		
    } catch (const invalid_cookie_error &e) {
        std::cerr << "Invalid Cookie Error: " << e.what() << '\n';
		return 2;
    } catch (const invalid_email_error &e) {
		std::cerr << "Email Error: " << e.what() << '\n';
		return 3;
		} catch (const decryption_error &e) {
		std::cerr << "Decryption Error: " << e.what() << '\n';
		return 4;
	} catch (const std::exception &e) {
        std::cerr << "General Exception: " << e.what() << '\n';
		return 5;
	}

}
