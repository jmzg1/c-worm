/*James Guan ==============================================================================================
7745303  ==================================================================================================
COMP3430 A2 ===============================================================================================
===========================================LLIST.C========================================================*/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>


#include "llist.h"
#include "enemy.h"
#include "bullet.h"

extern pthread_mutex_t bListMutex;
extern pthread_mutex_t cListMutex;


struct node* newList(caterPillarArgs_t args){
	/* This method creates a new linked list and returns a pointer
	to the head of the linked list*/
	
	//Lock down the list 
	pthread_mutex_lock(&cListMutex);
	node_t *temp = (node_t*) malloc(sizeof(node_t));
	
	
	if( temp == NULL){
		printf("\n Cannot create Node!");
	}
	

	//Assign the parameters of the node
	temp->args = args;
	temp->next = NULL;
	
	//Start thread for new caterpillar
	pthread_create(&(temp->thread), NULL, newCaterpillar ,&(temp->args));
	
	pthread_mutex_unlock(&cListMutex);
	return temp; //This should be assigned to the head
}//end newList

struct node2* newList2(bulletArgs_t args){
	/* This method creates a new linked list and returns a pointer
	to the head of the linked list. Only call this method if protected by mutex*/

	node2_t *temp = (node2_t*) malloc(sizeof(node2_t));
	
	
	if( temp == NULL){
		printf("\n Cannot create Node!");
	 }
	
	//Assign the parameters of the node
	temp->args = args;
	temp->next = NULL;
	
	//Start thread
	pthread_create(&(temp->thread), NULL, newBullet ,&(temp->args));
	
	return temp; //This should be assigned to the head
}//end newList2


void appendList(caterPillarArgs_t args, node_t *head ){
	/*This method creates a new node (for caterpillar) at the end of the list.
	Only call this method if protected by mutex*/
	
	node_t *currNode = head;
	
	
	//Go to end of list 
	while(currNode != NULL && currNode-> next != NULL){
		currNode = currNode->next;
	}
	if(currNode == NULL){return;}
	
	//Make new Node
	currNode->next =  malloc(sizeof(node_t));
	
	if(currNode->next == NULL){
		printf("/n Cannot create Node!");
	}
	
	//Initialize node values and add it to end of list
	currNode->next->args = args;
	currNode->next->next = NULL;
	
	//Start thread
	pthread_create(&(currNode->next->thread), NULL, newCaterpillar,&(currNode->next->args));
	
}//end appendList


void appendList2(bulletArgs_t args, node2_t *head ){
	/*This method creates a new node (for bullet) at the end of the list.
	Only call this method if protected by mutex*/

	node2_t *currNode = head;
	
	
	//Go to end of list 
	while( currNode != NULL&&currNode-> next != NULL ){
		currNode = currNode->next;
	}
	if(currNode == NULL){return;}
	
	//Make new Node
	currNode->next = malloc(sizeof(node2_t));
	
	if(currNode->next == NULL){
		printf("/n Cannot create Node!");
	}
	
	//Initialize node values and add it to end of list
	currNode->next->args = args;
	currNode->next->next = NULL;
	
	//Start thread
	pthread_create(&(currNode->next->thread), NULL, newBullet,&(currNode->next->args));
}//end appendList2


struct node* goToIndex(int index, node_t *head){
	/*This method goes to a specified element in the list and returns that node*/
	pthread_mutex_lock(&cListMutex);
	node_t *currNode;
	int indexCounter;
	
	currNode = head;
	indexCounter = 0;
	
	//Go to the index specified 
	while((indexCounter < index) && (currNode->next != NULL)){
		currNode = currNode->next;
		indexCounter++;
	}
	
	pthread_mutex_unlock(&cListMutex);
	return currNode;
	
}//end goToIndex

struct node2* goToIndex2(int index, node2_t *head){
	/*This method goes to the specified element in the lsit and returns that node*/
	pthread_mutex_lock(&bListMutex);
	node2_t *currNode;
	int indexCounter;
	
	currNode = head;
	indexCounter = 0;
	
	//Go to the index required
	while((indexCounter < index) && (currNode->next != NULL)){
		currNode = currNode->next;
		indexCounter++;
	}
	pthread_mutex_unlock(&bListMutex);
	return currNode;
	
}//end goToIndex2

int findNode(node_t *head, node_t *nodeToFind){
	/*This method finds the given node in the linked list based on the 
	Thread ID in a given node. This method returns the index of that node
	*/
	pthread_mutex_lock(&cListMutex);
	node_t *currNode;
	int indexCounter;
	int result = -1;
	currNode = head;
	indexCounter = 0;
	
	
	while(currNode != NULL){
		//Compare the threadIds  if they match, and record that index
		if(currNode->thread == nodeToFind->thread){
			result = indexCounter;
			break;
		}
		else{
			currNode = currNode->next;
			indexCounter++;
		}
	}
	pthread_mutex_unlock(&cListMutex);
	
	return result;
}//end findNode

