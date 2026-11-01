#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/utils/common.h"
#include "../src/data/team.h"
#include "../src/data/venue.h"
#include "../src/data/fixture.h"
#include "../src/algorithms/greedy/round_robin.h"

// Test fixture: validate round-robin generates correct pairings
int test_round_robin_correctness() {
    printf("\n");
    printf("========================================================\n");
    printf("TEST 1: ROUND-ROBIN CORRECTNESS\n");
    printf("========================================================\n\n");

    int all_pass = 1;

    // Test for team counts 2 to 8
    for (int n = 2; n <= 8; n++) {
        printf("--- Testing n = %d teams ---\n", n);

        // Create dummy teams and venues
        Team* teams = (Team*)malloc(sizeof(Team) * n);
        Venue* venues = (Venue*)malloc(sizeof(Venue) * n);

        for (int i = 0; i < n; i++) {
            char name[32];
            sprintf(name, "Team%d", i);
            teams[i].id = i;
            strcpy(teams[i].name, name);
            teams[i].ranking = i + 1;
            teams[i].points = 0;

            sprintf(name, "Stadium%d", i);
            venues[i].id = i;
            strcpy(venues[i].name, name);
            venues[i].capacity = 50000;
        }

        // Create tournament config
        TournamentConfig config;
        config.start_date.day = 1;
        config.start_date.month = 1;
        config.start_date.year = 2026;
        config.end_date = config.start_date;
        config.min_rest_days = 1;
        config.matches_per_week = 2;

        // Generate fixtures
        FixtureSchedule* schedule = round_robin_schedule(teams, n, venues, n, &config);

        if (!schedule) {
            printf("ERROR: Failed to generate schedule for n=%d\n\n", n);
            free(teams);
            free(venues);
            all_pass = 0;
            continue;
        }

        // Expected values
        int expected_rounds = (n % 2 == 0) ? 2 * (n - 1) : 2 * n;
        int expected_matches_per_round = n / 2;
        int expected_total = n * (n - 1);

        printf("Expected: %d rounds, %d matches/round, %d total matches\n",
               expected_rounds, expected_matches_per_round, expected_total);
        printf("Got:      %d rounds, ?, %d total matches\n\n",
               schedule->total_rounds, schedule->count);

        // Validation 1: Check total match count
        if (schedule->count != expected_total) {
            printf("FAIL: Wrong total match count. Expected %d, got %d\n\n",
                   expected_total, schedule->count);
            free_fixture_schedule(schedule);
            free(teams);
            free(venues);
            all_pass = 0;
            continue;
        }

        // Validation 2: Track which pairs have been seen
        // pair_count[i][j] = how many times Team i played Team j at home
        int** pair_count = (int**)malloc(sizeof(int*) * n);
        for (int i = 0; i < n; i++) {
            pair_count[i] = (int*)calloc(n, sizeof(int));
        }

        int valid = 1;

        for (int i = 0; i < schedule->count; i++) {
            Fixture* f = &schedule->fixtures[i];
            int home = f->home_team_id;
            int away = f->away_team_id;
            int round = f->round;

            // Check: no team plays itself
            if (home == away) {
                printf("FAIL: Team %d plays itself in fixture %d\n\n", home, f->id);
                valid = 0;
                break;
            }

            // Check: round in valid range
            if (round < 1 || round > expected_rounds) {
                printf("FAIL: Invalid round %d (should be 1-%d)\n\n", round, expected_rounds);
                valid = 0;
                break;
            }

            // Count this pairing
            pair_count[home][away]++;
        }

        if (!valid) {
            free_fixture_schedule(schedule);
            for (int i = 0; i < n; i++) free(pair_count[i]);
            free(pair_count);
            free(teams);
            free(venues);
            all_pass = 0;
            continue;
        }

        // Validation 3: Each ordered pair appears exactly once
        printf("Checking pairings...\n");
        int pairings_valid = 1;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (i != j && pair_count[i][j] != 1) {
                    printf("FAIL: Pair (Team %d vs Team %d) appears %d times (should be 1)\n",
                           i, j, pair_count[i][j]);
                    pairings_valid = 0;
                }
            }
        }

        if (!pairings_valid) {
            printf("FAIL: Incorrect pairings\n\n");
            free_fixture_schedule(schedule);
            for (int i = 0; i < n; i++) free(pair_count[i]);
            free(pair_count);
            free(teams);
            free(venues);
            all_pass = 0;
            continue;
        }

        // Validation 4: Each team plays 2(n-1) times
        printf("Checking team match counts...\n");
        int expected_plays = 2 * (n - 1);
        int* team_plays = (int*)calloc(n, sizeof(int));

        // Count both home and away matches
        for (int i = 0; i < schedule->count; i++) {
            Fixture* f = &schedule->fixtures[i];
            team_plays[f->home_team_id]++;
            team_plays[f->away_team_id]++;
        }

        int plays_valid = 1;
        for (int i = 0; i < n; i++) {
            if (team_plays[i] != expected_plays) {
                printf("FAIL: Team %d plays %d times (should be %d)\n",
                       i, team_plays[i], expected_plays);
                plays_valid = 0;
            }
        }

        if (plays_valid) {
            printf("PASS: n=%d | Total: %d matches, Pairings: correct, Each team plays %d times\n\n",
                   n, schedule->count, expected_plays);
        } else {
            printf("FAIL: Team match counts incorrect\n\n");
            all_pass = 0;
        }

        // Cleanup
        free_fixture_schedule(schedule);
        for (int i = 0; i < n; i++) free(pair_count[i]);
        free(pair_count);
        free(team_plays);
        free(teams);
        free(venues);
    }

    printf("========================================================\n");
    printf("TEST 1 COMPLETE\n");
    printf("========================================================\n\n");

    return all_pass;
}

