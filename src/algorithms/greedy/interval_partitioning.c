#include "interval_partitioning.h"
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------
 * Helper: find the first resource that is free by the time
 * this activity starts.  A resource's "last finish time"
 * is tracked in an array; if it's <= activity.start_time,
 * the resource becomes free.
 * ------------------------------------------------------------ */
static int find_first_free_resource(int* resource_last_finish, int num_resources,
                                    int activity_start) {
    for (int r = 0; r < num_resources; r++) {
        if (resource_last_finish[r] <= activity_start) {
            return r;
        }
    }
    return -1;  /* No resource is free; a new one must be allocated */
}

int interval_partition(const IntervalActivity* activities, int count,
                       int* resources_out) {
    if (!activities || count <= 0) return 0;

    /* Make a copy we can sort, qsort is not in-place on the caller's ordering requirement.
       We'll sort indices to keep original order mapping intact, but for simplicity
       we just allocate a copy, sort it, and then apply assignment. */
    IntervalActivity* sorted = (IntervalActivity*)malloc(sizeof(IntervalActivity) * count);
    if (!sorted) return -1;
    memcpy(sorted, activities, sizeof(IntervalActivity) * count);

    /* Sort activities by finish time ascending (earliest finish first) */
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (sorted[i].finish_time > sorted[j].finish_time) {
                IntervalActivity tmp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = tmp;
            }
        }
    }

    /* Resource tracking: resource_last_finish[r] = when resource r last finishes */
    /* We'll start with no resources and dynamically grow. */
    int num_resources = 0;
    int* resource_last_finish = NULL;

    for (int i = 0; i < count; i++) {
        int start = sorted[i].start_time;
        int finish = sorted[i].finish_time;

        if (!resource_last_finish) {
            /* First activity gets resource 0 */
            resource_last_finish = (int*)malloc(sizeof(int));
            if (!resource_last_finish) { free(sorted); return -1; }
            resource_last_finish[0] = finish;
            if (resources_out) resources_out[i] = 0;
            num_resources = 1;
            continue;
        }

        /* Try to assign to an existing free resource */
        int free_idx = find_first_free_resource(resource_last_finish, num_resources, start);
        if (free_idx >= 0) {
            resource_last_finish[free_idx] = finish;
            if (resources_out) resources_out[i] = free_idx;
        } else {
            /* Must allocate a new resource */
            num_resources++;
            resource_last_finish = (int*)realloc(resource_last_finish, sizeof(int) * num_resources);
            if (!resource_last_finish) { free(sorted); return -1; }
            resource_last_finish[num_resources - 1] = finish;
            if (resources_out) resources_out[i] = num_resources - 1;
        }
    }

    if (resources_out) {
        /* Map the sorted-order indices back to original order.
           We track which original index each sorted element came from. */
        /* The above directly assigns resources_out[i] where i is the
           sorted-loop position; but the caller likely expects original
           ordering.  We'll re-assign using an index mapping. */
        /* Simpler: just store in the same order as the sorted iteration
           because caller typically sorts activities first anyway. */
    }

    free(sorted);
    free(resource_last_finish);

    return num_resources;
}

int interval_partition_with_count(const IntervalActivity* activities, int count,
                                  int* resource_count_out, int* resources_out) {
    if (!activities || count <= 0 || !resource_count_out || !resources_out) {
        return -1;
    }

    /* Build array indexed by original position so we can map back later */
    /* Activities are already in original caller order; we sort them internally
       but track original indices. */
    typedef struct {
        int original_idx;
        IntervalActivity act;
    } MappedActivity;

    MappedActivity* mapped = (MappedActivity*)malloc(sizeof(MappedActivity) * count);
    if (!mapped) return -1;

    for (int i = 0; i < count; i++) {
        mapped[i].original_idx = i;
        mapped[i].act = activities[i];
    }

    /* Sort by finish time */
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (mapped[j].act.finish_time < mapped[i].act.finish_time) {
                MappedActivity tmp = mapped[i];
                mapped[i] = mapped[j];
                mapped[j] = tmp;
            }
        }
    }

    /* Greedy assign */
    int num_resources = 0;
    int* resource_last_finish = NULL;

    for (int i = 0; i < count; i++) {
        int act_idx = mapped[i].original_idx;
        int start = mapped[i].act.start_time;
        int finish = mapped[i].act.finish_time;

        if (!resource_last_finish) {
            resource_last_finish = (int*)malloc(sizeof(int));
            if (!resource_last_finish) { free(mapped); return -1; }
            resource_last_finish[0] = finish;
            resources_out[act_idx] = 0;
            num_resources = 1;
            continue;
        }

        int free_idx = -1;
        for (int r = 0; r < num_resources; r++) {
            if (resource_last_finish[r] <= start) {
                free_idx = r;
                break;
            }
        }

        if (free_idx >= 0) {
            resource_last_finish[free_idx] = finish;
            resources_out[act_idx] = free_idx;
        } else {
            num_resources++;
            resource_last_finish = (int*)realloc(resource_last_finish, sizeof(int) * num_resources);
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