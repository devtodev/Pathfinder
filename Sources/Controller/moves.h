/*
 * moves.h
 *
 *  Created on: 12/12/2016
 *      Author: Carlos Miguens
 */

#define LEFT 1
#define RIGHT 0

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

void move_init();
