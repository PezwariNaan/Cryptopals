#include <cstddef>
#include <cstring>
#include <exception>
#include <iostream>
#include <map>

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
	std::string email;
	std::string role;
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
	for (size_t i = 0; i < email.length(); i++) {
		if (email[i] == '&' || email[i] == '=')
			throw invalid_email_error("Invalid Character in Role");
	}

	for (size_t i = 0; i < role.length(); i++) {
		if (role[i] == '&' || role[i] == '=')
			throw invalid_email_error("Invalid Character In Email");
	}
	
	static int id = 1;
	cookie my_cookie = {id, email, role};

	return my_cookie;
}

int main(void) {
    try {
        std::string to_parse = "foo=bar&zap=zazzle";
        std::map<std::string, std::string> parsed = parse_cookie(to_parse);
        auto my_iter = parsed.begin();
        while (my_iter != parsed.end()) {
            std::cout << "Key: " << my_iter->first << '\n';
            std::cout << "Value: " << my_iter->second << '\n';
            my_iter++;
        }

		cookie my_cookie = profile_for("hello@gmail.com", "admin");
		std::cout << my_cookie.email << '\n';
		std::cout << my_cookie.id << '\n';
		std::cout << my_cookie.role << '\n';

    } catch (const invalid_cookie_error& e) {
        std::cerr << "Invalid Cookie Error: " << e.what() << '\n';
    } catch (const invalid_email_error& e) {
		std::cerr << "Email Error: " << e.what() << '\n';
	} catch (const std::exception& e) {
        std::cerr << "General Exception: " << e.what() << '\n';
	}
    return 0;
}

