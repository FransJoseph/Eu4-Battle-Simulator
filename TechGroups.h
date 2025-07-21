#pragma once
#include <string>
#include <vector>
#include <map>
using namespace std;

struct UnitPips {
    string name;    //Redcoat Infantry
    int techLvl;
    int type;   //Infantry (1), Cavalry (2), Artillery (3)

    int offensiveFirePip;
    int defensiveFirePip;
    int offensiveShockPip;
    int defensiveShockPip;
    int offensiveMoralePip;
    int defensiveMoralePip;
};

inline map<string, vector<UnitPips>> unitDatabase = {
    {
        "West Dwarven", {
            {"Shieldwalls", 0, 1, 1,1, 2,1, 1,1},
            {"Redcoat Infantry", 1, 1, 3,2, 3,2, 2,2},
            {"Flying Battery", 1, 1, 6,3, 0,1, 4,3}
        }
    },
    {
        "Eastern Orcish", {
            {"Eastern Spearmen", 0, 1, 0,1, 3,2, 1,1},
            {"Cossack Cavalry", 0, 2, 2,2, 5,3, 3,2}
        }
    }
};