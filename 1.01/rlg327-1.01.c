/********************************************************
 * @authors Thomas McCoy, Jens Rasmussen, and Jason Guo *
 ********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define minRoomNumber 6
#define maxRoomNumber 10
#define maxRoomSize 10 // Arbitrary value that can be changed
#define minRoomX 4
#define minRoomY 3
#define floorMaxX 80
#define floorMaxY 21
#define edgeChar '#'
#define roomChar ' '
#define corridorChar 'o'
#define rockChar '.'
#define upChar '<'
#define downChar '>'

struct tiles {
    int hardness; // Needs to be implemented
    char type;
};

struct rooms {
    int cornerX; // Top left
    int cornerY; // Top left
    int sizeX;
    int sizeY; 
};

/*****************************************
 *             Prototypes                *
 *****************************************/

void corridorGen(struct tiles floor[floorMaxY][floorMaxX], struct rooms roomList[maxRoomNumber], int roomsWanted);
void placeBorder(struct tiles floor[floorMaxY][floorMaxX]);
void roomGen(struct tiles floor[floorMaxY][floorMaxX], struct rooms roomList[maxRoomNumber], int roomsWanted);
void staircaseGen(struct tiles floor[floorMaxY][floorMaxX], struct rooms roomList[maxRoomNumber], int roomsWanted);
void printGame(struct tiles floor[floorMaxY][floorMaxX]);

/*****************************************
 *                Main                   *
 *****************************************/
int main(int argc, char *argv[])
{
    srand(time(NULL)); // Seed / Random
    int roomsWanted = (rand() % ((maxRoomNumber + 1) - minRoomNumber)) + minRoomNumber; // Always min , potential of adding rooms up to max  

    struct tiles floor[floorMaxY][floorMaxX];
    struct rooms roomList[roomsWanted];

    
    placeBorder(floor);
    roomGen(floor, roomList, roomsWanted); 
    corridorGen(floor, roomList, roomsWanted); 
    staircaseGen(floor, roomList, roomsWanted);
    printGame(floor);
    
    return 0;
}

/*****************************************
 *           Border Generator            *
 *****************************************/ 
void placeBorder(struct tiles floor[floorMaxY][floorMaxX])
{
    int i, j;
    for(i = 0; i < floorMaxY; i++){ // Top
        floor[i][0].type = edgeChar;
        floor[i][0].hardness = 0;
        floor[i][floorMaxX - 1].type = edgeChar;
        floor[i][floorMaxX- 1].hardness = 0;
    }
    for(j = 0; j < floorMaxX; j++){ // Sides
        floor[0][j].type = edgeChar;
        floor[0][j].hardness = 0;
        floor[floorMaxY - 1][j].type = edgeChar;
        floor[floorMaxY - 1][j].hardness = 0;
    }
}

/*****************************************
 *         Corridor Generator            *
 *****************************************/
void corridorGen(struct tiles floor[floorMaxY][floorMaxX], struct rooms roomList[maxRoomNumber], int roomsWanted) 
{
    int i, j, k, ranX, ranY, ranX2, ranY2;

    for (i = 0; i < roomsWanted; i++) {
        if(i < roomsWanted - 1) {
            ranX = roomList[i].cornerX + (rand() % roomList[i].sizeX);
            ranY = roomList[i].cornerY + (rand() % roomList[i].sizeY);
            ranX2 = roomList[i + 1].cornerX + (rand() % roomList[i + 1].sizeX);
            ranY2 = roomList[i + 1].cornerY + (rand() % roomList[i + 1].sizeY);
        }else { 
            ranX = roomList[i].cornerX + (rand() % roomList[i].sizeX);
            ranY = roomList[i].cornerY + (rand() % roomList[i].sizeY);
            ranX2 = roomList[0].cornerX + (rand() % roomList[0].sizeX);
            ranY2 = roomList[0].cornerY + (rand() % roomList[0].sizeY);
        }
        int l = 0;
        for(j = 0; j < abs(ranY - ranY2); j++){
            if(ranY < ranY2){
                l++;
                if(floor[ranY + j][ranX].type != roomChar){
                    floor[ranY + j][ranX].type = corridorChar;
                }
            }
            else{
                l--;
                if(floor[ranY - j][ranX].type != roomChar){
                    floor[ranY - j][ranX].type = corridorChar;
                }
            }
        }

        for(k = 0; k < abs(ranX - ranX2); k++){
            if(ranX < ranX2){
                if(floor[ranY + l][ranX + k].type != roomChar){
                    floor[ranY + l][ranX + k].type = corridorChar;
                }
            }
            else {
                if(floor[ranY + l][ranX - k].type != roomChar){
                    floor[ranY + l][ranX - k].type = corridorChar;
                }
            }
        }
    }
}

