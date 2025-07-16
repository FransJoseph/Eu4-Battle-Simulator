#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include <memory>
#include <random>

using namespace std;

struct UnitPips {   //Longbow, Redcoat Infantry, Flying Battery, Tortoise Formation, Caracole
    string techGroup;
    int techLvl;
    int type;   //Infantry, Cavalry, Artillery
    int offensiveFirePip;
    int defensiveFirePip;
    int offensiveShockPip;
    int defensiveShockPip;
    int offensiveMoralePip;
    int defensiveMoralePip;
};

struct Unit : UnitPips {
    int type;   //Infantry, Cavalry, Artillery
    string variant; //Bluecoat Infantry vs Redcoat infantry
    int strength = 1000;
    float morale;   //In battle morale
    float drill = 0;
    string special = "No"; //What special unit if any
};

struct General {
    string name;
    int fire;
    int shock;
    int maneuver;
    int siege;
    string trait = "No";
};

class Army : public Unit, public General {
public:

    string tag;
    int technologyLvl;

    string techGroup;
    string infType;
    string cavType;
    string artType;

    float maxMorale;
    float discipline;
    float milTactics;

    float infCombatAbility;
    float cavCombatAbility;
    float artCombatAbility;

    float fireDamageReceived;
    float fireDamageDone;
    float shockDamageReceived;
    float shockDamageDone;
    float moraleDamageReceived;
    float moraleDamage;

    bool isHorde;
    float moraleDamageTakenbyReserves;
    int diceRollBonus;
    float artDamageBackrow;
    string allowedSpecialUnits;

    int infAmout;
    int cavAmout;
    int artAmout;
    int powtorzeniaFazBiwy;
};

struct Technology {
    int number;
    string name;
    string AnbennarName;

    int maxMoraleBase;
    int combatWidth;
    float baseMilTactics;
    float flankingRangeBaseBonus;

    float infFireDamage;
    float infShockDamage;
    float cavFireDamage;
    float cavShockDamage;
    float artFireDamage;
    float artShockDamage;

    // Calculation variables
    int generalAdvantage;
    int baseCasualties;
    float multipliers;
    int strengthCasualties;
    float moraleCasualties;
    int i;

    // Shared variables
    int diceRoll;   // Except for this one
    int terrain;
    int crossing;
    float duration;
    bool phase=true;   // As 1 statement is true and therefore first in execution, it mean "FIRE" phase and 0 means "SHOCK" phase
};

class Battle : public Army {
public:
    Army A;   //Variable describing attacker
    Army D;   //Variable describing defender

    Army* aa;
    Army* dd;

    void FileReading (bool affiliation, Army *a);

    void StatsApplication (Army *a, Army *b);

    static int DamageCalc (int diceRoll, bool phase, int terrain, int crossing, float strength,
        float Amorale, float Dmorale, int Gfire, int Gshock, int EGfire, int EGshock, int Gmaneuver, int EGmaneuver,
        int diceRollBonus, int ofPip, int dfPip, int osPip, int dsPip, float fireDamage, float shockDamage, float milTac,
        float combatAbility, float discipline, float duration, float AfireDamageDone, float DfireDamageReceived,
        float AshockDamageDone, float DshockDamageReceived, float AmoraleDamageDone, float DmoraleDamageReceived);

    void Surrender(bool affiliation, Army *a, Army *d);
    void DamageApplication(bool affiliation, Army *a, Army *d);

