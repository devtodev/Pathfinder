/*
 * moves.c
 *
 *  Created on: 12/12/2016
 *      Author: Carlos Miguens
 */

#include <Driver/motor.h>
#include <Controller/moves.h>
#include "FRTOS1.h"
#include "string.h"
#include "math.h"

#define MAXNUMSTRLEN 30

int speed_Left = 0;
int speed_Right = 0;
int direction_Left = RELEASE;
int direction_Right = RELEASE;


void move_SpeedRefresh()
{
	motorSetSpeed(MOTORLEFT, (~speed_Left) * 0xFFF * 2);
	motorSetSpeed(MOTORRIGHT, (~speed_Right) * 0xFFF * 2);
}

void move_DirectionRefresh()
{
	motorDirection(MOTORLEFT, direction_Left);
	motorDirection(MOTORRIGHT, direction_Right);
}


void orientation2string(Orientation orientation, char *str)
{
	char temp[MAXNUMSTRLEN];
	strcpy(str, "[\0");
	UTIL1_NumFloatToStr(&temp[0], MAXNUMSTRLEN, orientation.magneticFields[0], 2);
	strcat(&str[0], &temp[0]);
	strcat(&str[0], ", ");
	UTIL1_NumFloatToStr(&temp[0], MAXNUMSTRLEN, orientation.magneticFields[1], 2);
	strcat(&str[0], &temp[0]);
	strcat(&str[0], ", ");
	UTIL1_NumFloatToStr(&temp[0], MAXNUMSTRLEN, orientation.magneticFields[2], 2);
	strcat(&str[0], &temp[0]);
	strcat(&str[0], "]\0");
}


void angles2string(Orientation orientation, char *str)
{
	char temp[MAXNUMSTRLEN];
	strcpy(str, "[\0");
	UTIL1_NumFloatToStr(&temp[0], MAXNUMSTRLEN, orientation.Psi, 2);
	strcat(&str[0], &temp[0]);
	strcat(&str[0], ", ");
	UTIL1_NumFloatToStr(&temp[0], MAXNUMSTRLEN, orientation.The, 2);
	strcat(&str[0], &temp[0]);
	strcat(&str[0], ", ");
	UTIL1_NumFloatToStr(&temp[0], MAXNUMSTRLEN, orientation.Phi, 2);
	strcat(&str[0], &temp[0]);
	strcat(&str[0], "]\0");
}

void point2string(int16_t point[], char *str)
{
	char temp[MAXNUMSTRLEN];
	strcpy(str, "{\0");
	UTIL1_Num16sToStr(&temp[0], MAXNUMSTRLEN, point[0]);
	strcat(&str[0], &temp[0]);
	strcat(&str[0], ", ");
	UTIL1_Num16sToStr(&temp[0], MAXNUMSTRLEN, point[1]);
	strcat(&str[0], &temp[0]);
	strcat(&str[0], ", ");
	UTIL1_Num16sToStr(&temp[0], MAXNUMSTRLEN, point[2]);
	strcat(&str[0], &temp[0]);
	strcat(&str[0], "}\0");
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
	speed_Left = (speed_Left<5)?speed_Left+1:speed_Left;
}

void move_SpeedLeftDecrease()
{
	speed_Left = (speed_Left>0)?speed_Left-1:speed_Left;
}

void move_SpeedRightIncrease()
{
	speed_Right = (speed_Right<5)?speed_Right+1:speed_Right;
}

void move_SpeedRightDecrease()
{
	speed_Right = (speed_Right>0)?speed_Right-1:speed_Right;
}

// increase or decrease the speed
void move_SpeedUp()
{
	move_SpeedLeftIncrease();
	move_SpeedRightIncrease();
	move_SpeedRefresh();
}

void move_SpeedDown()
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
	move_DirectionRefresh();
}

void move_Backward()
{
	direction_Left = BACKWARD;
	direction_Right = BACKWARD;
	move_DirectionRefresh();
}

void move_stop()
{
	move_SpeedRefresh();
	while ((speed_Left != 0) && (speed_Right != 0))
	{
		if (speed_Left != 0) speed_Left = (speed_Left < 0)?speed_Left+1:speed_Left-1;
		if (speed_Right != 0) speed_Right = (speed_Right < 0)?speed_Right+1:speed_Right-1;
		move_SpeedRefresh();
		vTaskDelay(50/portTICK_RATE_MS);
	}
	direction_Left = RELEASE;
	direction_Right = RELEASE;
	move_DirectionRefresh();
}

void move_Rotate(int direction, int angle)
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
	move_DirectionRefresh();
}

