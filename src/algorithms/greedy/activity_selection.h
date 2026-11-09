#ifndef ACTIVITY_SELECTION_H
#define ACTIVITY_SELECTION_H

#include "../../utils/common.h"

/**
 * @brief Represents a scheduled event / match activity requesting venue time.
 */
typedef struct {
    int id;             /**< Unique activity or match request identifier */
    int venue_id;       /**< Venue ID where the match/event takes place */
    int start_time;     /**< Start time (e.g., minute of day, hour, or timestamp) */
    int finish_time;    /**< Finish/end time */
} Activity;

/**
 * @brief Compare two activities by finish time (ascending order).
 * If finish times are identical, break ties by start time ascending.
 */
int compare_activities_by_finish_time(const void* a, const void* b);

/**
 * @brief Sort an array of activities in-place by finish time ascending.
 *
 * @param activities Array of activities
 * @param count Number of activities in array
 */
void sort_activities_by_finish_time(Activity* activities, int count);

/**
 * @brief Greedy Activity Selection algorithm.
 * Selects the maximum number of mutually compatible, non-overlapping
 * activities for a single venue/resource.
 *
 * @param activities Array of input activities (will be sorted by finish time)
 * @param count Total number of candidate activities
 * @param selected_out Pre-allocated array to store selected activities (size >= count), or NULL
 * @return int Number of selected non-overlapping activities
 */
int select_activities(Activity* activities, int count, Activity* selected_out);

/**
 * @brief Filter activities for a specific venue, then run activity selection.
 *
 * @param activities Array of all activities
 * @param count Total number of candidate activities
 * @param venue_id ID of the venue to filter and schedule for
 * @param selected_out Pre-allocated array to store selected activities, or NULL
 * @return int Number of selected non-overlapping activities for this venue
 */
int select_activities_for_venue(const Activity* activities, int count,
                               int venue_id, Activity* selected_out);

#endif /* ACTIVITY_SELECTION_H */
