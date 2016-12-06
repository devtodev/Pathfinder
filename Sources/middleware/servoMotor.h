/*
 * motor.h
 *
 *  Created on: 17/10/2016
 *      Author: Carlos Miguens
 */


void setServoOnLeft();
void setServoOnRight();
void setServoOnCenter();

void getServoPanoramic();

void setServoAngle(int newAngle);
int getServoAngle();

void setServoSpeed(int newSpeed);
void getServoSpeed();
void servoInit();
