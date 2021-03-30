#ifndef DUNGEON_H
#define DUNGEON_H

#include <string>
#include <cstring>

#include "heap.h"
#include "pc.h"
#include "npc.h"

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
#define fogVision     5

//typedef struct pc pc;
//typedef struct npc npc;

typedef struct room {
    int8_t cornerX;     // Top left corner
    int8_t cornerY;     // Top left corner
    int8_t sizeX;
    int8_t sizeY;
} room;

typedef struct stair {
    int8_t x;
    int8_t y;
} stair;

typedef struct corPath {
  heap_node_t *hn;
  uint8_t pos[2];
  int32_t cost;
} corPath;

class monDesc {
    public:
        std::string name;
        std::string symbol;
        std::string color;
        std::string speed;
        uint16_t ability;
        std::string health;
        std::string damage;
        std::string desc;
        uint8_t rarity;
};

class character {
    public:
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
};

class dungeon {
    public:
        corPath path[floorMaxY][floorMaxX];
        char floor[floorMaxY][floorMaxX];
        char fogMap[floorMaxY][floorMaxX];
        uint8_t hardness[floorMaxY][floorMaxX];
        uint8_t nonTunDist[floorMaxY][floorMaxX];
        uint8_t tunDist[floorMaxY][floorMaxX];
        character charMap[floorMaxY][floorMaxX];
        monDesc monDesc[defaultMonNum]; // Need to find the correct size for this somehow
        std::string version;
        room *roomList;
        stair *stairListU;
        stair *stairListD;
        int16_t numRooms;
        int16_t numUStairs;
        int16_t numDStairs;
        int numMon;
};

void corridorGen(dungeon *d);
void borderGen(dungeon *d);
void roomGen(dungeon *d);
void staircaseGen(dungeon *d);
void dungeonDelete(dungeon *d);
void gameGen(dungeon *d);

#endif