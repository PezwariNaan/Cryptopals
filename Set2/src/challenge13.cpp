#include "../Deps/openssl.hpp"
#include "utility.hpp"

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

struct cookie {
		std::string email;
		std::string role;
};

class Profile {
	public:
		std::vector<uint8_t> email;
		std::vector<uint8_t> role;

		Profile() {
			init_profile();
		}

		void create(std::string cookie) {
			struct cookie parsed_cookie = parse_cookie(cookie);
			std::vector<uint8_t> email_vec(parsed_cookie.email.begin(), parsed_cookie.email.end() - 1);
			std::vector<uint8_t> role_vec(parsed_cookie.role.begin(), parsed_cookie.role.end() - 1);

			for (size_t i = 0; i < email_vec.size(); i++) {
				if (email_vec[i] == '&' || email_vec[i] == '=')
					throw invalid_email_error("Invalid Character in Email");
			}

			for (size_t i = 0; i < role_vec.size(); i++) {
				if (role_vec[i] == '&' || role_vec[i] == '=')
					throw invalid_email_error("Invalid Character In role");
			}
			
			static int id = 1;
			
			id++;
			
			email = openssl::encrypt_ecb(ctx, email_vec, &key);
			role = openssl::encrypt_ecb(ctx, role_vec, &key);
			id = id;
			
			return;
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

			print_array(key);
			std::cout << '\n';
			
			return;
		}

		cookie parse_cookie(std::string cookie) {
			// Cookie Structure: email=foo&role=bar
			// Skip '&' & '='
			struct cookie parsed_cookie;
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
						parsed_cookie.email += cookie[index];
					}
					
					if (passed_equals && got_email) {
						parsed_cookie.role += cookie[index];
					}

					if (cookie[index] == '=') passed_equals = true;
					index++;
				}
			} else {
				throw invalid_cookie_error("No Cookie Provided");
			}
			
			return parsed_cookie;
		}
};

// Modify this function to get the blocksize that encrypt_cookie uses (16/32/64 etc...) 
// This function should be made universal and added to attack.cpp, then re-compiled to attack.o 
// and added to libcryptopals.a xoxo

// int get_blocksize(const std::vector<uint8_t> plaintext) {
//     int blocksize = 0;
//     std::vector<uint8_t> my_string = {'0'}; // This is what we will use to pad the start of each text block we pass
// 	// to the encryption function
//     // std::vector<uint8_t> ciphertext = encrypt_plaintext(my_string, plaintext);
//     std::vector<uint8_t> padded_ciphertext;

//     do {
//         padded_ciphertext = profile_for(my_string, plaintext);
//         my_string.insert(my_string.end(), '0');
//     } while (ciphertext.size() == padded_ciphertext.size());
//     blocksize = padded_ciphertext.size() - ciphertext.size();
    
//     return blocksize;
// }

int main(void) {
    try {
		std::string cookie = "email=hello@gmail.com&role=admin";
		
		Profile my_profile;
		my_profile.create(cookie);
		print_array(my_profile.email);
		print_array(my_profile.role);

		
    } catch (const invalid_cookie_error& e) {
        std::cerr << "Invalid Cookie Error: " << e.what() << '\n';
    } catch (const invalid_email_error& e) {
		std::cerr << "Email Error: " << e.what() << '\n';
	} catch (const std::exception& e) {
        std::cerr << "General Exception: " << e.what() << '\n';
	}

    return 0;
}
