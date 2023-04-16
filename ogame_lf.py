import argparse
import pandas as pd
import matplotlib.pyplot as plt


LIFEFORM = {
    1: "Human",
    2: "Rock´tal",
    3: "Mecha",
    4: "Kaelesh"
}


TECHS = {
    1: 3,
    2: 1,
    3: 2,
    4: 2,
    5: 2,
    6: 3,
    7: 2,
    8: 1,
    9: 2,
    10: 2,
    11: 2,
    12: 4,
    13: 2,
    14: 4,
    15: 2,
    16: 2,
    17: 1,
    18: 2
}


EXCHANGE = [2.4, 1.4, 1]


def min_notna(x, y):
    if pd.notna(x) and pd.notna(y):
        return min(x, y)
    elif pd.notna(x):
        return x
    else:
        return y


def calc_dse(ressources, exchange=EXCHANGE):
    return sum([base / factor for base, factor in zip(ressources, exchange)])


def calc_cost(entry, offset=1):
    return entry[0] * (entry[1] ** (entry[2] -1 + offset)) * (entry[2] + offset)


def calc_bonus(entry, offset=0):
    bonus_idx = 12
    bonus_list = [0, 0, 0]
    for idx, has_bonus in enumerate(entry[24:27]):
        if has_bonus:
            bonus_list[idx] = min_notna(entry[bonus_idx+2], calc_cost([entry[bonus_idx], entry[bonus_idx+1], entry[23]], offset=offset) / 100)
            if pd.notna(entry[bonus_idx+2]):
                bonus_list[idx] = min(has_bonus[idx], entry[bonus_idx+2])
            if pd.notna(entry[bonus_idx+3]):
                bonus_idx += 3
    return calc_dse(bonus_list)


def calc_tech_bonus(entry, offset=0):
    return min_notna(entry[14], calc_cost([entry[12], entry[13], entry[23]], offset=offset) / 100)


def step(data, debug=False):
    data["current_dse_bonus"] = data.apply(calc_bonus, axis=1)
    if data["tech"].any():
        tech_bonus = sum(data[data["tech"]].apply(calc_tech_bonus, axis=1))
        data.loc[(~data["tech"]) & (data["Type"] != "Building"), "dse_bonus"] = data["dse_bonus_base"] * (1 + tech_bonus)
        data.loc[data["tech"], "current_dse_bonus"] = sum(data[data["Type"] != "Building"]["current_dse_bonus"] * tech_bonus) / max(1, sum(data["tech"]))
    data["new_dse_bonus"] = data["dse_bonus"] + sum(data["current_dse_bonus"])
    if data["tech"].any():
        data.loc[data["tech"], "new_dse_bonus"] = sum(data["current_dse_bonus"]) + sum(data[(~data["tech"]) & (data["Type"] != "Building")]["current_dse_bonus"]) * data[data["tech"]]["dse_bonus"]
    data["new_dse_cost"] = data[["dse_base_cost", "metal increase factor", "level"]].apply(calc_cost, axis=1)
    data["new_bonus_cost_ratio"] = data["new_dse_bonus"] / data["new_dse_cost"]
    index = data.sort_values("new_bonus_cost_ratio", ascending=False).index[0]
    if debug:
        print(f"Upgrading {data.loc[index, 'Name EN']} to level {data.loc[index, 'level'] + 1}, new bonus: {data.loc[index, 'new_dse_bonus']}")
    return index, data.loc[index, ["new_dse_cost", "new_dse_bonus"]] 


def build_plot(lifeform, max_dse, debug=False):
    data = pd.read_excel("lf_data.xlsx")
    data.loc[data["Name EN"] == "High-Performance Transformer", ["bonus 1 base value", "bonus 1 increase factor", "bonus 1 max"]] = data.loc[data["Name EN"] == "High-Performance Transformer", ["bonus 2 base value", "bonus 2 increase factor", "bonus 2 max"]]
    data = data[(data["Lifeform"] == lifeform) | (data["Type"] != "Building")]
    data = data[data[["Type", "Lifeform"]].apply(lambda x: x[0] == "Building" or LIFEFORM[TECHS[int(x[0].split(" ")[-1])]] == x[1], axis=1)]
    data["level"] = 0
    for ressource in ["metal", "crystal", "deuterium"]:
        data[ressource] = data["Description EN"].apply(lambda x: ressource in x)
    data["tech"] = False
    data.loc[data["Name EN"].isin(["Metropolis", "High-Performance Transformer", "Chip Mass Production"]), "tech"] = True
    data = data[data[["metal", "crystal", "deuterium", "tech"]].any(axis=1)]
    data["dse_base_cost"] = data[["metal base cost", "crystal base cost", "deut base cost"]].apply(calc_dse, axis=1)
    data["dse_bonus"] = data.apply(lambda x: calc_bonus(x, offset=1), axis=1)
    data["dse_bonus_base"] = data["dse_bonus"]
    data.loc[data["tech"], "dse_bonus"] = data[data["tech"]][["bonus 1 base value", "bonus 1 increase factor", "level"]].apply(calc_cost, axis=1) / 100
    cummulative_dse_cost = [0]
    total_dse_bonus = [0]
    while cummulative_dse_cost[-1] <= max_dse:
        index, stats = step(data, debug)
        data.loc[index, "level"] += 1
        assert stats[0] >= 0, f"iteration {i}: \n {data.loc[index]}"
        cummulative_dse_cost.append(cummulative_dse_cost[-1] + stats[0])
        total_dse_bonus.append(stats[1])
        assert total_dse_bonus[-2] <= total_dse_bonus[-1], data.loc[index]
    plot = pd.DataFrame()
    plot["cummulative_dse_cost"] = cummulative_dse_cost
    plot["total_dse_bonus"] = total_dse_bonus
    print(lifeform)
    print(data[["Name EN", "level"]])
    return plot


if __name__ == '__main__':
    max_dse = 15e9
    human = build_plot("Human", max_dse)
    rock = build_plot("Rock´tal", max_dse)
    plt.plot(human["cummulative_dse_cost"], human["total_dse_bonus"])
    plt.plot(rock["cummulative_dse_cost"], rock["total_dse_bonus"])
    plt.xlabel("Investierte Deuterium Standard Einheiten (DSE)")
    plt.ylabel("Bonus auf DSE Produktion")
    plt.legend(["Menschen", "Rocks"],loc="lower right")
    plt.show()