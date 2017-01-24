/*
 * acelerometro.c
 *
 *  Created on: Mar 25, 2016
 *      Author: Carlos Miguens
 * 		Proyecto Pathfinder
 */

/* Registros para FreeFall Motion */
#define ACCEL_FF_MT_CFG	  0x15
#define ACCEL_FF_MT_SRC   0x16
#define ACCEL_FF_MT_THS   0x17
#define ACCEL_FF_MT_COUNT 0x18
#define ACCEL_REG_1	  	  0x2A
#define ACCEL_REG_2	      0x2B
#define ACCEL_REG_3  	  0x2C
#define ACCEL_REG_4  	  0x2D
#define ACCEL_REG_5  	  0xE2
#define ACCEL_REG_RANGE   0x0E

#define ACCEL_ACTIVE_BIT_MASK 1
#define ACCEL_READ_BIT_MASK   1 << 1
#define ACCEL_NOISE_MASK	  1 << 2
#define ACCEL_DR0_MASK		  1 << 3
#define ACCEL_DR1_MASK		  1 << 4
#define ACCEL_DR2_MASK		  1 << 5
#define ACCEL_ASLP_RATE0	  1 << 6
#define ACCEL_ASLP_RATE1	  1 << 7

#define ACCEL_ANTIREBOTE 30


#include <Driver/acelerometro.h>
#include "MMA1.h"
#include "GI2C1.h"
#include "FreeRTOS.h"


/*
	1. Register 0x2B bit 2 – SLPE Enable Sleep bit
	2. Register 0x2B Set the Sleep Mode Oversampling Rate
	3. Register 0x2A Sleep Sample Rate and Wake Sample Rate
	4. Register 0x29 Timeout Counter
	5. Register 0x2D Enable the Interrupts for the Selected Functions
	6. Register 0x2E Route the Interrupts to INT1 or INT2
	7. Register 0x2C Enable the Wake-from-Sleep Interrupts
 *
 */

/*
 *  CTRL_REG1 Register (0x2A)
 *

Bit 7 		Bit 6 		Bit 5 	Bit 4 	Bit 3 	Bit 2 	Bit 1 	Bit 0
ASLP_RATE1 	ASLP_RATE0 	DR2 	DR1 	DR0 	LNOISE 	F_READ 	ACTIVE

Sleep Mode Sample Rate Description
ASLP_RATE1 ASLP_RATE0   ODR 	Period
0 			0 			50 Hz 	20 ms
0 			1 			12.5 Hz 80 ms
1 			0 			6.25 Hz 160 ms
1 			1 			1.56 Hz 640 ms

Wake Mode Sample Rate Description
DR2 DR1 DR0 ODR 	 Period
0	0 	0 	800.0 Hz 1.25 ms
0 	0 	1 	400.0 Hz 2.5 ms
0 	1 	0 	200.0 Hz 5 ms
0 	1 	1 	100.0 Hz 10 ms
1 	0 	0 	50.0 Hz  20 ms
1 	0 	1 	12.5 Hz  80 ms
1 	1 	0 	6.25 Hz  160 ms
1 	1 	1 	1.56 Hz  640 ms

*
*  0x2B CTRL_REG2 Register (Read/Write) and Description
*

Bit 7 	Bit 6 	Bit 5 	Bit 4 	Bit 3 	Bit 2 	Bit 1 	Bit 0
ST 		RST 	0 		SMODS1 	SMODS0 	SLPE 	MODS1 	MODS0

Settings for Oversampling Modes
SMODS1 	SMODS0 	Power Mode
0			0 	Normal
0 			1 	Low Noise and Low Power
1 			0 	High Resolution
1 			1 	Low Power


*
*  0x29 ASLP_COUNT Register (Read/Write) and Description
*


*
*  The interrupt functions must be enabled in Register 0x2D
*

Register 0x2D CTRL_REG4 Register

Bit 7 		Bit 6 		Bit 5 		 Bit 4 			Bit 3 			Bit 2 			Bit 1 	Bit 0
INT_EN_ASLP INT_EN_FIFO INT_EN_TRANS INT_EN_LNDPRT	INT_EN_PULSE 	INT_EN_FF_MT 	— 		INT_EN_DRDY

*
* 0x0E XYZ_DATA_CFG Register (Read/Write)
*

Bit 7 	Bit 6 	Bit 5 	Bit 4 	Bit 3 	Bit 2 	Bit 1 Bit 0
— 		0	0 	0		HPF_Out	0 		0 		FS1 	FS0

Full Scale Selection
FS1 FS0 g Range
0 	0 	±2g
0 	1 	±4g
1 	0 	±8g
1 	1

 */

