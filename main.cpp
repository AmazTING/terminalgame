#include <iostream>
#include "entity.h"
#include "gear.h"
#include <random>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <iomanip>
#include <windows.h>
#include "color.hpp"
#include <vector>
#include <fstream>
#include <mmsystem.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

#pragma comment(lib, "user32")
using namespace std;
using namespace dye;
using namespace std::chrono_literals;
using namespace std::this_thread;
using namespace dye;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

bool is_empty(std::ifstream& pFile);

void loadGame(int save, entity &player, int &rounds, entity &enemy, bool &savedGame);

// set up scren
void setupScreen();

// Battle prototype
void battle(entity& player, entity enemy, int rounds);

// Point assign prototype
void assignStats(entity &player, int& defense, int& health, int& strength, int& agility, int statsPoints, int rounds, bool statsChosen);

bool intCheck(int integer);

void shop(entity &player, int rounds);

void saveGame(int save, entity player, int rounds);

bool check(string v);

bool reset(istream& in);

// RPG game where player can battle enemies
int main()
{
    //PlaySound(sound/attack.wav, NULL,SND_ASYNC | SND_FILENAME);
    //PlaySound("sound/attack-minor.wav", NULL,SND_ASYNC | SND_FILENAME);
    //PlaySound((R"(sound/dodge.wav)"), NULL,SND_ASYNC | SND_FILENAME);
    //PlaySound((R"(sound/protect.wav)"), NULL,SND_ASYNC | SND_FILENAME);
    //PlaySound((R"(sound/heal.wav)"), NULL,SND_ASYNC | SND_FILENAME);
    //PlaySound((R"(sound/strengthen.wav)"), NULL,SND_ASYNC | SND_FILENAME);
    //PlaySound("sound/music.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
    //PlaySound("sound/menuSelect.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);

    bool savedGame = false;

    setupScreen();

    // More rounds = stronger enemies
    int rounds = 0;
    int previousDef;


    // Random device creation
    random_device rd;
    random_device rd2;

    uniform_int_distribution<int> dist(0, 10);
    uniform_int_distribution<int> gearDist(0, 2);
    uniform_int_distribution<int> gearDist2(0, 2);
    uniform_int_distribution<int> swordDist(0, 2);
    uniform_int_distribution<int> coin(0, 1);

    // Gear and entities
    entity player;
    entity enemy;
    gear sword;
    gear shield;
    gear fist;

    // Defining enemy and player stats
    enemy.defense = 5;
    enemy.max_health = 5;
    enemy.health = enemy.max_health;
    enemy.agility = 3;
    enemy.strength = 5;
    enemy.xpWorth = 24 * (rounds + 1);
    enemy.isPlayer = false;

    player.isPlayer = true;
    player.xp = 0;
    player.level = 0;
    player.coins = 0;


    // Defining gear stats
    fist.damage = 0;
    fist.defense = 0;
    fist.name = "Fist";

    sword.damage = 2;
    sword.defense = 1;
    sword.name = "Damaged Sword";

    shield.damage = 1;
    shield.defense = 2;
    shield.name = "Damaged Shield";

    // Checks to see if the player is "ready to rumble"
    string ready;
    bool fighting = false;

    player.gear = fist;
    enemy.gear = fist;

    // counting files
    DIR *dp;
    int files = 0;
    struct dirent *ep;
    dp = opendir ("./saves");

    if (dp != NULL)
    {
        while (ep = readdir (dp))
            files++;

        (void) closedir (dp);
    }
    else
        perror ("Couldn't open the directory");

    files -= 2;
    bool isEmpty = false;

    vector<int> blankFiles;
    int j = 0;
    for (int i = 0; i < files; i++) {
        string fileToCheck = "saves/save" + to_string(i + 1) + ".txt";
        ifstream read(fileToCheck);

        if(!read) {
            ;
        }

        bool isEmpty = read.peek() == EOF;
        if (isEmpty) {
            blankFiles.push_back(i);
            j++;
        }
    }
    int saveSlot  = 0;
    if (blankFiles.size() == files) {
        cout << "You have no save files. Starting game.\n";
        sleep_for(1s);
    } else {
        cout << "Would you like to load a previously saved game? (Type anything else for no)\n";
        cout << "Current Files:\n";
        for (int i = 0; i < files; i++) {
            if (blankFiles.size() > i) {
                if (i != blankFiles[i]) {
                    string temp;
                    string saveToOpen = "saves/save" + to_string(i + 1) + ".txt";
                    vector<string> saves;
                    ifstream savefile;
                    savefile.open(saveToOpen);
                    if (!savefile) {
                        savedGame = false;
                    }
                    while (savefile) {
                        getline(savefile, temp);
                        saves.push_back(temp);
                        savedGame = true;
                    }

                    cout << "File " << i + 1 << ":" << endl;
                    cout << "\tRound: " << saves[0] << endl;
                    cout << "\tWeapon Name: " << saves[8] << endl;
                    cout << "\tLevel: " << saves[6] << endl;
                    cout << "\tCoins: " << saves[7] << endl;

                }
            } else {
                string temp;
                string saveToOpen = "saves/save" + to_string(i + 1) + ".txt";
                vector<string> saves;
                ifstream savefile;
                savefile.open(saveToOpen);
                if (!savefile) {
                    savedGame = false;
                }
                while (savefile) {
                    getline(savefile, temp);
                    saves.push_back(temp);
                    savedGame = true;
                }

                if (saves.size() > 11) {
                    cout << "File " << i + 1 << ":" << endl;
                    cout << "\tRound: " << saves[0] << endl;
                    cout << "\tWeapon Name: " << saves[8] << endl;
                    cout << "\tLevel: " << saves[6] << endl;
                    cout << "\tCoins: " << saves[7] << endl;
                } else {
                    cout << "File " << i + 1 << ":" << endl;
                    cout << "\tRound: " << "Fist" << endl;
                    cout << "\tWeapon Name: " << 0 << endl;
                    cout << "\tLevel: " << 0 << endl;
                    cout << "\tCoins: " << 0 << endl;
                }
            }
        }
        if (!(cin >> saveSlot)) {
            savedGame = false;
            cin.clear();
            cin.ignore(40000, '\n');
            system("CLS");
        } else if (saveSlot > 0){

            string fileToCheck = "saves/save" + to_string(saveSlot) + ".txt";
            ifstream read(fileToCheck);

            if(!read) {
                ;
            }

            bool isEmpty = read.peek() == EOF;

            if (isEmpty) {
                savedGame = false;
            }
            if (!isEmpty) {
                loadGame(saveSlot, player, rounds, enemy, savedGame);
            }
            cin.clear();
            cin.ignore(40000, '\n');
            system("CLS");
        } else {
            savedGame = false;
            cin.clear();
            cin.ignore(40000, '\n');
            system("CLS");
        }
    }

    // Check if player is fighting
    while (!fighting) {
        while (ready != "1") {
            cout << "";
            // Ask them if they will fight
            cout << "ARE YOU READY TO RUMBLE? (press 1 for yes, press any other button for no)\n";
            // Send response to ready variable
            getline(cin, ready);
        }
        if (!savedGame) {
            player.health = 1;
        }
        previousDef = player.defense;

        // If ready is 1
        while (ready == "1" && player.health > 0) {
            random_device rd3;
            uniform_int_distribution<int> rdxp(25, 75);
            uniform_int_distribution<int> rdcoin(7, 12);

            enemy.xpWorth =  rdxp(rd3) * (rounds + 1);
            enemy.coinWorth = rdcoin(rd3) * (rounds + 1);
            player.defense = previousDef;
            // Go to assign stats function
            if (rounds == 0) {
                assignStats(player, player.defense, player.max_health, player.strength, player.agility, 25, rounds, false);
            } else {
                assignStats(player, player.defense, player.max_health, player.strength, player.agility, 7, rounds, false);
            }
            // Set the player's current health to be the same as their max health
            player.health = player.max_health;
            // Prepare to exit loop
            fighting = true;
            // Tell player what their item is
            enemy.max_health += rounds;
            enemy.defense += rounds;
            enemy.strength += rounds;
            enemy.agility += rounds;
            enemy.health = enemy.max_health;
            previousDef = player.defense;
            // Start battle
            battle(player, enemy, rounds);

            //open shop
            if (player.health > 0) {
                shop(player, rounds);
            }

            previousDef = player.defense;
            rounds++;
            bool isEmpty;

            if ((saveSlot) != 0) {
                saveGame((saveSlot), player, rounds);
            } else {
                for (int i = 0; i < files; i++) {
                    string fileToCheck = "saves/save" + to_string(i + 1) + ".txt";
                    ifstream read(fileToCheck);

                    if(!read) {
                        ;
                    }

                    bool isEmpty = read.peek() == EOF;

                    if (isEmpty) {
                        saveGame(i + 1, player, rounds);
                        isEmpty = true;
                        break;
                    }
                    if (!isEmpty) {
                        saveGame(files + 1, player, rounds);
                        break;
                    }
                }
            }

        }
    }

}

