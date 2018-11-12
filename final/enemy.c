/*James Guan ==============================================================================================
7745303  ==================================================================================================
COMP3430 A2 ===============================================================================================
===========================================ENEMY.C========================================================*/

#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#include "enemy.h"
#include "console.h"
#include "gameglobals.h"
#include "llist.h"

//Game Board Constants
char* ENEMY_BODY[ENEMY_BODY_ANIM_TILES][ENEMY_HEIGHT] = 
{
  {"O",
   "<"},
  {"O",
   ">"},
  {"o",
   "="},
  {"!",
   ";"},
  {"|",
   "'"},
  {"!",
   ":"},
  {"|",
   "'"}
};



//External Variables
extern bool quitCondition;
extern bool winCondition;
extern bool playerHit;
extern int numEnemy;
extern int numPoints;
extern node_t *caterPillarList;
extern node2_t *bulletList;
extern char* GAME_HEADER;
extern char* GAME_HEADER2;
extern char* LOSE_GAME_PROMPT;

	

//Mutex Variables
extern pthread_mutex_t livesMutex;
extern pthread_mutex_t screenMutex;
extern pthread_mutex_t scoreMutex;
extern pthread_mutex_t quitMutex;
extern pthread_mutex_t bListMutex;
extern pthread_mutex_t cListMutex;
extern pthread_mutex_t numEnemyMutex;

void *newCaterpillar(void *args2){
	/*This thread method is for spawning a new caterpillar given the parameters in args2*/

	struct caterPillarArgs *args = (struct caterPillarArgs *) args2; 

	//Continously move and draw the caterpillar if its alive or if game is still going on
	//Check if the caterpillar is alive and if it needs to shoot
	while(!quitCondition && args->alive){
		moveEnemy(&(args->goRight), &(args->headHorPos), &(args->headVerPos), &(args->numSegments), &(args->wrappedBody));
		drawEnemy(args->goRight, args->headHorPos, args->headVerPos, args->numSegments, args->wrappedBody);
		shootEnemy(args->headHorPos, args->headVerPos);
		checkIfAlive(&(args->alive), args->numSegments );
					
		sleepTicks(args->speed);
	}
	
	//Caterpillar Died, or game ended
	pthread_mutex_lock(&numEnemyMutex);
	numEnemy--;
	pthread_mutex_unlock(&numEnemyMutex);
	if(numEnemy == 0){winCondition = true;}
	pthread_exit(NULL);
	
}//end newCaterpillar

void *caterPillarSpawner(void *n){
	/*This thread method is responsible for spawning multiple caterpillars throughout the game*/
	
	int randomTime;
	int i;
	
	//Initialize Win Condition to false
	winCondition = false;
	
	//Default Caterpillar Parameters
	struct caterPillarArgs enemyArgs;
	enemyArgs.headVerPos = ENEMY_START_POSITIONV;
	enemyArgs.headHorPos = ENEMY_START_POSITIONH;
	enemyArgs.numSegments = ENEMY_DEFAULT_SEGMENTS;
	enemyArgs.alive = true;
	enemyArgs.wrappedBody = ENEMY_DEFAULT_SEGMENTS-1; //Head is initially not wrapped
	enemyArgs.goRight = true;
	enemyArgs.speed = ENEMY_DEFAULT_SPEED;
	
	
	//Initialize the Caterpillar List and start first caterpillar thread
	caterPillarList = newList(enemyArgs);
	pthread_mutex_lock(&numEnemyMutex);
	numEnemy++;
	pthread_mutex_unlock(&numEnemyMutex);
	
	//Keep spawning caterpillars while game is still on
	while(!quitCondition){
		
		randomTime = rand()%SPAWN_WAIT + SPAWN_WAIT; //Generate an inte
		
		//Wait between 10 to 20 loops before spawning new caterPillar
		for( i = 0; i < randomTime; i++){
			 //Wait then check for quit condition again
			if(!quitCondition){sleepTicks(SPAWN_SLEEP);}
			else{break;}
		}

		//If there are still live caterPillars	
		if(numEnemy!= DEAD){
			pthread_mutex_lock(&cListMutex);	
			appendList(enemyArgs, caterPillarList);
			pthread_mutex_unlock(&cListMutex);
		}
			
		pthread_mutex_lock(&numEnemyMutex);
		numEnemy++;
		pthread_mutex_unlock(&numEnemyMutex);
		
	}//Spawning loop

	//If it is time to quit, clean the list of caterpillars
	deleteCaterpillarList(caterPillarList);
	 
	 //Exit spawner thread
	 pthread_exit(NULL);
	 
}//end caterPillarSpawner


