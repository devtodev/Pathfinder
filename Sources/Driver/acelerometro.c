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

void Accel_Init()
{
	MMA1_Init();
}

void Accel_Init_old(void)
{
	unsigned char ctrl_reg_1 = 0,  ctrl_reg_2 = 0,  ctrl_reg_3 = 0, ctrl_reg_5 = 0,  ctrl_reg_4 = 0, ctrl_reg_range = 0;
	unsigned char ff_mm_cfg = 0;

	uint8_t buf, rbuf;
	static const uint8_t addr = ACCEL_FF_MT_CFG; //ACCEL_FF_MT_SRC;

	ctrl_reg_1 |= ACCEL_ACTIVE_BIT_MASK;
	/*
	// configuracion sleep mode rate 640 ms
	ctrl_reg_1 |= ACCEL_ASLP_RATE0;
	ctrl_reg_1 |= ACCEL_ASLP_RATE1;
	// Wake Mode Sample Rate rate 10 ms
	ctrl_reg_1 |= ACCEL_DR0_MASK;
	ctrl_reg_1 |= ACCEL_DR1_MASK;
	#if ACCEL_LOW_POWER
	ctrl_reg_2 |= 1 << 3;
	ctrl_reg_2 |= 1 << 4;
	#endif
	#if ACCEL_HIGH_RESOLUTION
	ctrl_reg_2 |= 1 << 4;
	#endif
	// Enable sleep mode
	ctrl_reg_2 |= 1 << 2;
	*/
	// Choose Motion to wake the device from sleep
	ctrl_reg_3 |= 1 << 3; //WAKE_FF_MT
	// interrupt enable INT1
	ctrl_reg_4 |= 1 << 7; // INT_EN_ASLP   Auto-SLEEP/WAKE interrupt enabled.
	ctrl_reg_4 |= 1 << 2; // INT_EN_FF_MT  Freefall/Motion interrupt enabled
	ctrl_reg_4 |= 1; 	  // INT_EN_DRDY   Data Ready interrupt enabled

	//  interrupt enable INT2
	ctrl_reg_5 |= 1 << 7; // INT_CFG_ASLP   0: Interrupt is routed to INT2 pin; 1: Interrupt is routed to INT1 pin
	ctrl_reg_5 |= 1 << 2; // INT_CFG_FF_MT  0: Interrupt is routed to INT2 pin; 1: Interrupt is routed to INT1 pin
	ctrl_reg_5 |= 1; 	  // INT_CFG_DRDY   Data Ready interrupt enabled
	/*
	// Set the Dynamic Range to 4g
	ctrl_reg_range |= 1;

	MMA1_CalibrateX1g();
	MMA1_CalibrateY1g();
	MMA1_CalibrateZ1g();
*/
	/*
	* Freefall/Motion functional block configuration
	* 0x15: FF_MT_CFG Freefall/Motion Configuration Register
	Bit 7 	Bit 6 	Bit 5 	Bit 4 	Bit 3 	Bit 2 	Bit 1 	Bit 0
	ELE 	OAE 	ZEFE 	YEFE 	XEFE 	— 		— 		—

	 ELE 0: Event flag latch disabled; 1: event flag latch enabled (FROZEN FF_MT_SRC)
	 OAE 0: Freefall Flag 1: Motion Flag
	 ZEFE YEFE XEFE 0: event detection disabled 1: raise event flag on measured acceleration value beyond preset threshold

	* 0x16: FF_MT_SRC Freefall/Motion Source Register
	Bit 7 	Bit 6 	Bit 5 	Bit 4 	Bit 3 	Bit 2 	Bit 1 	Bit 0
	EA 		— 		ZHE 	ZHP 	YHE 	YHP 	XHE 	XHP

	 EA 0 No event flag has been asserted; 1: one or more event flag has been asserted
	 ZHE YHE XHE Motion event detected
	 ZHP YHP XHP Motion Polarity Flag

	* 0x17: FF_MT_THS Register (Read/Write)
	Bit 7 	Bit 6 	Bit 5 	Bit 4 	Bit 3 	Bit 2 	Bit 1 	Bit 0
	DBCNTM 	THS6 	THS5 	THS4 	THS3 	THS2 	THS1 	THS0

	 DBCNTM 0: increments or decrements debounce 1: increments or clears counter
	 THS[6:0] Freefall /Motion Threshold: Default value: 000_0000

	* 0x18: FF_MT_COUNT Debounce Register
	Bit 7 	Bit 6 	Bit 5 	Bit 4 	Bit 3 	Bit 2 	Bit 1 	Bit 0
	D7 		D6		D5 		D4 		D3 		D2 		D1 		D0


	 */

	/* Mode 3: Motion Detection with ELE = 0, OAE = 1
	In this mode, the EA bit indicates a motion event after the debounce counter time is reached. The ZEFE, YEFE, and XEFE
	control bits determine which axes are taken into consideration for motion detection. Once the EA bit is set, and DBCNTM = 0,
	the EA bit can get cleared only after the delay specified by FF_MT_COUNT. If DBCNTM = 1, the EA bit is cleared as soon as the
	motion high g condition disappears. The event flags ZHE, ZHP, YHE, YHP, XHE, and XHP reflect the motion detection status
	(i.e., high g event) without any debouncing, provided that the corresponding bits ZEFE, YEFE, and/or XEFE are set. Reading the
	FF_MT_SRC does not clear any flags, nor is the debounce counter reset.
	*/
	ff_mm_cfg |= 1 << 3; // x Event flag enable
	ff_mm_cfg |= 1 << 4; // y Event flag enable
	ff_mm_cfg |= 1 << 5; // z Event flag enable
	ff_mm_cfg |= 1 << 6; // motion detection

	// The interrupts
	GI2C1_SelectSlave(MMA1_I2C_ADDR);
	GI2C1_Init();
	MMA1_Init();
	FRTOS1_vTaskDelay(700/portTICK_RATE_MS);
	GI2C1_WriteByteAddress8(MMA1_I2C_ADDR, ACCEL_REG_1, ctrl_reg_1);
	GI2C1_WriteByteAddress8(MMA1_I2C_ADDR, ACCEL_REG_2, ctrl_reg_2);
	GI2C1_WriteByteAddress8(MMA1_I2C_ADDR, ACCEL_REG_4, ctrl_reg_4);
	GI2C1_WriteByteAddress8(MMA1_I2C_ADDR, ACCEL_REG_5, ctrl_reg_5);
	GI2C1_WriteByteAddress8(MMA1_I2C_ADDR, 0x15, ff_mm_cfg); // ACCEL_FF_MT_CFG
	FRTOS1_vTaskDelay(700/portTICK_RATE_MS);
	GI2C1_ReadByteAddress8(MMA1_I2C_ADDR, 0x0D, &buf); // WHO_I_AM = 0x1a
	GI2C1_ReadByteAddress8(MMA1_I2C_ADDR, 0x15, &buf); // ACCEL_FF_MT_CFG = 0 why??? :-(
	return 0;
}

