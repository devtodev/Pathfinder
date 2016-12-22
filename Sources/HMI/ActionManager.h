/*
 * ActionManager.h
 *
 *  Created on: 18/12/2016
 *      Author: Carlos Miguens
 */

#define MOVE_STOP	 'x'
#define SPEED_UP	 'w'
#define SPEED_DOWN	 's'
#define TURN_LEFT	 'a'
#define TURN_RIGHT   'd'
#define GOFORDWARD	 'i'
#define GOBACKWARD	 'k'
#define ROTATE_LEFT  'j'
#define ROTATE_RIGHT 'l'

void pushAction(char action);
void doAction();
