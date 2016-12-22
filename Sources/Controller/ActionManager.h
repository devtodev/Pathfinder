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

void initActions();
void doAction(char action);
void pushAction(char action);

QueueHandle_t queueSpeed, queueDirection;

typedef struct {
	char type;
	int delayms;
} Action;
