/* ------------------------------------------------------------
 * GUI Module for Graphify Sports League Scheduler
 * Provides a simple command-line interface for schedule validation
 * ------------------------------------------------------------ */
#include "gui.h"
#include "scheduling/validator.h"
#include <stdio.h>
#include <stdlib.h>

/* ------------------------------------------------------------
 * Display welcome banner
 * ------------------------------------------------------------ */
void gui_welcome_banner(void) {
    printf("========================================\n");
    printf("  Graphify - Sports League Scheduler\n");
    printf("  Validator with Comprehensive Checks\n");
    printf("========================================\n");
}

/* ------------------------------------------------------------
 * Display validation menu
 * ------------------------------------------------------------ */
void gui_display_menu(void) {
    printf("\nValidation Options:\n");
    printf("1. Validate schedule from file\n");
    printf("2. Quick test mode (sample data)\n");
    printf("3. Show this menu\n");
    printf("4. Exit\n");
    printf("Enter choice: ");
}

/* ------------------------------------------------------------
 * Display validation results report
 * ------------------------------------------------------------ */
void gui_display_report(const ValidationReport* report) {
    if (report->count == 0) {
        printf("\nNo violations detected. Schedule is valid!\n");
        return;
    }

    printf("\n=== Validation Results (%d violations) ===\n", report->count);
    for (int i = 0; i < report->count; i++) {
        const Violation* v = &report->items[i];
        printf(" [%d] %s: %s\n",
               i + 1,
               violation_name(v->type),
               v->message);
        if (v->other_fixture_id >= 0) {
            printf("         Other fixture ID: %d\n", v->other_fixture_id);
        }
        if (v->fixture_id >= 0) {
            printf("         Fixture ID: %d\n", v->fixture_id);
        }
    }
    printf("============================================\n");
}

/* ------------------------------------------------------------
 * Run quick test with sample data
 * ------------------------------------------------------------ */
int gui_quick_test(void) {
    Team teams[4];
    for (int i = 0; i < 4; i++) {
        teams[i].id = i;
    }

    Venue venues[2];
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

    Fixture fixtures[4];
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
    fixtures[1].venue_id = 0;
    fixtures[1].day = 0;
    fixtures[1].match_date = config.start_date;
    fixtures[1].time_slot = 1;
    fixtures[1].is_valid = TRUE;

    fixtures[2].id = 3;
    fixtures[2].round = 2;
    fixtures[2].home_team_id = 0;
    fixtures[2].away_team_id = 1;
    fixtures[2].venue_id = 0;
    fixtures[2].day = 0;
    fixtures[2].match_date = config.start_date;
    fixtures[2].time_slot = 1;
    fixtures[2].is_valid = TRUE;

    fixtures[3].id = 4;
    fixtures[3].round = 2;
    fixtures[3].home_team_id = 0;  /* Duplicate: team 0 plays twice in round 2 */
    fixtures[3].away_team_id = 1;
    fixtures[3].venue_id = 1;
    fixtures[3].day = 5;
    fixtures[3].match_date = config.start_date;
    fixtures[3].time_slot = 1;
    fixtures[3].is_valid = TRUE;

    ValidationOptions opts;
    opts.min_capacity = 0;
    opts.require_full_double_rr = 0;

    ValidationReport report;
    report.items = NULL;
    report.count = 0;
    report.capacity = 0;

    int violations = validate_schedule(fixtures, 4, 4, venues, 2, &config, &opts, &report);

    printf("\n=== Quick Test Results ===\n");
    printf("Violations found: %d\n", violations);
    gui_display_report(&report);

    return violations;
}

/* --------------------------------------------------------- ---
 * Main GUI loop
 * ------------------------------------------------------------ */
int gui_main_loop(void) {
    int choice;
    do {
        gui_display_menu();
        if (scanf("%d", &choice) != 1) {
            /* Clear invalid input */
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            choice = 0;
        }

        switch (choice) {
            case 1: {
                /* Validate schedule from file */
                char filepath[256];
                printf("Enter schedule file path: ");
                if (fgets(filepath, sizeof(filepath), stdin) == NULL) {
                    printf("Invalid input.\n");
                    break;
                }
                filepath[strcspn(filepath, "\n")] = 0;

                /* Load fixtures from file using CSV format */
                Fixture fixtures[MAX_FIXTURES];
                int fixture_count = 0;

                FILE* f = fopen(filepath, "r");
                if (!f) {
                    printf("ERROR: Could not open file '%s'\n", filepath);
                    break;
                }

                /* Read fixture data - CSV: id,round,home_team,away_team,venue,day,match_date(dd-mm-yyyy),time_slot */
                int id, round, home_team, away_team, venue, day, time_slot;
                Date match_date;
                while (fscanf(f, "%d,%d,%d,%d,%d,%d,%d-%d-%d,%d\n",
                              &id, &round, &home_team, &away_team,
                              &venue, &day, &match_date.day, &match_date.month, &match_date.year, &time_slot) == 10) {
                    fixtures[fixture_count].id = id;
                    fixtures[fixture_count].round = round;
                    fixtures[fixture_count].home_team_id = home_team;
                    fixtures[fixture_count].away_team_id = away_team;
                    fixtures[fixture_count].venue_id = venue;
                    fixtures[fixture_count].day = day;
                    fixtures[fixture_count].match_date = match_date;
                    fixtures[fixture_count].time_slot = time_slot;
                    fixtures[fixture_count].is_valid = TRUE;
                    fixture_count++;
                    if (fixture_count >= MAX_FIXTURES) break;
                }
                fclose(f);

                /* Set up tournament config for validation */
                TournamentConfig config;
                config.start_date.day = 1;
                config.start_date.month = 1;
                config.start_date.year = 2026;
                config.end_date.day = 31;
                config.end_date.month = 12;
                config.end_date.year = 2026;
                config.min_rest_days = 1;
                config.matches_per_week = 2;

                /* Set up venues (reuse pattern from quick test) */
                Venue venues[2];
                venues[0].id = 0;
                venues[0].capacity = 100000;
                venues[0].is_available = TRUE;
                venues[1].id = 1;
                venues[1].capacity = 100000;
                venues[1].is_available = TRUE;

                /* Set up validation options */
                ValidationOptions opts;
                opts.min_capacity = 0;
                opts.require_full_double_rr = 0;

                /* Run validation using existing C validator */
                ValidationReport report;
                report.items = NULL;
                report.count = 0;
                report.capacity = 0;

                int violations = validate_schedule(fixtures, fixture_count, 4, venues, 2, &config, &opts, &report);

                /* Display results using existing report formatter */
                printf("\n=== Validation Results ===\n");
                printf("Violations found: %d\n", violations);
                gui_display_report(&report);

                break;
            }
            case 2: {
                return gui_quick_test();
            }
            case 3: {
                /* Show menu again */
                break;
            }
            case 4: {
                printf("Exiting... goodbye!\n");
                return 0;
            }
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 4);

    return 0;
}