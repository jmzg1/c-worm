/*James Guan ==============================================================================================
7745303  ==================================================================================================
COMP3430 A2 ===============================================================================================
==============================================REFRESHER.C=================================================*/

#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>



#include "console.h"
#include "refresher.h"
#include "gameglobals.h"
#include "llist.h"
#include "bullet.h"

//Mutex Variables
extern pthread_mutex_t screenMutex;
extern pthread_mutex_t livesMutex;
extern pthread_mutex_t scoreMutex;
extern pthread_mutex_t keyboardMutex;
extern pthread_mutex_t quitMutex;
extern pthread_mutex_t playerHitMutex;
extern pthread_cond_t quitCond;
extern pthread_mutex_t numEnemyMutex;

//External Variables
extern int numLives;
extern int numPoints;
extern int numEnemy;
extern bool playerHit;
extern bool quitCondition;
extern bool winCondition;
extern char buffer[MAX_BUFFER];
extern char keyPressed;
extern char* QUIT_GAME_PROMPT;
extern char* WIN_GAME_PROMPT;
extern char* END_GAME_PROMPT;
extern char* HIT_PROMPT;
extern char* BLANK_LINE;
extern char QUIT_CONTROL;



//The LinkedLists
node_t *caterPillarList = NULL;
node2_t *bulletList = NULL;

void *refreshScreen(void * n){
	/*This thread method redraws all the elements on the screen*/
	
	//Continuously refresh the screen while game is running
	while(!quitCondition){
		pthread_mutex_lock(&screenMutex);
		consoleRefresh();
		pthread_mutex_unlock(&screenMutex);
		sleepTicks(MIN_REFRESH_RATE);
	}
	
	pthread_exit(NULL);
	
}//end refreshScreen

void *refreshValues(void *n){
	/*This thread method redraws the information in the header
	which is the number of lives and the score. This method also
	checks for game ending conditions, and monitors player hits*/
	
	//Continuously update the information on the headers and check for hits
	while(!quitCondition){
		refreshScore();
		refreshLives();
		checkPlayerHitFlag();
		checkForQuitConditions();
		refreshScore();
		refreshLives();
		
	
		sleepTicks(MIN_REFRESH_RATE);
	}
	pthread_cond_signal(&quitCond);
	pthread_exit(NULL);
	
}//end refreshValues


void refreshScore(void){
	/*This method is used to refresh the value of the score and draw it*/
	char *numPointsS;
	int newPoints;
	//Read in number of points
	pthread_mutex_lock(&scoreMutex);
	newPoints = numPoints;
	pthread_mutex_unlock(&scoreMutex);
	//Conver to string	
	sprintf(buffer, "%d", newPoints);
	numPointsS = buffer;
	//Display	
	pthread_mutex_lock(&screenMutex);
	putString(numPointsS, ROW_START, SCORE_POS, floor(log10(abs(newPoints)))+RIGHT);
	pthread_mutex_unlock(&screenMutex);
	
}//end refreshScore

void refreshLives(void){
	/*This method is used to refresh the value of the lives and draw it*/
	char *numLivesS;
	int newLives;
	//Read in number of lives
	pthread_mutex_lock(&livesMutex);
	newLives = numLives;
	pthread_mutex_unlock(&livesMutex);
	//Convert to String	
	sprintf(buffer, "%d", newLives);
	numLivesS = buffer;
	//Display	
	pthread_mutex_lock(&screenMutex);
	putString(numLivesS, ROW_START, LIVES_POS, floor(log10(abs(newLives)))+RIGHT);
	pthread_mutex_unlock(&screenMutex);
	
}//end refreshLives

void checkForQuitConditions(void){
	/*This method calls the three methods that can end the game*/
	checkForQuit(); //Checks for player hitting quit
	checkIfPlayerDead(); //Checks if the number of lives is 0
	checkIfEnemiesDead(); //Check is all enemies are killed
	
}//end checkForQuitConditions


