#include <vector>

#include "item.h"
#include "dice.h"
#include "dungeon.h"
#include "parsers.h"

# define rand_range(min, max) ((rand() % (((max) + 1) - (min))) + (min))

/*****************************************
 *           Item Generator              *
 *****************************************/
item::item(itemDesc &it, int16_t posY, int16_t posX, item *next) : 
    name(it.get_name()),
    desc(it.get_desc()),
    type(it.get_type()),
    color(it.get_color()),
    dam(it.get_dam()),
    hit(it.get_hit().rollDice()),
    dodge(it.get_dodge().rollDice()),
    def(it.get_def().rollDice()),
    weight(it.get_weight().rollDice()),
    speed(it.get_speed().rollDice()),
    attr(it.get_attr().rollDice()),
    val(it.get_val().rollDice()),
    seen(false),
    next(next),
    id(it)

{
    id.generate();
}

item::~item()
{
  id.destroy();
  if (next) {
    delete next;
  }
}

void gen_item(dungeon *d)
{
  item *it;
  uint32_t room;
  int y, x;
  std::vector<itemDesc> &v = d->objDesc;
  int i;

  do {  
    i = rand_range(0, v.size() - 1);
  } while (!v[i].can_be_generated() || !v[i].pass_rarity_roll());
  
  room = rand_range(0, d->numRooms - 1);
  do {
    y = rand_range(d->roomList[room].cornerY,
                          (d->roomList[room].cornerY +
                           d->roomList[room].sizeY - 1));
    x = rand_range(d->roomList[room].cornerX,
                          (d->roomList[room].cornerX +
                           d->roomList[room].sizeX - 1));
  } while (d->floor[y][x] == upChar || d->floor[y][x] == downChar); // > 

  it = new item(v[i], y, x, d->itemMap[y][x]);

  d->itemMap[it->posY][it->posX] = it;
  
}

void gen_items(dungeon *d)
{
  uint32_t i;

  memset(d->itemMap, 0, sizeof (d->itemMap));

  for (i = 0; i < maxItems; i++) {
    gen_item(d);
  }

  d->numItems = maxItems;
}

char item::get_symbol()
{
  return next ? '&' : item_symbol[type];
}

uint32_t item::get_color()
{
  return color;
}

const char *item::get_name()
{
  return name.c_str();
}

int32_t item::get_speed()
{
  return speed;
}

int32_t item::roll_dice()
{
  return dam.rollDice();
}

void destroy_items(dungeon *d)
{
  uint32_t y, x;

  for (y = 0; y < floorMaxY; y++) {
    for (x = 0; x < floorMaxX; x++) {
      if (d->itemMap[y][x]) {
        delete d->itemMap[y][x];
        d->itemMap[y][x] = 0;
      }
    }
  }
}

int32_t item::get_type()
{
  return type;
}