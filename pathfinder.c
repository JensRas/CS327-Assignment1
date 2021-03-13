#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "dungeon.h"
#include "heap.h"

/*****************************************
 *        Corridor Path Compare          *
 *****************************************/
static int32_t corridor_path_cmp(const void *key, const void *with) 
{
    return ((corPath *) key)->cost - ((corPath *) with)->cost;
}

/*****************************************
 *         Dungeon Cost Finder           *
 *****************************************/
void dijkstra(dungeon *d, char str[]) 
{
    static corPath *p;
    static uint32_t initialized = 0;
    heap_t h;
    uint32_t x, y;

    if (!initialized) {
        for (y = 0; y < floorMaxY; y++) {
            for (x = 0; x < floorMaxX; x++) {
                d->path[y][x].pos[dimY] = y;
                d->path[y][x].pos[dimX] = x;
            }
        }
        initialized = 1;
    }

    for (y = 0; y < floorMaxY; y++) {
        for (x = 0; x < floorMaxX; x++) {
            d->path[y][x].cost = INT_MAX;
        }
    }

    for (y = 0; y < floorMaxY; y++) {
        for (x = 0; x < floorMaxX; x++) {
            if(d->floor[y][x] == playerChar){
                d->path[y][x].cost = 0;
            }
        }
    }

    heap_init(&h, corridor_path_cmp, NULL); 
    
    if(!strcmp(str, "non-tunneling")){
        for (y = 0; y < floorMaxY; y++) {
            for (x = 0; x < floorMaxX; x++) {
                if (d->floor[y][x] != edgeChar && d->floor[y][x] != rockChar) {
                    d->path[y][x].hn = heap_insert(&h, &d->path[y][x]);
                } else {
                    d->path[y][x].hn = NULL;
                }
            }
        }
    } else if(!strcmp(str, "tunneling")){
        for (y = 0; y < floorMaxY; y++) {
            for (x = 0; x < floorMaxX; x++) {
                if (d->floor[y][x] != edgeChar) {
                    d->path[y][x].hn = heap_insert(&h, &d->path[y][x]);
                } else {
                    d->path[y][x].hn = NULL;
                }
            }
        }
    } else {
        exit(1);
    }

    while ((p = heap_remove_min(&h))) {
        p->hn = NULL;
        // 4 cases for cardinal directions
        if ((d->path[p->pos[dimY] - 1][p->pos[dimX]    ].hn) &&
            (d->path[p->pos[dimY] - 1][p->pos[dimX]    ].cost >
             p->cost + d->hardness[p->pos[dimY]][p->pos[dimX]] / 85)) {
            d->path[p->pos[dimY] - 1][p->pos[dimX]    ].cost = p->cost + d->hardness[p->pos[0]][p->pos[1]] / 85 + 1;
            heap_decrease_key_no_replace(&h, d->path[p->pos[dimY] - 1][p->pos[dimX]    ].hn);
        }
        if ((d->path[p->pos[dimY]    ][p->pos[dimX] - 1].hn) &&
            (d->path[p->pos[dimY]    ][p->pos[dimX] - 1].cost >
             p->cost + d->hardness[p->pos[dimY]][p->pos[dimX]] / 85)) {
            d->path[p->pos[dimY]    ][p->pos[dimX] - 1].cost = p->cost + d->hardness[p->pos[0]][p->pos[1]] / 85 + 1;
            heap_decrease_key_no_replace(&h, d->path[p->pos[dimY]    ][p->pos[dimX] - 1].hn);
        }
        if ((d->path[p->pos[dimY]    ][p->pos[dimX] + 1].hn) &&
            (d->path[p->pos[dimY]    ][p->pos[dimX] + 1].cost >
             p->cost + d->hardness[p->pos[dimY]][p->pos[dimX]] / 85)) {
            d->path[p->pos[dimY]    ][p->pos[dimX] + 1].cost = p->cost + d->hardness[p->pos[0]][p->pos[1]] / 85 + 1;
            heap_decrease_key_no_replace(&h, d->path[p->pos[dimY]    ][p->pos[dimX] + 1].hn);
        }
        if ((d->path[p->pos[dimY] + 1][p->pos[dimX]    ].hn) &&
            (d->path[p->pos[dimY] + 1][p->pos[dimX]    ].cost >
             p->cost + d->hardness[p->pos[dimY]][p->pos[dimX]] / 85)) {
            d->path[p->pos[dimY] + 1][p->pos[dimX]    ].cost = p->cost + d->hardness[p->pos[0]][p->pos[1]] / 85 + 1;
            heap_decrease_key_no_replace(&h, d->path[p->pos[dimY] + 1][p->pos[dimX]    ].hn);
        }
        // 4 more cases for diagonal directions. 
        if ((d->path[p->pos[dimY] - 1][p->pos[dimX] - 1].hn) &&
            (d->path[p->pos[dimY] - 1][p->pos[dimX] - 1].cost >
             p->cost + d->hardness[p->pos[dimY]][p->pos[dimX]] / 85)) {
            d->path[p->pos[dimY] - 1][p->pos[dimX] - 1].cost = p->cost + d->hardness[p->pos[0]][p->pos[1]] / 85 + 1;
            heap_decrease_key_no_replace(&h, d->path[p->pos[dimY] - 1][p->pos[dimX] - 1].hn);
        }
        if ((d->path[p->pos[dimY] - 1][p->pos[dimX] + 1].hn) &&
            (d->path[p->pos[dimY] - 1][p->pos[dimX] + 1].cost >
             p->cost + d->hardness[p->pos[dimY]][p->pos[dimX]] / 85)) {
            d->path[p->pos[dimY] - 1][p->pos[dimX] + 1].cost = p->cost + d->hardness[p->pos[0]][p->pos[1]] / 85 + 1;
            heap_decrease_key_no_replace(&h, d->path[p->pos[dimY] - 1][p->pos[dimX] + 1].hn);
        }
        if ((d->path[p->pos[dimY] + 1][p->pos[dimX] - 1].hn) &&
            (d->path[p->pos[dimY] + 1][p->pos[dimX] - 1].cost >
             p->cost + d->hardness[p->pos[dimY]][p->pos[dimX]] / 85)) {
            d->path[p->pos[dimY] + 1][p->pos[dimX] - 1].cost = p->cost + d->hardness[p->pos[0]][p->pos[1]] / 85 + 1;
            heap_decrease_key_no_replace(&h, d->path[p->pos[dimY] + 1][p->pos[dimX] - 1].hn);
        }
        if ((d->path[p->pos[dimY] + 1][p->pos[dimX] + 1].hn) &&
            (d->path[p->pos[dimY] + 1][p->pos[dimX] + 1].cost >
             p->cost + d->hardness[p->pos[dimY]][p->pos[dimX]] / 85)) {
            d->path[p->pos[dimY] + 1][p->pos[dimX] + 1].cost = p->cost + d->hardness[p->pos[0]][p->pos[1]] / 85 + 1;
            heap_decrease_key_no_replace(&h, d->path[p->pos[dimY] + 1][p->pos[dimX] + 1].hn);
        }
    }
    
    if(!strcmp(str, "non-tunneling")){
        for (y = 0; y < floorMaxY; y++) {
            for (x = 0; x < floorMaxX; x++) {
                if(d->hardness[y][x] == 0) // Room or corridor
                    d->nonTunDist[y][x] = d->path[y][x].cost;
                else 
                    d->nonTunDist[y][x] = 255;
            }
        }
    } else {
        for (y = 0; y < floorMaxY; y++) {
            for (x = 0; x < floorMaxX; x++) {
                d->tunDist[y][x] = d->path[y][x].cost;
            }
        }
    }
    free(p);
    heap_delete(&h);
    //printMap(d);
}