void drawEnemy(bool goRight, int headHorPos, int headVerPos, int numSegments, int wrappedSegments){
	/*This method draws the caterpillar based on the passed parameters*/
	char** head;
	char** headDef;
	char** segmentTile;
	int i, j;
	
	//Check for direction for correct head tile (To define direction of head)
	if(goRight){headDef = ENEMY_BODY[0];}
	else{headDef = ENEMY_BODY[1];}
	
	//Animate the Caterpillar
	for(i = 0; i < ENEMY_FRAMES; i++){
		//For Mouth Animations
		if( i%ENEMY_FRAMES == 0){head = headDef;}
		else{head = ENEMY_BODY[2];}

		
		//Draw Head  
		pthread_mutex_lock(&screenMutex);
		consoleClearImage(headVerPos,headHorPos,ENEMY_HEIGHT, SEGMENT_LENGTH);
		consoleDrawImage(headVerPos, headHorPos , head, ENEMY_HEIGHT);
		pthread_mutex_unlock(&screenMutex);
		
		//Draw the non-wrapped segments going in the direction of the head 
		for(j = 1; j < (numSegments - wrappedSegments)+1; j++){
			if(i%2==0){segmentTile = ENEMY_BODY[(j%3)+4];} //Animation
			else{segmentTile = ENEMY_BODY[(j%4)+3];}       //Animation
			pthread_mutex_lock(&screenMutex);
			
			if(goRight){ 
			//If head is going right, make the non-wrapped segments go right
			consoleClearImage(headVerPos,headHorPos-j,ENEMY_HEIGHT, SEGMENT_LENGTH);
			consoleDrawImage(headVerPos, headHorPos-j, segmentTile, ENEMY_HEIGHT);
			}
			else{
			//If head is going left, make the non-wrapped segments go left
			consoleClearImage(headVerPos,headHorPos+j,ENEMY_HEIGHT, SEGMENT_LENGTH);
			consoleDrawImage(headVerPos, headHorPos+j, segmentTile, ENEMY_HEIGHT);
			}
			pthread_mutex_unlock(&screenMutex);
		}
		
		if(headVerPos != ENEMY_START_POSITIONV){
			//Draw the wrapped segments, opposite direction of head if
			//there are any wrapped segments.
			for(j = 0; j < wrappedSegments; j++){
				if(i%2==0){segmentTile = ENEMY_BODY[(j%3)+4];}
				else{segmentTile = ENEMY_BODY[(j%2)+4];}
				pthread_mutex_lock(&screenMutex);
				
				if(goRight){
				//If head is going right, make the wrapped elements go left
					consoleClearImage(headVerPos-ENEMY_HEIGHT,COL_START+wrappedSegments+RIGHT,ENEMY_HEIGHT, SEGMENT_LENGTH);
					consoleDrawImage(headVerPos-ENEMY_HEIGHT, COL_START+j, segmentTile, ENEMY_HEIGHT);
				}
				else{
				//If head is going left, make the wrapped segments go right
					consoleClearImage(headVerPos-ENEMY_HEIGHT,COL_END-wrappedSegments+LEFT,ENEMY_HEIGHT, SEGMENT_LENGTH);
					consoleDrawImage(headVerPos-ENEMY_HEIGHT, COL_END-j, segmentTile, ENEMY_HEIGHT);
				}
				pthread_mutex_unlock(&screenMutex);
			}
		}
		else{
		//This method is only for the first lines, because wrapping overwrites the headers
			pthread_mutex_lock(&screenMutex);
			putString(GAME_HEADER, ROW_START+DOWN, COL_START, numSegments);
			putString(GAME_HEADER2, ROW_START, COL_START, numSegments);
			pthread_mutex_unlock(&screenMutex);
		}

		
		sleepTicks(ENEMY_FRAME_SPEED);
	}
}//end drawEnemy

