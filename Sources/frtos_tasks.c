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

#define ACCEL_ANTIREBOTE		 30
#define DELAY_BETWEEN_ACTIONS_MS 25

#define BUFFERDISTANCESIZE 		 5

static portTASK_FUNCTION(speedTask, pvParameters) {
	uint16_t SERVO1_position;
	Action action;

	queueSpeed = xQueueCreate( 20, sizeof( Action ) );
	for(;;) {
		if( xQueuePeek( queueSpeed, &( action ), portMAX_DELAY ) )
		{
			doAction(action.type);
			vTaskDelay(action.delayms/portTICK_RATE_MS);
		}
	}
	vTaskDelete(speedTask);
}
static portTASK_FUNCTION(directionTask, pvParameters) {
	uint16_t SERVO1_position;
	Action action;

	queueDirection = xQueueCreate( 20, sizeof( Action ) );
	for(;;) {
		if ((xQueuePeek( queueDirection, &( action ), portMAX_DELAY ) ) && (speed_Left == 0) && (speed_Right == 0))
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
  Distance_init();
  for(;;) {
	    Distance_doMeaseure();
	    distanceFront = Distance_getFront();
	    if (distanceFront < 50)
	    {
	    	pushAction(MOVE_STOP);
	    	// TODO: ROTATE
	    }
	    vTaskDelay(10/portTICK_RATE_MS);
  }
  /* Destroy the task */
  vTaskDelete(SensorUltrasonidoTask);
}
static portTASK_FUNCTION(AcelerometroTask, pvParameters) {
	int16_t xyz[3], xyzold[3], cambioEstado;
	Movimiento movimiento;

	MMA1_Init();

	xyzold[0] = xyz[0];
	xyzold[1] = xyz[1];
	xyzold[2] = xyz[2];
	for(;;)
	{
			xyz[0] = MMA1_GetX();
			xyz[1] = MMA1_GetY();
			xyz[2] = MMA1_GetZ();

			movimiento.x = (xyz[0] > xyzold[0])?xyz[0]-xyzold[0]:xyzold[0]-xyz[0];
			movimiento.y = (xyz[1] > xyzold[1])?xyz[1]-xyzold[1]:xyzold[1]-xyz[1];
			movimiento.z = (xyz[2] > xyzold[2])?xyz[2]-xyzold[2]:xyzold[2]-xyz[2];

			movimiento.x = (movimiento.x<0)?movimiento.x*-1:movimiento.x;
			movimiento.y = (movimiento.y<0)?movimiento.y*-1:movimiento.y;
			movimiento.z = (movimiento.z<0)?movimiento.z*-1:movimiento.z;

			if ((movimiento.x< ACCEL_ANTIREBOTE)&&
				(movimiento.y< ACCEL_ANTIREBOTE)&&
				(movimiento.z< ACCEL_ANTIREBOTE))
			{
				if (cambioEstado == 1)
				{
					// poner en cola mensaje de quieto
					cambioEstado = 0;
//!!!!!!					BT_showString("Quieto\r\n\0");
				}
			}
			if ((movimiento.x> ACCEL_ANTIREBOTE)&&
				(movimiento.y> ACCEL_ANTIREBOTE)&&
				(movimiento.z> ACCEL_ANTIREBOTE))
			{
				// en movimiento
				if (cambioEstado == 0)
				{
					cambioEstado = 1;
//!!!!!!					BT_showString("Movimiento\r\n\0");
				}
			}

 			FRTOS1_vTaskDelay(100/portTICK_RATE_MS);
			xyzold[0] = xyz[0];
			xyzold[1] = xyz[1];
			xyzold[2] = xyz[2];

	}
	/* Destroy the task */
	vTaskDelete(AcelerometroTask);
}

void CreateTasks(void) {
  move_init();
  BT_init();
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

