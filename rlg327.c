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
#include <limits.h>

#include "heap.h"
#include "heap.c"

#define minRoomNumber  6
#define maxRoomNumber 10
#define maxStairNum    3        // Arbitrary value that can be changed
#define maxRoomSize   10        // Arbitrary value that can be changed
#define minRoomX       4
#define minRoomY       3
#define floorMaxX     80
#define floorMaxY     21
#define edgeChar     '#'        //Hardness = 255
#define roomChar     ' '        //Hardness = 0
#define corridorChar 'o'        //Hardness = 0
#define rockChar     '.'        //Hardness = 100 (Non-zero, non-255)
#define upChar       '<'        //Hardness = 0
#define downChar     '>'        //Hardness = 0
#define playerChar   '@'
#define dimY          0
#define dimX          1      

typedef struct corPath {
  heap_node_t *hn;
  uint8_t pos[2];
  int32_t cost;
} corPath;

typedef struct room {
    int8_t cornerX;     // Top left corner
    int8_t cornerY;     // Top left corner
    int8_t sizeX;
    int8_t sizeY;
} room;

typedef struct pc { 
    int8_t x;
    int8_t y;
} pc;

typedef struct stair {
    int8_t x;
    int8_t y;
} stair;

typedef struct dungeon {
    corPath path[floorMaxY][floorMaxX];
    char floor[floorMaxY][floorMaxX];
    uint8_t hardness[floorMaxY][floorMaxX];
    uint8_t nonTunDist[floorMaxY][floorMaxX];
    uint8_t tunDist[floorMaxY][floorMaxX];
    room *roomList;
    stair *stairListU;
    stair *stairListD;
    pc player;
    int16_t numRooms;
    int16_t numUStairs;
    int16_t numDStairs;
} dungeon; 

/*****************************************
 *             Prototypes                *
 *****************************************/

void gameGen(dungeon *d);
void corridorGen(dungeon *d);
void borderGen(dungeon *d);
void roomGen(dungeon *d);
void staircaseGen(dungeon *d);
void playerGen(dungeon *d);
void dijkstra(dungeon *d, char str[]);
char *findFilePath();
void saveGame(FILE *f, dungeon d);
void loadGame(FILE *f, dungeon *d);
void printGame(dungeon *d);
void printMap(dungeon *d);
void dungeonDelete(dungeon d);

/*****************************************
 *                Main                   *
 *****************************************/
int main(int argc, char *argv[])
{
    srand(time(NULL)); // Seed/Random
    dungeon d;
    d.roomList = NULL;
    d.stairListU = NULL;
    d.stairListD = NULL;
    int i;
    bool gameLoaded = false;
    FILE *f;

    //printf("%s\n", findFilePath());
    if(argc >= 2){
        for(i = 1; i < argc; i++){
            if(!strcmp(argv[i], "--load")){
                if(!(f = fopen(findFilePath(), "rb"))){ //"/cygdrive/u/spring2021/COMS 327/Homework 1.02/CS327-Assignment1/samples/welldone.rlg327"
                    fprintf(stderr, "Failed to open file for reading");
                    return -1;
                }
                loadGame(f, &d);
                gameLoaded = true;
                break;
            }
        }
    }

    if(!gameLoaded){
        gameGen(&d);
    }
    
    dijkstra(&d, "non-tunneling");
    dijkstra(&d, "tunneling");

    if(argc >= 2){
        for(i = 1; i < argc; i++){
            if(!strcmp(argv[i], "--save")){
                if(!(f = fopen(findFilePath(), "wb"))){
                    fprintf(stderr, "Failed to open file for writing");
                    return -1;
                }
                saveGame(f, d);
                break;
            }
        }
    }
    dungeonDelete(d);
    return 0;
}

/*****************************************
 *            Game Generator             *
 *****************************************/
