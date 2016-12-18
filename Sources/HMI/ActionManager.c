/*
 * ActionManager.c
 *
 *  Created on: 18/12/2016
 *      Author: Carlos Miguens
 */

#include "ActionManager.h"
#include "../middleware/moves.h"

#define MAXACTIONS 20

char actions[MAXACTIONS] = {};
int actionIndexLimit = 0;
int actionIndex		 = 0;

void initActions()
{
	actionIndexLimit = 0;
	actionIndex		 = 0;
}

void pushAction(char action)
{
	actions[actionIndexLimit] = action;
	actionIndexLimit = (actionIndexLimit+1 >= MAXACTIONS)?0:actionIndexLimit+1; // Circular Buffer
}

void doAction()
{
	if (actionIndex == actionIndexLimit) return;

	switch(actions[actionIndex])
	{
		case SPEED_UP:
			move_SpeedUp();
			break;
		case SPEED_DOWN:
			move_SpeedDown();
			break;
		case TURN_LEFT:
			move_turnLeft();
			break;
		case TURN_RIGHT:
			move_turnRight();
			break;
		case GOFORDWARD:
			move_Forward();
			break;
		case GOBACKWARD:
			move_Backward();
			break;
		case ROTATE_LEFT:
			move_Rotate(LEFT, 90);
			break;
		case ROTATE_RIGHT:
			move_Rotate(RIGHT, 90);
			break;
	    default:
			break;
	}

	actionIndex = (actionIndex+1 >= MAXACTIONS)?0:actionIndex+1; // Circular Buffer
}
