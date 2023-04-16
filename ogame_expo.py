from recordtype import recordtype
import random

SHIPS = ["LJ", "SJ", "Xer", "SS", "Sxer", "BB", "Zerri", "RIP", "PF", "RP", "KT", "GT", "Kolo", "Rec", "Spio"]

HULL = {
    "Spio": 100,
    "KT": 400,
    "LJ": 400,
    "SJ": 1000,
    "GT": 1200,
    "Rec": 1600,
    "Xer": 2700,
    "Kolo": 3000,
    "SS": 6000,
    "Sxer": 7000,
    "BB": 7500,
    "Zerri": 11000,
    "RIP": 900000,
    "PF": 23000,
    "RP": 140000
}

SHIELD =  {
    "Spio": 0.01,
    "Rec": 10,
    "KT": 10,
    "LJ": 10,
    "GT": 25,
    "SJ": 25,
    "Xer": 50,
    "Kolo": 100,
    "SS": 200,
    "Sxer": 400,
    "BB": 500,
    "Zerri": 500,
    "RIP": 50000,
    "PF": 100,
    "RP": 700
}

DMG = {
    "Spio": 0.01,
    "Rec": 1,
    "KT": 5,
    "GT": 5,
    "LJ": 50,
    "Kolo": 50,
    "SJ": 150,
    "Xer": 400,
    "Sxer": 700,
    "SS": 1000,
    "BB": 1000,
    "Zerri": 2000,
    "RIP": 200000,
    "PF": 200,
    "RP": 2800
}

CAPA = {
    "Spio": 0,
    "KT": 5000,
    "GT": 25000,
    "Rec": 20000,
    "Kolo": 7500,
    "LJ": 50,
    "SJ": 100,
    "Xer": 800,
    "SS": 1500,
    "Sxer": 750,
    "BB": 500,
    "Zerri": 1000,
    "RIP": 1000000,
    "PF": 10000,
    "RP": 10000
}

FUEL = {
    "Spio": 1,
    "KT": 20,
    "GT": 50,
    "Rec": 300,
    "Kolo": 1000,
    "LJ": 20,
    "SJ": 75,
    "Xer": 300,
    "SS": 500,
    "Sxer": 250,
    "BB": 1000,
    "Zerri": 1000,
    "RIP": 1,
    "PF": 300,
    "RP": 1100
}

COST = {
    "LJ": [3000,1000,0],
    "SJ": [6000,4000,0],
    "Xer": [20000,7000,2000],
    "SS": [45000,15000,0],
    "Sxer": [30000,40000,15000],
    "BB": [50000,25000,15000],
    "Zerri": [60000,50000,15000],
    "RIP": [5000000,4000000,1000000],
    "KT": [2000,2000,0],
    "GT": [6000,6000,0],
    "Kolo": [10000,20000,10000],
    "Rec": [10000,6000,2000],
    "Spio": [0,1000,0],
    "PF": [8000,15000,8000],
    "RP": [85000,55000,20000]
}

RF = {
    "KT": {"Spio": 5},
    "GT": {"Spio": 5},
    "Rec": {"Spio": 5},
    "Kolo": {"Spio": 5},
    "LJ": {"Spio": 5},
    "SJ": {"Spio": 5, "KT": 3},
    "Xer": {"Spio": 5, "LJ": 6},
    "SS": {"Spio": 5, "PF": 5},
    "Sxer": {"Spio": 5, "SJ": 4, "Xer": 4, "SS": 7, "KT": 3, "GT": 3},
    "BB": {"Spio": 5},
    "Zerri": {"Spio": 5, "Sxer": 2},
    "RIP": {"Spio": 1250, "LJ": 200, "SJ": 100, "Xer": 33, "SS": 30, "BB": 25, "Zerri": 5, "KT": 250, "GT": 250, "Kolo": 250, "Rec": 250, "Sxer": 15, "PF": 30, "RP": 10},
    "PF": {"Spio": 5, "Xer": 3, "LJ": 3, "SJ": 2},
    "RP": {"Spio": 5, "SS": 7, "BB": 4, "Zerri": 3}
}

TECHS = [0,0,0]

LF_TECHS = {
    "LJ": 0,
    "SJ": 0,
    "Xer": 0,
    "SS": 0,
    "Sxer": 0,
    "BB": 0,
    "Zerri": 0,
    "GT": 0,
    "Rec": 0
}

Ship = recordtype("ship", "type hull shield")

def construct_fleet(fleet_spec: dict()):
    fleet = list()
    for ship, number in fleet_spec.items():
        for _ in range(number):
            fleet.append(Ship(ship, HULL[ship], SHIELD[ship]))
    return fleet

def deconstruct_fleet(fleet: dict()):
    fleet_spec = dict()
    for ship in fleet:
        if ship.type in fleet_spec.keys():
            fleet_spec[ship.type] += 1
        else:
            fleet_spec[ship.type] = 1 
    return fleet_spec

def fire_single(ship, target):
    target.hull = max(target.hull - (DMG[ship.type] - target.shield), 0)
    target.shield = max(target.shield - DMG[ship.type], 0)

def fire(attacker, defender):
    for ship in attacker:
        fire_single(ship, random.choice(defender))
        if ship.type in RF.keys():
            for rp_target, rapidfire in RF[ship.type].items():
                rp_targets = [s for s in defender if rp_target == s.type ]
                if len(rp_targets) > 0:
                    while random.random() < (rapidfire-1)/rapidfire:
                        fire_single(ship, random.choice(rp_targets))
    for ship in defender:
        ship.shield = SHIELD[ship.type]

def remove_destroyed(fleet):
    for ship in fleet:
        if ship.hull == 0:
            fleet.remove(ship)

def sim(fleet_spec1: dict(), fleet_spec2: dict()):
    fleet1 = construct_fleet(fleet_spec1)
    fleet2 = construct_fleet(fleet_spec2)
    for _ in range(6):
        if len(fleet1) == 0 or len(fleet2) == 0:
            break
        fire(fleet1, fleet2)
        fire(fleet2, fleet1)
        remove_destroyed(fleet1)
        remove_destroyed(fleet2)
    return fleet1, fleet2

def main():
    RUNS = 1
    avg_fleet1 = dict()
    avg_fleet2 = dict()
    for round in range(RUNS):
        fleet1, fleet2 = sim({"SS": 1500},{"Xer": 3000})
        fleet1, fleet2 = deconstruct_fleet(fleet1), deconstruct_fleet(fleet2)
        print(f"Round {round+1}: ", fleet1, "vs", fleet2)
        for ship, amount in fleet1.items():
            avg_fleet1[ship] = avg_fleet1[ship] + amount if ship in avg_fleet1.keys() else amount
        for ship, amount in fleet2.items():
            avg_fleet2[ship] = avg_fleet2[ship] + amount if ship in avg_fleet2.keys() else amount
    for ship in avg_fleet1.keys():
        avg_fleet1[ship] = int(round(avg_fleet1[ship] / RUNS, 0))
    for ship in avg_fleet2.keys():
        avg_fleet2[ship] = int(round(avg_fleet2[ship] / RUNS, 0))
    print("Fleet1:", avg_fleet1)
    print("Fleet2:", avg_fleet2)

if __name__ == '__main__':
    main()