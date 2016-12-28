/*
 * ActionManager.c
 *
 *  Created on: 18/12/2016
 *      Author: Carlos Miguens
 */

#include "Controller/moves.h"
#include "Driver/motor.h"
#include "ActionManager.h"

#define MAXACTIONS 		   		20


void initActions()
{
	queueDirection = xQueueCreate( MAXACTIONS, sizeof( struct Action ) );
	queueSpeed 	   = xQueueCreate( MAXACTIONS, sizeof( struct Action ) );
}

void pushAction(char action)
{
	struct Action reg;
	int iSpeedLeft, iSpeedRight;
	switch(action)
	{
		case SPEED_UP:
			reg.type = SPEED_UP;
			reg.delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueSpeed, ( void * ) &reg, ( TickType_t )  DELAY_SPEED_CHANGE+1 );
			break;
		case SPEED_DOWN:
			reg.type = SPEED_DOWN;
			reg.delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueSpeed, ( void * ) &reg, ( TickType_t )  DELAY_SPEED_CHANGE+1 );
			break;
		case TURN_LEFT:
			reg.type = TURN_LEFT;
			reg.delayms = DELAY_DIRECTION_CHANGE;
			xQueueSend( queueDirection, ( void * ) &reg, ( TickType_t )  DELAY_DIRECTION_CHANGE+1 );
			break;
		case TURN_RIGHT:
			reg.type = TURN_RIGHT;
			reg.delayms = DELAY_DIRECTION_CHANGE;
			xQueueSend( queueDirection, ( void * ) &reg, ( TickType_t )  DELAY_DIRECTION_CHANGE+1 );
			break;
		case MOVE_STOP:
			reg.type = MOVE_STOP;
			reg.delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueSpeed, ( void * ) &reg, ( TickType_t )  DELAY_SPEED_CHANGE+1 );
			break;
		case GOFORDWARD:
			iSpeedLeft = speed_Left;
			iSpeedRight = speed_Right;
			if ((direction_Left == FORWARD) && (direction_Right == FORWARD))
				return;
			reg.type = MOVE_STOP;
			reg.delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueSpeed, ( void * ) &reg, ( TickType_t )  DELAY_SPEED_CHANGE+1 );
			vTaskDelay(1000/portTICK_RATE_MS);
			reg.type = GOFORDWARD;
			reg.delayms = DELAY_DIRECTION_CHANGE;
			xQueueSend( queueDirection, ( void * ) &reg, ( TickType_t )  DELAY_DIRECTION_CHANGE+1 );
			speed_Left = iSpeedLeft;
			speed_Right = iSpeedRight;
			move_SpeedRefresh();
			break;
		case GOBACKWARD:
			iSpeedLeft = speed_Left;
			iSpeedRight = speed_Right;
			if ((direction_Left == BACKWARD) && (direction_Right == BACKWARD))
				return;
			reg.type = MOVE_STOP;
			reg.delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueSpeed, ( void * ) &reg, ( TickType_t )  DELAY_SPEED_CHANGE+1 );
			vTaskDelay(DELAY_DIRECTION_CHANGE/portTICK_RATE_MS);
			reg.type = GOBACKWARD;
			reg.delayms = DELAY_DIRECTION_CHANGE;
			xQueueSend( queueDirection, ( void * ) &reg, ( TickType_t )  DELAY_DIRECTION_CHANGE+1 );
			speed_Left = iSpeedLeft;
			speed_Right = iSpeedRight;
			move_SpeedRefresh();
			break;
		case ROTATE_LEFT:
			iSpeedLeft = speed_Left;
			iSpeedRight = speed_Right;
			reg.type = MOVE_STOP;
			reg.delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueSpeed, ( void * ) &reg, ( TickType_t )  DELAY_SPEED_CHANGE+1 );
			vTaskDelay(DELAY_DIRECTION_CHANGE/portTICK_RATE_MS);
			reg.type = ROTATE_LEFT;
			reg.delayms = DELAY_DIRECTION_CHANGE;
			xQueueSend( queueDirection, ( void * ) &reg, ( TickType_t )  DELAY_DIRECTION_CHANGE+1 );
			speed_Left = iSpeedLeft;
			speed_Right = iSpeedRight;
			move_SpeedRefresh();
			break;
		case ROTATE_RIGHT:
			iSpeedLeft = speed_Left;
			iSpeedRight = speed_Right;
			reg.type = MOVE_STOP;
			reg.delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueSpeed, ( void * ) &reg, ( TickType_t )  DELAY_SPEED_CHANGE+1 );
			vTaskDelay(DELAY_DIRECTION_CHANGE/portTICK_RATE_MS);
			reg.type = ROTATE_RIGHT;
			reg.delayms = DELAY_DIRECTION_CHANGE;
			xQueueSend( queueDirection, ( void * ) &reg, ( TickType_t )  DELAY_DIRECTION_CHANGE+1 );
			speed_Left = iSpeedLeft;
			speed_Right = iSpeedRight;
			move_SpeedRefresh();
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
			move_Rotate(LEFT, 90);
			break;
		case ROTATE_RIGHT:
			move_Rotate(RIGHT, 90);
			break;
	    default:
			break;
	}
}
