/*James Guan ==============================================================================================
7745303  ==================================================================================================
COMP3430 A2 ===============================================================================================
===========================================BULLET.H========================================================*/

#ifndef BULLET_H
#define BULLET_H

#include "gameglobals.h"



//Thread function to create a new bullet
void *newBullet(void *args2);

//Thread function to maintain list of bullets
void *bulletMaintenance(void *args);

//Method for moving the bullet
void moveBullet(bool *playerBullet, int *bulletHPos, int *bulletVPos,  bool *alive);

//Method for drawing bullet
void drawBullet(bool playerBullet, int bulletHPos, int bulletVPos, bool alive);

//Method for checking if the player is hit
bool checkPlayerHit(int bulletHPos, int bulletVPos, bool *alive);

//Method for checking if the enemy is hit
bool checkEnemyHit(int bulletHPos, int bulletVPos, bool* alive, node_t *currCaterpillar);


#endif



/*=======================================BULLET.H END=======================================================*/
/*=========================================================================================================*/
