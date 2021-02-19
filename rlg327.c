/********************************************************
 * @authors Thomas McCoy, Jens Rasmussen, and Jason Guo *
 ********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <endian.h>
#include <stdint.h>

#define minRoomNumber 6
#define maxRoomNumber 10
#define maxStairNum 3 // Arbitrary value that can be changed
#define maxRoomSize 10 // Arbitrary value that can be changed
#define minRoomX 4
#define minRoomY 3
#define floorMaxX 80
#define floorMaxY 21
#define edgeChar '#' //Hardness = 255
#define roomChar ' ' //Hardness = 0
#define corridorChar 'o' //Hardness = 0
#define rockChar '.' //Hardness = 100 (Non-zero, non-255)
#define upChar '<' //Hardness = 0
#define downChar '>' //Hardness = 0
#define playerChar '@'

struct dungeon {
    
}; 
int roomNumber;

struct tiles {
    uint8_t hardness; // Needs to be implemented
    unsigned char type;
};

struct rooms {
    int8_t cornerX; // Top left
    int8_t cornerY; // Top left
    int8_t sizeX;
    int8_t sizeY;
};

struct pc { 
    int8_t x;
    int8_t y;
};

struct stairs {
    int8_t x;
    int8_t y;
};

/*****************************************
 *             Prototypes                *
 *****************************************/

void gameGen(struct tiles floor[floorMaxY][floorMaxX], struct rooms **roomList, struct stairs stairListU[maxStairNum], struct stairs stairListD[maxStairNum], struct pc *player);
void corridorGen(struct tiles floor[floorMaxY][floorMaxX], struct rooms *roomList, int16_t roomsWanted);
void borderGen(struct tiles floor[floorMaxY][floorMaxX]);
void roomGen(struct tiles floor[floorMaxY][floorMaxX], struct rooms *roomList, int16_t roomsWanted);
void staircaseGen(struct tiles floor[floorMaxY][floorMaxX], struct rooms *roomList, struct stairs stairListU[maxStairNum], struct stairs stairListD[maxStairNum], int16_t roomsWanted, int16_t numStairs);
void playerGen(struct tiles floor[floorMaxY][floorMaxX], struct rooms *roomList, struct pc *player, int16_t roomsWanted);
char *findFilePath();
void saveGame(FILE *f, struct tiles floor[floorMaxY][floorMaxX], struct rooms *roomList, struct stairs stairListU[maxStairNum], struct stairs stairListD[maxStairNum], struct pc player);
void loadGame(FILE *f, struct tiles floor[floorMaxY][floorMaxX], struct rooms **roomList, struct stairs stairListU[maxStairNum], struct stairs stairListD[maxStairNum], struct pc *player);
void printGame(struct tiles floor[floorMaxY][floorMaxX]);

//Make dungeon struct
//Delete dungeon function(cleanup, freeing arrays)

/*****************************************
 *                Main                   *
 *****************************************/
int main(int argc, char *argv[])
{
    srand(time(NULL)); // Seed/Random
    
    struct rooms *roomList = NULL;
    struct tiles floor[floorMaxY][floorMaxX];
    struct stairs stairListU[maxStairNum];
    struct stairs stairListD[maxStairNum];
    struct pc player;

    int i;
    bool gameLoaded = false;
    FILE *f;

    printf("%s\n", findFilePath());

    if(argc >= 2){
        for(i = 1; i < argc; i++){
            if(!strcmp(argv[i], "--load")){
                //printf("pass");
                if(!(f = fopen("/home/Thomas McCoy/COM327-HW/CS327-Assignment1/samples/00.rlg327", "rb"))){
                    fprintf(stderr, "Failed to open file for reading");
                    return -1;
                }
                
                loadGame(f, floor, &roomList, stairListU, stairListD, &player);
                gameLoaded = true;
                break;
            }
        }
    }

    if(!gameLoaded){
        gameGen(floor, &roomList, stairListU, stairListD, &player);
    }
    
    if(argc >= 2){
        for(i = 1; i < argc; i++){
            if(!strcmp(argv[i], "--save")){
                if(!(f = fopen(findFilePath(), "wb"))){
                    fprintf(stderr, "Failed to open file for writing");
                    return -1;
                }
                saveGame(f, floor, roomList, stairListU, stairListD, player);
                break;
            }
        }
    }
    free(roomList);

    return 0;
}

