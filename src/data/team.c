#include "team.h"

Team* create_team(int id, const char* name, const char* stadium,
                  float lat, float lon, int ranking, int points) {
    Team* team = (Team*)malloc(sizeof(Team));
    if (!team) return NULL;

    team->id = id;
    strncpy(team->name, name, MAX_NAME_LENGTH - 1);
    team->name[MAX_NAME_LENGTH - 1] = '\0';
    strncpy(team->home_stadium, stadium, MAX_NAME_LENGTH - 1);
    team->home_stadium[MAX_NAME_LENGTH - 1] = '\0';
    team->latitude = lat;
    team->longitude = lon;
    team->ranking = ranking;
    team->points = points;

    return team;
}

void print_team(const Team* team) {
    if (!team) return;
    printf("Team #%d: %s (Stadium: %s, Ranking: %d, Points: %d)\n",
           team->id, team->name, team->home_stadium, team->ranking, team->points);
}

void free_team(Team* team) {
    if (team) free(team);
}

int compare_teams_by_ranking(const void* a, const void* b) {
    Team* t1 = (Team*)a;
    Team* t2 = (Team*)b;
    return t1->ranking - t2->ranking;
}

int compare_teams_by_points(const void* a, const void* b) {
    Team* t1 = (Team*)a;
    Team* t2 = (Team*)b;
    return t2->points - t1->points;
}
