#include <iostream>
#include <openssl/aes.h>

int main(void) {
    const unsigned char *plain_text = (unsigned char *)"Hello World!";
    const unsigned char *key_text = (unsigned char *)"BananaBananaBanana";
    const int key_len = 192;
    unsigned char cipher_text[AES_BLOCK_SIZE];

    AES_KEY key;
    AES_set_encrypt_key(key_text, key_len, &key);
    AES_encrypt(plain_text, cipher_text, &key);
    printf("%d\n", key_len);

    printf("%s\n", cipher_text);
    printf("%s\n", plain_text);
    return 0;
}
