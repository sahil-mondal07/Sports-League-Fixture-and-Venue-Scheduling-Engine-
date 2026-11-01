#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/utils/common.h"
#include "../src/data/team.h"
#include "../src/data/venue.h"
#include "../src/data/fixture.h"
#include "../src/scheduling/validator.h"
#include "../src/algorithms/greedy/round_robin.h"

// ============================================================================
// TEST 1: Valid schedules should produce zero violations
// ============================================================================
int test_valid_schedules() {
    printf("\n========================================================\n");
    printf("TEST 1: VALID SCHEDULES\n");
    printf("========================================================\n");
    int all_pass = 1;

    for (int n = 2; n <= 20; n++) {
        Team* teams = (Team*)malloc(sizeof(Team) * n);
        Venue* venues = (Venue*)malloc(sizeof(Venue) * n);

        for (int i = 0; i < n; i++) {
            teams[i].id = i;
            sprintf(teams[i].name, "Team%d", i);
            teams[i].ranking = i + 1;
            teams[i].points = 0;

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
            printf("FAIL: n=%d - Schedule generation failed\n", n);
            all_pass = 0;
            free(teams);
            free(venues);
            continue;
        }

        ValidationOptions opts;
        opts.min_capacity = 50000;
        opts.require_full_double_rr = 1;

        ValidationReport report;
        report.items = NULL;
        report.count = 0;
        report.capacity = 0;

        int violations = validate_schedule(schedule->fixtures, schedule->count, n,
                                           venues, n, &config, &opts, &report);

        if (violations != 0) {
            printf("FAIL: n=%d - Expected 0 violations, got %d\n", n, violations);
            printf("First violation: %s\n", report.items[0].message);
            all_pass = 0;
        } else {
            printf("PASS: n=%d - Zero violations\n", n);
        }

        free_validation_report(&report);
        free_fixture_schedule(schedule);
        free(teams);
        free(venues);
    }

    return all_pass;
}

// ============================================================================
// TEST 2: Team plays twice in same round
// ============================================================================
int test_team_twice_in_round() {
    printf("\n========================================================\n");
    printf("TEST 2: TEAM PLAYS TWICE IN SAME ROUND\n");
    printf("========================================================\n");

    int n = 4;
    Team* teams = (Team*)malloc(sizeof(Team) * n);
    Venue* venues = (Venue*)malloc(sizeof(Venue) * n);

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

    // Duplicate the first fixture to create a conflict
    Fixture* fixtures_copy = (Fixture*)malloc(sizeof(Fixture) * (schedule->count + 1));
    for (int i = 0; i < schedule->count; i++) {
        fixtures_copy[i] = schedule->fixtures[i];
    }
    fixtures_copy[schedule->count] = schedule->fixtures[0];  // Duplicate

    ValidationOptions opts;
    opts.min_capacity = 50000;
    opts.require_full_double_rr = 1;

    ValidationReport report;
    report.items = NULL;
    report.count = 0;
    report.capacity = 0;

    int violations = validate_schedule(fixtures_copy, schedule->count + 1, n,
                                       venues, n, &config, &opts, &report);

    int pass = 0;
    for (int i = 0; i < report.count; i++) {
        if (report.items[i].type == VIOL_DUPLICATE) {
            printf("PASS: Duplicate detected - %s\n", report.items[i].message);
            pass = 1;
            break;
        }
    }

    if (!pass) {
        printf("FAIL: Expected VIOL_DUPLICATE, got %d violations\n", violations);
    }

    free_validation_report(&report);
    free(fixtures_copy);
    free_fixture_schedule(schedule);
    free(teams);
    free(venues);
    return pass;
}

