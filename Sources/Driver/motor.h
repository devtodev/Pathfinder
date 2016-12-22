/*
 * motor.h
 *
 *  Created on: 17/10/2016
 *      Author: Carlos Miguens
 */

#define MOTOR1_A 2
#define MOTOR1_B 3
#define MOTOR2_A 1
#define MOTOR2_B 4
#define MOTOR3_A 5
#define MOTOR3_B 7
#define MOTOR4_A 0
#define MOTOR4_B 6

#define FORWARD -1
#define BACKWARD 1
#define RELEASE 0

#define MOTORLEFT  3
#define MOTORRIGHT 4

void motorsInit();
void motorSetSpeed(int motornum, int speed);
int motorDirection(int motornum, int cmd);
