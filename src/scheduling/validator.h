#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "../data/fixture.h"
#include "../data/venue.h"
#include "../utils/common.h"

typedef enum {
    VIOL_TEAM_TWICE_IN_ROUND,
    VIOL_SELF_MATCH,
    VIOL_BAD_ID,
    VIOL_VENUE_CLASH,
    VIOL_REST_DAYS,
    VIOL_VENUE_UNAVAILABLE,
    VIOL_CAPACITY,
    VIOL_DUPLICATE,
    VIOL_MISSING,
    VIOL_HOME_AWAY_IMBALANCE,
    VIOL_PAST_END_DATE
} ViolationType;

typedef struct {
    ViolationType type;
    int fixture_id;
    int other_fixture_id;
    char message[256];
} Violation;

typedef struct {
    Violation* items;
    int count;
    int capacity;
} ValidationReport;

typedef struct {
    int min_capacity;
    int require_full_double_rr;
} ValidationOptions;

int validate_schedule(const Fixture* f, int fixture_count, int team_count,
                      const Venue* v, int venue_count,
                      const TournamentConfig* cfg,
                      const ValidationOptions* opt, ValidationReport* out);

void apply_validation_flags(Fixture* f, int count, const ValidationReport* r);

void print_validation_report(const ValidationReport* r);

void free_validation_report(ValidationReport* r);

#endif
