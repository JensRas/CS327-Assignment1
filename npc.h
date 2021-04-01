#ifndef NPC_H
#define NPC_H

#include <cstdint>
#include <iostream>
#include <fstream>

#define defaultMonNum 30 

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
void monsAttk(dungeon *d);
character *moveMonst(dungeon *d, int newY, int newX, character *npc);
void makeMonstList(dungeon *d);

#endif