#include "activity_selection.h"
#include <stdlib.h>

int compare_activities_by_finish_time(const void* a, const void* b) {
    const Activity* act_a = (const Activity*)a;
    const Activity* act_b = (const Activity*)b;

    if (act_a->finish_time != act_b->finish_time) {
        return (act_a->finish_time > act_b->finish_time) - (act_a->finish_time < act_b->finish_time);
    }
    return (act_a->start_time > act_b->start_time) - (act_a->start_time < act_b->start_time);
}

void sort_activities_by_finish_time(Activity* activities, int count) {
    if (!activities || count <= 1) return;
    qsort(activities, (size_t)count, sizeof(Activity), compare_activities_by_finish_time);
}

int select_activities(Activity* activities, int count, Activity* selected_out) {
    if (!activities || count <= 0) return 0;

    /* Sort candidate activities by finish time ascending */
    sort_activities_by_finish_time(activities, count);

    int selected_count = 0;

    /* The first activity always has the earliest finish time */
    if (selected_out) {
        selected_out[selected_count] = activities[0];
    }
    int last_finish = activities[0].finish_time;
    selected_count++;

    /* Iterate through remaining activities greedily */
    for (int i = 1; i < count; i++) {
        if (activities[i].start_time >= last_finish) {
            if (selected_out) {
                selected_out[selected_count] = activities[i];
            }
            last_finish = activities[i].finish_time;
            selected_count++;
        }
    }

    return selected_count;
}

int select_activities_for_venue(const Activity* activities, int count,
                               int venue_id, Activity* selected_out) {
    if (!activities || count <= 0) return 0;

    Activity* venue_activities = (Activity*)malloc(sizeof(Activity) * count);
    if (!venue_activities) return 0;

    int venue_count = 0;
    for (int i = 0; i < count; i++) {
        if (activities[i].venue_id == venue_id) {
            venue_activities[venue_count++] = activities[i];
        }
    }

    if (venue_count == 0) {
        free(venue_activities);
        return 0;
    }

    int selected = select_activities(venue_activities, venue_count, selected_out);
    free(venue_activities);
    return selected;
}
