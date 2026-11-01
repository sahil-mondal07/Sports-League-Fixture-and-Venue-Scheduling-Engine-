#include "validator.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Helper to add violation to report
static void add_violation(ValidationReport* r, ViolationType type, int fixture_id,
                         int other_fixture_id, const char* message) {
    if (r->count >= r->capacity) {
        r->capacity = (r->capacity == 0) ? 10 : r->capacity * 2;
        r->items = (Violation*)realloc(r->items, sizeof(Violation) * r->capacity);
    }
    r->items[r->count].type = type;
    r->items[r->count].fixture_id = fixture_id;
    r->items[r->count].other_fixture_id = other_fixture_id;
    strncpy(r->items[r->count].message, message, 255);
    r->items[r->count].message[255] = '\0';
    r->count++;
}

int validate_schedule(const Fixture* f, int fixture_count, int team_count,
                      const Venue* v, int venue_count,
                      const TournamentConfig* cfg,
                      const ValidationOptions* opt, ValidationReport* out) {

    // NULL check
    if (!f || !v || !cfg || !out) return -1;
    if (fixture_count < 0 || team_count < 0 || venue_count < 0) return -1;

    out->count = 0;
    if (fixture_count == 0) return 0;

    // Check 1: Self-match and bad IDs
    for (int i = 0; i < fixture_count; i++) {
        if (f[i].home_team_id == f[i].away_team_id) {
            char msg[256];
            snprintf(msg, 255, "Fixture %d: team %d plays itself",
                    f[i].id, f[i].home_team_id);
            add_violation(out, VIOL_SELF_MATCH, f[i].id, -1, msg);
        }

        if (f[i].home_team_id < 0 || f[i].home_team_id >= team_count ||
            f[i].away_team_id < 0 || f[i].away_team_id >= team_count ||
            f[i].venue_id < 0 || f[i].venue_id >= venue_count) {
            char msg[256];
            snprintf(msg, 255, "Fixture %d: invalid team or venue ID", f[i].id);
            add_violation(out, VIOL_BAD_ID, f[i].id, -1, msg);
        }
    }

    // Check 2: Venue clashes (same venue, day, slot)
    for (int i = 0; i < fixture_count; i++) {
        for (int j = i + 1; j < fixture_count; j++) {
            if (f[i].venue_id == f[j].venue_id &&
                f[i].day == f[j].day &&
                f[i].time_slot == f[j].time_slot) {
                char msg[256];
                snprintf(msg, 255, "Fixture %d and %d: same venue, day, slot",
                        f[i].id, f[j].id);
                add_violation(out, VIOL_VENUE_CLASH, f[i].id, f[j].id, msg);
            }
        }
    }

    // Check 3: Team plays twice in same round
    for (int round = 1; round <= 100; round++) {
        for (int team = 0; team < team_count; team++) {
            int count = 0;
            int fixture_ids[2] = {-1, -1};
            for (int i = 0; i < fixture_count; i++) {
                if (f[i].round == round &&
                    (f[i].home_team_id == team || f[i].away_team_id == team)) {
                    if (count < 2) fixture_ids[count] = f[i].id;
                    count++;
                }
            }
            if (count > 1) {
                char msg[256];
                snprintf(msg, 255, "Fixture %d: team %d plays twice in round %d",
                        fixture_ids[0], team, round);
                add_violation(out, VIOL_TEAM_TWICE_IN_ROUND, fixture_ids[0],
                             fixture_ids[1], msg);
            }
        }
    }

    // Check 4: Rest days
    for (int team = 0; team < team_count; team++) {
        int* days = (int*)malloc(sizeof(int) * fixture_count);
        int day_count = 0;
        for (int i = 0; i < fixture_count; i++) {
            if (f[i].home_team_id == team || f[i].away_team_id == team) {
                days[day_count++] = f[i].day;
            }
        }

        for (int i = 0; i < day_count - 1; i++) {
            for (int j = i + 1; j < day_count; j++) {
                if (days[j] < days[i]) {
                    int tmp = days[i];
                    days[i] = days[j];
                    days[j] = tmp;
                }
            }
        }

        for (int i = 0; i < day_count - 1; i++) {
            int gap = days[i + 1] - days[i] - 1;
            if (gap < cfg->min_rest_days) {
                char msg[256];
                snprintf(msg, 255, "Team %d: insufficient rest between matches",
                        team);
                add_violation(out, VIOL_REST_DAYS, -1, -1, msg);
            }
        }
        free(days);
    }

    // Check 5: Venue unavailable
    for (int i = 0; i < fixture_count; i++) {
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
            if (v[f[i].venue_id].capacity < opt->min_capacity) {
                char msg[256];
                snprintf(msg, 255, "Fixture %d: venue capacity %d < %d",
                        f[i].id, v[f[i].venue_id].capacity, opt->min_capacity);
                add_violation(out, VIOL_CAPACITY, f[i].id, -1, msg);
            }
        }
    }

    // Check 7: Duplicates and missing pairs
    if (opt && opt->require_full_double_rr) {
        int** pair_count = (int**)malloc(sizeof(int*) * team_count);
        for (int i = 0; i < team_count; i++) {
            pair_count[i] = (int*)calloc(team_count, sizeof(int));
        }

        for (int i = 0; i < fixture_count; i++) {
            pair_count[f[i].home_team_id][f[i].away_team_id]++;
        }

        for (int i = 0; i < team_count; i++) {
            for (int j = 0; j < team_count; j++) {
                if (i != j) {
                    if (pair_count[i][j] > 1) {
                        char msg[256];
                        snprintf(msg, 255, "Duplicate pair: team %d home vs team %d",
                                i, j);
                        add_violation(out, VIOL_DUPLICATE, -1, -1, msg);
                    } else if (pair_count[i][j] == 0) {
                        char msg[256];
                        snprintf(msg, 255, "Missing pair: team %d home vs team %d",
                                i, j);
                        add_violation(out, VIOL_MISSING, -1, -1, msg);
                    }
                }
            }
        }

        // Check 8: Home/away balance
        int* home_count = (int*)calloc(team_count, sizeof(int));
        int* away_count = (int*)calloc(team_count, sizeof(int));
        for (int i = 0; i < fixture_count; i++) {
            home_count[f[i].home_team_id]++;
            away_count[f[i].away_team_id]++;
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

        free(home_count);
        free(away_count);
        for (int i = 0; i < team_count; i++) free(pair_count[i]);
        free(pair_count);
    }

    // Check 9: Past end date
    if (cfg->end_date.year != 0) {
        for (int i = 0; i < fixture_count; i++) {
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
        printf("✓ No violations found\n");
        return;
    }

    for (int i = 0; i < r->count; i++) {
        printf("%d. [%d] %s\n", i + 1, r->items[i].type, r->items[i].message);
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
