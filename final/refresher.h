/*James Guan ==============================================================================================
7745303  ==================================================================================================
COMP3430 A2 ===============================================================================================
===============================================REFRESHER.H================================================*/

#ifndef REFRESHER_H
#define REFRESHER_H




//Method for refreshing the screen
void *refreshScreen(void * n);

//Method for refreshing values
void *refreshValues(void * n);

//Methods for refreshing score and lives
void refreshScore(void);
void refreshLives(void);

//Methods for checking game end conditions
void checkForQuitConditions(void);
void checkForQuit(void);
void checkIfPlayerDead(void);
void checkIfEnemiesDead(void);

//Method for checking if player is hit
void checkPlayerHitFlag(void);




#endif //REFRESHER_H



/*=======================================REFRESHER.H END===================================================*/
/*=========================================================================================================*/