/*James Guan ==============================================================================================
7745303  ==================================================================================================
COMP3430 A2 ===============================================================================================
===========================================BULLET.C========================================================*/

#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#include "console.h"
#include "gameglobals.h"
#include "llist.h"
#include "bullet.h"
#include "enemy.h"

char* PLAYER_BULLET_CHAR[] = {"^"};
char* ENEMY_BULLET_CHAR[] = {"v"};

//Quit Condition Variable
extern bool quitCondition;


//Mutex Variables
extern pthread_mutex_t livesMutex;
extern pthread_mutex_t screenMutex;
extern pthread_mutex_t scoreMutex;
extern pthread_mutex_t quitMutex;
extern pthread_mutex_t bListMutex;
extern pthread_mutex_t playerHitMutex;
extern pthread_mutex_t cListMutex;
extern pthread_mutex_t numEnemyMutex;

void *newBullet(void *args2){
	/*This thread method is used for drawing and moving a single bullet. This also checks
	If this bullet is also colliding with other objects*/
	
	struct bulletArgs *args = (struct bulletArgs *) args2; 
	
	extern bool playerHit;
	extern int numPoints;
	extern node_t *caterPillarList;
	node_t *currCaterpillar;
	bool retVal;
	
	while(args->alive && !quitCondition){
		
		
		//This bullet is a player bullet. Check if it collides with an enemy
		if(args->playerBullet){
			
			currCaterpillar = caterPillarList;
			
			//Iterate over all caterpillars in the lsit to see which one is being hit
			while(currCaterpillar != NULL){
				if(currCaterpillar->args.alive == true){
					//check for collision
					retVal = checkEnemyHit(args->bulletHorPos, args->bulletVerPos, &(args->alive), currCaterpillar);
					
					//If it collided, make the caterpillar go faster and give points to player
					if(retVal){
						pthread_mutex_lock(&scoreMutex);
						numPoints = numPoints + HIT_SCORE;
						pthread_mutex_unlock(&scoreMutex);
						currCaterpillar->args.speed = currCaterpillar->args.speed/HALF;
						break;
					}
					//Move on to next caterpillar	
					else{currCaterpillar = currCaterpillar->next;}
				}
				//Move on to next caterpillar
				else{currCaterpillar = currCaterpillar->next;}
			}//Collision Loop
			
			//If Bullet did not collide with anything, move it
			if(!retVal){
				moveBullet(&(args->playerBullet), &(args->bulletHorPos), &(args->bulletVerPos), &(args->alive));
			}
			//If bullet collided with it, break out of its main loop
			else{break;}	
		}//For player bullet
		
		//This bullet is an enemy bullet. Check if it collides with the player
		else{
			retVal = checkPlayerHit(args->bulletHorPos, args->bulletVerPos, &(args->alive)); 
			if(retVal){
				//If player was hit, Set hit flag and exit bullet's main loop
				pthread_mutex_lock(&playerHitMutex);
				playerHit = true;
				pthread_mutex_unlock(&playerHitMutex);
				break;
			}//Player collision checl
			else{
				moveBullet(&(args->playerBullet), &(args->bulletHorPos), &(args->bulletVerPos), &(args->alive));
			}
		}

		//Draw the bullet at the given position
		drawBullet(args->playerBullet, args->bulletHorPos, args->bulletVerPos, args->alive);
		sleepTicks(DEFAULT_BULLET_SPEED);
	}//Bullet main loop
	
	//Kill Bullet
	args->alive = false;
	
	pthread_exit(NULL);
	
}//end newBullet


void *bulletMaintenance(void *args){
	/*This method removes the bullets that have gone over the borders.
	This method also removes the bullets that have hit objects */
	extern bool playerHit;
	extern node2_t *bulletList;
	node2_t *currNode;
	int count;
	
	while(!quitCondition && !playerHit){
		currNode = bulletList;
		
		//Iterate over the bullet list to check for dead bullets
		while(currNode != NULL){
			if(quitCondition){break;}
				if(currNode->args.alive == false){ 
					count = findNode2(bulletList, currNode);
					if(count!= -1){
						deleteFromList2(count, &bulletList);
				}
				currNode = bulletList;
			}
			else{
				currNode = currNode-> next;
			}
		}
	}//Bullet maintenance main loop
	
	//Delete all bullets on the list before quitting this thread
	deleteBulletList(bulletList);
	
	pthread_exit(NULL);
	
}//end bulletMaintenance 

