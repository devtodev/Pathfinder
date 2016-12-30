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
#include "Controller/ActionManager.h"
#include "Driver/acelerometro.h"
#include "Controller/moves.h"
#include "Driver/BT_actions.h"
#include "Controller/Distance.h"
#include "HMI/BT_frontend.h"
#include "Driver/magnetometro.h"
#include "Driver/motor.h"

#include "math.h"

#define TASK_HMI 1

#define DELAY_BETWEEN_ACTIONS_MS 25
#define MAX_ACTION_BUFFER 		 20
#define MAX_DISTANCE_TO_STOP	 35

#define BUFFERDISTANCESIZE 		 5

static portTASK_FUNCTION(speedTask, pvParameters) {
	struct Action action;

	for(;;) {
		if( xQueueReceive( queueSpeed, &( action ), portMAX_DELAY ) )
		{
			doAction(action.type);
			vTaskDelay(action.delayms/portTICK_RATE_MS);
		}
	}
	vTaskDelete(speedTask);
}
static portTASK_FUNCTION(directionTask, pvParameters) {
	struct Action action;
	for(;;) {
		if (xQueueReceive( queueDirection, &( action ), portMAX_DELAY ))
		{
			doAction(action.type);
			vTaskDelay(action.delayms/portTICK_RATE_MS);
		}
		vTaskDelay(25/portTICK_RATE_MS);
	}
	vTaskDelete(directionTask);
}
static portTASK_FUNCTION(HMI_BT_Task, pvParameters) {
  hmi_bt_init();
  char option;
  for(;;) {
	option = hmi_bt_getOption();
	pushAction(option);
  }
  vTaskDelete(HMI_BT_Task);
}
static portTASK_FUNCTION(SensorUltrasonidoTask, pvParameters) {
  int distanceFront;
  char temp[10];
  Distance_init();
  for(;;) {
	    Distance_doMeaseure();
	    distanceFront = Distance_getFront();
	    if ((distanceFront < MAX_DISTANCE_TO_STOP) && (direction_Left == FORWARD) && (direction_Right == FORWARD))
	    {
	    	pushAction(MOVE_STOP);
	    }
	    vTaskDelay(10/portTICK_RATE_MS);
  }
  /* Destroy the task */
  vTaskDelete(SensorUltrasonidoTask);
}
#define BUFFERPOSITIONDATA 20
static portTASK_FUNCTION(PositionTask, pvParameters) {
  Orientation orientation;
  Gforce gforce;
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

	  	point2string(gforcesTemp[cursor], str);
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
	  		orientation.magneticFields[0] = tempMag[0] / BUFFERPOSITIONDATA;
	  		orientation.magneticFields[1] = tempMag[1] / BUFFERPOSITIONDATA;
	  		orientation.magneticFields[2] = tempMag[2] / BUFFERPOSITIONDATA;
	  		gforce.xyz[0] = tempAccel[0] / BUFFERPOSITIONDATA;
	  		gforce.xyz[1] = tempAccel[1] / BUFFERPOSITIONDATA;
	  		gforce.xyz[2] = tempAccel[2] / BUFFERPOSITIONDATA;
		    eCompass(orientation.magneticFields[0], orientation.magneticFields[1], orientation.magneticFields[2],
		  			gforce.xyz[0], gforce.xyz[1], gforce.xyz[2]);
		  	orientation.Phi = Phi;
		  	orientation.The = The;
		  	orientation.Psi = Psi;
		  	orientation.Vx = Vx;
		  	orientation.Vy = Vy;
		  	orientation.Vz = Vz;
		  	cursor = 0;
		  	//point2string(gforce.xyz, str);
		  	angles2string(orientation, str);
		  	BT_showString(str);
		  	BT_showString("\r\n\0");
	  	}
	  	vTaskDelay(10/portTICK_RATE_MS);
  }
  /* Destroy the task */
  vTaskDelete(PositionTask);
}
/*
int16_t i = 0, x, y, z, xi, yi, zi, degree;
int16_t data[3][MAXMAGDATA];
char temp[10];
MAG1_Enable();
MAG1_GetX(&xi);
MAG1_GetY(&yi);
MAG1_GetZ(&zi);
for(;;) {
 	MAG1_GetX(&data[0][i]);
	MAG1_GetY(&data[1][i]);
	MAG1_GetZ(&data[2][i]);
	if (i >= MAXMAGDATA)
	{
		x = 0;
		y = 0;
		z = 0;
		for (i = 0; i < MAXMAGDATA; i++)
		{
			x =+ xi - data[0][i];
			y =+ yi - data[1][i];
			z =+ zi - data[2][i];
		}
		x = x / MAXMAGDATA;
		y = y / MAXMAGDATA;
		z = z / MAXMAGDATA;
		UTIL1_Num16sToStr(&temp[0], 20, x);
		BT_showString(temp);
		BT_showString(" ");
		UTIL1_Num16sToStr(&temp[0], 20, y);
		BT_showString(temp);
		BT_showString(" ");
		UTIL1_Num16sToStr(&temp[0], 20, z);
		BT_showString(temp);

		degree = 90-atan(y/x)*180/3.1416;
		BT_showString(" ");
		UTIL1_Num16sToStr(&temp[0], 20, degree);
		BT_showString(temp);

		BT_showString("\r\n");
		i = 0;
	} else {
		i++;
	}
	*/
