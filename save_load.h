#ifndef SAVE_LOAD_H
#define SAVE_LOAD_H

#include <stdio.h>

typedef struct dungeon d;

char *findFilePath();
void saveGame(FILE *f, dungeon d);
void loadGame(FILE *f, dungeon *d);

#endif