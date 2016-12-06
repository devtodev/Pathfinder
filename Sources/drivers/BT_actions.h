/*
 * BT_actions.h
 *
 *  Created on: Dec 23, 2015
 *      Author: Carlos Miguens
 */

// logMode
#define logMode_DEBUG 1
#define logMode_PROD  2

// BTmode
#define DEBUG	0
#define MENU	1
#define INPUT	2

#define MENUMAXLENGHT	64

char BT_logMode; // DEBUG, PROD
char BT_mode;  // LOG, MENU, INPUT

void BT_init();
void BT_setLogMode(char mode);
void BT_showString(char *text);
void BT_showLog(char *text);
void BT_showLogChar(char data);
int BT_showMenu(char text[MENUMAXLENGHT][64], char *reply);
void BT_askValue(char *text, char *value);
void BT_inputChar(char data);
void BT_sendSaltoLinea();
