/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : MotorsDirection.h
**     Project     : Pathfinder
**     Processor   : MKL46Z256VLL4
**     Component   : 74HC595
**     Version     : Component 01.022, Driver 01.00, CPU db: 3.00.000
**     Repository  : My Components
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-12-10, 12:54, # CodeGen: 64
**     Abstract    :
**
Driver for the 74HC595 8bit serial shift register.
**     Settings    :
**          Component name                                 : MotorsDirection
**          Latch (STCP)                                   : STCP
**          Data (DS)                                      : DS
**          Clock (SHCP)                                   : SHCP
**          Clock/Latch delay (ns)                         : 19
**          Wait                                           : WAIT1
**          MSB first                                      : no
**          Enable Pin (OE)                                : Enabled
**            Enable in Init                               : yes
**            Enable (OE)                                  : OE
**     Contents    :
**         Enable    - void MotorsDirection_Enable(void);
**         Disable   - void MotorsDirection_Disable(void);
**         ShiftByte - void MotorsDirection_ShiftByte(byte val);
**         Latch     - void MotorsDirection_Latch(void);
**         ReadByte  - byte MotorsDirection_ReadByte(void);
**         WriteByte - void MotorsDirection_WriteByte(byte val);
**         Deinit    - void MotorsDirection_Deinit(void);
**         Init      - void MotorsDirection_Init(void);
**
**     License   :  Open Source (LGPL)
**     Copyright : (c) Copyright Erich Styger, 2013, all rights reserved.
**     Web: http://mcuoneclipse.com
**     This an open source software implementing a driver using Processor Expert.
**     This is a free software and is opened for education, research and commercial developments under license policy of following terms:
**     * This is a free software and there is NO WARRANTY.
**     * No restriction on use. You can use, modify and redistribute it for personal, non-profit or commercial product UNDER YOUR RESPONSIBILITY.
**     * Redistributions of source code must retain the above copyright notice.
** ###################################################################*/
/*!
** @file MotorsDirection.h
** @version 01.00
** @brief
**
Driver for the 74HC595 8bit serial shift register.
*/         
/*!
**  @addtogroup MotorsDirection_module MotorsDirection module documentation
**  @{
*/         

#ifndef __MotorsDirection_H
#define __MotorsDirection_H

/* MODULE MotorsDirection. */

/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* Include inherited beans */
#include "STCP1.h"
#include "DS1.h"
#include "SHCP1.h"
#include "WAIT1.h"
#include "OE1.h"

#include "Cpu.h"




void MotorsDirection_Deinit(void);
/*
** ===================================================================
**     Method      :  MotorsDirection_Deinit (component 74HC595)
**     Description :
**         Deinitializes the driver
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void MotorsDirection_Init(void);
/*
** ===================================================================
**     Method      :  MotorsDirection_Init (component 74HC595)
**     Description :
**         Initializes the driver
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void MotorsDirection_Latch(void);
/*
** ===================================================================
**     Method      :  MotorsDirection_Latch (component 74HC595)
**     Description :
**         Sends a latch pulse to move the bits from the internal
**         memory to the output pins.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void MotorsDirection_ShiftByte(byte val);
/*
** ===================================================================
**     Method      :  MotorsDirection_ShiftByte (component 74HC595)
**     Description :
**         Shift a byte to the shift register. The most significant bit
**         will be on output pin 0. Use Latch() to show the data on the
**         output pins.
**     Parameters  :
**         NAME            - DESCRIPTION
**         val             - 8bit value to be shifted
**     Returns     : Nothing
** ===================================================================
*/

void MotorsDirection_Enable(void);
/*
** ===================================================================
**     Method      :  MotorsDirection_Enable (component 74HC595)
**     Description :
**         Enables the device
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void MotorsDirection_Disable(void);
/*
** ===================================================================
**     Method      :  MotorsDirection_Disable (component 74HC595)
**     Description :
**         Disables the device
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void MotorsDirection_WriteByte(byte val);
/*
** ===================================================================
**     Method      :  MotorsDirection_WriteByte (component 74HC595)
**     Description :
**         Shifts the given byte and latches it.
**     Parameters  :
**         NAME            - DESCRIPTION
**         val             - value to be shifted into the shift
**                           register. The value will be latched and
**                           will appear on the output pins.
**     Returns     : Nothing
** ===================================================================
*/

byte MotorsDirection_ReadByte(void);
/*
** ===================================================================
**     Method      :  MotorsDirection_ReadByte (component 74HC595)
**     Description :
**         Returns the current value of the shift register. The current
**         value of the shift register is cached in the driver.
**     Parameters  : None
**     Returns     :
**         ---             - current shift register value (cached)
** ===================================================================
*/

/* END MotorsDirection. */

#endif
/* ifndef __MotorsDirection_H */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
