#ifndef PC_H
# define PC_H

# include <stdint.h>

# include "object.h"
# include "dims.h"
# include "character.h"
# include "dungeon.h"

#define weapon_slot      0
#define offhand_slot     1
#define ranged_slot      2
#define armor_slot       3
#define helmet_slot      4
#define cloak_slot       5
#define gloves_slot      6
#define boots_slot       7
#define amulet_slot      8
#define light_slot       9
#define ring1_slot      10
#define ring2_slot      11

class pc : public character {
    public:
        ~pc() {}
        terrain_type known_terrain[DUNGEON_Y][DUNGEON_X];
        uint8_t visible[DUNGEON_Y][DUNGEON_X];
        uint8_t carry_slots_held = 0;
        object *carry_slots[10];
        object *equipment_slots[12] = {0};
        //uint32_t hp = 5000;
        bool is_filled[12] = {0};
};

void pc_delete(pc *pc);
uint32_t pc_is_alive(dungeon *d);
void config_pc(dungeon *d);
uint32_t pc_next_pos(dungeon *d, pair_t dir);
void place_pc(dungeon *d);
uint32_t pc_in_room(dungeon *d, uint32_t room);
void pc_learn_terrain(pc *p, pair_t pos, terrain_type ter);
terrain_type pc_learned_terrain(pc *p, int16_t y, int16_t x);
void pc_init_known_terrain(pc *p);
void pc_observe_terrain(pc *p, dungeon *d);
int32_t is_illuminated(pc *p, int16_t y, int16_t x);
void pc_reset_visibility(pc *p);

#endif
