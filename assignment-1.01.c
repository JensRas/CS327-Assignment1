/**
 * @author Thomas McCoy, Jens Rasmussen, and Jason Guo
 * */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define minRoomNumber 6
#define maxRoomNumber 10
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
    int cornerX;
    int cornerY;
    int sizeX;
    int sizeY; 
};

void roomGen(struct tiles(*floor)[80][21]);
void printGame(struct tiles(*floor)[80][21]);

int main(int argc, char *argv[])
{
    //srand(); // Testing
    int i, j;
    int roomCount = 0;
    int roomsWanted = rand()%4 + minRoomNumber;

    struct tiles floor[80][21];
    struct rooms roomList[maxRoomNumber];
    
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

    printGame(&floor);
    
    /* while(roomCount < roomsWanted){
        roomGen(&floor);
        roomCount++;
    } */
return 0;
}

void roomGen(struct tiles(*floor)[80][21])
{
    //srand(); // Testing
    int dimX, dimY, placeX, placeY;
    dimX = rand()%80;
    dimY = rand()%21;
    placeX = rand()%80 - dimX;
    placeY = rand()%21 - dimY;
    
}

void printGame(struct tiles(*floor)[80][21])
{
    int i, j;
    
    for(i = 0; i < floorMaxY; i++){
        for(int j = 0; j < floorMaxX; j++){
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