    Battle() {

        aa = &A;
        dd = &D;

        FileReading(true, aa);  //Load
        FileReading(false, dd);

        // Wyznaczenie bazowych wartosci dla zmiennych na podstawie innych zmiennych
        aa->milTactics = aa->baseMilTactics * (1 + aa->discipline);   // Regimenty zwiekszaja swoj poziom taktyki (umiejetonosci unikania obrazen) tylu krotnie, ile wynosi ich dyscyplina
        dd->milTactics = dd->baseMilTactics * (1 + dd->discipline);
        aa->infantry.morale = aa->maxMorale;
        dd->infantry.morale = dd->maxMorale;

        //Surrender(1, aa, dd);

        // Algorytm tworzenia liczb calkowitych losowych z przedzialu 0-9, potrzebny do generacji rzutu koscia
        const int diceFrom  = 0;
        const int diceTo    = 9;
        random_device                  rand_dev;
        mt19937                        generator(rand_dev());
        uniform_int_distribution<int>  distr(diceFrom, diceTo);

        int duration = 0;

        aa->diceRoll = distr(generator);
        dd->diceRoll = distr(generator);
        //cout<<endl<<aa->diceRoll<<endl;
        //cout<<endl<<dd->diceRoll<<endl;

        for (i=0; i<aa->powtorzeniaFazBiwy; i++) {

            for (i=0; i<3; i++) {    // 3 dni ostrzalow "FIRE"

            DamageCalcA(1, aa, dd);
            DamageCalcD(1, aa, dd);
            DamageApplication(1, aa, dd);
            DamageApplication(0, aa, dd);
            aa->duration++;
            dd->duration++;

            }

            aa->diceRoll = distr(generator);
            dd->diceRoll = distr(generator);
            //cout<<endl<<aa->diceRoll<<endl;
            //cout<<endl<<dd->diceRoll<<endl;
            for (i=0; i<3; i++) {    // 3 dni walki w recz "SHOCK"

            DamageCalcA(0, aa, dd);
            DamageCalcD(0, aa, dd);
            DamageApplication(1, aa, dd);
            DamageApplication(0, aa, dd);
            aa->duration++;
            dd->duration++;

            }

        }

    }
};

void Battle::StatsApplication (Army *a, Army *b) {

}

int Battle::DamageCalc (const int diceRoll, const bool phase, const int terrain, const int crossing, const float strength,
    const float Amorale, const float Dmorale, const int Gfire, const int Gshock, const int EGfire, const int EGshock, const int Gmaneuver, const int EGmaneuver,
    const int diceRollBonus, const int ofPip, const int dfPip, const int osPip, const int dsPip, const float fireDamage, const float shockDamage, const float milTac,
    const float combatAbility, const float discipline, const float duration, const float AfireDamageDone, const float DfireDamageReceived,
    const float AshockDamageDone, const float DshockDamageReceived, const float AmoraleDamageDone, const float DmoraleDamageReceived) {

    int strengthCasualties = 0;
    float moraleCasualties = 0;

    if (Amorale > 0 && strength > 0) {

        int generalAdvantage = 0;
        int unitAdvantage = 0;
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

        const int baseCasualties = max((15 + 5 * (diceRoll + diceRollBonus + generalAdvantage + unitAdvantage - terrain - crossing)),15);

        const float multipliers = ((strength / 1000) * (techDamageMultiplier / milTac) * (1 + combatAbility) * (1 + discipline) * (1 + duration / 100));

        strengthCasualties = floor(baseCasualties * multipliers * (1 + armyDamageDoneMultiplier) * (1 + armyDamageReceivedMultiplier));

        moraleCasualties = baseCasualties * multipliers * (1 + AmoraleDamageDone) * (1 + DmoraleDamageReceived) * (Amorale / 540) + ((Dmorale * d->infAmout / d->infAmout) / 100); // Work in progress - needs averange enemy morale across frontline

        cout<<"\nDamage done:\n"<<strengthCasualties<<endl<<moraleCasualties<< endl;

    }
    return strengthCasualties, moraleCasualties;

}

void Battle::Surrender(bool affiliation, Army *a, Army *d) {

    if (affiliation) {  //  Atakujacy wycofal sie z bitwy

        cout<<"\nAtakujaca armia kraju "<<a->tag<<" wycofala sie z bitwy.\nBitwe wygrala wiec armia kraju "<<d->tag<<".\n";

    } else { cout<<"\nBroniaca sie armia kraju "<<d->tag<<" wycofala sie z bitwy.\nBitwe wygrala wiec armia kraju "<<a->tag<<".\n"; }   // Obronca wycofal sie z bitwy

};

