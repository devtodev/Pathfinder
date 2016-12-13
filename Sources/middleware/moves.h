/*
 * moves.h
 *
 *  Created on: 12/12/2016
 *      Author: Carlos Miguens
 */

#define LEFT 1
#define RIGHT 0

// increase or decrease the speed
void move_SpeedMore();
void move_SpeedLess();
void move_turnLeft();
void move_turnRight();

// set the direction of the motors
void move_Forward();
void move_Backward();
void move_Rotate(int direction);

void move_init();