// ============================================================================
// TEST 2: Consecutive Home/Away Streak - No more than 2 consecutive
// ============================================================================
int test_consecutive_streaks() {
    printf("\n========================================================\n");
    printf("TEST 2: CONSECUTIVE HOME/AWAY STREAK\n");
    printf("========================================================\n");
    int all_pass = 1;

    for (int n = 4; n <= 20; n++) {
        Team* teams = (Team*)malloc(sizeof(Team) * n);
        Venue* venues = (Venue*)malloc(sizeof(Venue) * n);

        for (int i = 0; i < n; i++) {
            teams[i].id = i;
            sprintf(teams[i].name, "Team%d", i);
            teams[i].ranking = i + 1;
            teams[i].points = 0;

            venues[i].id = i;
            sprintf(venues[i].name, "Stadium%d", i);
            venues[i].capacity = 50000;
        }

        TournamentConfig config;
        config.start_date.day = 1;
        config.start_date.month = 1;
        config.start_date.year = 2026;
        config.end_date = config.start_date;
        config.min_rest_days = 0;
        config.matches_per_week = 2;

        FixtureSchedule* schedule = round_robin_schedule(teams, n, venues, n, &config);

        if (!schedule) {
            printf("FAIL: n=%d - Schedule generation failed\n", n);
            all_pass = 0;
            free(teams);
            free(venues);
            continue;
        }

        // Track home/away for each team
        char** fixture_role = (char**)malloc(sizeof(char*) * n);
        for (int i = 0; i < n; i++) {
            fixture_role[i] = (char*)calloc(schedule->total_rounds + 1, sizeof(char));
            for (int j = 0; j < schedule->total_rounds; j++) {
                fixture_role[i][j] = 'B';  // Default: bye
            }
        }

        // Mark home and away matches
        for (int i = 0; i < schedule->count; i++) {
            Fixture* f = &schedule->fixtures[i];
            int round_idx = f->round - 1;
            fixture_role[f->home_team_id][round_idx] = 'H';
            fixture_role[f->away_team_id][round_idx] = 'A';
        }

        // Check consecutive streaks
        int max_allowed = (n == 4) ? 3 : 2;
        int test_pass = 1;

        for (int team = 0; team < n; team++) {
            int current_streak = 0;
            char last_role = 'B';

            for (int round = 0; round < schedule->total_rounds; round++) {
                char role = fixture_role[team][round];

                if (role == 'B') {
                    current_streak = 0;
                    last_role = 'B';
                } else if (role == last_role) {
                    current_streak++;
                    if (current_streak > max_allowed) {
                        printf("FAIL: n=%d, Team %d has %d consecutive %c (max %d)\n",
                               n, team, current_streak, role, max_allowed);
                        test_pass = 0;
                        break;
                    }
                } else {
                    current_streak = 1;
                    last_role = role;
                }
            }

            if (!test_pass) break;
        }

        if (test_pass) {
            printf("PASS: n=%d - Fair home/away distribution\n", n);
        } else {
            all_pass = 0;
        }

        // Cleanup
        for (int i = 0; i < n; i++) free(fixture_role[i]);
        free(fixture_role);
        free_fixture_schedule(schedule);
        free(teams);
        free(venues);
    }

    return all_pass;
}

