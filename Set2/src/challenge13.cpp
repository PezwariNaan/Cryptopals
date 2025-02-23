#include "../Deps/openssl.hpp"
#include "utility.hpp"

#define MAP std::map<std::string, std::vector<uint8_t>>

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

int main(void) {
    try {
		std::string hacker_cookie = "email=hacker@hacker.hacked&role=user";

		int blocksize = 0;
		while (blocksize == 0) {
			Profile my_profile;
			blocksize = get_blocksize(hacker_cookie, std::bind(&Profile::create, &my_profile, std::placeholders::_1));
		}

		std::cout << "Blocksize: " << blocksize << '\n';
		return 0;
		
    } catch (const invalid_cookie_error& e) {
        std::cerr << "Invalid Cookie Error: " << e.what() << '\n';
		return 1;
    } catch (const invalid_email_error& e) {
		std::cerr << "Email Error: " << e.what() << '\n';
		return 2;
	} catch (const std::exception& e) {
        std::cerr << "General Exception: " << e.what() << '\n';
		return 3;
	}
}
