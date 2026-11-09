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
// TEST 1: Empty schedule
// ============================================================================
int test_empty_schedule() {
    printf("\n========================================================\n");
    printf("TEST 1: EMPTY SCHEDULE\n");
    printf("========================================================\n");

    Venue venues[2];
    venues[0].id = 0;
    venues[0].capacity = 100000;
    venues[0].is_available = TRUE;
    venues[1].id = 1;
    venues[1].capacity = 100000;
    venues[1].is_available = TRUE;

    TournamentConfig config;
    config.start_date.day = 1;
    config.start_date.month = 1;
    config.start_date.year = 2026;
    config.end_date.day = 31;
    config.end_date.month = 12;
    config.end_date.year = 2026;
    config.min_rest_days = 1;
    config.matches_per_week = 2;

    ValidationOptions opts;
    opts.min_capacity = 0;
    opts.require_full_double_rr = 0;

    ValidationReport report;
    report.items = NULL;
    report.count = 0;
    report.capacity = 0;

    int violations = validate_schedule(NULL, 0, 2, venues, 2, &config, &opts, &report);

    if (violations == 0) {
        printf("PASS: Empty schedule returns 0 violations\n");
        return 1;
    } else {
        printf("FAIL: Expected 0 violations, got %d\n", violations);
        free_validation_report(&report);
        return 0;
    }
}

// ============================================================================
// TEST 2: Out-of-range team ID
// ============================================================================
int test_out_of_range_team_id() {
    printf("\n========================================================\n");
    printf("TEST 2: OUT-OF-RANGE TEAM ID\n");
    printf("========================================================\n");

    Team teams[2];
    teams[0].id = 0;
    teams[1].id = 1;

    Venue venues[2];
    venues[0].id = 0;
    venues[0].capacity = 100000;
    venues[0].is_available = TRUE;
    venues[1].id = 1;
    venues[1].capacity = 100000;
    venues[1].is_available = TRUE;

    TournamentConfig config;
    config.start_date.day = 1;
    config.start_date.month = 1;
    config.start_date.year = 2026;
    config.end_date.day = 31;
    config.end_date.month = 12;
    config.end_date.year = 2026;
    config.min_rest_days = 1;
    config.matches_per_week = 2;

    // Create fixture with invalid team ID
    Fixture fixtures[1];
    fixtures[0].id = 1;
    fixtures[0].round = 1;
    fixtures[0].home_team_id = 5;  // INVALID - out of range
    fixtures[0].away_team_id = 1;
    fixtures[0].venue_id = 0;
    fixtures[0].day = 0;
    fixtures[0].match_date = config.start_date;
    fixtures[0].time_slot = 1;
    fixtures[0].travel_cost = 0.0;
    fixtures[0].is_valid = TRUE;

    ValidationOptions opts;
    opts.min_capacity = 0;
    opts.require_full_double_rr = 0;

    ValidationReport report;
    report.items = NULL;
    report.count = 0;
    report.capacity = 0;

    int violations = validate_schedule(fixtures, 1, 2, venues, 2, &config, &opts, &report);

    int pass = 0;
    for (int i = 0; i < report.count; i++) {
        if (report.items[i].type == VIOL_BAD_ID) {
            printf("PASS: Out-of-range team ID detected - %s\n", report.items[i].message);
            pass = 1;
            break;
        }
    }

    if (!pass) {
        printf("FAIL: Expected VIOL_BAD_ID\n");
    }

    free_validation_report(&report);
    return pass;
}

