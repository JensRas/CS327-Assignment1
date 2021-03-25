#ifndef PRINTERS_H
#define PRINTERS_H

typedef struct dungeon dungeon;
typedef struct pc pc;
typedef struct character character;

void printGame(dungeon *d, bool fog);
void printMap(dungeon *d);
void terminalInit();

#endif