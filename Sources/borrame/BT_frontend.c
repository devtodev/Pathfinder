/*
 * BT_frontend.c
 *
 *  Created on: 18/12/2016
 *      Author: Carlos Miguens
 */

#include <Driver/BT_actions.h>

#define SIZEBTMENU 9

char menuBT[SIZEBTMENU][64] = {"w) up Speed\r\n\0",
					"s) down Speed\r\n\0",
					"a) Turn left\r\n\0",
					"d) Turn right\r\n\0",
					"i) Forward\r\n\0",
					"k) Backward\r\n\0",
					"j) Rotate left\r\n\0",
					"l) Rotate right\r\n\0",
					"z) WiFi mode on\r\n\0",};

void hmi_bt_init()
{
	int i;
	for (i = 0; i < 30; i++)
		BT_sendSaltoLinea();
	BT_showString("Menu Robot");
	BT_sendSaltoLinea();
	BT_sendSaltoLinea();
	for (i = 0; i < SIZEBTMENU; i++)
		BT_sendSaltoLinea();
	for (i = 0; i < SIZEBTMENU; i++)
	{
		BT_showString(menuBT[i]);
		BT_sendSaltoLinea();
	}
}

char hmi_bt_getOption()
{
	char bufferInput[2];
	int i;
	for (i = 0; i < SIZEBTMENU; i++)
		BT_sendSaltoLinea();
	for (i = 0; i < SIZEBTMENU; i++)
	{
		BT_showString(menuBT[i]);
	}
	BT_askValue("\r", &bufferInput[0]);
	return bufferInput[0];
}