void gameGen(dungeon *d)
{
    // Finding number of rooms and allocating memory
    d->numRooms = (rand() % ((maxRoomNumber + 1) - minRoomNumber)) + minRoomNumber;
    d->roomList = malloc(d->numRooms * sizeof(room));
    // Finding number of up stairs and allocating memory
    d->numUStairs = ((rand() % d->numRooms) / 3);
    d->numUStairs = (d->numUStairs < 1) ? 1 : d->numUStairs;
    d->stairListU = malloc(d->numUStairs * sizeof(stair));
    // Finding number of down stairs and allocating memory
    d->numDStairs = ((rand() % d->numRooms) / 3);
    d->numDStairs = (d->numDStairs < 1) ? 1 : d->numDStairs;
    d->stairListD = malloc(d->numDStairs * sizeof(stair));

    borderGen(d);
    roomGen(d); 
    corridorGen(d); 
    staircaseGen(d);
    playerGen(d);
    printGame(d); // Needs to be last in function
}

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
 *           Player Generator            *
 *****************************************/
void playerGen(dungeon *d) {
    int ranY, ranX, ran;
    bool placed = false;

    while(!placed){
        ran = rand() % d->numRooms;
        ranY = d->roomList[ran].cornerY + (rand() % d->roomList[ran].sizeY);
        ranX = d->roomList[ran].cornerX + (rand() % d->roomList[ran].sizeX);
        
        if(d->floor[ranY][ranX] != upChar && d->floor[ranY][ranX] != downChar){
            d->player.y = ranY;
            d->player.x = ranX;
            d->floor[d->player.y][d->player.x] = playerChar;
            placed = true;
        }
    }
}

// Priority queue
// Two pathfinding algorithms (Dijkstra's treating each cell in the dungeon as a node in a graph with 8-way connectivity)
// Non-tunneling
// - Floor (and stairs) are weight 1
// - Ignore wall cells
// Tunneling
// - Hardness of 0 are weight 1
// - Hardness [1, 254] are weight 1 + (hardness/85)
// - Hardness of 255 has infinite weight (not in the queue)
// Two distance maps (non-tunneling and tunneling) of the distance from PC

/*****************************************
 *        Corridor Path Compare          *
 *****************************************/
static int32_t corridor_path_cmp(const void *key, const void *with) 
{
    return ((corPath *) key)->cost - ((corPath *) with)->cost;
}

/*****************************************
 *         Dungeon Cost Finder           *
 *****************************************/
