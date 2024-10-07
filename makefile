# Compiler
CC = g++
CFLAGS = -g -I ./Deps/
# Output Directory 
TARGET_DIR = ../target/
# Ignored Files
DEPS = ./Deps/encoding.cpp ./Deps/encrypting.cpp

# Challenge 3 Compiler Instructions
challenge3: challenge3.cpp
	$(CC) $(CFLAGS) $(DEPS) $^ -o $(TARGET_DIR)$@
	../target/$@

# Clean target
clean:
	rm -f $(TARGET_DIR)/*

# Phony targets
.PHONY: all clean
