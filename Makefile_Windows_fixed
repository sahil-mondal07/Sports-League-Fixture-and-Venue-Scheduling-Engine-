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

# Platform-specific commands (GNU Make + MinGW on Windows, GNU Make on Unix)
ifeq ($(OS),Windows_NT)
EXE = .exe
SHELL := cmd.exe
.SHELLFLAGS := /C
MKDIR = if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
RM_BUILD = if exist "$(BUILD_DIR)" rmdir /S /Q "$(BUILD_DIR)"
RM_BIN = if exist "$(BIN_DIR)" rmdir /S /Q "$(BIN_DIR)"
RUN_PREFIX =
else
EXE =
MKDIR = mkdir -p "$(1)"
RM_BUILD = rm -rf "$(BUILD_DIR)"
RM_BIN = rm -rf "$(BIN_DIR)"
RUN_PREFIX = ./
endif

TARGET = $(BIN_DIR)/scheduler$(EXE)
GUI_BRIDGE = $(BIN_DIR)/gui_bridge$(EXE)

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
          $(ALGO_DIR)/graph/prim.c \
          $(ALGO_DIR)/greedy/activity_selection.c \
          $(ALGO_DIR)/greedy/interval_partitioning.c \
          $(ALGO_DIR)/greedy/huffman.c \
          $(ALGO_DIR)/graph/kruskal.c \
          $(ALGO_DIR)/backtracking/n_queens.c \
          $(ALGO_DIR)/backtracking/subset_sum.c \
          $(ALGO_DIR)/divide_conquer/celebrity.c \
          $(ALGO_DIR)/divide_conquer/fast_power.c \
          $(ALGO_DIR)/divide_conquer/max_subarray.c \
          $(ALGO_DIR)/divide_conquer/strassen.c \
          $(ALGO_DIR)/dp/bellman_ford.c \
          $(ALGO_DIR)/dp/dp_knapsack.c \
          $(ALGO_DIR)/dp/dp_lcs.c \
          $(ALGO_DIR)/dp/dp_matrix_chain.c \
          $(ALGO_DIR)/dp/dp_optimal_bst.c \
          $(ALGO_DIR)/dp/floyd_warshall.c \
          $(ALGO_DIR)/dp/bounded_tsp.c \
          $(SRC_DIR)/benchmark/benchmark.c \
          $(SCHED_DIR)/travel_optimizer.c \
          $(SCHED_DIR)/validator.c \
          $(SRC_DIR)/gui.c

OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

all: directories $(TARGET) $(GUI_BRIDGE)

directories:
	@$(call MKDIR,$(BUILD_DIR))
	@$(call MKDIR,$(BIN_DIR))

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo Build successful: $@

$(GUI_BRIDGE): $(SRC_DIR)/gui_bridge.c $(DATA_DIR)/team.c $(DATA_DIR)/venue.c $(DATA_DIR)/loader.c $(DATA_DIR)/graph.c $(DATA_DIR)/fixture.c $(UTILS_DIR)/common.c $(ALGO_DIR)/greedy/round_robin.c $(ALGO_DIR)/graph/dijkstra.c $(SCHED_DIR)/validator.c | directories
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | directories
	@$(call MKDIR,$(dir $@))
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	@echo Starting scheduler...
	@$(RUN_PREFIX)$(TARGET)

clean:
	@$(RM_BUILD)
	@$(RM_BIN)
	@echo Cleaned build files

rebuild: clean all

help:
	@echo Available targets: all run test clean rebuild help

test: test-round-robin test-validator test-time-slots test-dijkstra test-travel test-prim test-quick-sort test-binary-search test-dp test-greedy-dc test-graph-search
	@echo All tests completed

test-round-robin: directories
	@echo Building round-robin tests...
	@$(CC) $(CFLAGS) tests/test_round_robin.c \
		$(DATA_DIR)/team.c $(DATA_DIR)/venue.c $(DATA_DIR)/fixture.c \
		$(UTILS_DIR)/common.c $(ALGO_DIR)/greedy/round_robin.c \
		-lm -o $(BUILD_DIR)/test_round_robin$(EXE)
	@echo Running round-robin tests...
	@$(RUN_PREFIX)$(BUILD_DIR)/test_round_robin$(EXE)

test-validator: directories
	@echo Building validator tests...
	@$(CC) $(CFLAGS) tests/test_validator.c \
		$(DATA_DIR)/team.c $(DATA_DIR)/venue.c $(DATA_DIR)/fixture.c \
		$(UTILS_DIR)/common.c $(ALGO_DIR)/greedy/round_robin.c \
		$(SCHED_DIR)/validator.c \
		-lm -o $(BUILD_DIR)/test_validator$(EXE)
	@echo Running validator tests...
	@$(RUN_PREFIX)$(BUILD_DIR)/test_validator$(EXE)

test-time-slots: directories
	@echo Building time-slots tests...
	@$(CC) $(CFLAGS) tests/test_time_slots.c \
		$(DATA_DIR)/team.c $(DATA_DIR)/venue.c $(DATA_DIR)/fixture.c \
		$(UTILS_DIR)/common.c $(ALGO_DIR)/greedy/round_robin.c \
		-lm -o $(BUILD_DIR)/test_time_slots$(EXE)
	@echo Running time-slots tests...
	@$(RUN_PREFIX)$(BUILD_DIR)/test_time_slots$(EXE)