void dijkstra(dungeon *d, char str[]) 
{
    static corPath *p;
    static uint32_t initialized = 0;
    heap_t h;
    uint32_t x, y;

    if (!initialized) {
        for (y = 0; y < floorMaxY; y++) {
            for (x = 0; x < floorMaxX; x++) {
                d->path[y][x].pos[dimY] = y;
                d->path[y][x].pos[dimX] = x;
            }
        }
        initialized = 1;
    }

    for (y = 0; y < floorMaxY; y++) {
        for (x = 0; x < floorMaxX; x++) {
            d->path[y][x].cost = INT_MAX;
        }
    }

    d->path[d->player.y][d->player.x].cost = 0; 

    heap_init(&h, corridor_path_cmp, NULL); 
    
    if(!strcmp(str, "non-tunneling")){
        for (y = 0; y < floorMaxY; y++) {
            for (x = 0; x < floorMaxX; x++) {
                if (d->floor[y][x] != edgeChar && d->floor[y][x] != rockChar) {
                    d->path[y][x].hn = heap_insert(&h, &d->path[y][x]);
                } else {
                    d->path[y][x].hn = NULL;
                }
            }
        }
    } else if(!strcmp(str, "tunneling")){
        for (y = 0; y < floorMaxY; y++) {
            for (x = 0; x < floorMaxX; x++) {
                if (d->floor[y][x] != edgeChar) {
                    d->path[y][x].hn = heap_insert(&h, &d->path[y][x]);
                } else {
                    d->path[y][x].hn = NULL;
                }
            }
        }
    } else {
        exit(1);
    }

    while ((p = heap_remove_min(&h))) {
        p->hn = NULL;
        // 4 cases for cardinal directions
        if ((d->path[p->pos[dimY] - 1][p->pos[dimX]    ].hn) &&
            (d->path[p->pos[dimY] - 1][p->pos[dimX]    ].cost >
             p->cost + d->hardness[p->pos[dimY]][p->pos[dimX]] / 85)) {
            d->path[p->pos[dimY] - 1][p->pos[dimX]    ].cost = p->cost + d->hardness[p->pos[0]][p->pos[1]] / 85 + 1;
            heap_decrease_key_no_replace(&h, d->path[p->pos[dimY] - 1][p->pos[dimX]    ].hn);
        }
        if ((d->path[p->pos[dimY]    ][p->pos[dimX] - 1].hn) &&
            (d->path[p->pos[dimY]    ][p->pos[dimX] - 1].cost >
             p->cost + d->hardness[p->pos[dimY]][p->pos[dimX]] / 85)) {
            d->path[p->pos[dimY]    ][p->pos[dimX] - 1].cost = p->cost + d->hardness[p->pos[0]][p->pos[1]] / 85 + 1;
            heap_decrease_key_no_replace(&h, d->path[p->pos[dimY]    ][p->pos[dimX] - 1].hn);
        }
        if ((d->path[p->pos[dimY]    ][p->pos[dimX] + 1].hn) &&
            (d->path[p->pos[dimY]    ][p->pos[dimX] + 1].cost >
             p->cost + d->hardness[p->pos[dimY]][p->pos[dimX]] / 85)) {
            d->path[p->pos[dimY]    ][p->pos[dimX] + 1].cost = p->cost + d->hardness[p->pos[0]][p->pos[1]] / 85 + 1;
            heap_decrease_key_no_replace(&h, d->path[p->pos[dimY]    ][p->pos[dimX] + 1].hn);
        }
        if ((d->path[p->pos[dimY] + 1][p->pos[dimX]    ].hn) &&
            (d->path[p->pos[dimY] + 1][p->pos[dimX]    ].cost >
             p->cost + d->hardness[p->pos[dimY]][p->pos[dimX]] / 85)) {
            d->path[p->pos[dimY] + 1][p->pos[dimX]    ].cost = p->cost + d->hardness[p->pos[0]][p->pos[1]] / 85 + 1;
            heap_decrease_key_no_replace(&h, d->path[p->pos[dimY] + 1][p->pos[dimX]    ].hn);
        }
        // 4 more cases for diagonal directions. 
        if ((d->path[p->pos[dimY] - 1][p->pos[dimX] - 1].hn) &&
            (d->path[p->pos[dimY] - 1][p->pos[dimX] - 1].cost >
             p->cost + d->hardness[p->pos[dimY]][p->pos[dimX]] / 85)) {
            d->path[p->pos[dimY] - 1][p->pos[dimX] - 1].cost = p->cost + d->hardness[p->pos[0]][p->pos[1]] / 85 + 1;
            heap_decrease_key_no_replace(&h, d->path[p->pos[dimY] - 1][p->pos[dimX] - 1].hn);
        }
        if ((d->path[p->pos[dimY] - 1][p->pos[dimX] + 1].hn) &&
            (d->path[p->pos[dimY] - 1][p->pos[dimX] + 1].cost >
             p->cost + d->hardness[p->pos[dimY]][p->pos[dimX]] / 85)) {
            d->path[p->pos[dimY] - 1][p->pos[dimX] + 1].cost = p->cost + d->hardness[p->pos[0]][p->pos[1]] / 85 + 1;
            heap_decrease_key_no_replace(&h, d->path[p->pos[dimY] - 1][p->pos[dimX] + 1].hn);
        }
        if ((d->path[p->pos[dimY] + 1][p->pos[dimX] - 1].hn) &&
            (d->path[p->pos[dimY] + 1][p->pos[dimX] - 1].cost >
             p->cost + d->hardness[p->pos[dimY]][p->pos[dimX]] / 85)) {
            d->path[p->pos[dimY] + 1][p->pos[dimX] - 1].cost = p->cost + d->hardness[p->pos[0]][p->pos[1]] / 85 + 1;
            heap_decrease_key_no_replace(&h, d->path[p->pos[dimY] + 1][p->pos[dimX] - 1].hn);
        }
        if ((d->path[p->pos[dimY] + 1][p->pos[dimX] + 1].hn) &&
            (d->path[p->pos[dimY] + 1][p->pos[dimX] + 1].cost >
             p->cost + d->hardness[p->pos[dimY]][p->pos[dimX]] / 85)) {
            d->path[p->pos[dimY] + 1][p->pos[dimX] + 1].cost = p->cost + d->hardness[p->pos[0]][p->pos[1]] / 85 + 1;
            heap_decrease_key_no_replace(&h, d->path[p->pos[dimY] + 1][p->pos[dimX] + 1].hn);
        }
    }
    printMap(d);
}


