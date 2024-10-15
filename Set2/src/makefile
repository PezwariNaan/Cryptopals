# Compiler Flags
CC = g++
DEPS_DIR = ../../Deps/
CFLAGS = -g -Wall -I $(DEPS_DIR)
# Output Directory 
OUTPUT_DIR = ../target/

#CPP Dependencies 
DEPS = $(DEPS_DIR)/encoding.cpp $(DEPS_DIR)/encrypting.cpp

challenge%: challenge%.cpp
	$(CC) $(CFLAGS) $(DEPS) $< -o $(OUTPUT_DIR)$@
	$(OUTPUT_DIR)$@

clean:
	rm $(OUTPUT_DIR)/*

.PHONY:
	clean