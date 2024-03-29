/* ###################################################################
**     Filename    : main.c
**     Project     : Pathfinder
**     Processor   : MKL46Z256VLL4
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-10-15, 23:40, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "FRTOS1.h"
#include "KSDK1.h"
#include "UTIL1.h"
#include "TSK1.h"
#include "MMA1.h"
#include "WAIT1.h"
#include "MotorSpeed_4.h"
#include "Motors_TimerUnit.h"
#include "TU1.h"
#include "MotorSpeed_3.h"
#include "PwmLdd1.h"
#include "MotorsDirection.h"
#include "STCP1.h"
#include "BitIoLdd1.h"
#include "DS1.h"
#include "BitIoLdd2.h"
#include "SHCP1.h"
#include "BitIoLdd3.h"
#include "OE1.h"
#include "BitIoLdd4.h"
#include "MAG1.h"
#include "MINT1.h"
#include "ExtIntLdd1.h"
#include "CLS1.h"
#include "Flash.h"
#include "IntFlashLdd1.h"
#include "ESP8266.h"
#include "BT.h"
#include "ASerialLdd2.h"
#include "ASerialLdd1.h"
#include "TRIG.h"
#include "PwmLdd2.h"
#include "GI2C1.h"
#include "TMOUT1.h"
#include "CS1.h"
#include "I2C0.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "frtos_tasks.h"

/* User includes (#include below this line is not maintained by Processor Expert) */

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
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
