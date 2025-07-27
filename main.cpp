#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include <memory>
#include <random>
#include <algorithm>
#include "Technologies.h"
#include "TechGroups.h"

using namespace std;

struct Unit : UnitPips {
    char type;   //Infantry (x), Cavalry (/), Artillery (*), Empty (0)
    string variant; //Bluecoat Infantry vs Redcoat infantry
    int strength = 1000;
    float morale;   //In battle morale
    int flankingRange;
    float drill = 0;
    string special = "No"; //What special unit if any
};

class Country {
public:
    bool defender;  //Attacker is always shown first in battle
    string tag = "Aul-Dwarov";

    int technologyLvl = 1;
    Technology technology = technologies[technologyLvl];

    string techGroup = "Dwarven";

    float moraleModifier = 0.2;
    float discipline = 0.1;

    float infCombatAbility = 0.1;
    float cavCombatAbility = 0;
    float artCombatAbility = 0.2;

    float fireDamageReceived = 0;
    float fireDamageDone = 0;
    float shockDamageReceived = 0.1;
    float shockDamageDone = 0;
    float moraleDamageReceived = 0;
    float moraleDamage = 0;

    bool isHorde = false;
    float moraleDamageTakenByReserves = 0;
    int diceRollBonus = 0;
    float artDamageFromBackrow = 0.2;
    float cavalryFlankingAbility = 0.0;

};

struct General {
    int fire;
    int shock;
    int maneuver;
    int siege;
    string trait = "No";
};

struct Army {   // Rather stack than army
    int id;                          // Many stacks can be used, (but up to 1 can battle at a time) so they can be easily swaped for comparison
    string name;                     // Eventual name

    Country* owner;                  // Points to the country that owns this army
    General* general;
    int infAmout = 0;
    int cavAmout = 0;
    int artAmout = 0;
    string infVariant;              // Longbow, Redcoat Infantry
    string cavVariant;              // Eastern Caracole, Dragoons
    string artVariant;              // Leather Cannon, Flying Battery
    float averageFrontlineDrill;    // Drill will be applied per stack, not regiment or country
    float averageArtilleryDrill;    // Because of meta, you can apply different drill to artillery regiments
};

class Battle : public Country {
public:

    Army* A;    //Type describing battle side A - not attacker
    Army* B;    //Type describing battle side B

    //void FileReading(bool affiliation, Army *A, Army *B);   //Affiliation != attacker/defender

    float StatsApplication(Country& c);

    static vector<Unit> GenerateReserves(Army& a);

    static vector<Unit> GenerateRegiments(char type, int amount, float morale, int flankingRange, string variant, float drill);

    static int VectorScan(vector<Unit> reserves, char type);

    static vector<Unit> Deployment(vector<Unit> reserves, int amount, char type, int maxCombatWidth, int enemyCombatWidth);

    static vector<Unit> DeploymentLogic(vector<Unit> reserves, int infAmount, int cavAmount, int artAmount, int maxCombatWidth, int enemyCombatWidth, int yourSize, int enemySize);

    vector<Unit> BattleFormation(Army& a, Army& b, bool affiliation, vector<Unit> reserves);

    static int diceRolls();

    static int DamageCalc(int diceRoll, int diceRollBonus, bool phase, int terrain, int crossing, Unit& A, Unit& D,
    int Gfire, int Gshock, int EGfire, int EGshock, int Gmaneuver, int EGmaneuver,
    int ofPip, int dfPip, int osPip, int dsPip,
    float fireDamage, float shockDamage, float milTac, float combatAbility, float discipline,
    float duration, float AfireDamageDone, float DfireDamageReceived,
    float AshockDamageDone, float DshockDamageReceived, float AmoraleDamageDone, float DmoraleDamageReceived, float maxMorale);

    //void Surrender(bool affiliation, Army *a, Army *d);

    void DamageApplication(bool affiliation, Country *a, Country *d);

