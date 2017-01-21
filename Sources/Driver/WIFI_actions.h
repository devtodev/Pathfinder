/*
 * BT_actions.c
 *
 *  Created on: Dec 22, 2015
 *      Author: Carlos Miguens
 */

#include "FreeRTOS.h"
#include "semphr.h"

#define WIFI_CONNECTED 		1
#define WIFI_DISCONNECTED	2
#define WIFI_CONNECTING		3
#define WIFI_ATMODE			4
#define MAXCANTSPOTSWIFI	64
#define MAXLENGHTWIFINAME	64

char spotSSID[MAXCANTSPOTSWIFI][MAXLENGHTWIFINAME];

char storePassword[MAXLENGHTWIFINAME];
char storeSSID[MAXLENGHTWIFINAME];

struct Connection {
	char ssid[MAXLENGHTWIFINAME];
	char password[MAXLENGHTWIFINAME];
	char IP[15];
	char status;
	char mode;
} typedef Connection;

xSemaphoreHandle xSemaphoreWifiATCommand, xSemaphoreWifiRefresh;
Connection connection;

void initGateway();
void setSSID(char *ssid);
void connectToAgroRobots();
void setPassword(char *password);
void enterWifiMode();
void connectionMode();
void getIP();
void connectingToServer();
char getStatus();
char *getWifiBuffer();
char tryToConnect();
void readBuffer();
void sendInfo(char *data);
void WIFI_AddCharToInputBuffer(char data);
int SSIDStoredVisible();
void disconectFromSpot();
