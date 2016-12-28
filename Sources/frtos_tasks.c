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
#include "MAG1.h"
#include "Driver/motor.h"

#include "math.h"


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
static portTASK_FUNCTION(AcelerometroTask, pvParameters) {
	int16_t xyz[3];
	char str[20];
	char temp[20];
	Accel_Init();

	/* TODO: Wall-E
	 *
	 * Rx PTE21
	 * Tx PTE20
	 *
	 */
	for(;;)
	{
		xyz[0] = MMA1_GetX();
		xyz[1] = MMA1_GetY();
		xyz[2] = MMA1_GetZ();

		point2string(&xyz[0], &str[0]);
		BT_showString(str);
		BT_showString("\r\n");

		FRTOS1_vTaskDelay(100/portTICK_RATE_MS);
	}
	/* Destroy the task */
	vTaskDelete(AcelerometroTask);
}

#define MAXMAGDATA 10
static portTASK_FUNCTION(MagnetometerTask, pvParameters) {
  int16_t i = 0, x, y, z, xi, yi, zi, degree;
  int16_t data[3][MAXMAGDATA];
  char temp[10];
  MAG1_Enable(); /* enable magnetometer */
  MAG1_GetX(&xi);
  MAG1_GetY(&yi);
  MAG1_GetZ(&zi);
  for(;;) {
	  /*
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
	    vTaskDelay(10/portTICK_RATE_MS);
  }
  /* Destroy the task */
  vTaskDelete(MagnetometerTask);
}
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
  if (FRTOS1_xTaskCreate(
	  MagnetometerTask,  /* pointer to the task */
      "MagnetometerTask", /* task name for kernel awareness debugging */
      configMINIMAL_STACK_SIZE , /* task stack size */
      (void*)NULL, /* optional task startup argument */
      tskIDLE_PRIORITY + 2,  /* initial priority */
      (xTaskHandle*)NULL /* optional task handle to create */
    ) != pdPASS) {
      /*lint -e527 */
      for(;;){}; /* error! probably out of memory */
      /*lint +e527 */
  }
  if (FRTOS1_xTaskCreate(
     AcelerometroTask,  /* pointer to the task */
      "AcelerometroTask", /* task name for kernel awareness debugging */
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

