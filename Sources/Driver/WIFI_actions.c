/*
 * WIFI_actions.c
 *
 *  Created on: Dec 22, 2015
 *      Author: Carlos Miguens
 */


// esptool.py --port /dev/ttyUSB0 write_flash 0x00000 'AiThinker_ESP8266_DIO_8M_8M_20160615_V1.5.4.bin'

#include "WIFI_actions.h"
#include "ESP8266.h"
#include "string.h"
#include "FreeRTOS.h"
#include "FRTOS1.h"
#include "semphr.h"
#include "Utils/utils.h"
#include "BT_actions.h"
#include "Utils/StorePassword.h"
#include "Utils/config.h"

void sendATCommand(char *ATCommand);
void sendPartialATCommand(char *ATCommand);
void setStoredConnections(Connection *connection);
void getStoredConnections();

#define MAXWIFIINPUTLEGHT 1024

char wifiInputBuffer[MAXWIFIINPUTLEGHT];
int cursorWifiInputBuffer;
xSemaphoreHandle xSemaphoreWifiATCommandSend;

void WIFI_AddCharToInputBuffer(char data)
{
	wifiInputBuffer[cursorWifiInputBuffer] = data;
	cursorWifiInputBuffer++;
	if (cursorWifiInputBuffer >= MAXWIFIINPUTLEGHT)
		cursorWifiInputBuffer = MAXWIFIINPUTLEGHT - 5;
	wifiInputBuffer[cursorWifiInputBuffer] = '\0';
#if DEBUG
	BT_showLogChar(data);
#endif
	// detectar OK y liberar el semaforo para interpretar el mensaje
	if ((cursorWifiInputBuffer > 5) &&
		(((wifiInputBuffer[cursorWifiInputBuffer-2] == 'O') && (wifiInputBuffer[cursorWifiInputBuffer-1] == 'K'))
			|| ((wifiInputBuffer[cursorWifiInputBuffer-4] == 'R') && (wifiInputBuffer[cursorWifiInputBuffer-3] == 'R') && (wifiInputBuffer[cursorWifiInputBuffer-2] == 'O') && (wifiInputBuffer[cursorWifiInputBuffer-1] == 'R')))
			|| ((wifiInputBuffer[cursorWifiInputBuffer-4] == 'F') && (wifiInputBuffer[cursorWifiInputBuffer-3] == 'A') && (wifiInputBuffer[cursorWifiInputBuffer-2] == 'I') && (wifiInputBuffer[cursorWifiInputBuffer-1] == 'L')))
	{
		xSemaphoreGive(xSemaphoreWifiATCommand);
	}
}

void sendATCommand(char *ATCommand)
{
	xSemaphoreTake(xSemaphoreWifiATCommandSend, portMAX_DELAY);
	cursorWifiInputBuffer = 0;
	sendPartialATCommand(ATCommand);
	ESP8266_SendChar('\r');
	ESP8266_SendChar('\n');

	xSemaphoreGive(xSemaphoreWifiATCommandSend);
}

void sendPartialATCommand(char *ATCommand)
{
	char *index = ATCommand;

	while (*index != '\0')
	{
		ESP8266_SendChar(*index);
//		BT_SendChar(*index);
		index++;
	}
}

void initGateway()
{
	xSemaphoreWifiATCommand = xSemaphoreCreateBinary();
	xSemaphoreWifiRefresh = xSemaphoreCreateBinary();
	xSemaphoreWifiATCommandSend = xSemaphoreCreateBinary();
	xSemaphoreGive(xSemaphoreWifiATCommandSend);
	FRTOS1_vTaskDelay(500/portTICK_RATE_MS);
	resetModule();
	// get store passwords
	getStoredConnections();
}

void setSSID(char *ssid)
{
	strcpy(ssid, connection.ssid);
}

void setPassword(char *password)
{
	strcpy(password, connection.password);
}

void refreshWifiSpots()
{
	sendATCommand("AT+CWLAP\0");
}

void enterWifiMode()
{
	sendATCommand("AT+CWMOD=3\0");
}

void connectionMode()
{
	sendATCommand("AT+CIPMUX=1\0");
}


void setDHCP()
{
//	sendATCommand("AT+CIFSR\0");
	sendATCommand("AT+CWDHCP=1,1\0");
}

void connectingToServer()
{
	sendATCommand("AT+CIPSTART=0,\"TCP\",\"54.208.231.134\",3000\0");
}

char getStatus()
{
	return connection.status;
}