void CreateTasks(void) {
	BT_init();
	//move_init();
	queueSpeed = xQueueCreate( MAX_ACTION_BUFFER, sizeof( struct Action ) );
	queueDirection = xQueueCreate( MAX_ACTION_BUFFER, sizeof( struct Action ) );
	pushAction(MOVE_STOP);
  if (FRTOS1_xTaskCreate(
		speedTask,  /* pointer to the task */
        "speedTask", /* task name for kernel awareness debugging */
		configMINIMAL_STACK_SIZE, /* task stack size */
        (void*)NULL, /* optional task startup argument */
        tskIDLE_PRIORITY + 3,  /* initial priority */
        (xTaskHandle*)NULL /* optional task handle to create */
      ) != pdPASS) {
        /*lint -e527 */
        for(;;){}; /* error! probably out of memory */
        /*lint +e527 */
    }
  if (FRTOS1_xTaskCreate(
  		  directionTask,  /* pointer to the task */
          "directionTask", /* task name for kernel awareness debugging */
		  configMINIMAL_STACK_SIZE, /* task stack size */
          (void*)NULL, /* optional task startup argument */
          tskIDLE_PRIORITY + 3,  /* initial priority */
          (xTaskHandle*)NULL /* optional task handle to create */
        ) != pdPASS) {
          /*lint -e527 */
          for(;;){}; /* error! probably out of memory */
          /*lint +e527 */
      }

#if TASK_HMI
  if (FRTOS1_xTaskCreate(
		  HMI_BT_Task,  /* pointer to the task */
          "HMI_BT_Task", /* task name for kernel awareness debugging */
          configMINIMAL_STACK_SIZE, /* task stack size */
          (void*)NULL, /* optional task startup argument */
          tskIDLE_PRIORITY + 1,  /* initial priority */
          (xTaskHandle*)NULL /* optional task handle to create */
        ) != pdPASS) {
          /*lint -e527 */
          for(;;){}; /* error! probably out of memory */
          /*lint +e527 */
      }
#endif
#if TASK_ULTRASONIDO
  if (FRTOS1_xTaskCreate(
	  SensorUltrasonidoTask,  /* pointer to the task */
      "SensorUltrasonidoTask", /* task name for kernel awareness debugging */
      configMINIMAL_STACK_SIZE+200, /* task stack size */
      (void*)NULL, /* optional task startup argument */
      tskIDLE_PRIORITY + 2,  /* initial priority */
      (xTaskHandle*)NULL /* optional task handle to create */
    ) != pdPASS) {
      /*lint -e527 */
      for(;;){}; /* error! probably out of memory */
      /*lint +e527 */
  }
#endif

  if (FRTOS1_xTaskCreate(
		  PositionTask,  /* pointer to the task */
      "PositionTask", /* task name for kernel awareness debugging */
      configMINIMAL_STACK_SIZE , /* task stack size */
      (void*)NULL, /* optional task startup argument */
      tskIDLE_PRIORITY + 2,  /* initial priority */
      (xTaskHandle*)NULL /* optional task handle to create */
    ) != pdPASS) {
      /*lint -e527 */
      for(;;){}; /* error! probably out of memory */
      /*lint +e527 */
  }

}

