/*
 * Navigation.h
 *
 *  Created on: 30/12/2016
 *      Author: karl
 */

#ifndef SOURCES_CONTROLLER_NAVIGATION_H_
#define SOURCES_CONTROLLER_NAVIGATION_H_

#include "stdint.h"

typedef struct {
	int16_t magneticFields[3];
	float Psi, The, Phi; /* yaw, pitch, roll angles in deg */
	float Vx, Vy, Vz; /* hard iron calibration coefficients */
} Orientation;

typedef struct {
	int16_t xyz[3];
	Orientation orientation;
} Position;

Position position;

  Position target;

  int travelMeetTheTarget();
  int travelAddNewTarget(Position newTarget);

#endif /* SOURCES_CONTROLLER_NAVIGATION_H_ */