// shop where player can buy and sell items
void shop(entity &player, int rounds) {

    string temp;

    gear playerGear = player.gear;
    vector<string> gearSuffixes;
    vector<string> gearPrefixes;
    vector<string> gearName;
    vector<gear> shopGear;
    random_device rd;



    ifstream file;
    file.open("names.txt");
    if (!file) {
        ;
    }

    while (getline(file, temp)) {
        gearName.push_back(temp);
    }
    file.close();



    ifstream file2;
    file2.open("prefixes.txt");
    if (!file2) {
        ;
    }

    while(getline(file2, temp)) {
        gearPrefixes.push_back(temp);
    }
    file2.close();


    ifstream file3;
    file3.open("suffixes.txt");
    if (!file3) {
        ;
    }

    while(getline(file3, temp)) {
        gearSuffixes.push_back(temp);
    }
    file3.close();

    uniform_int_distribution<int> prefixes(0, gearPrefixes.size() - 1);
    uniform_int_distribution<int> suffixes(0, gearSuffixes.size() - 1);
    uniform_int_distribution<int> names(0, gearName.size() - 1);
    uniform_int_distribution<int> damage(1 + rounds, 35 + rounds);
    uniform_int_distribution<int> defense(3 + rounds, 24 + rounds);
    uniform_int_distribution<int> cost(0 + rounds, 9 + rounds);

    for (int i = 0; i < 10; i++) {
        string prefix;
        string name;
        string suffix;
        prefix = gearPrefixes[prefixes(rd)];
        suffix = gearSuffixes[suffixes(rd)];
        name = gearName[names(rd)];

        gear randGear;
        string trueName = prefix + ' ' + name + ' ' + suffix;
        randGear.name = trueName;
        randGear.damage = damage(rd);
        randGear.defense = defense(rd);
        randGear.cost = randGear.damage + randGear.defense + cost(rd);
        shopGear.push_back(randGear);
    }

    cout << "Welcome to the shop! Your current balance is: " << player.coins << " coins." << endl;

    cout << "There are a variety of things you can buy:" << endl;

    for (int i = 0; i < shopGear.size(); i++) {
        cout << i + 1 << ". " << shopGear.at(i).name << " - " << yellow("Cost: ") << shopGear[i].cost << endl;
        cout << red("\tDMG: ") << shopGear[i].damage << endl;
        cout << green("\tDEF: ") << shopGear[i].defense << endl << endl;
    }
    cout << "\n\nCurrent Gear: " << player.gear.name << endl;
    cout << red("\tDMG: ") << player.gear.damage << endl;
    cout << green("\tDEF: ") << player.gear.defense << endl << endl << endl;

    int choice;

    bool itemChosen = false;

    cout << "\nChoose the number of the item you want to buy! (type 0 to leave)\n";

    while (!itemChosen) {

        if (choice == 0) {
            itemChosen = true;
        }
        while (!(cin >> choice)) {
            system("CLS");
            cout << "Welcome to the shop! Your current balance is: " << player.coins << " coins." << endl;
            cout << "There are a variety of things you can buy:" << endl;
            for (int i = 0; i < shopGear.size(); i++) {
                cout << i + 1 << ". " << shopGear.at(i).name << " - Cost:  (" << shopGear[i].cost << ")" << endl;
                cout << "\tDamage: " << shopGear[i].damage << endl;
                cout << "\tDefense: " << shopGear[i].defense << endl;
            }
            cout << "\n\nCurrent Gear: " << player.gear.name << endl;
            cout << "\tDamage: " << player.gear.damage << endl;
            cout << "\tDefense: " << player.gear.defense << endl << endl << endl;

            cout << "\nChoose the number of the item you want to buy! (type 0 to leave)\n";
            cin.clear();
            cin.ignore(40000, '\n');
        }
        if (choice == 0) {
            system("CLS");
            itemChosen = true;
            break;
        }


        if (choice > (shopGear.size() + 1) || choice <= 0) {
            system("CLS");
            cout << "Welcome to the shop! Your current balance is: " << player.coins << " coins." << endl;
            cout << "There are a variety of things you can buy:" << endl;
            for (int i = 0; i < shopGear.size(); i++) {
                cout << i + 1 << ". " << shopGear.at(i).name << " - Cost:  (" << shopGear[i].cost << ")" << endl;
                cout << "\tDamage: " << shopGear[i].damage << endl;
                cout << "\tDefense: " << shopGear[i].defense << endl;
            }
            cout << "\n\nCurrent Gear: " << player.gear.name << endl;
            cout << "\tDamage: " << player.gear.damage << endl;
            cout << "\tDefense: " << player.gear.defense << endl << endl << endl;
            cout << "\nChoose the number of the item you want to buy! (type 0 to leave)\n";
        } else {
            if (player.coins >= shopGear[choice - 1].cost) {
                player.gear = shopGear[choice - 1];
                player.coins -= shopGear[choice - 1].cost;
                cout << "\nThank you for your patronage!\n";
                PlaySound("sound/purchase.wav", NULL,SND_ASYNC | SND_FILENAME);
                system("CLS");
                itemChosen = true;
            } else {
                system("CLS");
                cout << "Welcome to the shop! Your current balance is: " << player.coins << " coins." << endl;
                cout << "There are a variety of things you can buy:" << endl;
                for (int i = 0; i < shopGear.size(); i++) {
                    cout << i + 1 << ". " << shopGear.at(i).name << " - Cost:  (" << shopGear[i].cost << ")" << endl;
                    cout << "\tDamage: " << shopGear[i].damage << endl;
                    cout << "\tDefense: " << shopGear[i].defense << endl;
                }
                cout << "\n\nCurrent Gear: " << player.gear.name << endl;
                cout << "\tDamage: " << player.gear.damage << endl;
                cout << "\tDefense: " << player.gear.defense << endl << endl << endl;
                cout << "\nSorry, you're too poor.\n";
            }
        }
    }



}

