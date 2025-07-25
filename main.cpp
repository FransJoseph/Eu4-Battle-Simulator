#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include <memory>
#include <random>
#include "Technologies.h"
#include "TechGroups.h"

using namespace std;

struct Unit : UnitPips {
    char type;   //Infantry (x), Cavalry (/), Artillery (*)
    string variant; //Bluecoat Infantry vs Redcoat infantry
    int strength = 1000;
    float morale;   //In battle morale
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

    string infType = "Scattergun Axualies";
    string cavType = "Ramsteel Cavalry";
    string artType = "Mithrill Gun";

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
    float averageFrontlineDrill;    // Drill will be applied per stack, not regiment
    float averageArtilleryDrill;    // Because of meta, you can apply different drill to artillery regiments
};

class Battle : public Country {
public:

    Army* A;    //Type describing battle side A - not attacker
    Army* B;    //Type describing battle side B

    //void FileReading(bool affiliation, Army *A, Army *B);   //Affiliation != attacker/defender

    //void StatsApplication(Army *a, Army *d);

    vector<Unit> BattleDeployment(int combatWidth, int unitAmount, string type);

    void BattleFormation(int combatWidth, bool initial, bool affiliation, int infantryAmount, int cavalryAmount, int artilleryAmount);    //Initial deployment on day 0 - true/false

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
                BattleFormation(30, false, false, 10, 0, 0);
                BattleFormation(30, false, true, 40, 0, 0);
                //DamageCalc(diceRolls(), 0, 0, 0, 1000, A.units[1], );
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

    Battle::~Battle() {
        delete A->general;
        delete A->owner;
        delete A;

        delete B->general;
        delete B->owner;
        delete B;
    }
};

//void Battle::StatsApplication (Army *a, Army *d) {}

vector<Unit> Battle::BattleDeployment(int combatWidth, int unitAmount, string type) {

    vector<Unit> reserves;
    for (int i = 0; i < unitAmount; i++) {  //Units are generated here and put into reserves
        Unit u;
        u.type = 'x';
        u.strength = 1000;
        u.morale = 3.0;
        u.variant = "Matchbeard Mortars";
        u.drill = 0.0;
        cout<<u.type;
        reserves.push_back(u);
    }
    return reserves;
}


void Battle::BattleFormation(int combatWidth, const bool initial, bool affiliation, int infantryAmount, int cavalryAmount, int artilleryAmount) {

    Unit* frontLine[combatWidth];
    Unit* backLine[combatWidth];
    vector<Unit> reserves;

    if (initial) {    //Initial deployment of units into battle, done only once - no reinforcements from other stacks are simulated

        for (int i = 0; i < infantryAmount; i++) {  //Units are generated here and put into reserves
            Unit u;
            u.type = 'x';
            u.strength = 1000;
            u.morale = 3.0;
            u.variant = "Matchbeard Mortars";
            u.drill = 0.0;
            cout<<u.type;
            reserves.push_back(u);
        }
    }

    float moraleDamageFirstLine = 0;
    float avgEnemyMaxMorale = 0;

    const int center = (combatWidth - 1) / 2;   // Left-biased center
    int leftFlank = center;
    int rightFlank = center + 1;

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