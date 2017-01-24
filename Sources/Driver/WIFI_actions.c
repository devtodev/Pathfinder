/*
 * WIFI_actions.c
 *
 *  Created on: Dec 22, 2015
 *      Author: Carlos Miguens
 */


// esptool.py --port /dev/ttyUSB0 write_flash 0x00000 'AiThinker_ESP8266_DIO_8M_8M_20160615_V1.5.4.bin'

#define DEBUG 1

#include "WIFI_actions.h"
#include "ESP8266.h"
#include "string.h"
#include "FreeRTOS.h"
#include "FRTOS1.h"
#include "semphr.h"
#include "Driver/BT_actions.h"
#include <Utils/StorePassword.h>
#include <Utils/utils.h>
#include <Utils/config.h>
#include "Controller/ActionManager.h"

void sendATCommand(char *ATCommand);
void sendPartialATCommand(char *ATCommand);
void resetModule();

#define MAXWIFIINPUTLEGHT 1024

char wifiInputBuffer[MAXWIFIINPUTLEGHT];
int cursorWifiInputBuffer;
xSemaphoreHandle xSemaphoreWifiATCommandSend;

void WIFI_AddCharToInputBuffer(char data)
{
	wifiInputBuffer[cursorWifiInputBuffer] = data;
	cursorWifiInputBuffer++;
	wifiInputBuffer[cursorWifiInputBuffer] = '\0';
//	BT_showLogChar(data);
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
//	BT_SendChar('\r');
//	BT_SendChar('\n');

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
	char *password;
	int position[10];

	xSemaphoreWifiATCommand = xSemaphoreCreateBinary();
	xSemaphoreWifiRefresh = xSemaphoreCreateBinary();
	xSemaphoreWifiATCommandSend = xSemaphoreCreateBinary();
	xSemaphoreGive(xSemaphoreWifiATCommandSend);
	FRTOS1_vTaskDelay(500/portTICK_RATE_MS);
	resetModule();
	// get store passwords
	password = (char *) NVMC_GetSSID_PASSData();

	if (password[0] == '|')
	{
		if (find(password, "|\0", (int *) &position) > 0)
		{
			strsub(password, 0+1, position[1]-1, storeSSID);
			strsub(password, position[1]+1, position[2]-1, storePassword);
		}
	}

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
	sendATCommand("AT+CIPMUX=0\0");
}


void getIP()
{
	sendATCommand("AT+CIFSR\0");
}

