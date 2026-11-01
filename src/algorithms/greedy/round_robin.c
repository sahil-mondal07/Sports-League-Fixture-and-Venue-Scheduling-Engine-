#include "round_robin.h"
#include <string.h>

FixtureSchedule* round_robin_schedule(Team* teams, int team_count,
                                      Venue* venues, int venue_count,
                                      TournamentConfig* config) {
    (void)teams;   // Suppress unused parameter warning
    (void)venues;  // Suppress unused parameter warning

    // Check for NULL config - SAFETY FIX C
    if (!config) return NULL;

    if (team_count < 2 || venue_count < 1) return NULL;

    FixtureSchedule* schedule = (FixtureSchedule*)malloc(sizeof(FixtureSchedule));
    if (!schedule) return NULL;

    // Handle odd team counts by adding a dummy team
    int n = team_count;
    int is_odd = (n % 2 == 1);
    if (is_odd) {
        n++;  // Add dummy team for bye handling
    }

    int total_rounds = 2 * (n - 1);  // Double round-robin
    int total_matches = team_count * (team_count - 1);  // Actual matches (no byes)

    // Check fixtures allocation - SAFETY FIX C
    schedule->fixtures = (Fixture*)malloc(sizeof(Fixture) * total_matches);
    if (!schedule->fixtures) {
        free(schedule);
        return NULL;
    }

    schedule->count = 0;
    schedule->total_rounds = total_rounds;

    // Allocate array for current round configuration
    int* current_round = (int*)malloc(sizeof(int) * n);
    if (!current_round) {
        free(schedule->fixtures);
        free(schedule);
        return NULL;
    }

    // Initialize: teams 0, 1, 2, ..., n-1
    for (int i = 0; i < n; i++) {
        current_round[i] = i;
    }

    int fixture_id = 1;
    int current_day = 0;
    Date current_date = config->start_date;

    // Track venue usage per round for slot safety - FIX B
    int* venue_slot_count = (int*)malloc(sizeof(int) * venue_count);
    if (!venue_slot_count) {
        free(current_round);
        free(schedule->fixtures);
        free(schedule);
        return NULL;
    }

    // ============================================================
    // BERGER TABLE ALGORITHM
    // ============================================================
    // First leg: normal pairings
    // Second leg: reverse home/away
    for (int leg = 0; leg < 2; leg++) {
        // Reset rotation for second leg
        if (leg == 1) {
            for (int i = 0; i < n; i++) {
                current_round[i] = i;
            }
        }

        for (int round_num = 0; round_num < n - 1; round_num++) {
            int matches_in_round = 0;

            // Reset venue slot counters for this round - FIX B
            for (int v = 0; v < venue_count; v++) {
                venue_slot_count[v] = 0;
            }

            // Pair teams: position i with position (n-1-i)
            for (int i = 0; i < n / 2; i++) {
                int team_a = current_round[i];
                int team_b = current_round[n - 1 - i];

                // Skip if either team is the dummy (team_count for odd n)
                if (team_a == team_count || team_b == team_count) {
                    continue;  // This is a bye round for one team
                }

                int home_team, away_team;

                // FIX A: Home/away fairness using round_num
                if (leg == 0) {
                    // First leg: alternate based on round parity
                    home_team = (round_num % 2 == 0) ? team_a : team_b;
                    away_team = (round_num % 2 == 0) ? team_b : team_a;
                } else {
                    // Second leg: swap the first-leg result
                    away_team = (round_num % 2 == 0) ? team_a : team_b;
                    home_team = (round_num % 2 == 0) ? team_b : team_a;
                }

                // Venue: home team's stadium
                int venue_idx = home_team % venue_count;

                // FIX B: Slot safety - find lowest unused slot at this venue
                int time_slot = venue_slot_count[venue_idx] + 1;

                // Check if venue would need 4th slot (impossible with 3 slots)
                if (time_slot > 3) {
                    free(venue_slot_count);
                    free(current_round);
                    free(schedule->fixtures);
                    free(schedule);
                    printf("ERROR: Not enough venues or time slots. Venue %d would need %d slots in round %d\n",
                           venue_idx, time_slot, round_num + 1);
                    return NULL;
                }

                // Increment count for this venue
                venue_slot_count[venue_idx]++;

                Fixture fixture;
                fixture.id = fixture_id++;
                fixture.round = (leg * (n - 1)) + round_num + 1;
                fixture.home_team_id = home_team;
                fixture.away_team_id = away_team;
                fixture.venue_id = venue_idx;
                fixture.day = current_day;
                fixture.match_date = current_date;
                fixture.time_slot = time_slot;
                fixture.travel_cost = 0.0;
                fixture.is_valid = TRUE;

                schedule->fixtures[schedule->count++] = fixture;
                matches_in_round++;
            }

            // Move to next matchday after rest days
            current_day += (config->min_rest_days + 1);
            current_date = add_days(&current_date, config->min_rest_days + 1);

            // ============================================================
            // ROTATION: Fix team 0, rotate others clockwise
            // ============================================================
            if (round_num < n - 2) {  // Don't rotate after last round
                // Save last team (position n-1)
                int last_team = current_round[n - 1];

                // Shift right: position i gets value from position i-1
                for (int i = n - 1; i > 1; i--) {
                    current_round[i] = current_round[i - 1];
                }

                // Position 1 gets the last team
                current_round[1] = last_team;
            }
        }
    }

    free(venue_slot_count);
    free(current_round);
    return schedule;
}

void print_fixture_schedule(FixtureSchedule* schedule) {
    if (!schedule || schedule->count == 0) {
        printf("\nNo fixtures generated.\n");
        return;
    }

    printf("\n=== FIXTURE SCHEDULE ===\n");
    printf("Total Rounds: %d | Total Matches: %d\n\n", schedule->total_rounds, schedule->count);

    int current_round = 0;
    for (int i = 0; i < schedule->count; i++) {
        Fixture* f = &schedule->fixtures[i];

        if (f->round != current_round) {
            printf("\n--- Round %d ---\n", f->round);
            current_round = f->round;
        }

        printf("Match #%d: Team %d vs Team %d | Venue %d | ",
               f->id, f->home_team_id, f->away_team_id, f->venue_id);
        print_date(&f->match_date);
        printf(" | Slot %d\n", f->time_slot);
    }
}

void free_fixture_schedule(FixtureSchedule* schedule) {
    if (schedule) {
        if (schedule->fixtures) {
            free(schedule->fixtures);
        }
        free(schedule);
    }
}
