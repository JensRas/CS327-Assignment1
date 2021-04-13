#ifndef PARSERS_H
#define PARSERS_H

// Monster Types
#define BIT_SMART       0x1
#define BIT_TELE        0x2
#define BIT_TUN         0x4
#define BIT_ERAT        0x8
#define BIT_PASS        0x10
#define BIT_PICKUP      0x20
#define BIT_DESTROY     0x40
#define BIT_UNIQ        0x80
#define BIT_BOSS        0x100
// Colors - ncurses
#define BIT_BLACK       0x1
#define BIT_BLUE        0x2
#define BIT_GREEN       0x4
#define BIT_CYAN        0x8
#define BIT_RED         0x10
#define BIT_MAGENTA     0x20
#define BIT_YELLOW      0x40
#define BIT_WHITE       0x80
// Object types
/* #define BIT_WEAPON      0x1
#define BIT_OFFHAND     0x2
#define BIT_RANGED      0x4
#define BIT_ARMOR       0x8
#define BIT_HELMET      0x10
#define BIT_CLOAK       0x20
#define BIT_GLOVES      0x40
#define BIT_BOOTS       0x80
#define BIT_RING        0x100
#define BIT_AMULET      0x200
#define BIT_LIGHT       0x400
#define BIT_SCROLL      0x800
#define BIT_BOOK        0x1000
#define BIT_FLASK       0x2000
#define BIT_GOLD        0x4000
#define BIT_AMMUNITION  0x8000
#define BIT_FOOD        0x10000
#define BIT_WAND        0x20000
#define BIT_CONTAINER   0x40000 */

typedef enum item_type {
  itemtype_no_type,
  itemtype_WEAPON,
  itemtype_OFFHAND,
  itemtype_RANGED,
  itemtype_LIGHT,
  itemtype_ARMOR,
  itemtype_HELMET,
  itemtype_CLOAK,
  itemtype_GLOVES,
  itemtype_BOOTS,
  itemtype_AMULET,
  itemtype_RING,
  itemtype_SCROLL,
  itemtype_BOOK,
  itemtype_FLASK,
  itemtype_GOLD,
  itemtype_AMMUNITION,
  itemtype_FOOD,
  itemtype_WAND,
  itemtype_CONTAINER
} item_type_t;

extern const char item_symbol[];

int parseMonFile(std::fstream &f, dungeon *d);
int parseObjFile(std::fstream & f, dungeon *d);
int rollDice(dice die);

#endif