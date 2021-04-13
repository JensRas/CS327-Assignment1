#include "dice.h"

/*****************************************
 *              Dice Roller              *
 *****************************************/
int rollDice(dice die)
{
    int i, roll = 0;

    for (i = 0; i < (int)die.numDice; i++) {
        roll += (rand() % die.sides) + 1;
    }
    roll += die.base;
    return roll; 
}