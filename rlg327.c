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
#include <unistd.h>
#include <ncurses.h>

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
#define defaultMonNum 30 
#define BIT_SMART     0x1
#define BIT_TELE      0x2
#define BIT_TUN       0x4  
#define BIT_ERAT      0x8


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
    
} pc;

typedef struct npc {
    int8_t type;
    int8_t knownPCX;
    int8_t knownPCY;
    char *adj;
} npc; 

typedef struct character {
    heap_node_t *hn;
    int8_t x;
    int8_t y;
    int8_t speed;
    int nTurn;
    int8_t isPC;
    int8_t isAlive;
    int8_t sequenceNum;
    union Entity {
        pc player;
        npc nonPlayer;
    } entity;
} character;

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
    character charMap[floorMaxY][floorMaxX];
    room *roomList;
    stair *stairListU;
    stair *stairListD;
    int16_t numRooms;
    int16_t numUStairs;
    int16_t numDStairs;
    int numMon;
} dungeon; 

/*****************************************
 *             Prototypes                *
 *****************************************/
void runGame(dungeon *d);
character *findPC(dungeon *d);
char *nameMonst();
void makeMonstList(dungeon *d);
void terminalInit();
void gameGen(dungeon *d);
void corridorGen(dungeon *d);
void borderGen(dungeon *d);
void roomGen(dungeon *d);
void staircaseGen(dungeon *d);
void playerGen(dungeon *d);
void monsterGen(dungeon *d);
void gameRunner(dungeon *d);
void movePC(dungeon *d, int yOff, int xOff);
void winGame();
void loseGame();
void dijkstra(dungeon *d, char str[]);
char *findFilePath();
void saveGame(FILE *f, dungeon d);
void loadGame(FILE *f, dungeon *d);
void printGame(dungeon *d);
void printMap(dungeon *d);
void dungeonDelete(dungeon *d);

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
    //bool gameSaved = false;
    bool monSet = false;
    //FILE *f;

    //printf("%s\n", findFilePath());
    if(argc >= 2){
        for(i = 1; i < argc; i++){
            /* if(!strcmp(argv[i], "--load")){
                if(!(f = fopen(findFilePath(), "rb"))){ //"/cygdrive/u/spring2021/COMS 327/Homework 1.02/CS327-Assignment1/samples/welldone.rlg327"
                    //fprintf(stderr, "Failed to open file for reading");
                    return -1;
                }
                loadGame(f, &d);
                //gameLoaded = true;
                break;
            } */
            /* if(!strcmp(argv[i], "--save")){
                if(!(f = fopen(findFilePath(), "wb"))){
                    //fprintf(stderr, "Failed to open file for writing");
                    return -1;
                }
                gameSaved = true;
                break;
            } */
            if(!strcmp(argv[i], "--nummon")){
                if(argv[i + 1] == NULL || !strcmp(argv[i + 1], "--load") || !strcmp(argv[i + 1], "--save")){
                    //fprintf(stderr, "--nummon doesn't have an entered value");
                    exit(1);
                }
                // Finding number of characters and allocating memory
                monSet = true;
                d.numMon = atoi(argv[i + 1]);
                break;
            }
        }
    }
   
    if (!monSet) {
        d.numMon = defaultMonNum;
    } 

    if(!gameLoaded){
        gameGen(&d);
    }
    runGame(&d);

    /* if(gameSaved){
        saveGame(&d);
    } */
    
    dungeonDelete(&d);
    return 0;
}

/*****************************************
 *               Run Game                *
 *****************************************/
