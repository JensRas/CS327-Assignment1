/********************************************************
 * @authors Thomas McCoy, Jens Rasmussen, and Jason Guo *
 ********************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include "dungeon.h"
#include "save_load.h"
#include "pc.h"
#include "npc.h"
#include "move.h"

/*****************************************
 *                Main                   *
 *****************************************/
int main(int argc, char *argv[])
{
    srand(time(NULL));
    dungeon d;
    d.roomList = NULL;
    d.stairListU = NULL;
    d.stairListD = NULL;
    int i;
    bool monSet = false;
    memset(&d, 0, sizeof d);

    if(argc >= 2){
        for(i = 1; i < argc; i++){
            if(!strcmp(argv[i], "--nummon")){
                if(argv[i + 1] == NULL || !strcmp(argv[i + 1], "--load") || !strcmp(argv[i + 1], "--save")){
                    //fprintf(stderr, "--nummon doesn't have an entered value");
                    exit(1);
                }
                monSet = true;
                d.numMon = atoi(argv[i + 1]);
                break;
            }
        }
    }
   
    if (!monSet) {
        d.numMon = defaultMonNum;
    } 

    gameGen(&d);
    runGame(&d);
    dungeonDelete(&d);

    return 0;
}

/*****************************************
 *          Main w/ Save & Load          *
 *****************************************/
/* int main(int argc, char *argv[])
{
    srand(time(NULL)); // Seed/Random
    dungeon d;
    d.roomList = NULL;
    d.stairListU = NULL;
    d.stairListD = NULL;
    int i;
    bool gameLoaded = false;
    bool gameSaved = false;
    bool monSet = false;
    FILE *f;

    //printf("%s\n", findFilePath());
    if(argc >= 2){
        for(i = 1; i < argc; i++){
            if(!strcmp(argv[i], "--load")){
                if(!(f = fopen(findFilePath(), "rb"))){ //"/cygdrive/u/spring2021/COMS 327/Homework 1.02/CS327-Assignment1/samples/welldone.rlg327"
                    //fprintf(stderr, "Failed to open file for reading");
                    return -1;
                }
                loadGame(f, &d);
                //gameLoaded = true;
                break;
            }
            if(!strcmp(argv[i], "--save")){
                if(!(f = fopen(findFilePath(), "wb"))){
                    //fprintf(stderr, "Failed to open file for writing");
                    return -1;
                }
                gameSaved = true;
                break;
            }
            if(!strcmp(argv[i], "--nummon")){
                if(argv[i + 1] == NULL || !strcmp(argv[i + 1], "--load") || !strcmp(argv[i + 1], "--save")){
                    //fprintf(stderr, "--nummon doesn't have an entered value");
                    exit(1);
                }
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

    if(gameSaved){
        saveGame(&d);
    }
    
    dungeonDelete(&d);
    return 0;
} */