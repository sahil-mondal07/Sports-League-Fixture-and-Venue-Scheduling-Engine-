# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = -lm

# Directories
SRC_DIR = src
DATA_DIR = $(SRC_DIR)/data
ALGO_DIR = $(SRC_DIR)/algorithms
UTILS_DIR = $(SRC_DIR)/utils
BUILD_DIR = build
BIN_DIR = bin

# Target executable
TARGET = $(BIN_DIR)/scheduler

# Source files
SOURCES = $(SRC_DIR)/main.c \
          $(DATA_DIR)/team.c \
          $(DATA_DIR)/venue.c \
          $(DATA_DIR)/fixture.c \
          $(DATA_DIR)/graph.c \
          $(DATA_DIR)/loader.c \
          $(UTILS_DIR)/common.c \
          $(ALGO_DIR)/greedy/round_robin.c \
          $(ALGO_DIR)/sorting/merge_sort.c \
          $(ALGO_DIR)/searching/binary_search.c

# Object files
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	@mkdir -p $(BUILD_DIR)/data
	@mkdir -p $(BUILD_DIR)/algorithms/greedy
	@mkdir -p $(BUILD_DIR)/algorithms/sorting
	@mkdir -p $(BUILD_DIR)/algorithms/searching
	@mkdir -p $(BUILD_DIR)/utils
	@mkdir -p $(BIN_DIR)

# Link object files to create executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "✓ Build successful: $(TARGET)"

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program
run: all
	@echo "Starting scheduler..."
	@./$(TARGET)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "✓ Cleaned build files"

# Clean and rebuild
rebuild: clean all

# Help target
help:
	@echo "Available targets:"
	@echo "  make          - Build the project"
	@echo "  make run      - Build and run the scheduler"
	@echo "  make clean    - Remove build files"
	@echo "  make rebuild  - Clean and rebuild"
	@echo "  make help     - Show this help message"

.PHONY: all directories run clean rebuild help
