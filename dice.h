#ifndef DICE_H
#define DICE_H

#include <stdint.h>

class dice {
    public:
        uint32_t base = 0;
        uint32_t sides;
        uint32_t numDice;
        inline int32_t get_base() const
        {
            return base;
        }
        inline int32_t get_number() const
        {
            return numDice;
        }
        inline int32_t get_sides() const
        {
            return sides;
        }
        int32_t rollDice(void) const;
        
};

#endif