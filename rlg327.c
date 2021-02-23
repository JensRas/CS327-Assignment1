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
#include "heap.h"

#define minRoomNumber 6
#define maxRoomNumber 10
#define maxStairNum 3       // Arbitrary value that can be changed
#define maxRoomSize 10      // Arbitrary value that can be changed
#define minRoomX 4
#define minRoomY 3
#define floorMaxX 80
#define floorMaxY 21
#define edgeChar '#'        //Hardness = 255
#define roomChar ' '        //Hardness = 0
#define corridorChar 'o'    //Hardness = 0
#define rockChar '.'        //Hardness = 100 (Non-zero, non-255)
#define upChar '<'          //Hardness = 0
#define downChar '>'        //Hardness = 0
#define playerChar '@'      

typedef struct tile {
    uint8_t hardness;
    uint8_t nonTunDist;
    uint8_t tunDist;
    char type;
} tile;

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
    tile floor[floorMaxY][floorMaxX];
    room *roomList;
    stair *stairListU;
    stair *stairListD;
    pc player;
    int16_t numRooms;
    int16_t numUStairs;
    int16_t numDStairs;
} dungeon; 

struct Graph {
    struct Node* head[N];
};

struct Node {
    int dest;
    struct Node* next;
};

struct Edge {
    int src, dest;
}

/*****************************************
 *             Prototypes                *
 *****************************************/