    Battle() {

        A = new Army();
        B = new Army();
        A->general = new General();
        B->general = new General();
        A->owner = new Country();
        B->owner = new Country();

        //FileReading(true, a, d);  //Load
        //FileReading(false, a, d);

        //Add generals

        int duration = 0;
        int combatWidth = max(A->owner->technology.combatWidth, B->owner->technology.combatWidth);

        for (int i=0; i < 120; i++) {

            for (int f=0; f<3; f++) {    //FIRE phase
                duration++;
                //BattleFormation(30, false, false, 10, 0, 0);
                //BattleFormation(30, false, true, 20, 0, 0);
                //DamageCalc(diceRolls(), 0, 0, 0, 1000, A.units[1]);
                //DamageCalc(1, A, d);
                //DamageApplication(true, a, d);
                //DamageApplication(false, a, d);
            }
            for (int s=0; s<3; s++) {    //SHOCK phase
                duration++;
                //cout<<"SHOCK phase\n";
                //DamageCalc(0, a, d);
                //DamageCalc(0, a, d);
                //DamageApplication(true, a, d);
                //DamageApplication(false, a, d);
            }
        }
    }
};

float Battle::StatsApplication(Country& c) {

    float morale;
    float discipline;
    float flankingRange;

}

vector<Unit> Battle::GenerateRegiments(const char type, const int amount, const float morale, const int flankingRange, string variant, const float drill) {

    vector<Unit> regiments;
    for (int i = 0; i < amount; i++) {  //Units are generated here and put into vector
        Unit u;
        u.type = type;
        u.strength = 1000;
        u.morale = morale;
        u.flankingRange = flankingRange;
        u.variant = variant;
        u.drill = drill;
        regiments.push_back(u);
    }
    return regiments;

}

vector<Unit> Battle::GenerateReserves(Army& a) {

    const float morale = a.owner->technology.maxMorale * (1 + a.owner->moraleModifier);
    int flankingRange;
    vector<Unit> reserves;  // Battle reserves "line"

    if (a.infAmout > 0) {   // Add generated regiments into reserves
        flankingRange = floor(1 * (1 + a.owner->technology.flankingRange));
        auto inf = GenerateRegiments('x', a.infAmout, morale, flankingRange, a.infVariant, a.averageFrontlineDrill);
        reserves.insert(reserves.end(), inf.begin(), inf.end());    // Add generated regiments into reserves
    }
    if (a.cavAmout > 0) {   // Cavalry
        flankingRange = floor(2 * (1 + a.owner->technology.flankingRange + a.owner->cavalryFlankingAbility));
        auto cav = GenerateRegiments('/', a.cavAmout, morale, flankingRange, a.cavVariant, a.averageFrontlineDrill);
        reserves.insert(reserves.end(), cav.begin(), cav.end());    // Add generated regiments into reserves
    }
    if (a.artAmout > 0) {   // Artillery
        flankingRange = floor(2 * (1 + a.owner->technology.flankingRange));
        auto art = GenerateRegiments('*', a.artAmout, morale, flankingRange, a.artVariant, a.averageArtilleryDrill);
        reserves.insert(reserves.end(), art.begin(), art.end());    // Add generated regiments into reserves
    }

    return reserves;
}

int Battle::VectorScan(vector<Unit> reserves, char type) {

    int index = 0;

    auto it = find_if(reserves.begin(), reserves.end(),
    [type](const Unit& u) { return u.type == type; });

    if (it != reserves.end()) {
       index = distance(reserves.begin(), it);
    } else {
        index = -1;
    }

    return index;
}

vector<Unit> Battle::Deployment(vector<Unit> reserves, const int amount, const char type, int maxCombatWidth, int enemyCombatWidth) {

    const int center = (maxCombatWidth - 1) / 2;   // Left-biased center
    int leftFlank = center;
    int rightFlank = center + 1;
    int placeHere = leftFlank;
    vector<Unit> line;

    for (int i = 0; i < amount; i++) {

        const int index = VectorScan(reserves, type);  // Find infantry regiment in reserves
        line[placeHere] = reserves[index]; // Add that regiment to the frontline
        reserves.erase(reserves.begin() + index);   // Remove that regiment from reserves
        if (placeHere == leftFlank) { leftFlank++; placeHere = rightFlank; }    // If it was added to the left side, offset flank to free slot and change to right side for the later regiment
        else { rightFlank++; placeHere = leftFlank; }   // Opposite
    }

    return line;
}

