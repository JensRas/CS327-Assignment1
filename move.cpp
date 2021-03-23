#include <cstdint>
#include <cstdlib>
#include <ncurses.h>

#include "move.h"
#include "dungeon.h"
#include "heap.h"
#include "pc.h"
#include "npc.h"
#include "pathfinder.h"
#include "printers.h"

static int32_t monster_cmp(const void *first, const void *second);

/*****************************************
 *             Game Runnner              *
 *****************************************/
void gameRunner(dungeon *d) // This is the problem!!!
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

    while((c = (character*) heap_remove_min(&h))){
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

        // This is probably broken
        if(!isSmart){
            c->entity.nonPlayer.knownPCX = 0;
            c->entity.nonPlayer.knownPCY = 0;
        }
        
        // If PC is in a corridor this breaks
        pcRoom = -1;
        for(i = 0; i < d->numRooms; i++){
            for(y = d->roomList[i].cornerY; y < d->roomList[i].sizeY; y++){
                for(x = d->roomList[i].cornerX; x < d->roomList[i].sizeX; x++){
                    if(d->charMap[y][x].isPC)
                        pcRoom = i;
                }
            }
        }
        if(pcRoom != -1){
            for(y = d->roomList[pcRoom].cornerY; y < d->roomList[pcRoom].sizeY; y++){
                for(x = d->roomList[pcRoom].cornerX; x < d->roomList[pcRoom].sizeX; x++){
                    if(c->x == x && c->y == y){
                        c->entity.nonPlayer.knownPCY = realPCY;
                        c->entity.nonPlayer.knownPCX = realPCX;
                    }
                }
            }
        } else {
            c->entity.nonPlayer.knownPCX = 0;
            c->entity.nonPlayer.knownPCY = 0;
        }

        if(isTele){
            c->entity.nonPlayer.knownPCX = realPCY;
            c->entity.nonPlayer.knownPCY = realPCX;    
        }
        
        int temp = 100;
        int tempY = 0; 
        int tempX = 0;

        if(c->isAlive){
            if(isSmart && isTun){
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
                    updateFog(d);
                    return;
                } else {
                    heap_insert(&h, c);
                     c->nTurn = c->nTurn + (1000 / c->speed);
                }
            }
        }
    }
    heap_delete(&h);
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
            case KEY_HOME:                  // Up Left
                movePC(d, -1, -1);
                break;
            case KEY_UP:                    // Up
                movePC(d, -1, 0);
                break;
            case KEY_PPAGE:                 // Up Right
                movePC(d, -1, 1);
                break;
            case KEY_RIGHT:                 // Right
                movePC(d, 0, 1);
                break;
            case KEY_NPAGE:                 // Down Right
                movePC(d, 1, 1);
                break;
            case KEY_DOWN:                  // Down
                movePC(d, 1, 0);
                break;
            case KEY_END:                   // Down Left
                movePC(d, 1, -1);
                break;
            case KEY_LEFT:                  // Left
                movePC(d, 0, -1);
                break;
            case KEY_B2:                    // Rest
                
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
            case '1':                       // Down Left
                movePC(d, 1, -1);
                break;
            case '2':                       // Down
                movePC(d, 1, 0);
                break;
            case '3':                       // Down Right
                movePC(d, 1, 1);
                break;
            case '4':                       // Left
                movePC(d, 0, -1);
                break;
            case '5':                       // Rest
                
                break;
            case '6':                       // Right
                movePC(d, 0, 1);
                break;
            case '7':                       // Up Left
                movePC(d, -1, -1);
                break;
            case '8':                       // Up
                movePC(d, -1, 0);
                break;
            case '9':                       // Up Right
                movePC(d, -1, 1);
                break;
            case 'b':                       // Down Left
                movePC(d, 1, -1);
                break;
            case 'c':                       // Display Character Info
                mvprintw(0, 0, "c");
                refresh();
                continue;
            case 'd':                       // Drop Item
                mvprintw(0, 0, "d");
                refresh();
                continue;
            case 'e':                       // Display Equipment
                mvprintw(0, 0, "e");
                refresh();
                continue;
            case 'f':                       // Toggle "Fog of War"
                mvprintw(0, 0, "f");
                refresh();
                continue;
            case 'g':                       // Teleport
                mvprintw(0, 0, "g");
                refresh();
                continue;
            case 'h':                       // Left
                movePC(d, 0, -1);
                break;
            case 'i':                       // Display Inventory
                mvprintw(0, 0, "i");
                refresh();
                continue;
            case 'j':                       // Down
                movePC(d, 1, 0);
                break;
            case 'k':                       // Up
                movePC(d, -1, 0);
                break;
            case 'l':                       // Right
                movePC(d, 0, 1);
                break;
            case 'm':                       // Display Monster List
                makeMonstList(d);
                clear();
                printGame(d);
                continue;
            case 'n':                       // Down Right
                movePC(d, 1, 1);
                break;
            case 's':                       // Display the Default Map
                mvprintw(0, 0, "s");
                refresh();
                continue;
            case 't':                       // Take Off Item
                mvprintw(0, 0, "t");
                refresh();
                continue;
            case 'u':                       // Up Right
                movePC(d, -1, 1);
                break;
            case 'w':                       // Wear Item
                mvprintw(0, 0, "w");
                refresh();
                continue;
            case 'x':                       // Expunge Item
                mvprintw(0, 0, "x");
                refresh();
                continue;
            case 'y':                       // Up Left
                movePC(d, -1, -1);
                break;
            case 'D':                       // Display the Non-Tunneling Map
                mvprintw(0, 0, "D");
                refresh();
                continue;
            case 'E':                       // Inspect Equipped Item
                mvprintw(0, 0, "E");
                refresh();
                continue;
            case 'H':                       // Display the Hardness Map
                mvprintw(0, 0, "H");
                refresh();
                continue;
            case 'I':                       // Inspect Inventory Item
                mvprintw(0, 0, "I");
                refresh();
                continue;
            case 'L':                       // Look At Monster
                mvprintw(0, 0, "L");
                refresh();
                continue;
            case 'Q':                       // Quit
                endwin();
                return;
                break;
            case 'T':                       // Display the Tunneling Map
                mvprintw(0, 0, "T");
                refresh();
                continue;
            default:                        // Default
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
        dijkstra(d, 0);
        dijkstra(d, 1);
    }
    endwin();
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
 *             Update Fog                *
 *****************************************/
void updateFog(dungeon *d)
{   
    int y, x;
    character *pc = findPC(d);

    for(y = pc->y - fogVision / 2; y <= pc->y + fogVision / 2; y++){
        for(x = pc->x - fogVision / 2; x <= pc->x + fogVision / 2; x++){
            if(y < 0 || y > floorMaxY || x < 0 || x > floorMaxX)
                continue;
            if(d->charMap[y][x].isAlive && !d->charMap[y][x].isPC){
                char *s = (char*) malloc(1 * sizeof(char*));
                sprintf(s, "%x", d->charMap[y][x].entity.nonPlayer.type);
                d->fogMap[y][x] = *s;
            } else if(d->charMap[y][x].isPC) {
                d->fogMap[y][x] = playerChar;
            } else {
                d->fogMap[y][x] = d->floor[y][x];
            }
        }
    }

}