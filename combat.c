#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "combat.h"

int main() {
    int attacker[15] = {0};
    int defender[15] = {0};
    attacker[0] = 30000;
    defender[2] = 2000;
    Battleresult * result = simulate(attacker, defender, SHIP_STATS, 10);
    switch (result->winner)
    {
    case 0:
        printf("Defender wins after %d rounds", result->rounds);
        break;
    case 2:
        printf("Attacker wins after %d rounds", result->rounds);
        break;
    case 1:
        printf("Draw after %d rounds", result->rounds);
        break;
    }
    for (int type=0;type<NUM_SHIP_TYPES;type++) {
        printf("\n%s: %d - %d", SHIP_NAMES[type], (((Subfleet*)result->attacker)+type)->size, (((Subfleet*)result->defender)+type)->size);
    }
    return 1;
}

Battleresult * simulate(int attacker_fleet[], int defender_fleet[], int stats[3][NUM_SHIP_TYPES], int iterations) {
    Battleresult * result = NULL;
    for (int i=0;i<iterations;i++) {
        int attacker_size = 0;
        int defender_size = 0;
        for (int type=0;type<NUM_SHIP_TYPES;type++) {
            attacker_size += attacker_fleet[type];
            defender_size += defender_fleet[type];
        }
        Subfleet * attacker = (Subfleet*)malloc(NUM_SHIP_TYPES * sizeof(Subfleet));
        Subfleet * defender = (Subfleet*)malloc(NUM_SHIP_TYPES * sizeof(Subfleet));
        for (int type=0;type<NUM_SHIP_TYPES;type++) {
            Subfleet * att_subfleet = attacker+type;
            Subfleet * def_subfleet = defender+type;
            att_subfleet->ship = NULL;
            att_subfleet->size = attacker_fleet[type];
            att_subfleet->exists = att_subfleet->size > 0;
            def_subfleet->ship = NULL;
            def_subfleet->size = defender_fleet[type];
            def_subfleet->exists = def_subfleet->size > 0;
        }
        Battleresult * iteration_result = simulate_battle(attacker, attacker_size, defender, defender_size, stats);
        if (result == NULL) {
            result = iteration_result;
        } else {
            result->rounds += iteration_result->rounds;
            result->winner += iteration_result->winner;
            for (int type=0;type<NUM_SHIP_TYPES;type++) {
                (((Subfleet*)result->attacker)+type)->size += (((Subfleet*)iteration_result->attacker)+type)->size;
                (((Subfleet*)result->defender)+type)->size += (((Subfleet*)iteration_result->defender)+type)->size;
            }
        }
    }
    result->rounds /= iterations;
    result->winner /= iterations;
    for (int type=0;type<NUM_SHIP_TYPES;type++) {
        (((Subfleet*)result->attacker)+type)->size /= iterations;
        (((Subfleet*)result->defender)+type)->size /= iterations;
    }
    return result;
}

Battleresult * simulate_battle(Subfleet * attacker, int attacker_size, Subfleet * defender, int defender_size, int stats[3][NUM_SHIP_TYPES]) {
    srand(time(NULL));
    Battleresult * result = (Battleresult*)malloc(sizeof(Battleresult));
    Fleet * att_fleet = construct_fleet(attacker, attacker_size, stats);
    Fleet * def_fleet = construct_fleet(defender, defender_size, stats);
    int round;
    for (round=0;round<6;round++) {
        Fleet * def_copy = construct_fleet(defender, def_fleet->size, stats);
        simulate_round(att_fleet, def_fleet, stats);
        simulate_round(def_copy, att_fleet, stats);
        if (att_fleet->size != 0 && def_fleet->size == 0) {
            result->winner=2;
            break;
        } else if (att_fleet->size == 0 && def_fleet->size != 0) {
            result->winner=0;
            break;
        } else if (att_fleet->size == 0 && def_fleet->size == 0) {
            result->winner=1;
            break;
        }
    }
    result->rounds = round+1;
    result->attacker=attacker;
    result->defender=defender;
    return result;
}

