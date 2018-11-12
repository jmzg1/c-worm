/*James Guan ==============================================================================================
7745303  ==================================================================================================
COMP3430 A2 ===============================================================================================
===========================================CATERPILLARS.C=================================================*/

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <math.h>
#include <pthread.h>
#include <stddef.h>
#include <time.h>

#include "console.h"
#include "caterpillars.h" 
#include "gameglobals.h"
#include "player.h"
#include "refresher.h"
#include "enemy.h"
#include "bullet.h"


//Game Board Constant
char *GAME_BOARD[] = {
"                   Score:          Lives:  ",
"=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-CATERPILLARS!=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-",
"",
"",
"",
"",
"", 
"=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" };




//Global Variables
int numPoints;				//The number of Points
int numLives;				//The number of Lives
int numEnemy;				//The number of alive enemy
bool quitCondition;			//Quit Condition 
bool winCondition;			//Win Condition
bool playerHit;				//Flag to check if player was hit
char keyPressed; 			//The user input
char buffer[MAX_BUFFER];	//Buffer for Strings


//External Constants
extern int START_LIVES;
extern char EMPTY_CHAR;
extern int START_ENEMY;


//Condition variables
pthread_cond_t quitCond;	  //Used to signal for game end

//Mutex Variables
pthread_mutex_t livesMutex; 	
pthread_mutex_t screenMutex;
pthread_mutex_t scoreMutex;
pthread_mutex_t positionMutex;
pthread_mutex_t keyboardMutex;
pthread_mutex_t playerMutex;
pthread_mutex_t playerHitMutex;
pthread_mutex_t quitMutex;
pthread_mutex_t cListMutex;
pthread_mutex_t bListMutex;
pthread_mutex_t numEnemyMutex;

//Main Threads
pthread_t screenThread;
pthread_t playerThread;
pthread_t keyboardThread;
pthread_t upkeepThread;
pthread_t spawnThread;
pthread_t bulletMainThread;

void gameRun(void){
	/*This method is the main game logic and where most of threads are started and joined in this method*/
		quitCondition = false;
		srand(time(NULL));
	
		/*Main Game Logic Thread*/
		
		//Initialize Mutex Variables and Condition Variables
		initializeMutex();
		
		//Initialize the Board
		initializeBoard();
		
		
		//Start Main Threads
		pthread_create(&screenThread,NULL, refreshScreen, NULL); //Start screenRefresher Thread
		pthread_create(&upkeepThread, NULL, refreshValues, NULL);//Start gameUpkeep Thread
		pthread_create(&playerThread, NULL, playerThreadMethod, NULL); //Start Player Thread
		pthread_create(&keyboardThread, NULL, keyboardThreadMethod, NULL); //Start Keyboard Thread
		pthread_create(&spawnThread, NULL, caterPillarSpawner, NULL); //Start Caterpillar Spawner Thread
		pthread_create(&bulletMainThread, NULL, bulletMaintenance, NULL); //Start Bullet Upkeep Thread 
		

		
		//Sleep and Let Game run fully
		pthread_cond_wait(&quitCond, &quitMutex);
		
		

		//Do Cleaning and thread joining		
	  	pthread_join(spawnThread, NULL);  
		pthread_join(bulletMainThread, NULL);
		pthread_join(playerThread, NULL);
		pthread_join(keyboardThread, NULL);
		pthread_join(upkeepThread, NULL);
		pthread_join(screenThread, NULL);
		
		//Deinitialize Mutex Variables
		destroyMutex();
		
				
		//End Game
        finalKeypress(); /* wait for final key before killing curses and game */        
        consoleFinish();        	
	
}//end gameRun

