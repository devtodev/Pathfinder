/**
 * Proyecto Pathfinder
 * Autor Carlos Miguens
 *
 */

/* Begin of <includes> initialization, DO NOT MODIFY LINES BELOW */

#include "TSK1.h"
#include "FRTOS1.h"
#include "frtos_tasks.h"

/* End <includes> initialization, DO NOT MODIFY LINES ABOVE */

#include "MMA1.h"
#include "MAG1.h"
#include "borrame/BT_frontend.h"
#include "Controller/ActionManager.h"
#include "Driver/acelerometro.h"
#include "Controller/moves.h"
#include "Driver/BT_actions.h"
#include "Controller/Distance.h"
#include "Driver/magnetometro.h"
#include "Driver/motor.h"
#include "Driver/WIFI_actions.h"

#include "math.h"

#define DELAY_BETWEEN_ACTIONS_MS 25
#define MAX_ACTION_BUFFER 		 20
#define MAX_DISTANCE_TO_STOP	 35
#define TASK_ULTRASONIDO		 1

#define BUFFERDISTANCESIZE 		 5


/**
 * DEBUG INFO SHOW
 */

int SHOW_GFORCES = 0;
int SHOW_ORIENTATION = 0;


static portTASK_FUNCTION(motorTask, pvParameters) {
	int overturn = 0;
	int distance = 0;
	for(;;) {
//		move_correction();
		if( xQueueReceive( queueMotor, &( currentAction ), 0) )
		{
			vTaskDelay(currentAction.delayms/portTICK_RATE_MS);
			doAction(currentAction.type);
		}
		distance = Distance_getFront();
	    if ((distance < MAX_DISTANCE_TO_STOP) && (direction_Left == FORWARD) && (direction_Right == FORWARD))
	    {
	    	BT_showString("STOP: Obstaculo\r\n\0");
	    	pushAction(MOVE_STOP);
	    }
/*
	    overturn =+ Accel_isOverturn();
	    if (overturn > 5)
	    {
	    	overturn = 0;
	    	BT_showString("STOP: vuelco\r\n\0");
	    	pushAction(MOVE_STOP);
	    }
*/
		vTaskDelay(10/portTICK_RATE_MS);
	}
	vTaskDelete(motorTask);
}
#if nav
static portTASK_FUNCTION(navigationTask, pvParameters) {
	int iStep = -1;
	for(;;) {
		/*
		 * Monitoring and control the correct direction to meet the target
		 *
		 *  the mission could be aborted putting iStep in zero
		 *
		 */
		travelMeetTheTarget();

		if( xQueueReceive( queueTarget, &( target ), 1) && (iStep <= 0) )
		{
			iStep = 0; // init the journey
			travelAddNewTarget(target);
			/*
			 * calculating the next moves
			 * to goal the target
			 * and add to the steps circular queue
			 */
		}
		// time between sensor control points
		vTaskDelay(10/portTICK_RATE_MS);
	}
	vTaskDelete(navigationTask);
}
#endif

static portTASK_FUNCTION(GatewayTask, pvParameters) {

  initGateway();
  for(;;) {
    xSemaphoreTake(xSemaphoreWifiATCommand, portMAX_DELAY);
    readBuffer();
  }
  /* Destroy the task */
  vTaskDelete(GatewayTask);
}