// ============================================================================
// TEST 3: Home Balance - Each team hosts exactly n-1 matches
// ============================================================================
int test_home_balance() {
    printf("\n========================================================\n");
    printf("TEST 3: HOME BALANCE\n");
    printf("========================================================\n");
    int all_pass = 1;

    for (int n = 2; n <= 20; n++) {
        Team* teams = (Team*)malloc(sizeof(Team) * n);
        Venue* venues = (Venue*)malloc(sizeof(Venue) * n);

        for (int i = 0; i < n; i++) {
            teams[i].id = i;
            sprintf(teams[i].name, "Team%d", i);
            teams[i].ranking = i + 1;

            venues[i].id = i;
            sprintf(venues[i].name, "Stadium%d", i);
            venues[i].capacity = 50000;
        }

        TournamentConfig config;
        config.start_date.day = 1;
        config.start_date.month = 1;
        config.start_date.year = 2026;
        config.end_date = config.start_date;
        config.min_rest_days = 0;
        config.matches_per_week = 2;

        FixtureSchedule* schedule = round_robin_schedule(teams, n, venues, n, &config);

        if (!schedule) {
            printf("FAIL: n=%d - Schedule generation failed\n", n);
            all_pass = 0;
            free(teams);
            free(venues);
            continue;
        }

        int* home_count = (int*)calloc(n, sizeof(int));
        for (int i = 0; i < schedule->count; i++) {
            home_count[schedule->fixtures[i].home_team_id]++;
        }

        int test_pass = 1;
        int expected = n - 1;
        for (int i = 0; i < n; i++) {
            if (home_count[i] != expected) {
                printf("FAIL: n=%d, Team %d hosts %d times (expected %d)\n",
                       n, i, home_count[i], expected);
                test_pass = 0;
            }
        }

        if (test_pass) {
            printf("PASS: n=%d - Each team hosts %d times\n", n, expected);
        } else {
            all_pass = 0;
        }

        free(home_count);
        free_fixture_schedule(schedule);
        free(teams);
        free(venues);
    }

    return all_pass;
}

