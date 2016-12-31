/*
 * ActionManager.h
 *
 *  Created on: 18/12/2016
 *      Author: Carlos Miguens
 */

#include "FRTOS1.h"
#include "FreeRTOS.h"
#include "queue.h"


#define MOVE_STOP	 'x'
#define SPEED_UP	 'w'
#define SPEED_DOWN	 's'
#define TURN_LEFT	 'a'
#define TURN_RIGHT   'd'
#define GOFORDWARD	 'i'
#define GOBACKWARD	 'k'
#define ROTATE_LEFT  'j'
#define ROTATE_RIGHT 'l'
#define ROTATE_90 	 'm'
#define ROTATE_180 	 'n'
#define ROTATE_270 	 'b'

void initActions();
void doAction(char action);
void pushAction(char action);


struct Action {
	char type;
	int delayms;
} tAction;

QueueHandle_t queueMotor, queueDirection;