static portTASK_FUNCTION(HMITask, pvParameters) {
	  char menuConectado[MENUMAXLENGHT][64] = {"Desconectar"};
	  char opcionHIM[30];
	  int intentsConnects = 0, temp = 0;
	  /* Write your task initialization code here ... */
	  BT_init();
/*	  MySegLCDPtr = SegLCD1_Init(NULL);
	  setLCD("9991");
	  SymbolON(11,0);
*/
	  for(;;) {
		  BT_sendSaltoLinea();BT_sendSaltoLinea();BT_sendSaltoLinea();BT_sendSaltoLinea();
		  BT_showString("Agro Robots WiFi Spot");
		  xSemaphoreTake(xSemaphoreWifiRefresh, portMAX_DELAY);
		  switch (connection.status)
		  {
		  	  case WIFI_DISCONNECTED:
			    // necesito obtener los spots
		  		BT_sendSaltoLinea();BT_sendSaltoLinea();
		  		FRTOS1_vTaskDelay(1000/portTICK_RATE_MS);
		  		refreshWifiSpots();
		  		xSemaphoreTake(xSemaphoreWifiRefresh, portMAX_DELAY);
		  		temp = SSIDStoredVisible();
		  		if ((temp >= 0) && (intentsConnects < 2))
		  		{
		  			strcpy(connection.ssid, storedConnections[temp].ssid);
		  			strcpy(connection.password, storedConnections[temp].password);
		  			tryToConnect();
		  			intentsConnects++;
		  		} else {
		  			temp = 0;
		  			while ((temp < MAXCANTSPOTSWIFI) && (spotSSID[temp][0] != '\0'))
		  			{
		  				temp++;
		  			}
		  			if ((temp > 0) && (strcmp("{Ingreso manual}\0", spotSSID[temp-1])!=0))
		  				strcpy(spotSSID[temp], "{Ingreso manual}\0");
					// mostrar los SSIDs
					if (BT_showMenu(&spotSSID, &connection.ssid[0]) != -69)
					{
						// setPassword
						BT_sendSaltoLinea();
						if (strcmp("{Ingreso manual}\0", &connection.ssid[0])!=0)
						{
							BT_showString("Seleccion: ");
							BT_showString(&connection.ssid[0]);
						} else {
							BT_askValue("SSID: ", &connection.ssid[0]);
						}
						BT_sendSaltoLinea();
						BT_askValue("Password: ", &connection.password[0]);
						// showDetails
						BT_sendSaltoLinea();BT_sendSaltoLinea();
						BT_showString("SSID: ");
						BT_showString(&connection.ssid[0]);
						BT_sendSaltoLinea();
						BT_showString("PASSWORD: ");
						BT_showString(&connection.password[0]);
						BT_sendSaltoLinea();
				  		// try to connect
						tryToConnect();
					} else {
						xSemaphoreGive(xSemaphoreWifiRefresh);
						for (int i = 0; i < 100; i++) BT_sendSaltoLinea();
					}
		  		}
			  break;
		  	  case WIFI_CONNECTING:
		  		intentsConnects = 0;
		  		FRTOS1_vTaskDelay(2000/portTICK_RATE_MS);
		  		connectionMode();
		  		xSemaphoreTake(xSemaphoreWifiRefresh, portMAX_DELAY);
		  		FRTOS1_vTaskDelay(1000/portTICK_RATE_MS);
				setDHCP();
				xSemaphoreTake(xSemaphoreWifiRefresh, portMAX_DELAY);
				FRTOS1_vTaskDelay(1000/portTICK_RATE_MS);
				connectingToServer();
			  break;
			  case WIFI_CONNECTED:
				switch (BT_showMenu(&menuConectado, &opcionHIM[0]))
				{
					case 0:
						disconectFromSpot();
					break;
					case -69:
						xSemaphoreGive(xSemaphoreWifiRefresh);
						for (int i = 0; i < 100; i++) BT_sendSaltoLinea();
					break;
				}
			  break;
		  }
	  }
	  /* Destroy the task */
	  vTaskDelete(HMITask);
}


/*
static portTASK_FUNCTION(HMI_BT_Task, pvParameters) {
  hmi_bt_init();
  char option;
  for(;;) {
	option = hmi_bt_getOption();
	pushAction(option);
  }
  vTaskDelete(HMI_BT_Task);
}
*/
static portTASK_FUNCTION(SensorUltrasonidoTask, pvParameters) {
  int distanceFront;
  char temp[10], count = 0;
  Distance_init();
  for(;;) {
	    if (connection.status == WIFI_CONNECTED)
	    {
			if (count >= 50)
			{
				sendInfo(Distance_getFront(), currentAction.type, gforceXYZ[0], gforceXYZ[1], gforceXYZ[2], position.orientation.Psi, position.orientation.The, position.orientation.Phi);
				count = 0;
			}
			count++;
	    }
	    Distance_doMeaseure();
	    vTaskDelay(10/portTICK_RATE_MS);
  }
  /* Destroy the task */
  vTaskDelete(SensorUltrasonidoTask);
}
#define BUFFERPOSITIONDATA 20
static portTASK_FUNCTION(PositionTask, pvParameters) {
  int16_t magneticFieldsTemp[BUFFERPOSITIONDATA][3];
  int16_t gforcesTemp[BUFFERPOSITIONDATA][3];
  int cursor = 0;
  char str[30];
  Mag_Init();
  Accel_Init();
  for(;;) {
	  	gforcesTemp[cursor][0] = MMA1_GetX();
	  	gforcesTemp[cursor][1] = MMA1_GetY();
	  	gforcesTemp[cursor][2] = MMA1_GetZ();
	  	MAG1_GetXYZ16(magneticFieldsTemp[cursor]);

	  	cursor++;

	  	if (cursor >= BUFFERPOSITIONDATA)
	  	{
	  		long tempMag[3] = {0};
	  		long tempAccel[3] = {0};
	  		for (cursor = 0; cursor < BUFFERPOSITIONDATA; cursor++)
	  		{
	  			tempMag[0] = tempMag[0] + magneticFieldsTemp[cursor][0];
	  			tempMag[1] = tempMag[1] + magneticFieldsTemp[cursor][1];
	  			tempMag[2] = tempMag[2] + magneticFieldsTemp[cursor][2];
	  			tempAccel[0] = tempAccel[0] + gforcesTemp[cursor][0];
	  			tempAccel[1] = tempAccel[1] + gforcesTemp[cursor][1];
	  			tempAccel[2] = tempAccel[2] + gforcesTemp[cursor][2];
	  		}
	  		position.orientation.magneticFields[0] = tempMag[0] / BUFFERPOSITIONDATA;
	  		position.orientation.magneticFields[1] = tempMag[1] / BUFFERPOSITIONDATA;
	  		position.orientation.magneticFields[2] = tempMag[2] / BUFFERPOSITIONDATA;
	  		gforceXYZ[0] = tempAccel[0] / BUFFERPOSITIONDATA;
	  		gforceXYZ[1] = tempAccel[1] / BUFFERPOSITIONDATA;
	  		gforceXYZ[2] = tempAccel[2] / BUFFERPOSITIONDATA;
	  		/*
	  		 * TODO: estimate the speed and traveled distance
	  		 */
		    eCompass(position.orientation.magneticFields[0], position.orientation.magneticFields[1], position.orientation.magneticFields[2],
		    		gforceXYZ[0], gforceXYZ[1], gforceXYZ[2]);
		  	position.orientation.Phi = Phi;
		  	position.orientation.The = The;
		  	position.orientation.Psi = Psi;
		  	position.orientation.Vx = Vx;
		  	position.orientation.Vy = Vy;
		  	position.orientation.Vz = Vz;
		  	cursor = 0;
/*
			if (SHOW_GFORCES)
			{
				point2string(gforceXYZ, str);
				BT_showString(str);
				BT_showString("\r\n\0");
			}
		  	if (SHOW_ORIENTATION)
		  	{
				angles2string(position.orientation, str);
				BT_showString(str);
				BT_showString("\r\n\0");
		  	}*/
	  	}
	  	vTaskDelay(10/portTICK_RATE_MS);
  }
  /* Destroy the task */
  vTaskDelete(PositionTask);
}