void moveBullet(bool *playerBullet, int *bulletHPos, int *bulletVPos,  bool *alive){
	/*This method is used for moving the bullet upwards if it a player bullet, or 
	downwards if it is an enemy bullet*/
	
	int prevBulletVPos = *bulletVPos;
	int prevBulletHPos = *bulletHPos;
	
	//If this is a player bullet, it moves upwards	
	if(*playerBullet){
		
		//Check if upper enemy border has been reached, if not move up
		if(*bulletVPos + UP >= ENEMY_MAX_TOP){
			*bulletVPos = *bulletVPos + UP;
		}
		//Borderhit, kill bullet
		else{
			*alive = false;
			pthread_mutex_lock(&screenMutex);
			consoleClearImage(*bulletVPos,*bulletHPos,BULLET_HEIGHT, BULLET_LENGTH);
			pthread_mutex_unlock(&screenMutex);
		}
		
	}
	//Else this is a caterpillar bullet, it moves downwards
	else{
		
		//Check if lower border has been reached, if not move down
		if(*bulletVPos + DOWN < ROW_END ){
			*bulletVPos = *bulletVPos + DOWN;
		}
		//Borderhit, kill bullet
		else{
			*alive = false;
			pthread_mutex_lock(&screenMutex);
			consoleClearImage(*bulletVPos,*bulletHPos,BULLET_HEIGHT, BULLET_LENGTH);
			pthread_mutex_unlock(&screenMutex);
			
		}
		
	}
	
	//Remove after image due to moving to prevent ghosting
	pthread_mutex_lock(&screenMutex);
	consoleClearImage(prevBulletVPos,prevBulletHPos,BULLET_HEIGHT, BULLET_LENGTH);
	pthread_mutex_unlock(&screenMutex);
	
	
	
}//end moveBullet


void drawBullet(bool playerBullet, int bulletHPos,int bulletVPos, bool alive){
	/*This method draws the bullet at the given position.*/
	
	char* bulletTile;
	
	//Pick the right tile for the bullet according to the owner
	if(playerBullet){bulletTile = PLAYER_BULLET_CHAR[0];}
	else{bulletTile = ENEMY_BULLET_CHAR[0];}
	
	//Draw The Bullet
	if(alive){
		
		pthread_mutex_lock(&screenMutex);
		consoleClearImage(bulletVPos,bulletHPos,BULLET_HEIGHT, BULLET_LENGTH);
		consoleDrawImage(bulletVPos, bulletHPos , &bulletTile, BULLET_HEIGHT);
		pthread_mutex_unlock(&screenMutex);}
	
}//end drawBullet


bool checkPlayerHit(int bulletHPos, int bulletVPos, bool* alive){
	/*This method checks if the enemy bullet has collided with the player. */
	extern int playerTopPos;
	extern int playerLeftPos;
	bool result = false;
	
	if(*alive){
		//Checks if the bullet has collided with the player and returns true
		if(bulletVPos == playerTopPos || bulletVPos == playerTopPos + DOWN){
			if(bulletHPos == playerLeftPos || bulletHPos == playerLeftPos+RIGHT || bulletHPos == playerLeftPos+RIGHT+RIGHT){
				result = true;
			}
		}
	}
	
	
	return result;
}//end checkPlayerHit

