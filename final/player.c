/*James Guan ==============================================================================================
7745303  ==================================================================================================
COMP3430 A2 ===============================================================================================
===============================================PLAYER.C===================================================*/

#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>


#include "player.h"
#include "console.h"
#include "gameglobals.h"
#include "bullet.h"
#include "llist.h"


//PLAYER TILES
char* PLAYER_BODY[PLAYER_BODY_ANIM_TILES][PLAYER_HEIGHT] = 
{
	{"!",
	 "|",
	 "^"},
	{"|",
	 "!",
	 "^"},
	 {" ",
	 "[",
	 "/"},
	 {" ",
	 "{",
	 ")"},
	 {" ",
	 "]",
	 "\\"},
	 {" ",
	 "}",
	 "("}
	
};

//Player Variables
int playerLeftPos;
int playerTopPos;

//External Variables
extern int numLives;
extern bool quitCondition;
extern char keyPressed;
extern char RIGHT_CONTROL;
extern char LEFT_CONTROL;
extern char UP_CONTROL;
extern char DOWN_CONTROL;
extern char SHOOT_CONTROL;
extern char EMPTY_CHAR;

//Mutex Variables
extern pthread_mutex_t livesMutex;
extern pthread_mutex_t screenMutex;
extern pthread_mutex_t scoreMutex;
extern pthread_mutex_t keyboardMutex;
extern pthread_mutex_t playerMutex;
extern pthread_mutex_t cListMutex;
extern pthread_mutex_t bListMutex;

void *playerThreadMethod(void *n){
	/*This method is the thread method that is responsible for the player
	and methods related to it*/
	
	
	//Initialize the player Position
	pthread_mutex_lock(&playerMutex);
	playerLeftPos = PLAYER_START_POSITIONH +LEFT;
	playerTopPos = PLAYER_START_POSITIONV;
	pthread_mutex_unlock(&playerMutex);
	

	//Continously move and draw the player, check for shooting
	while(!quitCondition){
		movePlayer();
		drawPlayer(playerLeftPos, playerTopPos);
		shootPlayer();
		sleepTicks(PLAYER_REFRESH_RATE);
	}
	
	pthread_exit(NULL);
	
}//end playerThreadMethod

void drawPlayer(int leftPos, int topPos){
	/*This method draws the player on the screen*/
	
	char** leftBody;
	char** midBody;
	char** rightBody;

	//Choose which version of the animation tiles to display 	
	int i;
	for(i = 0; i < PLAYER_FRAMES; i++){
	if( i%PLAYER_FRAMES == 0){
		leftBody = PLAYER_BODY[2];
		midBody = PLAYER_BODY[0];
		rightBody = PLAYER_BODY[4];}
	else{
		leftBody = PLAYER_BODY[3];
		midBody = PLAYER_BODY[1];
		rightBody = PLAYER_BODY[5];
		
	}
	
	//Draw the player at the given position
	pthread_mutex_lock(&screenMutex);
	consoleClearImage(topPos,leftPos,PLAYER_HEIGHT, strlen(leftBody[0]));
	consoleClearImage(topPos,leftPos+RIGHT,PLAYER_HEIGHT, strlen(midBody[0]));
	consoleClearImage(topPos,leftPos+RIGHT+RIGHT,PLAYER_HEIGHT, strlen(rightBody[0]));
	
	consoleDrawImage(topPos, leftPos , leftBody, PLAYER_HEIGHT);
	consoleDrawImage(topPos, leftPos+RIGHT, midBody, PLAYER_HEIGHT);
	consoleDrawImage(topPos, leftPos+RIGHT+RIGHT, rightBody, PLAYER_HEIGHT);
	pthread_mutex_unlock(&screenMutex);
	sleepTicks(PLAYER_REFRESH_RATE);}

}//end drawPlayer

