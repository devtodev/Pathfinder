/*
 * Georeference.h
 *
 *  Created on: 30/12/2016
 *      Author: karl
 */

#ifndef SOURCES_CONTROLLER_GEOREFERENCE_H_
#define SOURCES_CONTROLLER_GEOREFERENCE_H_

#include "stdint.h";

typedef struct {
	int16_t magneticFields[3];
	float Psi, The, Phi; /* yaw, pitch, roll angles in deg */
	float Vx, Vy, Vz; /* hard iron calibration coefficients */
} Orientation;

  Orientation *orientation;

  Orientation *getOrientation();
  void setOrientation(Orientation *position);

#endif /* SOURCES_CONTROLLER_GEOREFERENCE_H_ */