#define ACCEL_LOW_POWER 		0
#define ACCEL_HIGH_RESOLUTION 	1

int boardOrientation[3]; // stored initial gravity

void Accel_calibrate()
{
	MMA1_CalibrateX1g();
	MMA1_CalibrateY1g();
	MMA1_CalibrateZ1g();
	boardOrientation[0] = MMA1_GetX();
	boardOrientation[1] = MMA1_GetY();
	boardOrientation[2] = MMA1_GetZ();
}

void Accel_Init()
{
	MMA1_Init();
	Accel_calibrate();
}

#define GRAVITYCHANGUI	2000
int Accel_isOverturn()
{
	if ((boardOrientation[0] + GRAVITYCHANGUI < gforceXYZ[0]) || (boardOrientation[0] - GRAVITYCHANGUI > gforceXYZ[0]) ) return 1;
	if ((boardOrientation[1] + GRAVITYCHANGUI < gforceXYZ[1]) || (boardOrientation[1] - GRAVITYCHANGUI > gforceXYZ[1]) ) return 1;
	if ((boardOrientation[2] + GRAVITYCHANGUI < gforceXYZ[2]) || (boardOrientation[2] - GRAVITYCHANGUI > gforceXYZ[2]) ) return 1;
    return 0;
}

// detecta si hay movimiento
int getMovimiento()
{
	int16_t xyz[3], xyzold[3], cambioEstado;
	Gforce movimiento;

			xyzold[0] = MMA1_GetX();
			xyzold[1] = MMA1_GetY();
			xyzold[2] = MMA1_GetZ();

			FRTOS1_vTaskDelay(100/portTICK_RATE_MS);

			xyz[0] = MMA1_GetX();
			xyz[1] = MMA1_GetY();
			xyz[2] = MMA1_GetZ();

			movimiento.xyz[0] = (xyz[0] > xyzold[0])?xyz[0]-xyzold[0]:xyzold[0]-xyz[0];
			movimiento.xyz[1] = (xyz[1] > xyzold[1])?xyz[1]-xyzold[1]:xyzold[1]-xyz[1];
			movimiento.xyz[2] = (xyz[2] > xyzold[2])?xyz[2]-xyzold[2]:xyzold[2]-xyz[2];

			movimiento.xyz[0] = (movimiento.xyz[0]<0)?movimiento.xyz[0]*-1:movimiento.xyz[0];
			movimiento.xyz[1] = (movimiento.xyz[1]<0)?movimiento.xyz[1]*-1:movimiento.xyz[1];
			movimiento.xyz[2] = (movimiento.xyz[2]<0)?movimiento.xyz[2]*-1:movimiento.xyz[2];

			if ((movimiento.xyz[0]< ACCEL_ANTIREBOTE)&&
				(movimiento.xyz[1]< ACCEL_ANTIREBOTE)&&
				(movimiento.xyz[2]< ACCEL_ANTIREBOTE))
			{
				if (cambioEstado == 1)
				{
					// poner en cola mensaje de quieto
					cambioEstado = 0;
//!!!!!!					BT_showString("Quieto\r\n\0");
				}
			}
			if ((movimiento.xyz[0]> ACCEL_ANTIREBOTE)&&
				(movimiento.xyz[1]> ACCEL_ANTIREBOTE)&&
				(movimiento.xyz[2]> ACCEL_ANTIREBOTE))
			{
				// en movimiento
				if (cambioEstado == 0)
				{
					cambioEstado = 1;
//!!!!!!					BT_showString("Movimiento\r\n\0");
				}
			}

	return cambioEstado;
}

void Accel_Wake()
{
	MMA1_Enable();
}

void Accel_Sleep()
{
	MMA1_Disable();
}


