#include "validator.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Helper to add violation to report
static int add_violation(ValidationReport* r, ViolationType type, int fixture_id,
                         int other_fixture_id, const char* message) {
    if (r->count >= r->capacity) {
        int new_cap = (r->capacity == 0) ? 10 : r->capacity * 2;
        Violation* new_items = (Violation*)realloc(r->items, sizeof(Violation) * new_cap);
        if (!new_items) return -1;  // malloc check
        r->items = new_items;
        r->capacity = new_cap;
    }
    r->items[r->count].type = type;
    r->items[r->count].fixture_id = fixture_id;
    r->items[r->count].other_fixture_id = other_fixture_id;
    strncpy(r->items[r->count].message, message, 255);
    r->items[r->count].message[255] = '\0';
    r->count++;
    return 0;
}

// Get violation type name
static const char* violation_name(ViolationType type) {
    switch (type) {
        case VIOL_TEAM_TWICE_IN_ROUND: return "VIOL_TEAM_TWICE_IN_ROUND";
        case VIOL_SELF_MATCH: return "VIOL_SELF_MATCH";
        case VIOL_BAD_ID: return "VIOL_BAD_ID";
        case VIOL_VENUE_CLASH: return "VIOL_VENUE_CLASH";
        case VIOL_REST_DAYS: return "VIOL_REST_DAYS";
        case VIOL_VENUE_UNAVAILABLE: return "VIOL_VENUE_UNAVAILABLE";
        case VIOL_CAPACITY: return "VIOL_CAPACITY";
        case VIOL_DUPLICATE: return "VIOL_DUPLICATE";
        case VIOL_MISSING: return "VIOL_MISSING";
        case VIOL_HOME_AWAY_IMBALANCE: return "VIOL_HOME_AWAY_IMBALANCE";
        case VIOL_PAST_END_DATE: return "VIOL_PAST_END_DATE";
        default: return "UNKNOWN";
    }
}