void moveEnemy(bool *goRight, int *headHPos, int *headVPos, int *numSegments, int *wrappedBody ){
	/*This method moves the enemy to the direction indicated by goRight parameter*/
	int tailVPos;
	int tailHPos;
	
	//Check if going Right
	if(*goRight){
		
		//The Right border still hasnt been reached
		if(*headHPos + RIGHT <= COL_END){
			*headHPos = *headHPos + RIGHT;
			//Check for wrapped segments
			if((*headHPos-*numSegments) < COL_START){
				*wrappedBody = *numSegments - (ROW_START + *headHPos) -1;
				tailVPos = *headVPos-ENEMY_HEIGHT;
				tailHPos = COL_START+*wrappedBody;
				}			
			else{*wrappedBody = 0;
				tailVPos = *headVPos;
				tailHPos = *headHPos - *numSegments-1;
			}
			
		}
		//Border has been reached, time to wrap but first check if player area reached
		else{
			if(*headVPos + DOWN < PLAYER_MAX_TOP){
				*wrappedBody = *numSegments-1;
				tailVPos = *headVPos;
				tailHPos = *numSegments;
				*headVPos = *headVPos+ ENEMY_HEIGHT;
				*headHPos = COL_END;
				*goRight = false;
				}
			else{
				if(*headHPos == COL_END){
					//Reached end of allowable area, game end
					pthread_mutex_lock(&screenMutex);
					putString(LOSE_GAME_PROMPT, ROW_END/HALF, COL_END/HALF-sizeof(LOSE_GAME_PROMPT)/HALF, sizeof(LOSE_GAME_PROMPT)*HALF);
					pthread_mutex_unlock(&screenMutex);
					
					pthread_mutex_lock(&quitMutex);
					quitCondition = true;
					pthread_mutex_unlock(&quitMutex);
				}
			}
		}
		
	}//going right
	//Check if going left
	else{
		//The Left border still hasnt been reached
		if(*headHPos + LEFT >= COL_START) {
			*headHPos = *headHPos + LEFT;
			if((*headHPos+*numSegments) > COL_END){
				*wrappedBody = *numSegments - (COL_END - *headHPos) -1;	
				tailVPos = *headVPos-ENEMY_HEIGHT;
				tailHPos = COL_END-*wrappedBody;
				}
			else{*wrappedBody = 0;
				tailVPos = *headVPos;
				tailHPos = *headHPos+*numSegments+1;
			}
			
			}
		else{
			if(*headVPos + DOWN < PLAYER_MAX_TOP){
				*wrappedBody = *numSegments-1;
				tailVPos = *headVPos;
				tailHPos = COL_END-*numSegments;
				*headVPos = *headVPos + ENEMY_HEIGHT;
				*headHPos = COL_START;
				*goRight = true;
				}
			else{
				if(*headHPos == COL_START){
					//Reached end of allowable area, game end
					pthread_mutex_lock(&screenMutex);
					putString(LOSE_GAME_PROMPT, ROW_END/HALF, COL_END/HALF-sizeof(LOSE_GAME_PROMPT)/HALF, sizeof(LOSE_GAME_PROMPT)*HALF);
					pthread_mutex_unlock(&screenMutex);
					
					pthread_mutex_lock(&quitMutex);
					quitCondition = true;
					pthread_mutex_unlock(&quitMutex);
				}
			}
		}
	}//going left
	
	//Clear tail left behind after move (Avoid ghosting)
	pthread_mutex_lock(&screenMutex);
	consoleClearImage(tailVPos,tailHPos,ENEMY_HEIGHT, SEGMENT_LENGTH);
	pthread_mutex_unlock(&screenMutex);

	
}//end moveEnemy


void shootEnemy(int headHorPos, int headVerPos){
	/*This method is used by the enemy to shoot. The enemy will not shoot
	unless the player shoots first.*/
	
	
	int shootChance = rand()%ENEMY_SHOOT_PROBABILITY_MAX;
	
	//Do not let enemy shoot if player just got hit
	if(bulletList != NULL && !playerHit){
		//Let the enemy shoot based on given probability
		if(shootChance < ENEMY_SHOOT_PROBABILITY && pthread_mutex_trylock(&bListMutex) == 0 ){
			struct bulletArgs bulletArgs;
			bulletArgs.bulletVerPos = headVerPos+DOWN+DOWN;
			bulletArgs.bulletHorPos = headHorPos;
			bulletArgs.alive = true;
			bulletArgs.playerBullet = false;
			
			appendList2(bulletArgs, bulletList);
			pthread_mutex_unlock(&bListMutex);
		}
	}
	
}//end shoot

void checkIfAlive(bool *alive, int numSegments ){
	/*This method is used to check if the length of the catepillar
	falls below the minimum allowed. If so, kill the caterpillar*/
	if(numSegments 	< ENEMY_MIN_SEGMENTS){
		pthread_mutex_lock(&scoreMutex);
		numPoints = numPoints + KILL_SCORE;
		pthread_mutex_unlock(&scoreMutex);
		*alive = false;
	}
	
}//end checkIfAlive



/*=======================================ENEMY.C END=======================================================*/
/*=========================================================================================================*/
