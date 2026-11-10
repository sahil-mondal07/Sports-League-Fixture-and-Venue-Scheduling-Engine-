#include "branch_bound.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BB_INFINITY 1e30

/* State node in the search tree */
typedef struct BBNode {
    int* assignment;       /* Current partial assignment (size n) */
    int* assigned_venues;  /* Boolean mask of assigned venues (size n) */
    int level;             /* Number of matches assigned so far (0..n) */
    double current_cost;   /* Cost incurred so far */
    double lower_bound;    /* Estimated lower bound of complete solution */
    struct BBNode* next;   /* For linked list queue/stack */
} BBNode;

/* Compute lower bound for a partial assignment node */
static double compute_lower_bound(const double* cost_matrix, int n,
                                  const int* assigned_venues, int level, double current_cost) {
    double lb = current_cost;
    for (int i = level; i < n; i++) {
        double min_val = BB_INFINITY;
        for (int j = 0; j < n; j++) {
            if (!assigned_venues[j] && cost_matrix[i * n + j] < min_val) {
                min_val = cost_matrix[i * n + j];
            }
        }
        if (min_val < BB_INFINITY / 2.0) {
            lb += min_val;
        }
    }
    return lb;
}

static BBNode* create_node(int n, const int* parent_assignment, const int* parent_venues,
                           int level, double current_cost, double lower_bound) {
    BBNode* node = (BBNode*)malloc(sizeof(BBNode));
    if (!node) return NULL;

    node->assignment = (int*)malloc((size_t)n * sizeof(int));
    node->assigned_venues = (int*)malloc((size_t)n * sizeof(int));

    if (!node->assignment || !node->assigned_venues) {
        free(node->assignment);
        free(node->assigned_venues);
        free(node);
        return NULL;
    }

    if (parent_assignment && level > 0) {
        memcpy(node->assignment, parent_assignment, (size_t)level * sizeof(int));
    }
    if (parent_venues) {
        memcpy(node->assigned_venues, parent_venues, (size_t)n * sizeof(int));
    } else {
        memset(node->assigned_venues, 0, (size_t)n * sizeof(int));
    }

    node->level = level;
    node->current_cost = current_cost;
    node->lower_bound = lower_bound;
    node->next = NULL;
    return node;
}

static void free_node(BBNode* node) {
    if (node) {
        free(node->assignment);
        free(node->assigned_venues);
        free(node);
    }
}

BBResult solve_assignment_branch_bound(const double* cost_matrix, int n, BBStrategy strategy) {
    BBResult res;
    res.assignment = NULL;
    res.n = n;
    res.best_cost = BB_INFINITY;
    res.nodes_generated = 0;
    res.nodes_explored = 0;
    res.nodes_pruned = 0;
    res.strategy = strategy;

    if (!cost_matrix || n <= 0 || n > 20) {
        return res;
    }

    res.assignment = (int*)malloc((size_t)n * sizeof(int));
    if (!res.assignment) return res;
    for (int i = 0; i < n; i++) res.assignment[i] = -1;

    /* Compute root initial lower bound */
    int* root_venues = (int*)calloc((size_t)n, sizeof(int));
    double root_lb = compute_lower_bound(cost_matrix, n, root_venues, 0, 0.0);

    BBNode* root = create_node(n, NULL, root_venues, 0, 0.0, root_lb);
    free(root_venues);
    if (!root) {
        free(res.assignment);
        res.assignment = NULL;
        return res;
    }

    res.nodes_generated++;

    /* Data structure for queue / stack / priority list */
    BBNode* head = root;

    while (head != NULL) {
        /* Extract next node based on strategy */
        BBNode* curr = head;

        if (strategy == BB_STRATEGY_FIFO) {
            /* Dequeue from front */
            head = head->next;
        } else if (strategy == BB_STRATEGY_LIFO) {
            /* Pop from front (acting as stack) */
            head = head->next;
        } else {
            /* Least-cost: extract node with minimum lower_bound from list */
            BBNode* min_node = head;
            BBNode* min_prev = NULL;

            BBNode* p = head;
            BBNode* pp = NULL;
            while (p != NULL) {
                if (p->lower_bound < min_node->lower_bound) {
                    min_node = p;
                    min_prev = pp;
                }
                pp = p;
                p = p->next;
            }

            if (min_prev == NULL) {
                head = head->next;
            } else {
                min_prev->next = min_node->next;
            }
            curr = min_node;
        }

        res.nodes_explored++;

        /* If node's lower bound is already >= best_cost, prune it */
        if (curr->lower_bound >= res.best_cost - 1e-6) {
            res.nodes_pruned++;
            free_node(curr);
            continue;
        }

        /* If complete assignment reached */
        if (curr->level == n) {
            if (curr->current_cost < res.best_cost) {
                res.best_cost = curr->current_cost;
                memcpy(res.assignment, curr->assignment, (size_t)n * sizeof(int));
            }
            free_node(curr);
            continue;
        }

        /* Branch for match curr->level across all available venues */
        int match_idx = curr->level;
        for (int venue = 0; venue < n; venue++) {
            if (!curr->assigned_venues[venue]) {
                double new_cost = curr->current_cost + cost_matrix[match_idx * n + venue];

                /* Prune if partial cost exceeds current best */
                if (new_cost >= res.best_cost - 1e-6) {
                    res.nodes_pruned++;
                    continue;
                }

                curr->assigned_venues[venue] = 1;
                curr->assignment[match_idx] = venue;

                double new_lb = compute_lower_bound(cost_matrix, n, curr->assigned_venues,
                                                    match_idx + 1, new_cost);

                if (new_lb < res.best_cost - 1e-6) {
                    BBNode* child = create_node(n, curr->assignment, curr->assigned_venues,
                                                match_idx + 1, new_cost, new_lb);
                    if (child) {
                        res.nodes_generated++;
                        /* Insert child into list according to strategy */
                        if (strategy == BB_STRATEGY_LIFO) {
                            /* Push to front */
                            child->next = head;
                            head = child;
                        } else {
                            /* Push to back (FIFO or LC handles sorting) */
                            if (head == NULL) {
                                head = child;
                            } else {
                                BBNode* tail = head;
                                while (tail->next != NULL) tail = tail->next;
                                tail->next = child;
                            }
                        }
                    }
                } else {
                    res.nodes_pruned++;
                }

                curr->assigned_venues[venue] = 0; /* backtrack */
            }
        }

        free_node(curr);
    }

    return res;
}

void print_bb_result(const BBResult* res) {
    if (!res) return;

    const char* strat_name = (res->strategy == BB_STRATEGY_FIFO) ? "FIFO (Breadth-First)" :
                             (res->strategy == BB_STRATEGY_LIFO) ? "LIFO (Depth-First)" :
                             "Least-Cost (Best-First)";

    printf("\n=== Branch & Bound Assignment Result [%s] ===\n", strat_name);
    printf("Optimal Assignment Cost: %.2f\n", res->best_cost);
    printf("Performance Metrics:\n");
    printf("  - Nodes Generated: %ld\n", res->nodes_generated);
    printf("  - Nodes Explored:  %ld\n", res->nodes_explored);
    printf("  - Nodes Pruned:    %ld\n", res->nodes_pruned);
    printf("Assignment (Match -> Venue):\n");
    for (int i = 0; i < res->n; i++) {
        printf("  Match %2d -> Venue %2d\n", i, res->assignment[i]);
    }
}

void free_bb_result(BBResult* res) {
    if (res) {
        free(res->assignment);
        res->assignment = NULL;
        res->n = 0;
        res->best_cost = BB_INFINITY;
    }
}
