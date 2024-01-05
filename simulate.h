#define NUM_SHIP_TYPES 15
#define HULL 0
#define SHIELD 1
#define DMG 2
// LJ, SJ, Xer, SS, Sxer, BB, Zerri, RIP, RAPIDFIRE, PF, KT, GT, Kolo, Rec, Spio
int SHIP_STATS[3][NUM_SHIP_TYPES] = {
    {400, 1000, 2700, 6000, 7000, 7500, 11000, 900000, 14000, 2300, 400, 1200, 3000, 1600, 100}, //Hull
    {10, 25, 50, 200, 400, 500, 500, 50000, 700, 100, 10, 25, 100, 10, 0}, // Shield
    {50, 150, 400, 1000, 700, 1000, 2000, 200000, 2800, 200, 5, 5, 50, 1, 0} // Damage
};
const int RAPIDFIRE[NUM_SHIP_TYPES][NUM_SHIP_TYPES] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}, // LJ
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 5}, // SJ
    {6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}, // Xer
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 5}, // SS
    {0, 4, 4, 7, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 5}, // Sxer
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}, // BB
    {0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}, // Zerri
    {200, 100, 33, 30, 15, 25, 5, 0, 10, 30, 250, 250, 250, 250, 1250}, // RIP
    {0, 0, 0, 7, 0, 4, 3, 0, 0, 0, 0, 0, 0, 0, 5}, // RP
    {3, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}, // PF
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}, // KT
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}, // GT
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}, // Kolo
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}, // Rec
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} // Spio
};

// Single ship within a fleet
typedef struct Ship {
    unsigned short int type;
    int hull;
    int shield;
    struct Ship * next;
    struct Ship * prev;
} Ship;

// Keeps track of each type of ships in the fleet
typedef struct Subfleet {
    struct Ship * ship;
    int size;
    bool exists;
} Subfleet;

typedef struct Fleet {
    struct Ship * fleet;
    int size;
    struct Subfleet * subfleets;
} Fleet;

int simulate_battle(Subfleet * attacker, int attacker_size, Subfleet * defender, int defender_size, int stats[3][NUM_SHIP_TYPES]);
Fleet * construct_fleet(Subfleet * subfleets, int fleet_size, int stats[3][NUM_SHIP_TYPES]);
void fire(Ship * ship, int target_idx, Fleet * def_fleet, int stats[3][NUM_SHIP_TYPES]);
void simulate_round(Fleet * att_fleet, Fleet * def_fleet, int stats[3][NUM_SHIP_TYPES]);
int rand_lim(int limit);