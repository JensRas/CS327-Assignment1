/**
 * @author Thomas McCoy, Jens Rasmussen, and Jason Guo
 * */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define minRoomNumber 6
#define maxRoomNumber 10
#define maxRoomSize 10
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


// At least one cell of non-room 1 cell of non-room between any two different rooms

// 0 = Immutable
// 1 = Soft :)

struct tiles {
    int hardness;
    char type;
};

struct rooms {
    int cornerX; // Top left
    int cornerY; // Top left
    int sizeX;
    int sizeY; 
};

void roomGen(struct tiles(*floor)[floorMaxX][floorMaxY], struct rooms(*roomList)[maxRoomNumber], int roomsWanted);
void staircaseGen(struct tiles(*floor)[floorMaxX][floorMaxY], struct rooms(*roomList)[maxRoomNumber], int roomsWanted);
void corridorGen(struct tiles(*floor)[floorMaxX][floorMaxY], struct rooms(*roomList)[maxRoomNumber], int roomsWanted);
void printGame(struct tiles(*floor)[floorMaxX][floorMaxY]);
void placeBorder(struct tiles(*floor)[floorMaxX][floorMaxY]);

int main(int argc, char *argv[])
{
    srand(time(0)); // Random
    int roomsWanted = (rand() % ((maxRoomNumber + 1) - minRoomNumber)) + minRoomNumber; // Always min , potential of adding rooms up to max  

    struct tiles floor[floorMaxX][floorMaxY];
    struct rooms roomList[roomsWanted];

    placeBorder(&floor);
    roomGen(&floor, &roomList, roomsWanted);
    corridorGen(&floor, &roomList, roomsWanted);
    staircaseGen(&floor, &roomList, roomsWanted);
    
    // Bugs with rooms only on the left third of the floor having pieces taken out around specific areas of the floor
    // Bugs with border having pieces of the left third of the floor
    //placeBorder(&floor); // Covers up border problems
    printGame(&floor);
    return 0;
}

void roomGen(struct tiles(*floor)[floorMaxX][floorMaxY], struct rooms(*roomList)[maxRoomNumber], int roomsWanted)
{
    //srand(time(0)); // Random
    int i, j, k, failCount = 0;
    bool placedAll = false;

    while(!placedAll){
        placedAll = true;
        for(i = 1; i < floorMaxX - 1; i++) { // Set everything to rock
            for (j = 1; j < floorMaxY - 1; j++) {
                (*floor)[i][j].type = rockChar;
            }
        }

        for(i = 0; i < roomsWanted; i++){
            roomList[i]->sizeX = rand() % (maxRoomSize - minRoomX) + minRoomX;
            roomList[i]->sizeY = rand() % (maxRoomSize - minRoomY) + minRoomY;
            roomList[i]->cornerX = rand() % (floorMaxX - roomList[i]->sizeX - 2);
            roomList[i]->cornerY = rand() % (floorMaxY - roomList[i]->sizeY - 2);
            
            for (j = -1; j < roomList[i]->sizeX + 1; j++) {
                for (k = -1; k < roomList[i]->sizeY + 1; k++) {
                    if((*floor)[roomList[i]->cornerX + j][roomList[i]->cornerY + k].type == roomChar || 
                        (*floor)[roomList[i]->cornerX + j][roomList[i]->cornerY + k].type == edgeChar){
                        failCount++;
                        placedAll = false;
                    }
                }
            } 
            if(placedAll){
                for (j = 0; j < roomList[i]->sizeX; j++) {
                    for (k = 0; k < roomList[i]->sizeY; k++) {
                        (*floor)[roomList[i]->cornerX + j][roomList[i]->cornerY + k].type = roomChar;
                    }
                }
            }
        }
    }
    //printf("%d\n", failCount);
}

void staircaseGen(struct tiles(*floor)[floorMaxX][floorMaxY], struct rooms(*roomList)[maxRoomNumber], int roomsWanted) {
    int i, ranX = 0, ranY = 0, ranX2 = 0, ranY2 = 0, ranDown, ranUp;
    
    int numStairs = ((rand() % roomsWanted) / 3);
    if(numStairs < 1 ){
        numStairs = 1;
    }

    for(i = 0; i < numStairs; i++){
        ranX = 0, ranY = 0, ranX2 = 0, ranY2 = 0, ranDown = 0, ranUp = 0;
        
        ranDown = rand() % roomsWanted;
        ranUp = rand() % roomsWanted;
        
        while(ranX == ranX2 && ranY == ranY2) {
            ranX = roomList[ranDown]->cornerX + (rand() % roomList[ranDown]->sizeX);
            ranY = roomList[ranDown]->cornerY + (rand() % roomList[ranDown]->sizeY);
            ranX2 = roomList[ranUp]->cornerX + (rand() % roomList[ranUp]->sizeX);
            ranY2 = roomList[ranUp]->cornerY + (rand() % roomList[ranUp]->sizeY);
        }

        (*floor)[ranX][ranY].type = downChar;
        (*floor)[ranX2][ranY2].type = upChar;
    }
}

