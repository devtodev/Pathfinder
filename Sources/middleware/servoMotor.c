/*
 * motor.h
 *
 *  Created on: 17/10/2016
 *      Author: Carlos Miguens
 */

#include "servoMotor.h"
#include "SERVO1.h"
#include "FRTOS1.h"

int angle, speed;

void setServoOnLeft()
{
	for (angle = angle; angle >= 0; angle--)
	  {
			  SERVO1_SetPos(angle);
			  vTaskDelay(speed/portTICK_RATE_MS);
	  }
}

void setServoOnRight()
{
	for (angle = angle; angle <= 255; angle++)
	  {
			  SERVO1_SetPos(angle);
			  vTaskDelay(speed/portTICK_RATE_MS);
	  }
}

void setServoOnCenter()
{
	if (angle > 128)
	{
		for (angle = angle; angle >= 128; angle--)
		  {
				  SERVO1_SetPos(angle);
				  vTaskDelay(20/portTICK_RATE_MS);
		  }
	} else {
		for (angle = angle; angle <= 128; angle++)
		  {
				  SERVO1_SetPos(angle);
				  vTaskDelay(20/portTICK_RATE_MS);
		  }
	}
}

void getServoPanoramic()
{
	uint16_t SERVO1_position;
	for (SERVO1_position = 1; SERVO1_position <= 255; SERVO1_position++)
	  {
			  SERVO1_SetPos(SERVO1_position);
			  vTaskDelay(20/portTICK_RATE_MS);
	  }
}

int getServoAngle()
{
	return angle;
}

void setServoAngle(int newAngle)
{
	angle = newAngle;
}

void setServoSpeed(int newSpeed)
{
	speed = newSpeed;
}

void getServoSpeed()
{
	return speed;
}

void servoInit() {
	speed = 20;
	angle = 0;

}
