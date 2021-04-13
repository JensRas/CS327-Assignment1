#include "dice.h"
#include <cstdlib>

/*****************************************
 *              Dice Roller              *
 *****************************************/
int32_t dice::rollDice(void) const
{
    int i, roll = 0;

    for (i = 0; i < (int)numDice; i++) {
        roll += (rand() % sides) + 1;
    }
    roll += base;
    return roll; 
}