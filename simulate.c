#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "simulate.h"

//TODO Find out why results do not match online simulator
// https://github.com/jstar88/opbe

// LJ, SJ, Xer, SS, Sxer, BB, Zerri, RIP, RAPIDFIRE, PF, KT, GT, Kolo, Rec, Spio
const int HULL[] = {400, 1000, 2700, 6000, 7000, 7500, 11000, 900000, 14000, 2300, 400, 1200, 3000, 1600, 100};
const int SHIELD[] = {10, 25, 50, 200, 400, 500, 500, 50000, 700, 100, 10, 25, 100, 10, 0};
const int DMG[] = {50, 150, 400, 1000, 700, 1000, 2000, 200000, 2800, 200, 5, 5, 50, 1, 0};
const int NUM_SHIP_TYPES = 15;
const int RAPIDFIRE[NUM_SHIP_TYPES][NUM_SHIP_TYPES];
RAPIDFIRE[0] =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}; // LJ
RAPIDFIRE[1] =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 5}; // SJ
RAPIDFIRE[2] =  {6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}; // Xer
RAPIDFIRE[3] =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 5}; // SS
RAPIDFIRE[4] =  {0, 4, 4, 7, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 5}; // Sxer
RAPIDFIRE[5] =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}; // BB
RAPIDFIRE[6] =  {0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}; // Zerri
RAPIDFIRE[7] =  {200, 100, 33, 30, 15, 25, 5, 0, 10, 30, 250, 250, 250, 250, 1250}; // RIP
RAPIDFIRE[8] =  {0, 0, 0, 7, 0, 4, 3, 0, 0, 0, 0, 0, 0, 0, 5}; // RP
RAPIDFIRE[9] =  {3, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}; // PF
RAPIDFIRE[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}; // KT
RAPIDFIRE[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}; // GT
RAPIDFIRE[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}; // Kolo
RAPIDFIRE[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}; // Rec
RAPIDFIRE[14] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Spio

int main() {
    int size1 = 30000;
    int fleet1[size1*3];
    for (int i=0; i<size1*3; i+=3) {
        fleet1[i] = 2;
        fleet1[i+1] = HULL[fleet1[i]];
        fleet1[i+2] = SHIELD[fleet1[i]];
    }
    int size2 = 13000;
    int fleet2[size2*3];
    for (int i=0; i<size2*3; i+=3) {
        fleet2[i] = 3;
        fleet2[i+1] = HULL[fleet2[i]];
        fleet2[i+2] = SHIELD[fleet2[i]];
    }
    int winner = simulate_battle(fleet1, size1, fleet2, size2);
    switch (winner) {
        case 0: printf("Draw"); break;
        case 1: printf("Attacker wins"); break;
        case 2: printf("Defender wins"); break;
        default: printf("Error, unknown return signal %d", winner); break;
    } 
    return 1;
}

int simulate_battle(int *attacker, int att_size, int *defender, int def_size) {
    srand(time(NULL));
    struct Fleet * att_fleet = construct_fleet(attacker, att_size);
    struct Fleet * def_fleet = construct_fleet(defender, def_size);
    for (int round=0;round<6;round++) {
        struct Fleet * def_copy = (struct Ship*)malloc(def_size * sizeof(struct Ship));
        memcpy(&def_copy, &def_fleet, sizeof(def_fleet));
        simulate_round(att_fleet, def_fleet);
        simulate_round(def_copy, att_fleet);
        printf("Round: %d, Attacker: %d, Defender: %d\n", round+1, att_size, def_size);
        if (att_fleet->size != 0 && def_fleet->size == 0) {
            return 1;
        } else if (att_fleet->size == 0 && def_fleet->size != 0) {
            return 2;
        } else if (att_fleet->size == 0 && def_fleet->size == 0) {
            return 0;
        }
    }
    return 0;
}

struct Fleet * construct_fleet(int * fleet_spec, int fleet_size) {
    // Initialize subfleets
    struct Subfleet * subfleets = (struct Subfleet*)malloc(NUM_SHIP_TYPES*sizeof(struct Subfleet));
    for (int i=0; i<NUM_SHIP_TYPES;i++) { 
        struct Subfleet * subfleet = subfleets+i;
        subfleet->exists = false;
    }
    // Initiliaze ships in fleet
    struct Ship * ships = (struct Ship*)malloc(fleet_size * sizeof(struct Ship));
    for (int i=0; i<fleet_size;i++) {
        struct Ship * ship = ships+i;
        int ship_type = *(fleet_spec+3*i);
        ship->type = ship_type;
        ship->hull = *(fleet_spec+3*i+1);
        ship->shield = *(fleet_spec+3*i+2);
        struct Subfleet * subfleet = subfleets+ship_type;
        if (!subfleet->exists) {
            subfleet->exists = true;
            subfleet->ship = ship;
            subfleet->size = 1;
        } else {
            subfleet->size += 1;
        }
    }
    // Pack together into single struct
    struct Fleet * fleet = (struct Fleet*)malloc(sizeof(struct Fleet));
    fleet->fleet = ships;
    fleet->subfleets = subfleets;
    fleet->size = fleet_size;
    return fleet;
}

void fire(struct Ship * ship, int target_idx, struct Fleet * def_fleet) {
    struct Ship * target = def_fleet+target_idx;
    int shield = target->shield - DMG[ship->type];
    if (target->shield == 0 || (float)DMG[ship->type] / target->shield > 0.01) {
        if (shield < 0) {
            target->hull += shield;
            shield = 0;
        }
        target->shield = shield;
    }
    float hull_rem = (float)target->hull / HULL[target->type];
    if (hull_rem < 0.7 && (float)rand()/(float)RAND_MAX <= 1-hull_rem) {
        def_fleet->size--;
        if (def_fleet->size == 0) {
            return;
        }
        // Move last element of subfleet to destroyed ship
        // Decrement subfleet->size --
        // How to fill the hole? Move all last elements from subw
        // Linked list? Downside, have to iterate through list 
        // Potentially destroy subfleet (exists=false)
        *(def_fleet+target_idx) = *(def_fleet+def_fleet->size);
    }
}

void simulate_round(struct Fleet * att_fleet, struct Fleet * def_fleet) {
    for (int att_idx=0;att_idx<att_fleet->size;att_idx++) {
        struct Ship * att_ship = att_fleet+att_idx;
        int target_idx= rand_lim(def_fleet->size);
        fire(att_ship, target_idx, def_fleet);
        // Rapidfire
        for (int i=0; i<NUM_SHIP_TYPES; i++) {
            int rapidfire = RAPIDFIRE[att_ship->type][i];
            struct Subfleet * fleet_of_type = def_fleet->subfleets+i;
            if (rapidfire > 0 && fleet_of_type->exists) {
                // Need a map of how many of each ship types are left
                while ((rapidfire-1)/rapidfire <= (float)rand()/(float)RAND_MAX) {
                    int rapidfire_target_idx = rand_lim(fleet_of_type->size);
                    struct Ship * rapidfire_target = fleet_of_type->ship+rapidfire_target_idx;
                    fire(att_ship, rapidfire_target, def_fleet);
                }
            }
        }
    }   
    // Recharge shields
    for (int def_idx=0; def_idx<def_fleet->size; def_idx++) {
        struct Ship * ship = def_fleet+def_idx;
        ship->shield=SHIELD[ship->type];
    }
}

int rand_lim(int limit) {
    int divisor = RAND_MAX/(limit+1);
    int retval;
    do { 
        retval = rand() / divisor;
    } while (retval > limit);
    return retval;
}