void gameGen(dungeon *d);
void corridorGen(dungeon *d);
void borderGen(dungeon *d);
void roomGen(dungeon *d);
void staircaseGen(dungeon *d);
void playerGen(dungeon *d);
void Dijkstra(Graph, source);
void nonTunPF();
void tunPF();
char *findFilePath();
void saveGame(FILE *f, dungeon d);
void loadGame(FILE *f, dungeon *d);
void printGame(dungeon *d);
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
        d->floor[i][0].type = edgeChar;
        d->floor[i][0].hardness = 255;
        d->floor[i][floorMaxX - 1].type = edgeChar;
        d->floor[i][floorMaxX- 1].hardness = 255;
    }
    for(j = 0; j < floorMaxX; j++){ // Top/Bottom
        d->floor[0][j].type = edgeChar;
        d->floor[0][j].hardness = 255;
        d->floor[floorMaxY - 1][j].type = edgeChar;
        d->floor[floorMaxY - 1][j].hardness = 255;
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
                if(d->floor[ranY + j][ranX].type != roomChar){
                    d->floor[ranY + j][ranX].type = corridorChar;
                    d->floor[ranY + j][ranX].hardness = 0;
                }
            }
            else{
                l--;
                if(d->floor[ranY - j][ranX].type != roomChar){
                    d->floor[ranY - j][ranX].type = corridorChar;
                    d->floor[ranY - j][ranX].hardness = 0;
                }
            }
        }

        for(k = 0; k < abs(ranX - ranX2); k++){
            if(ranX < ranX2){
                if(d->floor[ranY + l][ranX + k].type != roomChar){
                    d->floor[ranY + l][ranX + k].type = corridorChar;
                    d->floor[ranY + l][ranX + k].hardness = 0;
                }
            }
            else {
                if(d->floor[ranY + l][ranX - k].type != roomChar){
                    d->floor[ranY + l][ranX - k].type = corridorChar;
                    d->floor[ranY + l][ranX - k].hardness = 0;
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
                d->floor[i][j].type = rockChar;
                d->floor[i][j].hardness = 100;
            }
        }
        
        for(i = 0; i < d->numRooms; i++){
            d->roomList[i].sizeX = rand() % (maxRoomSize - minRoomX) + minRoomX;
            d->roomList[i].sizeY = rand() % (maxRoomSize - minRoomY) + minRoomY;
            d->roomList[i].cornerX = rand() % (floorMaxX - d->roomList[i].sizeX - 2);
            d->roomList[i].cornerY = rand() % (floorMaxY - d->roomList[i].sizeY - 2);

            for (j = -1; j < d->roomList[i].sizeY + 1; j++) {
                for (k = -1; k < d->roomList[i].sizeX + 1; k++) {
                    if(d->floor[d->roomList[i].cornerY + j][d->roomList[i].cornerX + k].type == roomChar || 
                        d->floor[d->roomList[i].cornerY + j][d->roomList[i].cornerX + k].type == edgeChar){
                        placedAll = false;
                    }
                }
            } 
            if(placedAll){
                for (j = 0; j < d->roomList[i].sizeY; j++) {
                    for (k = 0; k < d->roomList[i].sizeX; k++) {
                        d->floor[d->roomList[i].cornerY + j][d->roomList[i].cornerX + k].type = roomChar;
                        d->floor[d->roomList[i].cornerY + j][d->roomList[i].cornerX + k].hardness = 0;
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

        d->floor[ranY][ranX].type = upChar;
    }
    // Down staircases
    for(i = 0; i < d->numDStairs; i++){
        ran = rand() % d->numRooms;
        
        ranY = d->roomList[ran].cornerY + (rand() % d->roomList[ran].sizeY);
        ranX = d->roomList[ran].cornerX + (rand() % d->roomList[ran].sizeX);

        d->stairListD[i].y = ranY;
        d->stairListD[i].x = ranX;

        d->floor[ranY][ranX].type = downChar;
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
        
        if(d->floor[ranY][ranX].type != upChar && d->floor[ranY][ranX].type != downChar){
            d->player.y = ranY;
            d->player.x = ranX;
            d->floor[d->player.y][d->player.x].type = playerChar;
            placed = true;
        }
    }
}


struct Graph* createGraph(struct Edge edges[], int n)
{
    unsigned i;
 
    // allocate storage for the graph data structure
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
 
    // initialize head pointer for all vertices
    for (i = 0; i < N; i++) {
        graph->head[i] = NULL;
    }
 
    // add edges to the directed graph one by one
    for (i = 0; i < n; i++)
    {
        // get the source and destination vertex
        int src = edges[i].src;
        int dest = edges[i].dest;
 
        // allocate a new node of adjacency list from `src` to `dest`
        struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
        newNode->dest = dest;
 
        // point new node to the current head
        newNode->next = graph->head[src];
 
        // point head pointer to the new node
        graph->head[src] = newNode;
    }
 
    return graph;
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
 *         Dijkstra's Algorithm          *
 *****************************************/
void Dijkstra(Graph, source) 
{
    dist[source] = 0                         // Initialization

    create vertex priority queue Q

    for (v; v < graph size; v++) {           //each vertex v in Graph:          
        if (v != source){
            dist[v] = INTMAX_MAX;            // Unknown distance from source to v // Cost set to infinity
            prev[v] = 0;                     // Predecessor of v // Set cost to 0
        }
        Q.add_with_priority(v, dist[v])
    }

    while (Q is not empty){                  // The main loop
        u = Q.extract_min()                  // Remove and return best vertex
        for (){                              // each neighbor v of u:     // only v that are still in Q
            alt = dist[u] + length(u, v)
            if (alt < dist[v]){
                dist[v] = alt;
                prev[v] = u;
                Q.decrease_priority(v, alt)
            }
        }  
    }
    return dist, prev
    // From Sheaffer's code : 
    static void dijkstra_corridor(dungeon_t *d, pair_t from, pair_t to) // take param of graoh instead of dungeon?
    {
        static corridor_path_t d->floor[DUNGEON_Y][DUNGEON_X], *p;
        static uint32_t initialized = 0;
        heap_t h;
        uint32_t x, y;

        if (!initialized) {
            for (y = 0; y < DUNGEON_Y; y++) {
                for (x = 0; x < DUNGEON_X; x++) {
                    d->floor[y][x].pos[dim_y] = y;
                    d->floor[y][x].pos[dim_x] = x;
                }
            }
            initialized = 1;
        }
  
        for (y = 0; y < DUNGEON_Y; y++) {
            for (x = 0; x < DUNGEON_X; x++) {
                d->floor[y][x].cost = INT_MAX;
            }
        }

        d->floor[from[dim_y]][from[dim_x]].cost = 0;

        heap_init(&h, corridor_path_cmp, NULL);

        for (y = 0; y < DUNGEON_Y; y++) {
            for (x = 0; x < DUNGEON_X; x++) {
                if (mapxy(x, y) != ter_wall_immutable) {
                    d->floor[y][x].hn = heap_insert(&h, &d->floor[y][x]);
                } else {
                    d->floor[y][x].hn = NULL;
                }
            }
        }

        while ((p = heap_remove_min(&h))) {
            p->hn = NULL;

            /* if ((p->pos[dim_y] == to[dim_y]) && p->pos[dim_x] == to[dim_x]) {
                for (x = to[dim_x], y = to[dim_y]; (x != from[dim_x]) || (y != from[dim_y]); p = &d->floor[y][x], x = p->from[dim_x], y = p->from[dim_y]) {
                    if (mapxy(x, y) != ter_floor_room) {
                        mapxy(x, y) = ter_floor_hall;
                        hardnessxy(x, y) = 0;
                    }
                }
                heap_delete(&h);
                return;
            } */

            if ((d->floor[p->pos[dim_y] - 1][p->pos[dim_x]    ].hn) &&
                (d->floor[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost >
                 p->cost + hardnesspair(p->pos))) {
                d->floor[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost = p->cost + hardnesspair(p->pos);
                d->floor[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
                d->floor[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
                heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]p->pos[dim_x]    ].hn);
            }
            if ((d->floor[p->pos[dim_y]    ][p->pos[dim_x] - 1].hn) &&
                (d->floor[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost >
                 p->cost + hardnesspair(p->pos))) {
                d->floor[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost = p->cost + hardnesspair(p->pos);
                d->floor[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
                d->floor[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
                heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ][p->pos[dim_x] - 1].hn);
            }
            if ((d->floor[p->pos[dim_y]    ][p->pos[dim_x] + 1].hn) &&
                (d->floor[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost >
                 p->cost + hardnesspair(p->pos))) {
                d->floor[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost = p->cost + hardnesspair(p->pos);
                d->floor[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
                d->floor[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
                heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ][p->pos[dim_x] + 1].hn);
            }
            if ((d->floor[p->pos[dim_y] + 1][p->pos[dim_x]    ].hn) &&
                (d->floor[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost >
                p->cost + hardnesspair(p->pos))) {
                d->floor[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost = p->cost + hardnesspair(p->pos);
                d->floor[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
                d->floor[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
                heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1][p->pos[dim_x]    ].hn);
            }
        }
    }
}

/*****************************************
 *          Non-tunneling Graph          *
 *****************************************/
void nonTunGraph()
{
    
}

/*****************************************
 *            Tunneling Graph            *
 *****************************************/
void tunGraph() 
{
    
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
            fwrite(&d.floor[i][j].hardness, 1, 1, f);
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
    d->floor[d->player.y][d->player.x].type = playerChar;
   
    // Dungeon hardness
    int i, j, k;
    for(i = 0; i < floorMaxY; i++){
        for(j = 0; j < floorMaxX; j++){
            fread(&d->floor[i][j].hardness, 1, 1, f);
            if(d->floor[i][j].hardness == 0 && d->floor[i][j].type != playerChar){
                d->floor[i][j].type = corridorChar; 
            } else if(d->floor[i][j].hardness != 0){
                d->floor[i][j].type = rockChar;
            } 
            if(d->floor[i][j].hardness == 255){
                d->floor[i][j].type = edgeChar;
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
                if(d->floor[d->roomList[i].cornerY + j][d->roomList[i].cornerX + k].type != playerChar){
                    d->floor[d->roomList[i].cornerY + j][d->roomList[i].cornerX + k].type = roomChar;
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
        d->floor[d->stairListU[i].y][d->stairListU[i].x].type = upChar;
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
        d->floor[d->stairListD[i].y][d->stairListD[i].x].type = downChar;
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
            switch(d->floor[i][j].type) {
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

/*****************************************
 *             Map Printer               *
 *****************************************/
void printMap(dungeon *d, char maptype[]) 
{
    int i, j;
    // Print distance maps
    if(!strcmp(maptype, "tunneler")){
        for(i = 0; i < floorMaxY; i++){
            for(j = 0; j < floorMaxX; j++){
                if(d->floor[i][j].tunDist != -1 || d->floor[i][j].type != playerChar){ // Distance from Player
                    printf("%d ", d->floor[i][j].tunDist % 10);
                } else if(d->floor[i][j].type == edgeChar) { // Border
                    printf("%c ", edgeChar);
                } else if(d->floor[i][j].type == playerChar) { // Player
                    printf("%c ", playerChar);
                } else { 
                    printf(" "); // Rooms
                }
            }
            printf("\n");
        }
    } else { 
        for(i = 0; i < floorMaxY; i++){
            for(j = 0; j < floorMaxX; j++){
                if(d->floor[i][j].type != playerChar){ // Distance from Player
                    printf("%d ", d->floor[i][j].nonTunDist % 10);
                } else if(d->floor[i][j].type == edgeChar) { // Border
                    printf("%c ", edgeChar);
                } else if(d->floor[i][j].type == playerChar) { // Player
                    printf("%c ", playerChar);
                } else { 
                    printf("E"); // Error
                }
            }
            printf("\n");
        }
    } 
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