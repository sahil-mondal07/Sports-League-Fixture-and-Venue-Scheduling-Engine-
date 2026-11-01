#include "loader.h"
#include "../utils/common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_csv_line(char* line, char** tokens, int max_tokens) {
    int count = 0;
    char* token = strtok(line, ",");
    while (token != NULL && count < max_tokens) {
        // Remove whitespace and newline
        while (*token == ' ') token++;
        char* end = token + strlen(token) - 1;
        while (end > token && (*end == ' ' || *end == '\n' || *end == '\r')) {
            *end = '\0';
            end--;
        }
        tokens[count++] = token;
        token = strtok(NULL, ",");
    }
    return count;
}

Team* load_teams(const char* filename, int* count) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open %s\n", filename);
        *count = 0;
        return NULL;
    }

    Team* teams = malloc(sizeof(Team) * MAX_TEAMS);
    if (!teams) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        *count = 0;
        return NULL;
    }

    char line[256];
    int team_count = 0;

    // Skip header line
    if (fgets(line, sizeof(line), file) == NULL) {
        fprintf(stderr, "Error: Empty file %s\n", filename);
        free(teams);
        fclose(file);
        *count = 0;
        return NULL;
    }

    while (fgets(line, sizeof(line), file) && team_count < MAX_TEAMS) {
        char* tokens[3];
        int token_count = parse_csv_line(line, tokens, 3);

        if (token_count == 3) {
            int id = atoi(tokens[0]);
            int rating = atoi(tokens[2]);
            Team* team = create_team(id, tokens[1], "", 0.0, 0.0, rating, 0);
            if (team) {
                teams[team_count++] = *team;
                free(team);
            }
        }
    }

    fclose(file);
    *count = team_count;
    printf("Loaded %d teams from %s\n", team_count, filename);
    return teams;
}

Venue* load_venues(const char* filename, int* count) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open %s\n", filename);
        *count = 0;
        return NULL;
    }

    Venue* venues = malloc(sizeof(Venue) * MAX_TEAMS);
    if (!venues) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        *count = 0;
        return NULL;
    }

    char line[256];
    int venue_count = 0;

    // Skip header
    if (fgets(line, sizeof(line), file) == NULL) {
        fprintf(stderr, "Error: Empty file %s\n", filename);
        free(venues);
        fclose(file);
        *count = 0;
        return NULL;
    }

    while (fgets(line, sizeof(line), file) && venue_count < MAX_TEAMS) {
        char* tokens[6];
        int token_count = parse_csv_line(line, tokens, 6);

        if (token_count == 6) {
            int id = atoi(tokens[0]);
            double lat = atof(tokens[3]);
            double lon = atof(tokens[4]);
            int capacity = atoi(tokens[5]);
            Venue* venue = create_venue(id, tokens[1], tokens[2], capacity, lat, lon);
            if (venue) {
                venues[venue_count++] = *venue;
                free(venue);
            }
        }
    }

    fclose(file);
    *count = venue_count;
    printf("Loaded %d venues from %s\n", venue_count, filename);
    return venues;
}

double** load_distance_matrix(const char* filename, int team_count) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open %s\n", filename);
        return NULL;
    }

    // Allocate distance matrix
    double** matrix = malloc(sizeof(double*) * team_count);
    if (!matrix) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    for (int i = 0; i < team_count; i++) {
        matrix[i] = malloc(sizeof(double) * team_count);
        if (!matrix[i]) {
            fprintf(stderr, "Error: Memory allocation failed\n");
            for (int j = 0; j < i; j++) free(matrix[j]);
            free(matrix);
            fclose(file);
            return NULL;
        }
        // Initialize with 0
        for (int j = 0; j < team_count; j++) {
            matrix[i][j] = 0.0;
        }
    }

    char line[256];

    // Skip header
    if (fgets(line, sizeof(line), file) == NULL) {
        fprintf(stderr, "Error: Empty file %s\n", filename);
        for (int i = 0; i < team_count; i++) free(matrix[i]);
        free(matrix);
        fclose(file);
        return NULL;
    }

    int entries = 0;
    while (fgets(line, sizeof(line), file)) {
        char* tokens[3];
        int token_count = parse_csv_line(line, tokens, 3);

        if (token_count == 3) {
            int team1 = atoi(tokens[0]);
            int team2 = atoi(tokens[1]);
            double distance = atof(tokens[2]);

            if (team1 >= 0 && team1 < team_count && team2 >= 0 && team2 < team_count) {
                matrix[team1][team2] = distance;
                matrix[team2][team1] = distance; // Symmetric
                entries++;
            }
        }
    }

    fclose(file);
    printf("Loaded %d distance entries from %s\n", entries, filename);
    return matrix;
}