// ============================================================================
// TEST 3: Out-of-range venue ID
// ============================================================================
int test_out_of_range_venue_id() {
    printf("\n========================================================\n");
    printf("TEST 3: OUT-OF-RANGE VENUE ID\n");
    printf("========================================================\n");

    Team teams[2];
    teams[0].id = 0;
    teams[1].id = 1;

    Venue venues[2];
    venues[0].id = 0;
    venues[0].capacity = 100000;
    venues[0].is_available = TRUE;
    venues[1].id = 1;
    venues[1].capacity = 100000;
    venues[1].is_available = TRUE;

    TournamentConfig config;
    config.start_date.day = 1;
    config.start_date.month = 1;
    config.start_date.year = 2026;
    config.end_date.day = 31;
    config.end_date.month = 12;
    config.end_date.year = 2026;
    config.min_rest_days = 1;
    config.matches_per_week = 2;

    // Create fixture with invalid venue ID
    Fixture fixtures[1];
    fixtures[0].id = 1;
    fixtures[0].round = 1;
    fixtures[0].home_team_id = 0;
    fixtures[0].away_team_id = 1;
    fixtures[0].venue_id = 10;  // INVALID - out of range
    fixtures[0].day = 0;
    fixtures[0].match_date = config.start_date;
    fixtures[0].time_slot = 1;
    fixtures[0].travel_cost = 0.0;
    fixtures[0].is_valid = TRUE;

    ValidationOptions opts;
    opts.min_capacity = 0;
    opts.require_full_double_rr = 0;

    ValidationReport report;
    report.items = NULL;
    report.count = 0;
    report.capacity = 0;

    int violations = validate_schedule(fixtures, 1, 2, venues, 2, &config, &opts, &report);

    int pass = 0;
    for (int i = 0; i < report.count; i++) {
        if (report.items[i].type == VIOL_BAD_ID) {
            printf("PASS: Out-of-range venue ID detected - %s\n", report.items[i].message);
            pass = 1;
            break;
        }
    }

    if (!pass) {
        printf("FAIL: Expected VIOL_BAD_ID\n");
    }

    free_validation_report(&report);
    return pass;
}

// ============================================================================
// TEST 4: n=80 schedule with violation in round 140
// ============================================================================
int test_large_schedule_violation() {
    printf("\n========================================================\n");
    printf("TEST 4: N=80 SCHEDULE WITH VIOLATION IN ROUND 140\n");
    printf("========================================================\n");

    int n = 80;
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
    config.end_date.year = 2027;  // Long tournament
    config.min_rest_days = 1;
    config.matches_per_week = 2;

    FixtureSchedule* schedule = round_robin_schedule(teams, n, venues, n, &config);

    if (!schedule) {
        printf("FAIL: Schedule generation failed\n");
        free(teams);
        free(venues);
        return 0;
    }

    printf("Generated %d fixtures for n=%d\n", schedule->count, n);

    // Find fixtures in round 140 and create a duplicate (team plays twice)
    int found_round = -1;
    int found_idx = -1;
    for (int i = 0; i < schedule->count; i++) {
        if (schedule->fixtures[i].round == 140) {
            found_round = 140;
            found_idx = i;
            break;
        }
    }

    if (found_idx == -1) {
        // Try to find a high round
        int max_round = 0;
        for (int i = 0; i < schedule->count; i++) {
            if (schedule->fixtures[i].round > max_round) {
                max_round = schedule->fixtures[i].round;
                found_idx = i;
            }
        }
        found_round = max_round;
        printf("Note: Using round %d instead (max in schedule)\n", found_round);
    }

    // Add a duplicate fixture to create violation
    Fixture* fixtures_dup = (Fixture*)malloc(sizeof(Fixture) * (schedule->count + 1));
    for (int i = 0; i < schedule->count; i++) {
        fixtures_dup[i] = schedule->fixtures[i];
    }
    fixtures_dup[schedule->count] = schedule->fixtures[found_idx];  // Duplicate
    fixtures_dup[schedule->count].id = 99999;

    ValidationOptions opts;
    opts.min_capacity = 0;
    opts.require_full_double_rr = 0;

    ValidationReport report;
    report.items = NULL;
    report.count = 0;
    report.capacity = 0;

    int violations = validate_schedule(fixtures_dup, schedule->count + 1, n, venues, n, &config, &opts, &report);

    int pass = 0;
    for (int i = 0; i < report.count; i++) {
        if (report.items[i].type == VIOL_TEAM_TWICE_IN_ROUND ||
            report.items[i].type == VIOL_DUPLICATE) {
            printf("PASS: Violation in round %d detected - %s\n", found_round, report.items[i].message);
            pass = 1;
            break;
        }
    }

    if (!pass) {
        printf("FAIL: Expected VIOL_TEAM_TWICE_IN_ROUND or VIOL_DUPLICATE\n");
    }

    free_validation_report(&report);
    free(fixtures_dup);
    free_fixture_schedule(schedule);
    free(teams);
    free(venues);
    return pass;
}

