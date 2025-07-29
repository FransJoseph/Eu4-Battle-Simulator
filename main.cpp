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
    char type = '0';   //Infantry (x), Cavalry (/), Artillery (*), Empty (0)
    string variant; //Bluecoat Infantry vs Redcoat infantry
    int strength = 1000;
    float morale;   //In battle morale
    int flankingRange;
    float drill = 0;
    string special = "No"; //What special unit if any
};

class Country {
public:
    string tag = "Aul-Dwarov";

    int technologyLvl = 1;
    Technology technology = technologies[technologyLvl];

    string techGroup = "West Dwarven";

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
    int infAmount = 4;
    int cavAmount = 4;
    int artAmount = 4;
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

    bool attackerIsA = true;

    vector<Unit> A_frontline;
    vector<Unit> A_backline;
    vector<Unit> A_reserves;

    vector<Unit> B_frontline;
    vector<Unit> B_backline;
    vector<Unit> B_reserves;

    //void FileReading(bool affiliation, Army *A, Army *B);   //Affiliation != attacker/defender

    static vector<Unit> GenerateRegiments(char type, int amount, float morale, int flankingRange, string variant, float drill);

    static vector<Unit> GenerateReserves(Army& a);

    static int VectorScan(vector<Unit> reserves, char type);

    static vector<Unit> Deployment(vector<Unit> reserves, bool attacker, int amount, char type, int maxCombatWidth, int enemyCombatWidth);

    static pair <vector<Unit>, vector<Unit>> DeploymentLogic(vector<Unit> reserves, bool attacker, int infAmount, int cavAmount, int artAmount, int enemySize, int maxCombatWidth, int enemyCombatWidth);

    static vector<Unit> BattleFormation(Army& a, Army& b, bool affiliation, vector<Unit> reserves);

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

        const int combatWidth = max(A->owner->technology.combatWidth, B->owner->technology.combatWidth);

        A_reserves = GenerateReserves(*A);
        B_reserves = GenerateReserves(*B);

        const int sizeA = A->infAmount + A->cavAmount + A->artAmount;
        const int sizeB = B->infAmount + B->cavAmount + B->artAmount;

        if (attackerIsA) {
            auto [aFront, aBack] = DeploymentLogic(A_reserves, true, A->infAmount, A->cavAmount, A->artAmount, sizeB, combatWidth, B->owner->technology.combatWidth);
            A_frontline = aFront;
            A_backline = aBack;
            auto [bFront, bBack] = DeploymentLogic(B_reserves, false, B->infAmount, B->cavAmount, B->artAmount, sizeA, combatWidth, A->owner->technology.combatWidth);
            B_frontline = bFront;
            B_backline = bBack;
        } else {
            auto [aFront, aBack] = DeploymentLogic(A_reserves, false, A->infAmount, A->cavAmount, A->artAmount, sizeB, combatWidth, B->owner->technology.combatWidth);
            A_frontline = aFront;
            A_backline = aBack;
            auto [bFront, bBack] = DeploymentLogic(B_reserves, true, B->infAmount, B->cavAmount, B->artAmount, sizeA, combatWidth, A->owner->technology.combatWidth);
            B_frontline = bFront;
            B_backline = bBack;
        }

        int duration = 0;
        for (int i=0; i < 120; i++) {

            for (int f=0; f<3; f++) {    //FIRE phase
                duration++;
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

        for (int i = combatWidth - 1; i >= 0; i--) {
            Unit& u = A_backline[i];
            cout<<u.type;
        } cout<<"\n";
        for (int i = combatWidth - 1; i >= 0; i--) {
            Unit& u = A_frontline[i];
            cout<<u.type;
        } cout<<"\n";
        cout<<"\n";
        for (int i = 0; i < combatWidth; i++) {
            Unit& u = B_frontline[i];
            cout<<u.type;
        } cout<<"\n";
        for (int i = 0; i < combatWidth; i++) {
            Unit& u = B_backline[i];
            cout<<u.type;
        } cout<<"\n";
    }
};

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

