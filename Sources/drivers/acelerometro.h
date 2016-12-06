/*
 * Accelerometro_driver.c
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

void Accel_Init(void);
void Accel_Wake();
Movimiento getMovimiento();