void Battle::DamageApplication(bool affiliation, Army *a, Army *d) {  // After all calculations are done, units take damage from the enemy they have faced

    if (affiliation) {  // Attacker damage applied to defender

        if (d->infantry.morale > a->moraleDamage) { d->infantry.morale = d->infantry.morale - a->moraleCasualties; }
        else { d->infantry.morale = 0.00; Surrender(false, aa, dd); }

        if (d->infantry.strength > a->strengthCasualties) { d->infantry.strength = d->infantry.strength - a->strengthCasualties; }
        else { d->infantry.strength = 0; }  // Unit loses all remaining manpower and (to be done) retreats
        cout<<endl<<"Defender stats"<<endl<<d->infantry.strength<<endl<<d->infantry.morale<<endl;
    }

    else {  // Defender damage applied to attacker

        if (a->infantry.morale > d->moraleDamage) { a->infantry.morale = a->infantry.morale - d->moraleCasualties; }
        else { a->infantry.morale = 0.00; Surrender(true, aa, dd); }

        if (a->infantry.strength > d->strengthCasualties) { a->infantry.strength = a->infantry.strength - d->strengthCasualties; }
        else { a->infantry.strength = 0; }  // Unit loses all remaining manpower and (to be done) retreats
        cout<<endl<<"Attacker stats"<<endl<<a->infantry.strength<<endl<<a->infantry.morale<<endl;
    }

};

void Battle::FileReading (bool affiliation, Army *a) {  //  Odczyt z plikow: affiliation - jesli 1 to umieszczamy dane Atakujacego, jesli 0 to Obroncy

        string fileInput="";    //  Zmienna przechowujaca surowy tekst z pliku
        int row=0;              // Rzad danych z pliku, pierwszy rzad ma numer 0
        int whereMark=0;
        fstream Plik;
        int wiersze = 38; // Ilosc wierszy do odczytania z pliku
        string fileArray[wiersze];

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

        row=0;
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
        case 18: a->moraleDamageTakenbyReserves = stof(fileArray[row]); break; //  Morale Damage Taken by Reserves
        case 19: a->diceRollBonus = stoi(fileArray[row]); break; //  Dice Roll Bonus
        case 20: a->artDamageBackrow = stof(fileArray[row]); break; //  Artillery Damage From Backrow
        case 21: a->allowedSpecialUnits = fileArray[row]; break; //  Allowed Special Units
        case 22: a->infantry.offensiveFirePip = stoi(fileArray[row]); break;
        case 23: a->infantry.defensiveFirePip = stoi(fileArray[row]); break;
        case 24: a->infantry.offensiveShockPip = stoi(fileArray[row]); break;
        case 25: a->infantry.defensiveShockPip = stoi(fileArray[row]); break;
        case 26: a->infantry.offensiveMoralePip = stoi(fileArray[row]); break;
        case 27: a->infantry.defensiveShockPip = stoi(fileArray[row]); break;
        case 28: a->firePip = stoi(fileArray[row]); break;
        case 29: a->shockPip = stoi(fileArray[row]); break;
        case 30: a->manauveurPip = stoi(fileArray[row]); break;
        case 31: a->siegePip = stoi(fileArray[row]); break;
        case 32: a->infFireDamage = stof(fileArray[row]); break;
        case 33: a->infShockDamage = stof(fileArray[row]); break;
        case 34: a->baseMilTactics = stof(fileArray[row]); break;
        case 35: a->terrain = stoi(fileArray[row]); break;
        case 36: a->crossing = stoi(fileArray[row]); break;
        case 37: a->powtorzeniaFazBiwy = stoi(fileArray[row]); break;

            // Linie z odczytem danych z plikow.
            // Ilosc linii do odczytania jest w zmiennej int wiersze
        default: break;
        } row++;
    } row=0;
};

int main(int argc, char **argv) {

    Battle battle;

    return 0;
};
