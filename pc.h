#ifndef PC_H
#define PC_H

typedef struct dungeon d;
typedef struct pc pc;

void playerGen(dungeon *d);
void movePC(dungeon *d, int yOff, int xOff);
character *findPC(dungeon *d);

#endif