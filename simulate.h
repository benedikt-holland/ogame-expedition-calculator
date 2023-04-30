struct Ship {
    unsigned short int type;
    int hull;
    int shield;
};

int simulate(int *attacker, int att_size, int *defender, int def_size);
struct Ship * construct_fleet(int * fleet_spec, int fleet_size);
void fire(struct Ship * ship, struct Ship * target);
int fire_fleet(struct Ship * att_fleet, int att_size, struct Ship * def_fleet, int def_size);