int findNode2(node2_t *head, node2_t *nodeToFind){
	/*This method finds the given node in the linked list based on the 
	Thread ID in a given node. This method returns the index of that node
	*/
	pthread_mutex_lock(&bListMutex);
	node2_t *currNode;
	int indexCounter;
	int result = -1;
	currNode = head;
	indexCounter = 0;
	
	
	while(currNode->next != NULL){
		//Compare the threadIds  if they match, and record that index
		if(currNode->thread == nodeToFind->thread){
			
			result = indexCounter;
			break;
		}
		else{
			currNode = currNode->next;
			indexCounter++;
		}
	}
	pthread_mutex_unlock(&bListMutex);
	
	return result;
}//end findNode2

int deleteFromList(int index, node_t ** head){
	/*This method finds the node given by the index and removes that from the list.
	Returns 0 if the deletion was successful*/
	pthread_mutex_lock(&cListMutex);
	struct node* currNode;
	struct node* prevNode;
	int indexCounter;
	int result;
	
	currNode = *head;
	prevNode = NULL;
	indexCounter = 0;
	result = -1;
	
	//List is empty
	if(*head==NULL){
		result = -1;
	}
	
	//Search for element to be deleted
	for(indexCounter = 0; indexCounter < index; indexCounter++){
		
		//If last Node
		if(currNode->next == NULL){
			break;
		}
		else{
			//Move to next node
			prevNode = currNode;
			currNode = currNode->next;
			
		}
	}
	//If node to be deleted is head
	if(currNode == *head){
		currNode = (*head);
		prevNode = (*head)-> next;
		pthread_join((currNode)->thread, NULL);	
		free(currNode);
		*head = prevNode;
		result = 0;
	}
	//If node to be deleted is tail
	else if(currNode -> next == NULL){
		pthread_join(currNode->thread, NULL);
		prevNode->next = NULL;
		free(currNode);
		currNode = NULL;
		result = 0;
	}
	//Else must be middle
	else{
		pthread_join(currNode->thread, NULL);
		prevNode->next = currNode->next;		
		free(currNode);
		currNode = NULL;
		result = 0;
	}
	pthread_mutex_unlock(&cListMutex);
	return result;
}//end deleteFromList


int deleteFromList2(int index, node2_t ** head){
	/*This method finds the node given by the index and removes that from the list.
	Returns 0 if the deletion was successful*/
	pthread_mutex_lock(&bListMutex);
	struct node2* currNode;
	struct node2* prevNode;
	int indexCounter;
	int result;
	
	currNode = *head;
	prevNode = NULL;
	indexCounter = 0;
	result = -1;
	
	//List is empty
	if(*head==NULL){
		result = -1;
	}
	
	//Search for element to be deleted
	for(indexCounter = 0; indexCounter < index; indexCounter++){
		
		//If last Node
		if(currNode->next == NULL){
			//break;
		}
		else{
			//Move to next node
			prevNode = currNode;
			currNode = currNode->next;
			
		}
	}
	//If node to be deleted is head
	if(currNode == *head){
		currNode = (*head);
		prevNode = (*head)-> next;
		pthread_join((currNode)->thread, NULL);
		free(currNode);
		*head = prevNode;
		result = 0;
	}
	//If node to be deleted is tail
	else if(currNode -> next == NULL){
		pthread_join(currNode->thread, NULL);
		prevNode->next = NULL;
		free(currNode);
		currNode = NULL;
		result = 0;
	}
	//Else must be middle
	else{
		pthread_join(currNode->thread, NULL);
		prevNode->next = currNode->next;
		free(currNode);
		currNode = NULL;
		result = 0;
	}
	pthread_mutex_unlock(&bListMutex);
	return result;
}//end deleteFromList2

void deleteBulletList(node2_t *listToDelete){
	/*This method is used to delete all the elements in the given linked list*/
	node2_t *currNode = listToDelete;
	
	while(currNode != NULL){
		deleteFromList2(0, &currNode);
	}
}//end deleteBulletList

void deleteCaterpillarList(node_t *listToDelete){
	/*This method is used to delete all the elements in the given linked list*/
	node_t *currNode = listToDelete;
	
	while(currNode != NULL){
		deleteFromList(0, &currNode);
	}
	
}//end deleteCaterpillarList

/*=======================================LLIST.C END=======================================================*/
/*=========================================================================================================*/