#!/usr/bin/env python3

import base64

def main():
    input = open("/home/haxor/Code/Personal/C++/CryptoPals/Set1/src/Texts/challenge6.txt", 'rb')
    output = open("challenge6_decoded.txt", 'wb')
    base64.decode(input, output)
    return 0

if __name__ == "__main__":
    main()