// ============================================================================
// TEST 5: 1000+ violations in one report
// ============================================================================
int test_1000_plus_violations() {
    printf("\n========================================================\n");
    printf("TEST 5: 1000+ VIOLATIONS IN ONE REPORT\n");
    printf("========================================================\n");

    int n = 10;
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

    // Create 1100 fixtures with violations (all at same venue, same time)
    int fixture_count = 1100;
    Fixture* fixtures = (Fixture*)malloc(sizeof(Fixture) * fixture_count);

    for (int i = 0; i < fixture_count; i++) {
        fixtures[i].id = i;
        fixtures[i].round = (i / 5) + 1;
        fixtures[i].home_team_id = i % n;
        fixtures[i].away_team_id = (i + 1) % n;
        fixtures[i].venue_id = 0;  // All same venue
        fixtures[i].day = i % 10;  // Only 10 different days
        fixtures[i].match_date = config.start_date;
        fixtures[i].time_slot = 1;  // All same slot
        fixtures[i].travel_cost = 0.0;
        fixtures[i].is_valid = TRUE;
    }

    ValidationOptions opts;
    opts.min_capacity = 0;
    opts.require_full_double_rr = 0;

    ValidationReport report;
    report.items = NULL;
    report.count = 0;
    report.capacity = 0;

    int violations = validate_schedule(fixtures, fixture_count, n, venues, 1, &config, &opts, &report);

    if (violations >= 1000) {
        printf("PASS: Generated %d violations (>=1000)\n", violations);
        free_validation_report(&report);
        free(fixtures);
        free(teams);
        free(venues);
        return 1;
    } else {
        printf("FAIL: Expected >=1000 violations, got %d\n", violations);
        free_validation_report(&report);
        free(fixtures);
        free(teams);
        free(venues);
        return 0;
    }
}

// ============================================================================
// TEST 6: Self-match detection
// ============================================================================
int test_self_match() {
    printf("\n========================================================\n");
    printf("TEST 6: SELF-MATCH\n");
    printf("========================================================\n");

    Team teams[2];
    teams[0].id = 0;
    teams[1].id = 1;

    Venue venues[2];
    venues[0].id = 0;
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
    opts.min_capacity = 0;
    opts.require_full_double_rr = 0;

    ValidationReport report;
    report.items = NULL;
    report.count = 0;
    report.capacity = 0;

    validate_schedule(fixtures, 1, 2, venues, 1, &config, &opts, &report);

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
    return pass;
}

// ============================================================================
// TEST 7: Venue clash
// ============================================================================
int test_venue_clash() {
    printf("\n========================================================\n");
    printf("TEST 7: VENUE CLASH\n");
    printf("========================================================\n");

    Team teams[4];
    for (int i = 0; i < 4; i++) {
        teams[i].id = i;
    }

    Venue venues[2];
    venues[0].id = 0;
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
    opts.min_capacity = 0;
    opts.require_full_double_rr = 0;

    ValidationReport report;
    report.items = NULL;
    report.count = 0;
    report.capacity = 0;

    validate_schedule(fixtures, 2, 4, venues, 1, &config, &opts, &report);

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
    return pass;
}

