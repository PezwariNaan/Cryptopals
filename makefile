# Compiler
CC = g++
CFLAGS = -g -I ./Deps/
# Output Directory 
TARGET_DIR = ../target/
# Ignored Files
DEPS = ./Deps/encoding.cpp ./Deps/encrypting.cpp

# Challenge [4-6] Compiler Instructions
challenge%: challenge%.cpp
	$(CC) $(CFLAGS) $(DEPS) $^ -o $(TARGET_DIR)$@
	../target/$@

# Clean target
clean:
	rm $(TARGET_DIR)*

# Phony targets
.PHONY:
	clean
