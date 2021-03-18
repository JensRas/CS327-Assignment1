#ifndef MOVE_H
#define MOVE_H

typedef struct dungeon dungeon;
typedef struct character character;

void gameRunner(dungeon *d);
void runGame(dungeon *d);
//static int32_t monster_cmp(const void *first, const void *second);
void winGame();
void loseGame();

#endif