void runGame(dungeon *d)
{
    character *pc;
    int entityCount, y, x, key;

    while(1){
        key = getch();
        entityCount = 0;
        printGame(d);
        switch (key) {
            case KEY_HOME:                  // 
                mvprintw(0, 0, "Home");
                refresh();
                continue;
            case KEY_UP:                    //
                mvprintw(0, 0, "Up");
                refresh();
                continue;
            case KEY_PPAGE:                 //
                mvprintw(0, 0, "PPage");
                refresh();
                continue;
            case KEY_RIGHT:                 //
                mvprintw(0, 0, "Right");
                refresh();
                continue;
            case KEY_NPAGE:                 // 
                mvprintw(0, 0, "NPage");
                refresh();
                continue;
            case KEY_DOWN:                  // Down
                mvprintw(0, 0, "Down");
                refresh();
                continue;
            case KEY_END:                   //
                mvprintw(0, 0, "End");
                refresh();
                continue;
            case KEY_LEFT:                  // Left
                mvprintw(0, 0, "Left");
                refresh();
                continue;
            case KEY_B2:                    //
                mvprintw(0, 0, "B2");
                refresh();
                continue;
            case ' ':                       // Rest 
                
                break;
            case '>':                       // Go Down Stairs
                pc = findPC(d);
                if(d->floor[pc->y][pc->x] == downChar) {
                    //dungeonDelete(d);   
                    gameGen(d);
                    break;
                }
                mvprintw(0, 0, "You have to be on a down staircase to do that!");
                refresh();
                break;
            case '<':                       // Go Up Stairs
                pc = findPC(d);
                if(d->floor[pc->y][pc->x] == upChar) {
                    //dungeonDelete(d);   
                    gameGen(d);
                    break;
                }
                mvprintw(0, 0, "You have to be on an up staircase to do that!");
                refresh();
                break;
            case '.':                       // Rest
                
                break;
            case '1':                       //
                movePC(d, 1, -1);
                break;
            case '2':                       //
                movePC(d, 1, 0);
                break;
            case '3':                       //
                movePC(d, 1, 1);
                break;
            case '4':                       //
                movePC(d, 0, -1);
                break;
            case '5':                       // Rest
                
                break;
            case '6':                       //
                movePC(d, 0, 1);
                break;
            case '7':                       //
                movePC(d, -1, -1);
                break;
            case '8':                       //
                movePC(d, -1, 0);
                break;
            case '9':                       //
                movePC(d, -1, 1);
                break;
            case 'b':                       //
                movePC(d, 1, -1);
                break;
            case 'c':                       //
                mvprintw(0, 0, "c");
                refresh();
                continue;
            case 'd':                       //
                mvprintw(0, 0, "d");
                refresh();
                continue;
            case 'e':
                mvprintw(0, 0, "e");
                refresh();
                continue;
            case 'f':
                mvprintw(0, 0, "f");
                refresh();
                continue;
            case 'g':
                mvprintw(0, 0, "g");
                refresh();
                continue;
            case 'h':
                movePC(d, 0, -1);
                break;
            case 'i':
                mvprintw(0, 0, "i");
                refresh();
                continue;
            case 'j':
                movePC(d, 1, 0);
                break;
            case 'k':
                movePC(d, -1, 0);
                break;
            case 'l':
                movePC(d, 0, 1);
                break;
            case 'm':
                makeMonstList(d);
                clear();
                printGame(d);
                continue;
            case 'n':
                movePC(d, 1, 1);
                break;
            case 's':
                mvprintw(0, 0, "s");
                refresh();
                continue;
            case 't':
                mvprintw(0, 0, "t");
                refresh();
                continue;
            case 'u':
                movePC(d, -1, 1);
                break;
            case 'w':
                mvprintw(0, 0, "w");
                refresh();
                continue;
            case 'x':
                mvprintw(0, 0, "x");
                refresh();
                continue;
            case 'y':
                movePC(d, -1, -1);
                break;
            case 'D':
                mvprintw(0, 0, "D");
                refresh();
                continue;
            case 'E':
                mvprintw(0, 0, "E");
                refresh();
                continue;
            case 'H':
                mvprintw(0, 0, "H");
                refresh();
                continue;
            case 'I':
                mvprintw(0, 0, "I");
                refresh();
                continue;
            case 'L':
                mvprintw(0, 0, "L");
                refresh();
                continue;
            case 'Q':                   // Quit
                endwin();
                return;
                break;
            case 'T':
                mvprintw(0, 0, "T");
                refresh();
                continue;
            default:
                mvprintw(23, 1, "Unknown key: %o", key);
                refresh();
                continue;
        }
        gameRunner(d);
        //check for pc and monsters
        for(y = 0; y < floorMaxY; y++){
            for(x = 0; x < floorMaxX; x++){
                if(d->charMap[y][x].isPC){
                    if(!d->charMap[y][x].isAlive) {
                        loseGame();
                        return;
                    }
                }
                if(d->charMap[y][x].isAlive){
                    entityCount++;
                }
            }
        }
        if(entityCount <= 1){
            winGame();
            return;
        }
        printGame(d);
        //usleep(10000 * 60);
        dijkstra(d, "non-tunneling");
        dijkstra(d, "tunneling");
    }
    endwin();
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
            return "deadly";
        case 1: 
            return "forsaken";
        case 2:
            return "lovable";
        case 3:        
            return "puny";
        case 4:
            return "ridiculous";
        case 5:
            return "shocking";
        case 6:     
            return "shy";
        case 7:      
            return "almighty";
        case 8:            
            return "brash";
        case 9:            
            return "bored";
        case 10:             
            return "aggressive";
        case 11:             
            return "charming";
        case 12:            
            return "jaded";
        case 13:             
            return "furious";
        case 14:             
            return "weak";
        case 15:            
            return "clever";
        default:
            return "error";
    }
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
    char **list = malloc(numAlive * sizeof(char *));
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
                nors = (pc->y - d->charMap[y][x].y > 0) ? "North" : "South";
                eorw = (pc->x - d->charMap[y][x].x > 0) ? "West" : "East";
                list[i] = malloc(15 + 2 + 2 + 5 + 4 + 11 + 1); //yDiff: 2, xDiff: 2, nors: 5, eorw: 4, spaces: 11, null term 1 
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