// Assign player stats function
void assignStats(entity &player, int &defense, int &health, int &strength, int &agility, int statsPoints, int rounds, bool statsChosen) {
     if (rounds == 0) {
         defense = 0;
         health = 0;
         strength = 0;
         agility = 0;
     }
    int defenseAddition = 0;
    int healthAddition = 0;
    int strengthAddition = 0;
    int agilityAddition = 0;
    int statsPointsMax = statsPoints;
    // Tell player the amount of stat points left
    cout << "\nYou have " << statsPoints << " stat points remaining.";
    cout << "\nThere are four different categories:\n";
    cout << light_red("HP (Hit Points)");
    cout << " | The amount of hit points you have remaining. When they hit 0, you die.\n";
    cout << red("STR (Strength)");
    cout << " | This is the power of your attack, which can also hurt the player's defense.\n";
    cout << green("DEF (Defense)");
    cout << " | Acts as a second HP bar, and is effective at countering strengths lower than it.\n";
    cout << aqua("AGY (Agility)");
    cout <<" | Agility increases your chance of dodging an attack, which negates all damage.\n\n";

    cout << "\nYou have " << health << light_red(" HP\n");
    cout << "\nYou have " << strength << red(" STR") << " (+" << player.gear.damage << red(" DMG") << " weapon modifier)\n";
    cout << "\nYou have " << defense << green(" DEF") << " (+" << player.gear.defense << green(" DEF") << " weapon modifier)\n";
    cout << "\nYou have " << agility << aqua(" AGY\n\n\n");
    // make this in case player fails to put stats in right
    int statsPointsPrevious = statsPoints;
    int stupidCounter = 0;
    while (!statsChosen) {
        // Check if all of these are chosen and have them in while loops
        bool defenseChosen = false;
        bool attackChosen = false;
        bool healthChosen = false;
        bool agilityChosen = false;

        while (!healthChosen) {
            cout << "\nChoose your" << light_red(" HP") << ":\n";
            while (!(cin >> healthAddition)) {
                stupidCounter++;
                if (stupidCounter < 3) {
                    cout << "\nThis must be a number betwen 0 and " << statsPointsPrevious << ":\n";
                    cin.clear();
                    cin.ignore(40000, '\n');
                }
                else {
                    cout << "\nYou're too stupid to figure this out I give up\n";
                    exit(0);
                }
            }
            statsPoints -= healthAddition;
            if (statsPoints < 0 || statsPoints > statsPointsPrevious) {
                if (stupidCounter < 3) {
                    cout << "\nThis must be a number betwen 0 and " << statsPointsPrevious << ":\n";
                    statsPoints = statsPointsPrevious;
                    stupidCounter++;
                }
                else {
                    cout << "\nYou're too stupid to figure this out I give up\n";
                    exit(0);
                }
            }
            else {
                cout << "\nYou have " << statsPoints << " stat points remaining.";
                statsPointsPrevious -= healthAddition;
                stupidCounter = 0;
                healthChosen = true;
                PlaySound("sound/menuSelect.wav", NULL, SND_ASYNC | SND_FILENAME);
            }
        }

        // Attack
        while (!attackChosen) {
            cout << "\nChoose your" << red(" STR") << ":\n";
            while (!(cin >> strengthAddition)) {
                stupidCounter++;
                if (stupidCounter < 2) {
                    cout << "\nThis must be a number betwen 0 and " << statsPointsPrevious << ":\n";
                    cin.clear();
                    cin.ignore(40000, '\n');
                }
                else {
                    cout << "\nYou're too stupid to figure this out I give up\n";
                    exit(0);
                }
            }
            statsPoints -= strengthAddition;
            if (statsPoints < 0 || statsPoints > statsPointsPrevious) {
                if (stupidCounter < 3) {
                    cout << "\nThis must be a number betwen 0 and " << statsPointsPrevious << ":\n";
                    statsPoints = statsPointsPrevious;
                    stupidCounter++;
                }
                else {
                    cout << "\nYou're too stupid to figure this out I give up\n";
                    exit(0);
                }
            }
            else {
                cout << "\nYou have " << statsPoints << " stat points remaining.";
                statsPointsPrevious -= strengthAddition;
                stupidCounter = 0;
                attackChosen = true;
                PlaySound("sound/menuSelect.wav", NULL, SND_ASYNC | SND_FILENAME);
            }
        }

        // Defense
        while (!defenseChosen){
            cout << "\nChoose your" << green(" DEF") << ":\n";
            while (!(cin >> defenseAddition)) {
                stupidCounter++;
                if (stupidCounter < 3) {
                    cout << "\nThis must be a number betwen 0 and " << statsPointsPrevious << ":\n";
                    cin.clear();
                    cin.ignore(40000, '\n');
                }
                else {
                    cout << "\nYou're too stupid to figure this out I give up\n";
                    exit(0);
                }
            }
            // Subtract points put in defense from stat points
            statsPoints -= defenseAddition;
            // Check if player put invalid input
            if (statsPoints < 0 || statsPoints > statsPointsPrevious) {
                if (stupidCounter < 3) {
                    cout << "\nThis must be a number betwen 0 and " << statsPointsPrevious << ":\n";
                    statsPoints = statsPointsPrevious;
                    stupidCounter++;
                }
                else {
                    cout << "\nYou're too stupid to figure this out I give up\n";
                    exit(0);
                }
            }
            // If player put in valid input
            else if (statsPoints >= 0 && statsPoints <= statsPointsPrevious){
                // Subtract the defense points from the stat point "save state" and tell player how many stat points they have left
                cout << "\nYou have " << statsPoints << " stat points remaining.";
                statsPointsPrevious -= defenseAddition;
                defenseChosen = true;
                PlaySound("sound/menuSelect.wav", NULL, SND_ASYNC | SND_FILENAME);
            }
            else {
                exit(0);
            }
        }


        while (!agilityChosen) {
            cout << "\nChoose your" << aqua(" AGY") << ":\n";
            while (!(cin >> agilityAddition)) {
                stupidCounter++;
                if (stupidCounter < 2) {
                    cout << "\nThis must be a number betwen 0 and " << statsPointsPrevious << ":\n";
                    cin.clear();
                    cin.ignore(40000, '\n');
                }
                else {
                    cout << "\nYou're too stupid to figure this out I give up\n";
                    exit(0);
                }
            }
            // Subtract points put in agility from stat points
            statsPoints -= agilityAddition;
            // Check if player put invalid input
            if (statsPoints < 0 || statsPoints > statsPointsMax) {
                if (stupidCounter < 2) {
                    cout << "\nThis must be a number betwen 0 and " << statsPointsPrevious << ":\n";
                    statsPoints = statsPointsPrevious;
                    stupidCounter++;
                }
                else {
                    cout << "\nYou're too stupid to figure this out I give up\n";
                    exit(0);
                }
            }
                // If player put in valid input
            else {
                // Subtract the defense points from the stat point "save state" and tell player how many stat points they have left
                cout << "\nYou have " << statsPoints << " stat points remaining.";
                statsPointsPrevious -= agilityAddition;
                agilityChosen = true;
                stupidCounter = 0;
                statsChosen = true;
                PlaySound("sound/menuSelect.wav", NULL, SND_ASYNC | SND_FILENAME);
            }
        }

        defense += defenseAddition;
        health += healthAddition;
        strength += strengthAddition;
        agility += agilityAddition;
        // Clear the console
        system("CLS");
    }
}

