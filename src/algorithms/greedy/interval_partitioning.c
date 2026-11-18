#include "interval_partitioning.h"
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------
 * Comparison function for qsort: sort by start time ascending.
 * If start times are equal, sort by finish time ascending.
 * ------------------------------------------------------------ */
static int cmp_start_asc(const void* a, const void* b) {
    const IntervalActivity* pa = (const IntervalActivity*)a;
    const IntervalActivity* pb = (const IntervalActivity*)b;
    if (pa->start_time < pb->start_time) return -1;
    if (pa->start_time > pb->start_time) return 1;
    return (pa->finish_time < pb->finish_time) ? -1 : (pa->finish_time > pb->finish_time) ? 1 : 0;
}

/* ------------------------------------------------------------
 * Interval partitioning (greedy).
 * Sort intervals by start time ascending.
 * Assign each interval to a resource whose last finish time is <= its start.
 * Intervals that only touch (finish == start) do not overlap.
 * The number of resources produced equals the maximum overlap depth.
 * ------------------------------------------------------------ */

int interval_partition(const IntervalActivity* activities, int count,
                       int* resources_out) {
    if (!activities || count <= 0) return 0;

    /* Make a copy and sort by start time ascending */
    IntervalActivity* sorted = (IntervalActivity*)malloc(sizeof(IntervalActivity) * count);
    if (!sorted) return -1;
    memcpy(sorted, activities, sizeof(IntervalActivity) * count);

    qsort(sorted, (size_t)count, sizeof(IntervalActivity), cmp_start_asc);

    /* Track original indices after sort, so resources_out maps back correctly.
     * Original index i -> sorted position maps via this array. */
    typedef struct {
        int original_idx;
        IntervalActivity act;
    } MappedActivity;

    MappedActivity* mapped = (MappedActivity*)malloc(sizeof(MappedActivity) * count);
    if (!mapped) { free(sorted); return -1; }

    for (int i = 0; i < count; i++) {
        mapped[i].original_idx = i;
        mapped[i].act = sorted[i];
    }

    free(sorted);

    /* Resource tracking: resource_last_finish[r] = when resource r last finishes */
    int num_resources = 0;
    int* resource_last_finish = NULL;

    for (int i = 0; i < count; i++) {
        int act_idx = mapped[i].original_idx;
        int start = mapped[i].act.start_time;
        int finish = mapped[i].act.finish_time;

        /* Try to assign to an existing resource whose last finish <= start.
         * Intervals that only touch (finish == start) do not overlap. */
        int assigned = 0;
        for (int r = 0; r < num_resources; r++) {
            if (resource_last_finish[r] <= start) {
                /* This resource is free; assign the interval to it */
                resource_last_finish[r] = finish;
                if (resources_out) resources_out[act_idx] = r;
                assigned = 1;
                break;
            }
        }

        if (!assigned) {
            /* Must allocate a new resource */
            num_resources++;
            resource_last_finish = (int*)realloc(resource_last_finish,
                                               sizeof(int) * num_resources);
            if (!resource_last_finish) { free(mapped); return -1; }
            resource_last_finish[num_resources - 1] = finish;
            if (resources_out) resources_out[act_idx] = num_resources - 1;
        }
    }

    free(mapped);
    free(resource_last_finish);

    return num_resources;
}

int interval_partition_with_count(const IntervalActivity* activities, int count,
                                  int* resource_count_out, int* resources_out) {
    if (!activities || count <= 0 || !resource_count_out || !resources_out) {
        return -1;
    }

    /* Make a copy and sort by start time ascending */
    IntervalActivity* sorted = (IntervalActivity*)malloc(sizeof(IntervalActivity) * count);
    if (!sorted) return -1;
    memcpy(sorted, activities, sizeof(IntervalActivity) * count);

    qsort(sorted, (size_t)count, sizeof(IntervalActivity), cmp_start_asc);

    /* Resource tracking */
    int num_resources = 0;
    int* resource_last_finish = NULL;

    /* Map sorted-order results back to original indices.
       We'll track original indices alongside the sorted activities. */
    typedef struct {
        int original_idx;
        IntervalActivity act;
    } MappedActivity;

    MappedActivity* mapped = (MappedActivity*)malloc(sizeof(MappedActivity) * count);
    if (!mapped) { free(sorted); return -1; }

    for (int i = 0; i < count; i++) {
        mapped[i].original_idx = i;
        mapped[i].act = sorted[i];
    }

    free(sorted);

    /* Greedy assign, tracking original indices */
    num_resources = 0;
    resource_last_finish = NULL;

    for (int i = 0; i < count; i++) {
        int act_idx = mapped[i].original_idx;
        int start = mapped[i].act.start_time;
        int finish = mapped[i].act.finish_time;

        /* Try to assign to an existing resource whose last finish <= start */
        int assigned = 0;
        for (int r = 0; r < num_resources; r++) {
            if (resource_last_finish[r] <= start) {
                resource_last_finish[r] = finish;
                resources_out[act_idx] = r;
                assigned = 1;
                break;
            }
        }

        if (!assigned) {
            /* Must allocate a new resource */
            num_resources++;
            resource_last_finish = (int*)realloc(resource_last_finish,
                                               sizeof(int) * num_resources);
            if (!resource_last_finish) { free(mapped); return -1; }
            resource_last_finish[num_resources - 1] = finish;
            resources_out[act_idx] = num_resources - 1;
        }
    }

    *resource_count_out = num_resources;
    free(mapped);
    free(resource_last_finish);

    return 0;
}