void connectingToServer()
{
	sendATCommand("AT+CIPSTART=\"TCP\",\"54.208.231.134\",3000\0");
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
	if (find(wifiInputBuffer, palabra, (int *) &position) > 0)
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
void evaluarGetIP();
void evaluarConeccionConServer();
void verificarEnvioAlServer();
void analizarEnvioDelServer();

void readBuffer()
{
// AT COMMANDS
  if (interpretarBuffer("+RST\0", setMode) == 1) return;
  if (interpretarBuffer("+CWMODE\0", enterMode) == 1) return;
  if (interpretarBuffer("+CWLAP\0", spotsParse) == 1) return;
  if (interpretarBuffer("+CIFSR\0", evaluarGetIP) == 1) return;
  if (interpretarBuffer("+CWJAP\0", spotsNewConnect) == 1) return;
  if (interpretarBuffer("+CIPMUX\0", evaluarConnectionMode) == 1) return;
  if (interpretarBuffer("+CIPSTART\0", evaluarConeccionConServer) == 1) return;
  if (interpretarBuffer("+CIPSEND\0", verificarEnvioAlServer) == 1) return;
  if (interpretarBuffer("CLOSED\0", resetModule) == 1) return;
  // ROBOT COMMANDS
  if (interpretarBuffer("+IPD\0", analizarEnvioDelServer) == 1) return;
}

void analizarEnvioDelServer()
{
	int position[10];
	char action = '\0';
	if (find(wifiInputBuffer, "{\0", (int *) &position) > 0)
	{
		action = wifiInputBuffer[position[0]+1];
		pushAction(action);
	}

	BT_showString("Robot command !\r\n\0");
}

void verificarEnvioAlServer()
{
	// verificar si se envio, reenviar en caso de que no se haya enviado!
	xSemaphoreGive(xSemaphoreWifiRefresh);
}

void sendInfo(char *data)
{
	// "DeviceID|TipoAccionId|Valor"
	sendATCommand("AT+CIPSEND=5\0");
	FRTOS1_vTaskDelay(1000/portTICK_RATE_MS);
	sendPartialATCommand(DEVICE_ID);
	sendPartialATCommand("|\0");
	sendPartialATCommand(data);
}

void evaluarConeccionConServer()
{
	int position[10];
	int error = find(wifiInputBuffer, "ERROR\0", (int *) &position);
	error = (error == 0)?find(wifiInputBuffer, "FAIL\0", (int *) &position):error;
	if (error > 0)
	{
		BT_sendSaltoLinea();
		BT_showString("Error al conectar al server\0");
		connectionMode();
		return;
	}
	// conecto correctamente
	connection.status = WIFI_CONNECTED;
	// presentarse !
	//sendInfo("presentarse|ok");
	xSemaphoreGive(xSemaphoreWifiRefresh);
}

void evaluarConnectionMode()
{
	int position[10];
	int error = find(wifiInputBuffer, "ERROR\0", (int *) &position);
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

void evaluarGetIP()
{
	int position[10];
	int error = find(wifiInputBuffer, "ERROR\0", (int *) &position);
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


void spotsNewConnect()
{
	char SSIDPassword[MAXLENGHTWIFINAME*2];

	int position[10];
	int fail = find(wifiInputBuffer, "FAIL\0", (int *) &position);
	fail = (fail == 0)? find(wifiInputBuffer, "ERROR\0", (int *) &position) : fail;
	if (fail > 0)
	{
		BT_sendSaltoLinea();
		BT_showString("Password incorrecta\0");
		xSemaphoreGive(xSemaphoreWifiRefresh);
		return;
	}
	// conecto correctamente
	connection.status = WIFI_CONNECTING;
	// almacena el SSID & Password
	strcpy(SSIDPassword, "|\0");
	strcat(SSIDPassword, connection.ssid);
	strcat(SSIDPassword, "|\0");
	strcat(SSIDPassword, connection.password);
	strcat(SSIDPassword, "|\0");
	(void)NVMC_SaveSSID_PASSData(&SSIDPassword, sizeof(SSIDPassword));

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
	sendATCommand("AT+CWMODE=1\0");
}

void spotsParse()
{
	int cursor = 0;
	int position[100];
	int found = find(wifiInputBuffer, "ERROR\0", (int *) &position);
	if (found > 0)
	{
		BT_showString("Error CWLAP");
		refreshWifiSpots();
		return;
	}
	found = find(wifiInputBuffer, "\"\0", (int *) &position);
	cursor = 0;
	while ((position[cursor * 4] != EOIL) && (found > cursor))
	{
	  strsub(wifiInputBuffer, position[cursor * 4]+1, position[cursor * 4 + 1]-1, (char *) &spotSSID[cursor][0]);
	  cursor++;
	}
	xSemaphoreGive(xSemaphoreWifiRefresh);
}

int SSIDStoredVisible()
{
	char opcion[MENUMAXLENGHT];
	int i = 0;
	if ((storeSSID == NULL) || (storeSSID[0] == '\0'))
		return FALSE;
	while (spotSSID[i] != '\0')
	{
		i++;
		//el SSID almacenado en memoria esta visible
		if (strcmp(storeSSID, spotSSID[i]))
			return TRUE;
	}
	//no se visualiza el SSID
	return FALSE;
}

void disconectFromSpot()
{
	(void)NVMC_SaveSSID_PASSData(&"\0", sizeof("\0"));
	resetModule();
}
