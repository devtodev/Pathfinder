/*
 * moves.c
 *
 *  Created on: 12/12/2016
 *      Author: Carlos Miguens
 */

#include "moves.h"
#include "../drivers/motor.h"

#define LEFT 1
#define RIGHT 0

int speed_Left = 0;
int speed_Right = 0;
int direction_Left = RELEASE;
int direction_Right = RELEASE;


void move_SpeedRefresh()
{
	int K = 0xFFF;
	motorSetSpeed(MOTORLEFT, speed_Left * K);
	motorSetSpeed(MOTORRIGHT, speed_Right * K);
}

void move_SpeedLeftIncrease()
{
	speed_Left = (speed_Left<10)?speed_Left+1:speed_Left;
}

void move_SpeedLeftDecrease()
{
	speed_Left = (speed_Left>-10)?speed_Left-1:speed_Left;
}

// increase or decrease the speed
void move_SpeedMore()
{
	move_SpeedLeftIncrease();
	move_SpeedRightIncrease();
	move_SpeedRefresh();
}

void move_SpeedLess()
{
	move_SpeedLeftDecrease();
	move_SpeedRightDecrease();
	move_SpeedRefresh();
}

void move_turnLeft()
{
	move_SpeedLeftDecrease();
	move_SpeedRefresh();
}

void move_turnRight()
{
	move_SpeedRightDecrease();
	move_SpeedRefresh();
}

// set the direction of the motors
void move_Forward()
{
	direction_Left = FORWARD;
	direction_Right = FORWARD;
}

void move_Backward()
{
	direction_Left = BACKWARD;
	direction_Right = BACKWARD;
}

void move_Rotate(int direction)
{
	switch(direction)
	{
		case LEFT:
			direction_Left = BACKWARD;
			direction_Right = FORWARD;
			break;
		case RIGHT:
			direction_Left = FORWARD;
			direction_Right = BACKWARD;
			break;
	}
}

