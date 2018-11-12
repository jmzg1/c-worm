/*James Guan ==============================================================================================
7745303  ==================================================================================================
COMP3430 A2 ===============================================================================================
===========================================CATERPILLARS.H=================================================*/

#ifndef CATERPILLARS_H
#define CATERPILLARS_H

void gameRun(void);		    //Main Game Logic Method
void initializeBoard(void); //Used for Initializing the board
void initializeMutex(void); //Used for initializing mutex variables
void destroyMutex(void);	//Used for destroying mutex variables

void *keyboardThreadMethod(void* n); //Keyboard Reading Thread Method

#endif //CATERPILLARS_H

/*=====================================CATERPILLARS.H END==================================================*/
/*=========================================================================================================*/