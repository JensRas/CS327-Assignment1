#ifndef ITEM_H
#define ITEM_H

#include <string>

#include "dice.h"
#include "dungeon.h"

class dungeon;

class itemDesc {
    public:
        std::string name;
        std::string desc;
        uint32_t type = 0;
        uint32_t color = 0;
        dice hit;
        dice dam;
        dice dodge;
        dice def;
        dice weight;
        dice speed;
        dice attr;
        dice val;
        bool art;
        uint8_t rarity = 0;
        uint32_t num_generated;
        uint32_t num_found;
        inline bool can_be_generated()
        {
            return !art || (art && !num_generated && !num_found);
        }
        inline bool pass_rarity_roll()
        {
            return rarity > (unsigned) (rand() % 100);
        }
        void set(const std::string &name,
           const std::string &description,
           const uint32_t type,
           const uint32_t color,
           const dice &hit,
           const dice &damage,
           const dice &dodge,
           const dice &defence,
           const dice &weight,
           const dice &speed,
           const dice &attrubute,
           const dice &value,
           const bool artifact,
           const uint32_t rarity);
            std::ostream &print(std::ostream &o);
            /* Need all these accessors because otherwise there is a *
            * circular dependancy that is difficult to get around.  */
            inline const std::string &get_name() const { return name; }
            inline const std::string &get_desc() const { return desc; }
            inline const uint32_t get_type() const { return type; }
            inline const uint32_t get_color() const { return color; }
            inline const dice &get_hit() const { return hit; }
            inline const dice &get_dam() const { return dam; }
            inline const dice &get_dodge() const { return dodge; }
            inline const dice &get_def() const { return def; }
            inline const dice &get_weight() const { return weight; }
            inline const dice &get_speed() const { return speed; }
            inline const dice &get_attr() const { return attr; }
            inline const dice &get_val() const { return val; }
            inline void generate() { num_generated++; }
            inline void destroy() { num_generated--; }
            inline void find() { num_found++; }
};

class item {
    public:
        const std::string &name;
        const std::string &desc;
        uint32_t type = 0;
        uint32_t color = 0;
        int16_t posX;
        int16_t posY;
        const dice &dam;
        int32_t hit, dodge, def, weight, speed, attr, val;
        //const std::string &art;
        uint8_t rarity = 0;
        bool seen;
        item *next;
        itemDesc &id;
        item(itemDesc &it, int16_t posY, int16_t posX, item *next);
        ~item();
        inline int32_t get_damage_base() const
        {
            return dam.get_base();
        }
        inline int32_t get_damage_number() const
        {
            return dam.get_number();
        }
        inline int32_t get_damage_sides() const
        {
            return dam.get_sides();
        }
        char get_symbol();
        uint32_t get_color();
        const char *get_name();
        int rollDice(dice die);
        int32_t get_speed();
        int32_t roll_dice();
        int32_t get_type();
        bool have_seen() { return seen; }
        void has_been_seen() { seen = true; }
        int16_t getPosX() { return posX; }
        int16_t getPosY() { return posY; }
};

void gen_items(dungeon *d);
char get_symbol(item *it);
void destroy_items(dungeon *d);

#endif