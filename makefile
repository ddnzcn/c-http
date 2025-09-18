# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -std=c99

# Directories
SRC_DIR = src
BIN_DIR = bin

# Find all .c source files recursively
SRC = $(shell find $(SRC_DIR) -name "*.c")

# Corresponding object files in BIN_DIR
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(SRC))

# Target executable
TARGET = $(BIN_DIR)/http_server

# Default rule
all: $(TARGET)

# Link step
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

# Compile each .c file to .o in bin/
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build
clean:
	rm -rf $(BIN_DIR)

# Rebuild everything
rebuild: clean all

.PHONY: all clean rebuild

