# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -D_GLIBCXX_USE_CXX11_ABI=1 -lPython3.12

# Target paths
TARGET_DIR = target
LIBRARY = $(TARGET_DIR)/libcryptopals.a

# Source and object files
SOURCES = encoding.cpp encrypting.cpp openssl.cpp
OBJECTS = $(SOURCES:%.cpp=$(TARGET_DIR)/%.o)

# Default target
all: $(LIBRARY)

# Compile each .cpp file to its respective .o file in the target directory
$(TARGET_DIR)/%.o: %.cpp %.hpp | $(TARGET_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Archive .o files into the static library
$(LIBRARY): $(OBJECTS)
	ar rcs $@ $(OBJECTS)

# Ensure target directory exists
$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

# Clean up all generated files
clean:
	rm -rf $(TARGET_DIR)

# Clean only object files and library
clean_objs:
	rm -f $(TARGET_DIR)/*.o $(LIBRARY)

.PHONY: all clean clean_objs
