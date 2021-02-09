/**
 * @author Thomas McCoy, Jens Rasmussen, and Jason Guo
 * */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define minRoomNumber 6
#define maxRoomNumber 10
#define maxRoomSize 10
#define minRoomX 4
#define minRoomY 3
#define floorMaxX 80
#define floorMaxY 21
#define edgeChar 'H'
#define roomChar '.'
#define corridorChar '#'
#define rockChar ' '
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
void printGame(struct tiles(*floor)[floorMaxX][floorMaxY]);

int main(int argc, char *argv[])
{
    printf("Error5");
    srand(time(0)); // Testing
    int i, j;
    int roomsWanted = (rand() % ((maxRoomNumber + 1) - minRoomNumber)) + minRoomNumber; // Always min , potential of adding rooms up to max

    struct tiles floor[floorMaxX][floorMaxY];
    struct rooms roomList[maxRoomNumber];
    
    printf("Error4");

    // Top / Bottom
    for(i = 0; i < floorMaxX; i++){
        floor[i][0].type = edgeChar;
        floor[i][0].hardness = 0;
        floor[i][floorMaxY - 1].type = edgeChar;
        floor[i][floorMaxY- 1].hardness = 0;
    }
    
    // Sides 
    for(j = 0; j < floorMaxY; j++){
        floor[0][j].type = edgeChar;
        floor[0][j].hardness = 0;
        floor[floorMaxX - 1][j].type = edgeChar;
        floor[floorMaxX - 1][j].hardness = 0;
    }

    roomGen(&floor, &roomList, roomsWanted);
    printGame(&floor);
    
    /* while(roomCount < roomsWanted){
        roomGen(&floor);
        roomCount++;
    } */
return 0;
}

void roomGen(struct tiles(*floor)[floorMaxX][floorMaxY], struct rooms(*roomList)[maxRoomNumber], int roomsWanted)
{
    srand(time(0)); // Testing
    int i, k, l, failCount;
    bool placed = false;

    while(!placed){
        for(i = 0; i < roomsWanted; i++){
            roomList[i]->sizeX = rand() % (maxRoomSize - 1) + 1;
            roomList[i]->sizeY = rand() % (maxRoomSize - 1) + 1;
            roomList[i]->cornerX = rand() % (floorMaxX - 1) + 1;
            roomList[i]->cornerY = rand() % (floorMaxY - 1) + 1;

            printf("Error1");

            if(floor[roomList[i]->cornerX][roomList[i]->cornerY]->type == rockChar && // Top left
               floor[roomList[i]->cornerX + roomList[i]->sizeX][roomList[i]->cornerY]->type == rockChar && // Top right
               floor[roomList[i]->cornerX + roomList[i]->sizeX][roomList[i]->cornerY + roomList[i]->sizeY]->type == rockChar && // Bottom right
               floor[roomList[i]->cornerX][roomList[i]->cornerY + roomList[i]->sizeY]->type == rockChar){ // Bottom left
                printf("Error2");
                for (k = 0; k < roomList[i]->sizeX; k++) {
                    for (l = 0; l < roomList[i]->sizeY; l++){
                        floor[roomList[i]->cornerX + k][roomList[i]->cornerY + l]->type = roomChar;
                        printf("Error3");
                    }
                }
            }
            else{
                failCount++;
                break;
            }
        }
        placed = true;
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
                default :
                    printf("%c", rockChar); // Maybe change to Error char 'E'
            }
        }
        printf("\n");
    }
}