    if (a.infAmount > 0) {   // Add generated regiments into reserves
        flankingRange = floor(1 * (1 + a.owner->technology.flankingRange));
        auto inf = GenerateRegiments('x', a.infAmount, morale, flankingRange, a.infVariant, a.averageFrontlineDrill);
        reserves.insert(reserves.end(), inf.begin(), inf.end());    // Add generated regiments into reserves
    }
    if (a.cavAmount > 0) {   // Cavalry
        flankingRange = floor(2 * (1 + a.owner->technology.flankingRange + a.owner->cavalryFlankingAbility));
        auto cav = GenerateRegiments('/', a.cavAmount, morale, flankingRange, a.cavVariant, a.averageFrontlineDrill);
        reserves.insert(reserves.end(), cav.begin(), cav.end());    // Add generated regiments into reserves
    }
    if (a.artAmount > 0) {   // Artillery
        flankingRange = floor(2 * (1 + a.owner->technology.flankingRange));
        auto art = GenerateRegiments('*', a.artAmount, morale, flankingRange, a.artVariant, a.averageArtilleryDrill);
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

vector<Unit> Battle::Deployment(vector<Unit> reserves, const bool attacker, const int amount, const char type, const int maxCombatWidth, const int enemyCombatWidth) {

    const int center = (maxCombatWidth - 1) / 2;   // Left-biased center for attacker and right biased center for the defender
    int leftFlank;
    int rightFlank;
    if (attacker) {
        leftFlank = center;
        rightFlank = center + 1;
    } else {
        rightFlank = center;
        leftFlank = center - 1;
    }

    int placeHere = center;
    Unit emptyUnit;
    emptyUnit.type = '0';
    vector<Unit> line(maxCombatWidth, Unit());

    for (int i = 0; i < amount; i++) {

        const int index = VectorScan(reserves, type);  // Find adequate regiment in reserves
        cout<<"Place "<<type<<" to "<<placeHere<<endl;
        line[placeHere] = reserves[index]; // Add that regiment to the frontline
        reserves.erase(reserves.begin() + index);   // Remove that regiment from reserves
        if (placeHere == leftFlank) { leftFlank--; placeHere = rightFlank; }    // If it was added to the left side, offset flank to free slot and change to right side for the later regiment
        else { rightFlank++; placeHere = leftFlank; }   // Opposite
    }

    return line;
}

pair<vector<Unit>, vector<Unit>> Battle::DeploymentLogic(vector<Unit> reserves, const bool attacker, int infAmount, int cavAmount, int artAmount, const int enemySize, const int maxCombatWidth, const int enemyCombatWidth) {

    Unit emptyUnit;
    emptyUnit.type = '0';
    vector<Unit> frontline(maxCombatWidth, emptyUnit);
    vector<Unit> backline(maxCombatWidth, emptyUnit);

    const int yourSize = infAmount + cavAmount + artAmount;

    if (yourSize <= enemySize) {

        if (infAmount < maxCombatWidth) {

            if (infAmount > 0) {
                auto infRegiments = Deployment(reserves, attacker, infAmount, 'x', maxCombatWidth, enemyCombatWidth);
                // Copy infRegiments into frontline starting at index 0
                for (size_t i = 0; i < infRegiments.size() && i < frontline.size(); ++i) {
                    frontline[i] = infRegiments[i];
                }
                infAmount = 0;
            }

            while (VectorScan(frontline, '0') != -1 && cavAmount > 0) {
                auto cavRegiments = Deployment(reserves, attacker, 1, '/', maxCombatWidth, enemyCombatWidth);
                // Find first empty spot in frontline to place cavalry
                int index = VectorScan(frontline, '0');
                if (index != -1) {
                    frontline[index] = cavRegiments[0];  // only one cavalry regiment expected
                    cavAmount--;
                } else {
                    break;  // no more space
                }
            }

            while (yourSize - artAmount < artAmount && VectorScan(frontline, '0') != -1 && artAmount > 0) {
                auto artRegiments = Deployment(reserves, attacker, 1, '*', maxCombatWidth, enemyCombatWidth);
                int index = VectorScan(frontline, '0');
                if (index != -1) {
                    frontline[index] = artRegiments[0];
                    artAmount--;
                } else {
                    break;
                }
            }

            while (VectorScan(backline, '0') != -1 && artAmount > 0) {
                auto artRegiments = Deployment(reserves, attacker, 1, '*', maxCombatWidth, enemyCombatWidth);
                int index = VectorScan(backline, '0');
                if (index != -1) {
                    backline[index] = artRegiments[0];
                    artAmount--;
                } else {
                    break;
                }
            }

        } else {
            // Your existing logic for infantry >= maxCombatWidth should be here, untouched
        }
    }

    return {frontline, backline};
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

    //BattleDeployment(reserves, a.infAmout, combatWidth, b.owner->technology.combatWidth, aSize, bSize);
    //BattleDeployment(reserves, b.infAmout, combatWidth, a.owner->technology.combatWidth, bSize, aSize);

    return reserves;

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
};