// ============================================================================
// TEST 8: Rest days violation
// ============================================================================
int test_rest_days() {
    printf("\n========================================================\n");
    printf("TEST 8: REST DAYS VIOLATION\n");
    printf("========================================================\n");

    Team teams[2];
    teams[0].id = 0;
    teams[1].id = 1;

    Venue venues[2];
    venues[0].id = 0;
    venues[0].capacity = 100000;
    venues[0].is_available = TRUE;
    venues[1].id = 1;
    venues[1].capacity = 100000;
    venues[1].is_available = TRUE;

    TournamentConfig config;
    config.start_date.day = 1;
    config.start_date.month = 1;
    config.start_date.year = 2026;
    config.end_date.day = 31;
    config.end_date.month = 12;
    config.end_date.year = 2026;
    config.min_rest_days = 3;  // Need 3 rest days
    config.matches_per_week = 2;

    // Create fixtures with insufficient rest (day 0 and day 1)
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
    fixtures[1].round = 2;
    fixtures[1].home_team_id = 1;
    fixtures[1].away_team_id = 0;  // Same team plays next day
    fixtures[1].venue_id = 1;
    fixtures[1].day = 1;  // Only 1 day gap (need 3)
    fixtures[1].match_date = config.start_date;
    fixtures[1].time_slot = 1;
    fixtures[1].is_valid = TRUE;

    ValidationOptions opts;
    opts.min_capacity = 0;
    opts.require_full_double_rr = 0;

    ValidationReport report;
    report.items = NULL;
    report.count = 0;
    report.capacity = 0;

    validate_schedule(fixtures, 2, 2, venues, 2, &config, &opts, &report);

    int pass = 0;
    for (int i = 0; i < report.count; i++) {
        if (report.items[i].type == VIOL_REST_DAYS) {
            printf("PASS: Rest days violation detected - %s\n", report.items[i].message);
            pass = 1;
            break;
        }
    }

    if (!pass) {
        printf("FAIL: Expected VIOL_REST_DAYS\n");
    }

    free_validation_report(&report);
    return pass;
}

// ============================================================================
// TEST 9: Venue unavailable
// ============================================================================
int test_venue_unavailable() {
    printf("\n========================================================\n");
    printf("TEST 9: VENUE UNAVAILABLE\n");
    printf("========================================================\n");

    Team teams[2];
    teams[0].id = 0;
    teams[1].id = 1;

    Venue venues[1];
    venues[0].id = 0;
    venues[0].capacity = 100000;
    venues[0].is_available = FALSE;  // NOT AVAILABLE

    TournamentConfig config;
    config.start_date.day = 1;
    config.start_date.month = 1;
    config.start_date.year = 2026;
    config.end_date.day = 31;
    config.end_date.month = 12;
    config.end_date.year = 2026;
    config.min_rest_days = 1;
    config.matches_per_week = 2;

    Fixture fixtures[1];
    fixtures[0].id = 1;
    fixtures[0].round = 1;
    fixtures[0].home_team_id = 0;
    fixtures[0].away_team_id = 1;
    fixtures[0].venue_id = 0;
    fixtures[0].day = 0;
    fixtures[0].match_date = config.start_date;
    fixtures[0].time_slot = 1;
    fixtures[0].is_valid = TRUE;

    ValidationOptions opts;
    opts.min_capacity = 0;
    opts.require_full_double_rr = 0;

    ValidationReport report;
    report.items = NULL;
    report.count = 0;
    report.capacity = 0;

    validate_schedule(fixtures, 1, 2, venues, 1, &config, &opts, &report);

    int pass = 0;
    for (int i = 0; i < report.count; i++) {
        if (report.items[i].type == VIOL_VENUE_UNAVAILABLE) {
            printf("PASS: Venue unavailable detected - %s\n", report.items[i].message);
            pass = 1;
            break;
        }
    }

    if (!pass) {
        printf("FAIL: Expected VIOL_VENUE_UNAVAILABLE\n");
    }

    free_validation_report(&report);
    return pass;
}

