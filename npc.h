#ifndef NPC_H
#define NPC_H

#include <cstdint>
#include <string>

#define defaultMonNum 30 
#define BIT_SMART     0x1
#define BIT_TELE      0x2
#define BIT_TUN       0x4
#define BIT_ERAT      0x8

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

#endif