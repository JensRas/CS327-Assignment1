#ifndef DUNGEON_H
#define DUNGEON_H

#include <string>
#include <cstring>
#include <vector>

#include "heap.h"
#include "pc.h"
#include "npc.h"
#include "item.h"
#include "dice.h"

#define minRoomNumber       6
#define maxRoomNumber      10
#define maxStairNum         3        // Arbitrary value that can be changed
#define maxRoomSize        10        // Arbitrary value that can be changed
#define minRoomX            4
#define minRoomY            3
#define floorMaxX          80
#define floorMaxY          21
#define fogVision           5
#define maxItems           10

#define dimY                0
#define dimX                1

#define edgeChar           '#'        //Hardness = 255
#define roomChar           ' '        //Hardness = 0
#define corridorChar       'o'        //Hardness = 0
#define rockChar           '.'        //Hardness = 100 (Non-zero, non-255)
#define upChar             '<'        //Hardness = 0
#define downChar           '>'        //Hardness = 0
#define playerChar         '@'

#define invalidChar        '*'
#define weaponChar         '|'
#define offhandChar        ')'
#define rangedChar         '}'
#define armorChar          '['
#define helmetChar         ']'
#define cloakChar          '('
#define glovesChar         '{'
#define bootsChar          '\\'
#define ringChar           '='
#define amuletChar         '"'
#define lightChar          '_'
#define scrollChar         '~'
#define bookChar           '?'
#define flaskChar          '!'
#define goldChar           '$'
#define ammunitionChar     '/'
#define foodChar           ','
#define wandChar           '-'
#define containerChar      '%'
#define stackChar          '&'

typedef int16_t pair_t[2];
class item;
class monDesc1;
class itemDesc;

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

class character {
    public:
        virtual ~character() {}
        heap_node_t *hn;
        int8_t x;
        int8_t y;
        char symbol;
        int32_t color;
        int32_t speed;
        int nTurn;
        int8_t isPC;
        int32_t isAlive;
        int32_t hp;
        const dice *dam;
        const char *name;
        int32_t sequenceNum;
        union Entity {
            Entity();
            ~Entity();
            pc player;
            npc nonPlayer;
        } entity;
        inline char get_symbol() { return symbol; }
};

class dungeon {
    public:
        dungeon() : floor{rockChar}, hardness{0}, nonTunDist{0}, 
                    tunDist{0}, charMap{0}, monDesc(), objDesc(),
                    numItems(0), numRooms(0), numMon(0) {}
        corPath path[floorMaxY][floorMaxX];
        char floor[floorMaxY][floorMaxX];
        char fogMap[floorMaxY][floorMaxX];
        uint8_t hardness[floorMaxY][floorMaxX];
        uint8_t nonTunDist[floorMaxY][floorMaxX];
        uint8_t tunDist[floorMaxY][floorMaxX];
        character *charMap[floorMaxY][floorMaxX];
        item *itemMap[floorMaxY][floorMaxX];
        std::vector<monDesc1> monDesc;
        std::vector<itemDesc> objDesc;
        std::string monVersion;
        std::string objVersion;
        room *roomList;
        stair *stairListU;
        stair *stairListD;
        int16_t numItems;
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