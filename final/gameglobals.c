/*James Guan ==============================================================================================
7745303  ==================================================================================================
COMP3430 A2 ===============================================================================================
===========================================GAMEGLOBALS.C=================================================*/




#include <string.h>
#include <unistd.h>

#include "gameglobals.h"


//GAME MESSAGES
char* END_GAME_PROMPT = "YOU ARE DEAD";
char* WIN_GAME_PROMPT = "YOU WIN!";
char* QUIT_GAME_PROMPT = "PLAYER QUIT";
char* LOSE_GAME_PROMPT = "CATERPILLARS WIN!";
char* HIT_PROMPT = "YOU GOT HIT!";
char* BLANK_LINE = "                           ";

//GAME HEADERS
char* GAME_HEADER = "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-CATERPILLARS!=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-";
char* GAME_HEADER2 = "                   Score:          Lives:                                      ";

//START NUM ENEMY
int START_ENEMY = 0;

//NUMBER OF STARTING LIVES 
int START_LIVES = 5;

//CONTROLS
char RIGHT_CONTROL = 'd';
char LEFT_CONTROL = 'a';
char UP_CONTROL = 'w';
char DOWN_CONTROL = 's';
char QUIT_CONTROL = 'q';
char SHOOT_CONTROL = ' ';
char EMPTY_CHAR = '\0';




/*=====================================GAMEGLOBALS.C END===================================================*/
/*=========================================================================================================*/