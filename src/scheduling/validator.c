#include "validator.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------
 * Static pointer to fixture array for qsort comparison function.
 * Set before calling qsort below in validate_schedule().
 * ---------------------------------------------------------- */
static const Fixture* s_f = NULL;

/* ------------------------------------------------------------
 * Helper: comparison function for qsort on fixture indices.
 * Sorts indices by corresponding fixture's day/venue/time_slot.
 * Uses static s_f pointer to access fixture data by index (portable C99).
 * ------------------------------------------------------------ */
static int compare_fixture_index_by_day_venue_slot(const void* a, const void* b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;

    const Fixture* fa = &s_f[ia];
    const Fixture* fb = &s_f[ib];

    if (fa->day != fb->day) return (fa->day > fb->day) ? 1 : -1;
    if (fa->venue_id != fb->venue_id) return (fa->venue_id > fb->venue_id) ? 1 : -1;
    if (fa->time_slot != fb->time_slot) return (fa->time_slot > fb->time_slot) ? 1 : -1;
    if (fa->id != fb->id) return (fa->id > fb->id) ? 1 : -1;
    return 0;
}

/* ------------------------------------------------------------
 * DayFixture struct and dedicated comparator for rest-day sorting
 * ------------------------------------------------------------ */
typedef struct {
    int day;
    int fixture_id;
} DayFixture;

static int compare_day_fixtures(const void* a, const void* b) {
    const DayFixture* fa = (const DayFixture*)a;
    const DayFixture* fb = (const DayFixture*)b;
    if (fa->day != fb->day) return (fa->day > fb->day) ? 1 : -1;
    if (fa->fixture_id != fb->fixture_id) return (fa->fixture_id > fb->fixture_id) ? 1 : -1;
    return 0;
}

/* ------------------------------------------------------------
 * Helper: add violation to report
 * ------------------------------------------------------------ */