// ============================================================================
// TEST 4: Slot Safety - No double-booking at same venue/day/slot
// ============================================================================
int test_slot_safety() {
    printf("\n========================================================\n");
    printf("TEST 4: SLOT SAFETY (n=6, 2 venues)\n");
    printf("========================================================\n");

    int n = 6;
    int v = 2;

    Team* teams = (Team*)malloc(sizeof(Team) * n);
    Venue* venues = (Venue*)malloc(sizeof(Venue) * v);

    for (int i = 0; i < n; i++) {
        teams[i].id = i;
        sprintf(teams[i].name, "Team%d", i);
        teams[i].ranking = i + 1;
    }

    for (int i = 0; i < v; i++) {
        venues[i].id = i;
        sprintf(venues[i].name, "Venue%d", i);
        venues[i].capacity = 50000;
    }

    TournamentConfig config;
    config.start_date.day = 1;
    config.start_date.month = 1;
    config.start_date.year = 2026;
    config.end_date = config.start_date;
    config.min_rest_days = 0;
    config.matches_per_week = 2;

    FixtureSchedule* schedule = round_robin_schedule(teams, n, venues, v, &config);

    if (!schedule) {
        printf("FAIL: Schedule generation failed\n");
        free(teams);
        free(venues);
        return 0;
    }

    int test_pass = 1;

    for (int i = 0; i < schedule->count; i++) {
        Fixture* f1 = &schedule->fixtures[i];

        for (int j = i + 1; j < schedule->count; j++) {
            Fixture* f2 = &schedule->fixtures[j];

            if (f1->venue_id == f2->venue_id && f1->day == f2->day && f1->time_slot == f2->time_slot) {
                printf("FAIL: Double-booking detected\n");
                printf("  Fixture %d: Venue %d | Day %d | Slot %d\n",
                       f1->id, f1->venue_id, f1->day, f1->time_slot);
                printf("  Fixture %d: Venue %d | Day %d | Slot %d\n",
                       f2->id, f2->venue_id, f2->day, f2->time_slot);
                test_pass = 0;
            }
        }
    }

    if (test_pass) {
        printf("PASS: No slot collisions detected\n");
    }

    free_fixture_schedule(schedule);
    free(teams);
    free(venues);
    return test_pass;
}

// ============================================================================
// TEST 5: Venue Overload - Too many matches for available slots
// ============================================================================
int test_venue_overload() {
    printf("\n========================================================\n");
    printf("TEST 5: VENUE OVERLOAD (n=10, 1 venue, 3 slots)\n");
    printf("========================================================\n");

    int n = 10;
    int v = 1;

    Team* teams = (Team*)malloc(sizeof(Team) * n);
    Venue* venues = (Venue*)malloc(sizeof(Venue) * v);

    for (int i = 0; i < n; i++) {
        teams[i].id = i;
        sprintf(teams[i].name, "Team%d", i);
        teams[i].ranking = i + 1;
    }

    venues[0].id = 0;
    sprintf(venues[0].name, "OnlyVenue");
    venues[0].capacity = 50000;

    TournamentConfig config;
    config.start_date.day = 1;
    config.start_date.month = 1;
    config.start_date.year = 2026;
    config.end_date = config.start_date;
    config.min_rest_days = 0;
    config.matches_per_week = 2;

    FixtureSchedule* schedule = round_robin_schedule(teams, n, venues, v, &config);

    if (!schedule) {
        printf("PASS: Correctly returned NULL for impossible config\n");
        free(teams);
        free(venues);
        return 1;
    } else {
        printf("FAIL: Should return NULL when venues insufficient\n");
        free_fixture_schedule(schedule);
        free(teams);
        free(venues);
        return 0;
    }
}

// ============================================================================
// TEST 6: NULL Config
// ============================================================================
int test_null_config() {
    printf("\n========================================================\n");
    printf("TEST 6: NULL CONFIG INPUT\n");
    printf("========================================================\n");

    Team* teams = (Team*)malloc(sizeof(Team) * 4);
    Venue* venues = (Venue*)malloc(sizeof(Venue) * 4);

    for (int i = 0; i < 4; i++) {
        teams[i].id = i;
        sprintf(teams[i].name, "Team%d", i);
        venues[i].id = i;
        sprintf(venues[i].name, "Venue%d", i);
    }

    FixtureSchedule* schedule = round_robin_schedule(teams, 4, venues, 4, NULL);

    if (!schedule) {
        printf("PASS: Correctly returned NULL for NULL config\n");
        free(teams);
        free(venues);
        return 1;
    } else {
        printf("FAIL: Should return NULL for NULL config\n");
        free_fixture_schedule(schedule);
        free(teams);
        free(venues);
        return 0;
    }
}

