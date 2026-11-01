# Data Structures Reference

## Core Data Structures in C

### 1. Team Structure

```c
// team.h
#ifndef TEAM_H
#define TEAM_H

#define MAX_NAME_LENGTH 50
#define MAX_TEAMS 100

typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    char home_stadium[MAX_NAME_LENGTH];
    float latitude;   // for distance calculation
    float longitude;
    int ranking;      // current league position
    int points;       // season points
} Team;

// Team operations
Team* create_team(int id, const char* name, const char* stadium, 
                  float lat, float lon, int ranking, int points);
void print_team(Team* team);
void free_team(Team* team);
int compare_teams_by_ranking(const void* a, const void* b);  // for qsort
int compare_teams_by_points(const void* a, const void* b);

#endif
```

**What it stores:** LaLiga team information  
**Example:** Real Madrid (id=1, stadium="Santiago Bernabéu", lat=40.4530, lon=-3.6883)

---

### 2. Venue Structure

```c
// venue.h
#ifndef VENUE_H
#define VENUE_H

#define MAX_VENUE_NAME 50
#define MAX_VENUES 50

typedef struct {
    int id;
    char name[MAX_VENUE_NAME];
    int capacity;
    float latitude;
    float longitude;
    int is_available;  // 1 if available, 0 if not
    char city[MAX_NAME_LENGTH];
} Venue;

typedef struct {
    int venue_id;
    int day;       // day number (0-365)
    int time_slot; // 0=morning, 1=afternoon, 2=evening
} VenueAvailability;

Venue* create_venue(int id, const char* name, int capacity, 
                    float lat, float lon, const char* city);
void print_venue(Venue* venue);
int compare_venues_by_capacity(const void* a, const void* b);
float calculate_distance(float lat1, float lon1, float lat2, float lon2);

#endif
```

**What it stores:** Stadium/venue details  
**Example:** Camp Nou (id=1, capacity=99354, city="Barcelona")

---

### 3. Fixture Structure

```c
// fixture.h
#ifndef FIXTURE_H
#define FIXTURE_H

#define MAX_FIXTURES 1000

typedef struct {
    int id;
    int round;          // matchday number (1-38 for LaLiga)
    int home_team_id;
    int away_team_id;
    int venue_id;
    int day;            // day number
    int time_slot;      // 0=morning, 1=afternoon, 2=evening
    float travel_cost;  // distance-based cost for away team
    int is_valid;       // 1 if passes all constraints
} Fixture;

Fixture* create_fixture(int id, int round, int home_team, int away_team,
                       int venue, int day, int time_slot);
void print_fixture(Fixture* fixture);
int validate_fixture(Fixture* fixture, Fixture* all_fixtures, int count);
int compare_fixtures_by_round(const void* a, const void* b);

#endif
```

**What it stores:** Individual match information  
**Example:** Round 10, Real Madrid vs Barcelona at Santiago Bernabéu, Day 100, Evening slot

---

### 4. Graph Structure (Adjacency List)

```c
// graph.h
#ifndef GRAPH_H
#define GRAPH_H

#define MAX_VERTICES 100

typedef struct Edge {
    int dest;           // destination vertex
    float weight;       // edge weight (distance, cost, etc.)
    struct Edge* next;  // next edge in adjacency list
} Edge;

typedef struct {
    int num_vertices;
    Edge* adj_list[MAX_VERTICES];  // array of adjacency lists
} Graph;

// Graph operations
Graph* create_graph(int vertices);
void add_edge(Graph* g, int src, int dest, float weight);
void add_undirected_edge(Graph* g, int u, int v, float weight);
void print_graph(Graph* g);
void free_graph(Graph* g);

// For distance matrix representation (alternative)
typedef struct {
    int size;
    float distances[MAX_VERTICES][MAX_VERTICES];
} DistanceMatrix;

DistanceMatrix* create_distance_matrix(int size);
void set_distance(DistanceMatrix* dm, int from, int to, float dist);
float get_distance(DistanceMatrix* dm, int from, int to);

#endif
```

**What it stores:** Venue connectivity, travel distances  
**Example:** Graph where vertices = cities, edges = distances between them

---

### 5. Schedule Structure

```c
// fixture.h (continued)

typedef struct {
    Fixture* fixtures;
    int fixture_count;
    int total_rounds;
    float total_travel_cost;
    float fairness_score;  // computed metric
    int conflict_count;    // number of constraint violations
} Schedule;

Schedule* create_schedule(int max_fixtures);
void add_fixture_to_schedule(Schedule* s, Fixture* f);
void print_schedule(Schedule* s);
float calculate_total_travel_cost(Schedule* s, Team* teams, DistanceMatrix* dm);
float calculate_fairness_score(Schedule* s, int num_teams);
void free_schedule(Schedule* s);
```

**What it stores:** Complete season fixture list with metrics

---

### 6. Priority Queue (for Dijkstra, Branch & Bound)

```c
// utils/priority_queue.h
#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

typedef struct {
    int vertex;
    float priority;  // distance/cost
} PQNode;

typedef struct {
    PQNode* nodes;
    int size;
    int capacity;
} PriorityQueue;

PriorityQueue* create_pq(int capacity);
void pq_insert(PriorityQueue* pq, int vertex, float priority);
PQNode pq_extract_min(PriorityQueue* pq);
int pq_is_empty(PriorityQueue* pq);
void free_pq(PriorityQueue* pq);

#endif
```

**What it's for:** Dijkstra's algorithm, Prim's algorithm, Least-Cost Branch & Bound

---

### 7. Union-Find (for Kruskal's MST)

```c
// utils/union_find.h
#ifndef UNION_FIND_H
#define UNION_FIND_H

typedef struct {
    int* parent;
    int* rank;
    int size;
} UnionFind;

UnionFind* create_uf(int n);
int uf_find(UnionFind* uf, int x);
void uf_union(UnionFind* uf, int x, int y);
void free_uf(UnionFind* uf);

#endif
```

**What it's for:** Kruskal's MST algorithm

---

### 8. Result Structures for Algorithms

```c
// algorithms/common.h
#ifndef ALGO_COMMON_H
#define ALGO_COMMON_H

// For Dijkstra/Bellman-Ford results
typedef struct {
    float* distances;   // distances[i] = shortest distance to vertex i
    int* predecessors;  // predecessors[i] = previous vertex in path to i
    int source;
    int num_vertices;
} ShortestPathResult;

// For MST results (Prim/Kruskal)
typedef struct {
    Edge* edges;        // edges in the MST
    int edge_count;
    float total_weight;
} MSTResult;

// For matching results
typedef struct {
    int* matches;       // matches[i] = j means team i matched to venue j (-1 if unmatched)
    int match_count;
    int size;
} MatchingResult;

// For knapsack results
typedef struct {
    int* selected;      // selected[i] = 1 if item i is selected
    int total_value;
    int total_weight;
} KnapsackResult;

#endif
```

---

## Memory Management Notes

**Important for C:**
- Always `malloc()` for dynamic structures
- Always `free()` when done
- Use `MAX_*` constants to avoid buffer overflows
- Validate array bounds before access

**C++ Alternative (if we use C++ for some modules):**
- Use `std::vector<Team>` instead of `Team*` array
- Use `std::string` instead of `char[]`
- Automatic memory management (RAII)

**I'll explain any C++ code if we use it**, but we'll keep it minimal and primarily use C.

---

**Next:** Testing strategy document, then start coding!