char *getWifiBuffer()
{
	return &wifiInputBuffer[0];
}

char tryToConnect()
{
	sendPartialATCommand("AT+CWJAP=\"");
	sendPartialATCommand(connection.ssid);
	sendPartialATCommand("\",\"");
	sendPartialATCommand(connection.password);
	sendATCommand("\"");
}

int interpretarBuffer(char *palabra, void (*functionPointer)())
{
	int position[100];
	if (find(wifiInputBuffer, palabra, (int *) &position, 100) > 0)
	{
		functionPointer();
		cursorWifiInputBuffer = 0;
		return 1;
	}
}

void resetModule();
void enterMode();
void setMode();
void spotsParse();
void spotsNewConnect();
void evaluarConnectionMode();
void evaluarDHCP();
void evaluarConeccionConServer();
void verificarEnvioAlServer();
void analizarEnvioDelServer();

void readBuffer()
{
// AT COMMANDS
  if (interpretarBuffer("+RST\0", setMode) == 1) return;
  if (interpretarBuffer("+CWMODE\0", enterMode) == 1) return;
  if (interpretarBuffer("+CWLAP\0", spotsParse) == 1) return;
  if (interpretarBuffer("+CWJAP\0", spotsNewConnect) == 1) return;
  if (interpretarBuffer("+CIPMUX\0", evaluarConnectionMode) == 1) return;
  if (interpretarBuffer("+CIPSTART\0", evaluarConeccionConServer) == 1) return;
  if (interpretarBuffer("+CIPSEND\0", verificarEnvioAlServer) == 1) return;
  if (interpretarBuffer("CLOSED\0", resetModule) == 1) return;
  if (interpretarBuffer("+CWDHCP\0", evaluarDHCP) == 1) return;
  // ROBOT COMMANDS
  if (interpretarBuffer("+IPD\0", analizarEnvioDelServer) == 1) return;
}

void analizarEnvioDelServer()
{
	int position[10];
	char action = '\0';
	if (find(wifiInputBuffer, "{\0", (int *) &position, 10) > 0)
	{
		action = wifiInputBuffer[position[0]+1];
		pushAction(action);
	}

	BT_showString("Robot command!\r\n\0");
}

void verificarEnvioAlServer()
{
	int position[10];
	int error = find(wifiInputBuffer, "ERROR\0", (int *) &position, 10);
	error = (error == 0)?find(wifiInputBuffer, "not valid\0", (int *) &position, 10):error;
	if (error > 0)
	{
		BT_sendSaltoLinea();
		BT_showString("Error al enviar al server\0");
		//connectionMode();
		connection.status = WIFI_DISCONNECTED;
		xSemaphoreGive(xSemaphoreWifiRefresh);
		return;
	}

	xSemaphoreGive(xSemaphoreWifiRefresh);
}

void sendInfo(int distance, char action, int gx, int gy, int gz, int Psi, int The, int Phi)
{
	char data[100], command[200], temp[10], saction[2];

	strcpy(data, "{\"distance\":");
	Num16sToStr(temp, 10, distance);
	strcat(data, temp);
	strcat(data, ",\"action\":\"");
	saction[0] = action;
	saction[1] = '\0';
	strcat(data, saction);
/*	strcat(data, "\",\"gforce\":{\"x\":\0");
	Num16sToStr(temp, 10, gx);
	strcat(data, temp);
	strcat(data, ",\"y\":\0");
	Num16sToStr(temp, 10, gy);
	strcat(data, temp);
	strcat(data, ",\"z\":\0");
	Num16sToStr(temp, 10, gz);
	strcat(data, temp);
	strcat(data, "\"} }\0"); */
	strcat(data, "\"}\0");

	Num16sToStr(temp, 10, strlen(data));

	strcpy(command, "AT+CIPSEND=0,\0");
	strcat(command, temp);

	// "DeviceID|TipoAccionId|Valor"
	sendATCommand(command);
	FRTOS1_vTaskDelay(1000/portTICK_RATE_MS);
	sendPartialATCommand(data);
//	sendPartialATCommand(data);
}

void evaluarConeccionConServer()
{
	int position[10];
	int error = find(wifiInputBuffer, "ERROR\0", (int *) &position, 10);
	error = (error == 0)?find(wifiInputBuffer, "FAIL\0", (int *) &position, 10):error;
	if (error > 0)
	{
		BT_sendSaltoLinea();
		BT_showString("Error al conectar al server\0");
		//connectionMode();
		connection.status = WIFI_DISCONNECTED;
		xSemaphoreGive(xSemaphoreWifiRefresh);
		return;
	}
	// conecto correctamente
	connection.status = WIFI_CONNECTED;
	// TODO: presentarse !
	xSemaphoreGive(xSemaphoreWifiRefresh);
}