void corridorGen(struct tiles(*floor)[floorMaxX][floorMaxY], struct rooms(*roomList)[maxRoomNumber], int roomsWanted) 
{
    int i, j, k, ranX, ranY, ranX2, ranY2;
    //roomsWanted
    for (i = 0; i < roomsWanted; i++) {
        if(i < roomsWanted - 1) {
            ranX = roomList[i]->cornerX + (rand() % roomList[i]->sizeX);
            ranY = roomList[i]->cornerY + (rand() % roomList[i]->sizeY);
            ranX2 = roomList[i + 1]->cornerX + (rand() % roomList[i + 1]->sizeX);
            ranY2 = roomList[i + 1]->cornerY + (rand() % roomList[i + 1]->sizeY);
        }else { 
            ranX = roomList[i]->cornerX + (rand() % roomList[i]->sizeX);
            ranY = roomList[i]->cornerY + (rand() % roomList[i]->sizeY);
            ranX2 = roomList[0]->cornerX + (rand() % roomList[0]->sizeX);
            ranY2 = roomList[0]->cornerY + (rand() % roomList[0]->sizeY);
        }

        int l = 0;
        for(j = 0; j < abs(ranX - ranX2); j++){
            if(ranX < ranX2){
                //(*floor)[ranX + j][ranY].type = corridorChar;
                l++;
                if((*floor)[ranX + j][ranY].type != roomChar){
                    (*floor)[ranX + j][ranY].type = corridorChar;
                }
            }
            else{
                //(*floor)[ranX - j][ranY].type = corridorChar;
                l--;
                if((*floor)[ranX - j][ranY].type != roomChar){
                    (*floor)[ranX - j][ranY].type = corridorChar;
                }
            }
        }

        for(k = 0; k < abs(ranY - ranY2); k++){
            if(ranY < ranY2){
                //(*floor)[ranX + l][ranY + k].type = corridorChar;
                if((*floor)[ranX + l][ranY + k].type != roomChar){
                    (*floor)[ranX + l][ranY + k].type = corridorChar;
                }
            }
            else {
                //(*floor)[ranX + l][ranY - k].type = corridorChar;
                if((*floor)[ranX + l][ranY - k].type != roomChar){
                    (*floor)[ranX + l][ranY - k].type = corridorChar;
                }
            }
        }

        /////////////////////////
        //Debug Version
        /////////////////////////
        //Debug Version
        /////////////////////////
        /* for(j = 0; j < abs(ranX - ranX2); j++){
            if(ranX - ranX2 > 0){
                    (*floor)[ranX + j][ranY].type = corridorChar;
                    l++;
            }
            else{
                    (*floor)[ranX - j][ranY].type = corridorChar;
                    l--;
            }
        }
        
        for(k = 0; k < abs(ranY - ranY2); k++){
            if(ranY - ranY2 > 0){
                    (*floor)[ranX + l][ranY + k].type = corridorChar;
            }
            else {
                    (*floor)[ranX + l][ranY - k].type = corridorChar;
            }
        }  */
    }
}

void placeBorder(struct tiles(*floor)[floorMaxX][floorMaxY])
{
    int i, j;
    for(i = 0; i < floorMaxX; i++){ // Top
        (*floor)[i][0].type = edgeChar;
        (*floor)[i][0].hardness = 0;
        (*floor)[i][floorMaxY - 1].type = edgeChar;
        (*floor)[i][floorMaxY- 1].hardness = 0;
    }
    for(j = 0; j < floorMaxY; j++){ // Sides
        (*floor)[0][j].type = edgeChar;
        (*floor)[0][j].hardness = 0;
        (*floor)[floorMaxX - 1][j].type = edgeChar;
        (*floor)[floorMaxX - 1][j].hardness = 0;
    }
}

void printGame(struct tiles(*floor)[floorMaxX][floorMaxY])
{
    int i, j;
    for(i = 0; i < floorMaxY; i++){
        for(j = 0; j < floorMaxX; j++){
            switch((*floor)[j][i].type) {
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