bool checkEnemyHit(int bulletHPos, int bulletVPos, bool* alive, node_t *currCaterpillar){
	/*This method checks if the player bullet has collided with the enemy*/
	extern node_t *caterPillarList;
	extern int numEnemy;
	
	bool result = false;
	int hitPoint;
	int remainingBody;

		//Check if bullet is in the same row as the head of a caterpillar
		if(bulletVPos == currCaterpillar->args.headVerPos+DOWN){ 
			if(currCaterpillar->args.goRight){ //caterpillar is going right
				//Check if the bullet is in the columns that the caterpillar occupies
				if(bulletHPos <= currCaterpillar->args.headHorPos && bulletHPos >= ( currCaterpillar->args.headHorPos - ( currCaterpillar->args.numSegments + currCaterpillar->args.wrappedBody))){
					
					//Find where the caterpillar is hit and adjust its number of segments
					hitPoint = currCaterpillar->args.headHorPos - bulletHPos;
					remainingBody = bulletHPos - (currCaterpillar->args.headHorPos - currCaterpillar->args.numSegments);
					currCaterpillar->args.numSegments = hitPoint;
					
					//If the new length of the caterpillar after being hit is more than the minimum
					//Draw a new caterpillar from where bullet collided that has the length of the segments lost
					if(remainingBody > ENEMY_MIN_SEGMENTS){
						struct caterPillarArgs enemyArgs;
						enemyArgs.headVerPos = currCaterpillar->args.headVerPos;
						enemyArgs.headHorPos = currCaterpillar->args.headHorPos-currCaterpillar->args.numSegments-1;
						enemyArgs.numSegments = remainingBody;
						enemyArgs.alive = true;
						enemyArgs.wrappedBody = 0;
						enemyArgs.goRight = true;
						enemyArgs.speed = ENEMY_DEFAULT_SPEED;
						
						//Add this new caterpillar to the lsit
						pthread_mutex_lock(&cListMutex);	
						appendList(enemyArgs, caterPillarList );
						pthread_mutex_unlock(&cListMutex);
					
						//Increase live enemy count
						pthread_mutex_lock(&numEnemyMutex);
						numEnemy++;
						pthread_mutex_unlock(&numEnemyMutex);}
					
					result = true;
				}
			}//Going right, bullet at head level
			else{ //caterpillar is going left
				//Check if the bullet is in the columns that the caterpillar occupies
				if(bulletHPos >= currCaterpillar->args.headHorPos && bulletHPos <= ( currCaterpillar->args.headHorPos + ( currCaterpillar->args.numSegments - currCaterpillar->args.wrappedBody))){

					//Find where the caterpillaris hit and adjust its number of segments
					hitPoint =  bulletHPos-currCaterpillar->args.headHorPos;
					remainingBody = (currCaterpillar->args.headHorPos + currCaterpillar->args.numSegments) - bulletHPos;
					currCaterpillar->args.numSegments = hitPoint;
					
					//If the new length of the caterpillar after being hit is more than the minimum
					//Draw a new caterpillar from where bullet collided that has the length of the segments lost
					if(remainingBody > ENEMY_MIN_SEGMENTS){
						struct caterPillarArgs enemyArgs;
						enemyArgs.headVerPos = currCaterpillar->args.headVerPos;
						enemyArgs.headHorPos = currCaterpillar->args.headHorPos+currCaterpillar->args.numSegments+1;
						enemyArgs.numSegments = remainingBody;
						enemyArgs.alive = true;
						enemyArgs.wrappedBody = 0;
						enemyArgs.goRight = false;
						enemyArgs.speed = ENEMY_DEFAULT_SPEED;
					
						//Add this new caterpillar to the list
						pthread_mutex_lock(&cListMutex);	
						appendList(enemyArgs, caterPillarList );
						pthread_mutex_unlock(&cListMutex);

						//Increase live enemy count
						pthread_mutex_lock(&numEnemyMutex);
						numEnemy++;
						pthread_mutex_unlock(&numEnemyMutex);}
					
					result = true;
				}
			}//Going left, bullet at head level			
		}//Bullet at head Level Check
		
		//Check if the bullet is hitting the wrapped segments
		else if(bulletVPos == currCaterpillar->args.headVerPos+UP && currCaterpillar->args.wrappedBody != 0){
			if(!currCaterpillar->args.goRight){ //caterpillar is going left, wrappedBody going right
				//Check if the bullet is in the columns that the wrapped body segments occupy
				if(bulletHPos <= COL_END && bulletHPos >= (COL_END - currCaterpillar->args.wrappedBody)){
					
					//Find where the caterpillar is hit and adjust its number of segments
					hitPoint = bulletHPos - (COL_END-currCaterpillar->args.wrappedBody);
					remainingBody = currCaterpillar->args.wrappedBody;
					currCaterpillar->args.wrappedBody = currCaterpillar->args.wrappedBody - hitPoint;
					remainingBody = remainingBody - currCaterpillar->args.wrappedBody;
					currCaterpillar->args.numSegments = currCaterpillar->args.numSegments - hitPoint;
					
					
					//If the new length of the caterpillar after being hit is more than the minimum
					//Draw a new caterpillar from where bullet collided that has the length of the segments lost
					if(remainingBody > ENEMY_MIN_SEGMENTS){
						struct caterPillarArgs enemyArgs;
						enemyArgs.headVerPos = currCaterpillar->args.headVerPos+UP+UP;
						enemyArgs.headHorPos = COL_END -  currCaterpillar->args.wrappedBody;
						enemyArgs.numSegments = remainingBody;
						enemyArgs.alive = true;
						enemyArgs.wrappedBody = 0;
						enemyArgs.goRight = true;
						enemyArgs.speed = ENEMY_DEFAULT_SPEED;
						
						//Add this new caterpillar to the list
						pthread_mutex_lock(&cListMutex);	
						appendList(enemyArgs, caterPillarList );
						pthread_mutex_unlock(&cListMutex);
					
						//Increase live enemy count
						pthread_mutex_lock(&numEnemyMutex);
						numEnemy++;
						pthread_mutex_unlock(&numEnemyMutex);}
					
					result = true;
					
				}
			}//Going left, Wrapped segments going right, bullet at wrapped level
			else{//caterpillar is going left
				//Check if the bullet is in the columns that the wrapped body segments occupy
				if(bulletHPos >= COL_START && bulletHPos <= (COL_START + currCaterpillar->args.wrappedBody)){
					
					//Find where the caterpillar is hit and adjust its number of segments
					hitPoint = (COL_START+currCaterpillar->args.wrappedBody) - bulletHPos;
					remainingBody = currCaterpillar->args.wrappedBody;
					currCaterpillar->args.wrappedBody = currCaterpillar->args.wrappedBody - hitPoint;
					remainingBody = remainingBody - currCaterpillar->args.wrappedBody;
					currCaterpillar->args.numSegments = currCaterpillar->args.numSegments - hitPoint;
					
					//If the new length of the caterpillar after being hit is more than the minimum
					//Draw a new caterpillar from where bullet collided that has the length of the segments lost
					if(remainingBody > ENEMY_MIN_SEGMENTS){
						struct caterPillarArgs enemyArgs;
						enemyArgs.headVerPos = currCaterpillar->args.headVerPos+UP+UP;
						enemyArgs.headHorPos = COL_START +  currCaterpillar->args.wrappedBody;
						enemyArgs.numSegments = remainingBody;
						enemyArgs.alive = true;
						enemyArgs.wrappedBody = 0;
						enemyArgs.goRight = false;
						enemyArgs.speed = ENEMY_DEFAULT_SPEED;
					
						//Add this new caterpillar to the list
						pthread_mutex_lock(&cListMutex);	
						appendList(enemyArgs, caterPillarList );
						pthread_mutex_unlock(&cListMutex);

						//Increase live enemy count
						pthread_mutex_lock(&numEnemyMutex);
						numEnemy++;
						pthread_mutex_unlock(&numEnemyMutex);}
					
					result = true;
				}			
			}//Going right, wrapped segments going left, bullet at wrapped level			
		}//Bullet is at the level of the wrapped segments
				
		else{
			//Bullet did not hit anything, return
			result = false;
		}
		
	return result;
}//end checkEnemyHit


/*=======================================BULLET.C END======================================================*/
/*=========================================================================================================*/
