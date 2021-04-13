#ifndef NPC_H
#define NPC_H

#include <cstdint>
#include <iostream>
#include <fstream>

#include "dice.h"
#include "dungeon.h"

#define defaultMonNum 30 

class monDesc1;
typedef uint32_t npc_type;

class npc {
    public:
        npc(dungeon *d, monDesc1 &m);
        ~npc();
        npc_type type;
        uint32_t seenPC;
        int8_t knownPCX;
        int8_t knownPCY;
        const char *adj;
        monDesc1 &md; 
};

class monDesc1 {
    public:
        std::string name;
        std::string symbol;
        uint8_t color = 0;
        dice speed;
        uint16_t ability = 0;
        dice health;
        dice damage;
        std::string desc;
        uint8_t rarity = 0;
        npc *generateNpc();
};

void monsterGen(dungeon *d);
char *nameMonst();
void monsAttk(dungeon *d);
character *moveMonst(dungeon *d, int newY, int newX, character *npc);
void makeMonstList(dungeon *d);

#endif