void evaluarConnectionMode()
{
	int position[10];
	int error = find(wifiInputBuffer, "ERROR\0", (int *) &position, 10);
	if (error > 0)
	{
		BT_sendSaltoLinea();
		BT_showString("Error en CIPMUX\0");
		connectionMode();
		return;
	}
	// conecto correctamente
	xSemaphoreGive(xSemaphoreWifiRefresh);
}

void evaluarDHCP()
{
	int position[10];
	int error = find(wifiInputBuffer, "ERROR\0", (int *) &position, 10);
	if (error > 0)
	{
		BT_sendSaltoLinea();
		BT_showString("Al obtener el IP\0");
		connectionMode();
		return;
	}
	// conecto correctamente
	xSemaphoreGive(xSemaphoreWifiRefresh);
}

void enterMode()
{
	xSemaphoreGive(xSemaphoreWifiRefresh);
}

void resetModule()
{
	cursorWifiInputBuffer = 0;
	connection.status = WIFI_DISCONNECTED;
	sendATCommand("AT+RST\0");
}

void setMode()
{
	FRTOS1_vTaskDelay(2000/portTICK_RATE_MS);
	sendATCommand("AT+CWMODE=3\0");
}

#define MAXPIPESWIFI 100

void spotsParse()
{
	int cursor = 0;
	int position[MAXPIPESWIFI];
//  For test
//	strcpy(wifiInputBuffer, "AT+CWLAP\r\r\n+CWLAP:(3,\"Telecentro-27DC\",-58,\"8c:04:ff:bc:70:73\",1,-2,0)\r\n+CWLAP:(4,\"Galicia 2015\",-55,\"f8:35:dd:6e:3f:1d\",1,-19,0)\r\n+CWLAP:(4,\"Galicia 2015\",-55,\"f8:35:dd:6e:3f:1d\",1,-19,0)\r\n+CWLAP:(4,\"Galicia 2015\",-55,\"f8:35:dd:6e:3f:1d\",1,-19,0)\r\n+CWLAP:(4,\"Galicia 2015\",-55,\"f8:35:dd:6e:3f:1d\",1,-19,0)\r\n+CWLAP:(4,\"Galicia 2015\",-55,\"f8:35:dd:6e:3f:1d\",1,-19,0)\r\n+CWLAP:(4,\"Galicia 2015\",-55,\"f8:35:dd:6e:3f:1d\",1,-19,0)\r\n+CWLAP:(4,\"Galicia 2015\",-55,\"f8:35:dd:6e:3f:1d\",1,-19,0)\r\n+CWLAP:(4,\"Galicia 2015\",-55,\"f8:35:dd:6e:3f:1d\",1,-19,0)\r\n+CWLAP:(4,\"Galicia 2015\",-55,\"f8:35:dd:6e:3f:1d\",1,-19,0)\r\n+CWLAP:(4,\"Galicia 2015\",-55,\"f8:35:dd:6e:3f:1d\",1,-19,0)\r\n+CWLAP:(4,\"Galicia 2015\",-55,\"f8:35:dd:6e:3f:1d\",1,-19,0)\r\n+CWLAP:(4,\"Galicia 2015\",-55,\"f8:35:dd:6e:3f:1d\",1,-19,0)\r\n+CWLAP:(4,\"Galicia 2015\",-55,\"f8:35:dd:6e:3f:1d\",1,-19,0)\r\n+CWLAP:(4,\"Galicia 2015\",-55,\"f8:35:dd:6e:3f:1d\",1,-19,0)\r\n+CWLAP:(4,\"Galicia 2015\",-\0");
	int found = find(wifiInputBuffer, "ERROR\0", (int *) &position, MAXPIPESWIFI);
	if (found > 0)
	{
		BT_showString("Error CWLAP");
		refreshWifiSpots();
		return;
	}
	found = find(wifiInputBuffer, "\"\0", (int *) &position, MAXPIPESWIFI);
	cursor = 0;
	while ((position[cursor * 4] != EOIL) && (found > cursor*4) && (MAXPIPESWIFI > cursor*4))
	{
	  strsub(wifiInputBuffer, position[cursor * 4]+1, position[cursor * 4 + 1]-1, (char *) &spotSSID[cursor][0]);
	  cursor++;
	}
	xSemaphoreGive(xSemaphoreWifiRefresh);
}


