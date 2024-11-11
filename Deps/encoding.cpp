#include "encoding.hpp"
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <sys/types.h>

#define PY_SSIZE_T_CLEAN
#include <python3.12/Python.h>

std::string cp::base64_encode(const std::vector<uint8_t> &input) {
	const std::string b64_lookup_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	std::string b64_string;
	int input_len = input.size();
	if (input_len <= 0) return "No Input";
	
	// Calculate length of base64 string and reserve memory
	int b64_length = std::floor((input_len + 2) / 3) * 4;
	b64_string.reserve(b64_length);

	// main converstion logic
	for (int i = 0; i < input_len; i += 3) {
		uint8_t byte1 = input[i];
		uint8_t byte2 = (i + 1 < input_len) ? input[i + 1] : 0;
		uint8_t byte3 = (i + 2 < input_len) ? input[i + 2] : 0;
		
		// Get first 6 bit
		b64_string.push_back(b64_lookup_table[byte1 >> 2]);
		// Append previous 2 bytes to next 4
		// Use bitwise AND to get relevant bit§.
		// Use bitwise OR to create new 6 bit value
		b64_string.push_back(b64_lookup_table[((byte1 & 0x03) << 4) | (byte2 >> 4)]);
		// Repeat previous step
		b64_string.push_back(b64_lookup_table[((byte2 & 0x0F) << 2) | (byte3 >> 6)]);
		// get last 6 bits with bitwise AND
		b64_string.push_back(b64_lookup_table[byte3 & 0x3F]);
	}

	int padding = input_len % 3;
	if (padding > 0) {
		b64_string[b64_length - 1] = '=';
		if (padding == 1) 
			b64_string[b64_length - 2] = '=';
	}
	
	return b64_string;
}

std::vector<uint8_t> cp::base64_decode(const std::vector<uint8_t> &input) {
    const std::string b64_lookup_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::vector<uint8_t> byte_array;

    // Precompute a reverse lookup table for Base64 characters
    std::vector<int> lookup_table(256, -1);
    for (size_t i = 0; i < b64_lookup_table.size(); i++) {
        lookup_table[b64_lookup_table[i]] = i;
    }

    int input_len = input.size();
    
    // Process input in blocks of 4 Base64 characters
    for (int i = 0; i < input_len; i += 4) {
        // Check for padding and make sure we don't go out of bounds
        int first_char_index  = lookup_table[static_cast<uint8_t>(input[i])];
        int second_char_index = lookup_table[static_cast<uint8_t>(input[i + 1])];
        int third_char_index  = (i + 2 < input_len && input[i + 2] != '=') ? lookup_table[static_cast<uint8_t>(input[i + 2])] : 0;
        int fourth_char_index = (i + 3 < input_len && input[i + 3] != '=') ? lookup_table[static_cast<uint8_t>(input[i + 3])] : 0;

        // Decode
        uint8_t byte1 = (first_char_index << 2) | (second_char_index >> 4);
        byte_array.push_back(byte1);
        
        if (i + 2 < input_len && input[i + 2] != '=') {
            uint8_t byte2 = ((second_char_index & 0x0F) << 4) | (third_char_index >> 2);
            byte_array.push_back(byte2);
        }
        
        if (i + 3 < input_len && input[i + 3] != '=') {
            uint8_t byte3 = ((third_char_index & 0x03) << 6) | fourth_char_index;
            byte_array.push_back(byte3);
        }
    }
    
    return byte_array;
}

std::vector<uint8_t> cp::py_base64decode(const std::string filename, const char *argv) {
    std::vector<uint8_t> b64_vector;

    PyStatus status;
    PyConfig config;
    PyConfig_InitPythonConfig(&config);

    /* optional but recommended */
    status = PyConfig_SetBytesString(&config, &config.program_name, &argv[0]);
    if (PyStatus_Exception(status)) {
        PyConfig_Clear(&config);  // Clear on error
        Py_ExitStatusException(status);
    }

    /* Initialize the Python interpreter with the config */
    status = Py_InitializeFromConfig(&config);
    if (PyStatus_Exception(status)) {
        PyConfig_Clear(&config);  // Clear on error
        Py_ExitStatusException(status);
    }

    /* Clear the config after initialization */
    PyConfig_Clear(&config);

    /* Base64 decode selected string using Python */
    char code[1024];
    snprintf(code, sizeof(code), 
    "from base64 import b64decode\n"
    "with open('%s', 'rb') as file:\n"
    "\tlines = b' '.join(file.readlines())\n"
    "\tb64_decoded = b64decode(lines)\n", filename.data());

    PyRun_SimpleString(code);

    /* Initalise python 'main' & access the global dictionary */
    PyObject *main_module = PyImport_AddModule("__main__");
    PyObject *global_dict = PyModule_GetDict(main_module);

    PyObject *ptr_b64decoded = PyDict_GetItemString(global_dict, "b64_decoded");

    if (ptr_b64decoded) {
        if (PyBytes_Check(ptr_b64decoded)) {
            Py_ssize_t size = PyBytes_GET_SIZE(ptr_b64decoded);
            uint8_t *data = reinterpret_cast<uint8_t*>(PyBytes_AS_STRING(ptr_b64decoded));
            b64_vector.assign(data, data + size);

        } else {
            throw std::runtime_error("Error: 'b64_decoded' is not of type bytes.");
        }
    } else {
        throw std::runtime_error("Error: Variable 'b64_decoded' not found in Global Dictionary.");
    }

    /* Finalize the Python interpreter */
    if (Py_FinalizeEx() < 0) {
        exit(120);
    }

    return b64_vector;
}

std::vector<uint8_t> cp::hex_decode(const std::vector<uint8_t> &input) {
	std::vector<uint8_t> byte_array;

	for (size_t i = 0; i < input.size(); i+=2) {
		if (i + 1 >= input.size()) break;
		
		// 1 Byte = 2 hex characters; so that's what we get
		uint8_t first_nibble = input[i];
		uint8_t second_nibble = input[i + 1];

		std::string byte_string;
		byte_string.push_back(static_cast<char>(first_nibble));
		byte_string.push_back(static_cast<char>(second_nibble));
		
		// Convert the 2 characters into one byte
		uint8_t byte_value = static_cast<uint8_t> (
					std::stoi(byte_string, nullptr, 16)
				);
		byte_array.push_back(byte_value);
	}

	return byte_array;
}

std::string cp::hex_encode(const std::vector<uint8_t> &input) {
	std::stringstream hex_string;
	hex_string << std::hex;

	for (size_t i = 0; i < input.size(); i++) {
		hex_string << std::setw(2) << std::setfill('0') << static_cast<int>(input[i]);
	}
	
	return hex_string.str();
}
