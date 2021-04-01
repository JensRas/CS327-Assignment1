#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

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
    bool fail = false;
    size_t pos, prePos;
    std::getline(f, str); 
    if(str != "RLG327 MONSTER DESCRIPTION 1")
        return -1;
    d->monVersion = str;
    // Checks for end of file
    while(!f.eof()) {
        monDesc monster;
        fail = false;

        while(std::getline(f, str)){
            if(str == "BEGIN MONSTER")
                break;
        }
        while(std::getline(f, str)) {
            if(str == "END") {
                d->monDesc.push_back(monster);
                break;
            }
            // Parses string and checks if empty
            if (str.find("NAME") != std::string::npos && monster.name.empty()) {
                monster.name = str.substr(5);
            // Parses string and checks if empty does internal checks for line size and appends strings along with newline characters 
            } else if (str.find("DESC") != std::string::npos && monster.desc.empty()) { 
                while(std::getline(f, str)) {
                    if(str == ".")
                        break;
                    if(str.length() >= 78) {
                        fail = true;
                        break;
                    }
                    monster.desc += str + "\n";
                }
                if(fail) {
                    std::getline(f, str); 
                    break;
                }
            // Bitvector assinments check first if 0
            } else if (str.find("COLOR") != std::string::npos && monster.color == 0) {
                str.erase(0, str.find(delimiter) + delimiter.length());
                s = str;
                monster.color = 0;
                while (true) {
                    pos = str.find(delimiter);
                    s = str.substr(0, pos);
                    str.erase(0, pos + delimiter.length());

                    if (s == "BLACK")
                        monster.color |= BIT_BLACK;
                    else if (s == "BLUE") 
                        monster.color |= BIT_BLUE;
                    else if (s == "GREEN")
                        monster.color |= BIT_GREEN;
                    else if (s == "CYAN")
                        monster.color |= BIT_CYAN;
                    else if (s == "RED")
                        monster.color |= BIT_RED;
                    else if (s == "MAGENTA")
                        monster.color |= BIT_MAGENTA;
                    else if (s == "YELLOW")
                        monster.color |= BIT_YELLOW;
                    else if (s == "WHITE")
                        monster.color |= BIT_WHITE;

                    if (pos == std::string::npos)
                        break;                  
                }
            } else if (str.find("ABIL") != std::string::npos && monster.ability == 0) {
                str.erase(0, str.find(delimiter) + delimiter.length());
                s = str;
                monster.ability = 0;
                while (true) {
                    pos = str.find(delimiter);
                    s = str.substr(0, pos);
                    str.erase(0, pos + delimiter.length());

                    if (s == "SMART")
                        monster.ability |= BIT_SMART;
                    else if (s == "TELE") 
                        monster.ability |= BIT_TELE;
                    else if (s == "TUNNEL")
                        monster.ability |= BIT_TUN;
                    else if (s == "ERRATIC")
                        monster.ability |= BIT_ERAT;
                    else if (s == "PASS")
                        monster.ability |= BIT_PASS;
                    else if (s == "PICKUP")
                        monster.ability |= BIT_PICKUP;
                    else if (s == "DESTROY")
                        monster.ability |= BIT_DESTROY;
                    else if (s == "UNIQ")
                        monster.ability |= BIT_UNIQ;
                    else if (s == "BOSS")
                        monster.ability |= BIT_BOSS;

                    if (pos == std::string::npos)
                        break;                  
                }
            // Dice object parts checks base if 0 and parses string to get dice "parts"
            } else if (str.find("SPEED") != std::string::npos && monster.speed.base == 0) { 
                pos = str.find("+");
                monster.speed.base = stoi(str.substr(6, pos));
                prePos = pos + 1;
                pos = str.find("d");
                monster.speed.numDice = stoi(str.substr(prePos, pos));
                monster.speed.sides = stoi(str.substr((pos + 1)));
            } else if (str.find("HP") != std::string::npos && monster.health.base == 0) { 
                pos = str.find("+");
                monster.health.base = stoi(str.substr(3, pos));
                prePos = pos + 1;
                pos = str.find("d");
                monster.health.numDice = stoi(str.substr(prePos, pos));
                monster.health.sides = stoi(str.substr((pos + 1)));
            } else if (str.find("DAM") != std::string::npos && monster.damage.base == 0) { 
                pos = str.find("+");
                monster.damage.base = stoi(str.substr(4, pos));
                prePos = pos + 1;
                pos = str.find("d");
                monster.damage.numDice = stoi(str.substr(prePos, pos));
                monster.damage.sides = stoi(str.substr((pos + 1)));
            // Parses string and checks if empty
            } else if (str.find("SYMB") != std::string::npos && monster.symbol.empty()) {
                monster.symbol = str.substr(5);
            // Parses string converts to int and checks if empty
            } else if (str.find("RRTY") != std::string::npos && monster.rarity == 0) {
                monster.rarity = stoi(str.substr(5));
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
    bool fail = false;
    size_t pos, prePos;
    std::getline(f, str); 
    if(str != "RLG327 OBJECT DESCRIPTION 1")
        return -1;
    d->objVersion = str;
    // Checks for end of file
    while(!f.eof()) {
        objDesc obj;
        fail = false;

        while(std::getline(f, str)){
            if(str == "BEGIN OBJECT")
                break;
        }
        while(std::getline(f, str)) {
            if(str == "END") {
                d->objDesc.push_back(obj);
                break;
            }
            // Parses string and checks if empty
            if (str.find("NAME") != std::string::npos && obj.name.empty()) {
                obj.name = str.substr(5);
            // Parses string and checks if empty does internal checks for line size and appends strings along with newline characters
            } else if (str.find("DESC") != std::string::npos && obj.desc.empty()) { 
                while(std::getline(f, str)) {
                    if(str == ".")
                        break;
                    if(str.length() >= 78) {
                        fail = true;
                        break;
                    }
                    obj.desc += str + "\n";
                }
                if(fail) {
                    std::getline(f, str); 
                    break;
                }
            // Bitvector assinments check first if 0
            } else if (str.find("TYPE") != std::string::npos && obj.type == 0) {
                str.erase(0, str.find(delimiter) + delimiter.length());
                s = str;
                obj.type = 0;
                while (true) {
                    pos = str.find(delimiter);
                    s = str.substr(0, pos);
                    str.erase(0, pos + delimiter.length());

                    if (s == "WEAPON")
                        obj.type |= BIT_WEAPON;
                    else if (s == "OFFHAND") 
                        obj.type |= BIT_OFFHAND;
                    else if (s == "RANGED")
                        obj.type |= BIT_RANGED;
                    else if (s == "ARMOR")
                        obj.type |= BIT_ARMOR;
                    else if (s == "HELMET")
                        obj.type |= BIT_HELMET;
                    else if (s == "CLOAK")
                        obj.type |= BIT_CLOAK;
                    else if (s == "GLOVES")
                        obj.type |= BIT_GLOVES;
                    else if (s == "BOOTS")
                        obj.type |= BIT_BOOTS;
                    else if (s == "RING")
                        obj.type |= BIT_RING;
                    else if (s == "AMULET")
                        obj.type |= BIT_AMULET;
                    else if (s == "LIGHT")
                        obj.type |= BIT_LIGHT;
                    else if (s == "SCROLL")
                        obj.type |= BIT_SCROLL;
                    else if (s == "BOOK")
                        obj.type |= BIT_BOOK;
                    else if (s == "FLASK")
                        obj.type |= BIT_FLASK;
                    else if (s == "GOLD")
                        obj.type |= BIT_GOLD;
                    else if (s == "AMMUNITION")
                        obj.type |= BIT_AMMUNITION;
                    else if (s == "FOOD")
                        obj.type |= BIT_FOOD;
                    else if (s == "WAND")
                        obj.type |= BIT_WAND;
                    else if (s == "CONTAINER")
                        obj.type |= BIT_CONTAINER;                

                    if (pos == std::string::npos)
                        break;                  
                }
            } else if (str.find("COLOR") != std::string::npos && obj.color == 0) {
                str.erase(0, str.find(delimiter) + delimiter.length());
                s = str;
                obj.color = 0;
                while (true) {
                    pos = str.find(delimiter);
                    s = str.substr(0, pos);
                    str.erase(0, pos + delimiter.length());

                    if (s == "BLACK")
                        obj.color |= BIT_BLACK;
                    else if (s == "BLUE") 
                        obj.color |= BIT_BLUE;
                    else if (s == "GREEN")
                        obj.color |= BIT_GREEN;
                    else if (s == "CYAN")
                        obj.color |= BIT_CYAN;
                    else if (s == "RED")
                        obj.color |= BIT_RED;
                    else if (s == "MAGENTA")
                        obj.color |= BIT_MAGENTA;
                    else if (s == "YELLOW")
                        obj.color |= BIT_YELLOW;
                    else if (s == "WHITE")
                        obj.color |= BIT_WHITE;

                    if (pos == std::string::npos)
                        break;                  
                }
            // Dice object parts checks base if 0 and parses string to get dice "parts"
            } else if (str.find("HIT") != std::string::npos && obj.hit.base == 0) { 
                pos = str.find("+");
                obj.hit.base = stoi(str.substr(4, pos));
                prePos = pos + 1;
                pos = str.find("d");
                obj.hit.numDice = stoi(str.substr(prePos, pos));
                obj.hit.sides = stoi(str.substr((pos + 1)));
            } else if (str.find("DAM") != std::string::npos && obj.dam.base == 0) { 
                pos = str.find("+");
                obj.dam.base = stoi(str.substr(4, pos));
                prePos = pos + 1;
                pos = str.find("d");
                obj.dam.numDice = stoi(str.substr(prePos, pos));
                obj.dam.sides = stoi(str.substr((pos + 1)));
            } else if (str.find("DODGE") != std::string::npos && obj.dodge.base == 0) { 
                pos = str.find("+");
                obj.dodge.base = stoi(str.substr(6, pos));
                prePos = pos + 1;
                pos = str.find("d");
                obj.dodge.numDice = stoi(str.substr(prePos, pos));
                obj.dodge.sides = stoi(str.substr((pos + 1)));
            } else if (str.find("DEF") != std::string::npos && obj.def.base == 0) { 
                pos = str.find("+");
                obj.def.base = stoi(str.substr(4, pos));
                prePos = pos + 1;
                pos = str.find("d");
                obj.def.numDice = stoi(str.substr(prePos, pos));
                obj.def.sides = stoi(str.substr((pos + 1)));
            } else if (str.find("WEIGHT") != std::string::npos && obj.weight.base == 0) { 
                pos = str.find("+");
                obj.weight.base = stoi(str.substr(7, pos));
                prePos = pos + 1;
                pos = str.find("d");
                obj.weight.numDice = stoi(str.substr(prePos, pos));
                obj.weight.sides = stoi(str.substr((pos + 1)));
            } else if (str.find("SPEED") != std::string::npos && obj.speed.base == 0) { 
                pos = str.find("+");
                obj.speed.base = stoi(str.substr(6, pos));
                prePos = pos + 1;
                pos = str.find("d");
                obj.speed.numDice = stoi(str.substr(prePos, pos));
                obj.speed.sides = stoi(str.substr((pos + 1)));
            } else if (str.find("ATTR") != std::string::npos && obj.attr.base == 0) { 
                pos = str.find("+");
                obj.attr.base = stoi(str.substr(5, pos));
                prePos = pos + 1;
                pos = str.find("d");
                obj.attr.numDice = stoi(str.substr(prePos, pos));
                obj.attr.sides = stoi(str.substr((pos + 1)));
            } else if (str.find("VAL") != std::string::npos && obj.val.base == 0) { 
                pos = str.find("+");
                obj.val.base = stoi(str.substr(4, pos));
                prePos = pos + 1;
                pos = str.find("d");
                obj.val.numDice = stoi(str.substr(prePos, pos));
                obj.val.sides = stoi(str.substr((pos + 1)));
            // Parses string and checks if empty
            } else if (str.find("ART") != std::string::npos && obj.art.empty()) {
                obj.art = str.substr(4);
            // Parses string and checks if empty then converts to int
            } else if (str.find("RRTY") != std::string::npos && obj.rarity == 0) {
                obj.rarity = stoi(str.substr(5));
            } else {
                std::getline(f, str);
                break;
            }
        }
    }
    return 0;
}