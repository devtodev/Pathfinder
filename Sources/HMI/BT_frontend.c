/*
 * BT_frontend.c
 *
 *  Created on: 18/12/2016
 *      Author: Carlos Miguens
 */

#include "../drivers/BT_actions.h"

char menuBT[8][64] = {"w) + Speed\n\0",
					"s) - Speed\n\0",
					"a) Turn left\n\0",
					"d) Turn right\n\0",
					"i) Forward\n\0",
					"k) Backward\n\0",
					"j) Rotate left\n\0",
					"l) Rotate right\n\0",};

// increase or decrease the speed
void move_SpeedMore();
void move_SpeedLess();
void move_turnLeft();
void move_turnRight();

// set the direction of the motors
void move_Forward();
void move_Backward();
void move_Rotate(int direction, int angle);



void hmi_bt_init()
{
	for (int i = 0; i < 30; i++)
		BT_sendSaltoLinea();
	BT_showString("Menu Robot");
	BT_sendSaltoLinea();
	BT_sendSaltoLinea();
}

char hmi_bt_getOption()
{
	int i = 0;
	char bufferInput[2];
	for (i = 0; i < 8; i++)
	{
		BT_showString(menuBT[i]);
		BT_sendSaltoLinea();
	}
	BT_askValue("Select an option: ", &bufferInput);
	return bufferInput[0];
}
