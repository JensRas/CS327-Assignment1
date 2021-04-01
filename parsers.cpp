#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>

#include "dungeon.h"
#include "npc.h"
#include "pc.h"
#include "parsers.h"

/*****************************************
 *            Monster Parser             *
 *****************************************/
int parseMonFile(std::fstream &f, dungeon *d)
{   
    std::string str;
    std::string delimiter = " ";
    std::string s;
    int i = 0;
    bool fail = false;
    size_t pos, prePos;
    std::getline(f, str); 
    if(str != "RLG327 MONSTER DESCRIPTION 1")
        return -1;
    d->monVersion = str;

    while(!f.eof()) {
        fail = false;
        while(std::getline(f, str)){
            if(str == "BEGIN MONSTER")
                break;
        }

        while(std::getline(f, str)) {
            if(str == "END") {
                i++;
                break;
            }
            d->monDesc[i].valid = true;
            // Implement check if it happens twice
            if (str.find("NAME") != std::string::npos) {
                d->monDesc[i].name = str.substr(5);
            } else if (str.find("DESC") != std::string::npos) { // get new line until new line == '.' => lines also can't be longer than 77.
                while(std::getline(f, str)) {
                    if(str == ".")
                        break;
                    if(str.length() >= 78) {
                        fail = true;
                        break;
                    }
                    d->monDesc[i].desc += str + "\n";
                }
                if(fail) {
                    std::getline(f, str); 
                    break;
                }
            } else if (str.find("COLOR") != std::string::npos) {
                str.erase(0, str.find(delimiter) + delimiter.length());
                s = str;
                d->monDesc[i].color = 0;
                while (true) {
                    pos = str.find(delimiter);
                    s = str.substr(0, pos);
                    str.erase(0, pos + delimiter.length());

                    if (s == "BLACK")
                        d->monDesc[i].color |= BIT_BLACK;
                    else if (s == "BLUE") 
                        d->monDesc[i].color |= BIT_BLUE;
                    else if (s == "GREEN")
                        d->monDesc[i].color |= BIT_GREEN;
                    else if (s == "CYAN")
                        d->monDesc[i].color |= BIT_CYAN;
                    else if (s == "RED")
                        d->monDesc[i].color |= BIT_RED;
                    else if (s == "MAGENTA")
                        d->monDesc[i].color |= BIT_MAGENTA;
                    else if (s == "YELLOW")
                        d->monDesc[i].color |= BIT_YELLOW;
                    else if (s == "WHITE")
                        d->monDesc[i].color |= BIT_WHITE;

                    if (pos == std::string::npos)
                        break;                  
                }
            } else if (str.find("SPEED") != std::string::npos) { // Dice
                pos = str.find("+");
                d->monDesc[i].speed.base = stoi(str.substr(6, pos));
                prePos = pos + 1;
                pos = str.find("d");
                d->monDesc[i].speed.numDice = stoi(str.substr(prePos, pos));
                d->monDesc[i].speed.sides = stoi(str.substr((pos + 1)));
            } else if (str.find("ABIL") != std::string::npos) {
                str.erase(0, str.find(delimiter) + delimiter.length());
                s = str;
                d->monDesc[i].ability = 0;
                while (true) {
                    pos = str.find(delimiter);
                    s = str.substr(0, pos);
                    str.erase(0, pos + delimiter.length());

                    if (s == "SMART")
                        d->monDesc[i].ability |= BIT_SMART;
                    else if (s == "TELE") 
                        d->monDesc[i].ability |= BIT_TELE;
                    else if (s == "TUNNEL")
                        d->monDesc[i].ability |= BIT_TUN;
                    else if (s == "ERRATIC")
                        d->monDesc[i].ability |= BIT_ERAT;
                    else if (s == "PASS")
                        d->monDesc[i].ability |= BIT_PASS;
                    else if (s == "PICKUP")
                        d->monDesc[i].ability |= BIT_PICKUP;
                    else if (s == "DESTROY")
                        d->monDesc[i].ability |= BIT_DESTROY;
                    else if (s == "UNIQ")
                        d->monDesc[i].ability |= BIT_UNIQ;
                    else if (s == "BOSS")
                        d->monDesc[i].ability |= BIT_BOSS;

                    if (pos == std::string::npos)
                        break;                  
                }
            } else if (str.find("HP") != std::string::npos) { // Dice
                pos = str.find("+");
                d->monDesc[i].health.base = stoi(str.substr(3, pos));
                prePos = pos + 1;
                pos = str.find("d");
                d->monDesc[i].health.numDice = stoi(str.substr(prePos, pos));
                d->monDesc[i].health.sides = stoi(str.substr((pos + 1)));
            } else if (str.find("DAM") != std::string::npos) { // Dice
                pos = str.find("+");
                d->monDesc[i].damage.base = stoi(str.substr(4, pos));
                prePos = pos + 1;
                pos = str.find("d");
                d->monDesc[i].damage.numDice = stoi(str.substr(prePos, pos));
                d->monDesc[i].damage.sides = stoi(str.substr((pos + 1)));
            } else if (str.find("SYMB") != std::string::npos) {
                d->monDesc[i].symbol = str.substr(5);
            } else if (str.find("RRTY") != std::string::npos) {
                d->monDesc[i].rarity = stoi(str.substr(5));
            } else {
                std::getline(f, str);
                break;
            }
        }
    }
    return 0;
}

