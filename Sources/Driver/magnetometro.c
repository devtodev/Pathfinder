/*
 * magnetometro.c
 *
 *  Created on: Dec 28, 2016
 *      Author: Carlos Miguens
 * 		Proyecto Pathfinder
 */

#include "MAG1.h"
#include "magnetometro.h"

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

/* multiplicative conversion constants */
#define DegToRad 0.017453292F
#define RadToDeg 57.295779F
/* NED tilt-compensated e-Compass function */
void eCompass(float Bx, float By, float Bz, float Gx, float Gy, float Gz)
{
	float sinAngle, cosAngle; /* sine and cosine */
	float Bfx, Bfy, Bfz; /* calibrated mag data in uT after tilt correction */
	/* subtract off the hard iron interference computed using equation 9 */
	Bx -= Vx;
	By -= Vy;
	Bz -= Vz;
	/* calculate roll angle Phi (-180deg, 180deg) and sin, cos */
	Phi = atan2(Gy, Gz) * RadToDeg; /* Equation 2 */
	sinAngle = sin(Phi * DegToRad); /* sin(Phi) */
	cosAngle = cos(Phi * DegToRad); /* cos(Phi) */
	/* de-rotate by roll angle Phi */
	Bfy = By * cosAngle - Bz * sinAngle; /* Equation 5 y component */
	Bz = By * sinAngle + Bz * cosAngle; /* Bz=(By-Vy).sin(Phi)+(Bz-Vz).cos(Phi) */
	Gz = Gy * sinAngle + Gz * cosAngle; /* Gz=Gy.sin(Phi)+Gz.cos(Phi) */
	/* calculate pitch angle Theta (-90deg, 90deg) and sin, cos*/
	The = atan(-Gx / Gz) * RadToDeg; /* Equation 3 */
	sinAngle = sin(The * DegToRad); /* sin(Theta) */
	cosAngle = cos(The * DegToRad); /* cos(Theta) */
	/* de-rotate by pitch angle Theta */
	Bfx = Bx * cosAngle + Bz * sinAngle; /* Equation 5 x component */
	Bfz = -Bx * sinAngle + Bz * cosAngle; /* Equation 5 z component */
	/* calculate yaw = ecompass angle psi (-180deg, 180deg) */
	Psi = atan2(-Bfy, Bfx) * RadToDeg; /* Equation 7 */
	return;
}



