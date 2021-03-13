#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <endian.h>
#include <limits.h>

#include "dungeon.h"
#include "heap.h"
#include "npc.h"
#include "pc.h"
#include "pathfinder.h"

/*****************************************
 *           Border Generator            *
 *****************************************/ 
void borderGen(dungeon *d)
{
    int i, j;
    for(i = 0; i < floorMaxY; i++){ // Sides
        d->floor[i][0] = edgeChar;
        d->hardness[i][0] = 255;
        d->floor[i][floorMaxX - 1] = edgeChar;
        d->hardness[i][floorMaxX - 1] = 255;
    }
    for(j = 0; j < floorMaxX; j++){ // Top/Bottom
        d->floor[0][j] = edgeChar;
        d->hardness[0][j] = 255;
        d->floor[floorMaxY - 1][j] = edgeChar;
        d->hardness[floorMaxY - 1][j] = 255;
    }
}
/*****************************************
 *           Room Generator              *
 *****************************************/
void roomGen(dungeon *d)
{
    int i, j, k, failCount = 0; // For testing
    bool placedAll = false;

    while(!placedAll){
        placedAll = true;
        // Set everything (not border) to rock
        for(i = 1; i < floorMaxY - 1; i++) { 
            for (j = 1; j < floorMaxX - 1; j++) {
                d->floor[i][j] = rockChar;
                d->hardness[i][j] = 100;
            }
        }
        
        for(i = 0; i < d->numRooms; i++){
            d->roomList[i].sizeX = rand() % (maxRoomSize - minRoomX) + minRoomX;
            d->roomList[i].sizeY = rand() % (maxRoomSize - minRoomY) + minRoomY;
            d->roomList[i].cornerX = rand() % (floorMaxX - d->roomList[i].sizeX - 2);
            d->roomList[i].cornerY = rand() % (floorMaxY - d->roomList[i].sizeY - 2);

            for (j = -1; j < d->roomList[i].sizeY + 1; j++) {
                for (k = -1; k < d->roomList[i].sizeX + 1; k++) {
                    if(d->floor[d->roomList[i].cornerY + j][d->roomList[i].cornerX + k] == roomChar || 
                        d->floor[d->roomList[i].cornerY + j][d->roomList[i].cornerX + k] == edgeChar){
                        placedAll = false;
                    }
                }
            } 
            if(placedAll){
                for (j = 0; j < d->roomList[i].sizeY; j++) {
                    for (k = 0; k < d->roomList[i].sizeX; k++) {
                        d->floor[d->roomList[i].cornerY + j][d->roomList[i].cornerX + k] = roomChar;
                        d->hardness[d->roomList[i].cornerY + j][d->roomList[i].cornerX + k] = 0;
                    }
                }
            }
        }
        failCount++;
    }
    //printf("Failed: %d\n", failCount);
}
/*****************************************
 *         Corridor Generator            *
 *****************************************/
void corridorGen(dungeon *d) 
{
    int i, j, k, ranX, ranY, ranX2, ranY2;

    for (i = 0; i < d->numRooms; i++) {
        if(i < d->numRooms - 1) {
            ranX = d->roomList[i].cornerX + (rand() % d->roomList[i].sizeX);
            ranY = d->roomList[i].cornerY + (rand() % d->roomList[i].sizeY);
            ranX2 = d->roomList[i + 1].cornerX + (rand() % d->roomList[i + 1].sizeX);
            ranY2 = d->roomList[i + 1].cornerY + (rand() % d->roomList[i + 1].sizeY);
        }else { 
            ranX = d->roomList[i].cornerX + (rand() % d->roomList[i].sizeX);
            ranY = d->roomList[i].cornerY + (rand() % d->roomList[i].sizeY);
            ranX2 = d->roomList[0].cornerX + (rand() % d->roomList[0].sizeX);
            ranY2 = d->roomList[0].cornerY + (rand() % d->roomList[0].sizeY);
        }
        int l = 0;
        for(j = 0; j < abs(ranY - ranY2); j++){
            if(ranY < ranY2){
                l++;
                if(d->floor[ranY + j][ranX] != roomChar){
                    d->floor[ranY + j][ranX] = corridorChar;
                    d->hardness[ranY + j][ranX] = 0;
                }
            }
            else{
                l--;
                if(d->floor[ranY - j][ranX] != roomChar){
                    d->floor[ranY - j][ranX] = corridorChar;
                    d->hardness[ranY - j][ranX] = 0;
                }
            }
        }

        for(k = 0; k < abs(ranX - ranX2); k++){
            if(ranX < ranX2){
                if(d->floor[ranY + l][ranX + k] != roomChar){
                    d->floor[ranY + l][ranX + k] = corridorChar;
                    d->hardness[ranY + l][ranX + k] = 0;
                }
            }
            else {
                if(d->floor[ranY + l][ranX - k] != roomChar){
                    d->floor[ranY + l][ranX - k] = corridorChar;
                    d->hardness[ranY + l][ranX - k] = 0;
                }
            }
        }
    }
}
/*****************************************
 *         Staircase Generator           *
 *****************************************/
void staircaseGen(dungeon *d) 
{
    int i, ranX, ranY, ran;
    // Up staircases
    for(i = 0; i < d->numUStairs; i++){
        ran = rand() % d->numRooms;
        
        ranY = d->roomList[ran].cornerY + (rand() % d->roomList[ran].sizeY);
        ranX = d->roomList[ran].cornerX + (rand() % d->roomList[ran].sizeX);

        d->stairListU[i].y = ranY;
        d->stairListU[i].x = ranX;

        d->floor[ranY][ranX] = upChar;
    }
    // Down staircases
    for(i = 0; i < d->numDStairs; i++){
        ran = rand() % d->numRooms;
        
        ranY = d->roomList[ran].cornerY + (rand() % d->roomList[ran].sizeY);
        ranX = d->roomList[ran].cornerX + (rand() % d->roomList[ran].sizeX);

        d->stairListD[i].y = ranY;
        d->stairListD[i].x = ranX;

        d->floor[ranY][ranX] = downChar;
    }
}
/*****************************************
 *           Dungeon Deletor             *
 *****************************************/
void dungeonDelete(dungeon *d)
{
    free(d->roomList);
    free(d->stairListU);
    free(d->stairListD);
}
/*****************************************
 *            Game Generator             *
 *****************************************/
void gameGen(dungeon *d)
{
    // Finding number of rooms and allocating memory
    d->numRooms = (rand() % ((maxRoomNumber + 1) - minRoomNumber)) + minRoomNumber;
    d->roomList = calloc(d->numRooms , sizeof(room));
    // Finding number of up stairs and allocating memory
    d->numUStairs = ((rand() % d->numRooms) / 3);
    d->numUStairs = (d->numUStairs < 1) ? 1 : d->numUStairs;
    d->stairListU = calloc(d->numUStairs , sizeof(stair));
    // Finding number of down stairs and allocating memory
    d->numDStairs = ((rand() % d->numRooms) / 3);
    d->numDStairs = (d->numDStairs < 1) ? 1 : d->numDStairs;
    d->stairListD = calloc(d->numDStairs , sizeof(stair));

    borderGen(d);
    roomGen(d); 
    corridorGen(d); 
    staircaseGen(d);
    playerGen(d);
    dijkstra(d, "non-tunneling");
    dijkstra(d, "tunneling");
    monsterGen(d);
    terminalInit();
    printGame(d);
    refresh();
}