/*****************************************
 *           File Path Finder            *
 *****************************************/
char *findFilePath()
{
    char *home = getenv("HOME"); // Final game & Thomas'
    char *gameDir = ".rlg327"; // Final game
    char *saveFile = "dungeon"; // Final game
    char *path = malloc((strlen(home) + strlen(gameDir) + strlen(saveFile) + 2 + 1)); // Final game
    sprintf(path, "%s/%s/%s", home, gameDir, saveFile); // Final game

    //char *thomas = "COM327-HW/CS327-Assignment1"; // Thomas'
    //char *path = malloc((strlen(home) + strlen(thomas) + strlen(gameDir) + strlen(saveFile) + 3 + 1) * sizeof(char)); // Thomas'
    //sprintf(path, "%s/%s/%s/%s", home, thomas, gameDir, saveFile); // Thomas'

    //char *jens = "/cygdrive/u/spring2021/COMS 327/Homework 1.02/CS327-Assignment1"; // Jens'
    //char *path = malloc((strlen(jens) + strlen(gameDir) + strlen(saveFile) + 2 + 1)); // Jens'
    //sprintf(path, "%s/%s/%s", jens, gameDir, saveFile); // Jens'

    return path;
}

/*****************************************
 *             Game Saver                *
 *****************************************/
void saveGame(FILE *f, dungeon d)
{    
    // Semantic file-type marker
    char semantic[] = "RLG327-S2021";
    fwrite(semantic, 1, 12, f);

    // Version number
    int32_t version = 0;
    version = htobe32(version);
    fwrite(&version, 4, 1, f);

    // File size
    int32_t size = 1708 + 4 * d.numRooms + 2 * d.numUStairs + 2 * d.numDStairs;
    size = htobe32(size);
    fwrite(&size, 4, 1, f);

    // Player Character location
    fwrite(&d.player.x, 1, 1, f);
    fwrite(&d.player.y, 1, 1, f);
    
    // Dungeon hardness
    int i, j;
    for(i = 0; i < floorMaxY; i++){
        for(j = 0; j < floorMaxX; j++){
            fwrite(&d.hardness[i][j], 1, 1, f);
        }
    }

    // Number of rooms
    int16_t roomNum = htobe16(d.numRooms);
    fwrite(&roomNum, 2, 1, f);

    for(i = 0; i < d.numRooms; i++){
        fwrite(&d.roomList[i].cornerX, 1, 1, f);
        fwrite(&d.roomList[i].cornerY, 1, 1, f);
        fwrite(&d.roomList[i].sizeX, 1, 1, f);
        fwrite(&d.roomList[i].sizeY, 1, 1, f);
    }
    
    // Number of up stairs
    int16_t upNum = htobe16(d.numUStairs);
    fwrite(&upNum, 2, 1, f);

    // Location of up stairs
    for(i = 0; i < d.numUStairs; i++) {
        fwrite(&d.stairListU[i].x, 1, 1, f);
        fwrite(&d.stairListU[i].y, 1, 1, f);
    }

    // Number of down stairs
    int16_t downNum = htobe16(d.numDStairs);
    fwrite(&downNum, 2, 1, f);

    // Location of up stairs
    for(i = 0; i < d.numDStairs; i++) {
        fwrite(&d.stairListD[i].x, 1, 1, f);
        fwrite(&d.stairListD[i].y, 1, 1, f);
    }
    fclose(f);
}

