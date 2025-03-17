#include "utility.hpp"
#include <algorithm>
#include <exception>
#include <iterator>
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
            char forbidden[] = {';' ,'='};
            
            user_input.erase(std::remove_if(user_input.begin(), user_input.end(), [&](char c){
                return std::find(std::begin(forbidden), std::end(forbidden), c) != std::end(forbidden);
            }), user_input.end());
            
            cookie.append(prefix);
            cookie.append(user_input);
            cookie.append(postfix);

            return cookie;
        }
};

int main() {
    GetHacked server;
    std::string cookie = server.create_cookie("H==el==l;;o===;;;;;;;;;World!");
    
    std::cout << cookie << std::endl;

    return 0;
}
