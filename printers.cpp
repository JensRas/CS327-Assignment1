#include <cstdio>
#include <cstdlib>
#include <string>
#include <ncurses.h>
#include <iostream>

#include "printers.h"
#include "dungeon.h"
#include "pc.h"
#include "npc.h"
#include "parsers.h"

/*****************************************
 *            Game Printer               *
 *****************************************/
void printGame(dungeon *d, bool fog)
{
    int y, x;
    for(y = 0; y < floorMaxY; y++){ 
        for(x = 0; x < floorMaxX; x++){
            if (fog) {
                mvprintw(1 + y, x, "%c", d->fogMap[y][x]);
            } else {
                if (d->charMap[y][x]->isAlive && !d->charMap[y][x]->isPC) {
                    //attron(COLOR_PAIR(COLOR_X))
                    mvprintw(1 + y, x, "%x", d->charMap[y][x]->entity.nonPlayer.type); // needs to print new symbols
                    //attroff(COLOR_PAIR(COLOR_X))
                } else if (d->charMap[y][x]->isPC && d->charMap[y][x]->isAlive) {
                    //attron(COLOR_PAIR(COLOR_X))
                    mvprintw(1 + y, x, "%c", playerChar);
                    //attroff(COLOR_PAIR(COLOR_X))
                } else if (!d->itemMap[y][x]->name.empty()) {
                    //attron(COLOR_PAIR(COLOR_X))
                    mvprintw(1 + y, x, "%c", d->itemMap[y][x]->type); // needs to print from bitvector
                    //attroff(COLOR_PAIR(COLOR_X))
                } else {
                    mvprintw(1 + y, x, "%c", d->floor[y][x]);
                }
            }
        }
    }
    refresh();
}

/*****************************************
 *             Map Printer               *
 *****************************************/
