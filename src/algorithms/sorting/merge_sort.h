#ifndef MERGE_SORT_H
#define MERGE_SORT_H

#include "../../data/team.h"
#include "../../utils/common.h"

// Merge sort for teams by ranking
void merge_sort_teams(Team* teams, int left, int right);

// Helper function to merge two sorted subarrays
void merge_teams(Team* teams, int left, int mid, int right);

// Print sorted teams
void print_sorted_teams(Team* teams, int count);

#endif
