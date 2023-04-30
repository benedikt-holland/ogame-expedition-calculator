#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "simulate.h"

// Min 100
#define PRECISION 100

//TODO Find out why results do not match online simulator

// LJ, SJ, Xer, SS, Sxer, BB, Zerri, RIP, RP, PF, KT, GT, Kolo, Rec, Spio
const int HULL[] = {400, 1000, 2700, 6000, 7000, 7500, 11000, 900000, 14000, 2300, 400, 1200, 3000, 1600, 100};
const int SHIELD[] = {10, 25, 50, 200, 400, 500, 500, 50000, 700, 100, 10, 25, 100, 10, 0};
const int DMG[] = {50, 150, 400, 1000, 700, 1000, 2000, 200000, 2800, 200, 5, 5, 50, 1, 0};

int main() {
    int size1 = 30000;
    int fleet1[size1*3];
    for (int i=0; i<size1*3; i+=3) {
        fleet1[i] = 2;
        fleet1[i+1] = HULL[fleet1[i]];
        fleet1[i+2] = SHIELD[fleet1[i]];
    }
    int size2 = 12000;
    int fleet2[size2*3];
    for (int i=0; i<size2*3; i+=3) {
        fleet2[i] = 3;
        fleet2[i+1] = HULL[fleet2[i]];
        fleet2[i+2] = SHIELD[fleet2[i]];
    }
    int winner = simulate(fleet1, size1, fleet2, size2);
    switch (winner) {
        case 0: printf("Draw"); break;
        case 1: printf("Attacker wins"); break;
        case 2: printf("Defender wins"); break;
        default: printf("Error, unknown return signal %d", winner); break;
    } 
    return 1;
}

int simulate(int *attacker, int att_size, int *defender, int def_size) {
    srand(time(NULL));
    struct Ship * att_fleet = construct_fleet(attacker, att_size);
    struct Ship * def_fleet = construct_fleet(defender, def_size);
    for (int round=0;round<6;round++) {
        struct Ship * def_copy = (struct Ship*)malloc(def_size * sizeof(struct Ship));
        memcpy(&def_copy, &def_fleet, sizeof(def_fleet));
        int def_size_copy = def_size;
        def_size = fire_fleet(att_fleet, att_size, def_fleet, def_size);
        att_size = fire_fleet(def_copy, def_size_copy, att_fleet, att_size);
        printf("Round: %d, Attacker: %d, Defender: %d\n", round+1, att_size, def_size);
        if (att_size != 0 && def_size == 0) {
            return 1;
        } else if (att_size == 0 && def_size != 0) {
            return 2;
        } else if (att_size == 0 && def_size == 0) {
            return 0;
        }
    }
    return 0;
}

struct Ship * construct_fleet(int * fleet_spec, int fleet_size) {
    struct Ship * fleet = (struct Ship*)malloc(fleet_size * sizeof(struct Ship));
    for (int i=0; i<fleet_size;i++) {
        struct Ship * ship = fleet+i;
        ship->type = *(fleet_spec+3*i);
        ship->hull = *(fleet_spec+3*i+1);
        ship->shield = *(fleet_spec+3*i+2);
    }
    return fleet;
}

void fire(struct Ship * ship, struct Ship * target) {
    int shield = target->shield - DMG[ship->type];
    if (target->shield == 0 || (PRECISION * DMG[ship->type]) / target->shield > PRECISION/100) {
        if (shield < 0) {
            target->hull += shield;
            shield = 0;
        }
        target->shield = shield;
        int hull_rem = (PRECISION * target->hull) / HULL[target->type];
        if (hull_rem < (PRECISION/100)*70 && PRECISION-hull_rem > rand() % PRECISION) {
            target->hull = 0;
        }
    }
}

int fire_fleet(struct Ship * att_fleet, int att_size, struct Ship * def_fleet, int def_size) {
    for (int att_idx=0;att_idx<att_size;att_idx++) {
        struct Ship * att_ship = att_fleet+att_idx;
        int target_idx= rand() % def_size;
        struct Ship * target = def_fleet+target_idx;
        fire(att_ship, target);
        if (target->hull <= 0) {
            def_size--;
            if (def_size == 0) {
                return def_size;
            }
            *target = *(def_fleet+def_size);
        }
    }   
    // Recharge shields
    for (int def_idx=0; def_idx<def_size; def_idx++) {
        struct Ship * ship = def_fleet+def_idx;
        ship->shield=SHIELD[ship->type];
    }
    return def_size;
}