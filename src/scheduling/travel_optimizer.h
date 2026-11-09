#ifndef TRAVEL_OPTIMIZER_H
#define TRAVEL_OPTIMIZER_H

#include "../data/venue.h"
#include "../data/fixture.h"
#include "../data/graph.h"
#include "../utils/common.h"
#include "../algorithms/greedy/round_robin.h"

/**
 * @brief Compute the total travel distance for a fixture schedule
 *
 * @param schedule The fixture schedule with venues assigned
 * @param venues Array of venue structures
 * @param venue_count Number of venues
 * @param distance_matrix Pre-computed distance matrix (km), or NULL if to use Haversine
 * @param matrix_size Size of the distance matrix (number of rows/columns)
 * @return double Total travel distance in kilometers (0.0 on error)
 */
double compute_total_travel(const FixtureSchedule* schedule,
                            const Venue* venues,
                            int venue_count,
                            const double* const* distance_matrix,
                            int matrix_size);

/**
 * @brief Generate a travel report showing total away-travel distance
 *
 * @param schedule Fixture schedule
 * @param venues Array of venue structures
 * @param venue_count Number of venues
 * @param distance_matrix Pre-computed distance matrix (km), or NULL
 * @param matrix_size Size of the distance matrix (number of rows/columns)
 * @param optimized_schedule Unused / optional pointer (can be NULL)
 * @return int 0 on success, -1 on error
 */
int generate_travel_report(const FixtureSchedule* schedule,
                           const Venue* venues,
                           int venue_count,
                           const double* const* distance_matrix,
                           int matrix_size,
                           FixtureSchedule* optimized_schedule);

/* Free any internal state allocated by the optimizer */
void free_travel_optimizer(void);

#endif /* TRAVEL_OPTIMIZER_H */