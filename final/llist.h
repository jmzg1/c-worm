/*James Guan ==============================================================================================
7745303  ==================================================================================================
COMP3430 A2 ===============================================================================================
===========================================LLIST.H========================================================*/


#ifndef LLIST_H
#define LLIST_H
#include "gameglobals.h"



//Make a new List (one for Caterpillars, the other for bullets)
struct node *newList(caterPillarArgs_t args);
struct node2 *newList2(bulletArgs_t args);

//Go to the Node at the given index (one for Caterpillars, the other for bullets)
struct node *goToIndex(int index, node_t *head);
struct node2 *goToIndex2(int index, node2_t *head); 

//Find the node given in the list
int findNode(node_t *head, node_t *nodeToFind);
int findNode2(node2_t *head, node2_t *nodeToFind);

//Add a node at the end of the list (one for Caterpillars, the other for bullets)
void appendList(caterPillarArgs_t args, node_t *head );
void appendList2(bulletArgs_t args, node2_t *head);

//Delete a node from the list 
int deleteFromList(int index, node_t ** head);
int deleteFromList2(int index, node2_t **head);

//Delete all nodes from the list
void deleteBulletList(node2_t *listToDelete);
void deleteCaterpillarList(node_t *listToDelete);
#endif //LLIST.H



/*=======================================LLIST.H END=======================================================*/
/*=========================================================================================================*/