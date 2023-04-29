#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "simulate.h"

struct Ship {
    int hull;
    int shield;
    int dmg;
};

int main() {
    //int attacker[] = {400, 10, 50, 400, 10, 50, 400, 10, 40, 400, 10, 40};
    //int defender[] = {400, 10, 50, 400, 10, 50};
    int size = 20000;
    int fleet[size*3];
    for (int i=0; i<size; i++) {
        fleet[i] = 400;
        fleet[i+1] = 10;
        fleet[i+2] = 50;
    }
    int winner = simulate(fleet, size, fleet, size);
    printf("%d", winner);
    return 1;
}

int simulate(int *attacker, int att_size, int *defender, int def_size) {
    srand(time(NULL));
    struct Ship att_fleet[att_size];
    int shield_att[att_size];
    for (int i=0; i<att_size;i++) {
        att_fleet[i].hull = *(attacker+3*i);
        att_fleet[i].shield = *(attacker+3*i+1);
        shield_att[i] = att_fleet[i].shield;
        att_fleet[i].dmg = *(attacker+3*i+2);
    }
    struct Ship def_fleet[def_size];
    int shield_def[def_size];
    for (int i=0; i<att_size;i++) {
        def_fleet[i].hull = *(defender+3*i);
        def_fleet[i].shield = *(defender+3*i+1);
        shield_def[i] = def_fleet[i].shield;
        def_fleet[i].dmg = *(defender+3*i+2);
    }
    for (int round=0;round<6;round++) {
        struct Ship def_copy[def_size];
        memcpy(&def_copy, &def_fleet, sizeof(def_fleet));
        int def_size_copy = def_size;
        // Attacker fires
        for (int att_idx=0;att_idx<att_size;att_idx++) {
            int target = rand() % def_size;
            int shield = def_fleet[target].shield - att_fleet[att_idx].dmg;
            if (shield < 0) {
                def_fleet[target].hull += shield;
                shield = 0;
            }
            def_fleet[target].shield = shield;
            // Remove ship when hull < 0
            if (def_fleet[target].hull < 0) {
                def_fleet[target] = def_fleet[def_size-1];
                def_size -= 1;
                if (def_size == 0) {
                    // Attacker wins
                    return 1;
                }
            }
        }
        // Defender fires
        for (int def_idx=0;def_idx<def_size_copy;def_idx++) {
            int target = rand() % att_size;
            int shield = att_fleet[target].shield - def_copy[def_idx].dmg;
            if (shield < 0) {
                att_fleet[target].hull += shield;
                shield = 0;
            }
            att_fleet[target].shield = shield;
            // Remove ship when hull < 0
            if (att_fleet[target].hull < 0) {
                att_fleet[target] = att_fleet[att_size-1];
                att_size -= 1;
                if (att_size == 0) {
                    // Defender wins
                    return 2;
                }
            }
        }
        // Recharge shields
        for (int i=0;i<att_size;i++) {
            att_fleet[i].shield = shield_att[i];
        }
        for (int i=0;i<def_size;i++) {
            def_fleet[i].shield = shield_def[i];
        }
    }
    // Draw
    return 0;
}