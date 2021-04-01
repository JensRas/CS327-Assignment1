#include <cstdlib>
#include <ncurses.h>
#include <string>
#include <iostream>
#include <fstream>

#include "dungeon.h"
#include "npc.h"
#include "pc.h"
#include "parsers.h"

/*****************************************
 *          Monster Generator            *
 *****************************************/
void monsterGen(dungeon *d) {
    int i, j, ranY, ranX, ran;
    bool placed;
    
    for (i = 0; i < d->numMon; i++) {
        ran = rand() % (d->numRooms - 1); // Room Monster is placed in (Not player's)
        placed = false;
        int roomSpace = 0;
        for(j = 0; j < d->numRooms - 1; j++){ 
            roomSpace += d->roomList[j].sizeY * d->roomList[j].sizeX;
        }
        roomSpace -= d->numUStairs - d->numDStairs - 1 - i; // Total floor space in all rooms except where player is at (i is # monsters placed)

        while(!placed && roomSpace > 0){

            ranY = d->roomList[ran].cornerY + (rand() % d->roomList[ran].sizeY);
            ranX = d->roomList[ran].cornerX + (rand() % d->roomList[ran].sizeX);

            if(d->floor[ranY][ranX] == roomChar){
                placed = true;
                roomSpace--;
            }
        }

        d->charMap[ranY][ranX].x = ranX;
        d->charMap[ranY][ranX].y = ranY;
        d->charMap[ranY][ranX].entity.nonPlayer.type = rand() & 0xf;
        d->charMap[ranY][ranX].isPC = 0;
        d->charMap[ranY][ranX].isAlive = 1;
        d->charMap[ranY][ranX].speed = (rand() % 16) + 5;
        d->charMap[ranY][ranX].nTurn = 0;
        d->charMap[ranY][ranX].sequenceNum = i + 1;
        d->charMap[ranY][ranX].entity.nonPlayer.knownPCX = 0;
        d->charMap[ranY][ranX].entity.nonPlayer.knownPCY = 0;
        d->charMap[ranY][ranX].entity.nonPlayer.adj = nameMonst();
    }
}
/*****************************************
 *             Monster Namer             *
 *****************************************/
char *nameMonst()
{
    int s1;
    s1 = rand() % 16;

    switch(s1){
        case 0:
            return (char*) "deadly";
        case 1: 
            return (char*) "forsaken";
        case 2:
            return (char*) "lovable";
        case 3:        
            return (char*) "puny";
        case 4:
            return (char*) "ridiculous";
        case 5:
            return (char*) "shocking";
        case 6:     
            return (char*) "shy";
        case 7:      
            return (char*) "almighty";
        case 8:            
            return (char*) "brash";
        case 9:            
            return (char*) "bored";
        case 10:             
            return (char*) "aggressive";
        case 11:             
            return (char*) "charming";
        case 12:            
            return (char*) "jaded";
        case 13:             
            return (char*) "furious";
        case 14:             
            return (char*) "weak";
        case 15:            
            return (char*) "clever";
        default:
            return (char*) "error";
    }
}
/*****************************************
 *           Monster Attacker            *
 *****************************************/
void monsAttk(dungeon *d)
{

}

/*****************************************
 *            Monster Mover              *
 *****************************************/
character *moveMonst(dungeon *d, int newY, int newX, character *npc)
{
    int oldY, oldX;

    oldY = npc->y;
    oldX = npc->x;
    npc->y = newY;
    npc->x = newX;
    d->charMap[newY][newX] = *npc;
    d->charMap[oldY][oldX].y = oldY;
    d->charMap[oldY][oldX].x = oldX;
    d->charMap[oldY][oldX].speed = 0;
    d->charMap[oldY][oldX].isPC = 0;
    d->charMap[oldY][oldX].nTurn = 0;
    d->charMap[oldY][oldX].isAlive = 0;
    d->charMap[oldY][oldX].sequenceNum = 0;
    d->charMap[oldY][oldX].entity.nonPlayer.type = 0;
    return npc;
}
/*****************************************
 *             Monster List              *
 *****************************************/
