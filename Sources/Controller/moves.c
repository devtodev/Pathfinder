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
#include "Driver/acelerometro.h"

#define MAXNUMSTRLEN 30
#define CANTSPEEDS 2

int speed_Left = 0;
int speed_Right = 0;
int direction_Left = RELEASE;
int direction_Right = RELEASE;


void move_correction()
{
	/* TODO: When go forward or backward, check the course and, if necessary, make a correction
	if ((currentAction.type == GOFORDWARD) || (currentAction.type == GOBACKWARD))
	{
		// PID controller for the direction
	} */
}

void move_SpeedRefresh()
{
	int speed = (speed_Right == CANTSPEEDS)? 0: 0xFFFF / (speed_Right + 1);
	motorSetSpeed(MOTORRIGHT, speed);
	speed = (speed_Left == CANTSPEEDS)? 0: 0xFFFF / (speed_Left + 1);
	motorSetSpeed(MOTORLEFT, speed);
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

int getRealSpeed()
{
	int speed = 0;
	speed = (gforceXYZ[0] < 0)? speed + gforceXYZ[0] * -1:speed + gforceXYZ[0];
	speed = (gforceXYZ[1] < 0)? speed + gforceXYZ[1] * -1:speed + gforceXYZ[1];
//	speed = (gforceXYZ[2] < 0)? speed + gforceXYZ[2] * -1:speed + gforceXYZ[2];
	return speed;
}

void point2string(int16_t point[], char *str)
{
	char temp[MAXNUMSTRLEN];
	strcpy(str, "{\0");
/*	UTIL1_Num16sToStr(&temp[0], MAXNUMSTRLEN, point[0]);
	strcat(&str[0], &temp[0]);
	strcat(&str[0], ", ");
	UTIL1_Num16sToStr(&temp[0], MAXNUMSTRLEN, point[1]);
	strcat(&str[0], &temp[0]);
	strcat(&str[0], ", ");
	UTIL1_Num16sToStr(&temp[0], MAXNUMSTRLEN, point[2]);
	strcat(&str[0], &temp[0]);
	strcat(&str[0], ", ");*/
	UTIL1_Num16sToStr(&temp[0], MAXNUMSTRLEN, getRealSpeed(point));
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
	speed_Left = (speed_Left<CANTSPEEDS)?speed_Left+1:speed_Left;
}

void move_SpeedLeftDecrease()
{
	speed_Left = (speed_Left>0)?speed_Left-1:speed_Left;
}

void move_SpeedRightIncrease()
{
	speed_Right = (speed_Right<CANTSPEEDS)?speed_Right+1:speed_Right;
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
	move_stop();
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

	if (angle > 0)
	{
		int rotationTime = (angle / 90) * 666;
		move_SpeedUp();
		vTaskDelay(rotationTime/portTICK_RATE_MS);
		move_stop();
	}

}