void initializeBoard(void){
	/*This method initializes and draws the game board. This method also initializes
	the starting number of lives and starting numbre of points*/
	
	//String Representation of Lives and Points
	char *numLivesS;
	char *numPointsS;
	
	
	//Locks not really needed here since this is only used at the beginning when theres
	//only a single thread, But use them anyway.
	pthread_mutex_lock(&screenMutex);
	if (consoleInit(GAME_ROWS, GAME_COLS, GAME_BOARD)){
		
		pthread_mutex_lock(&livesMutex);
        numLives = START_LIVES;
		pthread_mutex_unlock(&livesMutex);
		
		pthread_mutex_lock(&scoreMutex);
		numPoints = START_POINTS;
		pthread_mutex_unlock(&scoreMutex);
		
		pthread_mutex_lock(&numEnemyMutex);
		numEnemy = START_ENEMY;
		pthread_mutex_unlock(&numEnemyMutex);
		
		sprintf(buffer, "%d", numLives);
		numLivesS = buffer;
		putString(numLivesS, ROW_START, LIVES_POS, floor(log10(abs(numLives)))+1);
		
		sprintf(buffer, "%d", numPoints);
		numPointsS = buffer;
		putString(numPointsS, ROW_START, SCORE_POS, floor(log10(abs(numPoints)))+1);
		consoleRefresh();
		
		
	}
	else{
		printf("Failed to Initialize Board");
	}
	pthread_mutex_unlock(&screenMutex);
	//unlock the mutex
}//end initializeBoard

void initializeMutex(void){
	/*This method initializes the mutex variables.*/
		//Initialize the Mutex Variables
		pthread_mutex_init(&screenMutex, NULL);
		pthread_mutex_init(&livesMutex, NULL);
		pthread_mutex_init(&scoreMutex, NULL);
		pthread_mutex_init(&positionMutex, NULL);
		pthread_mutex_init(&keyboardMutex, NULL);
		pthread_mutex_init(&playerMutex, NULL);
		pthread_mutex_init(&playerHitMutex, NULL);
		pthread_mutex_init(&quitMutex, NULL);
		pthread_mutex_init(&cListMutex, NULL);
		pthread_mutex_init(&bListMutex, NULL);
		pthread_mutex_init(&numEnemyMutex, NULL);
		
		//Initialize Condition Variables
		pthread_cond_init(&quitCond, NULL);
	
}//end initializeMutex

void destroyMutex(void){
	/*This method destroys the mutex variables*/ 
		
		pthread_mutex_destroy(&screenMutex);
		pthread_mutex_destroy(&livesMutex);
		pthread_mutex_destroy(&scoreMutex);
		pthread_mutex_destroy(&positionMutex);
		pthread_mutex_destroy(&keyboardMutex);
		pthread_mutex_destroy(&playerMutex);
		pthread_mutex_destroy(&playerHitMutex);
		pthread_mutex_destroy(&quitMutex);
		pthread_mutex_destroy(&cListMutex);
		pthread_mutex_destroy(&bListMutex);
		pthread_mutex_destroy(&numEnemyMutex);
		pthread_cond_destroy(&quitCond);
}//end destroyMutex


void *keyboardThreadMethod(void* n){
	/*This method reads in the input from the user using the keyboard and
	and stores it in a variable to be read in other threads*/
	fd_set readfd;
	struct timeval timeval;
	char inChar;
	int retVal;
	
	timeval.tv_sec = KEYTIMES;
	timeval.tv_usec = KEYTIMEMS;   
	 
	//Continuously read user input form the keyboard
	while(!quitCondition){
		FD_ZERO(&readfd);
		FD_SET(0, &readfd);
		retVal = select(1, &readfd, NULL, NULL, &timeval);
		if(retVal > 0){
			inChar = getchar();
			if(inChar != EMPTY_CHAR){
				pthread_mutex_lock(&keyboardMutex);
				keyPressed = inChar;
				inChar = EMPTY_CHAR;
				
				pthread_mutex_unlock(&keyboardMutex);}
			}
		else if(retVal == 0){
			//No Input Yet, Do nothing
		}
		else{
			perror("select()");
			exit(EXIT_FAILURE);
		}
	}
	pthread_exit(NULL);
	
}//end keyboardThreadMethod

/*=====================================CATERPILLARS.C END==================================================*/
/*=========================================================================================================*/