#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "data/team.h"
#include "data/venue.h"
#include "data/fixture.h"
#include "data/graph.h"
#include "data/loader.h"
#include "utils/common.h"
#include "algorithms/greedy/round_robin.h"
#include "algorithms/sorting/merge_sort.h"
#include "algorithms/searching/binary_search.h"

// Global data structures
Team* teams = NULL;
Venue* venues = NULL;
double** distance_matrix = NULL;
Graph* graph = NULL;
int team_count = 0;
int venue_count = 0;
TournamentConfig tournament;

void free_all_data() {
    if (teams) free(teams);
    if (venues) free(venues);
    if (distance_matrix) {
        for (int i = 0; i < team_count; i++) {
            free(distance_matrix[i]);
        }
        free(distance_matrix);
    }
    if (graph) free_graph(graph);
}

void load_from_csv() {
    printf("\n=== Load Data from CSV ===\n");
    printf("\n================================================\n");
    printf("          SELECT DATASET TO LOAD                \n");
    printf("================================================\n\n");

    printf("FULL LEAGUES (For Demos & Evaluation):\n");
    printf("  [laliga]      - Spanish LaLiga Football (20 teams)\n");
    printf("  [euroleague]  - EuroLeague Basketball (12 teams)\n");
    printf("  [volleyball]  - Italian SuperLega Volleyball (12 teams)\n");
    printf("  [ipl]         - Indian Premier League Cricket (10 teams)\n");
    printf("\n");
    printf("TESTING & DEVELOPMENT:\n");
    printf("  [test]        - Small Test Dataset (4 teams - QUICK)\n");
    printf("\n");
    printf("CUSTOM:\n");
    printf("  [custom]      - Enter custom file paths\n");
    printf("\n");
    printf("Type dataset name: ");

    char choice[32];
    if (fgets(choice, sizeof(choice), stdin) == NULL) {
        printf("Invalid input.\n");
        return;
    }
    choice[strcspn(choice, "\n")] = 0;

    // Convert to lowercase for easier matching
    for (int i = 0; choice[i]; i++) {
        choice[i] = tolower(choice[i]);
    }

    char teams_file[256], venues_file[256], distances_file[256];

    if (strcmp(choice, "laliga") == 0) {
        strcpy(teams_file, "data/football/laliga_teams.csv");
        strcpy(venues_file, "data/football/laliga_venues.csv");
        strcpy(distances_file, "data/football/laliga_distances.csv");
        printf("\n[Loading LaLiga 2026-27 Season - 20 Teams]\n");
    } else if (strcmp(choice, "euroleague") == 0) {
        strcpy(teams_file, "data/basketball/euroleague_teams.csv");
        strcpy(venues_file, "data/basketball/euroleague_venues.csv");
        strcpy(distances_file, "");
        printf("\n[Loading EuroLeague Basketball - 12 Teams]\n");
    } else if (strcmp(choice, "volleyball") == 0) {
        strcpy(teams_file, "data/volleyball/italy_teams.csv");
        strcpy(venues_file, "data/volleyball/italy_venues.csv");
        strcpy(distances_file, "");
        printf("\n[Loading Italian SuperLega Volleyball - 12 Teams]\n");
    } else if (strcmp(choice, "ipl") == 0) {
        strcpy(teams_file, "data/cricket/ipl_teams.csv");
        strcpy(venues_file, "data/cricket/ipl_venues.csv");
        strcpy(distances_file, "");
        printf("\n[Loading IPL Cricket - 10 Teams]\n");
    } else if (strcmp(choice, "test") == 0) {
        strcpy(teams_file, "data/test/small_teams.csv");
        strcpy(venues_file, "data/test/small_venues.csv");
        strcpy(distances_file, "data/test/small_distances.csv");
        printf("\n[Loading TEST Dataset - 4 Teams for Quick Testing]\n");
    } else if (strcmp(choice, "custom") == 0) {
        printf("\nEnter teams CSV path: ");
        fgets(teams_file, sizeof(teams_file), stdin);
        teams_file[strcspn(teams_file, "\n")] = 0;

        printf("Enter venues CSV path: ");
        fgets(venues_file, sizeof(venues_file), stdin);
        venues_file[strcspn(venues_file, "\n")] = 0;

        printf("Enter distances CSV path (or press Enter to skip): ");
        fgets(distances_file, sizeof(distances_file), stdin);
        distances_file[strcspn(distances_file, "\n")] = 0;
        printf("\n[Loading Custom Dataset]\n");
    } else {
        printf("\nERROR: Unknown dataset: '%s'\n", choice);
        printf("Valid options: laliga, euroleague, volleyball, ipl, test, custom\n");
        return;
    }

    free_all_data();

    teams = load_teams(teams_file, &team_count);
    venues = load_venues(venues_file, &venue_count);

    if (teams && venues) {
        if (strlen(distances_file) > 0) {
            distance_matrix = load_distance_matrix(distances_file, team_count);
        } else {
            printf("No distance file - will calculate on demand.\n");
        }
        printf("\nSUCCESS: Data loaded successfully!\n");
        printf("  Teams: %d | Venues: %d\n", team_count, venue_count);
    } else {
        printf("\nERROR: Failed to load data. Check file paths.\n");
    }
}