/*****************************************
 *           Room Generator              *
 *****************************************/
void roomGen(struct tiles floor[floorMaxY][floorMaxX], struct rooms roomList[maxRoomNumber], int roomsWanted)
{
    int i, j, k, failCount = 0;
    bool placedAll = false;

    while(!placedAll){
        placedAll = true;
        // Set everything (not border) to rock
        for(i = 1; i < floorMaxY - 1; i++) { 
            for (j = 1; j < floorMaxX - 1; j++) {
                floor[i][j].type = rockChar;
            }
        }
        
        for(i = 0; i < roomsWanted; i++){
            roomList[i].sizeX = rand() % (maxRoomSize - minRoomX) + minRoomX;
            roomList[i].sizeY = rand() % (maxRoomSize - minRoomY) + minRoomY;
            roomList[i].cornerX = rand() % (floorMaxX - roomList[i].sizeX - 2);
            roomList[i].cornerY = rand() % (floorMaxY - roomList[i].sizeY - 2);

            for (j = -1; j < roomList[i].sizeY + 1; j++) {
                for (k = -1; k < roomList[i].sizeX + 1; k++) {
                    if(floor[roomList[i].cornerY + j][roomList[i].cornerX + k].type == roomChar || 
                        floor[roomList[i].cornerY + j][roomList[i].cornerX + k].type == edgeChar){
                        placedAll = false;
                    }
                }
            } 
            if(placedAll){
                for (j = 0; j < roomList[i].sizeY; j++) {
                    for (k = 0; k < roomList[i].sizeX; k++) {
                        floor[roomList[i].cornerY + j][roomList[i].cornerX + k].type = roomChar;
                    }
                }
            }
        }
        failCount++;
    }
    //printf("Failed: %d\n", failCount);
}

/*****************************************
 *         Staircase Generator           *
 *****************************************/
void staircaseGen(struct tiles floor[floorMaxY][floorMaxX], struct rooms roomList[maxRoomNumber], int roomsWanted) {
    int i, ranX, ranY, ranX2, ranY2, ranDown, ranUp;
    
    int numStairs = ((rand() % roomsWanted) / 3);
    if(numStairs < 1 ){
        numStairs = 1;
    }

    for(i = 0; i < numStairs; i++){
        ranX = 0, ranY = 0, ranX2 = 0, ranY2 = 0, ranDown = 0, ranUp = 0;
        
        ranDown = rand() % roomsWanted;
        ranUp = rand() % roomsWanted;
        
        while(ranX == ranX2 && ranY == ranY2) {
            ranX = roomList[ranDown].cornerX + (rand() % roomList[ranDown].sizeX);
            ranY = roomList[ranDown].cornerY + (rand() % roomList[ranDown].sizeY);
            ranX2 = roomList[ranUp].cornerX + (rand() % roomList[ranUp].sizeX);
            ranY2 = roomList[ranUp].cornerY + (rand() % roomList[ranUp].sizeY);
        }

        floor[ranY][ranX].type = downChar;
        floor[ranY2][ranX2].type = upChar;
    }
}

/*****************************************
 *            Game Printer               *
 *****************************************/
void printGame(struct tiles floor[floorMaxY][floorMaxX])
{
    int i, j;
    for(i = 0; i < floorMaxY; i++){ 
        for(j = 0; j < floorMaxX; j++){
            switch(floor[i][j].type) {
                case edgeChar :
                    printf("%c", edgeChar);
                    break;
                case roomChar :
                    printf("%c", roomChar);
                    break;
                case corridorChar :
                    printf("%c", corridorChar);
                    break;
                case upChar :
                    printf("%c", upChar);
                    break;
                case downChar :
                    printf("%c", downChar);
                    break;
                case rockChar :
                    printf("%c", rockChar);
                    break;
                default :
                    printf("E"); // Error
            }
        }
        printf("\n");
    }
}