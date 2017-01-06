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
#include "Controller/ActionManager.h"
#include "Driver/acelerometro.h"
#include "Controller/moves.h"
#include "Driver/BT_actions.h"
#include "Controller/Distance.h"
#include "HMI/BT_frontend.h"
#include "Driver/magnetometro.h"
#include "Driver/motor.h"

#include "math.h"

#define SHOW_ORIENTATION 1
#define TASK_HMI 1

#define DELAY_BETWEEN_ACTIONS_MS 25
#define MAX_ACTION_BUFFER 		 20
#define MAX_DISTANCE_TO_STOP	 35

#define BUFFERDISTANCESIZE 		 5

static portTASK_FUNCTION(motorTask, pvParameters) {

	for(;;) {
		move_correction();
		if( xQueueReceive( queueMotor, &( currentAction ), 0) )
		{
			vTaskDelay(currentAction.delayms/portTICK_RATE_MS);
			doAction(currentAction.type);
		}
		vTaskDelay(10/portTICK_RATE_MS);
	}
	vTaskDelete(motorTask);
}
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

			#if SHOW_GFORCES
		  	point2string(gforce.xyz, str);
		  	BT_showString(str);
		  	BT_showString("\r\n\0");
			#endif
		  	#if SHOW_ORIENTATION
		  	angles2string(position.orientation, str);
		  	BT_showString(str);
		  	BT_showString("\r\n\0");
		  	#endif
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
        tskIDLE_PRIORITY + 2,  /* initial priority */
        (xTaskHandle*)NULL /* optional task handle to create */
      ) != pdPASS) {
        /*lint -e527 */
        for(;;){}; /* error! probably out of memory */
        /*lint +e527 */
    }
  if (FRTOS1_xTaskCreate(
  		  navigationTask,  /* pointer to the task */
          "navigationTask", /* task name for kernel awareness debugging */
		  configMINIMAL_STACK_SIZE, /* task stack size */
          (void*)NULL, /* optional task startup argument */
          tskIDLE_PRIORITY + 2,  /* initial priority */
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
      tskIDLE_PRIORITY + 3,  /* initial priority */
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
      tskIDLE_PRIORITY + 3,  /* initial priority */
      (xTaskHandle*)NULL /* optional task handle to create */
    ) != pdPASS) {
      /*lint -e527 */
      for(;;){}; /* error! probably out of memory */
      /*lint +e527 */
  }

}

