/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : Flash.c
**     Project     : Pathfinder
**     Processor   : MKL46Z256VLL4
**     Component   : IntFLASH
**     Version     : Component 02.409, Driver 01.02, CPU db: 3.00.000
**     Repository  : Kinetis
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-01-19, 12:56, # CodeGen: 96
**     Abstract    :
**         This component "IntFLASH" implements an access to internal FLASH.
**         The component support reading/writing data into FLASH, erasing of
**         selected sector.
**         The component supports events if the write interrupt is supported.
**         The component supports following modes of write operations:
**           - Write - writing without any test.
**           - Destructive write - sector is erased if necessary.
**           - Safe write - user event is invoked to save and resore data
**                          from the current sector.
**         The component requires on-chip FLASH memory (not used/allocated by
**         other components).
**     Settings    :
**          Component name                                 : Flash
**          FLASH                                          : FTFA
**          FLASH_LDD                                      : FLASH_LDD
**          Write method                                   : Safe write (with save & erase)
**            Buffer type                                  : Implemented by the component
**          Interrupt service/event                        : Disabled
**          Wait in RAM                                    : yes
**          Virtual page                                   : Disabled
**          Initialization                                 : 
**            Events enabled in init.                      : yes
**            Wait enabled in init.                        : yes
**          CPU clock/speed selection                      : 
**            FLASH clock                                  : 
**            High speed mode                              : This component enabled
**            Low speed mode                               : This component disabled
**            Slow speed mode                              : This component disabled
**     Contents    :
**         DisableEvent  - byte Flash_DisableEvent(void);
**         EnableEvent   - byte Flash_EnableEvent(void);
**         SetByteFlash  - byte Flash_SetByteFlash(Flash_TAddress Addr, byte Data);
**         SetWordFlash  - byte Flash_SetWordFlash(Flash_TAddress Addr, word Data);
**         SetBlockFlash - byte Flash_SetBlockFlash(Flash_TDataAddress Source, Flash_TAddress Dest, word...
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
** @file Flash.c
** @version 01.02
** @brief
**         This component "IntFLASH" implements an access to internal FLASH.
**         The component support reading/writing data into FLASH, erasing of
**         selected sector.
**         The component supports events if the write interrupt is supported.
**         The component supports following modes of write operations:
**           - Write - writing without any test.
**           - Destructive write - sector is erased if necessary.
**           - Safe write - user event is invoked to save and resore data
**                          from the current sector.
**         The component requires on-chip FLASH memory (not used/allocated by
**         other components).
*/         
/*!
**  @addtogroup Flash_module Flash module documentation
**  @{
*/         

/* MODULE Flash. */

#include "Flash.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Internal method prototypes */

#define Flash_TOTAL_FLASH_BLOCK_COUNT  0x02U
#define Flash_P_FLASH_SIZE             0x00040000U
#define Flash_P_FLASH_BLOCK_COUNT      0x02U
#define Flash_P_FLASH_BLOCK_SIZE       0x00020000U

#define Flash_CMD_NONE                 0x00U
#define Flash_CMD_READ                 0x01U
#define Flash_CMD_ERASE_SECTOR         0x02U
#define Flash_CMD_ERASE_BLOCK          0x03U
#define Flash_CMD_VERIFY_ERASED_BLOCK  0x04U
#define Flash_CMD_WRITE                0x05U
#define Flash_CMD_WRITE_ERASE          0x06U

typedef struct Flash_TSector_Struct {
  uint32_t Data[IntFlashLdd1_ERASABLE_UNIT_SIZE/4U];
} Flash_TSector;

static uint32_t Flash_CurrentCommand;  /* Current command */
static bool Flash_CmdResult;           /* Last command result */
static bool Flash_CmdPending;          /* Current command state */
static bool Flash_EnMode;              /* State component in clock configuration (enabled/disabled) */
static bool Flash_EnEvent;             /* State of events (enabled/disabled) */

static LDD_TDeviceData* IntFlashLdd1_DevDataPtr;
static LDD_TData *Flash_CurrentDataAddress;
static LDD_FLASH_TDataSize Flash_CurrentDataSize;
static LDD_FLASH_TAddress Flash_FlashAddress;
static LDD_TData *Flash_NextSrcDataAddress;
static LDD_FLASH_TDataSize Flash_RemainingDataSize;
static Flash_TSector tmpSector;

byte Flash_SetFlash(Flash_TDataAddress Source, Flash_TAddress Dest, word Count);

/*
** ===================================================================
**     Method      :  Flash_SetFlash (component IntFLASH)
**
**     Description :
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
byte Flash_SetFlash(Flash_TDataAddress Src, Flash_TAddress Dst, word Count)
{
  LDD_TError                 Result;
  uint32_t                   x;
  uint32_t                   SectorAddress, SectorOffset;

  if (!Flash_EnMode) {
    return ERR_SPEED;
  }
  if (Flash_CmdPending) {
    return ERR_BUSY;
  }
  Result = ERR_OK;                     /* Suppose area is erased */
  for (x=0U; x<Count; x++) {           /* Check if written area is erased */
    if (((uint8_t*)Dst)[x] != 0xFFU) { /* Byte erased? */
      Result = ERR_FAILED;             /* No, Erase is required */
     break;
    }
  }
  if (Result == ERR_FAILED) {
    SectorAddress = Dst & ~(LDD_FLASH_TAddress)IntFlashLdd1_ERASABLE_UNIT_MASK;
    SectorOffset = Dst & IntFlashLdd1_ERASABLE_UNIT_MASK;
    tmpSector = *(Flash_TSector *)SectorAddress; /* Create copy of the sector */
    do {                               /* Rewrite part of the sector with new data */
      ((uint8_t*)(void*)&tmpSector)[SectorOffset] = *(uint8_t*)(void*)(Src);
      Src++;
      Count--;
    } while ((++SectorOffset != IntFlashLdd1_ERASABLE_UNIT_SIZE) && (Count != 0U));
    Flash_CmdPending = TRUE;
    Flash_CurrentCommand = Flash_CMD_WRITE_ERASE;
    Flash_CurrentDataAddress = (LDD_TData *)&tmpSector;
    Flash_CurrentDataSize = IntFlashLdd1_ERASABLE_UNIT_SIZE;
    Flash_FlashAddress = SectorAddress;
    Flash_NextSrcDataAddress = (LDD_TData *)Src;
    Flash_RemainingDataSize = (LDD_FLASH_TDataSize) Count;
    Result = IntFlashLdd1_Erase(IntFlashLdd1_DevDataPtr, Flash_FlashAddress, Flash_CurrentDataSize);
    if (Result == ERR_OK) {
      do {
        IntFlashLdd1_Main(IntFlashLdd1_DevDataPtr);
      } while (Flash_CmdPending);
      Result = Flash_CmdResult;
    } else {
      Flash_CmdPending = FALSE;        /* Command parameter error */
      if (Result == ERR_PARAM_ADDRESS) {
        Result = ERR_RANGE;
      }
    }
    return (byte)Result;
  }
  Flash_RemainingDataSize = 0U;
  Flash_CurrentCommand = Flash_CMD_WRITE;
  Flash_CmdPending = TRUE;
  Result = IntFlashLdd1_Write(IntFlashLdd1_DevDataPtr, (LDD_TData *)Src, Dst, (LDD_FLASH_TDataSize)Count); /* Start reading from the flash memory */
  if (Result == ERR_OK) {
    do {
      IntFlashLdd1_Main(IntFlashLdd1_DevDataPtr);
    } while (Flash_CmdPending);
    Result = Flash_CmdResult;
  } else {
    Flash_CmdPending = FALSE;          /* Command parameter error */
    if (Result == ERR_PARAM_ADDRESS) {
      Result = ERR_RANGE;
    }
  }
  return (byte)Result;
}

