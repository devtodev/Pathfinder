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

void hmi_bt_init()
{
	int i;
	for (i = 0; i < 30; i++)
		BT_sendSaltoLinea();
	BT_showString("Menu Robot");
	BT_sendSaltoLinea();
	BT_sendSaltoLinea();
	for (i = 0; i < 8; i++)
	{
		BT_showString(menuBT[i]);
		BT_sendSaltoLinea();
	}
}

char hmi_bt_getOption()
{
	char bufferInput[2];
	BT_askValue("\r", &bufferInput[0]);
	return bufferInput[0];
}