// ============================================================================
// TEST 3: Self-match (team plays itself)
// ============================================================================
int test_self_match() {
    printf("\n========================================================\n");
    printf("TEST 3: SELF-MATCH\n");
    printf("========================================================\n");

    Team* teams = (Team*)malloc(sizeof(Team) * 2);
    Venue* venues = (Venue*)malloc(sizeof(Venue) * 2);

    for (int i = 0; i < 2; i++) {
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

    // Create a self-match fixture
    Fixture fixtures[1];
    fixtures[0].id = 1;
    fixtures[0].round = 1;
    fixtures[0].home_team_id = 0;
    fixtures[0].away_team_id = 0;  // SELF-MATCH
    fixtures[0].venue_id = 0;
    fixtures[0].day = 0;
    fixtures[0].match_date = config.start_date;
    fixtures[0].time_slot = 1;
    fixtures[0].travel_cost = 0.0;
    fixtures[0].is_valid = TRUE;

    ValidationOptions opts;
    opts.min_capacity = 50000;
    opts.require_full_double_rr = 0;

    ValidationReport report;
    report.items = NULL;
    report.count = 0;
    report.capacity = 0;

    int violations = validate_schedule(fixtures, 1, 2, venues, 2, &config, &opts, &report);

    int pass = 0;
    for (int i = 0; i < report.count; i++) {
        if (report.items[i].type == VIOL_SELF_MATCH) {
            printf("PASS: Self-match detected - %s\n", report.items[i].message);
            pass = 1;
            break;
        }
    }

    if (!pass) {
        printf("FAIL: Expected VIOL_SELF_MATCH\n");
    }

    free_validation_report(&report);
    free(teams);
    free(venues);
    return pass;
}

// ============================================================================
// TEST 4: Venue clash (same venue, day, slot)
// ============================================================================
int test_venue_clash() {
    printf("\n========================================================\n");
    printf("TEST 4: VENUE CLASH\n");
    printf("========================================================\n");

    Team* teams = (Team*)malloc(sizeof(Team) * 4);
    Venue* venues = (Venue*)malloc(sizeof(Venue) * 2);

    for (int i = 0; i < 4; i++) {
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

    // Create two fixtures at same venue, day, slot
    Fixture fixtures[2];
    fixtures[0].id = 1;
    fixtures[0].round = 1;
    fixtures[0].home_team_id = 0;
    fixtures[0].away_team_id = 1;
    fixtures[0].venue_id = 0;
    fixtures[0].day = 0;
    fixtures[0].match_date = config.start_date;
    fixtures[0].time_slot = 1;
    fixtures[0].is_valid = TRUE;

    fixtures[1].id = 2;
    fixtures[1].round = 1;
    fixtures[1].home_team_id = 2;
    fixtures[1].away_team_id = 3;
    fixtures[1].venue_id = 0;  // SAME VENUE
    fixtures[1].day = 0;       // SAME DAY
    fixtures[1].match_date = config.start_date;
    fixtures[1].time_slot = 1; // SAME SLOT
    fixtures[1].is_valid = TRUE;

    ValidationOptions opts;
    opts.min_capacity = 50000;
    opts.require_full_double_rr = 0;

    ValidationReport report;
    report.items = NULL;
    report.count = 0;
    report.capacity = 0;

    int violations = validate_schedule(fixtures, 2, 4, venues, 2, &config, &opts, &report);

    int pass = 0;
    for (int i = 0; i < report.count; i++) {
        if (report.items[i].type == VIOL_VENUE_CLASH) {
            printf("PASS: Venue clash detected - %s\n", report.items[i].message);
            pass = 1;
            break;
        }
    }

    if (!pass) {
        printf("FAIL: Expected VIOL_VENUE_CLASH\n");
    }

    free_validation_report(&report);
    free(teams);
    free(venues);
    return pass;
}

// ============================================================================
// TEST 5: NULL arguments
// ============================================================================
int test_null_arguments() {
    printf("\n========================================================\n");
    printf("TEST 5: NULL ARGUMENTS\n");
    printf("========================================================\n");

    ValidationReport report;
    report.items = NULL;
    report.count = 0;
    report.capacity = 0;

    int result = validate_schedule(NULL, 0, 0, NULL, 0, NULL, NULL, &report);

    if (result == -1) {
        printf("PASS: Returned -1 for NULL arguments\n");
        return 1;
    } else {
        printf("FAIL: Expected -1, got %d\n", result);
        return 0;
    }
}

// ============================================================================
// TEST 6: apply_validation_flags
// ============================================================================
int test_apply_validation_flags() {
    printf("\n========================================================\n");
    printf("TEST 6: APPLY VALIDATION FLAGS\n");
    printf("========================================================\n");

    int n = 4;
    Team* teams = (Team*)malloc(sizeof(Team) * n);
    Venue* venues = (Venue*)malloc(sizeof(Venue) * n);

    for (int i = 0; i < n; i++) {
        teams[i].id = i;
        sprintf(teams[i].name, "Team%d", i);
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

    // Create a bad fixture and add it
    Fixture* fixtures_bad = (Fixture*)malloc(sizeof(Fixture) * (schedule->count + 1));
    for (int i = 0; i < schedule->count; i++) {
        fixtures_bad[i] = schedule->fixtures[i];
    }
    fixtures_bad[schedule->count].id = 999;
    fixtures_bad[schedule->count].home_team_id = 0;
    fixtures_bad[schedule->count].away_team_id = 1;
    fixtures_bad[schedule->count].venue_id = 0;
    fixtures_bad[schedule->count].day = 0;
    fixtures_bad[schedule->count].time_slot = 1;
    fixtures_bad[schedule->count].is_valid = TRUE;

    ValidationOptions opts;
    opts.min_capacity = 50000;
    opts.require_full_double_rr = 0;

    ValidationReport report;
    report.items = NULL;
    report.count = 0;
    report.capacity = 0;

    validate_schedule(fixtures_bad, schedule->count + 1, n, venues, n, &config, &opts, &report);

    // Apply flags
    apply_validation_flags(fixtures_bad, schedule->count + 1, &report);

    // Check that bad fixture is marked invalid
    int pass = !fixtures_bad[schedule->count].is_valid ? 1 : 0;

    if (pass) {
        printf("PASS: apply_validation_flags marked bad fixture as invalid\n");
    } else {
        printf("FAIL: Bad fixture not marked as invalid\n");
    }

    free_validation_report(&report);
    free(fixtures_bad);
    free_fixture_schedule(schedule);
    free(teams);
    free(venues);
    return pass;
}

int main() {
    int pass_count = 0;
    int fail_count = 0;

    printf("\n");
    printf("================================\n");
    printf("VALIDATOR COMPREHENSIVE TESTS\n");
    printf("================================\n");

    if (!test_valid_schedules()) fail_count++;
    else pass_count++;

    if (!test_team_twice_in_round()) fail_count++;
    else pass_count++;

    if (!test_self_match()) fail_count++;
    else pass_count++;

    if (!test_venue_clash()) fail_count++;
    else pass_count++;

    if (!test_null_arguments()) fail_count++;
    else pass_count++;

    if (!test_apply_validation_flags()) fail_count++;
    else pass_count++;

    printf("\n");
    printf("========================================================\n");
    printf("FINAL RESULTS: %d passed, %d failed\n", pass_count, fail_count);
    printf("========================================================\n\n");

    return fail_count > 0 ? 1 : 0;
}
