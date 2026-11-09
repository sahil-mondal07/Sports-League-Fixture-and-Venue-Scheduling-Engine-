#ifndef INTERVAL_PARTITIONING_H
#define INTERVAL_PARTITIONING_H

#include "../../utils/common.h"

/**
 * @brief Represents a single activity requesting a venue/time resource.
 * Each activity needs a venue during a specific time interval.
 */
typedef struct {
    int id;                /**< Unique activity identifier */
    int venue_id;          /**< Preferred or required venue ID (unused for general partition) */
    int start_time;        /**< Start time (e.g. minute of day) */
    int finish_time;       /**< Finish time */
} IntervalActivity;

/**
 * @brief Compare two interval activities by their finish times (ascending).
 * Used by qsort for greedy scheduling of earliest-finish-first.
 */
int compare_interval_activities_by_finish_time(const void* a, const void* b);

/**
 * @brief Interval Partitioning (Greedy) Algorithm.
 * Determines the minimum number of parallel resources (venues/rooms) needed
 * so that no activity overlaps another on the same resource.
 *
 * The algorithm: Sort activities by finish time. For each activity, assign it
 * to the first available resource. If none is available, allocate a new one.
 *
 * @param activities Array of interval activities to schedule
 * @param count Number of activities
 * @param resources_out Pre-allocated array of size count to store the assigned resource
 *                    for each activity, or NULL if only the count is needed
 * @return int Minimum number of resources (venues/rooms) needed
 */
int interval_partition(const IntervalActivity* activities, int count,
                       int* resources_out);

/**
 * @brief Compute the number of resources needed and assign each activity
 * to a specific resource (0-indexed).
 *
 * @param activities Array of interval activities to schedule
 * @param count Number of activities
 * @param resource_count_out Pointer to store the minimum number of resources needed
 * @param resources_out Pre-allocated array of size count to store assigned resource index
 *                     for each corresponding activity
 * @return int 0 on success, -1 on allocation failure
 */
int interval_partition_with_count(const IntervalActivity* activities, int count,
                                  int* resource_count_out, int* resources_out);

#endif /* INTERVAL_PARTITIONING_H */