void checkForQuit(void){
	/*This method is used to check for the quit conditions and update the value of the
	variable appropriately.*/
	char newKey;

	//Read in keyPressed
	pthread_mutex_lock(&keyboardMutex);
	newKey = keyPressed;
	pthread_mutex_unlock(&keyboardMutex);
	
	//Check if the key pressed is Q
	if(newKey == QUIT_CONTROL){
		pthread_mutex_lock(&screenMutex);
		putString(QUIT_GAME_PROMPT, ROW_END/HALF, COL_END/HALF-sizeof(QUIT_GAME_PROMPT)/HALF, sizeof(QUIT_GAME_PROMPT)*HALF);
		pthread_mutex_unlock(&screenMutex);
		pthread_mutex_lock(&quitMutex);
		quitCondition = true;
		pthread_mutex_unlock(&quitMutex); 
		
	}
	
}//end checkQuit

void checkIfPlayerDead(void){
	/*This method is used to check if the player still has lives. If not, ends the game*/
	int lives;
	pthread_mutex_lock(&livesMutex);
	lives = numLives;
	pthread_mutex_unlock(&livesMutex);
	
	if(lives <= DEAD){
		pthread_mutex_lock(&screenMutex);
		putString(END_GAME_PROMPT, ROW_END/HALF, COL_END/HALF-sizeof(END_GAME_PROMPT)/HALF, sizeof(END_GAME_PROMPT)*HALF);
		pthread_mutex_unlock(&screenMutex);
		pthread_mutex_lock(&quitMutex);
		quitCondition = true;
		pthread_mutex_unlock(&quitMutex); 
	}
	
}//end checkIfPlayerDead

void checkIfEnemiesDead(void){
	/*This method is used to check if there are no more enemies on the board. If so, player wins*/
	extern bool winCondition;
	extern int numEnemy;
	if( numEnemy == DEAD && winCondition == true){
		
		pthread_mutex_lock(&screenMutex);
		putString(WIN_GAME_PROMPT, ROW_END/HALF, COL_END/HALF-sizeof(WIN_GAME_PROMPT)/HALF, sizeof(WIN_GAME_PROMPT)*HALF);
		pthread_mutex_unlock(&screenMutex);
		pthread_mutex_lock(&quitMutex);
		quitCondition = true;
		pthread_mutex_unlock(&quitMutex); 	
	}
}//end checkIfEnemiesDead



void checkPlayerHitFlag(void){
	/*This method is ued to check if the player has been hit by enemy bullet.
	If hit, decrease the number of lives and display message. Getting hit also
	kills all live bullets and enemy is not allowed to shoot until the player shoots*/
	bool checkPlayer;
	extern bool winCondition;
	extern pthread_t bulletMainThread;
	
	//Check if the player was hit
	pthread_mutex_lock(&playerHitMutex);
	checkPlayer = playerHit;
	pthread_mutex_unlock(&playerHitMutex);
	
	if(checkPlayer == true){
		//Decrease number of lives
		pthread_mutex_lock(&livesMutex);
		numLives = numLives-1;
		pthread_mutex_unlock(&livesMutex);
		
		
		//Display Hit message to player
		pthread_mutex_lock(&screenMutex);
		putString(HIT_PROMPT, ROW_END/HALF, COL_END/HALF-sizeof(HIT_PROMPT)/HALF, sizeof(HIT_PROMPT)*HALF);
		pthread_mutex_unlock(&screenMutex);
		
		if(numLives != DEAD){
			//Sleep to allow Player to read message and for bullets to be cleaned
			sleep(REGROUP_TIME);

			pthread_join(bulletMainThread, NULL);
			bulletList = NULL;
		
		
			//Clear the messsage		
			pthread_mutex_lock(&screenMutex);
			putString(BLANK_LINE, ROW_END/HALF, COL_END/HALF-sizeof(BLANK_LINE)/HALF, sizeof(BLANK_LINE)*HALF);
			pthread_mutex_unlock(&screenMutex);
		
			//Clear player hit condition
			pthread_mutex_lock(&playerHitMutex);
			playerHit = false;
			pthread_mutex_unlock(&playerHitMutex);
		
			//Restart bullet maintenance thread
			pthread_create(&bulletMainThread, NULL, bulletMaintenance, NULL);
		}
		else{
			pthread_mutex_lock(&screenMutex);
			putString(END_GAME_PROMPT, ROW_END/HALF, COL_END/HALF-sizeof(END_GAME_PROMPT)/HALF, sizeof(END_GAME_PROMPT)*HALF);
			pthread_mutex_unlock(&screenMutex);
			
		}	
	}
	
	
}//end checkPlayerHitFlag


/*=======================================REFRESHER.C END===================================================*/
/*=========================================================================================================*/