// ============================================================================
// TEST 10: Capacity violation
// ============================================================================
int test_capacity() {
    printf("\n========================================================\n");
    printf("TEST 10: CAPACITY VIOLATION\n");
    printf("========================================================\n");

    Team teams[2];
    teams[0].id = 0;
    teams[1].id = 1;

    Venue venues[1];
    venues[0].id = 0;
    venues[0].capacity = 5000;  // Small capacity
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

    Fixture fixtures[1];
    fixtures[0].id = 1;
    fixtures[0].round = 1;
    fixtures[0].home_team_id = 0;
    fixtures[0].away_team_id = 1;
    fixtures[0].venue_id = 0;
    fixtures[0].day = 0;
    fixtures[0].match_date = config.start_date;
    fixtures[0].time_slot = 1;
    fixtures[0].is_valid = TRUE;

    ValidationOptions opts;
    opts.min_capacity = 10000;  // Require 10000
    opts.require_full_double_rr = 0;

    ValidationReport report;
    report.items = NULL;
    report.count = 0;
    report.capacity = 0;

    validate_schedule(fixtures, 1, 2, venues, 1, &config, &opts, &report);

    int pass = 0;
    for (int i = 0; i < report.count; i++) {
        if (report.items[i].type == VIOL_CAPACITY) {
            printf("PASS: Capacity violation detected - %s\n", report.items[i].message);
            pass = 1;
            break;
        }
    }

    if (!pass) {
        printf("FAIL: Expected VIOL_CAPACITY\n");
    }

    free_validation_report(&report);
    return pass;
}