/*****************************************
 *             Object Parser             *
 *****************************************/
int parseObjFile(std::fstream &f, dungeon *d)
{   
    std::string str;
    std::string delimiter = " ";
    std::string s;
    int i = 0;
    bool fail = false;
    size_t pos, prePos;
    std::getline(f, str); 
    if(str != "RLG327 OBJECT DESCRIPTION 1")
        return -1;
    d->objVersion = str;

    while(!f.eof()) {
        fail = false;
        while(std::getline(f, str)){
            if(str == "BEGIN OBJECT")
                break;
        }
        while(std::getline(f, str)) {
            if(str == "END") {
                i++;
                break;
            }
            d->objDesc[i].valid = true;
            // Implement check if it happens twice
            if (str.find("NAME") != std::string::npos) {
                d->objDesc[i].name = str.substr(5);
            } else if (str.find("DESC") != std::string::npos) { // get new line until new line == '.' => lines also can't be longer than 77.
                while(std::getline(f, str)) {
                    if(str == ".")
                        break;
                    if(str.length() >= 78) {
                        fail = true;
                        break;
                    }
                    d->objDesc[i].desc += str + "\n";
                }
                if(fail) {
                    std::getline(f, str); 
                    break;
                }
            } else if (str.find("TYPE") != std::string::npos) {
                str.erase(0, str.find(delimiter) + delimiter.length());
                s = str;
                d->objDesc[i].type = 0;
                while (true) {
                    pos = str.find(delimiter);
                    s = str.substr(0, pos);
                    str.erase(0, pos + delimiter.length());

                    if (s == "WEAPON")
                        d->objDesc[i].type |= BIT_WEAPON;
                    else if (s == "OFFHAND") 
                        d->objDesc[i].type |= BIT_OFFHAND;
                    else if (s == "RANGED")
                        d->objDesc[i].type |= BIT_RANGED;
                    else if (s == "ARMOR")
                        d->objDesc[i].type |= BIT_ARMOR;
                    else if (s == "HELMET")
                        d->objDesc[i].type |= BIT_HELMET;
                    else if (s == "CLOAK")
                        d->objDesc[i].type |= BIT_CLOAK;
                    else if (s == "GLOVES")
                        d->objDesc[i].type |= BIT_GLOVES;
                    else if (s == "BOOTS")
                        d->objDesc[i].type |= BIT_BOOTS;
                    else if (s == "RING")
                        d->objDesc[i].type |= BIT_RING;
                    else if (s == "AMULET")
                        d->objDesc[i].type |= BIT_AMULET;
                    else if (s == "LIGHT")
                        d->objDesc[i].type |= BIT_LIGHT;
                    else if (s == "SCROLL")
                        d->objDesc[i].type |= BIT_SCROLL;
                    else if (s == "BOOK")
                        d->objDesc[i].type |= BIT_BOOK;
                    else if (s == "FLASK")
                        d->objDesc[i].type |= BIT_FLASK;
                    else if (s == "GOLD")
                        d->objDesc[i].type |= BIT_GOLD;
                    else if (s == "AMMUNITION")
                        d->objDesc[i].type |= BIT_AMMUNITION;
                    else if (s == "FOOD")
                        d->objDesc[i].type |= BIT_FOOD;
                    else if (s == "WAND")
                        d->objDesc[i].type |= BIT_WAND;
                    else if (s == "CONTAINER")
                        d->objDesc[i].type |= BIT_CONTAINER;                

                    if (pos == std::string::npos)
                        break;                  
                }
            } else if (str.find("COLOR") != std::string::npos) {
                str.erase(0, str.find(delimiter) + delimiter.length());
                s = str;
                d->objDesc[i].color = 0;
                while (true) {
                    pos = str.find(delimiter);
                    s = str.substr(0, pos);
                    str.erase(0, pos + delimiter.length());

                    if (s == "BLACK")
                        d->objDesc[i].color |= BIT_BLACK;
                    else if (s == "BLUE") 
                        d->objDesc[i].color |= BIT_BLUE;
                    else if (s == "GREEN")
                        d->objDesc[i].color |= BIT_GREEN;
                    else if (s == "CYAN")
                        d->objDesc[i].color |= BIT_CYAN;
                    else if (s == "RED")
                        d->objDesc[i].color |= BIT_RED;
                    else if (s == "MAGENTA")
                        d->objDesc[i].color |= BIT_MAGENTA;
                    else if (s == "YELLOW")
                        d->objDesc[i].color |= BIT_YELLOW;
                    else if (s == "WHITE")
                        d->objDesc[i].color |= BIT_WHITE;

                    if (pos == std::string::npos)
                        break;                  
                }
            } else if (str.find("HIT") != std::string::npos) { // Dice
                pos = str.find("+");
                d->objDesc[i].hit.base = stoi(str.substr(4, pos));
                prePos = pos + 1;
                pos = str.find("d");
                d->objDesc[i].hit.numDice = stoi(str.substr(prePos, pos));
                d->objDesc[i].hit.sides = stoi(str.substr((pos + 1)));
            } else if (str.find("DAM") != std::string::npos) { // Dice
                pos = str.find("+");
                d->objDesc[i].dam.base = stoi(str.substr(4, pos));
                prePos = pos + 1;
                pos = str.find("d");
                d->objDesc[i].dam.numDice = stoi(str.substr(prePos, pos));
                d->objDesc[i].dam.sides = stoi(str.substr((pos + 1)));
            } else if (str.find("DODGE") != std::string::npos) { // Dice
                pos = str.find("+");
                d->objDesc[i].dodge.base = stoi(str.substr(6, pos));
                prePos = pos + 1;
                pos = str.find("d");
                d->objDesc[i].dodge.numDice = stoi(str.substr(prePos, pos));
                d->objDesc[i].dodge.sides = stoi(str.substr((pos + 1)));
            } else if (str.find("DEF") != std::string::npos) { // Dice
                pos = str.find("+");
                d->objDesc[i].def.base = stoi(str.substr(4, pos));
                prePos = pos + 1;
                pos = str.find("d");
                d->objDesc[i].def.numDice = stoi(str.substr(prePos, pos));
                d->objDesc[i].def.sides = stoi(str.substr((pos + 1)));
            } else if (str.find("WEIGHT") != std::string::npos) { // Dice
                pos = str.find("+");
                d->objDesc[i].weight.base = stoi(str.substr(7, pos));
                prePos = pos + 1;
                pos = str.find("d");
                d->objDesc[i].weight.numDice = stoi(str.substr(prePos, pos));
                d->objDesc[i].weight.sides = stoi(str.substr((pos + 1)));
            } else if (str.find("SPEED") != std::string::npos) { // Dice
                pos = str.find("+");
                d->objDesc[i].speed.base = stoi(str.substr(6, pos));
                prePos = pos + 1;
                pos = str.find("d");
                d->objDesc[i].speed.numDice = stoi(str.substr(prePos, pos));
                d->objDesc[i].speed.sides = stoi(str.substr((pos + 1)));
            } else if (str.find("ATTR") != std::string::npos) { // Dice
                pos = str.find("+");
                d->objDesc[i].attr.base = stoi(str.substr(5, pos));
                prePos = pos + 1;
                pos = str.find("d");
                d->objDesc[i].attr.numDice = stoi(str.substr(prePos, pos));
                d->objDesc[i].attr.sides = stoi(str.substr((pos + 1)));
            } else if (str.find("VAL") != std::string::npos) { // Dice
                pos = str.find("+");
                d->objDesc[i].val.base = stoi(str.substr(4, pos));
                prePos = pos + 1;
                pos = str.find("d");
                d->objDesc[i].val.numDice = stoi(str.substr(prePos, pos));
                d->objDesc[i].val.sides = stoi(str.substr((pos + 1)));
            } else if (str.find("ART") != std::string::npos) {
                d->objDesc[i].art = str.substr(4);
            } else if (str.find("RRTY") != std::string::npos) {
                d->objDesc[i].rarity = stoi(str.substr(5));
            } else {
                std::getline(f, str);
                break;
            }
        }
    }
    return 0;
}