void makeMonstList(dungeon *d)
{
    WINDOW *myWin;
    int startY, startX, width, height, ch, i, y, x, offset, yDiff, xDiff;
    int numAlive = 0;
    for(y = 0; y < floorMaxY; y++){
        for(x = 0; x < floorMaxX; x++){
            if(d->charMap[y][x].isAlive && !d->charMap[y][x].isPC)
                numAlive++;
        }
    }
    char **list = (char**)malloc(numAlive * sizeof(char *));
    height = 19;
    width = 40;
    startY = 2;
    startX = 21;
    i = 0, offset = 0, yDiff = 0, xDiff = 0;
    char *nors;
    char *eorw;
    character *pc = findPC(d);

    for(y = 0; y < floorMaxY; y++){
        for(x = 0; x < floorMaxX; x++){
            if(d->charMap[y][x].isAlive && !d->charMap[y][x].isPC){
                yDiff = (pc->y - d->charMap[y][x].y > 0) ?  pc->y - d->charMap[y][x].y : d->charMap[y][x].y - pc->y;
                xDiff = (pc->x - d->charMap[y][x].x > 0) ?  pc->x - d->charMap[y][x].x : d->charMap[y][x].x - pc->x;
                nors = (pc->y - d->charMap[y][x].y > 0) ? (char*) "North" : (char*) "South";
                eorw = (pc->x - d->charMap[y][x].x > 0) ? (char*) "West" : (char*) "East";
                list[i] = (char*) malloc(15 + 2 + 2 + 5 + 4 + 11 + 1); //yDiff: 2, xDiff: 2, nors: 5, eorw: 4, spaces: 11, null term 1 
                sprintf(list[i], "A %-11s %x: %2d %s by %2d %s", d->charMap[y][x].entity.nonPlayer.adj, d->charMap[y][x].entity.nonPlayer.type, yDiff, nors, xDiff, eorw);
                i++;
            } 
        }
    }

    myWin = newwin(height, width, startY, startX);
    keypad(myWin, TRUE);
    scrollok(myWin, TRUE);
    
    box(myWin, 0, 0);
    wrefresh(myWin);

    for(i = 0; i < 15; i++) {
        mvwprintw(myWin, i + 2, 1, "%s", list[i]);
    }
    mvwprintw(myWin, 1, 1, "Press Escape to exit.");
    mvwprintw(myWin, 17, 1, "Arrows to Scroll. ESC to continue.");
    wrefresh(myWin);

    while((ch = wgetch(myWin)) != 27) {
        switch (ch) {
            case KEY_DOWN:
                if(offset + 15 < d->numMon){
                    offset += 15;
                    wclear(myWin);
                    box(myWin, 0, 0);
                    for(i = offset; i < offset + 15; i++) {
                        if (i < numAlive) {
                            mvwprintw(myWin, i - offset + 2, 1, "%s", list[i]);
                        }
                    }
                    mvwprintw(myWin, 1, 1, "Press Escape to exit.");
                    mvwprintw(myWin, 17, 1, "Arrows to Scroll. ESC to continue.");
                }             
                wrefresh(myWin);
                break; 
            case KEY_UP:
                if(offset - 15 >= 0){
                    offset -= 15;
                    wclear(myWin);
                    box(myWin, 0, 0);
                    for(i = offset; i < offset + 15; i++) {
                        if (i < numAlive) {
                            mvwprintw(myWin, i - offset + 2, 1, "%s", list[i]);
                        }
                    }
                    mvwprintw(myWin, 1, 1, "Press Escape to exit.");
                    mvwprintw(myWin, 17, 1, "Arrows to Scroll. ESC to continue.");
                }
                wrefresh(myWin); 
                break;
        }    
    }
    delwin(myWin);
    free(list);
}