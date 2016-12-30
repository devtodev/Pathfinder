/*
 * megnetometro.c
 *
 *  Created on: Dec 28, 2016
 *      Author: Carlos Miguens
 * 		Proyecto Pathfinder
 */

/* ecompas variables */
float Psi, The, Phi; /* yaw, pitch, roll angles in deg */
float Vx, Vy, Vz; /* hard iron calibration coefficients */

void Mag_Init(void);
void Mag_Wake();
void Mag_Sleep();
void eCompass(float Bx, float By, float Bz, float Gx, float Gy, float Gz);
