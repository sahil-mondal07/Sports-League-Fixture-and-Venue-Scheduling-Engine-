#ifndef TEAM_H
#define TEAM_H

#include "../utils/common.h"

typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    char home_stadium[MAX_NAME_LENGTH];
    float latitude;
    float longitude;
    int ranking;
    int points;
} Team;

Team* create_team(int id, const char* name, const char* stadium,
                  float lat, float lon, int ranking, int points);

void print_team(const Team* team);

void free_team(Team* team);

int compare_teams_by_ranking(const void* a, const void* b);

int compare_teams_by_points(const void* a, const void* b);

#endif
