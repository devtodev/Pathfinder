/*
 * motor.c
 *
 *  Created on: 17/10/2016
 *      Author: Carlos Miguens
 */

#include "motor.h"
#include "MotorSpeed_4.h"
#include "MotorSpeed_3.h"
#include "MotorsDirection.h"


#define HC595_SHIFT_MSB_FIRST 1
int latch_state = 0;

void HC595_ShiftByte(int val)
{
	uint8_t i;

	/* precondition: latch pin, data pin and clock pin are all low */
	for(i=0;i<8;i++) { /* shift all the 8 bits */
	/* put data bit */
	#if HC595_SHIFT_MSB_FIRST
		if (val&0x80) { /* LSB bit first */
	#else /* LSB first */
		if (val&1) { /* LSB bit first */
	#endif
		  DS1_SetVal();
		} else {
		  DS1_ClrVal();
		}
		SHCP1_SetVal(); /* CLK high: data gets transfered into memory */
		DS1_ClrVal(); /* data line low */
		SHCP1_ClrVal(); /* CLK high: data gets transfered into memory */
	#if HC595_SHIFT_MSB_FIRST
		val <<= 1; /* next bit */
	#else
		val >>= 1; /* next bit */
	#endif
	  }

	/* send a latch pulse to show the data on the output pins */
	STCP1_SetVal(); /* set latch to high */
	STCP1_ClrVal(); /* set latch to low */
}

void motorsInit()
{
	OE1_PutVal(0); // enable output of the shift register
	motorSetSpeed(MOTORRIGHT, 0);
	motorSetSpeed(MOTORLEFT, 0);
	motorDirection(MOTORRIGHT, RELEASE);
	motorDirection(MOTORRIGHT, RELEASE);
}

void motorSetSpeed(int motornum, int speed)
{
	switch(motornum) {
		case MOTORLEFT:
			MotorSpeed_3_SetRatio16(speed);
			break;
		case MOTORRIGHT:
			MotorSpeed_4_SetRatio16(speed);
			break;
	}

}


int motorDirection(int motornum, int cmd) {
  int a, b;
  /* Section 1: choose two shift register outputs based on which
   * motor this instance is associated with.   motornum is the
   * motor number that was passed to this instance's constructor.
   */
  switch (motornum) {
	  case 1:
		a = MOTOR1_A; b = MOTOR1_B; break;
	  case 2:
		a = MOTOR2_A; b = MOTOR2_B; break;
	  case MOTORLEFT:
		a = MOTOR3_A; b = MOTOR3_B; break;
	  case MOTORRIGHT:
		a = MOTOR4_A; b = MOTOR4_B; break;
	  default:
		return -1;
  }

  /* Section 2: set the selected shift register outputs to high/low,
   * low/high, or low/low depending on the command.  This is done
   * by updating the appropriate bits of latch_state and then
   * calling tx_latch() to send latch_state to the chip.
   */
  switch (cmd) {
  case FORWARD:               // high/low
    latch_state |= (1 << a);
    latch_state &= ~(1 << b);
    break;
  case BACKWARD:              // low/high
    latch_state &= ~ (1 << a);
    latch_state |= (1 << b);
    break;
  case RELEASE:               // low/low
    latch_state &= ~(1 << a);
    latch_state &= ~(1 << b);
    break;
  }

  HC595_ShiftByte(latch_state);
}
