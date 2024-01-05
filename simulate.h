// Single ship within a fleet
typedef struct Ship {
    unsigned short int type;
    int hull;
    int shield;
    struct Ship * next;
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

int simulate_battle(int *attacker, int att_size, int *defender, int def_size);
struct Fleet * construct_fleet(int * fleet_spec, int fleet_size);
void fire(struct Ship * ship, int target_idx, struct Fleet * def_fleet) ;
void simulate_round(struct Fleet * att_fleet, struct Fleet * def_fleet);
int rand_lim(int limit);