// Battle function
void battle(entity &player, entity enemy, int rounds) {
    int PDI = (int) player.defense * 0.25;
    int PSI = (int) player.strength * 0.25;
    int PHI = (int) player.health * 0.25;

    int EDI = (int) enemy.defense * 0.25;
    int ESI = (int) enemy.strength * 0.25;
    int EHI = (int) enemy.health * 0.25;

    int startingDef = player.defense;
    int strIncreaseTurn;
    int enemyStrIncreaseTurn;

    bool defIncrease = false;
    bool strIncrease = false;
    int playerDefPrevious = player.defense;

    bool enemyDefIncrease = false;
    bool enemyStrIncrease = false;
    int enemyDefPrevious = player.defense;

    // Turn and whether or not the battle is finished
    int turn = 0;
    bool end = false;

    // Chance of dodging is affected by agility
    double dodgeChance = (double)rand() / RAND_MAX;


    // Checks to see if the player or enemy has won
    bool playerWin = false;
    bool enemyWin = false;

    // detect whether or not the options are chosen
    bool optionsChosen = false;

    int playerStrNormal = player.strength;
    int enemyStrNormal = enemy.strength;
    // Battle loop
    cout << "ROUND " << rounds + 1 << "!" << " BEGIN!\n";
    while (!end) {
        // Detect if player or enemy health is 0 and end battle
        if (player.health <= 0) {
            end = true;
            enemyWin = true;
        }
        else if (enemy.health <= 0) {
            end = true;
            playerWin = true;
        }
        else {
            //Announce turn, and stats and get options
            int option = 0;
            cout << "It is your turn. You are at turn " << turn+1 << "." << endl << endl;
            cout << "    Stats:";
            cout << right << setw(20) << fixed << "Enemy Stats:\n";
            cout << light_red("HP  |") << player.health;
            cout << right << setw(24 - to_string(player.health).size()) << fixed << enemy.health << light_red("|") << endl;
            cout << red("STR |") << player.strength + player.gear.damage;
            cout << right << setw(24 - to_string(player.strength + player.gear.damage).size()) << fixed << enemy.strength << red("|") << endl;
            cout << green("DEF |") << player.defense + player.gear.defense;
            cout << right << setw(24 - to_string(player.defense + player.gear.defense).size()) << fixed << enemy.defense << green("|") << endl;
            cout << aqua("AGY |") <<  player.agility;
            cout << right << setw(24 - to_string(player.agility).size()) << fixed << enemy.agility << aqua("|") << endl << endl;

            cout << "Actions:\n";
            cout << "1." <<  red("Attack") << " | Attack the enemy.\n";
            cout << "2." << light_red("Heal") << " | Heal 25% of your health.\n";
            cout << "3." << green("Protect") << " | Increase your " << green("DEF") << " by 25% for 1 turn.\n";
            cout << "4." << purple("Overdrive") << " | Increase your " << red("STR") << " by 25% for 1 turn.\n";
            cout << "Your choice: ";
            while (!(cin >> option)) {
                cout << "" << endl;
                cin.clear();
                system("CLS");
                cout << "It is your turn. You are at turn " << turn+1 << "." << endl << endl;
                cout << "    Stats:";
                cout << right << setw(20) << fixed << "Enemy Stats:\n";
                cout << light_red("HP  |") << player.health;
                cout << right << setw(24 - to_string(player.health).size()) << fixed << enemy.health << light_red("|") << endl;
                cout << red("STR |") << player.strength + player.gear.damage;
                cout << right << setw(24 - to_string(player.strength + player.gear.damage).size()) << fixed << enemy.strength << red("|") << endl;
                cout << green("DEF |") << player.defense + player.gear.defense;
                cout << right << setw(24 - to_string(player.defense + player.gear.defense).size()) << fixed << enemy.defense << green("|") << endl;
                cout << aqua("AGY |") <<  player.agility;
                cout << right << setw(24 - to_string(player.agility).size()) << fixed << enemy.agility << aqua("|") << endl << endl;
                cout << "Actions:\n";
                cout << "1." <<  red("Attack") << " | Attack the enemy.\n";
                cout << "2." << light_red("Heal") << " | Heal 25% of your health.\n";
                cout << "3." << green("Protect") << " | Increase your " << green("DEF") << " by 25% for 1 turn.\n";
                cout << "4." << purple("Overdrive") << " | Increase your " << red("STR") << " by 25% for 1 turn.\n";
                cout << "Your choice: ";
                cin.ignore(1);
            }

            bool dodge_bool = false;
            uniform_int_distribution<int> dodge(0, 100);
            random_device rd_dodge;
            int dodge_divisor = 2;
            if (enemy.agility <= 20) {
                dodge_divisor = 10;
            } else if (enemy.agility <= 30) {
                dodge_divisor = 9;
            } else if (enemy.agility <= 40) {
                dodge_divisor = 8;
            } else if (enemy.agility <= 50) {
                dodge_divisor = 7;
            } else if (enemy.agility <= 60) {
                dodge_divisor = 6;
            } else if (enemy.agility <= 70) {
                dodge_divisor = 5;
            } else if (enemy.agility <= 80) {
                dodge_divisor = 4;
            } else if (enemy.agility <= 90) {
                dodge_divisor = 3;
            } else if (enemy.agility <= 100) {
                dodge_divisor = 2;
            } else if (enemy.agility > 100) {
                dodge_divisor = 1.9;
            }

            if (dodge(rd_dodge) <= enemy.agility - player.agility/dodge_divisor) {
                dodge_bool = true;
            }

            // Detect options
            switch (option) {
                case 1:

                    // If the player strength is less than or equal to the enemy defense
                    if (enemy.defense >= player.strength + player.gear.damage && !dodge_bool) {

                        // deal the strength of the player / 5 to the enemy defense
                        int hurt = (int)(player.strength + player.gear.damage) / 5;
                        // if it is 0 damage, then make it 1
                        if (hurt == 0) {
                            hurt = 1;
                        }
                        enemy.defense -= hurt;

                        // If the enemy defense is less than or equal to 0, make it 0 (prevent negatives from showing)
                        if (enemy.defense <= 0) {
                            enemy.defense = 0;
                            hurt = 0;
                        }
                        PlaySound("sound/attack-minor.wav", NULL,SND_ASYNC | SND_FILENAME);
                        // Print damage done
                        cout << "You weren't strong enough to pierce the enemy's " << green("DEF") << ", but hit their " << green("DEF") << " for " << hurt << red(" DMG") << "." << endl;

                    }
                    else if (dodge_bool) {
                        PlaySound("sound/dodge.wav", NULL,SND_ASYNC | SND_FILENAME);
                        cout << "The enemy dodged your attack with their " << aqua("AGY") << "." << endl;
                        sleep_for(2s);
                    }
                        // If the player strength is more than the enemy defense
                    else {
                        // Make strength value equivalent to player strength
                        int strength = player.strength + player.gear.damage;
                        // Subtract the enemies defense from this strength value (so that the players original defense remains unchanged
                        strength -= enemy.defense;
                        // Deal half of the players strength to the defense
                        enemy.defense -= (int)(player.strength + player.gear.damage / 2);
                        // Deal what is left of the strength variable to the player's defense
                        enemy.health -= strength;

                        // If the enemy defense is less than or equal to 0, make it 0 (prevent negatives from showing)
                        if (enemy.defense <= 0) {
                            enemy.defense = 0;
                        }
                        PlaySound("sound/attack.wav", NULL,SND_ASYNC | SND_FILENAME);
                        // Print damage done using strength variable
                        cout << "You pierced the enemies " << green("DEF ") << "and hit them for " << strength << red(" DMG") << "!" << endl << endl;

                    }
                    // Increase the turn by 1
                    turn++;
                    // Confirm an option has been chosen
                    optionsChosen = true;
                    break;
                case 2:
                    // If the player's health + 2 is less than or equal to max health, heal them
                    if (player.health == player.max_health) {
                        cout << "You didn't heal anything because you were already at max" << light_red(" HP.") << endl << endl;
                    }
                    else if (player.health + PHI >= player.max_health) {
                        cout << "You healed yourself for " << player.max_health - player.health << light_red(" HP.") << endl << endl;
                        player.health = player.max_health;

                        PlaySound("sound/heal.wav", NULL,SND_ASYNC | SND_FILENAME);
                    }
                        // If the player's health + 1 is less than or equal to max health, heal them
                    else if (player.health + PHI < player.max_health) {
                        cout << "You healed yourself for " << PHI << light_red(" HP.") << endl << endl;
                        player.health += PHI;
                        PlaySound("sound/heal.wav", NULL,SND_ASYNC | SND_FILENAME);
                    }   
                    // Increase turn
                    turn++;
                    // put option chosen as true
                    optionsChosen = true;
                    break;
                case 3:
                    playerDefPrevious = player.defense;
                    player.defense += PDI;

                    cout << "You temporarily increased your" << green(" DEF") << " for 1 turn by " << PDI << " points" << endl << endl;
                    PlaySound("sound/protect.wav", NULL,SND_ASYNC | SND_FILENAME);
                    defIncrease = true;
                    optionsChosen = true;
                    turn++;
                    break;
                case 4:
                    player.strength += PSI;
                    cout << "You temporarily increased your" << red(" STR") << " for 1 turn by "<< PSI << " points." << endl << endl;
                    PlaySound("sound/strengthen.wav", NULL,SND_ASYNC | SND_FILENAME);
                    optionsChosen = true;
                    turn++;
                    if (!strIncrease) {
                        strIncreaseTurn = turn;
                    }
                    strIncrease = true;
                    break;
                default:
                    // bozo
                    cout << "bozo" << endl << endl;
                    system("CLS");
                case 0:
                    // bozo
                    cout << "bozo" << endl << endl;
                    system("CLS");
            }
            if (strIncrease && strIncreaseTurn != turn) {
                strIncrease = false;
                cout << "Your overdrive ran out of juice and your " << red("STR") << " decreased by " << PSI << " points.\n";
                player.strength = playerStrNormal;
                if (option == 4) {
                    player.strength += PSI;
                    strIncrease = true;
                }
            }
            if (enemyDefIncrease) {
                enemyDefIncrease = false;
                cout << "The enemy stopped protecting and their " << green("DEF") << " reset to " << enemyDefPrevious << ".\n";
                enemy.defense = enemyDefPrevious;
            }


            // Detect if the enemy can make an action
            if (optionsChosen && enemy.health > 0) {
                bool dodge_bool = false;
                // Random device (for enemy move)
                uniform_int_distribution<int> coin(0, 3);
                random_device rd;

                uniform_int_distribution<int> dodge(0, 100);
                random_device rd_dodge;
                int dodge_divisor = 2;
                if (player.agility <= 20) {
                    dodge_divisor = 6;
                } else if (player.agility <= 40) {
                    dodge_divisor = 5;
                } else if (player.agility <= 50) {
                    dodge_divisor = 4;
                } else if (player.agility <= 75) {
                    dodge_divisor = 3;
                } else if (player.agility <= 100) {
                    dodge_divisor = 2;
                }


                if (dodge(rd_dodge) <= player.agility - enemy.agility/dodge_divisor) {
                    dodge_bool = true;
                }

                sleep_for(1s);
                cout << "The enemy is ready to fight!\n";
                sleep_for(1s);

                // detect if coin flip is 0 or the enemy's health is at its max (so they dont try to heal at max health)
                // Uses same code as player

                switch (coin(rd)) {
                    case 0:
                        if (player.defense + player.gear.defense >= enemy.strength && !dodge_bool) {

                            int hurt = (int)enemy.strength / 5;
                            player.defense -= (int)enemy.strength / 5;
                            if (hurt == 0) {
                                hurt = 1;
                            }
                            cout << "The enemy wasn't strong enough to pierce your " << green("DEF") << ", but hit your " << green("DEF") << " for " << hurt << red(" DMG") << "." << endl;
                            PlaySound("sound/attack-minor.wav", NULL,SND_ASYNC | SND_FILENAME);
                        }
                        else if (dodge_bool) {
                            cout << "You completely dodged the enemy's attack with your " << aqua("AGY") << ".\n";
                            PlaySound("sound/dodge.wav", NULL,SND_ASYNC | SND_FILENAME);
                            sleep_for(2s);
                        }
                        else {
                            int strength = enemy.strength;
                            strength -= player.defense;
                            player.defense -= (int)(enemy.strength / 4);
                            player.health -= strength;
                            if (player.defense < 0) {
                                player.defense = 0;
                            }
                            cout << "The enemy pierced your defenses and hit you for " << strength << red(" DMG") << "!\n";
                            PlaySound("sound/attack.wav", NULL,SND_ASYNC | SND_FILENAME);

                        }
                        break;
                    case 1:
                        if (enemy.health + EHI > enemy.max_health) {
                            enemy.health = enemy.max_health;
                            cout << "The enemy healed themselves for " << enemy.max_health - enemy.health << light_red(" HP") << ".\n";
                            PlaySound("sound/heal.wav", NULL,SND_ASYNC | SND_FILENAME);
                        }
                        else if (enemy.health + EHI <= enemy.max_health) {
                            enemy.health += EHI;
                            cout << "The enemy healed themselves for " << EHI << light_red(" HP") << ".\n";
                            PlaySound("sound/heal.wav", NULL,SND_ASYNC | SND_FILENAME);
                        }
                        else {
                            cout << "The enemy pulled a bozo move and healed themselves for 0 " << light_red("HP") << ".\n";
                        }
                        break;
                    case 2:
                        if (!enemyStrIncrease) {
                            enemyStrIncreaseTurn = turn;
                        }
                        enemy.strength += ESI;
                        cout << "The enemy temporarily increased their" << red(" STR") << " for 1 turn by " << ESI << " points." << endl << endl;
                        PlaySound("sound/strengthen.wav", NULL,SND_ASYNC | SND_FILENAME);
                        enemyStrIncrease = true;
                        break;
                    case 3:
                        enemyDefPrevious = enemy.defense;
                        enemy.defense += EDI;
                        cout << "The enemy temporarily increased their" << green(" DEF") << " for 1 turn by " << EDI << " points" << endl << endl;
                        PlaySound("sound/protect.wav", NULL,SND_ASYNC | SND_FILENAME);
                        enemyDefIncrease = true;
                        break;
                    default:
                        break;
                }
                if (defIncrease) {
                    defIncrease = false;
                    cout << "You stopped protecting and your " << green("DEF") << " reset to " << playerDefPrevious << "." << endl;
                    player.defense = playerDefPrevious;
                }
                if (enemyStrIncrease && enemyStrIncreaseTurn != turn) {
                    enemyStrIncrease = false;
                    cout << "The enemy's overdrive ran out of juice and their " << red("STR") << " decreased by " << ESI << " points.\n";
                    enemy.strength = enemyStrNormal;
                    if (coin(rd) == 2) {
                        enemy.strength += ESI;
                    }
                }
                sleep_for(5s);
                system("CLS");
            }
        }
    }

    // rip
    if (turn == 0) {
        cout << "\nWell, it looks like the battle ended before it could even start.";
    }
        // Message if the player wins
    else if (playerWin) {
        cout << "\nYou beat the enemy, and still had " << player.health << light_red(" HP") << " remaining.";
        player.xp += enemy.xpWorth;
        player.defense = startingDef;
        cout << "\nYou got " << enemy.coinWorth << " coins!\n";
        cout << "You got " << enemy.xpWorth << " xp!\n";
        player.levelUp();
        player.coins += enemy.coinWorth;
        startingDef = player.defense;

        sleep_for(5s);
        system("CLS");
        player.defense = startingDef;
    }
        // Message if the enemy wins
    else if (enemyWin) {
        cout << "\nYou were fatally wounded by the enemy, who still had " << enemy.health <<  light_red(" HP") << " remaining.";
        sleep_for(2s);
        system("CLS");
    }
        // e
    else {
        cout << "Either this is a glitch and you should inform the developer, or YOURE A DIRTY CHEATER";
        exit(101);
    }
}

