#include "fixture.h"

Fixture* create_fixture(int id, int round, int home_team, int away_team,
                       int venue, int day, int time_slot) {
    Fixture* fixture = (Fixture*)malloc(sizeof(Fixture));
    if (!fixture) return NULL;

    fixture->id = id;
    fixture->round = round;
    fixture->home_team_id = home_team;
    fixture->away_team_id = away_team;
    fixture->venue_id = venue;
    fixture->day = day;
    fixture->match_date.day = 0;    // Will be set by scheduler
    fixture->match_date.month = 0;
    fixture->match_date.year = 0;
    fixture->time_slot = time_slot;
    fixture->travel_cost = 0.0;
    fixture->is_valid = TRUE;

    return fixture;
}

void print_fixture(const Fixture* fixture) {
    if (!fixture) return;
    printf("Fixture #%d: Round %d, Team %d vs Team %d at Venue %d (Day %d",
           fixture->id, fixture->round, fixture->home_team_id, fixture->away_team_id,
           fixture->venue_id, fixture->day);

    // Print actual date if set
    if (fixture->match_date.year > 0) {
        printf(", Date: ");
        print_date((Date*)&fixture->match_date);
    }

    printf(", Slot %d, Cost: %.2f km)\n", fixture->time_slot, fixture->travel_cost);
}

void free_fixture(Fixture* fixture) {
    if (fixture) free(fixture);
}

int compare_fixtures_by_round(const void* a, const void* b) {
    Fixture* f1 = (Fixture*)a;
    Fixture* f2 = (Fixture*)b;
    return f1->round - f2->round;
}

int compare_fixtures_by_day(const void* a, const void* b) {
    Fixture* f1 = (Fixture*)a;
    Fixture* f2 = (Fixture*)b;
    if (f1->day != f2->day) {
        return f1->day - f2->day;
    }
    return f1->time_slot - f2->time_slot;
}
