/*
 * ActionManager.c
 *
 *  Created on: 18/12/2016
 *      Author: Carlos Miguens
 */

#define MAXACTIONS 20

char actions[MAXACTIONS] = {};
int actionIndex = 0;

void pushAction(char action)
{
	actions[actionIndex] = action;
	actionIndex = (actionIndex+1 >= MAXACTIONS)?0:actionIndex+1; // Circular Buffer
}
