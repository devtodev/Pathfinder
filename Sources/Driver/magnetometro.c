/*
 * magnetometro.c
 *
 *  Created on: Dec 28, 2016
 *      Author: Carlos Miguens
 * 		Proyecto Pathfinder
 */

#include "MAG1.h"
#include "magnetometro.h"

Posicion posicion;

void Mag_Init(void)
{
	MAG1_Init();
	MAG1_Enable();
}

void Mag_Wake()
{
	MAG1_Enable();
}

void Mag_Sleep()
{
	MAG1_Disable();
}
Posicion getPosicion()
{
	MAG1_GetTemperature(&posicion.temperature);
	MAG1_GetXYZ16(&posicion.xyz);
	return posicion;
}



