# Compiler
CC = g++
CFLAGS = -g -I ./Deps/
OPENSSL_LIBS = crypto
# Output Directory 
TARGET_DIR = ../target/
# Ignored Files
DEPS = ./Deps/encoding.cpp ./Deps/encrypting.cpp

# Challenge [4-6] Compiler Instructions
challenge%: challenge%.cpp
	$(CC) $(CFLAGS) $(DEPS) $^ -o $(TARGET_DIR)$@
	../target/$@

challenge%_crypto: challenge%.cpp
	$(CC) $(CFLAGS) -l$(OPENSSL_LIBS) $(DEPS) $^ -o $(TARGET_DIR)$(basename $<)
	../target/$(basename $<)

# Clean target
clean:
	rm $(TARGET_DIR)*

# Phony targets
.PHONY:
	clean
