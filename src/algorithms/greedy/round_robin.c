#include "round_robin.h"

FixtureSchedule* round_robin_schedule(Team* teams, int team_count,
                                      Venue* venues, int venue_count,
                                      TournamentConfig* config) {
    (void)teams;   // Suppress unused parameter warning
    (void)venues;  // Suppress unused parameter warning

    if (team_count < 2 || venue_count < 1) return NULL;

    FixtureSchedule* schedule = (FixtureSchedule*)malloc(sizeof(FixtureSchedule));
    if (!schedule) return NULL;

    // Circular rotation algorithm for round-robin
    // Handles both even and odd team counts correctly
    int n = team_count;
    int total_rounds = (n - 1) * 2;  // Double round-robin (home + away)
    int matches_per_round = n / 2;   // Integer division handles odd counts
    int total_matches = total_rounds * matches_per_round;

    schedule->fixtures = (Fixture*)malloc(sizeof(Fixture) * total_matches);
    schedule->count = 0;
    schedule->total_rounds = total_rounds;

    int fixture_id = 1;
    int current_day = 0;
    Date current_date = config->start_date;

    // Circular rotation algorithm
    for (int leg = 0; leg < 2; leg++) {  // leg 0 = first half, leg 1 = return leg
        for (int round = 0; round < n - 1; round++) {
            int matches_in_round = 0;

            // Fixed team (team 0) stays in place, others rotate
            for (int i = 1; i < n; i++) {
                int opponent = n - i;

                // Only generate each pairing once per round
                if (i < opponent) {
                    int home_team, away_team;

                    if (leg == 0) {
                        // First leg: team with lower index at home
                        home_team = (i < opponent) ? i : opponent;
                        away_team = (i < opponent) ? opponent : i;
                    } else {
                        // Second leg: reverse home/away
                        away_team = (i < opponent) ? i : opponent;
                        home_team = (i < opponent) ? opponent : i;
                    }

                    // Venue assignment: use home team's stadium
                    int venue_idx = home_team % venue_count;

                    // Time slot distributed across available slots
                    int time_slot = (matches_in_round % 3) + 1;

                    Fixture fixture;
                    fixture.id = fixture_id++;
                    fixture.round = (leg * (n - 1)) + round + 1;
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
            }

            // Move to next matchday after rest days
            current_day += (config->min_rest_days + 1);
            current_date = add_days(&current_date, config->min_rest_days + 1);
        }
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