Fleet * construct_fleet(Subfleet * subfleets, int fleet_size, int stats[3][NUM_SHIP_TYPES]) {
    Ship * head = NULL;
    Ship * current_ship = NULL;
    for (int type=0;type<NUM_SHIP_TYPES;type++) {
        Subfleet * subfleet = subfleets+type;
        for (int i=0;i<subfleet->size;i++) {
            Ship * new_ship = (Ship*)malloc(sizeof(Ship));
            if (subfleet->ship == NULL) {
                subfleet->ship = new_ship;
            }
            new_ship->prev = current_ship;
            new_ship->next = NULL;
            if (current_ship != NULL) current_ship->next = new_ship;
            new_ship->type = type;
            new_ship->hull = stats[HULL][type];
            new_ship->shield = stats[SHIELD][type];
            current_ship = new_ship;
            if (head == NULL) head = current_ship;
        }
    }
    // Pack together into single struct
    Fleet * fleet = (Fleet*)malloc(sizeof(Fleet));
    fleet->fleet = head;
    fleet->subfleets = subfleets;
    fleet->size = fleet_size;
    return fleet;
}

void fire(Ship * ship, int target_idx, Fleet * def_fleet, int stats[3][NUM_SHIP_TYPES]) {
    Ship * target = def_fleet->fleet;
    for (int i=0;i<target_idx-1;i++) {
        target = target->next;
    }
    int shield = target->shield - stats[DMG][ship->type];
    if (target->shield == 0 || (float)stats[DMG][ship->type] / target->shield > 0.01) {
        if (shield < 0) {
            target->hull += shield;
            shield = 0;
        }
        target->shield = shield;
    }
    float hull_rem = (float)target->hull / stats[HULL][target->type];
    if (hull_rem < 0.7 && (double)rand() / RAND_MAX < (double)1-hull_rem) {
        // Decrement and potentially delete subfleet
        Subfleet * fleet_of_type = def_fleet->subfleets+target->type;
        fleet_of_type->size--;
        if (fleet_of_type->size <= 0) {
            fleet_of_type->exists = false;
        }
        // Remove destroyed ship from fleet
        if (def_fleet->size > 1) {
            if (target->prev == NULL) {
                target->next->prev = NULL;
                def_fleet->fleet = target->next;
            } else if (target->next == NULL) {
                target->prev->next = NULL;
            } else {
                Ship * prev_ship = target->prev;
                target->prev->next = target->next;
                target->next->prev = prev_ship;
            }
        }
        free(target);
        // Decrement fleet size
        def_fleet->size--;
    }
}

void simulate_round(Fleet * att_fleet, Fleet * def_fleet, int stats[3][NUM_SHIP_TYPES]) {
    Ship * att_ship = att_fleet->fleet;
    while (att_ship != NULL) {
        int target_idx= rand() % def_fleet->size;
        fire(att_ship, target_idx, def_fleet, stats);
        if (def_fleet->size <= 0) return;
        // Rapidfire
        for (int i=0; i<NUM_SHIP_TYPES; i++) {
            int rapidfire = RAPIDFIRE[att_ship->type][i];
            Subfleet * fleet_of_type = def_fleet->subfleets+i;
            if (rapidfire > 0 && fleet_of_type->exists) {
                // Need a map of how many of each ship types are left
                while ((double)rand() / RAND_MAX < (double)(rapidfire-1)/rapidfire) {
                    int rapidfire_target_idx = rand() % fleet_of_type->size;
                    fire(att_ship, rapidfire_target_idx, def_fleet, stats);
                    if (def_fleet->size <= 0) return;
                }
            }
        }
        att_ship = att_ship->next;
    } 
    // Recharge shields
    Ship * def_ship = def_fleet->fleet;
    while (def_ship != NULL) {
        def_ship->shield=stats[SHIELD][def_ship->type];
        def_ship = def_ship->next;
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