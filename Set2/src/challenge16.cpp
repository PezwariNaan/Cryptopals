#include "utility.hpp"
#include <exception>
#include <string>

class InputError : public std::exception {
    private:
        std::string msg;

    public:
        explicit InputError(const std::string &message) : msg(message) {}

        const char* what() const noexcept override {
            return msg.c_str();
        }
};

class GetHacked {
    private:
        BYTES key;
        void generate_key() {
            return;
        }

    public:
        GetHacked() {
            generate_key();
        }

        std::string create_cookie(std::string user_input) {
            std::string cookie;
            std::string prefix = "comment1=cooking%20MCs;userdata=";
            std::string postfix = ";comment2=%20like%20a%20pound%20of%20bacon";



            cookie.insert(cookie.begin(), prefix.begin(), prefix.end());
            cookie.insert(cookie.end(), user_input.begin(), user_input.end());
            cookie.insert(cookie.end(), postfix.begin(), postfix.end());

            return cookie;
        }
};

int main() {
    GetHacked server;
    std::string cookie = server.create_cookie("Hello World!");
    
    std::cout << cookie << std::endl;

    return 0;
}