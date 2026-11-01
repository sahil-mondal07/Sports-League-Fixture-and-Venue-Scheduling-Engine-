#ifndef VENUE_H
#define VENUE_H

#include "../utils/common.h"

typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    char city[MAX_NAME_LENGTH];
    int capacity;
    float latitude;
    float longitude;
    bool is_available;
} Venue;

Venue* create_venue(int id, const char* name, const char* city,
                    int capacity, float lat, float lon);

void print_venue(const Venue* venue);

void free_venue(Venue* venue);

int compare_venues_by_capacity(const void* a, const void* b);

float calculate_distance(float lat1, float lon1, float lat2, float lon2);

#endif
