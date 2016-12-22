/*
 * ActionManager.c
 *
 *  Created on: 18/12/2016
 *      Author: Carlos Miguens
 */

#include "Controller/moves.h"
#include "ActionManager.h"

#define MAXACTIONS 		   		20
#define DELAY_SPEED_CHANGE 		25
#define DELAY_DIRECTION_CHANGE  25


void initActions()
{
	queueDirection = xQueueCreate( MAXACTIONS, sizeof( Action ) );
	queueSpeed 	   = xQueueCreate( MAXACTIONS, sizeof( Action ) );
}

void pushAction(char action)
{
	Action *reg = malloc(sizeof(Action));
	switch(action)
	{
		case SPEED_UP:
			reg->type = SPEED_UP;
			reg->delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueSpeed, ( void * ) &reg, ( TickType_t )  DELAY_SPEED_CHANGE+1 );
			break;
		case SPEED_DOWN:
			reg->type = SPEED_DOWN;
			reg->delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueSpeed, ( void * ) &reg, ( TickType_t )  DELAY_SPEED_CHANGE+1 );
			break;
		case TURN_LEFT:
			reg->type = TURN_LEFT;
			reg->delayms = DELAY_DIRECTION_CHANGE;
			xQueueSend( queueDirection, ( void * ) &reg, ( TickType_t )  DELAY_DIRECTION_CHANGE+1 );
			break;
		case TURN_RIGHT:
			reg->type = TURN_RIGHT;
			reg->delayms = DELAY_DIRECTION_CHANGE;
			xQueueSend( queueDirection, ( void * ) &reg, ( TickType_t )  DELAY_DIRECTION_CHANGE+1 );
			break;
		case MOVE_STOP:
			reg->type = MOVE_STOP;
			reg->delayms = DELAY_SPEED_CHANGE;
			xQueueSend( queueSpeed, ( void * ) &reg, ( TickType_t )  DELAY_SPEED_CHANGE+1 );
			break;
		case GOFORDWARD:
			reg->type = GOFORDWARD;
			reg->delayms = DELAY_DIRECTION_CHANGE;
			xQueueSend( queueDirection, ( void * ) &reg, ( TickType_t )  DELAY_DIRECTION_CHANGE+1 );
			break;
		case GOBACKWARD:
			reg->type = GOBACKWARD;
			reg->delayms = DELAY_DIRECTION_CHANGE;
			xQueueSend( queueDirection, ( void * ) &reg, ( TickType_t )  DELAY_DIRECTION_CHANGE+1 );
			break;
		case ROTATE_LEFT:
			reg->type = ROTATE_LEFT;
			reg->delayms = DELAY_DIRECTION_CHANGE;
			xQueueSend( queueDirection, ( void * ) &reg, ( TickType_t )  DELAY_DIRECTION_CHANGE+1 );
			break;
		case ROTATE_RIGHT:
			reg->type = ROTATE_RIGHT;
			reg->delayms = DELAY_DIRECTION_CHANGE;
			xQueueSend( queueDirection, ( void * ) &reg, ( TickType_t )  DELAY_DIRECTION_CHANGE+1 );
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
