#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/utils/common.h"
#include "../src/data/team.h"
#include "../src/data/venue.h"
#include "../src/data/fixture.h"
#include "../src/algorithms/greedy/round_robin.h"

// ============================================================================
// TEST 1: No time slot collisions with adequate venues
// ============================================================================
int test_no_collisions_adequate_venues() {
    printf("\n========================================================\n");
    printf("TEST 1: NO COLLISIONS (adequate venues)\n");
    printf("========================================================\n");

    int n = 6;  // 3 matches per round
    Team* teams = (Team*)malloc(sizeof(Team) * n);
    Venue* venues = (Venue*)malloc(sizeof(Venue) * n);  // 6 venues for 6 teams

    for (int i = 0; i < n; i++) {
        teams[i].id = i;
        sprintf(teams[i].name, "Team%d", i);
        teams[i].ranking = i + 1;

        venues[i].id = i;
        sprintf(venues[i].name, "Venue%d", i);
        venues[i].capacity = 100000;
        venues[i].is_available = TRUE;
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

    FixtureSchedule* schedule = round_robin_schedule(teams, n, venues, n, &config);

    if (!schedule) {
        printf("FAIL: Schedule generation failed\n");
        free(teams);
        free(venues);
        return 0;
    }

    // Check for collisions
    int collision_count = 0;
    for (int i = 0; i < schedule->count; i++) {
        for (int j = i + 1; j < schedule->count; j++) {
            Fixture* f1 = &schedule->fixtures[i];
            Fixture* f2 = &schedule->fixtures[j];

            if (f1->venue_id == f2->venue_id &&
                f1->day == f2->day &&
                f1->time_slot == f2->time_slot) {
                printf("COLLISION: Fixture %d and %d at Venue %d, Day %d, Slot %d\n",
                       f1->id, f2->id, f1->venue_id, f1->day, f1->time_slot);
                collision_count++;
            }
        }
    }

    if (collision_count == 0) {
        printf("PASS: No collisions found in %d fixtures\n", schedule->count);
        free_fixture_schedule(schedule);
        free(teams);
        free(venues);
        return 1;
    } else {
        printf("FAIL: Found %d collisions\n", collision_count);
        free_fixture_schedule(schedule);
        free(teams);
        free(venues);
        return 0;
    }
}

// ============================================================================
// TEST 2: Slot distribution across multiple venues
// ============================================================================
int test_slot_distribution() {
    printf("\n========================================================\n");
    printf("TEST 2: SLOT DISTRIBUTION\n");
    printf("========================================================\n");

    int n = 6;  // 3 matches per round
    Team* teams = (Team*)malloc(sizeof(Team) * n);
    Venue* venues = (Venue*)malloc(sizeof(Venue) * 2);  // Only 2 venues

    for (int i = 0; i < n; i++) {
        teams[i].id = i;
        sprintf(teams[i].name, "Team%d", i);
        teams[i].ranking = i + 1;
    }

    for (int i = 0; i < 2; i++) {
        venues[i].id = i;
        sprintf(venues[i].name, "Venue%d", i);
        venues[i].capacity = 100000;
        venues[i].is_available = TRUE;
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

    FixtureSchedule* schedule = round_robin_schedule(teams, n, venues, 2, &config);

    if (!schedule) {
        printf("FAIL: Schedule generation failed\n");
        free(teams);
        free(venues);
        return 0;
    }

    // Check slot usage per venue per round
    int max_slots_used = 0;
    for (int round = 1; round <= 10; round++) {
        for (int v = 0; v < 2; v++) {
            int slots_used = 0;
            for (int i = 0; i < schedule->count; i++) {
                if (schedule->fixtures[i].round == round &&
                    schedule->fixtures[i].venue_id == v) {
                    slots_used++;
                }
            }
            if (slots_used > max_slots_used) {
                max_slots_used = slots_used;
            }
            if (slots_used > 3) {
                printf("FAIL: Round %d, Venue %d uses %d slots (max 3)\n",
                       round, v, slots_used);
                free_fixture_schedule(schedule);
                free(teams);
                free(venues);
                return 0;
            }
        }
    }

    printf("PASS: Max slots used per venue per round: %d (limit 3)\n", max_slots_used);
    free_fixture_schedule(schedule);
    free(teams);
    free(venues);
    return 1;
}

// ============================================================================
// TEST 3: Overload detection - not enough venues for all matches
// ============================================================================
int test_overload_detection() {
    printf("\n========================================================\n");
    printf("TEST 3: OVERLOAD DETECTION\n");
    printf("========================================================\n");

    int n = 10;  // 5 matches per round (impossible with 1 venue, 3 slots)
    Team* teams = (Team*)malloc(sizeof(Team) * n);
    Venue* venues = (Venue*)malloc(sizeof(Venue) * 1);

    for (int i = 0; i < n; i++) {
        teams[i].id = i;
        sprintf(teams[i].name, "Team%d", i);
        teams[i].ranking = i + 1;
    }

    venues[0].id = 0;
    sprintf(venues[0].name, "OnlyVenue");
    venues[0].capacity = 100000;
    venues[0].is_available = TRUE;

    TournamentConfig config;
    config.start_date.day = 1;
    config.start_date.month = 1;
    config.start_date.year = 2026;
    config.end_date.day = 31;
    config.end_date.month = 12;
    config.end_date.year = 2026;
    config.min_rest_days = 1;
    config.matches_per_week = 2;

    FixtureSchedule* schedule = round_robin_schedule(teams, n, venues, 1, &config);

    if (!schedule) {
        printf("PASS: Correctly rejected impossible configuration\n");
        free(teams);
        free(venues);
        return 1;
    } else {
        printf("FAIL: Should have returned NULL for impossible config\n");
        free_fixture_schedule(schedule);
        free(teams);
        free(venues);
        return 0;
    }
}

// ============================================================================
// TEST 4: Minimal venues (2 venues for 4 teams)
// ============================================================================
int test_minimal_venues() {
    printf("\n========================================================\n");
    printf("TEST 4: MINIMAL VENUES (2 venues, 4 teams)\n");
    printf("========================================================\n");

    int n = 4;  // 2 matches per round
    Team* teams = (Team*)malloc(sizeof(Team) * n);
    Venue* venues = (Venue*)malloc(sizeof(Venue) * 2);

    for (int i = 0; i < n; i++) {
        teams[i].id = i;
        sprintf(teams[i].name, "Team%d", i);
        teams[i].ranking = i + 1;
    }

    for (int i = 0; i < 2; i++) {
        venues[i].id = i;
        sprintf(venues[i].name, "Venue%d", i);
        venues[i].capacity = 100000;
        venues[i].is_available = TRUE;
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

    FixtureSchedule* schedule = round_robin_schedule(teams, n, venues, 2, &config);

    if (!schedule) {
        printf("FAIL: Schedule generation failed\n");
        free(teams);
        free(venues);
        return 0;
    }

    // Verify no collisions
    for (int i = 0; i < schedule->count; i++) {
        for (int j = i + 1; j < schedule->count; j++) {
            Fixture* f1 = &schedule->fixtures[i];
            Fixture* f2 = &schedule->fixtures[j];

            if (f1->venue_id == f2->venue_id &&
                f1->day == f2->day &&
                f1->time_slot == f2->time_slot) {
                printf("FAIL: Collision detected\n");
                free_fixture_schedule(schedule);
                free(teams);
                free(venues);
                return 0;
            }
        }
    }

    printf("PASS: No collisions with 2 venues for 4 teams\n");
    free_fixture_schedule(schedule);
    free(teams);
    free(venues);
    return 1;
}

int main() {
    int pass_count = 0;
    int fail_count = 0;

    printf("\n");
    printf("================================\n");
    printf("TIME SLOT COLLISION TESTS\n");
    printf("================================\n");

    if (!test_no_collisions_adequate_venues()) fail_count++;
    else pass_count++;

    if (!test_slot_distribution()) fail_count++;
    else pass_count++;

    if (!test_overload_detection()) fail_count++;
    else pass_count++;

    if (!test_minimal_venues()) fail_count++;
    else pass_count++;

    printf("\n");
    printf("========================================================\n");
    printf("FINAL RESULTS: %d passed, %d failed\n", pass_count, fail_count);
    printf("========================================================\n\n");

    return fail_count > 0 ? 1 : 0;
}