void spotsNewConnect()
{
	int position[10];
	int fail = find(wifiInputBuffer, "FAIL\0", (int *) &position, 10);
	fail = (fail == 0)? find(wifiInputBuffer, "ERROR\0", (int *) &position, 10) : fail;
	if (fail > 0)
	{
		BT_sendSaltoLinea();
		BT_showString("Password incorrecta\0");
		xSemaphoreGive(xSemaphoreWifiRefresh);
		return;
	}
	// cambia el status dde la conexion
	connection.status = WIFI_CONNECTING;

	setStoredConnections(&connection);

	xSemaphoreGive(xSemaphoreWifiRefresh);
}

int SSIDStoredVisible()
{
	char opcion[MENUMAXLENGHT];
	int i = 0;

	for (int j = 0; j < storeConnectionsSize; j++)
	{
		i = 0;
		while ((spotSSID != NULL)&&(spotSSID[i][0] != '\0'))
		{
			//el SSID almacenado en memoria esta visible
			if (strcmp(storedConnections[j].ssid, spotSSID[i]) == 0)
				return j;
			i++;
		}
	}
	//no se visualiza el SSID
	return -1;
}

void disconectFromSpot()
{
	// (void)NVMC_SaveSSID_PASSData(&"\0", sizeof("\0"));
	resetModule();
}

void populateStoredConections()
{
	char SSIDPassword[MAXLENGHTWIFINAME*2+20];
	char temp[15];
	for (int i = 0; i < storeConnectionsSize; i++)
	{
		// almacena el SSID & Password
		strcpy(SSIDPassword, "|\0");
		strcat(SSIDPassword, storedConnections[i].ssid);
		strcat(SSIDPassword, "|\0");
		strcat(SSIDPassword, storedConnections[i].password);
		strcat(SSIDPassword, "|\0");
		Num16sToStr(&temp[0], 15, storedConnections[i].status);
		strcat(SSIDPassword, &temp[0]);
		strcat(SSIDPassword, "|\0");
		(void)NVMC_SaveSSID_PASSData(&SSIDPassword, i);
	}
}

void setStoredConnections(Connection *connection)
{
	// verificar si el ssid existe en la lista
	int i = 0;
	int cursor = -1;
	while ((i < storeConnectionsSize) && (cursor == -1))
	{
		if (strcmp(connection->ssid, storedConnections[i].ssid) == 0)
		{
			// 	si existe verificar si la password no cambio
			if (strcmp(connection->password, storedConnections[i].password) == 0)
			{
				return;
			} else {
				cursor = i;
				strcpy(storedConnections[cursor].password, connection->password); // por si cambio actualiza la password
			}
		}
		i++;
	}
	if (cursor == -1)
	{
		cursor = storeConnectionsSize;
		strcpy(storedConnections[cursor].password, connection->password);
		strcpy(storedConnections[cursor].ssid, connection->ssid);
		storedConnections[cursor].status = 0;
		storeConnectionsSize = (storeConnectionsSize < STORED_CONNECTIONS_SIZE)? storeConnectionsSize + 1 : storeConnectionsSize;
	}

	storedConnections[cursor].status++;
#if !TEST_CONNECTIONS
	populateStoredConections(connection);
#endif
}

#if TEST_CONNECTIONS
void testStoredConnections()
{
	getStoredConnections();
	Connection conn;
	for (int i = 0; i < 32; i++)
	{
		strcpy(conn.ssid, "ssid\0");
		strcpy(conn.password, "pass\0");
		conn.status = i;
		setStoredConnections(&conn);
	}
	populateStoredConections(storedConnections);
}
#endif

void getStoredConnections()
{
	char *data;
	int position[10];
	int i = 0;
	char temp[15];
	storeConnectionsSize = i;
	while (i < STORED_CONNECTIONS_SIZE)
	{
		data = (char *) NVMC_GetSSID_PASSData(i);
		if (data[0] == '|')
		{
			storeConnectionsSize++;
			if (find(data, "|\0", (int *) &position, 10) > 0)
			{
				strsub(data, 0+1, position[1]-1, storedConnections[i].ssid);
				strsub(data, position[1]+1, position[2]-1, storedConnections[i].password);
				Num16sToStr(&temp[0], 15, storedConnections[i].status);
				strsub(data, position[3]+1, position[4]-1, temp);
			} else return;
		} else return;
		i++;
	}
}