void saveGame (int save, entity player, int rounds) {
    string fileToOpen = "saves/save" + to_string(save) + ".txt";

    ofstream exportFile;
    exportFile.open(fileToOpen, ofstream::trunc);
    exportFile << rounds << endl;
    exportFile << player.max_health << endl;
    exportFile << player.strength << endl;
    exportFile << player.defense << endl;
    exportFile << player.agility << endl;
    exportFile << player.xp << endl;
    exportFile << player.level << endl;
    exportFile << player.coins << endl;
    exportFile << player.gear.name << endl;
    exportFile << player.gear.damage << endl;
    exportFile << player.gear.defense << endl;
    exportFile.close();
}

void loadGame(int save, entity &player, int &rounds, entity &enemy, bool &savedGame) {
    savedGame = false;
    string temp;
    string saveToOpen = "saves/save" + to_string(save) + ".txt";
    vector<string> saves;
    ifstream savefile;
    savefile.open(saveToOpen);
    if (!save) {
        savedGame = false;
    }
    while (savefile) {
        getline(savefile, temp);
        saves.push_back(temp);
        savedGame = true;
    }

    if (savedGame) {
        rounds = stoi(saves[0]);
        player.max_health = stoi(saves[1]);
        player.strength = stoi(saves[2]);
        player.defense = stoi(saves[3]);
        player.agility = stoi(saves[4]);
        player.xp = stoi(saves[5]);
        player.level = stoi(saves[6]);
        player.coins = stoi(saves[7]);
        player.gear.name = saves[8];
        player.gear.damage = stoi(saves[9]);
        player.gear.defense = stoi(saves[10]);
        for (int i = 0; i < rounds; i++) {
            enemy.max_health += i;
            enemy.defense += i;
            enemy.strength += i;
            enemy.agility += i;
        }
        player.health = player.max_health;
    }
    savefile.close();

}

