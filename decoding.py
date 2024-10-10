#!/usr/bin/env python3

import base64
import sys

def get_args():
    if len(sys.argv) != 3:
        raise Exception("Usage: [Input File] [Output File]")
    else:
        input = sys.argv[1]
        output = sys.argv[2]
        return (input, output)

def main():
    input_file, output_file = get_args()
    OUTPUT_DIR = "./Texts/"
    try:
        input= open(OUTPUT_DIR + input_file, 'rb')
        output = open(OUTPUT_DIR + output_file, 'wb')

        base64.decode(input, output)
    except Exception as e:
        print(e)
    return 0

if __name__ == "__main__":
    main()
