#ifndef QUICK_SORT_H
#define QUICK_SORT_H

#include "../../data/fixture.h"
#include "../../data/team.h"
#include "../../utils/common.h"

/**
 * Quick sort for Fixture array (sorted by round)
 * @param fixtures Array of fixtures to sort
 * @param left Left index
 * @param right Right index
 */
void quick_sort_fixtures(Fixture* fixtures, int left, int right);

/**
 * Quick sort for Team array (sorted by ranking)
 * @param teams Array of teams to sort
 * @param left Left index
 * @param right Right index
 */
void quick_sort_teams(Team* teams, int left, int right);

#endif /* QUICK_SORT_H */