/*James Guan ==============================================================================================
7745303  ==================================================================================================
COMP3430 A2 ===============================================================================================
===============================================PLAYER.H===================================================*/

#ifndef PLAYER_H
#define PLAYER_H



//This thread method for the player
void *playerThreadMethod(void *n); 

//This method draws the player on the screen
void drawPlayer(int leftPos, int topPos); 

//This method moves the player using WASD controls
void movePlayer(void); 

//This method is for firing bullets
void shootPlayer(void); 


#endif   //PLAYER_H


/*=======================================PLAYER.H END======================================================*/
/*=========================================================================================================*/