// Pre-recorded Indian cities for quick location selection
typedef struct {
    char name[MAX_NAME_LENGTH];
    float latitude;
    float longitude;
} City;

City indian_cities[] = {
    {"Mumbai", 19.0760, 72.8777},
    {"Delhi", 28.7041, 77.1025},
    {"Bangalore", 12.9716, 77.5946},
    {"Chennai", 13.0827, 80.2707},
    {"Hyderabad", 17.3850, 78.4867},
    {"Pune", 18.5204, 73.8567},
    {"Kolkata", 22.5726, 88.3639},
    {"Ahmedabad", 23.0225, 72.5714}
};

#define NUM_CITIES 8

void display_cities() {
    printf("\n=== Available Cities (Pre-recorded Coordinates) ===\n");
    for (int i = 0; i < NUM_CITIES; i++) {
        printf("  %d) %-15s (%.4f, %.4f)\n", i + 1, indian_cities[i].name,
               indian_cities[i].latitude, indian_cities[i].longitude);
    }
    printf("  %d) Manual Entry (enter own coordinates)\n", NUM_CITIES + 1);
}

float* get_location_hybrid(const char* entity_name) {
    static float location[2];
    int choice;

    display_cities();
    printf("\n%s - Pick location (%d-%d): ", entity_name, 1, NUM_CITIES + 1);
    scanf("%d", &choice);
    getchar();

    if (choice >= 1 && choice <= NUM_CITIES) {
        location[0] = indian_cities[choice - 1].latitude;
        location[1] = indian_cities[choice - 1].longitude;
        printf("  Selected: %s\n", indian_cities[choice - 1].name);
    } else if (choice == NUM_CITIES + 1) {
        printf("  Enter Latitude: ");
        scanf("%f", &location[0]);
        printf("  Enter Longitude: ");
        scanf("%f", &location[1]);
        getchar();
        printf("  Coordinates: (%.4f, %.4f)\n", location[0], location[1]);
    } else {
        printf("  Invalid choice. Using Mumbai defaults.\n");
        location[0] = 19.0760;
        location[1] = 72.8777;
    }

    return location;
}