// detecta si hay movimiento
int getMovimiento()
{
	int16_t xyz[3], xyzold[3], cambioEstado;
	Movimiento movimiento;

			xyzold[0] = MMA1_GetX();
			xyzold[1] = MMA1_GetY();
			xyzold[2] = MMA1_GetZ();

			FRTOS1_vTaskDelay(100/portTICK_RATE_MS);

			xyz[0] = MMA1_GetX();
			xyz[1] = MMA1_GetY();
			xyz[2] = MMA1_GetZ();

			movimiento.x = (xyz[0] > xyzold[0])?xyz[0]-xyzold[0]:xyzold[0]-xyz[0];
			movimiento.y = (xyz[1] > xyzold[1])?xyz[1]-xyzold[1]:xyzold[1]-xyz[1];
			movimiento.z = (xyz[2] > xyzold[2])?xyz[2]-xyzold[2]:xyzold[2]-xyz[2];

			movimiento.x = (movimiento.x<0)?movimiento.x*-1:movimiento.x;
			movimiento.y = (movimiento.y<0)?movimiento.y*-1:movimiento.y;
			movimiento.z = (movimiento.z<0)?movimiento.z*-1:movimiento.z;

			if ((movimiento.x< ACCEL_ANTIREBOTE)&&
				(movimiento.y< ACCEL_ANTIREBOTE)&&
				(movimiento.z< ACCEL_ANTIREBOTE))
			{
				if (cambioEstado == 1)
				{
					// poner en cola mensaje de quieto
					cambioEstado = 0;
//!!!!!!					BT_showString("Quieto\r\n\0");
				}
			}
			if ((movimiento.x> ACCEL_ANTIREBOTE)&&
				(movimiento.y> ACCEL_ANTIREBOTE)&&
				(movimiento.z> ACCEL_ANTIREBOTE))
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

/*
 *
Movimiento getMovimiento()
{
    Movimiento resultado;
	unsigned char lectura;
	uint8_t buf;
	static const uint8_t addr = ACCEL_FF_MT_SRC;

	if(GI2C1_ReadAddress(MMA1_I2C_ADDR, (uint8_t*)&addr, sizeof(addr), &buf, 1)!=ERR_OK) {
		resultado.flag = -1;
		return resultado;
	}
	lectura = buf;
	/*
		resultado.flag = 0;
		return resultado;	* 0x16: FF_MT_SRC Freefall/Motion Source Register
	Bit 7 	Bit 6 	Bit 5 	Bit 4 	Bit 3 	Bit 2 	Bit 1 	Bit 0
	EA 		— 		ZHE 	ZHP 	YHE 	YHP 	XHE 	XHP

	 EA 0 No event flag has been asserted; 1: one or more event flag has been asserted
	 ZHE YHE XHE Motion event detected
	 ZHP YHP XHP Motion Polarity Flag

	 * *

	if (!lectura && (1 << 7))
	{
		resultado.flag = 0;
		resultado.x = 0;
		resultado.y = 0;
		resultado.z = 0;
		return resultado;
	}
	// se detecto movimiento !
	resultado.flag = 1;

	if (lectura && (1 << 1))
	{
		resultado.x = (lectura && 1)?ACCEL_POSITIVO:ACCEL_NEGATIVO;
	} else {
		resultado.x = ACCEL_NEUTRO;
	}

	if (lectura && (1 << 3))
	{
		resultado.y = (lectura && (1<< 2))?ACCEL_POSITIVO:ACCEL_NEGATIVO;
	} else {
		resultado.y = ACCEL_NEUTRO;
	}

	if (lectura && (1 << 5))
	{
		resultado.z = (lectura && (1<< 4))?ACCEL_POSITIVO:ACCEL_NEGATIVO;
	} else {
		resultado.z = ACCEL_NEUTRO;
	}

	return resultado;
	*/


void Accel_Wake()
{
	MMA1_Enable();
}

void Accel_Sleep()
{
	MMA1_Disable();
}


