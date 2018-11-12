/*James Guan ==============================================================================================
7745303  ==================================================================================================
COMP3430 A2 ===============================================================================================
=============================================ENEMY.H======================================================*/

#ifndef ENEMY_H
#define ENEMY_H


#include "gameglobals.h"

//Thread function to create a single caterpillar
void *newCaterpillar(void *args2);

//Thread function to continually spawn caterpillars
void *caterPillarSpawner(void *n);

//This method draws the player on the screen
void drawEnemy(bool goRight, int headHPos, int headVPos, int numSegments, int wrappedSegments);

//This method moves the player using WASD controls
void moveEnemy(bool *goRight, int *headHPos, int *headVPos, int *numSegments, int *wrappedBody );

//This method lets the enemy shoot
void shootEnemy(int headHorPos, int headVerPos);

//This method checks if the caterpillar is still alive
void checkIfAlive(bool *alive, int numSegments );
#endif 



/*=======================================ENEMY.H END=======================================================*/
/*=========================================================================================================*/