/*****************************************
 *          nCurses Generator            *
 *****************************************/
void terminalInit() 
{
    initscr();
    raw();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
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
    dijkstra(d, "non-tunneling");
    dijkstra(d, "tunneling");
    monsterGen(d);
    terminalInit();
    printGame(d);
    refresh();
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
 *          Monster Generator            *
 *****************************************/
void monsterGen(dungeon *d) {
    int i, ranY, ranX, ran;
    bool placed;
    
    for (i = 0; i < d->numMon; i++) {
        ran = rand() % (d->numRooms - 1); // Room Monster is placed in (Not player's)
        placed = false;
        int j, roomSpace;
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
 *           Monster Compare             *
 *****************************************/
static int32_t monster_cmp(const void *first, const void *second) {
    character *f = (character *) first;
    character *s = (character *) second;
    if (f->nTurn == s->nTurn) {
        return f->sequenceNum - s->sequenceNum; // Tie-breaker
    } else {
        return f->nTurn - s->nTurn;
    }
}

/*****************************************
 *             Game Runnner              *
 *****************************************/
void gameRunner(dungeon *d)
{
    character *c;
    heap_t h; 
    int y, x, i;
    int ran, ran2;
    int oldY, oldX;

    heap_init(&h, monster_cmp, NULL); 
   
    for(y = 0; y < floorMaxY; y++){
        for(x = 0; x < floorMaxX; x++){
            if(d->charMap[y][x].isPC && d->charMap[y][x].isAlive) {
                d->charMap[y][x].hn = heap_insert(&h, &d->charMap[y][x]);
            }
            else 
                d->charMap[y][x].hn = NULL;
        }
    }

    for(y = 0; y < floorMaxY; y++){
        for(x = 0; x < floorMaxX; x++){
            if(d->charMap[y][x].isAlive && !d->charMap[y][x].isPC) {
                d->charMap[y][x].hn = heap_insert(&h, &d->charMap[y][x]);
            }
            else 
                d->charMap[y][x].hn = NULL;
        }
    }

    while((c = heap_remove_min(&h))){
        //printf("x:%d y:%d speed:%d nTurn:%d, sNum:%d type:%x alive:%d\n", c->x, c->y, c->speed, c->nTurn, c->sequenceNum, c->entity.nonPlayer.type, c->isAlive);
        c->hn = NULL;
        bool isSmart = false;
        bool isTele = false;
        bool isTun = false;
        bool isErat = false;
        int realPCY = 0;
        int realPCX = 0;
        int pcRoom = 0;

        if(c->isPC){
            realPCY = c->y;
            realPCX = c->x;
            // Move pc ???
            // Update realPCY/X ?
        } else { // Monster
            if(c->entity.nonPlayer.type & BIT_SMART) // Understand layout, move on shortest path (nowhere if no LoS)
                isSmart = true;
            if(c->entity.nonPlayer.type & BIT_TELE) // Always knows where PC is, always move toward PC
                isTele = true;
            if(c->entity.nonPlayer.type & BIT_TUN) // Tunnel through rock (tunnelling map)
                isTun = true;
            if(c->entity.nonPlayer.type & BIT_ERAT) // 50% chance of moving to random neighboring cell
                isErat = true;
        }

        if(!isSmart){
            c->entity.nonPlayer.knownPCX = 0;
            c->entity.nonPlayer.knownPCY = 0;
        }
        
        if(isTele){
            c->entity.nonPlayer.knownPCX = realPCY;
            c->entity.nonPlayer.knownPCY = realPCX;        
        }
        
        pcRoom = -1;
        for(i = 0; i < d->numRooms; i++){
            for(y = d->roomList[i].cornerY; y < d->roomList[i].sizeY; y++){
                for(x = d->roomList[i].cornerX; x < d->roomList[i].sizeX; x++){
                    if(d->charMap[y][x].isPC)
                        pcRoom = i;
                }
            }
        }
        for(y = d->roomList[pcRoom].cornerY; y < d->roomList[pcRoom].sizeY; y++){
            for(x = d->roomList[pcRoom].cornerX; x < d->roomList[pcRoom].sizeX; x++){
                if(c->x == x && c->y == y){
                    c->entity.nonPlayer.knownPCY = realPCY;
                    c->entity.nonPlayer.knownPCX = realPCX;
                }
            }
        }
        
        int temp = 100;
        int tempY = 0; 
        int tempX = 0;

        if(c->isAlive){
            if(isSmart && isTun){ // Need to do tunneling
                for(y = -1; y < 1; y++){
                    for(x = -1; x < 1; x++){
                        if(temp > d->tunDist[y + c->y][x + c->x]) {
                            temp = d->tunDist[y + c->y][x + c->x];
                            tempY = y + c->y;
                            tempX = x + c->x;
                        }
                    }
                }

                if(isErat && rand() % 2){
                    ran = 1, ran2 = 1;
                    while (ran == 1 || ran2 == 1) {
                        ran = rand() % 3;
                        ran2 = rand() % 3;
                    }
                    if(d->hardness[c->y - 1 + ran][c->x - 1 + ran2] != 255){ // Check walls
                        tempY = c->y - 1 + ran;
                        tempX = c->x - 1 + ran2;
                    }
                } 
                //update hardness
                if(d->hardness[tempY][tempX] == 0){
                    if(c->y != tempY && c->x != tempX){
                        oldY = c->y;
                        oldX = c->x;
                        c->y = tempY;
                        c->x = tempX;
                        d->charMap[tempY][tempX] = *c;
                        d->charMap[oldY][oldX].y = oldY;
                        d->charMap[oldY][oldX].x = oldX;
                        d->charMap[oldY][oldX].speed = 0;
                        d->charMap[oldY][oldX].nTurn = 0;
                        d->charMap[oldY][oldX].isPC = 0;
                        d->charMap[oldY][oldX].isAlive = 0;
                        d->charMap[oldY][oldX].sequenceNum = 0;
                        d->charMap[oldY][oldX].entity.nonPlayer.type = 0;
                        c = &d->charMap[tempY][tempX];
                    }
                }
                else if (d->hardness[tempY][tempX] > 85) {
                    d->hardness[tempY][tempX] = d->hardness[tempY][tempX] - 85;
                    tempY = c->y;
                    tempX = c->x;
                }
                else {
                    d->hardness[tempY][tempX] = 0; 
                    d->floor[tempY][tempX] = corridorChar;
                }
            } else if (isSmart) { // smart non tunneling possibly telepatic       
                for(y = -1; y < 1; y++){
                    for(x = -1; x < 1; x++){
                        if(temp > d->nonTunDist[y + c->y][x + c->x]){
                            temp = d->nonTunDist[y + c->y][x + c->x];
                            tempY = y + c->y;
                            tempX = x + c->x;
                        }
                    }
                }
                if(isErat && rand() % 2){
                    ran = 1, ran2 = 1;
                    while (ran == 1 || ran2 == 1) {
                        ran = rand() % 3;
                        ran2 = rand() % 3;
                    }
                    if(d->hardness[c->y - 1 + ran][c->x - 1 + ran2] == 0){ // Check walls
                        tempY = c->y - 1 + ran;
                        tempX = c->x - 1 + ran2;
                    }
                    if(c->y != tempY && c->x != tempX){
                        oldY = c->y;
                        oldX = c->x;
                        c->y = tempY;
                        c->x = tempX;
                        d->charMap[tempY][tempX] = *c;
                        d->charMap[oldY][oldX].y = oldY;
                        d->charMap[oldY][oldX].x = oldX;
                        d->charMap[oldY][oldX].speed = 0;
                        d->charMap[oldY][oldX].nTurn = 0;
                        d->charMap[oldY][oldX].isPC = 0;
                        d->charMap[oldY][oldX].isAlive = 0;
                        d->charMap[oldY][oldX].sequenceNum = 0;
                        d->charMap[oldY][oldX].entity.nonPlayer.type = 0;
                        c = &d->charMap[tempY][tempX];
                    }
                }

            } else if (isTun) { // Not smart (No Memory), Tunnling possibly telepathic
                //gets "straight" direction toward known player location
                if(c->entity.nonPlayer.knownPCY > c->y){ 
                    tempY = c->y + 1;
                } else if(c->entity.nonPlayer.knownPCY < c->y && c->entity.nonPlayer.knownPCY != 0){
                    tempY = c->y - 1;
                } else{
                    tempY = c->y;
                }
                if(c->entity.nonPlayer.knownPCX > c->x){ 
                    tempX = c->x + 1;
                } else if(c->entity.nonPlayer.knownPCX < c->x && c->entity.nonPlayer.knownPCX != 0){
                    tempX = c->x - 1;
                } else{
                    tempX = c->x;
                }
                
                if(isErat && rand() % 2){
                    ran = 1, ran2 = 1;
                    while (ran == 1 || ran2 == 1) {
                        ran = rand() % 3;
                        ran2 = rand() % 3;
                    }
                    if(d->hardness[c->y - 1 + ran][c->x - 1 + ran2] != 255){ // Check walls
                        tempY = c->y - 1 + ran;
                        tempX = c->x - 1 + ran2;
                    }
                } 
                //update hardness
                if(d->hardness[tempY][tempX] == 0){
                    if(c->y != tempY && c->x != tempX){
                        oldY = c->y;
                        oldX = c->x;
                        c->y = tempY;
                        c->x = tempX;
                        d->charMap[tempY][tempX] = *c;
                        d->charMap[oldY][oldX].y = oldY;
                        d->charMap[oldY][oldX].x = oldX;
                        d->charMap[oldY][oldX].speed = 0;
                        d->charMap[oldY][oldX].nTurn = 0;
                        d->charMap[oldY][oldX].isPC = 0;
                        d->charMap[oldY][oldX].isAlive = 0;
                        d->charMap[oldY][oldX].sequenceNum = 0;
                        d->charMap[oldY][oldX].entity.nonPlayer.type = 0;
                        c = &d->charMap[tempY][tempX];
                    }
                }
                else if (d->hardness[tempY][tempX] > 85 && d->hardness[tempY][tempX] != 255) {
                    d->hardness[tempY][tempX] = d->hardness[tempY][tempX] - 85;
                    tempY = c->y;
                    tempX = c->x;
                }
                else {
                    d->hardness[tempY][tempX] = 0; 
                    d->floor[tempY][tempX] = corridorChar;
                }
            } else { // Not Smart (No memory), Not Tunneling, possibly telepathic
                if(c->entity.nonPlayer.knownPCY > c->y){ 
                    tempY = c->y + 1;
                } else if(c->entity.nonPlayer.knownPCY < c->y && c->entity.nonPlayer.knownPCY != 0){
                    tempY = c->y - 1;
                } else {
                    tempY = c->y;
                }
                if(c->entity.nonPlayer.knownPCX > c->x){ 
                    tempX = c->x + 1;
                } else if(c->entity.nonPlayer.knownPCX < c->x && c->entity.nonPlayer.knownPCX != 0){
                    tempX = c->x - 1;
                } else {
                    tempX = c->x;
                }

                if(isErat && rand() % 2){
                    ran = 1, ran2 = 1;
                    while (ran == 1 || ran2 == 1) {
                        ran = rand() % 3;
                        ran2 = rand() % 3;
                    }
                    if(d->hardness[c->y - 1 + ran][c->x - 1 + ran2] == 0){
                        tempY = c->y - 1 + ran;
                        tempX = c->x - 1 + ran2;
                    }
                }
                if(c->y != tempY && c->x != tempX){
                    oldY = c->y;
                    oldX = c->x;
                    c->y = tempY;
                    c->x = tempX;
                    d->charMap[tempY][tempX] = *c;
                    d->charMap[oldY][oldX].y = oldY;
                    d->charMap[oldY][oldX].x = oldX;
                    d->charMap[oldY][oldX].speed = 0;
                    d->charMap[oldY][oldX].nTurn = 0;
                    d->charMap[oldY][oldX].isPC = 0;
                    d->charMap[oldY][oldX].isAlive = 0;
                    d->charMap[oldY][oldX].sequenceNum = 0;
                    d->charMap[oldY][oldX].entity.nonPlayer.type = 0;
                    c = &d->charMap[tempY][tempX];
                }
            }
 
            if(c->isAlive){
                //printf("x:%d y:%d speed:%d nTurn:%d, sNum:%d type:%x alive:%d\n", c->x, c->y, c->speed, c->nTurn, c->sequenceNum, c->entity.nonPlayer.type, c->isAlive);
                if(c->isPC) {
                     c->nTurn = c->nTurn + (1000 / c->speed);
                    heap_insert(&h, c);
                    return;
                } else {
                    heap_insert(&h, c);
                     c->nTurn = c->nTurn + (1000 / c->speed);
                }
            }
        }
    }
}

/*****************************************
 *            Monster Mover              *
 *****************************************/
void moveMonst()
{
    //implement to shorted the monster ai.
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
 *        Corridor Path Compare          *
 *****************************************/
static int32_t corridor_path_cmp(const void *key, const void *with) 
{
    return ((corPath *) key)->cost - ((corPath *) with)->cost;
}

/*****************************************
 *              Game Win                 *
 *****************************************/
void winGame()
{
    mvprintw(23, 35, "YOU WIN");
}

/*****************************************
 *               Game Lose               *
 *****************************************/
void loseGame()
{
    mvprintw(23, 35, "YOU LOSE");
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

    for (y = 0; y < floorMaxY; y++) {
        for (x = 0; x < floorMaxX; x++) {
            if(d->floor[y][x] == playerChar){
                d->path[y][x].cost = 0;
            }
        }
    }

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
    
    if(!strcmp(str, "non-tunneling")){
        for (y = 0; y < floorMaxY; y++) {
            for (x = 0; x < floorMaxX; x++) {
                if(d->hardness[y][x] == 0) // Room or corridor
                    d->nonTunDist[y][x] = d->path[y][x].cost;
                else 
                    d->nonTunDist[y][x] = 255;
            }
        }
    } else {
        for (y = 0; y < floorMaxY; y++) {
            for (x = 0; x < floorMaxX; x++) {
                d->tunDist[y][x] = d->path[y][x].cost;
            }
        }
    }
    free(p);
    //printMap(d);
}

/*****************************************
 *            Swap Character             *
 *****************************************/
void characterSwap(character *old, character *new)
{
    character *tmp;

    tmp = old; 
    old = new;
    new = tmp;

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
    int y, x;
    for (y = 0; y < floorMaxY; y++) {
        for (x = 0; x < floorMaxX; x++) {
            if (d.charMap[y][x].isPC) {
                fwrite(&d.charMap[y][x].x, 1, 1, f);
                fwrite(&d.charMap[y][x].y, 1, 1, f);
            }
        }
    }
    
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
    int y, x;
    for (y = 0; y < floorMaxY; y++) {
        for (x = 0; x < floorMaxX; x++) {
            if (d->charMap[y][x].isPC) {
                fread(&d->charMap[y][x].x, 1, 1, f);
                fread(&d->charMap[y][x].y, 1, 1, f);
                d->floor[d->charMap[y][x].y][d->charMap[y][x].x] = playerChar;
            }
        }
    }
   
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
    int y, x;
    for(y = 0; y < floorMaxY; y++){ 
        for(x = 0; x < floorMaxX; x++){
            if (d->charMap[y][x].isAlive && !d->charMap[y][x].isPC) {
                //printf("%x", d->charMap[y][x].entity.nonPlayer.type);
                mvprintw(1 + y, x, "%x", d->charMap[y][x].entity.nonPlayer.type);
            } else if (d->charMap[y][x].isPC && d->charMap[y][x].isAlive) {
                mvprintw(1 + y, x, "%c", playerChar);
                //printf("%c", playerChar);
            } else {
                //printf("%c", d->floor[y][x]);
                mvprintw(1 + y, x, "%c", d->floor[y][x]);
            }
        }
        //printf("\n");
    }
    //printf("\n");
    refresh();
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
void dungeonDelete(dungeon *d)
{
    free(d->roomList);
    free(d->stairListU);
    free(d->stairListD);
}