void CreateTasks(void) {
	BT_init();
	initActions();
//	move_init();
	pushAction(MOVE_STOP);


	if (FRTOS1_xTaskCreate(
		motorTask,  /* pointer to the task */
        "motorTask", /* task name for kernel awareness debugging */
		configMINIMAL_STACK_SIZE, /* task stack size */
        (void*)NULL, /* optional task startup argument */
        tskIDLE_PRIORITY + 1,  /* initial priority */
        (xTaskHandle*)NULL /* optional task handle to create */
      ) != pdPASS) {
        /*lint -e527 */
        for(;;){}; /* error! probably out of memory */
        /*lint +e527 */
    }
#if nav
	if (FRTOS1_xTaskCreate(
  		  navigationTask,
          "navigationTask",
		  configMINIMAL_STACK_SIZE,
          (void*)NULL,
          tskIDLE_PRIORITY + 2,
          (xTaskHandle*)NULL
        ) != pdPASS) {
          for(;;){};
    }

	if (FRTOS1_xTaskCreate(
		  PositionTask,  /* pointer to the task */
      "PositionTask", /* task name for kernel awareness debugging */
      configMINIMAL_STACK_SIZE +200, /* task stack size */
      (void*)NULL, /* optional task startup argument */
      tskIDLE_PRIORITY + 1,  /* initial priority */
      (xTaskHandle*)NULL /* optional task handle to create */
    ) != pdPASS) {
      /*lint -e527 */
      for(;;){}; /* error! probably out of memory */
      /*lint +e527 */
  }

#endif


	if (FRTOS1_xTaskCreate(
	  SensorUltrasonidoTask,
	  "SensorUltrasonidoTask",
	  configMINIMAL_STACK_SIZE,
	  (void*)NULL,
	  tskIDLE_PRIORITY + 1,
	  (xTaskHandle*)NULL
	) != pdPASS) {
	  for(;;){};
	}

  if (FRTOS1_xTaskCreate(
     GatewayTask,  /* pointer to the task */
      "Gateway", /* task name for kernel awareness debugging */
      1500, /* task stack size */
      (void*)NULL, /* optional task startup argument */
      tskIDLE_PRIORITY + 2,  /* initial priority */
      (xTaskHandle*)NULL /* optional task handle to create */
    ) != pdPASS) {
      /*lint -e527 */
      for(;;){}; /* error! probably out of memory */
      /*lint +e527 */
  }

  if (FRTOS1_xTaskCreate(
     HMITask,  /* pointer to the task */
      "HMI", /* task name for kernel awareness debugging */
      1200, /* task stack size */
      (void*)NULL, /* optional task startup argument */
      tskIDLE_PRIORITY + 2,  /* initial priority */
      (xTaskHandle*)NULL /* optional task handle to create */
    ) != pdPASS) {
      /*lint -e527 */
      for(;;){}; /* error! probably out of memory */
      /*lint +e527 */
  }
}

