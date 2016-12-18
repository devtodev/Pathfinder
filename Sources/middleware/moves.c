/*
 * moves.c
 *
 *  Created on: 12/12/2016
 *      Author: Carlos Miguens
 */

#include "moves.h"
#include "../drivers/motor.h"
#include "FRTOS1.h"

#define LEFT 1
#define RIGHT 0
#define SMOTH_SPEEDCHANGE_MS 75

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

void move_DirectionRefresh()
{
	motorSetSpeed(MOTORLEFT, speed_Left);
	motorSetSpeed(MOTORRIGHT, speed_Right);
}


void move_init()
{
	int speed_Left = 0;
	int speed_Right = 0;
	int direction_Left = RELEASE;
	int direction_Right = RELEASE;
	move_DirectionRefresh();
	move_SpeedRefresh();
}

void move_SpeedLeftIncrease()
{
	speed_Left = (speed_Left<10)?speed_Left+1:speed_Left;
}

void move_SpeedLeftDecrease()
{
	speed_Left = (speed_Left>-10)?speed_Left-1:speed_Left;
}

void move_SpeedRightIncrease()
{
	speed_Right = (speed_Right<10)?speed_Right+1:speed_Right;
}

void move_SpeedRightDecrease()
{
	speed_Right = (speed_Right>-10)?speed_Right-1:speed_Right;
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
	if ((direction_Left == FORWARD) && (direction_Right == FORWARD))
		return;
	move_stop();
	FRTOS1_vTaskDelay(SMOTH_SPEEDCHANGE_MS/portTICK_RATE_MS);
	direction_Left = FORWARD;
	direction_Right = FORWARD;
	move_DirectionRefresh();
}

void move_Backward()
{
	if ((direction_Left == BACKWARD) && (direction_Right == BACKWARD))
		return;
	move_stop();
	FRTOS1_vTaskDelay(SMOTH_SPEEDCHANGE_MS/portTICK_RATE_MS);
	direction_Left = BACKWARD;
	direction_Right = BACKWARD;
	move_DirectionRefresh();
}

void move_stop()
{
	while ((speed_Left != 0) && (speed_Right != 0))
	{
		FRTOS1_vTaskDelay(SMOTH_SPEEDCHANGE_MS/portTICK_RATE_MS);
		if (speed_Left != 0) speed_Left = (speed_Left < 0)?speed_Left+1:speed_Left-1;
		if (speed_Right != 0) speed_Right = (speed_Right < 0)?speed_Right+1:speed_Right-1;
		move_SpeedRefresh();
	}
	FRTOS1_vTaskDelay(SMOTH_SPEEDCHANGE_MS/portTICK_RATE_MS);
	direction_Left = RELEASE;
	direction_Right = RELEASE;
	move_DirectionRefresh();
}

void move_Rotate(int direction, int angle)
{
	move_stop();
	FRTOS1_vTaskDelay(SMOTH_SPEEDCHANGE_MS/portTICK_RATE_MS);
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
	move_DirectionRefresh();
}
