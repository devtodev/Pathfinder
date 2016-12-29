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

/* multiplicative conversion constants */
#define DegToRad 0.017453292F
#define RadToDeg 57.295779F
/* NED tilt-compensated e-Compass function */
void eCompass(float Bx, float By, float Bz, float Gx, float Gy, float Gz)
{
	float sinAngle, cosAngle; /* sine and cosine */
	float Bfx, Bfy, Bfz; /* calibrated mag data in uT after tilt correction */
	/* subtract off the hard iron interference computed using equation 9 */
	Bx -= Vx;
	By -= Vy;
	Bz -= Vz;
	/* calculate roll angle Phi (-180deg, 180deg) and sin, cos */
	Phi = atan2(Gy, Gz) * RadToDeg; /* Equation 2 */
	sinAngle = sin(Phi * DegToRad); /* sin(Phi) */
	cosAngle = cos(Phi * DegToRad); /* cos(Phi) */
	/* de-rotate by roll angle Phi */
	Bfy = By * cosAngle - Bz * sinAngle; /* Equation 5 y component */
	Bz = By * sinAngle + Bz * cosAngle; /* Bz=(By-Vy).sin(Phi)+(Bz-Vz).cos(Phi) */
	Gz = Gy * sinAngle + Gz * cosAngle; /* Gz=Gy.sin(Phi)+Gz.cos(Phi) */
	/* calculate pitch angle Theta (-90deg, 90deg) and sin, cos*/
	The = atan(-Gx / Gz) * RadToDeg; /* Equation 3 */
	sinAngle = sin(The * DegToRad); /* sin(Theta) */
	cosAngle = cos(The * DegToRad); /* cos(Theta) */
	/* de-rotate by pitch angle Theta */
	Bfx = Bx * cosAngle + Bz * sinAngle; /* Equation 5 x component */
	Bfz = -Bx * sinAngle + Bz * cosAngle; /* Equation 5 z component */
	/* calculate yaw = ecompass angle psi (-180deg, 180deg) */
	Psi = atan2(-Bfy, Bfx) * RadToDeg; /* Equation 7 */
	return;
}

void position2string(char *str)
{
	char temp[MAXNUMSTRLEN];
	strcpy(str, "[\0");
	UTIL1_NumFloatToStr(&temp[0], MAXNUMSTRLEN, Psi, 2);
	strcat(&str[0], &temp[0]);
	strcat(&str[0], ", ");
	UTIL1_NumFloatToStr(&temp[0], MAXNUMSTRLEN, The, 2);
	strcat(&str[0], &temp[0]);
	strcat(&str[0], ", ");
	UTIL1_NumFloatToStr(&temp[0], MAXNUMSTRLEN, Phi, 2);
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

