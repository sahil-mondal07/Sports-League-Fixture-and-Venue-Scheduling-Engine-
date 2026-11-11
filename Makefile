CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -g -O2
LDFLAGS = -lm

SRC_DIR = src
TEST_DIR = tests

COMMON_OBJS = \
	$(SRC_DIR)/data/team.o \
	$(SRC_DIR)/data/venue.o \
	$(SRC_DIR)/data/fixture.o \
	$(SRC_DIR)/data/loader.o \
	$(SRC_DIR)/data/graph.o \
	$(SRC_DIR)/utils/common.o \
	$(SRC_DIR)/scheduling/validator.o \
	$(SRC_DIR)/scheduling/travel_optimizer.o \
	$(SRC_DIR)/algorithms/greedy/round_robin.o \
	$(SRC_DIR)/algorithms/sorting/merge_sort.o \
	$(SRC_DIR)/algorithms/sorting/quick_sort.o \
	$(SRC_DIR)/algorithms/searching/binary_search.o \
	$(SRC_DIR)/algorithms/graph/dijkstra.o \
	$(SRC_DIR)/algorithms/graph/prim.o \
	$(SRC_DIR)/algorithms/graph/kruskal.o \
	$(SRC_DIR)/algorithms/dp/bellman_ford.o \
	$(SRC_DIR)/algorithms/dp/floyd_warshall.o \
	$(SRC_DIR)/algorithms/dp/bounded_tsp.o \
	$(SRC_DIR)/algorithms/dp/dp_optimal_bst.o \
	$(SRC_DIR)/algorithms/dp/dp_knapsack.o \
	$(SRC_DIR)/algorithms/dp/dp_matrix_chain.o \
	$(SRC_DIR)/algorithms/dp/dp_lcs.o \
	$(SRC_DIR)/algorithms/backtracking/n_queens.o \
	$(SRC_DIR)/algorithms/backtracking/subset_sum.o \
	$(SRC_DIR)/algorithms/divide_conquer/max_subarray.o \
	$(SRC_DIR)/algorithms/divide_conquer/fast_power.o \
	$(SRC_DIR)/algorithms/divide_conquer/celebrity.o \
	$(SRC_DIR)/algorithms/divide_conquer/strassen.o \
	$(SRC_DIR)/algorithms/flow/ford_fulkerson.o \
	$(SRC_DIR)/algorithms/flow/bipartite_matching.o \
	$(SRC_DIR)/algorithms/greedy/activity_selection.o \
	$(SRC_DIR)/algorithms/greedy/interval_partitioning.o \
	$(SRC_DIR)/algorithms/greedy/huffman.o \
	$(SRC_DIR)/algorithms/string_matching/string_matcher.o \
	$(SRC_DIR)/algorithms/number_theory/crt.o

all: validator_test main_app

main_app: $(COMMON_OBJS) $(SRC_DIR)/main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

validator_test: $(COMMON_OBJS) $(TEST_DIR)/test_validator.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test: validator_test
	./validator_test

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -f $(COMMON_OBJS) $(SRC_DIR)/main.o $(TEST_DIR)/test_validator.o validator_test.exe main_app.exe validator_test main_app