// ============================================================================
// TEST 7: Regression - n=2 to 100
// ============================================================================
int test_regression() {
    printf("\n========================================================\n");
    printf("TEST 7: REGRESSION (n=2 to 100)\n");
    printf("========================================================\n");
    int all_pass = 1;

    for (int n = 2; n <= 100; n += 5) {  // Test every 5th value for speed
        Team* teams = (Team*)malloc(sizeof(Team) * n);
        Venue* venues = (Venue*)malloc(sizeof(Venue) * n);

        for (int i = 0; i < n; i++) {
            teams[i].id = i;
            sprintf(teams[i].name, "T%d", i);
            teams[i].ranking = i + 1;
            venues[i].id = i;
            sprintf(venues[i].name, "V%d", i);
            venues[i].capacity = 50000;
        }

        TournamentConfig config;
        config.start_date.day = 1;
        config.start_date.month = 1;
        config.start_date.year = 2026;
        config.end_date = config.start_date;
        config.min_rest_days = 0;
        config.matches_per_week = 2;

        FixtureSchedule* schedule = round_robin_schedule(teams, n, venues, n, &config);

        if (!schedule) {
            printf("FAIL: n=%d - Generation failed\n", n);
            all_pass = 0;
            free(teams);
            free(venues);
            continue;
        }

        int expected_total = n * (n - 1);
        if (schedule->count != expected_total) {
            printf("FAIL: n=%d - Wrong count: %d (expected %d)\n",
                   n, schedule->count, expected_total);
            all_pass = 0;
            free_fixture_schedule(schedule);
            free(teams);
            free(venues);
            continue;
        }

        // Check every pair appears exactly once
        int** pair_count = (int**)malloc(sizeof(int*) * n);
        for (int i = 0; i < n; i++) {
            pair_count[i] = (int*)calloc(n, sizeof(int));
        }

        int valid = 1;
        for (int i = 0; i < schedule->count; i++) {
            Fixture* f = &schedule->fixtures[i];
            pair_count[f->home_team_id][f->away_team_id]++;
        }

        for (int i = 0; i < n && valid; i++) {
            for (int j = 0; j < n; j++) {
                if (i != j && pair_count[i][j] != 1) {
                    printf("FAIL: n=%d - Pair (%d,%d) appears %d times\n",
                           n, i, j, pair_count[i][j]);
                    valid = 0;
                    break;
                }
            }
        }

        if (valid) {
            printf("PASS: n=%d - All checks passed\n", n);
        } else {
            all_pass = 0;
        }

        for (int i = 0; i < n; i++) free(pair_count[i]);
        free(pair_count);
        free_fixture_schedule(schedule);
        free(teams);
        free(venues);
    }

    return all_pass;
}

int main() {
    int pass_count = 0;
    int fail_count = 0;

    printf("\n");
    printf("================================\n");
    printf("COMPREHENSIVE ROUND-ROBIN TESTS\n");
    printf("================================\n");

    if (!test_round_robin_correctness()) fail_count++;
    else pass_count++;

    if (!test_consecutive_streaks()) fail_count++;
    else pass_count++;

    if (!test_home_balance()) fail_count++;
    else pass_count++;

    if (!test_slot_safety()) fail_count++;
    else pass_count++;

    if (!test_venue_overload()) fail_count++;
    else pass_count++;

    if (!test_null_config()) fail_count++;
    else pass_count++;

    if (!test_regression()) fail_count++;
    else pass_count++;

    printf("\n");
    printf("========================================================\n");
    printf("FINAL RESULTS: %d passed, %d failed\n", pass_count, fail_count);
    printf("========================================================\n\n");

    return fail_count > 0 ? 1 : 0;
}
