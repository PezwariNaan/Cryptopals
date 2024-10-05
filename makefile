# Compiler
CC = g++

# Directories
TARGET_DIR = ../target/

# Ignored Files
IGNORED = encoding.cpp encrypting.cpp

# Automatically find all .cpp files except encoding.cpp
SOURCES = $(filter-out $(IGNORED), $(wildcard *.cpp))

# Always include encoding.cpp
COMMON_SOURCE = encoding.cpp

# Set TARGET name to the name of each .cpp file without the extension
TARGETS = $(patsubst %.cpp,$(TARGET_DIR)%,$(SOURCES))

INCLUDES = -I .

# Compilation flags
CFLAGS = -Wall -g $(INCLUDES)

# Default target
all: $(TARGETS)

# Rule to build each executable
$(TARGET_DIR)%: %.cpp $(COMMON_SOURCE) | $(TARGET_DIR)
	$(CC) $(CFLAGS) $(COMMON_SOURCE) $< -o $@; $@

# Create necessary directories
$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

# Clean target
clean:
	rm -f $(TARGETS)

# Phony targets
.PHONY: all clean
