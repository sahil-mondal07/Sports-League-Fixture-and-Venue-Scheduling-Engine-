# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = -lm

# Directories
SRC_DIR = src
DATA_DIR = $(SRC_DIR)/data
ALGO_DIR = $(SRC_DIR)/algorithms
UTILS_DIR = $(SRC_DIR)/utils
SCHED_DIR = $(SRC_DIR)/scheduling
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
          $(ALGO_DIR)/sorting/quick_sort.c \
          $(ALGO_DIR)/searching/binary_search.c \
          $(ALGO_DIR)/graph/dijkstra.c \
          $(SCHED_DIR)/validator.c

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
	@mkdir -p $(BUILD_DIR)/algorithms/graph
	@mkdir -p $(BUILD_DIR)/scheduling
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
	@echo "  make test     - Build and run all tests"
	@echo "  make clean    - Remove build files"
	@echo "  make rebuild  - Clean and rebuild"
	@echo "  make help     - Show this help message"

# Test targets
test: test-round-robin test-validator test-time-slots test-dijkstra
	@echo "✓ All tests completed"

test-round-robin: directories
	@echo "Building round-robin tests..."
	@$(CC) $(CFLAGS) tests/test_round_robin.c \
		$(DATA_DIR)/team.c $(DATA_DIR)/venue.c $(DATA_DIR)/fixture.c \
		$(UTILS_DIR)/common.c $(ALGO_DIR)/greedy/round_robin.c \
		-lm -o $(BUILD_DIR)/test_round_robin
	@echo "Running round-robin tests..."
	@./$(BUILD_DIR)/test_round_robin

test-validator: directories
	@echo "Building validator tests..."
	@$(CC) $(CFLAGS) tests/test_validator.c \
		$(DATA_DIR)/team.c $(DATA_DIR)/venue.c $(DATA_DIR)/fixture.c \
		$(UTILS_DIR)/common.c $(ALGO_DIR)/greedy/round_robin.c \
		$(SCHED_DIR)/validator.c \
		-lm -o $(BUILD_DIR)/test_validator
	@echo "Running validator tests..."
	@./$(BUILD_DIR)/test_validator

test-time-slots: directories
	@echo "Building time-slots tests..."
	@$(CC) $(CFLAGS) tests/test_time_slots.c \
		$(DATA_DIR)/team.c $(DATA_DIR)/venue.c $(DATA_DIR)/fixture.c \
		$(UTILS_DIR)/common.c $(ALGO_DIR)/greedy/round_robin.c \
		-lm -o $(BUILD_DIR)/test_time_slots
	@echo "Running time-slots tests..."
	@./$(BUILD_DIR)/test_time_slots

test-dijkstra: directories
	@echo "Building dijkstra tests..."
	@$(CC) $(CFLAGS) tests/test_dijkstra.c \
		$(DATA_DIR)/graph.c $(UTILS_DIR)/common.c $(ALGO_DIR)/graph/dijkstra.c \
		-lm -o $(BUILD_DIR)/test_dijkstra
	@echo "Running dijkstra tests..."
	@./$(BUILD_DIR)/test_dijkstra

.PHONY: all directories run clean rebuild help test test-round-robin test-validator test-time-slots test-dijkstra
