#ifndef FORD_FULKERSON_H
#define FORD_FULKERSON_H

#include "../../utils/common.h"

#define MAX_FLOW_VERTICES 50

/**
 * @brief Flow network represented by capacity matrix.
 */
typedef struct {
    double capacity[MAX_FLOW_VERTICES][MAX_FLOW_VERTICES]; /**< Maximum capacity from u to v */
    double flow[MAX_FLOW_VERTICES][MAX_FLOW_VERTICES];     /**< Current flow from u to v */
    int num_vertices;                                      /**< Total number of nodes */
} FlowNetwork;

/**
 * @brief Result structure for maximum flow.
 */
typedef struct {
    double max_flow_value;                                 /**< Total maximum flow from source to sink */
    double flow[MAX_FLOW_VERTICES][MAX_FLOW_VERTICES];     /**< Resulting flow matrix */
    int num_vertices;
    int source;
    int sink;
} FlowResult;

/**
 * @brief Initialize a flow network with given vertex count.
 */
FlowNetwork* flow_network_create(int num_vertices);

/**
 * @brief Add a directed edge with capacity to the flow network.
 */
void flow_network_add_edge(FlowNetwork* net, int u, int v, double capacity);

/**
 * @brief Free a flow network.
 */
void flow_network_free(FlowNetwork* net);

/**
 * @brief Compute Maximum Flow from source to sink using Edmonds-Karp (Ford-Fulkerson).
 *
 * In sports scheduling context: Determines maximum tournament attendance,
 * match capacity throughput, or broadcast bandwidth allocation under strict
 * venue capacity limits.
 *
 * @param net Pointer to FlowNetwork
 * @param source Source node index
 * @param sink Sink node index
 * @return FlowResult containing max flow value and flow distribution.
 */
FlowResult ford_fulkerson(const FlowNetwork* net, int source, int sink);

/**
 * @brief Print Ford-Fulkerson max flow result.
 */
void print_flow_result(const FlowResult* res);

#endif /* FORD_FULKERSON_H */
