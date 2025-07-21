#pragma once
#include <vector>
#include <string>
using namespace std;

struct Technology {
    int lvl;
    string name;
    string AnbennarName;

    int maxMorale;
    int combatWidth;
    float milTactics;
    float flankingRange;
    float supplyLimitModifier;

    float infFireDamage;
    float infShockDamage;
    float cavFireDamage;
    float cavShockDamage;
    float artFireDamage;
    float artShockDamage;
};

// Global list of all technologies
inline vector<Technology> technologies = {
    {0, "Pre-Medieval Millitary", "Pre-Castanorian Millitary",
        2, 15, 0.5, 0, 0,
        0.25, 0.20,
        0.0, 0.8,
        0.0, 0.0},

    {1, "", "",
        2, 15, 0.5, 0, 0,
        0.35, 0.30,
        0.0, 0.8,
        0.0, 0.0}
};