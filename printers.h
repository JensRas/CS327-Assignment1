#ifndef PRINTERS_H
#define PRINTERS_H

#include <string>

typedef struct dungeon dungeon;
typedef struct pc pc;
typedef struct character character;

void printGame(dungeon *d, bool fog);
void printMap(dungeon *d);
std::string findFilePath(int x, std::string s);
void printMonDef(dungeon *d);
void terminalInit();

#endif