// ============================================================================
// TEST 11: Duplicate pair
// ============================================================================
int test_duplicate() {
    printf("\n========================================================\n");
    printf("TEST 11: DUPLICATE PAIR\n");
    printf("========================================================\n");

    Team teams[3];
    for (int i = 0; i < 3; i++) {
        teams[i].id = i;
    }

    Venue venues[3];
    for (int i = 0; i < 3; i++) {
        venues[i].id = i;
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

    // Create same pair twice
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
    fixtures[1].round = 2;
    fixtures[1].home_team_id = 0;  // SAME PAIR
    fixtures[1].away_team_id = 1;
    fixtures[1].venue_id = 1;
    fixtures[1].day = 10;
    fixtures[1].match_date = config.start_date;
    fixtures[1].time_slot = 1;
    fixtures[1].is_valid = TRUE;

    ValidationOptions opts;
    opts.min_capacity = 0;
    opts.require_full_double_rr = 0;

    ValidationReport report;
    report.items = NULL;
    report.count = 0;
    report.capacity = 0;

    validate_schedule(fixtures, 2, 3, venues, 3, &config, &opts, &report);

    int pass = 0;
    for (int i = 0; i < report.count; i++) {
        if (report.items[i].type == VIOL_DUPLICATE) {
            printf("PASS: Duplicate pair detected - %s\n", report.items[i].message);
            pass = 1;
            break;
        }
    }

    if (!pass) {
        printf("FAIL: Expected VIOL_DUPLICATE\n");
    }

    free_validation_report(&report);
    return pass;
}

// ============================================================================
// TEST 12: Missing pair
// ============================================================================
int test_missing_pair() {
    printf("\n========================================================\n");
    printf("TEST 12: MISSING PAIR\n");
    printf("========================================================\n");

    Team teams[3];
    for (int i = 0; i < 3; i++) {
        teams[i].id = i;
    }

    Venue venues[3];
    for (int i = 0; i < 3; i++) {
        venues[i].id = i;
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

    // Create incomplete schedule (missing pairs)
    Fixture fixtures[1];
    fixtures[0].id = 1;
    fixtures[0].round = 1;
    fixtures[0].home_team_id = 0;
    fixtures[0].away_team_id = 1;
    fixtures[0].venue_id = 0;
    fixtures[0].day = 0;
    fixtures[0].match_date = config.start_date;
    fixtures[0].time_slot = 1;
    fixtures[0].is_valid = TRUE;

    ValidationOptions opts;
    opts.min_capacity = 0;
    opts.require_full_double_rr = 1;  // Require complete

    ValidationReport report;
    report.items = NULL;
    report.count = 0;
    report.capacity = 0;

    validate_schedule(fixtures, 1, 3, venues, 3, &config, &opts, &report);

    int pass = 0;
    for (int i = 0; i < report.count; i++) {
        if (report.items[i].type == VIOL_MISSING) {
            printf("PASS: Missing pair detected - %s\n", report.items[i].message);
            pass = 1;
            break;
        }
    }

    if (!pass) {
        printf("FAIL: Expected VIOL_MISSING\n");
    }

    free_validation_report(&report);
    return pass;
}

// ============================================================================
// TEST 13: Home/away imbalance
// ============================================================================
int test_home_away_imbalance() {
    printf("\n========================================================\n");
    printf("TEST 13: HOME/AWAY IMBALANCE\n");
    printf("========================================================\n");

    Team teams[3];
    for (int i = 0; i < 3; i++) {
        teams[i].id = i;
    }

    Venue venues[3];
    for (int i = 0; i < 3; i++) {
        venues[i].id = i;
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

    // Create fixtures where team 0 is always home
    Fixture fixtures[4];
    for (int i = 0; i < 4; i++) {
        fixtures[i].id = i + 1;
        fixtures[i].round = i + 1;
        fixtures[i].home_team_id = 0;  // Always home
        fixtures[i].away_team_id = (i % 2) + 1;
        fixtures[i].venue_id = 0;
        fixtures[i].day = i * 10;
        fixtures[i].match_date = config.start_date;
        fixtures[i].time_slot = 1;
        fixtures[i].is_valid = TRUE;
    }

    ValidationOptions opts;
    opts.min_capacity = 0;
    opts.require_full_double_rr = 1;

    ValidationReport report;
    report.items = NULL;
    report.count = 0;
    report.capacity = 0;

    validate_schedule(fixtures, 4, 3, venues, 3, &config, &opts, &report);

    int pass = 0;
    for (int i = 0; i < report.count; i++) {
        if (report.items[i].type == VIOL_HOME_AWAY_IMBALANCE) {
            printf("PASS: Home/away imbalance detected - %s\n", report.items[i].message);
            pass = 1;
            break;
        }
    }

    if (!pass) {
        printf("FAIL: Expected VIOL_HOME_AWAY_IMBALANCE\n");
    }

    free_validation_report(&report);
    return pass;
}

// ============================================================================
// TEST 14: Past end date
// ============================================================================
int test_past_end_date() {
    printf("\n========================================================\n");
    printf("TEST 14: PAST END DATE\n");
    printf("========================================================\n");

    Team teams[2];
    teams[0].id = 0;
    teams[1].id = 1;

    Venue venues[1];
    venues[0].id = 0;
    venues[0].capacity = 100000;
    venues[0].is_available = TRUE;

    TournamentConfig config;
    config.start_date.day = 1;
    config.start_date.month = 1;
    config.start_date.year = 2026;
    config.end_date.day = 31;
    config.end_date.month = 1;
    config.end_date.year = 2026;  // Ends Jan 31
    config.min_rest_days = 1;
    config.matches_per_week = 2;

    // Match in March (after end date)
    Fixture fixtures[1];
    fixtures[0].id = 1;
    fixtures[0].round = 1;
    fixtures[0].home_team_id = 0;
    fixtures[0].away_team_id = 1;
    fixtures[0].venue_id = 0;
    fixtures[0].day = 0;
    fixtures[0].match_date.day = 15;
    fixtures[0].match_date.month = 3;  // March - after end
    fixtures[0].match_date.year = 2026;
    fixtures[0].time_slot = 1;
    fixtures[0].is_valid = TRUE;

    ValidationOptions opts;
    opts.min_capacity = 0;
    opts.require_full_double_rr = 0;

    ValidationReport report;
    report.items = NULL;
    report.count = 0;
    report.capacity = 0;

    validate_schedule(fixtures, 1, 2, venues, 1, &config, &opts, &report);

    int pass = 0;
    for (int i = 0; i < report.count; i++) {
        if (report.items[i].type == VIOL_PAST_END_DATE) {
            printf("PASS: Past end date detected - %s\n", report.items[i].message);
            pass = 1;
            break;
        }
    }

    if (!pass) {
        printf("FAIL: Expected VIOL_PAST_END_DATE\n");
    }

    free_validation_report(&report);
    return pass;
}

// ============================================================================
// TEST 15: Team plays twice in round
// ============================================================================
int test_team_twice_in_round() {
    printf("\n========================================================\n");
    printf("TEST 15: TEAM TWICE IN ROUND\n");
    printf("========================================================\n");

    int n = 4;
    Team* teams = (Team*)malloc(sizeof(Team) * n);
    Venue* venues = (Venue*)malloc(sizeof(Venue) * n);

    for (int i = 0; i < n; i++) {
        teams[i].id = i;
        sprintf(teams[i].name, "Team%d", i);
        teams[i].ranking = i + 1;
        teams[i].points = 0;

        venues[i].id = i;
        sprintf(venues[i].name, "Venue%d", i);
        venues[i].capacity = 50000;
        venues[i].is_available = TRUE;
        venues[i].latitude = 0.0f;
        venues[i].longitude = 0.0f;
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

    FixtureSchedule* sched = round_robin_schedule(teams, n, venues, n, &config);
    if (!sched || sched->count < 2) {
        printf("FAIL: Failed to generate valid schedule\n");
        free(teams);
        free(venues);
        if (sched) free_fixture_schedule(sched);
        return 0;
    }

    /* Make team 0 play twice in round 1 */
    int round1_count = 0;
    for (int i = 0; i < sched->count; i++) {
        if (sched->fixtures[i].round == 1) {
            round1_count++;
            if (round1_count == 2) {
                sched->fixtures[i].home_team_id = 0;
                break;
            }
        }
    }

    ValidationOptions opts;
    opts.min_capacity = 0;
    opts.require_full_double_rr = 0;

    ValidationReport report;
    report.items = NULL;
    report.count = 0;
    report.capacity = 0;

    validate_schedule(sched->fixtures, sched->count, n, venues, n, &config, &opts, &report);

    int pass = 0;
    for (int i = 0; i < report.count; i++) {
        if (report.items[i].type == VIOL_TEAM_TWICE_IN_ROUND) {
            printf("PASS: Team twice in round detected - %s\n", report.items[i].message);
            pass = 1;
            break;
        }
    }

    if (!pass) {
        printf("FAIL: Expected VIOL_TEAM_TWICE_IN_ROUND\n");
    }

    free_validation_report(&report);
    free_fixture_schedule(sched);
    free(teams);
    free(venues);
    return pass;
}

// ============================================================================
// MAIN
// ============================================================================
int main() {
    int pass_count = 0;
    int fail_count = 0;

    printf("\n");
    printf("================================\n");
    printf("VALIDATOR COMPREHENSIVE TESTS\n");
    printf("================================\n");

    if (!test_empty_schedule()) fail_count++; else pass_count++;
    if (!test_out_of_range_team_id()) fail_count++; else pass_count++;
    if (!test_out_of_range_venue_id()) fail_count++; else pass_count++;
    if (!test_large_schedule_violation()) fail_count++; else pass_count++;
    if (!test_1000_plus_violations()) fail_count++; else pass_count++;
    if (!test_self_match()) fail_count++; else pass_count++;
    if (!test_venue_clash()) fail_count++; else pass_count++;
    if (!test_rest_days()) fail_count++; else pass_count++;
    if (!test_venue_unavailable()) fail_count++; else pass_count++;
    if (!test_capacity()) fail_count++; else pass_count++;
    if (!test_duplicate()) fail_count++; else pass_count++;
    if (!test_missing_pair()) fail_count++; else pass_count++;
    if (!test_home_away_imbalance()) fail_count++; else pass_count++;
    if (!test_past_end_date()) fail_count++; else pass_count++;
    if (!test_team_twice_in_round()) fail_count++; else pass_count++;

    printf("\n");
    printf("========================================================\n");
    printf("FINAL RESULTS: %d passed, %d failed\n", pass_count, fail_count);
    printf("========================================================\n\n");

    return fail_count > 0 ? 1 : 0;
}
