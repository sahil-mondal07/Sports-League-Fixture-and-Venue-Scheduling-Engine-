/* Desktop GUI bridge: tab-delimited output, one record per line.
 * All fixture generation, validation and routing use the existing C modules. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data/loader.h"
#include "data/graph.h"
#include "algorithms/greedy/round_robin.h"
#include "algorithms/graph/dijkstra.h"
#include "scheduling/validator.h"

static void safe_text(const char* s) {
    for (; *s; ++s) putchar((*s == '\t' || *s == '\r' || *s == '\n') ? ' ' : *s);
}
static void show_error(const char* s) { printf("ERROR\t%s\n", s); }
int main(int argc, char** argv) {
    if (argc < 5) { show_error("Usage: gui_bridge action teams.csv venues.csv ..."); return 2; }
    const char* action = argv[1];
    int nt=0, nv=0;
    Team* teams = load_teams(argv[2], &nt);
    Venue* venues = load_venues(argv[3], &nv);
    if (!teams || !venues || nt < 2 || nv < 1) {
        show_error("Could not load at least two teams and one venue");
        free(teams); free(venues); return 2;
    }
    if (strcmp(action,"list") == 0) {
        printf("META\t%d\t%d\n",nt,nv);
        for(int i=0;i<nt;i++) { printf("TEAM\t%d\t",i);safe_text(teams[i].name);printf("\t%d\n",teams[i].ranking); }
        for(int i=0;i<nv;i++) { printf("VENUE\t%d\t",i);safe_text(venues[i].name);printf("\t");safe_text(venues[i].city);printf("\t%d\t%.6f\t%.6f\n",venues[i].capacity,venues[i].latitude,venues[i].longitude); }
    } else if(strcmp(action,"schedule") == 0) {
        if(argc < 12) { show_error("Schedule requires start/end dates and rest days"); free(teams);free(venues);return 2; }
        TournamentConfig cfg = {{atoi(argv[4]),atoi(argv[5]),atoi(argv[6])}, {atoi(argv[7]),atoi(argv[8]),atoi(argv[9])}, atoi(argv[10]), atoi(argv[11])};
        FixtureSchedule* schedule = round_robin_schedule(teams,nt,venues,nv,&cfg);
        if (!schedule) { show_error("Fixture generation failed; check venues, slots and configuration");free(teams);free(venues);return 3; }
        printf("SCHEDULE\t%d\t%d\n",schedule->total_rounds,schedule->count);
        for(int i=0;i<schedule->count;i++) {
            Fixture* f=&schedule->fixtures[i];
            printf("FIXTURE\t%d\t%d\t%d\t%d\t%d\t%d\t%02d/%02d/%04d\t%d\n",f->id,f->round,f->home_team_id,f->away_team_id,f->venue_id,f->day,f->match_date.day,f->match_date.month,f->match_date.year,f->time_slot);
        }
        ValidationOptions opt={0,1};ValidationReport report={0};
        int rc=validate_schedule(schedule->fixtures,schedule->count,nt,venues,nv,&cfg,&opt,&report);
        if(rc<0) show_error("Validation failed");
        else {
            printf("VALIDATION\t%d\n",report.count);
            for(int i=0;i<report.count;i++) {
                Violation* v=&report.items[i];
                printf("VIOLATION\t%s\t%d\t%d\t",violation_name(v->type),v->fixture_id,v->other_fixture_id);safe_text(v->message);putchar('\n');
            }
        }
        free_validation_report(&report);
        free_fixture_schedule(schedule);
    } else if(strcmp(action,"route") == 0) {
        if(argc<6) {show_error("Route requires source and destination IDs");free(teams);free(venues);return 2;}
        int src=atoi(argv[4]),dst=atoi(argv[5]);
        if(src<0||dst<0||src>=nv||dst>=nv) {show_error("Invalid venue ID");free(teams);free(venues);return 2;}
        Graph* graph=create_graph(nv);
        if(!graph) {show_error("Cannot allocate graph");free(teams);free(venues);return 3;}
        for(int i=0;i<nv;i++)for(int j=i+1;j<nv;j++){
            float d=calculate_distance(venues[i].latitude,venues[i].longitude,venues[j].latitude,venues[j].longitude);
            add_undirected_edge(graph,i,j,d);
        }
        DijkstraResult* result=dijkstra_shortest_path(graph,src);
        if(!result) show_error("Dijkstra failed");
        else {
            int* path=malloc(sizeof(int)*(size_t)nv);
            if(!path) show_error("Out of memory");
            else {
                int len=dijkstra_get_path(result,dst,path);
                printf("ROUTE\t%.3f\t%d",result->dist[dst],len);
                for(int i=0;i<len;i++)printf("\t%d",path[i]);
                putchar('\n');free(path);
            }
            free_dijkstra_result(result);
        }
        free_graph(graph);
    } else show_error("Unknown bridge action");
    free(teams);free(venues);return 0;
}