int validate_schedule(const Fixture* f, int fixture_count, int team_count,
                      const Venue* v, int venue_count,
                      const TournamentConfig* cfg,
                      const ValidationOptions* opt, ValidationReport* out) {

    // Early return for empty schedule
    if (fixture_count == 0) {
        if (!out) return -1;
        out->items = NULL;
        out->count = 0;
        out->capacity = 0;
        return 0;
    }

    // Initialize output
    if (!out) return -1;
    out->items = NULL;
    out->count = 0;
    out->capacity = 0;

    // NULL check
    if (!f || !v || !cfg) return -1;
    if (fixture_count < 0 || team_count < 0 || venue_count < 0) return -1;

    // Check 1: Self-match and bad IDs - SKIP other checks for bad IDs
    int* has_bad_id = (int*)calloc(fixture_count, sizeof(int));
    if (!has_bad_id) return -1;

    for (int i = 0; i < fixture_count; i++) {
        // Self-match check (always run, even if bad ID)
        if (f[i].home_team_id == f[i].away_team_id) {
            char msg[256];
            snprintf(msg, 255, "Fixture %d: team %d plays itself",
                    f[i].id, f[i].home_team_id);
            add_violation(out, VIOL_SELF_MATCH, f[i].id, -1, msg);
        }

        // Bad ID check
        if (f[i].home_team_id < 0 || f[i].home_team_id >= team_count ||
            f[i].away_team_id < 0 || f[i].away_team_id >= team_count ||
            f[i].venue_id < 0 || f[i].venue_id >= venue_count) {
            char msg[256];
            snprintf(msg, 255, "Fixture %d: invalid team or venue ID", f[i].id);
            add_violation(out, VIOL_BAD_ID, f[i].id, -1, msg);
            has_bad_id[i] = 1;  // Mark for skipping
        }
    }

    // Check 2: Venue clashes - O(F log F) using qsort + sweep
    int* idx = (int*)malloc(sizeof(int) * fixture_count);
    if (!idx) { free(has_bad_id); return -1; }

    for (int i = 0; i < fixture_count; i++) idx[i] = i;

    // Sort by (day, venue, time_slot)
    for (int i = 0; i < fixture_count - 1; i++) {
        for (int j = i + 1; j < fixture_count; j++) {
            int swap = 0;
            if (f[idx[i]].day > f[idx[j]].day) swap = 1;
            else if (f[idx[i]].day == f[idx[j]].day && f[idx[i]].venue_id > f[idx[j]].venue_id) swap = 1;
            else if (f[idx[i]].day == f[idx[j]].day && f[idx[i]].venue_id == f[idx[j]].venue_id && f[idx[i]].time_slot > f[idx[j]].time_slot) swap = 1;
            if (swap) { int tmp = idx[i]; idx[i] = idx[j]; idx[j] = tmp; }
        }
    }

    // Sweep for clashes
    for (int i = 0; i < fixture_count - 1; i++) {
        if (has_bad_id[idx[i]]) continue;

        int j = i + 1;
        while (j < fixture_count &&
               f[idx[i]].day == f[idx[j]].day &&
               f[idx[i]].venue_id == f[idx[j]].venue_id) {
            if (!has_bad_id[idx[j]] && f[idx[i]].time_slot == f[idx[j]].time_slot) {
                char msg[256];
                snprintf(msg, 255, "Fixture %d and %d: same venue, day, slot",
                        f[idx[i]].id, f[idx[j]].id);
                add_violation(out, VIOL_VENUE_CLASH, f[idx[i]].id, f[idx[j]].id, msg);
            }
            j++;
        }
    }

    // Find max round in data (no hardcoded 100 limit)
    int max_round = 0;
    for (int i = 0; i < fixture_count; i++) {
        if (f[i].round > max_round) max_round = f[i].round;
    }

    // Check 3: Team plays twice in same round
    for (int round = 1; round <= max_round; round++) {
        for (int team = 0; team < team_count; team++) {
            int count = 0;
            int fixture_ids[2] = {-1, -1};
            for (int i = 0; i < fixture_count; i++) {
                if (has_bad_id[i]) continue;
                if (f[i].round == round &&
                    (f[i].home_team_id == team || f[i].away_team_id == team)) {
                    if (count < 2) fixture_ids[count] = f[i].id;
                    count++;
                }
            }
            if (count > 1) {
                char msg[256];
                snprintf(msg, 255, "Fixture %d and %d: team %d plays twice in round %d",
                        fixture_ids[0], fixture_ids[1], team, round);
                add_violation(out, VIOL_TEAM_TWICE_IN_ROUND, fixture_ids[0],
                             fixture_ids[1], msg);
            }
        }
    }

    // Check 4: Rest days - O(F log F) using sort + sweep per team
    for (int team = 0; team < team_count; team++) {
        int team_day_count = 0;
        for (int i = 0; i < fixture_count; i++) {
            if (has_bad_id[i]) continue;
            if (f[i].home_team_id == team || f[i].away_team_id == team) {
                team_day_count++;
            }
        }

        if (team_day_count < 2) continue;

        int* days = (int*)malloc(sizeof(int) * team_day_count);
        int* fixture_ids = (int*)malloc(sizeof(int) * team_day_count);
        if (!days || !fixture_ids) {
            free(days);
            free(fixture_ids);
            continue;
        }

        int d = 0;
        for (int i = 0; i < fixture_count; i++) {
            if (has_bad_id[i]) continue;
            if (f[i].home_team_id == team || f[i].away_team_id == team) {
                days[d] = f[i].day;
                fixture_ids[d] = f[i].id;
                d++;
            }
        }

        // Sort days
        for (int i = 0; i < team_day_count - 1; i++) {
            for (int j = i + 1; j < team_day_count; j++) {
                if (days[j] < days[i]) {
                    int tmp = days[i];
                    days[i] = days[j];
                    days[j] = tmp;
                    tmp = fixture_ids[i];
                    fixture_ids[i] = fixture_ids[j];
                    fixture_ids[j] = tmp;
                }
            }
        }

        for (int i = 0; i < team_day_count - 1; i++) {
            int gap = days[i + 1] - days[i] - 1;
            if (gap < cfg->min_rest_days) {
                char msg[256];
                snprintf(msg, 255, "Fixture %d and %d: team %d insufficient rest (%d days, need %d)",
                        fixture_ids[i], fixture_ids[i+1], team, gap, cfg->min_rest_days);
                add_violation(out, VIOL_REST_DAYS, fixture_ids[i], fixture_ids[i+1], msg);
            }
        }
        free(days);
        free(fixture_ids);
    }

    // Check 5: Venue unavailable
    for (int i = 0; i < fixture_count; i++) {
        if (has_bad_id[i]) continue;
        if (!v[f[i].venue_id].is_available) {
            char msg[256];
            snprintf(msg, 255, "Fixture %d: venue %d is unavailable",
                    f[i].id, f[i].venue_id);
            add_violation(out, VIOL_VENUE_UNAVAILABLE, f[i].id, -1, msg);
        }
    }

    // Check 6: Venue capacity
    if (opt && opt->min_capacity > 0) {
        for (int i = 0; i < fixture_count; i++) {
            if (has_bad_id[i]) continue;
            if (v[f[i].venue_id].capacity < opt->min_capacity) {
                char msg[256];
                snprintf(msg, 255, "Fixture %d: venue capacity %d < %d",
                        f[i].id, v[f[i].venue_id].capacity, opt->min_capacity);
                add_violation(out, VIOL_CAPACITY, f[i].id, -1, msg);
            }
        }
    }

    // Check 7: Duplicates - ALWAYS run (not just when require_full_double_rr)
    int** pair_count = (int**)malloc(sizeof(int*) * team_count);
    if (pair_count) {
        int alloc_ok = 1;
        for (int i = 0; i < team_count; i++) {
            pair_count[i] = (int*)calloc(team_count, sizeof(int));
            if (!pair_count[i]) { alloc_ok = 0; break; }
        }

        if (alloc_ok) {
            for (int i = 0; i < fixture_count; i++) {
                if (!has_bad_id[i]) {
                    pair_count[f[i].home_team_id][f[i].away_team_id]++;
                }
            }

            for (int i = 0; i < team_count; i++) {
                for (int j = 0; j < team_count; j++) {
                    if (i != j && pair_count[i][j] > 1) {
                        char msg[256];
                        snprintf(msg, 255, "Duplicate pair: team %d home vs team %d (count: %d)",
                                i, j, pair_count[i][j]);
                        add_violation(out, VIOL_DUPLICATE, -1, -1, msg);
                    }
                }
            }
        }

        for (int i = 0; i < team_count; i++) {
            if (pair_count[i]) free(pair_count[i]);
        }
        free(pair_count);
    }

    // Check 8: Missing pairs + Home/away balance
    if (opt && opt->require_full_double_rr) {
        // Missing pairs
        int** pair_count2 = (int**)malloc(sizeof(int*) * team_count);
        if (pair_count2) {
            int alloc_ok = 1;
            for (int i = 0; i < team_count; i++) {
                pair_count2[i] = (int*)calloc(team_count, sizeof(int));
                if (!pair_count2[i]) { alloc_ok = 0; break; }
            }

            if (alloc_ok) {
                for (int i = 0; i < fixture_count; i++) {
                    if (!has_bad_id[i]) {
                        pair_count2[f[i].home_team_id][f[i].away_team_id]++;
                    }
                }

                for (int i = 0; i < team_count; i++) {
                    for (int j = 0; j < team_count; j++) {
                        if (i != j && pair_count2[i][j] == 0) {
                            char msg[256];
                            snprintf(msg, 255, "Missing pair: team %d home vs team %d",
                                    i, j);
                            add_violation(out, VIOL_MISSING, -1, -1, msg);
                        }
                    }
                }
            }

            for (int i = 0; i < team_count; i++) {
                if (pair_count2[i]) free(pair_count2[i]);
            }
            free(pair_count2);
        }

        // Check 9: Home/away balance
        int* home_count = (int*)calloc(team_count, sizeof(int));
        int* away_count = (int*)calloc(team_count, sizeof(int));
        if (home_count && away_count) {
            for (int i = 0; i < fixture_count; i++) {
                if (!has_bad_id[i]) {
                    home_count[f[i].home_team_id]++;
                    away_count[f[i].away_team_id]++;
                }
            }

            for (int i = 0; i < team_count; i++) {
                int diff = home_count[i] - away_count[i];
                if (diff > 1 || diff < -1) {
                    char msg[256];
                    snprintf(msg, 255, "Team %d: home/away imbalance (%d vs %d)",
                            i, home_count[i], away_count[i]);
                    add_violation(out, VIOL_HOME_AWAY_IMBALANCE, -1, -1, msg);
                }
            }
        }
        if (home_count) free(home_count);
        if (away_count) free(away_count);
    }

    // Check 10: Past end date
    if (cfg->end_date.year != 0) {
        for (int i = 0; i < fixture_count; i++) {
            if (has_bad_id[i]) continue;
            if (f[i].match_date.year > cfg->end_date.year ||
                (f[i].match_date.year == cfg->end_date.year &&
                 f[i].match_date.month > cfg->end_date.month) ||
                (f[i].match_date.year == cfg->end_date.year &&
                 f[i].match_date.month == cfg->end_date.month &&
                 f[i].match_date.day > cfg->end_date.day)) {
                char msg[256];
                snprintf(msg, 255, "Fixture %d: match date after tournament end",
                        f[i].id);
                add_violation(out, VIOL_PAST_END_DATE, f[i].id, -1, msg);
            }
        }
    }

    free(has_bad_id);
    free(idx);
    return out->count;
}

void apply_validation_flags(Fixture* f, int count, const ValidationReport* r) {
    if (!f || !r) return;

    for (int i = 0; i < count; i++) {
        f[i].is_valid = TRUE;
    }

    for (int i = 0; i < r->count; i++) {
        for (int j = 0; j < count; j++) {
            if (f[j].id == r->items[i].fixture_id) {
                f[j].is_valid = FALSE;
            }
            if (r->items[i].other_fixture_id > 0 && f[j].id == r->items[i].other_fixture_id) {
                f[j].is_valid = FALSE;
            }
        }
    }
}

void print_validation_report(const ValidationReport* r) {
    if (!r) return;

    printf("\n========================================================\n");
    printf("VALIDATION REPORT: %d violations\n", r->count);
    printf("========================================================\n");

    if (r->count == 0) {
        printf("No violations found\n");
        return;
    }

    for (int i = 0; i < r->count; i++) {
        printf("%d. [%s] %s\n", i + 1, violation_name(r->items[i].type), r->items[i].message);
    }
}

void free_validation_report(ValidationReport* r) {
    if (r && r->items) {
        free(r->items);
        r->items = NULL;
        r->count = 0;
        r->capacity = 0;
    }
}