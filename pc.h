#ifndef PC_H
#define PC_H

typedef struct dungeon dungeon;
typedef struct pc pc;
typedef struct character character;

typedef struct pc {
} pc;

void playerGen(dungeon *d);
void movePC(dungeon *d, int yOff, int xOff);
void movePC(dungeon *d, int yOff, int xOff, bool override);
character *findPC(dungeon *d);

#endif