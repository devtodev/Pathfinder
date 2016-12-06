/*
 * Ultrasonic.c
 *
 *  Created on: 03/11/2016
 *      Author: Carlos Miguens
 */
#include <middleware/Distance.h>
#include "drivers/Ultrasonic.h"

#define BUFFERDISTANCESIZE 20

int bufferDistanceFront[BUFFERDISTANCESIZE];
int iBufferFront;

void Distance_doMeaseure()
{
		bufferDistanceFront[iBufferFront] = getDistanceFront();
		if (bufferDistanceFront[iBufferFront] != 0)
		{
			iBufferFront++;
			if (iBufferFront > BUFFERDISTANCESIZE)
			{
				iBufferFront = 0;
			}
		}
}

int Distance_getFront()
{
	int distanceFront, i;
	distanceFront = 0;
	for (i = 0; i < BUFFERDISTANCESIZE; i++)
	{
		distanceFront = distanceFront + bufferDistanceFront[i];
	}
	distanceFront = distanceFront / BUFFERDISTANCESIZE;
	return distanceFront;
}


void Distance_init()
{
	US_Init();
	iBufferFront = 0;
}
