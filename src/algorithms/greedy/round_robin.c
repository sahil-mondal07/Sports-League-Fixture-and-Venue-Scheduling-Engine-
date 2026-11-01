#include "round_robin.h"

FixtureSchedule* round_robin_schedule(Team* teams, int team_count,
                                      Venue* venues, int venue_count,
                                      TournamentConfig* config) {
    (void)teams;   // Suppress unused parameter warning
    (void)venues;  // Suppress unused parameter warning

    if (team_count < 2 || venue_count < 1) return NULL;

    FixtureSchedule* schedule = (FixtureSchedule*)malloc(sizeof(FixtureSchedule));
    if (!schedule) return NULL;

    int total_rounds = (team_count - 1) * 2;  // Double round-robin
    int matches_per_round = team_count / 2;
    int total_matches = total_rounds * matches_per_round;

    schedule->fixtures = (Fixture*)malloc(sizeof(Fixture) * total_matches);
    schedule->count = 0;
    schedule->total_rounds = total_rounds;

    int fixture_id = 1;
    int current_day = 0;
    Date current_date = config->start_date;

    // Round-Robin algorithm: Generate all matchups
    for (int round = 1; round <= total_rounds; round++) {
        int matches_in_round = 0;

        // First half: Home-Away
        if (round <= team_count - 1) {
            // Generate first-leg matches
            for (int i = 0; i < team_count / 2; i++) {
                int home_team = i;
                int away_team = team_count - 1 - i;

                if (home_team < away_team) {
                    // Pick venue for this match (rotate through venues)
                    int venue_idx = (fixture_id - 1) % venue_count;

                    // Create fixture
                    Fixture fixture;
                    fixture.id = fixture_id++;
                    fixture.round = round;
                    fixture.home_team_id = home_team;
                    fixture.away_team_id = away_team;
                    fixture.venue_id = venue_idx;
                    fixture.day = current_day;
                    fixture.match_date = current_date;
                    fixture.time_slot = (fixture.id % 3) + 1;  // 3 time slots
                    fixture.travel_cost = 0.0;  // Will be calculated later
                    fixture.is_valid = TRUE;

                    schedule->fixtures[schedule->count++] = fixture;
                    matches_in_round++;
                }
            }
        } else {
            // Generate second-leg matches (reverse)
            for (int i = 0; i < team_count / 2; i++) {
                int home_team = team_count - 1 - i;
                int away_team = i;

                if (away_team < home_team) {
                    int venue_idx = (fixture_id - 1) % venue_count;

                    Fixture fixture;
                    fixture.id = fixture_id++;
                    fixture.round = round;
                    fixture.home_team_id = home_team;
                    fixture.away_team_id = away_team;
                    fixture.venue_id = venue_idx;
                    fixture.day = current_day;
                    fixture.match_date = current_date;
                    fixture.time_slot = (fixture.id % 3) + 1;
                    fixture.travel_cost = 0.0;
                    fixture.is_valid = TRUE;

                    schedule->fixtures[schedule->count++] = fixture;
                    matches_in_round++;
                }
            }
        }

        // Move to next matchday after rest days
        current_day += (config->min_rest_days + 1);
        current_date = add_days(&current_date, config->min_rest_days + 1);
    }

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
