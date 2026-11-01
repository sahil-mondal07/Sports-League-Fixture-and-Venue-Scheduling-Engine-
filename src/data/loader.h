#ifndef LOADER_H
#define LOADER_H

#include "team.h"
#include "venue.h"
#include "graph.h"

// Load teams from CSV (id,name,rating)
// Returns array of teams, sets count via pointer
Team* load_teams(const char* filename, int* count);

// Load venues from CSV (id,name,city,lat,lon,capacity)
// Returns array of venues, sets count via pointer
Venue* load_venues(const char* filename, int* count);

// Load distance matrix from CSV
// Format: row per team pair (team1_id,team2_id,distance)
// Returns distance matrix (2D array), sets size via pointer
double** load_distance_matrix(const char* filename, int team_count);

// Helper: Parse CSV line into tokens (modifies input string)
// Returns number of tokens found
int parse_csv_line(char* line, char** tokens, int max_tokens);

#endif
