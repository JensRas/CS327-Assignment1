#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <ncurses.h>

#include "printers.h"
#include "dungeon.h"
#include "pc.h"
#include "npc.h"

/*****************************************
 *            Game Printer               *
 *****************************************/
void printGame(dungeon *d)
{
    int y, x;
    for(y = 0; y < floorMaxY; y++){ 
        for(x = 0; x < floorMaxX; x++){
            // if fogOfWar
            mvprintw(1 + y, x, "%c", d->fogMap[y][x]);
            // else 
            /* if (d->charMap[y][x].isAlive && !d->charMap[y][x].isPC) {
                mvprintw(1 + y, x, "%x", d->charMap[y][x].entity.nonPlayer.type);
            } else if (d->charMap[y][x].isPC && d->charMap[y][x].isAlive) {
                mvprintw(1 + y, x, "%c", playerChar);
            } else {
                mvprintw(1 + y, x, "%c", d->floor[y][x]);
            } */
        }
    }
    refresh();
}

/*****************************************
 *             Map Printer               *
 *****************************************/
void printMap(dungeon *d) 
{
    // Needs updating to ncurses
    int i, j;
    
    for(i = 0; i < floorMaxY; i++){
        for(j = 0; j < floorMaxX; j++){
            if(d->path[i][j].cost < 1000 && d->floor[i][j] != playerChar){ // Distance from Player
                printf("%d", d->path[i][j].cost % 10);
            } else if(d->floor[i][j] == edgeChar) { // Border
                printf("%c", edgeChar);
            } else if(d->floor[i][j] == playerChar) { // Player
                printf("%c", playerChar);
            } else { 
                printf("%c", rockChar); // Rocks
            }
        }
        printf("\n");
    }
    printf("\n");
}

/*****************************************
 *          nCurses Generator            *
 *****************************************/
void terminalInit() 
{
    initscr();
    raw();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
}