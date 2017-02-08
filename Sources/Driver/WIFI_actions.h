/*
 * BT_actions.c
 *
 *  Created on: Dec 22, 2015
 *      Author: Carlos Miguens
 */

#include "FreeRTOS.h"
#include "semphr.h"

#define WIFI_CONNECTED 			1
#define WIFI_DISCONNECTED		2
#define WIFI_CONNECTING			3
#define WIFI_ATMODE				4
#define MAXCANTSPOTSWIFI		40
#define MAXLENGHTWIFINAME		64
#define STORED_CONNECTIONS_SIZE 6

char spotSSID[MAXCANTSPOTSWIFI][MAXLENGHTWIFINAME];

struct Connection {
	char ssid[MAXLENGHTWIFINAME];
	char password[30];
	int status;
} typedef Connection;

xSemaphoreHandle xSemaphoreWifiATCommand, xSemaphoreWifiRefresh;
Connection connection;
Connection storedConnections[STORED_CONNECTIONS_SIZE];

int storeConnectionsSize;

void initGateway();
void setSSID(char *ssid);
void connectToAgroRobots();
void setPassword(char *password);
void enterWifiMode();
void connectionMode();
void setDHCP();
void refreshWifiSpots();
void connectingToServer();
char getStatus();
char *getWifiBuffer();
char tryToConnect();
void readBuffer();
void sendInfo(char *data);
int SSIDStoredVisible();
void disconectFromSpot();

#define TEST_CONNECTIONS 0
void testStoredConnections();
