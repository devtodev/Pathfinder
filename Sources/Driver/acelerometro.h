/*
 * acelerometro.c
 *
 *  Created on: Mar 25, 2016
 *      Author: Carlos Miguens
 * 		Proyecto Pathfinder
 */

#define ACCEL_NEUTRO   0
#define ACCEL_POSITIVO 1
#define ACCEL_NEGATIVO -1
#include "stdint.h"

typedef struct {
	int16_t xyz[3];
} Gforce;


void Accel_Init();
void Accel_Wake();
void Accel_Sleep();
int getMovimiento();
