# Compiler Flags
CC = g++
DEPS_DIR = ../../Deps/
LIB_DIR = $(DEPS_DIR)target/
CFLAGS = -g -std=c++14 -Wall -I$(DEPS_DIR) -D_GLIBCXX_USE_CXX11_ABI=1
# Output Directory 
OUTPUT_DIR = ../target/

#CPP Dependencies 
DEPS = -lpython3.13 -lcrypto -L$(LIB_DIR) -lcryptopals

challenge%: challenge%.cpp
	$(CC) $(CFLAGS) $< -o $(OUTPUT_DIR)$@ $(DEPS)
	$(OUTPUT_DIR)$@

clean:
	rm $(OUTPUT_DIR)*

.PHONY:
	clean
	run 