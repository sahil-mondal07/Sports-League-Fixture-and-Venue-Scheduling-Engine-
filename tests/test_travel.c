#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../src/data/team.h"
#include "../src/data/venue.h"
#include "../src/data/fixture.h"
#include "../src/data/loader.h"
#include "../src/algorithms/greedy/round_robin.h"
#include "../src/scheduling/travel_optimizer.h"

int main(void) {
    printf("========================================\n");
    printf("TEST: LaLiga Total Away-Travel\n");
    printf("========================================\n\n");

    int team_count = 0;
    Team* teams = load_teams("data/football/laliga_teams.csv", &team_count);
    if (!teams || team_count != 20) {
        fprintf(stderr, "FAIL: Could not load 20 teams from laliga_teams.csv\n");
        return 1;
    }

    int venue_count = 0;
    Venue* venues = load_venues("data/football/laliga_venues.csv", &venue_count);
    if (!venues || venue_count != 20) {
        fprintf(stderr, "FAIL: Could not load 20 venues from laliga_venues.csv\n");
        free(teams);
        return 1;
    }

    double** distance_matrix = load_distance_matrix("data/football/laliga_distances.csv", team_count);
    if (!distance_matrix) {
        fprintf(stderr, "FAIL: Could not load distance matrix from laliga_distances.csv\n");
        free(teams);
        free(venues);
        return 1;
    }

    TournamentConfig config;
    config.start_date.day = 1;
    config.start_date.month = 1;
    config.start_date.year = 2026;
    config.end_date.day = 31;
    config.end_date.month = 12;
    config.end_date.year = 2026;
    config.min_rest_days = 1;
    config.matches_per_week = 2;

    FixtureSchedule* schedule = round_robin_schedule(teams, team_count, venues, venue_count, &config);
    if (!schedule || !schedule->fixtures || schedule->count == 0) {
        fprintf(stderr, "FAIL: Could not generate round-robin schedule\n");
        for (int i = 0; i < team_count; i++) free(distance_matrix[i]);
        free(distance_matrix);
        free(teams);
        free(venues);
        return 1;
    }

    double total = compute_total_travel(schedule, venues, venue_count,
                                        (const double* const*)distance_matrix, team_count);
    printf("Computed total travel: %.2f km\n", total);

    double expected = 211720.2;
    double diff = fabs(total - expected);

    int exit_code = 0;
    if (diff > 1.0) {
        printf("FAIL: Expected %.2f +/- 1.0 km, got %.2f km (diff: %.2f km)\n", expected, total, diff);
        exit_code = 1;
    } else {
        printf("PASS: Total travel %.2f km matches expected %.2f km (+/- 1.0 km)\n", total, expected);
    }

    free_fixture_schedule(schedule);
    for (int i = 0; i < team_count; i++) free(distance_matrix[i]);
    free(distance_matrix);
    free(teams);
    free(venues);

    return exit_code;
}
