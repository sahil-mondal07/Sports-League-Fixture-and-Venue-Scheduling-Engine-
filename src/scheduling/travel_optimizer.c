#include "travel_optimizer.h"
#include "../data/venue.h"
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>

/* =========================================================================
 * compute_venue_distance
 *===========================================================================*/

double compute_venue_distance(const Graph* g, int src_idx, int dest_idx,
                              const Venue* venues,
                              const double* const* distance_matrix,
                              int matrix_size)
{
    /* Suppress unused parameter warning */
    (void)g;

    if (distance_matrix && src_idx < matrix_size) {
        /* Use pre-computed distance matrix */
        if (src_idx >= 0 && src_idx < matrix_size && dest_idx >= 0 && dest_idx < matrix_size) {
            return distance_matrix[src_idx][dest_idx];
        }
    }

    /* Fall back to Haversine calculation from venue coordinates */
    if (venues && src_idx >= 0 && dest_idx >= 0) {
        float dist = calculate_distance(
            venues[src_idx].latitude, venues[src_idx].longitude,
            venues[dest_idx].latitude, venues[dest_idx].longitude
        );
        return (double)dist;
    }

    return 0.0;
}

/* =========================================================================
 * compute_total_travel
 *===========================================================================*/

double compute_total_travel(const FixtureSchedule* schedule,
                            const Venue* venues, int venue_count,
                            const double* const* distance_matrix,
                            int matrix_size)
{
    if (!schedule || !schedule->fixtures || schedule->count == 0) return 0.0;
    double total = 0.0;
    for (int i = 0; i < schedule->count; i++) {
        const Fixture* f = &schedule->fixtures[i];
        int home = f->home_team_id;
        int away = f->away_team_id;
        if (home < 0 || away < 0 || home >= venue_count || away >= venue_count) continue;
        /* team index == its home venue index. Away team travels away -> home */
        total += compute_venue_distance(NULL, away, home, venues,
                                        distance_matrix, matrix_size);
    }
    return total;
}

/* =========================================================================
 * generate_travel_report
 *===========================================================================*/

int generate_travel_report(const FixtureSchedule* schedule,
                           const Venue* venues,
                           int venue_count,
                           const double* const* distance_matrix,
                           int matrix_size,
                           FixtureSchedule* optimized_schedule)
{
    (void)optimized_schedule;
    if (!schedule || !venues || venue_count <= 0) {
        return -1;
    }

    double total = compute_total_travel(schedule, venues, venue_count,
                                        distance_matrix, matrix_size);
    printf("=== Travel Report ===\n");
    printf("Total away-travel distance: %.2f km\n", total);
    return 0;
}

/* =========================================================================
 * free_travel_optimizer
 *===========================================================================*/

void free_travel_optimizer(void)
{
    /* No persistent allocation in current implementation */
}
