#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_NAME_LENGTH 50
#define MAX_TEAMS 100
#define MAX_VENUES 50
#define MAX_FIXTURES 10000

#define TRUE 1
#define FALSE 0

#define SUCCESS 0
#define ERROR -1

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define EARTH_RADIUS_KM 6371.0

typedef int bool;

// Date structure for tournament scheduling
typedef struct {
    int day;
    int month;
    int year;
} Date;

// Tournament configuration
typedef struct {
    Date start_date;
    Date end_date;
    int min_rest_days;      // Minimum days between matches for same team
    int matches_per_week;   // Max matches per team per week
} TournamentConfig;

// Date utility functions
int date_to_days(Date* date);
Date days_to_date(int days);
int days_between(Date* d1, Date* d2);
int is_valid_date(Date* date);
void print_date(Date* date);
Date add_days(Date* date, int days);

#endif
