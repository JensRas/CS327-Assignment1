/********************************************************
 * @authors Thomas McCoy, Jens Rasmussen, and Jason Guo *
 ********************************************************/
#include <cstdlib>
#include <cstring>
#include <cstdbool>
#include <ctime>
#include <unistd.h>

#include "dungeon.h"
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