#pragma once
#include <iostream>
#include "gear.h"
using namespace std;

// Entity (both enemies and players have this)
class entity {
public:
    entity();
    ~entity();

    int health;
    int max_health;
    int strength;
    int agility;
    int defense;
    bool isPlayer;
    int xp;
    int level;
    int xpWorth;
    int coinWorth;
    int coins;
    gear gear;

    void levelUp();
};