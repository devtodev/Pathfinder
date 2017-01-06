/*
 * ActionManager.c
 *
 *  Created on: 18/12/2016
 *      Author: Carlos Miguens
 */

#include <Controller/Navigation.h>
#include "Controller/moves.h"
#include "Driver/motor.h"
#include "ActionManager.h"

#define MAXACTIONS 		   		20


void initActions()
{
	queueMotor 	   = xQueueCreate( MAXACTIONS, sizeof( struct Action ) );
	queueTarget    = xQueueCreate( MAXACTIONS, sizeof( Position ) );
}

void pushAction(char action)
{
	struct Action reg;
	switch(action)
	{
		case SPEED_UP:
			reg.type = SPEED_UP;
			reg.delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueMotor, ( void * ) &reg, portMAX_DELAY );
			break;
		case SPEED_DOWN:
			reg.type = SPEED_DOWN;
			reg.delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueMotor, ( void * ) &reg, portMAX_DELAY );
			break;
		case TURN_LEFT:
			reg.type = TURN_LEFT;
			reg.delayms = DELAY_DIRECTION_CHANGE;
			xQueueSend( queueMotor, ( void * ) &reg, portMAX_DELAY );
			break;
		case TURN_RIGHT:
			reg.type = TURN_RIGHT;
			reg.delayms = DELAY_DIRECTION_CHANGE;
			xQueueSend( queueMotor, ( void * ) &reg, portMAX_DELAY );
			break;
		case MOVE_STOP:
			reg.type = MOVE_STOP;
			reg.delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueMotor, ( void * ) &reg, portMAX_DELAY );
			break;
		case GOFORDWARD:
			if ((direction_Left == FORWARD) && (direction_Right == FORWARD))
				return;
			reg.type = MOVE_STOP;
			reg.delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueMotor, ( void * ) &reg, portMAX_DELAY );
			reg.type = GOFORDWARD;
			reg.delayms = DELAY_DIRECTION_CHANGE;
			xQueueSend( queueMotor, ( void * ) &reg, portMAX_DELAY );
			reg.type = SPEED_UP;
			reg.delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueMotor, ( void * ) &reg, portMAX_DELAY);
			break;
		case GOBACKWARD:
			if ((direction_Left == BACKWARD) && (direction_Right == BACKWARD))
				return;
			reg.type = MOVE_STOP;
			reg.delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueMotor, ( void * ) &reg, portMAX_DELAY );
			reg.type = GOBACKWARD;
			reg.delayms = DELAY_DIRECTION_CHANGE;
			xQueueSend( queueMotor, ( void * ) &reg, portMAX_DELAY );
			reg.type = SPEED_UP;
			reg.delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueMotor, ( void * ) &reg, portMAX_DELAY);
			break;
		case ROTATE_LEFT:
			reg.type = MOVE_STOP;
			reg.delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueMotor, ( void * ) &reg, portMAX_DELAY );
			reg.type = ROTATE_LEFT;
			reg.delayms = DELAY_DIRECTION_CHANGE;
			xQueueSend( queueMotor, ( void * ) &reg, portMAX_DELAY );
			reg.type = SPEED_UP;
			reg.delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueMotor, ( void * ) &reg, portMAX_DELAY);
			break;
		case ROTATE_RIGHT:
			reg.type = MOVE_STOP;
			reg.delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueMotor, ( void * ) &reg, portMAX_DELAY );
			reg.type = ROTATE_RIGHT;
			reg.delayms = DELAY_DIRECTION_CHANGE;
			xQueueSend( queueMotor, ( void * ) &reg, portMAX_DELAY );
			reg.type = SPEED_UP;
			reg.delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueMotor, ( void * ) &reg, portMAX_DELAY);
			break;
		case ROTATE_90:
			reg.type = MOVE_STOP;
			reg.delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueMotor, ( void * ) &reg, portMAX_DELAY );
			reg.type = ROTATE_90;
			reg.delayms = DELAY_DIRECTION_CHANGE;
			xQueueSend( queueMotor, ( void * ) &reg, portMAX_DELAY );
			break;
		case ROTATE_180:
			reg.type = MOVE_STOP;
			reg.delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueMotor, ( void * ) &reg, portMAX_DELAY );
			reg.type = ROTATE_180;
			reg.delayms = DELAY_DIRECTION_CHANGE;
			xQueueSend( queueMotor, ( void * ) &reg, portMAX_DELAY );
			break;
		case ROTATE_270:
			reg.type = MOVE_STOP;
			reg.delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueMotor, ( void * ) &reg, portMAX_DELAY );
			reg.type = ROTATE_270;
			reg.delayms = DELAY_DIRECTION_CHANGE;
			xQueueSend( queueMotor, ( void * ) &reg, portMAX_DELAY );
			break;
	    default:
			break;
	}

}

void doAction(char action)
{
	switch(action)
	{
		case SPEED_UP:
			move_SpeedUp();
			break;
		case SPEED_DOWN:
			move_SpeedDown();
			break;
		case TURN_LEFT:
			move_turnLeft();
			break;
		case TURN_RIGHT:
			move_turnRight();
			break;
		case MOVE_STOP:
			move_stop();
			break;
		case GOFORDWARD:
			move_Forward();
			break;
		case GOBACKWARD:
			move_Backward();
			break;
		case ROTATE_LEFT:
			move_Rotate(LEFT, 0);
			break;
		case ROTATE_RIGHT:
			move_Rotate(RIGHT, 0);
			break;
		case ROTATE_90:
			move_Rotate(RIGHT, 90);
			break;
		case ROTATE_180:
			move_Rotate(RIGHT, 180);
			break;
		case ROTATE_270:
			move_Rotate(LEFT, 90);
			break;
	    default:
			break;
	}
}
