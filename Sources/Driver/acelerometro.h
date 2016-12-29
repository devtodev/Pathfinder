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

typedef struct {
	char x, y, z;
	char flag;
} Movimiento;


void Accel_Init();
void Accel_Wake();
void Accel_Sleep();
int getMovimiento();