/*****************************************
 *            Game Generator             *
 *****************************************/
void gameGen(struct tiles floor[floorMaxY][floorMaxX], struct rooms **roomList, struct stairs stairListU[maxStairNum], struct stairs stairListD[maxStairNum], struct pc *player)
{
    int16_t roomsWanted = (rand() % ((maxRoomNumber + 1) - minRoomNumber)) + minRoomNumber;
    *roomList = malloc(roomsWanted * sizeof(**roomList));
    roomNumber = roomsWanted;

    int16_t numStairs = ((rand() % roomsWanted) / 3);
    numStairs = (numStairs < 1) ? 1 : numStairs;

    borderGen(floor);
    roomGen(floor, *roomList, roomsWanted); 
    corridorGen(floor, *roomList, roomsWanted); 
    staircaseGen(floor, *roomList, stairListU, stairListD, roomsWanted, numStairs);
    playerGen(floor, *roomList, player, roomsWanted);
    printGame(floor); // Needs to be last in function
}

/*****************************************
 *           Border Generator            *
 *****************************************/ 
void borderGen(struct tiles floor[floorMaxY][floorMaxX])
{
    int i, j;
    for(i = 0; i < floorMaxY; i++){ // Top
        floor[i][0].type = edgeChar;
        floor[i][0].hardness = 255;
        floor[i][floorMaxX - 1].type = edgeChar;
        floor[i][floorMaxX- 1].hardness = 255;
    }
    for(j = 0; j < floorMaxX; j++){ // Sides
        floor[0][j].type = edgeChar;
        floor[0][j].hardness = 255;
        floor[floorMaxY - 1][j].type = edgeChar;
        floor[floorMaxY - 1][j].hardness = 255;
    }
}

/*****************************************
 *         Corridor Generator            *
 *****************************************/
void corridorGen(struct tiles floor[floorMaxY][floorMaxX], struct rooms *roomList, int16_t roomsWanted) 
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
                    floor[ranY + j][ranX].hardness = 0;
                }
            }
            else{
                l--;
                if(floor[ranY - j][ranX].type != roomChar){
                    floor[ranY - j][ranX].type = corridorChar;
                    floor[ranY - j][ranX].hardness = 0;
                }
            }
        }

        for(k = 0; k < abs(ranX - ranX2); k++){
            if(ranX < ranX2){
                if(floor[ranY + l][ranX + k].type != roomChar){
                    floor[ranY + l][ranX + k].type = corridorChar;
                    floor[ranY + l][ranX + k].hardness = 0;
                }
            }
            else {
                if(floor[ranY + l][ranX - k].type != roomChar){
                    floor[ranY + l][ranX - k].type = corridorChar;
                    floor[ranY + l][ranX - k].hardness = 0;
                }
            }
        }
    }
}

/*****************************************
 *           Room Generator              *
 *****************************************/
