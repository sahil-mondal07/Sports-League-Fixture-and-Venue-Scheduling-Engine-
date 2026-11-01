#include "venue.h"

Venue* create_venue(int id, const char* name, const char* city,
                    int capacity, float lat, float lon) {
    Venue* venue = (Venue*)malloc(sizeof(Venue));
    if (!venue) return NULL;

    venue->id = id;
    strncpy(venue->name, name, MAX_NAME_LENGTH - 1);
    venue->name[MAX_NAME_LENGTH - 1] = '\0';
    strncpy(venue->city, city, MAX_NAME_LENGTH - 1);
    venue->city[MAX_NAME_LENGTH - 1] = '\0';
    venue->capacity = capacity;
    venue->latitude = lat;
    venue->longitude = lon;
    venue->is_available = TRUE;

    return venue;
}

void print_venue(const Venue* venue) {
    if (!venue) return;
    printf("Venue #%d: %s in %s (Capacity: %d, Available: %s)\n",
           venue->id, venue->name, venue->city, venue->capacity,
           venue->is_available ? "Yes" : "No");
}

void free_venue(Venue* venue) {
    if (venue) free(venue);
}

int compare_venues_by_capacity(const void* a, const void* b) {
    Venue* v1 = (Venue*)a;
    Venue* v2 = (Venue*)b;
    return v2->capacity - v1->capacity;
}

float calculate_distance(float lat1, float lon1, float lat2, float lon2) {
    float dlat = (lat2 - lat1) * M_PI / 180.0;
    float dlon = (lon2 - lon1) * M_PI / 180.0;

    lat1 = lat1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;

    float a = sin(dlat/2) * sin(dlat/2) +
              sin(dlon/2) * sin(dlon/2) * cos(lat1) * cos(lat2);
    float c = 2 * atan2(sqrt(a), sqrt(1-a));

    return EARTH_RADIUS_KM * c;
}
