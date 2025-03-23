#include "encrypting.hpp"
#include "utility.hpp"
#include "openssl.hpp"
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
            BYTES encrypted_cookie = openssl::encrypt_cbc(ctx, blocksize, cookie_vec,key, iv); 

            return encrypted_cookie;
        }

        bool check_admin(BYTES cookie) {
            bool is_admin = false;
            BYTES decrypted = openssl::decrypt_cbc(ctx, blocksize, cookie, key, iv);
            std::string decrypted_str(decrypted.begin(), decrypted.end());

            std::string substring = ";admin=true;";

            if (decrypted_str.find(substring) != std::string::npos) {
                is_admin = true;
            }
            std::cout << decrypted_str << std::endl;

            return is_admin;
        }

};

BYTES xor_input(std::string mask1_str, std::string mask2_str) {
    BYTES input;
    BYTES mask1(mask1_str.begin(), mask1_str.end());
    BYTES mask2(mask2_str.begin(), mask2_str.end());

    input = cp::fixed_xor(mask1, mask2);

    return input;
}

BYTES xor_cookie(BYTES cookie, BYTES mask) {
    int blocksize = 16;

    // XOR the 3rd block directly
    for (int i = 0; i < blocksize; ++i) {
        cookie[2 * blocksize + i] ^= mask[i];
    }

    return cookie;
}

int main() {
    GetHacked server;
    std::string mask1(16, 'a');
    std::string mask2 = "aaaaa;admin=true";

    BYTES vec_input = xor_input(mask1, mask2);
    std::string user_input(vec_input.begin(), vec_input.end());
    user_input.insert(user_input.begin(), 16, 'a');

    BYTES cookie = server.request(user_input);
    BYTES new_cookie = xor_cookie(cookie, BYTES(mask1.begin(), mask1.end()));

    bool check = server.check_admin(new_cookie);
    std::cout << check << std::endl;

    return 0;
}
