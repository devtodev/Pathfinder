/*
 * Navigation.h
 *
 *  Created on: 30/12/2016
 *      Author: karl
 */

#include "Navigation.h"
#include "ActionManager.h"
#include "moves.h"

#define MAXSTEPSCIRCULARTAIL 20

Position targets[MAXSTEPSCIRCULARTAIL];
int iTarget = -1, lastTarget = -1;

int checkingMeetTheTarget();

int checkingMeetTheTarget()
{
	// TODO:
	return 0;
}

int travelMeetTheTarget()
{
	if (iTarget < 0) return iTarget;
	if (checkingMeetTheTarget())
	{
		pushAction(MOVE_STOP);
		iTarget++;
		if (iTarget >= lastTarget)
		{
			iTarget = -1;
			lastTarget = -1;
		}
	}
	return iTarget;
}

int travelAddNewTarget(Position newTarget)
{
	lastTarget++;
	targets[lastTarget] = newTarget;
	return lastTarget;
}

int travelInit()
{
	if (lastTarget < 0) return lastTarget;
	iTarget = (iTarget < 0)? 0 : iTarget;
	return iTarget;
}
