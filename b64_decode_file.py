#!/usr/bin/env python3

import base64
import sys

def get_args():
    if len(sys.argv) != 4:
        raise Exception("Usage: [Set] [Input File] [Output File]")
    else:
        cp_set = sys.argv[1]
        input = sys.argv[2]
        output = sys.argv[3]
        return (cp_set, input, output)

def main():
    cp_set, input_file, output_file = get_args()
    FILE_DIR = f"{cp_set}/src/Texts/"
    try:
        input= open(FILE_DIR + input_file, 'rb')
        output = open(FILE_DIR + output_file, 'wb')

        base64.decode(input, output)
    except Exception as e:
        print(e)
    return 0

if __name__ == "__main__":
    main()
