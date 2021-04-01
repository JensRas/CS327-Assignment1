#ifndef NPC_H
#define NPC_H

#include <cstdint>
#include <iostream>
#include <fstream>

#define defaultMonNum 30 
#define BIT_SMART     0x1
#define BIT_TELE      0x2
#define BIT_TUN       0x4
#define BIT_ERAT      0x8
#define BIT_PASS      0x10
#define BIT_PICKUP    0x20
#define BIT_DESTROY   0x40
#define BIT_UNIQ      0x80
#define BIT_BOSS      0x100

typedef struct dungeon dungeon;
typedef struct character character;

typedef struct npc {
    int8_t type;
    int8_t knownPCX;
    int8_t knownPCY;
    char *adj;
} npc;

void monsterGen(dungeon *d);
char *nameMonst();
void moveMonst();
void makeMonstList(dungeon *d);
int parseMonFile(std::fstream &f, dungeon *d);

#endif