/*
** ===================================================================
**     Method      :  Flash_SetClockConfiguration (component IntFLASH)
**
**     Description :
**         This method changes the clock configuration.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void Flash_SetClockConfiguration(LDD_TClockConfiguration ClockConfiguration)
{
  switch (ClockConfiguration) {
    case CPU_CLOCK_CONFIG_0:
      Flash_EnMode = TRUE;             /* Set the flag "device enabled" in the actual speed CPU mode */
      break;
    default:
      Flash_EnMode = FALSE;            /* Set the flag "device disabled" in the actual speed CPU mode */
      break;
  }
}

/*
** ===================================================================
**     Method      :  Flash_Init (component IntFLASH)
**
**     Description :
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void Flash_Init(void)
{
  IntFlashLdd1_DevDataPtr = IntFlashLdd1_Init(NULL);
  Flash_CmdPending = FALSE;
  Flash_EnEvent = TRUE;                /* Remember events state */
  Flash_EnMode = TRUE;                 /* Remember mode state */
}

/*
** ===================================================================
**     Method      :  Flash_DisableEvent (component IntFLASH)
*/
/*!
**     @brief
**         This method disables all the events except [OnSaveBuffer],
**         [OnRestoreBuffer] and [OnEraseError]. The method is
**         available only if any event is enabled.
**     @return
**                         - Error code, possible codes: 
**                           - ERR_OK - OK 
**                           - ERR_SPEED - This device does not work in
**                           the active speed mode
*/
/* ===================================================================*/
byte Flash_DisableEvent(void)
{
  if (!Flash_EnMode) {
    return ERR_SPEED;
  }
  Flash_EnEvent = FALSE;
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  Flash_EnableEvent (component IntFLASH)
*/
/*!
**     @brief
**         This method enables all the events except [OnSaveBuffer],
**         [OnRestoreBuffer] and [OnEraseError]. The method is
**         available only if any event is enabled.
**     @return
**                         - Error code, possible codes: 
**                           - ERR_OK - OK 
**                           - ERR_SPEED - This device does not work in
**                           the active speed mode
*/
/* ===================================================================*/
byte Flash_EnableEvent(void)
{
  if (!Flash_EnMode) {
    return ERR_SPEED;
  }
  Flash_EnEvent = TRUE;
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  Flash_SetByteFlash (component IntFLASH)
*/
/*!
**     @brief
**         Writes a byte to an address in FLASH. The operation of this
**         method depends on the "Write method" property and state of
**         the flash. Please see more details on general info page of
**         the help.
**     @param
**         Addr            - Address to FLASH.
**     @param
**         Data            - Data to write.
**     @return
**                         - Error code, possible codes: 
**                           - ERR_OK - OK 
**                           - ERR_NOTAVAIL - Desired program/erase
**                           operation is not available 
**                           - ERR_RANGE - Address is out of range 
**                           - ERR_VALUE - Read value is not equal to
**                           written value 
**                           - ERR_SPEED - This device does not work in
**                           the active speed mode 
**                           - ERR_BUSY - Device is busy 
**                           - ERR_PROTECT - Flash is write protect
*/
/* ===================================================================*/
byte Flash_SetByteFlash(Flash_TAddress Addr, byte Data)
{
  return Flash_SetFlash((Flash_TDataAddress)(void*)&Data, Addr,1U);
}

/*
** ===================================================================
**     Method      :  Flash_SetWordFlash (component IntFLASH)
*/
/*!
**     @brief
**         Writes a word to an address in FLASH. The operation of this
**         method depends on the "Write method" property and state of
**         the flash. Please see more details on general info page of
**         the help.
**     @param
**         Addr            - Address to FLASH.
**     @param
**         Data            - Data to write.
**     @return
**                         - Error code, possible codes: 
**                           - ERR_OK - OK 
**                           - ERR_NOTAVAIL - Desired program/erase
**                           operation is not available 
**                           - ERR_RANGE - Address is out of range 
**                           - ERR_VALUE - Read value is not equal to
**                           written value 
**                           - ERR_SPEED - This device does not work in
**                           the active speed mode 
**                           - ERR_BUSY - Device is busy 
**                           - ERR_PROTECT - Flash is write protect
*/
/* ===================================================================*/
byte Flash_SetWordFlash(Flash_TAddress Addr, word Data)
{
  return Flash_SetFlash((Flash_TDataAddress)(void*)&Data, Addr, 2U);
}

/*
** ===================================================================
**     Method      :  Flash_SetBlockFlash (component IntFLASH)
*/
/*!
**     @brief
**         Writes data to FLASH. The operation of this method depends
**         on the "Write method" property and state of the flash.
**         Please see more details on general info page of the help.
**     @param
**         Source          - Source address of the data.
**     @param
**         Dest            - Destination address in FLASH.
**     @param
**         Count           - Count of the data fields (in the
**                           smallest addressable units).
**     @return
**                         - Error code, possible codes: 
**                           - ERR_OK - OK 
**                           - ERR_NOTAVAIL - Desired program/erase
**                           operation is not available 
**                           - ERR_RANGE - The address is out of range 
**                           - ERR_BUSY - Device is busy 
**                           - ERR_SPEED - This device does not work in
**                           the active speed mode 
**                           - ERR_PROTECT - Flash is write protect 
**                           - ERR_VALUE - Read value is not equal to
**                           written value
*/
/* ===================================================================*/
byte Flash_SetBlockFlash(Flash_TDataAddress Source, Flash_TAddress Dest, word Count)
{
  return Flash_SetFlash(Source, Dest, Count);
}

/*
** ===================================================================
**     Method      :  Flash_OnOperationComplete (component IntFLASH)
**
**     Description :
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void IntFlashLdd1_OnOperationComplete(LDD_TUserData *UserDataPtr)
{
  uint32_t  x;
  
  (void)UserDataPtr;                   /* Parameter is not used, suppress unused argument warning */
  Flash_CmdResult = ERR_OK;            /* No error appears */
  switch (Flash_CurrentCommand) {
    case Flash_CMD_WRITE:
      if (Flash_RemainingDataSize != 0U) {
        Flash_CurrentDataAddress = Flash_NextSrcDataAddress;
        Flash_FlashAddress += Flash_CurrentDataSize;
        if (Flash_RemainingDataSize % IntFlashLdd1_ERASABLE_UNIT_SIZE) {
          /* Remaining data size is NOT multiple of sector size */
          if (Flash_RemainingDataSize < IntFlashLdd1_ERASABLE_UNIT_SIZE) { /* Last sector? */
            for (x=0U; x<IntFlashLdd1_ERASABLE_UNIT_SIZE;x++) {
              if (x<Flash_RemainingDataSize) {
                ((uint8_t*)(void*)&tmpSector)[x] = ((uint8_t*)(void*)Flash_NextSrcDataAddress)[x];
              } else {
                ((uint8_t*)(void*)&tmpSector)[x] = ((uint8_t*)(void*)Flash_FlashAddress)[x];
              }
            }
            Flash_CurrentDataAddress = &tmpSector;
            Flash_CurrentDataSize = IntFlashLdd1_ERASABLE_UNIT_SIZE;
            Flash_RemainingDataSize = 0U;
          } else {
            Flash_CurrentDataSize = (Flash_RemainingDataSize / IntFlashLdd1_ERASABLE_UNIT_SIZE)*IntFlashLdd1_ERASABLE_UNIT_SIZE;
            Flash_RemainingDataSize = Flash_RemainingDataSize % IntFlashLdd1_ERASABLE_UNIT_SIZE;
            Flash_NextSrcDataAddress = (Flash_TDataAddress)((uint32_t)Flash_NextSrcDataAddress + Flash_CurrentDataSize);
          }
        } else {
          /* Remaining data size is multiple of sector size */
          Flash_CurrentDataSize = Flash_RemainingDataSize;
          Flash_RemainingDataSize = 0U;
        }
        Flash_CurrentCommand = Flash_CMD_WRITE_ERASE;
        Flash_CmdResult = (byte)IntFlashLdd1_Erase(IntFlashLdd1_DevDataPtr, Flash_FlashAddress, Flash_CurrentDataSize);
        if (Flash_CmdResult != ERR_OK) { /* Erase command error? */
          Flash_CmdPending = FALSE;    /* Command is finished */
        }
        return;
      } else {
        Flash_CmdPending = FALSE;      /* Command is finished */
      }
      break;
    case Flash_CMD_WRITE_ERASE:
      Flash_CurrentCommand = Flash_CMD_WRITE;
      Flash_CmdResult = (byte)IntFlashLdd1_Write(IntFlashLdd1_DevDataPtr, Flash_CurrentDataAddress, Flash_FlashAddress, Flash_CurrentDataSize); /* Write data */
      if (Flash_CmdResult != ERR_OK) { /* Write command error? */
        Flash_CmdPending = FALSE;      /* Command is finished */
      }
      return;
    default:
      Flash_CmdPending = FALSE;        /* Command is finished */
      return;
  }
}

/*
** ===================================================================
**     Method      :  Flash_OnError (component IntFLASH)
**
**     Description :
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void IntFlashLdd1_OnError(LDD_TUserData *UserDataPtr)
{
  LDD_FLASH_TErrorStatus FLASH_LDD_ErrorStatus;

  (void)UserDataPtr;                   /* Parameter is not used, suppress unused argument warning */
  Flash_CmdPending = FALSE;
  Flash_CurrentCommand = Flash_CMD_NONE;
  IntFlashLdd1_GetError(IntFlashLdd1_DevDataPtr, &FLASH_LDD_ErrorStatus);
  if (FLASH_LDD_ErrorStatus.CurrentErrorFlags & LDD_FLASH_PROTECTION_VIOLATION) {
    Flash_CmdResult = ERR_PROTECT;     /* Protection violation */
  } else {
    Flash_CmdResult = ERR_VALUE;       /* Access error or Read collision error */
  }
}

/* END Flash. */

#ifdef __cplusplus
}  /* extern "C" */
#endif

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