vector<Unit> Battle::DeploymentLogic(vector<Unit> reserves, int infAmount, int cavAmount, int artAmount, const int maxCombatWidth, const int enemyCombatWidth, const int yourSize, const int enemySize) {

    vector<Unit> frontline(maxCombatWidth);
    vector<Unit> backline(maxCombatWidth);

    if (yourSize <= enemySize) {    // Case for armies that do not need to place cavalry closer to the center due to enemy smaller size - so case that army is smaller, but equal also fits

        if (infAmount < maxCombatWidth) {   // Case: There is not enough infantry to fill first line

            if (infAmount > 0) {   // Deploy all infantry into frontline because it will fit

                auto infRegiments = Deployment(reserves, infAmount, 'x', maxCombatWidth, enemyCombatWidth); // Call function to pass infantry from reserves to frontline
                frontline.insert(frontline.end(), infRegiments.begin(), infRegiments.end());    // Join function results with frontline
                infAmount = 0;    // Acknowledge that all requested (all) infantry was added
            }
            while (VectorScan(frontline, '0') == -1 && cavAmount > 0) { // Deploy as much cavalry to the frontline flanks as possible, but only if it will fit into combat width

                auto cavRegiments = Deployment(reserves, 1, '/', maxCombatWidth, enemyCombatWidth); // Call function to pass cavalry from reserves to frontline
                frontline.insert(frontline.end(), cavRegiments.begin(), cavRegiments.end());    // Join function results with frontline
                cavAmount--;    // Acknowledge that 1 cavalry was added
            }
            while (yourSize - artAmount < artAmount && VectorScan(frontline, '0') != -1) {    // Put artillery into frontline until it can fit behind inf and/or cav or frontline is full

                auto artRegiments = Deployment(reserves, 1, '*', maxCombatWidth, enemyCombatWidth); // Call function to pass artillery from reserves to frontline
                frontline.insert(frontline.end(), artRegiments.begin(), artRegiments.end());    // Join function results with frontline
                artAmount--;    // Acknowledge that 1 artillery was added
            }
            while (VectorScan(backline, '0') != -1 && artAmount > 0) {  // Deploy rest of the artillery into the backline until backline is full

                auto artRegiments = Deployment(reserves, 1, '*', maxCombatWidth, enemyCombatWidth); // Call function to pass artillery from reserves to backline
                backline.insert(backline.end(), artRegiments.begin(), artRegiments.end());  // Join function results with backline
                artAmount--;    // Acknowledge that 1 artillery was added
            }
        }
    }

    return frontline, backline;
}

vector<Unit> Battle::BattleFormation(Army& a, Army& b, bool affiliation, vector<Unit> reserves) {

    // Battle combat width
    const int combatWidth = max(a.owner->technology.combatWidth, b.owner->technology.combatWidth);

    // Remove regiments with 0 strength and/or 0 morale from reserves
    reserves.erase(
    remove_if(reserves.begin(), reserves.end(),
        [](const Unit& u) {
            return u.strength <= 0 || u.morale <= 0.0;  // Condition to remove
        }),
    reserves.end()
    );

    // Decide what army is bigger if any
    const int aSize = a.infAmout + a.cavAmout + a.artAmout;
    const int bSize = b.infAmout + b.cavAmout + b.artAmout;

    BattleDeployment(reserves, a.infAmout, combatWidth, b.owner->technology.combatWidth, aSize, bSize);
    BattleDeployment(reserves, b.infAmout, combatWidth, a.owner->technology.combatWidth, bSize, aSize);

}

