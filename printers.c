#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
            if (d->charMap[y][x].isAlive && !d->charMap[y][x].isPC) {
                //printf("%x", d->charMap[y][x].entity.nonPlayer.type);
                mvprintw(1 + y, x, "%x", d->charMap[y][x].entity.nonPlayer.type);
            } else if (d->charMap[y][x].isPC && d->charMap[y][x].isAlive) {
                mvprintw(1 + y, x, "%c", playerChar);
                //printf("%c", playerChar);
            } else {
                //printf("%c", d->floor[y][x]);
                mvprintw(1 + y, x, "%c", d->floor[y][x]);
            }
        }
        //printf("\n");
    }
    //printf("\n");
    refresh();
}
/*****************************************
 *             Map Printer               *
 *****************************************/
void printMap(dungeon *d) 
{
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