void manual_entry() {
    printf("\n=== Manual Entry - Create Custom League ===\n");
    printf("How many teams? (2-%d): ", MAX_TEAMS);
    scanf("%d", &team_count);
    getchar();

    if (team_count < 2 || team_count > MAX_TEAMS) {
        printf("Invalid count. Must be between 2 and %d.\n", MAX_TEAMS);
        return;
    }

    free_all_data();
    teams = malloc(sizeof(Team) * team_count);

    printf("\n--- ENTERING TEAMS ---\n");
    for (int i = 0; i < team_count; i++) {
        char name[MAX_NAME_LENGTH];
        int rating;

        printf("\nTeam %d:\n", i + 1);
        printf("  Name: ");
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = 0;

        printf("  Rating (0-100): ");
        scanf("%d", &rating);
        getchar();

        // Hybrid location selection - user picks from list or enters manual
        float* loc = get_location_hybrid("Team location");

        Team* team = create_team(i, name, "", loc[0], loc[1], rating, 0);
        if (team) {
            teams[i] = *team;
            free(team);
        }
    }

    printf("\nSUCCESS: %d teams entered!\n\n", team_count);

    // Ask for venues
    printf("How many venues? (1-%d): ", team_count);
    scanf("%d", &venue_count);
    getchar();

    if (venue_count < 1 || venue_count > team_count) {
        printf("Invalid count. Using %d venues (one per team).\n", team_count);
        venue_count = team_count;
    }

    venues = malloc(sizeof(Venue) * venue_count);

    printf("\n--- ENTERING VENUES ---\n");
    for (int i = 0; i < venue_count; i++) {
        char name[MAX_NAME_LENGTH];
        int capacity;

        printf("\nVenue %d:\n", i + 1);
        printf("  Name: ");
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = 0;

        printf("  Capacity: ");
        scanf("%d", &capacity);
        getchar();

        // Hybrid location selection - user picks from list or enters manual
        float* loc = get_location_hybrid("Venue location");

        // Auto-detect city name from coordinates
        char city[MAX_NAME_LENGTH] = "Custom";
        for (int j = 0; j < NUM_CITIES; j++) {
            if (loc[0] == indian_cities[j].latitude &&
                loc[1] == indian_cities[j].longitude) {
                strcpy(city, indian_cities[j].name);
                break;
            }
        }

        Venue* venue = create_venue(i, name, city, capacity, loc[0], loc[1]);
        if (venue) {
            venues[i] = *venue;
            free(venue);
        }
    }

    printf("\nSUCCESS: %d venues entered!\n", venue_count);
    printf("Note: Distance matrix will be auto-calculated using Haversine formula.\n\n");

    // Auto-calculate distance matrix using Haversine
    distance_matrix = malloc(sizeof(double*) * team_count);
    for (int i = 0; i < team_count; i++) {
        distance_matrix[i] = malloc(sizeof(double) * team_count);
        for (int j = 0; j < team_count; j++) {
            if (i == j) {
                distance_matrix[i][j] = 0.0;
            } else {
                int venue_i = i % venue_count;
                int venue_j = j % venue_count;
                distance_matrix[i][j] = calculate_distance(
                    venues[venue_i].latitude, venues[venue_i].longitude,
                    venues[venue_j].latitude, venues[venue_j].longitude
                );
            }
        }
    }

    printf("SUCCESS: Distance matrix calculated!\n");
}

void display_teams() {
    if (!teams || team_count == 0) {
        printf("\nNo teams loaded. Load data first.\n");
        return;
    }

    printf("\n=== Teams (%d) ===\n", team_count);
    for (int i = 0; i < team_count; i++) {
        print_team(&teams[i]);
    }
}

void display_venues() {
    if (!venues || venue_count == 0) {
        printf("\nNo venues loaded. Load data first.\n");
        return;
    }

    printf("\n=== Venues (%d) ===\n", venue_count);
    for (int i = 0; i < venue_count; i++) {
        print_venue(&venues[i]);
    }
}

void display_distance_matrix() {
    if (!distance_matrix || team_count == 0) {
        printf("\nNo distance matrix loaded.\n");
        return;
    }

    printf("\n=== Distance Matrix (km) ===\n");
    printf("     ");
    for (int i = 0; i < team_count; i++) {
        printf("%6d ", i);
    }
    printf("\n");

    for (int i = 0; i < team_count; i++) {
        printf("%3d: ", i);
        for (int j = 0; j < team_count; j++) {
            printf("%6.1f ", distance_matrix[i][j]);
        }
        printf("\n");
    }
}

