#include <cstdio>
#include <cstdlib>
#include <string>
#include <ncurses.h>
#include <iostream>

#include "printers.h"
#include "dungeon.h"
#include "pc.h"
#include "npc.h"

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
                if (d->charMap[y][x].isAlive && !d->charMap[y][x].isPC) {
                    mvprintw(1 + y, x, "%x", d->charMap[y][x].entity.nonPlayer.type);
                } else if (d->charMap[y][x].isPC && d->charMap[y][x].isAlive) {
                    mvprintw(1 + y, x, "%c", playerChar);
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

    for (i = 0; i < (int)sizeof(d->monDesc) / (int)sizeof (d->monDesc[0]); i++) {
        
        strColor = d->monDesc[i].color.substr(6, d->monDesc[i].color.length());
        strAbils = "";

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

        std::cout << d->monDesc[i].name << std::endl << d->monDesc[i].desc 
                  << d->monDesc[i].symbol << std::endl << strColor
                  << std::endl << d->monDesc[i].speed << std::endl << strAbils
                  << std::endl << d->monDesc[i].health << std::endl << d->monDesc[i].damage
                  << std::endl << (int)d->monDesc[i].rarity << std::endl << std::endl;
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
}