static int add_violation(ValidationReport* r, ViolationType type, int fixture_id,
                         int other_fixture_id, const char* message) {
    if (!r) return -1;
    if (r->count >= r->capacity) {
        int new_cap = (r->capacity == 0) ? 16 : r->capacity * 2;
        Violation* new_items = (Violation*)realloc(r->items, sizeof(Violation) * new_cap);
        if (!new_items) return -1;  /* Propagate allocation failure */
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

/* ------------------------------------------------------------
 * Get violation type name
 * ------------------------------------------------------------ */
const char* violation_name(ViolationType type) {
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

/* ------------------------------------------------------------
 * Free validation report items
 * ---------------------------------------------------------- */
void free_validation_report(ValidationReport* r) {
    if (r && r->items) {
        free(r->items);
        r->items = NULL;
        r->count = 0;
        r->capacity = 0;
    }
}

/* ----------------------------------------------------------
 * Print validation report
 * ---------------------------------------------------------- */
void print_validation_report(const ValidationReport* r) {
    if (!r) return;
    if (r->count == 0) {
        printf("No violations detected.\n");
        return;
    }
    printf("=== Validation Report (%d violations) ===\n", r->count);
    for (int i = 0; i < r->count; i++) {
        const Violation* v = &r->items[i];
        printf("[%d] %s", i + 1, violation_name(v->type));
        if (v->fixture_id >= 0) {
            printf(" fixture_id=%d", v->fixture_id);
        }
        if (v->other_fixture_id >= 0) {
            printf(" other_fixture_id=%d", v->other_fixture_id);
        }
        printf(": %s\n", v->message);
    }
    printf("========================================\n");
}

/* ------------------------------------------------------------
 * Validation function
 * ---------------------------------------------------------- */
int validate_schedule(const Fixture* f, int fixture_count, int team_count,
                      const Venue* v, int venue_count,
                      const TournamentConfig* cfg,
                      const ValidationOptions* opt, ValidationReport* out) {

    /* Set static pointer for comparison function */
    s_f = f;

    /* Initialize output */
    if (!out) return -1;
    out->items = NULL;
    out->count = 0;
    out->capacity = 0;

    /* Early return for empty schedule */
    if (fixture_count == 0) {
        return 0;
    }

    /* NULL check */
    if (!f || !v || !cfg) return -1;
    if (fixture_count < 0 || team_count < 0 || venue_count < 0) return -1;

    int* has_bad_id = (int*)calloc(fixture_count, sizeof(int));
    if (!has_bad_id) return -1;

    int* idx = (int*)malloc(sizeof(int) * fixture_count);
    if (!idx) {
        free(has_bad_id);
        return -1;
    }
    for (int i = 0; i < fixture_count; i++) idx[i] = i;

    /* ----------------------------------------------------------
     * Step 1: Identify all bad IDs, record VIOL_BAD_ID, and skip others.
     * ---------------------------------------------------------- */
    for (int i = 0; i < fixture_count; i++) {
        if (f[i].home_team_id < 0 || f[i].home_team_id >= team_count ||
            f[i].away_team_id < 0 || f[i].away_team_id >= team_count ||
            f[i].venue_id < 0 || f[i].venue_id >= venue_count) {
            has_bad_id[i] = 1;
            char msg[256];
            snprintf(msg, 255, "Fixture %d: invalid team or venue ID (home=%d, away=%d, venue=%d)",
                    f[i].id, f[i].home_team_id, f[i].away_team_id, f[i].venue_id);
            if (add_violation(out, VIOL_BAD_ID, f[i].id, -1, msg) < 0) {
                free(has_bad_id);
                free(idx);
                return -1;
            }
        }
    }

    /* Self-match check only for fixtures WITHOUT bad IDs */
    for (int i = 0; i < fixture_count; i++) {
        if (has_bad_id[i]) continue;

        if (f[i].home_team_id == f[i].away_team_id) {
            char msg[256];
            snprintf(msg, 255, "Fixture %d: team %d plays itself",
                    f[i].id, f[i].home_team_id);
            if (add_violation(out, VIOL_SELF_MATCH, f[i].id, -1, msg) < 0) {
                free(has_bad_id);
                free(idx);
                return -1;
            }
        }
    }

    /* ----------------------------------------------------------
     * Step 2: Venue clashes - O(F log F) using qsort + sweep
     * ---------------------------------------------------------- */
    qsort(idx, fixture_count, sizeof(int), compare_fixture_index_by_day_venue_slot);

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
                if (add_violation(out, VIOL_VENUE_CLASH, f[idx[i]].id, f[idx[j]].id, msg) < 0) {
                    free(has_bad_id);
                    free(idx);
                    return -1;
                }
            }
            j++;
        }
    }

    /* ----------------------------------------------------------
     * Step 3: Team plays twice in same round
     * ---------------------------------------------------------- */
    int max_round = 0;
    for (int i = 0; i < fixture_count; i++) {
        if (!has_bad_id[i] && f[i].round > max_round) max_round = f[i].round;
    }

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
                if (add_violation(out, VIOL_TEAM_TWICE_IN_ROUND, fixture_ids[0],
                             fixture_ids[1], msg) < 0) {
                    free(has_bad_id);
                    free(idx);
                    return -1;
                }
            }
        }
    }

    /* ----------------------------------------------------------
     * Step 4: Rest days - O(F log F) using sort + sweep per team
     * ---------------------------------------------------------- */
    for (int team = 0; team < team_count; team++) {
        int da = 0, da_cap = 8;
        DayFixture* df = (DayFixture*)malloc(sizeof(DayFixture) * da_cap);
        if (!df) {
            free(has_bad_id);
            free(idx);
            free_validation_report(out);
            return -1;
        }

        for (int i = 0; i < fixture_count; i++) {
            if (has_bad_id[i]) continue;
            if (f[i].home_team_id == team || f[i].away_team_id == team) {
                if (da >= da_cap) {
                    int new_cap = da_cap * 2;
                    DayFixture* new_df = (DayFixture*)realloc(df, sizeof(DayFixture) * new_cap);
                    if (!new_df) {
                        free(df);
                        free(has_bad_id);
                        free(idx);
                        free_validation_report(out);
                        return -1;
                    }
                    df = new_df;
                    da_cap = new_cap;
                }
                df[da].day = f[i].day;
                df[da].fixture_id = f[i].id;
                da++;
            }
        }

        if (da < 2) {
            free(df);
            continue;
        }

        qsort(df, da, sizeof(DayFixture), compare_day_fixtures);

        for (int i = 0; i < da - 1; i++) {
            int gap = df[i + 1].day - df[i].day - 1;
            if (gap < cfg->min_rest_days) {
                int second_fixture_id = -1;
                for (int k = i + 1; k < da; k++) {
                    if (df[k].fixture_id != df[i].fixture_id) {
                        second_fixture_id = df[k].fixture_id;
                        break;
                    }
                }
                char msg[256];
                snprintf(msg, 255, "Fixture %d and %d: team %d insufficient rest (%d days, need %d)",
                        df[i].fixture_id, second_fixture_id, team, gap, cfg->min_rest_days);
                if (add_violation(out, VIOL_REST_DAYS, df[i].fixture_id, second_fixture_id, msg) < 0) {
                    free(df);
                    free(has_bad_id);
                    free(idx);
                    return -1;
                }
            }
        }

        free(df);
    }

    /* ----------------------------------------------------------
     * Step 5: Venue unavailable
     * ---------------------------------------------------------- */
    for (int i = 0; i < fixture_count; i++) {
        if (has_bad_id[i]) continue;
        if (!v[f[i].venue_id].is_available) {
            char msg[256];
            snprintf(msg, 255, "Fixture %d: venue %d is unavailable",
                    f[i].id, f[i].venue_id);
            if (add_violation(out, VIOL_VENUE_UNAVAILABLE, f[i].id, -1, msg) < 0) {
                free(has_bad_id);
                free(idx);
                return -1;
            }
        }
    }

    /* ----------------------------------------------------------
     * Step 6: Venue capacity
     * ---------------------------------------------------------- */
    if (opt && opt->min_capacity > 0) {
        for (int i = 0; i < fixture_count; i++) {
            if (has_bad_id[i]) continue;
            if (v[f[i].venue_id].capacity < opt->min_capacity) {
                char msg[256];
                snprintf(msg, 255, "Fixture %d: venue capacity %d < %d",
                        f[i].id, v[f[i].venue_id].capacity, opt->min_capacity);
                if (add_violation(out, VIOL_CAPACITY, f[i].id, -1, msg) < 0) {
                    free(has_bad_id);
                    free(idx);
                    return -1;
                }
            }
        }
    }

    /* ----------------------------------------------------------
     * Step 7: Duplicates
     * ---------------------------------------------------------- */
    int** pair_count = (int**)malloc(sizeof(int*) * team_count);
    if (!pair_count) {
        free(has_bad_id);
        free(idx);
        return -1;
    }
    for (int i = 0; i < team_count; i++) pair_count[i] = NULL;

    int alloc_ok = 1;
    for (int i = 0; i < team_count; i++) {
        pair_count[i] = (int*)calloc(team_count, sizeof(int));
        if (!pair_count[i]) {
            alloc_ok = 0;
            break;
        }
    }

    if (!alloc_ok) {
        for (int i = 0; i < team_count; i++) {
            if (pair_count[i]) free(pair_count[i]);
        }
        free(pair_count);
        free(has_bad_id);
        free(idx);
        return -1;
    }

    for (int i = 0; i < fixture_count; i++) {
        if (!has_bad_id[i]) {
            pair_count[f[i].home_team_id][f[i].away_team_id]++;
        }
    }

    for (int i = 0; i < team_count; i++) {
        for (int j = 0; j < team_count; j++) {
            if (i != j && pair_count[i][j] > 1) {
                int dup_fixture_id = -1;
                int other_fixture_id = -1;
                for (int k = 0; k < fixture_count; k++) {
                    if (!has_bad_id[k] && f[k].home_team_id == i && f[k].away_team_id == j) {
                        dup_fixture_id = f[k].id;
                        for (int m = k + 1; m < fixture_count; m++) {
                            if (!has_bad_id[m] && f[m].home_team_id == i && f[m].away_team_id == j) {
                                other_fixture_id = f[m].id;
                                break;
                            }
                        }
                        break;
                    }
                }
                char msg[256];
                snprintf(msg, 255, "Duplicate pair: team %d home vs team %d (count: %d)",
                        i, j, pair_count[i][j]);
                if (add_violation(out, VIOL_DUPLICATE, dup_fixture_id, other_fixture_id, msg) < 0) {
                    for (int x = 0; x < team_count; x++) free(pair_count[x]);
                    free(pair_count);
                    free(has_bad_id);
                    free(idx);
                    return -1;
                }
            }
        }
    }

    for (int i = 0; i < team_count; i++) {
        free(pair_count[i]);
    }
    free(pair_count);

    /* ----------------------------------------------------------
     * Step 8: Missing pairs + Home/away balance
     * ---------------------------------------------------------- */
    if (opt && opt->require_full_double_rr) {
        int** pair_count2 = (int**)malloc(sizeof(int*) * team_count);
        if (!pair_count2) {
            free(has_bad_id);
            free(idx);
            return -1;
        }
        for (int i = 0; i < team_count; i++) pair_count2[i] = NULL;

        int alloc_ok2 = 1;
        for (int i = 0; i < team_count; i++) {
            pair_count2[i] = (int*)calloc(team_count, sizeof(int));
            if (!pair_count2[i]) {
                alloc_ok2 = 0;
                break;
            }
        }

        if (!alloc_ok2) {
            for (int i = 0; i < team_count; i++) {
                if (pair_count2[i]) free(pair_count2[i]);
            }
            free(pair_count2);
            free(has_bad_id);
            free(idx);
            return -1;
        }

        for (int i = 0; i < fixture_count; i++) {
            if (!has_bad_id[i]) {
                pair_count2[f[i].home_team_id][f[i].away_team_id]++;
            }
        }

        for (int i = 0; i < team_count; i++) {
            for (int j = 0; j < team_count; j++) {
                if (i != j && pair_count2[i][j] == 0) {
                    int missing_fixture_id = -1;
                    for (int k = 0; k < fixture_count; k++) {
                        if (!has_bad_id[k] &&
                            f[k].home_team_id == i && f[k].away_team_id == j) {
                            missing_fixture_id = f[k].id;
                            break;
                        }
                    }
                    char msg[256];
                    snprintf(msg, 255, "Missing pair: team %d home vs team %d",
                            i, j);
                    if (add_violation(out, VIOL_MISSING, missing_fixture_id, -1, msg) < 0) {
                        for (int x = 0; x < team_count; x++) free(pair_count2[x]);
                        free(pair_count2);
                        free(has_bad_id);
                        free(idx);
                        return -1;
                    }
                }
            }
        }

        for (int i = 0; i < team_count; i++) {
            free(pair_count2[i]);
        }
        free(pair_count2);

        /* Home/away balance */
        int* home_count = (int*)calloc(team_count, sizeof(int));
        int* away_count = (int*)calloc(team_count, sizeof(int));
        if (!home_count || !away_count) {
            if (home_count) free(home_count);
            if (away_count) free(away_count);
            free(has_bad_id);
            free(idx);
            return -1;
        }

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
                if (add_violation(out, VIOL_HOME_AWAY_IMBALANCE, -1, -1, msg) < 0) {
                    free(home_count);
                    free(away_count);
                    free(has_bad_id);
                    free(idx);
                    return -1;
                }
            }
        }
        free(home_count);
        free(away_count);
    }

    /* ----------------------------------------------------------
     * Step 10: Past end date
     * ---------------------------------------------------------- */
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
                if (add_violation(out, VIOL_PAST_END_DATE, f[i].id, -1, msg) < 0) {
                    free(has_bad_id);
                    free(idx);
                    return -1;
                }
            }
        }
    }

    free(has_bad_id);
    free(idx);
    return out->count;
}