void roomGen(struct tiles floor[floorMaxY][floorMaxX], struct rooms *roomList, int16_t roomsWanted)
{
    int i, j, k, failCount = 0;
    bool placedAll = false;
    

    while(!placedAll){
        placedAll = true;
        // Set everything (not border) to rock
        for(i = 1; i < floorMaxY - 1; i++) { 
            for (j = 1; j < floorMaxX - 1; j++) {
                floor[i][j].type = rockChar;
                floor[i][j].hardness = 100;
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
                        floor[roomList[i].cornerY + j][roomList[i].cornerX + k].hardness = 0;
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
void staircaseGen(struct tiles floor[floorMaxY][floorMaxX], struct rooms *roomList, struct stairs stairListU[maxStairNum], struct stairs stairListD[maxStairNum], int16_t roomsWanted, int16_t numStairs) 
{
    int i, ranX, ranY, ranX2, ranY2, ranDown, ranUp;

    

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

        stairListD[i].x = ranX;
        stairListD[i].y = ranY;

        stairListU[i].x = ranX2;
        stairListU[i].y = ranY2;
        
        floor[ranY][ranX].type = downChar;
        floor[ranY2][ranX2].type = upChar;
    }
}

/*****************************************
 *           Player Generator            *
 *****************************************/
void playerGen(struct tiles floor[floorMaxY][floorMaxX], struct rooms *roomList, struct pc *player, int16_t roomsWanted) {
    int ranY, ranX, ran;
    bool placed = false;

    while(!placed){
        ran = rand() % roomsWanted;
        ranY = roomList[ran].cornerY + (rand() % roomList[ran].sizeY);
        ranX = roomList[ran].cornerX + (rand() % roomList[ran].sizeX);
        
        if(floor[ranY][ranX].type != upChar && floor[ranY][ranX].type != downChar){
            player->y = ranY;
            player->x = ranX;
            floor[player->y][player->x].type = playerChar;
            placed = true;
        }
    }
}

/*****************************************
 *          File Path Finder             *
 *****************************************/
char *findFilePath()
{   // Get rid of temp (maybe?)
    char *home = getenv("HOME");
    char *temp = "COM327-HW/CS327-Assignment1";
    char *gameDir = ".rlg327";
    char *saveFile = "dungeon";
    char *path = malloc((strlen(home) + strlen(temp) + strlen(gameDir) + strlen(saveFile) + 3 + 1) * sizeof(char));
    sprintf(path, "%s/%s/%s/%s", home, temp, gameDir, saveFile); // Get rid of temp
    return path;
}

/*****************************************
 *             Game Saver                *
 *****************************************/
void saveGame(FILE *f, struct tiles floor[floorMaxY][floorMaxX], struct rooms *roomList, struct stairs stairListU[maxStairNum], 
              struct stairs stairListD[maxStairNum], struct pc player)
{
    int numRooms = roomNumber;
    int UstairNum = sizeof(*stairListU) / sizeof(stairListU[0]);
    int DstairNum = sizeof(*stairListD) / sizeof(stairListD[0]);
    
    // Semantic file-type marker
    char semantic[] = "RLG327-S2021";
    fwrite(semantic, 1, 12, f);

    // Version number
    int32_t version = 0;
    version = htobe32(version);
    fwrite(&version, 4, 1, f);

    // File size
    int32_t size = 1708 + 4 * numRooms + 2 * DstairNum + 2 * UstairNum;
    size = htobe32(size);
    fwrite(&size, 4, 1, f);

    // Player Character location
    fwrite(&player.x, 1, 1, f);
    fwrite(&player.y, 1, 1, f);
    
    // Dungeon hardness
    int i, j;
    for(i = 0; i < floorMaxY; i++){
        for(j = 0; j < floorMaxX; j++){
            fwrite(&floor[i][j].hardness, 1, 1, f);
        }
    }

    // Number of rooms
    int16_t roomNum = htobe16(numRooms);
    fwrite(&roomNum, 2, 1, f);

    for(i = 0; i < roomNumber; i++){
        fwrite(&roomList[i].cornerX, 1, 1, f);
        fwrite(&roomList[i].cornerY, 1, 1, f);
        fwrite(&roomList[i].sizeX, 1, 1, f);
        fwrite(&roomList[i].sizeY, 1, 1, f);
    }
    
    // Number of up stairs
    int16_t upNum = htobe16(UstairNum);
    fwrite(&upNum, 2, 1, f);

    // Location of up stairs
    for(i = 0; i < UstairNum; i++) {
        fwrite(&stairListU[i].x, 1, 1, f);
        fwrite(&stairListU[i].y, 1, 1, f);
    }

    // Number of down stairs
    int16_t downNum = htobe16(DstairNum);
    fwrite(&downNum, 2, 1, f);

    // Location of up stairs
    for(i = 0; i < DstairNum; i++) {
        fwrite(&stairListD[i].x, 1, 1, f);
        fwrite(&stairListD[i].y, 1, 1, f);
    }
    fclose(f);
}

/*****************************************
 *            Game Loader                *
 *****************************************/
void loadGame(FILE *f, struct tiles floor[floorMaxY][floorMaxX], struct rooms **roomList, struct stairs stairListU[maxStairNum], struct stairs stairListD[maxStairNum], struct pc *player)
{
    // Semantic file-type marker
    char semantic[13];
    semantic[12] = '\0';
    fread(semantic, 1, 12, f);

    // Version number
    int32_t version;
    fread(&version, 4, 1, f);
    version = be32toh(version);

    // File size
    int32_t size;
    fread(&size, 4, 1, f);
    size = be32toh(size);

    // Player Character location
    fread(&player->x, 1, 1, f);
    fread(&player->y, 1, 1, f);
    floor[player->y][player->x].type = playerChar;
   
    // Dungeon hardness
    int i, j, k;
    for(i = 0; i < floorMaxY; i++){
        for(j = 0; j < floorMaxX; j++){
            fread(&floor[i][j].hardness, 1, 1, f);
            if(floor[i][j].hardness == 0 && floor[i][j].type != playerChar){
                floor[i][j].type = corridorChar; 
            } else if(floor[i][j].hardness != 0){
                floor[i][j].type = rockChar;
            } 
            if(floor[i][j].hardness == 255){
                floor[i][j].type = edgeChar;
            }
            //printf("%3d ", floor[i][j].hardness);
        }
        //printf("\n");
    }
    
    // Number of rooms
    int16_t roomsWanted;
    fread(&roomsWanted, 2, 1, f);
    roomsWanted = be16toh(roomsWanted);

    *roomList = calloc(roomsWanted, sizeof(struct rooms));
    if(*roomList == NULL){
        fprintf(stderr, "error allocating memory for list");
        exit(1);
    }
    roomNumber = roomsWanted;

    for(i = 0; i < roomsWanted; i++){
        //printf("cX: %d, cY: %d, sX: %d, sY %d\n\n", tempCornerX, tempCornerY, tempSizeX, tempSizeY);
        fread(&roomList[i]->cornerX, 1, 1, f);
        fread(&roomList[i]->cornerY, 1, 1, f);
        fread(&roomList[i]->sizeX, 1, 1, f);
        fread(&roomList[i]->sizeY, 1, 1, f);
   
        for (j = 0; j < roomList[i]->sizeY; j++) {
            for (k = 0; k < roomList[i]->sizeX; k++) {
                if(floor[roomList[i]->cornerY + j][roomList[i]->cornerX + k].type != playerChar){
                    floor[roomList[i]->cornerY + j][roomList[i]->cornerX + k].type = roomChar;
                }
            }
        }
    }

    // Number of up stairs
    int16_t upNum;
    fread(&upNum, 2, 1, f);
    upNum = be16toh(upNum);
    
    // Location of up stairs
    for(i = 0; i < upNum; i++) {
        fread(&stairListU[i].x, 1, 1, f);
        fread(&stairListU[i].y, 1, 1, f);
        floor[stairListU[i].y][stairListU[i].x].type = upChar;
    }

    // Number of down stairs
    int16_t downNum;
    fread(&downNum, 2, 1, f);
    downNum = be16toh(downNum);

    // Location of down stairs
    for(i = 0; i < downNum; i++) {
        fread(&stairListD[i].x, 1, 1, f);
        fread(&stairListD[i].y, 1, 1, f);
        floor[stairListD[i].y][stairListD[i].x].type = downChar;
    }
    
    printGame(floor);
    fclose(f);
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
                case playerChar :
                    printf("%c", playerChar);
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