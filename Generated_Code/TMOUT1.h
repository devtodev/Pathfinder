/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : TMOUT1.h
**     Project     : Pathfinder
**     Processor   : MKL46Z256VLL4
**     Component   : Timeout
**     Version     : Component 01.030, Driver 01.00, CPU db: 3.00.000
**     Repository  : My Components
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-12-13, 15:51, # CodeGen: 68
**     Abstract    :
**
The module implements timeout functionality. With this implementation,
it is possible to wait for a given time, and the time is counted by
a periodic interrupt.
**     Settings    :
**          Component name                                 : TMOUT1
**          Critical Section                               : CS1
**          Maximum counters                               : 1
**          Counter tick period (ms)                       : 10
**          RTOS                                           : Disabled
**     Contents    :
**         GetCounter     - TMOUT1_CounterHandle TMOUT1_GetCounter(TMOUT1_CounterType nofTicks);
**         LeaveCounter   - void TMOUT1_LeaveCounter(TMOUT1_CounterHandle handle);
**         Value          - TMOUT1_CounterType TMOUT1_Value(TMOUT1_CounterHandle handle);
**         SetCounter     - TMOUT1_CounterType TMOUT1_SetCounter(TMOUT1_CounterHandle handle,...
**         CounterExpired - bool TMOUT1_CounterExpired(TMOUT1_CounterHandle handle);
**         AddTick        - void TMOUT1_AddTick(void);
**         Init           - void TMOUT1_Init(void);
**
**     License   :  Open Source (LGPL)
**     Copyright : (c) Copyright Erich Styger, 2011-2015, all rights reserved.
**     This an open source software implementing timeout routines using Processor Expert.
**     This is a free software and is opened for education,  research  and commercial developments under license policy of following terms:
**     * This is a free software and there is NO WARRANTY.
**     * No restriction on use. You can use, modify and redistribute it for   personal, non-profit or commercial product UNDER YOUR RESPONSIBILITY.
**     * Redistributions of source code must retain the above copyright notice.
** ###################################################################*/
/*!
** @file TMOUT1.h
** @version 01.00
** @brief
**
The module implements timeout functionality. With this implementation,
it is possible to wait for a given time, and the time is counted by
a periodic interrupt.
*/         
/*!
**  @addtogroup TMOUT1_module TMOUT1 module documentation
**  @{
*/         

#ifndef __TMOUT1_H
#define __TMOUT1_H

/* MODULE TMOUT1. */

/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* Include inherited beans */
#include "CS1.h"

#include "Cpu.h"


#ifndef __BWUserType_TMOUT1_CounterType
#define __BWUserType_TMOUT1_CounterType
  typedef int16_t TMOUT1_CounterType ; /* Base type of counter which holds the counting value */
#endif
#ifndef __BWUserType_TMOUT1_CounterHandle
#define __BWUserType_TMOUT1_CounterHandle
  typedef byte TMOUT1_CounterHandle ;  /* Type for the timeout counter handle. */
#endif

#define TMOUT1_OUT_OF_HANDLE 0xff  /* special handle to indicate that we were running out of handlers */
#define TMOUT1_TICK_PERIOD_MS  \
  10                                    /* Tick period of timeout counter in milliseconds, as specified in component properties */


TMOUT1_CounterHandle TMOUT1_GetCounter(TMOUT1_CounterType nofTicks);
/*
** ===================================================================
**     Method      :  TMOUT1_GetCounter (component Timeout)
**     Description :
**         Initializes a new timeout counter and returns the handle to
**         it. At the end, use LeaveCounter() to free up the resource.
**     Parameters  :
**         NAME            - DESCRIPTION
**         nofTicks        - Number of ticks for the counter
**                           until it expires.
**     Returns     :
**         ---             - Handle to the counter, to be used for
**                           further API calls.
** ===================================================================
*/

void TMOUT1_LeaveCounter(TMOUT1_CounterHandle handle);
/*
** ===================================================================
**     Method      :  TMOUT1_LeaveCounter (component Timeout)
**     Description :
**         To be called to return the counter. Note that a counter
**         always should be returned so it can be reused.
**     Parameters  :
**         NAME            - DESCRIPTION
**         handle          - Counter handle
**     Returns     : Nothing
** ===================================================================
*/

bool TMOUT1_CounterExpired(TMOUT1_CounterHandle handle);
/*
** ===================================================================
**     Method      :  TMOUT1_CounterExpired (component Timeout)
**     Description :
**         Returns true if the timeout counter has been expired
**     Parameters  :
**         NAME            - DESCRIPTION
**         handle          - The timeout handle retrieved using
**                           GetCounter()
**     Returns     :
**         ---             - Returns TRUE if the counter has been
**                           expired, FALSE otherwise
** ===================================================================
*/

void TMOUT1_AddTick(void);
/*
** ===================================================================
**     Method      :  TMOUT1_AddTick (component Timeout)
**     Description :
**         Method to be called from a periodic timer or interrupt. It
**         will decrement all current counters by one down to zero.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void TMOUT1_Init(void);
/*
** ===================================================================
**     Method      :  TMOUT1_Init (component Timeout)
**     Description :
**         Initialization of the driver
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

TMOUT1_CounterType TMOUT1_Value(TMOUT1_CounterHandle handle);
/*
** ===================================================================
**     Method      :  TMOUT1_Value (component Timeout)
**     Description :
**         Return the current value of the counter (in ticks)
**     Parameters  :
**         NAME            - DESCRIPTION
**         handle          - Handle of the timeout counter
**     Returns     :
**         ---             - Returns the value of the timeout counter.
** ===================================================================
*/

TMOUT1_CounterType TMOUT1_SetCounter(TMOUT1_CounterHandle handle, TMOUT1_CounterType nofTicks);
/*
** ===================================================================
**     Method      :  TMOUT1_SetCounter (component Timeout)
**     Description :
**         Sets the counter to a new value and returns the value just
**         prior to the call.
**     Parameters  :
**         NAME            - DESCRIPTION
**         handle          - Counter handle which shall get a new
**                           value.
**         nofTicks        - New value (tick count) of the
**                           timeout counter. Pass zero to have it
**                           expire immediately.
**     Returns     :
**         ---             - Value of counter before reset.
** ===================================================================
*/

/* END TMOUT1. */

#endif
/* ifndef __TMOUT1_H */
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