int Battle::DamageCalc (const int diceRoll, const int diceRollBonus, const bool phase, const int terrain, const int crossing, Unit& A, Unit& D,
    const int Gfire, const int Gshock, const int EGfire, const int EGshock, const int Gmaneuver, const int EGmaneuver,
    const int ofPip, const int dfPip, const int osPip, const int dsPip,
    const float fireDamage, const float shockDamage, const float milTac, const float combatAbility, const float discipline,
    const float duration, const float AfireDamageDone, const float DfireDamageReceived,
    const float AshockDamageDone, const float DshockDamageReceived, const float AmoraleDamageDone, const float DmoraleDamageReceived, float maxMorale) {

    int strengthCasualties = 0;
    float moraleCasualties = 0;

    if (A.morale > 0 && A.strength > 0) {

        int generalAdvantage = 0;
        int unitAdvantage = 0;  //Comparison of regiment FIRE/SHOCK pips
        const int moraleAdvantage = A.offensiveMoralePip - D.defensiveMoralePip;    //Comparison of regiment MORALE pips
        float techDamageMultiplier = 0;
        float armyDamageDoneMultiplier = 0;
        float armyDamageReceivedMultiplier = 0;

        if (phase) {    //Fire is first, so 0 = FIRE phase
            generalAdvantage = max(Gfire - EGfire, 0);
            unitAdvantage = ofPip - dfPip;
            techDamageMultiplier = fireDamage;
            armyDamageDoneMultiplier = AfireDamageDone;
            armyDamageReceivedMultiplier = DfireDamageReceived;
        }
        else {    //Shock is second, so 1 = SHOCK phase
            generalAdvantage = max(Gshock - EGshock, 0);
            unitAdvantage = osPip - dsPip;
            techDamageMultiplier = shockDamage;
            armyDamageDoneMultiplier = AshockDamageDone;
            armyDamageReceivedMultiplier = DshockDamageReceived;
        }

        const int baseStrengthCasualties = max((15 + 5 * (diceRoll + diceRollBonus + generalAdvantage + unitAdvantage - terrain - crossing)), 15);

        const float multipliers = ((A.strength / 1000) * (techDamageMultiplier / milTac) * (1 + combatAbility) * (1 + discipline) * (1 + duration / 100));

        strengthCasualties = floor(baseStrengthCasualties * multipliers * (1 + armyDamageDoneMultiplier) * (1 + armyDamageReceivedMultiplier));

        const int baseMoraleCasualties = max((15 + 5 * (diceRoll + diceRollBonus + generalAdvantage + moraleAdvantage + unitAdvantage - terrain - crossing)), 15);

        //moraleCasualties = baseMoraleCasualties * multipliers * (1 + AmoraleDamageDone) * (1 + DmoraleDamageReceived) * (maxMorale / 540) + ((Dmorale * d->infAmout / d->infAmout) / 100); // Work in progress - needs averange enemy morale across lines

        cout<<"\nDamage done:\n"<<strengthCasualties<<endl<<moraleCasualties<<endl;

    }

    return strengthCasualties, moraleCasualties;

}

int Battle::diceRolls () {

    const int diceFrom  = 0;
    const int diceTo    = 9;
    random_device                  rand_dev;
    mt19937                        generator(rand_dev());
    uniform_int_distribution<int>  distr(diceFrom, diceTo);

    return distr(generator);

}

/*void Battle::Surrender(bool affiliation, Army *a, Army *d) {

    if (affiliation) {  //  Atakujacy wycofal sie z bitwy

        //cout<<"\nAtakujaca armia kraju "<<a->tag<<" wycofala sie z bitwy.\nBitwe wygrala wiec armia kraju "<<d->tag<<".\n";

    } else {
        //cout<<"\nBroniaca sie armia kraju "<<d->tag<<" wycofala sie z bitwy.\nBitwe wygrala wiec armia kraju "<<a->tag<<".\n";
    }   // Obronca wycofal sie z bitwy

};*/

void Battle::DamageApplication(bool affiliation, Country *a, Country *d) {  // After all calculations are done, units take damage from the enemy they have faced

    if (affiliation) {  // Attacker damage applied to defender

        //if (morale > moraleDamage) { morale = morale - moraleCasualties; }
        //else { d->infantry.morale = 0.00; Surrender(false, aa, dd); }

        //if (d->infantry.strength > a->strengthCasualties) { d->infantry.strength = d->infantry.strength - a->strengthCasualties; }
        //else { d->infantry.strength = 0; }  // Unit loses all remaining manpower and (to be done) retreats
        //cout<<endl<<"Defender stats"<<endl<<d->infantry.strength<<endl<<d->infantry.morale<<endl;
    }

    else {  // Defender damage applied to attacker

        //if (a->infantry.morale > d->moraleDamage) { a->infantry.morale = a->infantry.morale - d->moraleCasualties; }
        //else { a->infantry.morale = 0.00; Surrender(true, aa, dd); }

        //if (a->infantry.strength > d->strengthCasualties) { a->infantry.strength = a->infantry.strength - d->strengthCasualties; }
        //else { a->infantry.strength = 0; }  // Unit loses all remaining manpower and (to be done) retreats
        //cout<<endl<<"Attacker stats"<<endl<<a->infantry.strength<<endl<<a->infantry.morale<<endl;
    }

};

