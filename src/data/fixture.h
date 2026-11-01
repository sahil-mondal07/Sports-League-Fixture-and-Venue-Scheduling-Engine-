#ifndef FIXTURE_H
#define FIXTURE_H

#include "../utils/common.h"

typedef struct {
    int id;
    int round;
    int home_team_id;
    int away_team_id;
    int venue_id;
    int day;
    Date match_date;        // Actual calendar date
    int time_slot;
    float travel_cost;
    bool is_valid;
} Fixture;

Fixture* create_fixture(int id, int round, int home_team, int away_team,
                       int venue, int day, int time_slot);

void print_fixture(const Fixture* fixture);

void free_fixture(Fixture* fixture);

int compare_fixtures_by_round(const void* a, const void* b);

int compare_fixtures_by_day(const void* a, const void* b);

#endif
