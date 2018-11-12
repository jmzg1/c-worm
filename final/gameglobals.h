/*James Guan ==============================================================================================
7745303  ==================================================================================================
COMP3430 A2 ===============================================================================================
===========================================GAMEGLOBALS.H=================================================*/

#ifndef GAMEGLOBALS_H
#define GAMEGLOBALS_H

#include <stdbool.h>
#include <pthread.h>

//Board Parameters
#define GAME_ROWS 24
#define GAME_COLS 80
#define PLAYER_MAX_TOP 17
#define ENEMY_MAX_TOP 2
#define ROW_START 0
#define COL_START 0
#define ROW_END 23
#define COL_END 79
#define LIVES_POS 42
#define SCORE_POS 28
#define START_POINTS 0
#define HIT_SCORE 50
#define KILL_SCORE 150

//Keyboard Read Timing
#define KEYTIMEMS 500000
#define KEYTIMES  0

//Buffer Size
#define MAX_BUFFER 1000

//Refresh method Parameters
#define MIN_REFRESH_RATE 1
#define HALF 2 
#define DEAD 0
#define REGROUP_TIME 3



//Player Parameters
#define PLAYER_HEIGHT 3
#define PLAYER_BODY_ANIM_TILES 6
#define PLAYER_START_POSITIONH 40
#define PLAYER_START_POSITIONV 20
#define PLAYER_FRAMES 2
#define PLAYER_REFRESH_RATE 1
#define PLAYER_WIDTH 3
#define PLAYER_SHOOT_CD 20

//Caterpillar Default Parameters
#define ENEMY_HEIGHT 2
#define ENEMY_BODY_ANIM_TILES 7
#define ENEMY_START_POSITIONH 0
#define ENEMY_START_POSITIONV 2
#define ENEMY_FRAMES 2
#define ENEMY_REFRESH_RATE 1
#define ENEMY_DEFAULT_SEGMENTS 30
#define ENEMY_DEFAULT_SPEED 10
#define ENEMY_FRAME_SPEED 5
#define ENEMY_SHOOT_PROBABILITY 2
#define ENEMY_SHOOT_PROBABILITY_MAX 20
#define ENEMY_MIN_SEGMENTS 5
#define SEGMENT_LENGTH 1
#define GOING_LEFT 1
#define GOING_RIGHT 0
#define SPAWN_WAIT 10
#define SPAWN_SLEEP 150

//Bullet Parameters
#define DEFAULT_BULLET_SPEED 5
#define BULLET_HEIGHT 1
#define BULLET_LENGTH 1	

//Struct for Caterpillar
typedef struct caterPillarArgs{
	int headVerPos;
	int headHorPos;
	int numSegments;
	bool alive;
	int wrappedBody;
	bool goRight;
	int speed; 
    
}caterPillarArgs_t;

//Struct for Bullets
typedef struct bulletArgs{
	int bulletVerPos;
	int bulletHorPos;
	bool alive;
	bool playerBullet;
}bulletArgs_t;

//Node for Caterpillar
typedef struct node{
	struct node *next;
	pthread_t thread;
	caterPillarArgs_t args;
}node_t;

//Node for Bullets
typedef struct node2{
	struct node2 *next;
	pthread_t thread;
	bulletArgs_t args;
	
}node2_t;



#endif //GAMEGLOBALS_H


/*=====================================GAMEGLOBALS.H END===================================================*/
/*=========================================================================================================*/