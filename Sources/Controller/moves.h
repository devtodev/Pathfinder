/*
 * moves.h
 *
 *  Created on: 12/12/2016
 *      Author: Carlos Miguens
 */
#include "stdint.h"

#define LEFT 1
#define RIGHT 0

#define DELAY_SPEED_CHANGE 		25
#define DELAY_DIRECTION_CHANGE  500

typedef struct {
	int16_t magneticFields[3];
	float Psi, The, Phi; /* yaw, pitch, roll angles in deg */
	float Vx, Vy, Vz; /* hard iron calibration coefficients */
} Orientation;


int speed_Left;
int speed_Right;
int direction_Left;
int direction_Right;

// increase or decrease the speed
void move_SpeedUp();
void move_SpeedDown();
void move_turnLeft();
void move_turnRight();

// set the direction of the motors
void move_Forward();
void move_Backward();
void move_Rotate(int direction, int angle);
void move_stop();

void move_init();
void point2string(int16_t *point, char *str);
void orientation2string(Orientation orientation, char *str);
void angles2string(Orientation orientation, char *str);