// setup screen to be fullscreen
void setupScreen() {
    ::SendMessage(::GetConsoleWindow(), WM_SYSKEYDOWN, VK_RETURN, 0x20000000);
    // get handle to the console window
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    // retrieve screen buffer info
    CONSOLE_SCREEN_BUFFER_INFO scrBufferInfo;
    GetConsoleScreenBufferInfo(hOut, &scrBufferInfo);

    // current window size
    short winWidth = scrBufferInfo.srWindow.Right - scrBufferInfo.srWindow.Left + 1;
    short winHeight = scrBufferInfo.srWindow.Bottom - scrBufferInfo.srWindow.Top + 1;

    // current screen buffer size
    short scrBufferWidth = scrBufferInfo.dwSize.X;
    short scrBufferHeight = scrBufferInfo.dwSize.Y;

    // to remove the scrollbar, make sure the window height matches the screen buffer height
    COORD newSize;
    newSize.X = scrBufferWidth;
    newSize.Y = winHeight;

    // set the new screen buffer dimensions
    int Status = SetConsoleScreenBufferSize(hOut, newSize);
    if (Status == 0)
    {
        cout << "SetConsoleScreenBufferSize() failed! Reason : " << GetLastError() << endl;
        exit(Status);
    }

    // print the current screen buffer dimensions
    GetConsoleScreenBufferInfo(hOut, &scrBufferInfo);
}

entity::entity() {}
entity::~entity() {}
gear::gear() {}
gear::~gear() {}