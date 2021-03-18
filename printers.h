#ifndef PRINTERS_H
#define PRINTERS_H

typedef struct dungeon dungeon;
typedef struct pc pc;
typedef struct character character;

void printGame(dungeon *d);
void printMap(dungeon *d);
void terminalInit();

#endif