/*****************************************
 *            Game Loader                *
 *****************************************/
void loadGame(FILE *f, dungeon *d)
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
    fread(&d->player.x, 1, 1, f);
    fread(&d->player.y, 1, 1, f);
    d->floor[d->player.y][d->player.x] = playerChar;
   
    // Dungeon hardness
    int i, j, k;
    for(i = 0; i < floorMaxY; i++){
        for(j = 0; j < floorMaxX; j++){
            fread(&d->hardness[i][j], 1, 1, f);
            if(d->hardness[i][j] == 0 && d->floor[i][j] != playerChar){
                d->floor[i][j] = corridorChar; 
            } else if(d->hardness[i][j] != 0){
                d->floor[i][j] = rockChar;
            } 
            if(d->hardness[i][j] == 255){
                d->floor[i][j] = edgeChar;
            }
        }
    }
    
    // Number of rooms
    int16_t roomsWanted;
    fread(&roomsWanted, 2, 1, f);
    d->numRooms = be16toh(roomsWanted);

    d->roomList = calloc(d->numRooms, sizeof(room));

    for(i = 0; i < d->numRooms; i++){
        fread(&d->roomList[i].cornerX, 1, 1, f);
        fread(&d->roomList[i].cornerY, 1, 1, f);
        fread(&d->roomList[i].sizeX, 1, 1, f);
        fread(&d->roomList[i].sizeY, 1, 1, f);
   
        for (j = 0; j < d->roomList[i].sizeY; j++) {
            for (k = 0; k < d->roomList[i].sizeX; k++) {
                if(d->floor[d->roomList[i].cornerY + j][d->roomList[i].cornerX + k] != playerChar){
                    d->floor[d->roomList[i].cornerY + j][d->roomList[i].cornerX + k] = roomChar;
                }
            }
        }
    }

    // Number of up stairs
    int16_t upNum;
    fread(&upNum, 2, 1, f);
    d->numUStairs = be16toh(upNum);

    d->stairListU = calloc(d->numUStairs, sizeof(stair));
    
    // Location of up stairs
    for(i = 0; i < d->numUStairs; i++) {
        fread(&d->stairListU[i].x, 1, 1, f); // BUG
        fread(&d->stairListU[i].y, 1, 1, f);
        d->floor[d->stairListU[i].y][d->stairListU[i].x] = upChar;
    }

    // Number of down stairs
    int16_t downNum;
    fread(&downNum, 2, 1, f);
    d->numDStairs = be16toh(downNum);

    d->stairListD = calloc(d->numDStairs, sizeof(stair));

    // Location of down stairs
    for(i = 0; i < d->numDStairs; i++) {
        fread(&d->stairListD[i].x, 1, 1, f);
        fread(&d->stairListD[i].y, 1, 1, f);
        d->floor[d->stairListD[i].y][d->stairListD[i].x] = downChar;
    }
    
    printGame(d);
    fclose(f);
}

/*****************************************
 *            Game Printer               *
 *****************************************/
void printGame(dungeon *d)
{
    int i, j;
    for(i = 0; i < floorMaxY; i++){ 
        for(j = 0; j < floorMaxX; j++){
            switch(d->floor[i][j]) {
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
    printf("\n");
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
 *           Dungeon Deletor             *
 *****************************************/
void dungeonDelete(dungeon d)
{
    free(d.roomList);
    free(d.stairListU);
    free(d.stairListD);
}