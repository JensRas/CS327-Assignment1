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
#include "parsers.h"

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
                        c  = moveMonst(d, tempY, tempX, c);
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
                        c  = moveMonst(d, tempY, tempX, c);
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
                        c  = moveMonst(d, tempY, tempX, c);
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
                    c  = moveMonst(d, tempY, tempX, c);
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
    bool fog = true;

    while(1){
        key = getch();
        entityCount = 0;
        printGame(d, fog);
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
                fog = fog ? false : true;
                printGame(d, fog);
                continue;
            case 'g':                       // Teleport
                curs_set(1);
                pc = findPC(d);
                y = pc->y + 1, x = pc->x;
                move(y, x);
                while(1) {
                    refresh();
                    key = getch();
                    if(key == 'g'){
                        movePC(d, y - 1, x, true);
                        curs_set(0);
                        break;
                    } else if (key == 'r'){
                        curs_set(0);
                        movePC(d, (rand() % (floorMaxY - 1)) + 1, (rand() % (floorMaxX - 1)) + 1, true);
                        break;
                    } else if (key == '1' || key == 'b' || key == KEY_END) { // Down Left
                        move(++y, --x);
                        continue;
                    } else if (key == '4' || key == 'h' || key == KEY_LEFT) { // Left
                        move(y, --x);
                        continue;
                    } else if (key == '7' || key == 'y' || key == KEY_HOME) { // Up Left
                        move(--y, --x);
                        continue;
                    } else if (key == '8' || key == 'k' || key == KEY_UP) { // Up
                        move(--y, x);
                        continue;
                    } else if (key == '9' || key == 'u' || key == KEY_PPAGE) { // Up Right
                        move(--y, ++x);
                        continue;
                    } else if (key == '6' || key == 'l' || key == KEY_RIGHT) { // Right
                        move(y, ++x);
                        continue;
                    } else if (key == '3' || key == 'n' || key == KEY_NPAGE) { // Down Right
                        move(++y, ++x);
                        continue;
                    } else if (key == '2' || key == 'j' || key == KEY_DOWN) { // Down
                        move(++y, x);
                        continue;
                    }
                }
                break;
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
                printGame(d, fog);
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

        printGame(d, fog);

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
    character *pc;
    pc = findPC(d);

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