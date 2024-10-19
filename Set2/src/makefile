# Compiler Flags
CC = g++
DEPS_DIR = ../../Deps/
LIB_DIR = $(DEPS_DIR)target/
CFLAGS = -g -Wall -I $(DEPS_DIR)
# Output Directory 
OUTPUT_DIR = ../target/
# Add library path to linker path
export LD_LIBRARY_PATH += $(LIB_DIR)

#CPP Dependencies 
DEPS = -lencoding -lencrypting -lcrypto

challenge%: challenge%.cpp
	$(CC) $(CFLAGS) -L$(LIB_DIR) $(DEPS) $< -o $(OUTPUT_DIR)$@
	$(OUTPUT_DIR)$@

clean:
	rm $(OUTPUT_DIR)*

.PHONY:
	clean