#ifndef ROUND_ROBIN_H
#define ROUND_ROBIN_H

#include "../../data/team.h"
#include "../../data/fixture.h"
#include "../../data/venue.h"
#include "../../utils/common.h"

typedef struct {
    Fixture* fixtures;
    int count;
    int total_rounds;
} FixtureSchedule;

FixtureSchedule* round_robin_schedule(Team* teams, int team_count,
                                      Venue* venues, int venue_count,
                                      TournamentConfig* config);

void print_fixture_schedule(FixtureSchedule* schedule);
void free_fixture_schedule(FixtureSchedule* schedule);

#endif
