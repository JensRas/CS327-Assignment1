#include <stdlib.h>

#include "dungeon.h"
#include "pc.h"

/*****************************************
 *           Player Generator            *
 *****************************************/
void playerGen(dungeon *d) 
{
    int ranY, ranX, room, x , y;
    bool placed = false;

    for(y = 0; y < floorMaxY; y++){
        for(x = 0; x < floorMaxX; x++){
            d->charMap[y][x].y = y;
            d->charMap[y][x].x = x;
            d->charMap[y][x].isPC = 0;
            d->charMap[y][x].isAlive = 0;
            d->charMap[y][x].speed = 0;
            d->charMap[y][x].sequenceNum = 0;
            d->charMap[y][x].nTurn = 0;
        }
    }

    while(!placed){
        room = d->numRooms - 1;
        ranY = d->roomList[room].cornerY + (rand() % d->roomList[room].sizeY);
        ranX = d->roomList[room].cornerX + (rand() % d->roomList[room].sizeX);
        
        if(d->floor[ranY][ranX] != upChar && d->floor[ranY][ranX] != downChar) {
            d->charMap[ranY][ranX].y = ranY;
            d->charMap[ranY][ranX].x = ranX;
            d->charMap[ranY][ranX].isPC = 1;
            d->charMap[ranY][ranX].isAlive = 1;
            d->charMap[ranY][ranX].speed = 10;
            d->charMap[ranY][ranX].sequenceNum = 0;
            placed = true;
        }
    }
}
/*****************************************
 *             Player Mover              *
 *****************************************/
void movePC(dungeon *d, int yOff, int xOff)
{
    int y, x, oldY, oldX, tempY, tempX;
    character *pc; 

    for(y = 0; y < floorMaxY; y++){
        for(x = 0; x < floorMaxX; x++){
            if(d->charMap[y][x].isPC)
                pc = &d->charMap[y][x];
        }
    }
    
    tempY = yOff + pc->y;
    tempX = xOff + pc->x;
    if(d->floor[tempY][tempX] == rockChar)
        return;
    oldY = pc->y;
    oldX = pc->x;
    pc->y = tempY;
    pc->x = tempX;
    d->charMap[tempY][tempX] = *pc;
    d->charMap[oldY][oldX].y = oldY;
    d->charMap[oldY][oldX].x = oldX;
    d->charMap[oldY][oldX].speed = 0;
    d->charMap[oldY][oldX].nTurn = 0;
    d->charMap[oldY][oldX].isPC = 0;
    d->charMap[oldY][oldX].isAlive = 0;
    d->charMap[oldY][oldX].sequenceNum = 0;
    d->charMap[oldY][oldX].entity.nonPlayer.type = 0;
}
/*****************************************
 *            Find the Player            *
 *****************************************/
character *findPC(dungeon *d)
{
    int x, y;
    for (y = 0; y < floorMaxY; y++) {
        for (x = 0; x < floorMaxX; x++) {
            if (d->charMap[y][x].isPC) {
                return &d->charMap[y][x];
            }
        }
    }
    return NULL;
}