test-dijkstra: directories
	@echo Building dijkstra tests...
	@$(CC) $(CFLAGS) tests/test_dijkstra.c \
		$(DATA_DIR)/graph.c $(UTILS_DIR)/common.c $(ALGO_DIR)/graph/dijkstra.c \
		-lm -o $(BUILD_DIR)/test_dijkstra$(EXE)
	@echo Running dijkstra tests...
	@$(RUN_PREFIX)$(BUILD_DIR)/test_dijkstra$(EXE)

test-travel: directories
	@echo Building travel tests...
	@$(CC) $(CFLAGS) tests/test_travel.c \
		$(DATA_DIR)/team.c $(DATA_DIR)/venue.c $(DATA_DIR)/fixture.c \
		$(DATA_DIR)/graph.c $(DATA_DIR)/loader.c $(UTILS_DIR)/common.c \
		$(ALGO_DIR)/greedy/round_robin.c $(SCHED_DIR)/travel_optimizer.c \
		-lm -o $(BUILD_DIR)/test_travel$(EXE)
	@echo Running travel tests...
	@$(RUN_PREFIX)$(BUILD_DIR)/test_travel$(EXE)

test-prim: directories
	@echo Building prim tests...
	@$(CC) $(CFLAGS) tests/test_prim.c \
		$(DATA_DIR)/graph.c $(DATA_DIR)/venue.c $(UTILS_DIR)/common.c \
		$(ALGO_DIR)/graph/prim.c \
		-lm -o $(BUILD_DIR)/test_prim$(EXE)
	@echo Running prim tests...
	@$(RUN_PREFIX)$(BUILD_DIR)/test_prim$(EXE)

test-quick-sort: directories
	@echo Building quick-sort tests...
	@$(CC) $(CFLAGS) tests/test_quick_sort.c \
		$(DATA_DIR)/team.c $(DATA_DIR)/fixture.c $(UTILS_DIR)/common.c \
		$(ALGO_DIR)/sorting/quick_sort.c \
		-lm -o $(BUILD_DIR)/test_quick_sort$(EXE)
	@echo Running quick-sort tests...
	@$(RUN_PREFIX)$(BUILD_DIR)/test_quick_sort$(EXE)

test-binary-search: directories
	@echo Building binary-search tests...
	@$(CC) $(CFLAGS) tests/test_binary_search.c \
		$(DATA_DIR)/fixture.c $(UTILS_DIR)/common.c \
		$(ALGO_DIR)/searching/binary_search.c \
		-lm -o $(BUILD_DIR)/test_binary_search$(EXE)
	@echo Running binary-search tests...
	@$(RUN_PREFIX)$(BUILD_DIR)/test_binary_search$(EXE)

test-dp: directories
	@echo Building DP tests...
	@$(CC) $(CFLAGS) tests/test_dp.c \
		$(ALGO_DIR)/dp/dp_knapsack.c \
		$(ALGO_DIR)/dp/dp_matrix_chain.c \
		$(ALGO_DIR)/dp/dp_lcs.c \
		$(ALGO_DIR)/dp/dp_optimal_bst.c \
		$(ALGO_DIR)/dp/bellman_ford.c \
		$(ALGO_DIR)/dp/floyd_warshall.c \
		$(ALGO_DIR)/dp/bounded_tsp.c \
		-lm -o $(BUILD_DIR)/test_dp$(EXE)
	@echo Running DP tests...
	@$(RUN_PREFIX)$(BUILD_DIR)/test_dp$(EXE)

test-greedy-dc: directories
	@echo Building greedy and DC tests...
	@$(CC) $(CFLAGS) tests/test_greedy_dc.c \
		$(ALGO_DIR)/greedy/activity_selection.c \
		$(ALGO_DIR)/greedy/interval_partitioning.c \
		$(ALGO_DIR)/greedy/huffman.c \
		$(ALGO_DIR)/divide_conquer/max_subarray.c \
		$(ALGO_DIR)/divide_conquer/fast_power.c \
		$(ALGO_DIR)/divide_conquer/celebrity.c \
		$(ALGO_DIR)/divide_conquer/strassen.c \
		$(ALGO_DIR)/sorting/merge_sort.c \
		-lm -o $(BUILD_DIR)/test_greedy_dc$(EXE)
	@echo Running greedy and DC tests...
	@$(RUN_PREFIX)$(BUILD_DIR)/test_greedy_dc$(EXE)

test-graph-search: directories
	@echo Building graph search tests...
	@$(CC) $(CFLAGS) tests/test_graph_search.c \
		$(DATA_DIR)/graph.c $(ALGO_DIR)/flow/ford_fulkerson.c \
		$(ALGO_DIR)/flow/bipartite_matching.c \
		$(ALGO_DIR)/graph/kruskal.c \
		$(ALGO_DIR)/graph/prim.c \
		$(ALGO_DIR)/backtracking/n_queens.c \
		$(ALGO_DIR)/backtracking/subset_sum.c \
		$(ALGO_DIR)/number_theory/crt.c \
		$(ALGO_DIR)/string_matching/string_matcher.c \
		-lm -o $(BUILD_DIR)/test_graph_search$(EXE)
	@echo Running graph search tests...
	@$(RUN_PREFIX)$(BUILD_DIR)/test_graph_search$(EXE)

.PHONY: all directories run clean rebuild help test test-round-robin test-validator test-time-slots test-dijkstra test-travel test-prim test-quick-sort test-binary-search test-dp test-greedy-dc test-graph-search
