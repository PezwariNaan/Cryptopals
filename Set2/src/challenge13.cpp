#include "../Deps/openssl.hpp"
#include "utility.hpp"

#define STACK_SIZE      1024

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
	int id;
	std::vector<uint8_t> email;
	std::vector<uint8_t> role;
};

std::map<std::string,std::string> parse_cookie(std::string cookie) {
    std::map<std::string, std::string> user_dict;
    //Cookie Structure: a=b & c=d
    // If last symbol == '=' and current symbol == '&' error
    std::string current_key;
    std::string current_value;
    size_t index = 0;

    if (cookie.length() > 1) {
		do {
			while (cookie[index] != '=' && index < cookie.length()) {
				current_key += cookie[index];
				index++;
			}

			if (index < cookie.length() || cookie[index] == '=') {
				index++;
			}

			while (cookie[index] != '&' && index < cookie.length()) {
				current_value += cookie[index];
				index++;
			}
			
			if (index < cookie.length() && cookie[index] == '&') {
				index++;
			}

			user_dict[current_key] = current_value;
			current_key.clear();
			current_value.clear();

		} while (index < cookie.length());
    
    } else {
        throw invalid_cookie_error("No Cookie Provided");
    }

    return user_dict;
}

cookie profile_for(std::string email, std::string role) {
	std::vector<uint8_t> email_vec(email.begin(), email.end() - 1);
	std::vector<uint8_t> role_vec(role.begin(), role.end() - 1);

	for (size_t i = 0; i < email.size(); i++) {
		if (email[i] == '&' || email[i] == '=')
			throw invalid_email_error("Invalid Character in Role");
	}

	for (size_t i = 0; i < role.size(); i++) {
		if (role[i] == '&' || role[i] == '=')
			throw invalid_email_error("Invalid Character In Email");
	}
	
	static int id = 1;
	cookie my_cookie = {id, email_vec, role_vec};
	id++;
	return my_cookie;
}

void encrypt_cookie(cookie &plain_cookie, EVP_CIPHER_CTX *ctx, std::vector<u_int8_t> key) {
	plain_cookie.email = openssl::encrypt_ecb(ctx, plain_cookie.email, key);
	plain_cookie.role = openssl::encrypt_ecb(ctx, plain_cookie.role, key);
	return;
}

int main(void) {
    try {
		srand(time(0));
		EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

		const char lookup_table[] = "abcdefghijlkmnopABCDEFGHIJKLMNOP";
		char key_size = 16;
		char index = 0;

		std::vector<uint8_t> key;
		while (index < key_size) {
			uint8_t random_index = rand() % sizeof(lookup_table) / sizeof(char);
			key.push_back(lookup_table[random_index]);
			index++;
		}

		cookie my_cookie = profile_for("hello@gmail.com", "admin");
		//cookie my_cookie2 = profile_for("hello@gmail.com", "admin"); // Only here for testing
		//cookie my_cookie2 = profile_for("hello2@gmail.com", "user");

		encrypt_cookie(my_cookie, ctx, key);
		//encrypt_cookie(my_cookie2, ctx, key);

		print_array(my_cookie.email);
		std::cout << '\n';
		print_array(my_cookie.role);
		std::cout << '\n';


    } catch (const invalid_cookie_error& e) {
        std::cerr << "Invalid Cookie Error: " << e.what() << '\n';
    } catch (const invalid_email_error& e) {
		std::cerr << "Email Error: " << e.what() << '\n';
	} catch (const std::exception& e) {
        std::cerr << "General Exception: " << e.what() << '\n';
	}
    return 0;
}