void configure_tournament() {
    printf("\n=== Configure Tournament Dates ===\n");

    printf("\nTournament Start Date:\n");
    printf("  Day (1-31): ");
    scanf("%d", &tournament.start_date.day);
    printf("  Month (1-12): ");
    scanf("%d", &tournament.start_date.month);
    printf("  Year (2024-2030): ");
    scanf("%d", &tournament.start_date.year);
    getchar();

    if (!is_valid_date(&tournament.start_date)) {
        printf("ERROR: Invalid start date!\n");
        return;
    }

    printf("\nTournament End Date:\n");
    printf("  Day (1-31): ");
    scanf("%d", &tournament.end_date.day);
    printf("  Month (1-12): ");
    scanf("%d", &tournament.end_date.month);
    printf("  Year (2024-2030): ");
    scanf("%d", &tournament.end_date.year);
    getchar();

    if (!is_valid_date(&tournament.end_date)) {
        printf("ERROR: Invalid end date!\n");
        return;
    }

    int duration = days_between(&tournament.start_date, &tournament.end_date);
    if (duration <= 0) {
        printf("ERROR: End date must be after start date!\n");
        return;
    }

    printf("\nScheduling Constraints:\n");
    printf("  Minimum rest days between matches for same team (1-7): ");
    scanf("%d", &tournament.min_rest_days);
    printf("  Maximum matches per team per week (1-3): ");
    scanf("%d", &tournament.matches_per_week);
    getchar();

    printf("\nSUCCESS: Tournament configured successfully!\n");
    printf("  Duration: %d days (", duration);
    print_date(&tournament.start_date);
    printf(" to ");
    print_date(&tournament.end_date);
    printf(")\n");
    printf("  Rest days: %d | Max matches/week: %d\n",
           tournament.min_rest_days, tournament.matches_per_week);
}

void display_tournament_config() {
    if (tournament.start_date.year == 0) {
        printf("\nNo tournament configuration set. Use option 3 to configure.\n");
        return;
    }

    printf("\n=== Tournament Configuration ===\n");
    printf("Start Date: ");
    print_date(&tournament.start_date);
    printf("\nEnd Date: ");
    print_date(&tournament.end_date);

    int duration = days_between(&tournament.start_date, &tournament.end_date);
    printf("\nDuration: %d days\n", duration);
    printf("Min rest days: %d\n", tournament.min_rest_days);
    printf("Max matches/week: %d\n", tournament.matches_per_week);

    // Calculate if tournament duration is sufficient
    if (team_count > 0) {
        int total_rounds = (team_count - 1) * 2; // Double round-robin
        int min_days_needed = total_rounds * (tournament.min_rest_days + 1);

        printf("\n--- Feasibility Check ---\n");
        printf("Teams: %d | Total rounds: %d\n", team_count, total_rounds);
        printf("Minimum days needed: %d\n", min_days_needed);

        if (duration >= min_days_needed) {
            printf("SUCCESS: Tournament duration is SUFFICIENT\n");
        } else {
            printf("WARNING: Tournament duration might be TOO SHORT!\n");
            printf("  Consider: longer duration or fewer rest days\n");
        }
    }
}

