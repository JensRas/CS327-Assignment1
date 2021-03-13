#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "dungeon.h"
#include "save_load.h"
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