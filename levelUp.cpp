#include <iostream>
#include "entity.h"
#include <chrono>
#include <thread>
#include <windows.h>
#include <mmsystem.h>
#include <random>

using namespace std::chrono_literals;
using namespace std::this_thread;

// Detect if player has enough xp to level up
void entity::levelUp() {
    random_device rd;
    uniform_int_distribution<int> levelSound(1, 3);

    if (isPlayer) {
        int levelPrevious = level;
        // leveling system is 100 x level is the amount of xp you need (lel)
        while (100 * level <= xp) {
            int previousMaxHP = max_health;
            int previousDEF = defense;
            int previousSTR = strength;
            int previousAGY = agility;
            xp -= 100*level;
            level += 1;
            cout << "\n\n\nYou leveled up! You are now level " << level << "." << endl;
            if (levelSound(rd) == 1) {
                PlaySound("sound/LevelUp1.wav", NULL, SND_ASYNC | SND_FILENAME);
            } else if (levelSound(rd) == 2) {
                PlaySound("sound/LevelUp2.wav", NULL, SND_ASYNC | SND_FILENAME);
            } else if (levelSound(rd) == 3) {
                PlaySound("sound/LevelUp3.wav", NULL, SND_ASYNC | SND_FILENAME);
            } else {
                PlaySound("sound/LevelUp3.wav", NULL, SND_ASYNC | SND_FILENAME);
            }
            sleep_for(2s);
            max_health += level;
            defense += level;
            strength += level;
            agility += level;

            cout << ("\n\nHP: ") << previousMaxHP << " ---> " << max_health << endl;
            sleep_for(2s);
            cout << ("DEF: ") << previousDEF << " ---> " << defense << endl;
            sleep_for(2s);
            cout << ("STR: ") << previousSTR << " ---> " << strength << endl;
            sleep_for(2s);
            cout << ("AGY: ") << previousAGY << " ---> " << agility << endl;
            sleep_for(2s);
            previousMaxHP = max_health;
            previousDEF = defense;
            previousSTR = strength;
            previousAGY = agility;
        }
        //if (levelPrevious == level) {
        cout << endl;
        cout << endl;
        int barWidth = 100;

        cout << "Level " << level << " ";
        cout << "[";
        int pos = xp/level;
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos){
                cout << "=";
                sleep_for(0.025s);
            }
            else if (i == pos) {
                cout << ">";
                sleep_for(0.025s);
            }
            else {
                cout << "-";
                sleep_for(0.025s);
            }
        }
        sleep_for(0.025s);
        cout << "] ";
        sleep_for(0.025s);
        cout << " Level " << level + 1 << " (";
        cout << int((xp/level*100)*0.01) << " %)\r";
        sleep_for(5s);

        cout << "Level " << level << " ";
        cout << "[";

        for (int i = 0; i < barWidth; ++i) {
            if (i < pos){
                cout << "=";
            }
            else if (i == pos) {
                cout << ">";
            }
            else {
                cout << "-";
            }
        }
        cout << "] ";

        cout << " Level " << level + 1 << " (";
        cout << xp << "/" << level*100 << ")";

        //}
    }
}