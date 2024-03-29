/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : TU1.h
**     Project     : Pathfinder
**     Processor   : MKL46Z256VLL4
**     Component   : TimerUnit_LDD
**     Version     : Component 01.164, Driver 01.11, CPU db: 3.00.000
**     Repository  : Kinetis
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-12-10, 11:34, # CodeGen: 57
**     Abstract    :
**          This TimerUnit component provides a low level API for unified hardware access across
**          various timer devices using the Prescaler-Counter-Compare-Capture timer structure.
**     Settings    :
**          Component name                                 : TU1
**          Module name                                    : TPM2
**          Counter                                        : TPM2_CNT
**          Counter direction                              : Up
**          Counter width                                  : 16 bits
**          Value type                                     : Optimal
**          Input clock source                             : Internal
**            Counter frequency                            : 2.62144 MHz
**          Counter restart                                : On-overrun
**            Overrun period                               : 25 ms
**            Interrupt                                    : Enabled
**              Interrupt                                  : INT_TPM2
**              Interrupt priority                         : medium priority
**          Channel list                                   : 1
**            Channel 0                                    : 
**              Mode                                       : Capture
**                Capture                                  : TPM2_C0V
**                Capture input pin                        : ADC0_DP3/ADC0_SE3/PTE22/TPM2_CH0/UART2_TX
**                Capture input signal                     : US_Echo_D2
**                Edge                                     : both edges
**                Maximum time of event                    : 25.00001792 ms
**                Interrupt                                : Enabled
**                  Interrupt                              : INT_TPM2
**                  Interrupt priority                     : medium priority
**          Initialization                                 : 
**            Enabled in init. code                        : yes
**            Auto initialization                          : no
**            Event mask                                   : 
**              OnCounterRestart                           : Enabled
**              OnChannel0                                 : Enabled
**              OnChannel1                                 : Disabled
**              OnChannel2                                 : Disabled
**              OnChannel3                                 : Disabled
**              OnChannel4                                 : Disabled
**              OnChannel5                                 : Disabled
**              OnChannel6                                 : Disabled
**              OnChannel7                                 : Disabled
**          CPU clock/configuration selection              : 
**            Clock configuration 0                        : This component enabled
**            Clock configuration 1                        : This component disabled
**            Clock configuration 2                        : This component disabled
**            Clock configuration 3                        : This component disabled
**            Clock configuration 4                        : This component disabled
**            Clock configuration 5                        : This component disabled
**            Clock configuration 6                        : This component disabled
**            Clock configuration 7                        : This component disabled
**     Contents    :
**         Init            - LDD_TDeviceData* TU1_Init(LDD_TUserData *UserDataPtr);
**         Deinit          - void TU1_Deinit(LDD_TDeviceData *DeviceDataPtr);
**         ResetCounter    - LDD_TError TU1_ResetCounter(LDD_TDeviceData *DeviceDataPtr);
**         GetCaptureValue - LDD_TError TU1_GetCaptureValue(LDD_TDeviceData *DeviceDataPtr, uint8_t...
**
**     Copyright : 1997 - 2015 Freescale Semiconductor, Inc. 
**     All Rights Reserved.
**     
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**     
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**     
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**     
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**     
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**     
**     http: www.freescale.com
**     mail: support@freescale.com
** ###################################################################*/
/*!
** @file TU1.h
** @version 01.11
** @brief
**          This TimerUnit component provides a low level API for unified hardware access across
**          various timer devices using the Prescaler-Counter-Compare-Capture timer structure.
*/         
/*!
**  @addtogroup TU1_module TU1 module documentation
**  @{
*/         

#ifndef __TU1_H
#define __TU1_H

/* MODULE TU1. */

/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* Include inherited beans */

#include "TPM_PDD.h"
#include "Cpu.h"

#ifdef __cplusplus
extern "C" {
#endif 


#ifndef __BWUserType_TU1_TValueType
#define __BWUserType_TU1_TValueType
  typedef uint32_t TU1_TValueType ;    /* Type for data parameters of methods */
#endif
#define TU1_CNT_INP_FREQ_U_0 0x00280000UL /* Counter input frequency in Hz */
#define TU1_CNT_INP_FREQ_U_0_CFG_0 0x00280000UL /* Counter input frequency in Hz for Clock configuration 0 */
#define TU1_CNT_INP_FREQ_R_0 2621438.120953155F /* Counter input frequency in Hz */
#define TU1_CNT_INP_FREQ_R_0_CFG_0 2621438.120953155F /* Counter input frequency in Hz for Clock configuration 0 */
#define TU1_CNT_INP_FREQ_COUNT 0U      /* Count of predefined counter input frequencies */
#define TU1_PERIOD_TICKS   0x00010000UL /* Initialization value of period in 'counter ticks' */
#define TU1_NUMBER_OF_CHANNELS 0x01U   /* Count of predefined channels */
#define TU1_COUNTER_WIDTH  0x10U       /* Counter width in bits  */
#define TU1_COUNTER_DIR    DIR_UP      /* Direction of counting */
/*! Peripheral base address of a device allocated by the component. This constant can be used directly in PDD macros. */
#define TU1_PRPH_BASE_ADDRESS  0x4003A000U
  
/* Methods configuration constants - generated for all enabled component's methods */
#define TU1_Init_METHOD_ENABLED        /*!< Init method of the component TU1 is enabled (generated) */
#define TU1_Deinit_METHOD_ENABLED      /*!< Deinit method of the component TU1 is enabled (generated) */
#define TU1_ResetCounter_METHOD_ENABLED /*!< ResetCounter method of the component TU1 is enabled (generated) */
#define TU1_GetCaptureValue_METHOD_ENABLED /*!< GetCaptureValue method of the component TU1 is enabled (generated) */

/* Events configuration constants - generated for all enabled component's events */
#define TU1_OnCounterRestart_EVENT_ENABLED /*!< OnCounterRestart event of the component TU1 is enabled (generated) */
#define TU1_OnChannel0_EVENT_ENABLED   /*!< OnChannel0 event of the component TU1 is enabled (generated) */



/*
** ===================================================================
**     Method      :  TU1_Init (component TimerUnit_LDD)
*/
/*!
**     @brief
**         Initializes the device. Allocates memory for the device data
**         structure, allocates interrupt vectors and sets interrupt
**         priority, sets pin routing, sets timing, etc. If the
**         property ["Enable in init. code"] is set to "yes" value then
**         the device is also enabled (see the description of the
**         [Enable] method). In this case the [Enable] method is not
**         necessary and needn't to be generated. This method can be
**         called only once. Before the second call of Init the [Deinit]
**         must be called first.
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. This pointer will be
**                           passed as an event or callback parameter.
**     @return
**                         - Pointer to the dynamically allocated private
**                           structure or NULL if there was an error.
*/
/* ===================================================================*/
LDD_TDeviceData* TU1_Init(LDD_TUserData *UserDataPtr);

/*
** ===================================================================
**     Method      :  TU1_Deinit (component TimerUnit_LDD)
*/
/*!
**     @brief
**         Deinitializes the device. Switches off the device, frees the
**         device data structure memory, interrupts vectors, etc.
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by Init method
*/
/* ===================================================================*/
void TU1_Deinit(LDD_TDeviceData *DeviceDataPtr);

/*
** ===================================================================
**     Method      :  TU1_ResetCounter (component TimerUnit_LDD)
*/
/*!
**     @brief
**         Resets counter. If counter is counting up then it is set to
**         zero. If counter is counting down then counter is updated to
**         the reload value.
**         The method is not available if HW doesn't allow resetting of
**         the counter.
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by [Init] method.
**     @return
**                         - Error code, possible codes:
**                           ERR_OK - OK 
**                           ERR_SPEED - The component does not work in
**                           the active clock configuration
*/
/* ===================================================================*/
LDD_TError TU1_ResetCounter(LDD_TDeviceData *DeviceDataPtr);

/*
** ===================================================================
**     Method      :  TU1_GetCaptureValue (component TimerUnit_LDD)
*/
/*!
**     @brief
**         Returns the content of capture register specified by the
**         parameter ChannelIdx. This method is available when at least
**         one channel is configured.
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by [Init] method.
**     @param
**         ChannelIdx      - Index of the component
**                           channel.
**     @param
**         ValuePtr        - Pointer to return value of the
**                           capture register.
**     @return
**                         - Error code, possible codes:
**                           ERR_OK - OK 
**                           ERR_PARAM_INDEX - ChannelIdx parameter is
**                           out of possible range
**                           ERR_NOTAVAIL -  The capture mode is not
**                           selected for selected channel.
**                           ERR_SPEED - The component does not work in
**                           the active clock configuration
*/
/* ===================================================================*/
LDD_TError TU1_GetCaptureValue(LDD_TDeviceData *DeviceDataPtr, uint8_t ChannelIdx, TU1_TValueType *ValuePtr);

/*
** ===================================================================
**     Method      :  TU1_SetClockConfiguration (component TimerUnit_LDD)
**
**     Description :
**         This method changes the clock configuration. During a clock 
**         configuration change the component changes it's setting 
**         immediately upon a request.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void TU1_SetClockConfiguration(LDD_TDeviceData *DeviceDataPtr, LDD_TClockConfiguration ClockConfiguration);

/*
** ===================================================================
**     Method      :  TU1_Interrupt (component TimerUnit_LDD)
**
**     Description :
**         The method services the interrupt of the selected peripheral(s)
**         and eventually invokes event(s) of the component.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
/* {FreeRTOS RTOS Adapter} ISR function prototype */
PE_ISR(TU1_Interrupt);

/* END TU1. */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

#endif
/* ifndef __TU1_H */
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
