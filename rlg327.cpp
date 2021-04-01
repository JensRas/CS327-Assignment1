/********************************************************
 * @authors Thomas McCoy, Jens Rasmussen, and Jason Guo *
 ********************************************************/
#include <cstdlib>
#include <cstring>
#include <cstdbool>
#include <cstdio>
#include <ctime>
#include <unistd.h>
#include <fstream>

#include "dungeon.h"
#include "move.h"
#include "printers.h"
#include "parsers.h"

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
    //memset(&d, 0, sizeof(d));

    // /*

    std::fstream f (findFilePath(0, "monster_desc.txt"), f.in);
    if(!f.is_open()){
        fprintf(stderr, "Failed to open monster file for reading");
        return -1;
    } else {
        parseMonFile(f, &d);
        printf("\n******************************MONSTERS******************************\n\n");
        printMonDef(&d);
        f.close();
    }
    f.open(findFilePath(0, "object_desc.txt"), f.in);
    if(!f.is_open()){
        fprintf(stderr, "Failed to open object file for reading");
        return -1;
    } else {
        parseObjFile(f, &d);
        printf("\n******************************OBJECTS******************************\n\n");
        printObjDef(&d);
        f.close();
        return 0;
    }

    // */

    if(argc >= 2){
        for(i = 1; i < argc; i++){
            if(!strcmp(argv[i], "--nummon")){
                if(argv[i + 1] == NULL){
                    fprintf(stderr, "--nummon doesn't have an entered value");
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