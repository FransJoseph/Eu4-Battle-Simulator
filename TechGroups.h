#pragma once
#include <string>
#include <vector>
#include <map>
using namespace std;

struct UnitPips {
    string name;    //Redcoat Infantry
    int techLvl;
    char type;   //Infantry (x), Cavalry (/), Artillery (*)

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
            {"Shieldwalls", 1, 'x', 0,0, 0,1, 0,2},
            {"Ram Riders", 6, '/', 0,0, 2,1, 1,1},
            {"Flying Battery", 29, '*', 4,4, 0,2, 4,4}
        }
    },
    {
        "Eastern Orcish", {
            {"Eastern Spearmen", 0, 'x', 0,1, 3,2, 1,1},
            {"Cossack Cavalry", 0, '/', 2,2, 5,3, 3,2}
        }
    }
};