#include "utility.hpp"
#include "encoding.hpp"
#include "openssl.hpp"
#include <algorithm>
#include <openssl/evp.h>
#include <random>

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
        const BYTES key;
        const BYTES iv;
        static constexpr int blocksize = 16;
        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        
        void seed(void) {
            static bool seeded = false;
            if (!seeded) {
                std::random_device rd;
                srand(rd());
                seeded = true;
            }
            return;
        }

        BYTES generate_key(void) {
            seed();
            BYTES _key;
            for (int i = 0; i < blocksize; i++) {
                char random_char = rand() % 256;
                _key.push_back(random_char);
            }

            return _key;
        }

        BYTES generate_iv(void) {
            seed();
            BYTES _iv;
            for (int i = 0; i < blocksize; i++) {
                _iv.push_back(rand() % 256);
            }
            return _iv;
        }

        void pkcs7(std::string &input) {
            int blocksize = 16;
            int padding = blocksize - (input.size() % blocksize);
            input.append(padding, static_cast<char>(padding));

            return;
        }

    public:
        GetHacked() : key(generate_key()), iv(generate_iv()) {}

        BYTES request(std::string user_input) {
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
            pkcs7(cookie);

            BYTES cookie_vec(cookie.begin(), cookie.end());
            BYTES encrypted_cookie = openssl::encrypt_cbc(ctx, blocksize, cookie_vec,iv, key); 

            return encrypted_cookie;
        }

        bool check_admin(BYTES cookie) {
            bool is_admin = false;
            
            return is_admin;
        }


};

int main() {
    GetHacked server;
    std::string user_input(16, 'a');
    BYTES cookie = server.request(user_input);
    std::cout << cp::hex_encode(cookie) << std::endl;

    return 0;
}