/*void Battle::FileReading(bool affiliation, Army *A, Army *B) {  //  Odczyt z plikow: affiliation - jesli 1 to umieszczamy dane Atakujacego, jesli 0 to Obroncy

        string fileInput="";    //Varriable containing raw text from file
        int row=0;              //Row of data from file, first = 0
        int whereMark=0;
        fstream Plik;
        int wiersze = 38;   //Amount of rows to read
        string fileArray[37];

        if (affiliation)    {
            Plik.open("Army_A.txt");
        }   else    {
            Plik.open("Army_D.txt");
        }

        while (row<wiersze) {
            getline (Plik, fileInput);
            whereMark = fileInput.find("=");
            fileArray[row] = fileInput.substr(whereMark+1, 40); row++;
        }
        Plik.close();

        /*row=0;
        while (row<wiersze) {    //  Wpisanie danych z tabeli do odpowiednich zmiennych wraz z konwertowaniem jezeli potrzeba
        switch (row) {

        case 0: a->tag = fileArray[row]; break;  //  Country
        case 1: a->technologyLvl = stoi(fileArray[row]); break;  //  Technology Level
        case 2: a->techGroup = fileArray[row]; break;    //  Technology Group
        case 3: a->infType = fileArray[row]; break;  //  Infantry unit
        case 4: a->cavType = fileArray[row]; break;  //  Cavalry unit
        case 5: a->artType = fileArray[row]; break;  //  Artillery unit
        case 6: a->maxMorale = stof(fileArray[row]); break;  //  Land Morale
        case 7: a->discipline = stof(fileArray[row]); break;  //  Discipline
        case 8: a->infCombatAbility = stof(fileArray[row]); break;   //  Infantry Combat Ability
        case 9: a->cavCombatAbility = stof(fileArray[row]); break;  //  Cavalry Combat Ability
        case 10: a->artCombatAbility = stof(fileArray[row]); break; //  Artillery  Combat Ability
        case 11: a->fireDamageDone = stof(fileArray[row]); break; //  Land Fire Damage
        case 12: a->fireDamageReceived = stof(fileArray[row]); break; //  Fire Damage Received
        case 13: a->shockDamageDone = stof(fileArray[row]); break;//  Shock Damage
        case 14: a->shockDamageReceived = stof(fileArray[row]); break; //  Shock Damage Received
        case 15: a->moraleDamage = stof(fileArray[row]); break; //  Morale Damage
        case 16: a->moraleDamageReceived = stof(fileArray[row]); break; //  Morale Damage Received
        case 17: a->isHorde = stoi(fileArray[row]); break; //  Is Horde?
        //case 18: a->moraleDamageTakenbyReserves = stof(fileArray[row]); break; //  Morale Damage Taken by Reserves
        case 19: a->diceRollBonus = stoi(fileArray[row]); break; //  Dice Roll Bonus
        //case 20: a->artDamageBackrow = stof(fileArray[row]); break; //  Artillery Damage From Backrow
        //case 21: a->allowedSpecialUnits = fileArray[row]; break; //  Allowed Special Units
        //case 22: a->infantry.offensiveFirePip = stoi(fileArray[row]); break;
        //case 23: a->infantry.defensiveFirePip = stoi(fileArray[row]); break;
        //case 24: a->infantry.offensiveShockPip = stoi(fileArray[row]); break;
        //case 25: a->infantry.defensiveShockPip = stoi(fileArray[row]); break;
        //case 26: a->infantry.offensiveMoralePip = stoi(fileArray[row]); break;
        //case 27: a->infantry.defensiveShockPip = stoi(fileArray[row]); break;
        //case 28: a->firePip = stoi(fileArray[row]); break;
        //case 29: a->shockPip = stoi(fileArray[row]); break;
        //case 30: a->manauveurPip = stoi(fileArray[row]); break;
        //case 31: a->siegePip = stoi(fileArray[row]); break;
        //case 32: a->infFireDamage = stof(fileArray[row]); break;
        //case 33: a->infShockDamage = stof(fileArray[row]); break;
        //case 34: a->milTactics = stof(fileArray[row]); break;
        //case 35: a->terrain = stoi(fileArray[row]); break;
        //case 36: a->crossing = stoi(fileArray[row]); break;
        case 37: break;

            // Linie z odczytem danych z plikow.
            // Ilosc linii do odczytania jest w zmiennej int wiersze
        default: break;
        } row++;
    } row=0;
}*/

int main(int argc, char **argv) {

    Battle battle;

    return 0;
};