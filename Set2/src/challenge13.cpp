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

class Profile {
	public:
		std::vector<uint8_t> email;
		std::vector<uint8_t> role;

		Profile() {
			init_profile();
		}

		void create(std::string cookie) {
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
			
			email = openssl::encrypt_ecb(ctx, temp_email, &key);
			role = openssl::encrypt_ecb(ctx, temp_role, &key);
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

			return;
		}

		void parse_cookie(std::string cookie) {
			// Cookie Structure: email=foo&role=bar
			// Skip '&' & '='
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
		std::string cookie2 = "email=hello2@gmail.com&role=user";
		
		Profile my_profile;
		my_profile.create(cookie);
		print_array(my_profile.email);
		std::cout<<'\n';
		print_array(my_profile.role);
		std::cout << '\n';

		Profile my_profile2;
		my_profile2.create(cookie2);
		print_array(my_profile2.email);
		std::cout<<'\n';
		print_array(my_profile2.role);
		
    } catch (const invalid_cookie_error& e) {
        std::cerr << "Invalid Cookie Error: " << e.what() << '\n';
    } catch (const invalid_email_error& e) {
		std::cerr << "Email Error: " << e.what() << '\n';
	} catch (const std::exception& e) {
        std::cerr << "General Exception: " << e.what() << '\n';
	}

    return 0;
}
