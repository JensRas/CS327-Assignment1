#ifndef PATHFINDER_H
#define PATHFINDER_H

typedef struct dungeon d;

//static int32_t corridor_path_cmp(const void *key, const void *with);
void dijkstra(dungeon *d, char str[]);

#endif