void test_algorithms() {
    if (!teams || team_count == 0) {
        printf("\nERROR: No teams loaded. Load data first.\n");
        return;
    }
    if (!venues || venue_count == 0) {
        printf("\nERROR: No venues loaded. Load data first.\n");
        return;
    }
    if (tournament.start_date.year == 0) {
        printf("\nERROR: Tournament not configured. Use option 3 first.\n");
        return;
    }

    printf("\n");
    printf("========================================================\n");
    printf("WEEK 1 ALGORITHM TESTS\n");
    printf("========================================================\n");

    // Test 1: Merge Sort
    printf("\n[TEST 1] MERGE SORT - Sorting Teams by Rating\n");
    printf("----------\n");
    Team* teams_copy = (Team*)malloc(sizeof(Team) * team_count);
    for (int i = 0; i < team_count; i++) {
        teams_copy[i] = teams[i];
    }

    printf("BEFORE:\n");
    print_sorted_teams(teams_copy, team_count);

    merge_sort_teams(teams_copy, 0, team_count - 1);

    printf("\nAFTER (sorted by rating - best to worst):\n");
    print_sorted_teams(teams_copy, team_count);
    free(teams_copy);

    // Test 2: Round-Robin Schedule Generation
    printf("\n[TEST 2] ROUND-ROBIN - Fixture Generation\n");
    printf("----------\n");
    FixtureSchedule* schedule = round_robin_schedule(teams, team_count, venues, venue_count, &tournament);
    if (schedule) {
        print_fixture_schedule(schedule);

        // Test 3: Binary Search on Fixtures
        if (schedule->count > 0) {
            printf("\n[TEST 3] BINARY SEARCH - Finding Fixtures\n");
            printf("----------\n");
            demo_binary_search(schedule->fixtures, schedule->count);
        }

        free_fixture_schedule(schedule);
    } else {
        printf("ERROR: Failed to generate fixtures.\n");
    }

    printf("\n========================================================\n");
    printf("ALGORITHM TESTS COMPLETED\n");
    printf("========================================================\n");
}

void show_menu() {
    printf("\n");
    printf("================================================\n");
    printf("   SPORTS LEAGUE FIXTURE SCHEDULER\n");
    printf("   Sport-Agnostic Scheduling System\n");
    printf("================================================\n");
    printf("\n");
    printf("DATA INPUT:\n");
    printf("  1. Load from CSV files\n");
    printf("  2. Manual entry (custom teams/venues)\n");
    printf("  3. Configure tournament dates\n");
    printf("\n");
    printf("VIEW DATA:\n");
    printf("  4. Display teams\n");
    printf("  5. Display venues\n");
    printf("  6. Display distance matrix\n");
    printf("  7. Display tournament config\n");
    printf("\n");
    printf("ALGORITHMS (WEEK 1 COMPLETE):\n");
    printf("  8. Test All Algorithms (Merge Sort, Round-Robin, Binary Search)\n");
    printf("\n");
    printf("COMING SOON:\n");
    printf("  9. Optimize travel distances (Dijkstra)\n");
    printf("  10. Find shortest routes\n");
    printf("\n");
    printf("  0. Exit\n");
    printf("\n");
    printf("Choice: ");
}

int main() {
    int choice;

    printf("\n");
    printf("================================================\n");
    printf("  Welcome to Sports League Scheduler (v1.0)\n");
    printf("  DAA Project - PBL 2026\n");
    printf("================================================\n");

    while (1) {
        show_menu();

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Enter a number.\n");
            while (getchar() != '\n'); // Clear buffer
            continue;
        }
        getchar(); // Clear newline

        switch (choice) {
            case 1:
                load_from_csv();
                break;
            case 2:
                manual_entry();
                break;
            case 3:
                configure_tournament();
                break;
            case 4:
                display_teams();
                break;
            case 5:
                display_venues();
                break;
            case 6:
                display_distance_matrix();
                break;
            case 7:
                display_tournament_config();
                break;
            case 8:
                test_algorithms();
                break;
            case 9:
                printf("\nWARNING: Feature not yet implemented. Coming in Week 2!\n");
                break;
            case 10:
                printf("\nWARNING: Feature not yet implemented. Coming in Week 2!\n");
                break;
            case 0:
                printf("\nCleaning up...\n");
                free_all_data();
                printf("Goodbye!\n\n");
                return 0;
            default:
                printf("\nERROR: Invalid choice. Try again.\n");
        }
    }

    return 0;
}