void movePlayer(){
	/*This method is used for moving the player around using WASD controls*/
	
	int prevLeftPos = playerLeftPos;
	int prevTopPos = playerTopPos;
	char newKey;

	//Read in the keyPressed
	pthread_mutex_lock(&keyboardMutex);
	newKey = keyPressed;
	pthread_mutex_unlock(&keyboardMutex);
	
	pthread_mutex_lock(&playerMutex);
	
	//Move Player Up
	if(newKey == UP_CONTROL){
		//Check if the player is at the boundary
		if(playerTopPos + UP > PLAYER_MAX_TOP){
			playerTopPos = playerTopPos + UP;
			pthread_mutex_lock(&screenMutex);
			consoleClearImage(prevTopPos,playerLeftPos,PLAYER_HEIGHT, PLAYER_WIDTH);
			pthread_mutex_unlock(&screenMutex);
		}
	}//Move up
	
	//Move Player Left
	else if(newKey == LEFT_CONTROL){
		
		//Check if the player is at the boundary
		if(playerLeftPos + LEFT > COL_START){
			playerLeftPos = playerLeftPos + LEFT;
			pthread_mutex_lock(&screenMutex);
			consoleClearImage(playerTopPos,prevLeftPos+2,PLAYER_HEIGHT, PLAYER_WIDTH);
			pthread_mutex_unlock(&screenMutex);
			
		}	
	}//Move left
	
	//Move Player Down
	else if(newKey == DOWN_CONTROL){

		//Check if the player is at the boundary
		if(playerTopPos+DOWN+DOWN+ DOWN < ROW_END){
			playerTopPos = playerTopPos + DOWN;
			pthread_mutex_lock(&screenMutex);
			consoleClearImage(prevTopPos,playerLeftPos,PLAYER_HEIGHT, PLAYER_WIDTH);
			pthread_mutex_unlock(&screenMutex);
		}
		
	}//Move Down
	
	//Move Player Right
	else if(newKey == RIGHT_CONTROL){
		
		//Check if the player is at the boundary
		if(playerLeftPos+RIGHT+RIGHT+ RIGHT < COL_END){
			playerLeftPos = playerLeftPos + RIGHT;
			pthread_mutex_lock(&screenMutex);
			consoleClearImage(playerTopPos,prevLeftPos,PLAYER_HEIGHT, PLAYER_WIDTH);
			pthread_mutex_unlock(&screenMutex);
		}	
	}//Move Right	
	
	else{
		//Do Nothing
	}
	
	pthread_mutex_unlock(&playerMutex);
	pthread_mutex_lock(&keyboardMutex);
	keyPressed = EMPTY_CHAR; //Clear keyPressed
	pthread_mutex_unlock(&keyboardMutex);
}//end movePlayer

void shootPlayer(){
	/*This method is used for letting the player shoot*/
	
	extern node2_t *bulletList;
	extern bool playerHit;
	char newKey;
	
	//Read in keyPressed
	pthread_mutex_lock(&keyboardMutex);
	newKey = keyPressed;
	pthread_mutex_unlock(&keyboardMutex);
	
	
	//Check if shoot was pressed and fire bullet
	if(newKey == SHOOT_CONTROL && !playerHit){
		//Make a struct for the bullet
		struct bulletArgs bulletArgs;
		bulletArgs.bulletVerPos = playerTopPos+UP;
		bulletArgs.bulletHorPos = playerLeftPos+RIGHT;
		bulletArgs.alive = true;
		bulletArgs.playerBullet = true;
	
		if(bulletList == NULL){
			pthread_mutex_lock(&bListMutex);
			//if(pthread_mutex_trylock(&bListMutex) == 0){ //Shoot not responsive if trylock        
			bulletList = newList2(bulletArgs);
			pthread_mutex_unlock(&bListMutex);
		}
		else{
			pthread_mutex_lock(&bListMutex);
			//if(pthread_mutex_trylock(&bListMutex) == 0){ //Shoot not responsive if trylock
			appendList2(bulletArgs, bulletList);
			pthread_mutex_unlock(&bListMutex);
		}
		sleepTicks(PLAYER_SHOOT_CD);
	}
	
}//end shoot
	


/*=======================================PLAYER.C END======================================================*/
/*=========================================================================================================*/