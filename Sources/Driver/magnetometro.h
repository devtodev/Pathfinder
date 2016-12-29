/*
 * megnetometro.c
 *
 *  Created on: Dec 28, 2016
 *      Author: Carlos Miguens
 * 		Proyecto Pathfinder
 */

typedef struct {
	int16_t xyz[3];
	char temperature;
} Posicion;

void Mag_Init(void);
void Mag_Wake();
void Mag_Sleep();
Posicion getPosicion();