void printMap(dungeon *d) 
{
    // Needs updating to ncurses
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
 *           File Path Finder            *
 *****************************************/
std::string findFilePath(int x, std::string s)
{   
    std::string home; 
    std::string gameDir = ".rlg327";
    std::string fileName = s; 
    std::string path;
    std::string jens;
    std::string thomas;

    if(x == 0) { // Final Game
        home = getenv("HOME"); 
        //path = (std::string)malloc(home.length() + gameDir.length() + fileName.length() + 2 + 1);
        //sprintf(path, "%s/%s/%s", home, gameDir, fileName); 
        path = home + "/" + gameDir + "/" + fileName;
    } else if(x == 1) { // Thomas'
        home = getenv("HOME");
        thomas = "com327";
        path = home + "/" + thomas + "/" + gameDir + "/" + fileName;
        //path = (std::string)malloc((home.length() + thomas.length() + gameDir.length() + fileName.length() + 3 + 1) * sizeof(char));
        //sprintf(path, "%s/%s/%s/%s", home, thomas, gameDir, fileName);
    } else if(x == 2) { // Jens'
        jens = "/cygdrive/u/spring2021/COMS 327/Homework 1.02/CS327-Assignment1";
        //path = (std::string)malloc(jens.length() + gameDir.length() +fileName.length() + 2 + 1);
        //sprintf(path, "%s/%s/%s", jens, gameDir, fileName); 
        path = jens + "/" + gameDir + "/" + fileName;
    } else {
        //path = (std::string)malloc(10);
        //sprintf(path, "Error");
        path = "ERROR";
    }
    
    return path;
}

/*****************************************
 *       Monster Definition Printer      *
 *****************************************/
void printMonDef(dungeon *d)
{
    int i;
    std::string strColor;
    std::string strAbils;

    for (i = 0; i < (int)d->monDesc.size(); i++) {

        strColor = "";
        strAbils = "";
        // Construct color string from bitvector
        if (d->monDesc[i].color & BIT_BLACK)
            strColor += "BLACK ";
        if (d->monDesc[i].color & BIT_BLUE) 
            strColor += "BLUE ";
        if (d->monDesc[i].color & BIT_GREEN)
            strColor += "GREEN ";
        if (d->monDesc[i].color & BIT_CYAN)
            strColor += "CYAN ";
        if (d->monDesc[i].color & BIT_RED)
            strColor += "RED ";
        if (d->monDesc[i].color & BIT_MAGENTA)
            strColor += "MAGENTA ";
        if (d->monDesc[i].color & BIT_YELLOW)
            strColor += "YELLOW ";
        if (d->monDesc[i].color & BIT_WHITE)
            strColor += "WHITE ";
        // Construct ability string from bitvector
        if (d->monDesc[i].ability & BIT_SMART)
            strAbils += "SMART ";
        if (d->monDesc[i].ability & BIT_TELE) 
            strAbils += "TELE ";
        if (d->monDesc[i].ability & BIT_TUN)
            strAbils += "TUNNEL ";
        if (d->monDesc[i].ability & BIT_ERAT)
            strAbils += "ERRATIC ";
        if (d->monDesc[i].ability & BIT_PASS)
            strAbils += "PASS ";
        if (d->monDesc[i].ability & BIT_PICKUP)
            strAbils += "PICKUP ";
        if (d->monDesc[i].ability & BIT_DESTROY)
            strAbils += "DESTROY ";
        if (d->monDesc[i].ability & BIT_UNIQ)
            strAbils += "UNIQ ";
        if (d->monDesc[i].ability & BIT_BOSS)
            strAbils += "BOSS ";

        // Very long print statement that could probably be written easier. 
        std::cout << d->monDesc[i].name << std::endl << d->monDesc[i].desc 
                  << d->monDesc[i].symbol << std::endl << strColor << std::endl 
                  << d->monDesc[i].speed.base << "+" << d->monDesc[i].speed.numDice 
                  << "d" << d->monDesc[i].speed.sides << std::endl << strAbils
                  << std::endl << d->monDesc[i].health.base << "+" << d->monDesc[i].health.numDice
                  << "d" << d->monDesc[i].health.sides << std::endl << d->monDesc[i].damage.base 
                  << "+" << d->monDesc[i].damage.numDice << "d" << d->monDesc[i].damage.sides
                  << std::endl << (int)d->monDesc[i].rarity << std::endl << std::endl;
    }
}

/*****************************************
 *       Object Definition Printer      *
 *****************************************/
void printObjDef(dungeon *d)
{
    int i;
    std::string strColor;
    std::string strType;

    for (i = 0; i < (int)d->objDesc.size(); i++) {
        // Empties strings
        strColor = "";
        strType = "";
        // Construct color string from bitvector
        if (d->objDesc[i].color & BIT_BLACK)
            strColor += "BLACK ";
        if (d->objDesc[i].color & BIT_BLUE) 
            strColor += "BLUE ";
        if (d->objDesc[i].color & BIT_GREEN)
            strColor += "GREEN ";
        if (d->objDesc[i].color & BIT_CYAN)
            strColor += "CYAN ";
        if (d->objDesc[i].color & BIT_RED)
            strColor += "RED ";
        if (d->objDesc[i].color & BIT_MAGENTA)
            strColor += "MAGENTA ";
        if (d->objDesc[i].color & BIT_YELLOW)
            strColor += "YELLOW ";
        if (d->objDesc[i].color & BIT_WHITE)
            strColor += "WHITE ";
        // Construct type string from bitvector
        if (d->objDesc[i].type & BIT_WEAPON)
            strType += "WEAPON ";
        if (d->objDesc[i].type & BIT_OFFHAND) 
            strType += "OFFHAND ";
        if (d->objDesc[i].type & BIT_RANGED)
            strType += "RANGED ";
        if (d->objDesc[i].type & BIT_ARMOR)
            strType += "ARMOR ";
        if (d->objDesc[i].type & BIT_HELMET)
            strType += "HELMET ";
        if (d->objDesc[i].type & BIT_CLOAK)
            strType += "CLOAK ";
        if (d->objDesc[i].type & BIT_GLOVES)
            strType += "GLOVES ";
        if (d->objDesc[i].type & BIT_BOOTS)
            strType += "BOOTS ";
        if (d->objDesc[i].type & BIT_RING)
            strType += "RING ";
        if (d->objDesc[i].type & BIT_AMULET) 
            strType += "AMULET ";
        if (d->objDesc[i].type & BIT_LIGHT)
            strType += "LIGHT ";
        if (d->objDesc[i].type & BIT_SCROLL)
            strType += "SCROLL ";
        if (d->objDesc[i].type & BIT_BOOK)
            strType += "BOOK ";
        if (d->objDesc[i].type & BIT_FLASK)
            strType += "FLASK ";
        if (d->objDesc[i].type & BIT_GOLD)
            strType += "GOLD ";
        if (d->objDesc[i].type & BIT_AMMUNITION)
            strType += "AMMUNITION ";
        if (d->objDesc[i].type & BIT_FOOD)
            strType += "FOOD ";
        if (d->objDesc[i].type & BIT_WAND)
            strType += "WAND ";
        if (d->objDesc[i].type & BIT_CONTAINER)
            strType += "CONTAINER ";

        // Very long print statement that could probably be written easier. 
        std::cout << d->objDesc[i].name << std::endl << d->objDesc[i].desc << strType << std::endl << strColor << std::endl 
                  << d->objDesc[i].hit.base << "+" << d->objDesc[i].hit.numDice << "d" << d->objDesc[i].hit.sides << std::endl
                  << d->objDesc[i].dam.base << "+" << d->objDesc[i].dam.numDice << "d" << d->objDesc[i].dam.sides << std::endl
                  << d->objDesc[i].dodge.base << "+" << d->objDesc[i].dodge.numDice << "d" << d->objDesc[i].dodge.sides << std::endl
                  << d->objDesc[i].def.base << "+" << d->objDesc[i].def.numDice << "d" << d->objDesc[i].def.sides << std::endl
                  << d->objDesc[i].weight.base << "+" << d->objDesc[i].weight.numDice << "d" << d->objDesc[i].weight.sides << std::endl
                  << d->objDesc[i].speed.base << "+" << d->objDesc[i].speed.numDice << "d" << d->objDesc[i].speed.sides << std::endl
                  << d->objDesc[i].attr.base << "+" << d->objDesc[i].attr.numDice << "d" << d->objDesc[i].attr.sides << std::endl
                  << d->objDesc[i].val.base << "+" << d->objDesc[i].val.numDice << "d" << d->objDesc[i].val.sides << std::endl
                  << d->objDesc[i].art << std::endl << (int)d->objDesc[i].rarity << std::endl << std::endl;
    }
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
    start_color();
}