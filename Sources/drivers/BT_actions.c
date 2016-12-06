/*
 * BT_actions.c
 *
 *  Created on: Dec 23, 2015
 *      Author: Carlos Miguens
 */

#include "BT_actions.h"
#include "BT.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "FRTOS1.h"

#define BTinputBufferMAXLEGHT 64
#define OPTIONMENUBASE 'a'

xSemaphoreHandle xSemaphoreBTWrite, xSemaphoreBTRead;
char BTinputBuffer[BTinputBufferMAXLEGHT];
char BTinputBufferCursor;
char menuMaxOption;

void BT_init()
{
	xSemaphoreBTWrite = xSemaphoreCreateBinary();
	xSemaphoreBTRead = xSemaphoreCreateBinary();
	xSemaphoreGive(xSemaphoreBTWrite);
	BT_logMode = logMode_DEBUG;
}

void BT_setLogMode(char mode)
{
	BT_logMode = mode;
}

void BT_sendSaltoLinea()
{
	BT_SendChar('\r');
	BT_SendChar('\n');
	FRTOS1_vTaskDelay(5/portTICK_RATE_MS);
}

void BT_showString(char *text)
{
	char *buff = text;
	xSemaphoreTake(xSemaphoreBTWrite, portMAX_DELAY);
	while (*buff != '\0')
	{
		BT_SendChar(*buff);
		buff++;
	}
	xSemaphoreGive(xSemaphoreBTWrite);
}

void BT_showLog(char *text)
{
	xSemaphoreTake(xSemaphoreBTWrite, portMAX_DELAY);
	if (BT_logMode == logMode_DEBUG)
		BT_showString(text);
	xSemaphoreGive(xSemaphoreBTWrite);
}

void BT_showLogChar(char data)
{
	xSemaphoreTake(xSemaphoreBTWrite, portMAX_DELAY);
	if (BT_logMode == logMode_DEBUG)
		BT_SendChar(data);
	xSemaphoreGive(xSemaphoreBTWrite);
}

int BT_showMenu(char text[MENUMAXLENGHT][64], char *reply)
{
	char opcion[MENUMAXLENGHT];
	int i = 0;
	BT_sendSaltoLinea();
	BTinputBufferCursor= 0;
	BTinputBuffer[0] = '\0';
	while (*text[i] != '\0')
	{
		opcion[0] = OPTIONMENUBASE+i;
		opcion[1] = ')';
		opcion[2] = ' ';
		opcion[3] = '\0';
		BT_showString(&opcion[0]);
		BT_showString(text[i]);
		BT_sendSaltoLinea();
		i++;
	}
	BT_mode = MENU;
	BT_sendSaltoLinea();
	strcpy(&opcion[0], "[1-3]: ");
	opcion[1] = OPTIONMENUBASE;
	opcion[3] = OPTIONMENUBASE+i-1;
	menuMaxOption = opcion[3];
	BT_showString(opcion);
	if (!xSemaphoreTake(xSemaphoreBTRead, 5000/portTICK_RATE_MS))
	{
		return -69;
	}
	strcpy(reply, text[BTinputBuffer[0] - OPTIONMENUBASE]);
	return BTinputBuffer[0] - OPTIONMENUBASE; // return selected index
}

void BT_askValue(char *text, char *value)
{
	BT_mode = INPUT;
	BTinputBufferCursor = 0;
	BTinputBuffer[BTinputBufferCursor] = '\0';
	BT_showString(text);
	xSemaphoreTake(xSemaphoreBTRead, portMAX_DELAY);
	strcpy(value, &BTinputBuffer[0]);
}


void BT_inputChar(char data)
{
	if ((BT_mode != MENU) && (BT_mode != INPUT))
		return;

	if (data != '\n')
	{
		BT_SendChar(data);
		BTinputBuffer[BTinputBufferCursor] = data;
		BTinputBufferCursor++;
		if ((BTinputBufferCursor >= BTinputBufferMAXLEGHT) || (data == '?') || ((BT_mode == MENU) && ((data < OPTIONMENUBASE) || (data > menuMaxOption))))
		{
			BTinputBufferCursor = 0;
		}
		BTinputBuffer[BTinputBufferCursor] = '\0';
	}


	if ((data == '\n') || (data == '?') ||
			((BT_mode == MENU) && ((data >= OPTIONMENUBASE) && (data <= menuMaxOption))))
	{
		xSemaphoreGive(xSemaphoreBTRead);
		return;
	}


}

