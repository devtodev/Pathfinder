/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : BT.c
**     Project     : Pathfinder
**     Processor   : MKL46Z256VLL4
**     Component   : AsynchroSerial
**     Version     : Component 02.611, Driver 01.01, CPU db: 3.00.000
**     Repository  : Kinetis
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-01-19, 12:56, # CodeGen: 96
**     Abstract    :
**         This component "AsynchroSerial" implements an asynchronous serial
**         communication. The component supports different settings of
**         parity, word width, stop-bit and communication speed,
**         user can select interrupt or polling handler.
**         Communication speed can be changed also in runtime.
**         The component requires one on-chip asynchronous serial channel.
**     Settings    :
**          Component name                                 : BT
**          Channel                                        : UART1
**          Interrupt service/event                        : Enabled
**            Interrupt RxD                                : INT_UART1
**            Interrupt RxD priority                       : medium priority
**            Interrupt TxD                                : INT_UART1
**            Interrupt TxD priority                       : medium priority
**            Interrupt Error                              : INT_UART1
**            Interrupt Error priority                     : medium priority
**            Input buffer size                            : 1024
**            Output buffer size                           : 1024
**            Handshake                                    : 
**              CTS                                        : Disabled
**              RTS                                        : Disabled
**          Settings                                       : 
**            Parity                                       : none
**            Width                                        : 8 bits
**            Stop bit                                     : 1
**            Receiver                                     : Enabled
**              RxD                                        : LCD_P49/PTE1/SPI1_MOSI/UART1_RX/SPI1_MISO/I2C1_SCL
**              RxD pin signal                             : 
**            Transmitter                                  : Enabled
**              TxD                                        : LCD_P48/PTE0/SPI1_MISO/UART1_TX/RTC_CLKOUT/CMP0_OUT/I2C1_SDA
**              TxD pin signal                             : 
**            Baud rate                                    : 9600 baud
**            Break signal                                 : Disabled
**            Wakeup condition                             : Idle line wakeup
**            Transmitter output                           : Not inverted
**            Receiver input                               : Not inverted
**            Stop in wait mode                            : no
**            Idle line mode                               : starts after start bit
**            Break generation length                      : Short
**          Initialization                                 : 
**            Enabled in init. code                        : yes
**            Events enabled in init.                      : yes
**          CPU clock/speed selection                      : 
**            High speed mode                              : This component enabled
**            Low speed mode                               : This component disabled
**            Slow speed mode                              : This component disabled
**          Referenced components                          : 
**            Serial_LDD                                   : Serial_LDD
**     Contents    :
**         RecvChar        - byte BT_RecvChar(BT_TComData *Chr);
**         SendChar        - byte BT_SendChar(BT_TComData Chr);
**         RecvBlock       - byte BT_RecvBlock(BT_TComData *Ptr, word Size, word *Rcv);
**         SendBlock       - byte BT_SendBlock(BT_TComData *Ptr, word Size, word *Snd);
**         ClearRxBuf      - byte BT_ClearRxBuf(void);
**         ClearTxBuf      - byte BT_ClearTxBuf(void);
**         GetCharsInRxBuf - word BT_GetCharsInRxBuf(void);
**         GetCharsInTxBuf - word BT_GetCharsInTxBuf(void);
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
** @file BT.c
** @version 01.01
** @brief
**         This component "AsynchroSerial" implements an asynchronous serial
**         communication. The component supports different settings of
**         parity, word width, stop-bit and communication speed,
**         user can select interrupt or polling handler.
**         Communication speed can be changed also in runtime.
**         The component requires one on-chip asynchronous serial channel.
*/         
/*!
**  @addtogroup BT_module BT module documentation
**  @{
*/         

/* MODULE BT. */

#include "BT.h"
#include "Events.h"

#ifdef __cplusplus
extern "C" {
#endif 


#define OVERRUN_ERR      0x01U         /* Overrun error flag bit    */
#define FRAMING_ERR      0x02U         /* Framing error flag bit    */
#define PARITY_ERR       0x04U         /* Parity error flag bit     */
#define CHAR_IN_RX       0x08U         /* Char is in RX buffer      */
#define FULL_TX          0x10U         /* Full transmit buffer      */
#define RUNINT_FROM_TX   0x20U         /* Interrupt is in progress  */
#define FULL_RX          0x40U         /* Full receive buffer       */
#define NOISE_ERR        0x80U         /* Noise error flag bit      */
#define IDLE_ERR         0x0100U       /* Idle character flag bit   */
#define BREAK_ERR        0x0200U       /* Break detect              */
#define COMMON_ERR       0x0800U       /* Common error of RX       */

LDD_TDeviceData *ASerialLdd2_DeviceDataPtr; /* Device data pointer */
static bool EnMode;                    /* Enable/Disable SCI in speed mode */
static word SerFlag;                   /* Flags for serial communication */
                                       /* Bits: 0 - OverRun error */
                                       /*       1 - Framing error */
                                       /*       2 - Parity error */
                                       /*       3 - Char in RX buffer */
                                       /*       4 - Full TX buffer */
                                       /*       5 - Running int from TX */
                                       /*       6 - Full RX buffer */
                                       /*       7 - Noise error */
                                       /*       8 - Idle character  */
                                       /*       9 - Break detected  */
                                       /*      10 - Unused */
                                       /*      11 - Unused */
static word BT_InpLen;                 /* Length of input buffer's content */
static word InpIndexR;                 /* Index for reading from input buffer */
static word InpIndexW;                 /* Index for writing to input buffer */
static BT_TComData InpBuffer[BT_INP_BUF_SIZE]; /* Input buffer for SCI communication */
static BT_TComData BufferRead;         /* Input char for SCI communication */
static word BT_OutLen;                 /* Length of output bufer's content */
static word OutIndexR;                 /* Index for reading from output buffer */
static word OutIndexW;                 /* Index for writing to output buffer */
static BT_TComData OutBuffer[BT_OUT_BUF_SIZE]; /* Output buffer for SCI communication */
static bool OnFreeTxBufSemaphore;      /* Disable the false calling of the OnFreeTxBuf event */

/*
** ===================================================================
**     Method      :  HWEnDi (component AsynchroSerial)
**
**     Description :
**         Enables or disables the peripheral(s) associated with the bean.
**         The method is called automatically as a part of the Enable and 
**         Disable methods and several internal methods.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void HWEnDi(void)
{
  if (EnMode) {                        /* Enable device? */
    (void)ASerialLdd2_Enable(ASerialLdd2_DeviceDataPtr); /* Enable device */
    (void)ASerialLdd2_ReceiveBlock(ASerialLdd2_DeviceDataPtr, &BufferRead, 1U); /* Receive one data byte */
    if ((BT_OutLen) != 0U) {           /* Is number of bytes in the transmit buffer greater then 0? */
      SerFlag |= RUNINT_FROM_TX;       /* Set flag "running int from TX"? */
      (void)ASerialLdd2_SendBlock(ASerialLdd2_DeviceDataPtr, (LDD_TData *)&OutBuffer[OutIndexR], 1U); /* Send one data byte */
    }
  } else {
    SerFlag &= (byte)~(RUNINT_FROM_TX); /* Clear RUNINT_FROM_TX flag */
    (void)ASerialLdd2_Disable(ASerialLdd2_DeviceDataPtr); /* Disable device */
  }
}

/*
** ===================================================================
**     Method      :  BT_RecvChar (component AsynchroSerial)
**     Description :
**         If any data is received, this method returns one character,
**         otherwise it returns an error code (it does not wait for
**         data). This method is enabled only if the receiver property
**         is enabled.
**         [Note:] Because the preferred method to handle error and
**         break exception in the interrupt mode is to use events
**         <OnError> and <OnBreak> the return value ERR_RXEMPTY has
**         higher priority than other error codes. As a consequence the
**         information about an exception in interrupt mode is returned
**         only if there is a valid character ready to be read.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Chr             - Pointer to a received character
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_RXEMPTY - No data in receiver
**                           ERR_BREAK - Break character is detected
**                           (only when the <Interrupt service> property
**                           is disabled and the <Break signal> property
**                           is enabled)
**                           ERR_COMMON - common error occurred (the
**                           <GetError> method can be used for error
**                           specification)
** ===================================================================
*/
byte BT_RecvChar(BT_TComData *Chr)
{
  byte Result = ERR_OK;                /* Return error code */

  if (!EnMode) {                       /* Is the device disabled in the actual speed CPU mode? */
    return ERR_SPEED;                  /* If yes then error */
  }
  if (BT_InpLen > 0x00U) {             /* Is number of received chars greater than 0? */
    EnterCritical();                   /* Disable global interrupts */
    BT_InpLen--;                       /* Decrease number of received chars */
    *Chr = InpBuffer[InpIndexR++];     /* Received char */
    if (InpIndexR >= BT_INP_BUF_SIZE) { /* Is the index out of the receive buffer? */
      InpIndexR = 0x00U;               /* Set index to the first item into the receive buffer */
    }
    Result = (byte)((SerFlag & (OVERRUN_ERR|COMMON_ERR|FULL_RX))? ERR_COMMON : ERR_OK);
    SerFlag &= (word)~(word)(OVERRUN_ERR|COMMON_ERR|FULL_RX|CHAR_IN_RX); /* Clear all errors in the status variable */
    ExitCritical();                    /* Enable global interrupts */
  } else {
    return ERR_RXEMPTY;                /* Receiver is empty */
  }
  return Result;                       /* Return error code */
}

/*
** ===================================================================
**     Method      :  BT_SendChar (component AsynchroSerial)
**     Description :
**         Sends one character to the channel. If the component is
**         temporarily disabled (Disable method) SendChar method only
**         stores data into an output buffer. In case of a zero output
**         buffer size, only one character can be stored. Enabling the
**         component (Enable method) starts the transmission of the
**         stored data. This method is available only if the
**         transmitter property is enabled.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Chr             - Character to send
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_TXFULL - Transmitter is full
** ===================================================================
*/
byte BT_SendChar(BT_TComData Chr)
{
  if (!EnMode) {                       /* Is the device disabled in the actual speed CPU mode? */
    return ERR_SPEED;                  /* If yes then error */
  }
  if (BT_OutLen == BT_OUT_BUF_SIZE) {  /* Is number of chars in buffer is the same as a size of the transmit buffer */
    return ERR_TXFULL;                 /* If yes then error */
  }
  EnterCritical();                     /* Disable global interrupts */
  BT_OutLen++;                         /* Increase number of bytes in the transmit buffer */
  OutBuffer[OutIndexW++] = Chr;        /* Store char to buffer */
  if (OutIndexW >= BT_OUT_BUF_SIZE) {  /* Is the pointer out of the transmit buffer */
    OutIndexW = 0x00U;                 /* Set index to first item in the transmit buffer */
  }
  if ((SerFlag & RUNINT_FROM_TX) == 0U) {
    SerFlag |= RUNINT_FROM_TX;         /* Set flag "running int from TX"? */
    (void)ASerialLdd2_SendBlock(ASerialLdd2_DeviceDataPtr, (LDD_TData *)&OutBuffer[OutIndexR], 1U); /* Send one data byte */
  }
  ExitCritical();                      /* Enable global interrupts */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  BT_RecvBlock (component AsynchroSerial)
**     Description :
**         If any data is received, this method returns the block of
**         the data and its length (and incidental error), otherwise it
**         returns an error code (it does not wait for data).
**         This method is available only if non-zero length of the
**         input buffer is defined and the receiver property is enabled.
**         If less than requested number of characters is received only
**         the available data is copied from the receive buffer to the
**         user specified destination. The value ERR_EXEMPTY is
**         returned and the value of variable pointed by the Rcv
**         parameter is set to the number of received characters.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Ptr             - Pointer to the block of received data
**         Size            - Size of the block
**       * Rcv             - Pointer to real number of the received data
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_RXEMPTY - The receive buffer didn't
**                           contain the requested number of data. Only
**                           available data has been returned.
**                           ERR_COMMON - common error occurred (the
**                           GetError method can be used for error
**                           specification)
** ===================================================================
*/
byte BT_RecvBlock(BT_TComData *Ptr, word Size, word *Rcv)
{
  register word count;                 /* Number of received chars */
  register byte result = ERR_OK;       /* Last error */

  if (!EnMode) {                       /* Is the device disabled in the actual speed CPU mode? */
    return ERR_SPEED;                  /* If yes then error */
  }
  for (count = 0x00U; count < Size; count++) {
    switch (BT_RecvChar(Ptr++)) {      /* Receive data and test the return value*/
    case ERR_RXEMPTY:                  /* No data in the buffer */
      if (result == ERR_OK) {          /* If no receiver error reported */
        result = ERR_RXEMPTY;          /* Return info that requested number of data is not available */
      }
     *Rcv = count;                     /* Return number of received chars */
      return result;
    case ERR_COMMON:                   /* Receiver error reported */
      result = ERR_COMMON;             /* Return info that an error was detected */
      break;
    default:
      break;
    }
  }
  *Rcv = count;                        /* Return number of received chars */
  return result;                       /* OK */
}

/*
** ===================================================================
**     Method      :  BT_SendBlock (component AsynchroSerial)
**     Description :
**         Sends a block of characters to the channel.
**         This method is available only if non-zero length of the
**         output buffer is defined and the transmitter property is
**         enabled.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Ptr             - Pointer to the block of data to send
**         Size            - Size of the block
**       * Snd             - Pointer to number of data that are sent
**                           (moved to buffer)
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_TXFULL - It was not possible to send
**                           requested number of bytes
** ===================================================================
*/
byte BT_SendBlock(BT_TComData *Ptr, word Size, word *Snd)
{
  word count = 0x00U;                  /* Number of sent chars */
  BT_TComData *TmpPtr = Ptr;           /* Temporary output buffer pointer */
  bool tmpOnFreeTxBufSemaphore = OnFreeTxBufSemaphore; /* Local copy of OnFreeTxBufSemaphore state */

  if (!EnMode) {                       /* Is the device disabled in the actual speed CPU mode? */
    return ERR_SPEED;                  /* If yes then error */
  }
  while ((count < Size) && (BT_OutLen < BT_OUT_BUF_SIZE)) { /* While there is some char desired to send left and output buffer is not full do */
    EnterCritical();                   /* Enter the critical section */
    OnFreeTxBufSemaphore = TRUE;       /* Set the OnFreeTxBufSemaphore to block OnFreeTxBuf calling */
    BT_OutLen++;                       /* Increase number of bytes in the transmit buffer */
    OutBuffer[OutIndexW++] = *TmpPtr++; /* Store char to buffer */
    if (OutIndexW >= BT_OUT_BUF_SIZE) { /* Is the index out of the transmit buffer? */
      OutIndexW = 0x00U;               /* Set index to the first item in the transmit buffer */
    }
    count++;                           /* Increase the count of sent data */
    if ((count == Size) || (BT_OutLen == BT_OUT_BUF_SIZE)) { /* Is the last desired char put into buffer or the buffer is full? */
      if (!tmpOnFreeTxBufSemaphore) {  /* Was the OnFreeTxBufSemaphore clear before enter the method? */
        OnFreeTxBufSemaphore = FALSE;  /* If yes then clear the OnFreeTxBufSemaphore */
      }
    }
    if ((SerFlag & RUNINT_FROM_TX) == 0U) {
      SerFlag |= RUNINT_FROM_TX;       /* Set flag "running int from TX"? */
      (void)ASerialLdd2_SendBlock(ASerialLdd2_DeviceDataPtr, (LDD_TData *)&OutBuffer[OutIndexR], 1U); /* Send one data byte */
    }
    ExitCritical();                    /* Exit the critical section */
  }
  *Snd = count;                        /* Return number of sent chars */
  if (count != Size) {                 /* Is the number of sent chars less then desired number of chars */
    return ERR_TXFULL;                 /* If yes then error */
  }
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  BT_ClearRxBuf (component AsynchroSerial)
**     Description :
**         Clears the receive buffer.
**         This method is available only if non-zero length of the
**         input buffer is defined and the receiver property is enabled.
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
byte BT_ClearRxBuf(void)
{
  if (!EnMode) {                       /* Is the device disabled in the actual speed CPU mode? */
    return ERR_SPEED;                  /* If yes then error */
  }
  EnterCritical();                     /* Disable global interrupts */
  BT_InpLen = 0x00U;                   /* Set number of chars in the transmit buffer to 0 */
  InpIndexW = 0x00U;                   /* Set index on the first item in the transmit buffer */
  InpIndexR = 0x00U;
  ExitCritical();                      /* Enable global interrupts */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  BT_ClearTxBuf (component AsynchroSerial)
**     Description :
**         Clears the transmit buffer.
**         This method is available only if non-zero length of the
**         output buffer is defined and the receiver property is
**         enabled.
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
byte BT_ClearTxBuf(void)
{
  if (!EnMode) {                       /* Is the device disabled in the actual speed CPU mode? */
    return ERR_SPEED;                  /* If yes then error */
  }
  EnterCritical();                     /* Disable global interrupts */
  BT_OutLen = 0x00U;                   /* Set number of chars in the receive buffer to 0 */
  OutIndexW = 0x00U;                   /* Set index on the first item in the receive buffer */
  OutIndexR = 0x00U;
  ExitCritical();                      /* Enable global interrupts */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  BT_GetCharsInRxBuf (component AsynchroSerial)
**     Description :
**         Returns the number of characters in the input buffer. This
**         method is available only if the receiver property is enabled.
**     Parameters  : None
**     Returns     :
**         ---             - The number of characters in the input
**                           buffer.
** ===================================================================
*/
word BT_GetCharsInRxBuf(void)
{
  return BT_InpLen;                    /* Return number of chars in receive buffer */
}

/*
** ===================================================================
**     Method      :  BT_GetCharsInTxBuf (component AsynchroSerial)
**     Description :
**         Returns the number of characters in the output buffer. This
**         method is available only if the transmitter property is
**         enabled.
**     Parameters  : None
**     Returns     :
**         ---             - The number of characters in the output
**                           buffer.
** ===================================================================
*/
word BT_GetCharsInTxBuf(void)
{
  return BT_OutLen;                    /* Return number of chars in the transmitter buffer */
}

/*
** ===================================================================
**     Method      :  BT_Init (component AsynchroSerial)
**
**     Description :
**         Initializes the associated peripheral(s) and the bean internal 
**         variables. The method is called automatically as a part of the 
**         application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void BT_Init(void)
{
  SerFlag = 0x00U;                     /* Reset flags */
  BT_InpLen = 0x00U;                   /* No char in the receive buffer */
  InpIndexR = 0x00U;                   /* Set index on the first item in the receive buffer */
  InpIndexW = 0x00U;
  BT_OutLen = 0x00U;                   /* No char in the transmit buffer */
  OutIndexR = 0x00U;                   /* Set index on the first item in the transmit buffer */
  OutIndexW = 0x00U;
  ASerialLdd2_DeviceDataPtr = ASerialLdd2_Init(NULL); /* Calling init method of the inherited component */
  EnMode = TRUE;                       /* Set the flag "device enabled" in the actual speed CPU mode */
  HWEnDi();                            /* Enable/disable device according to status flags */
}

/*
** ===================================================================
**     Method      :  BT_SetClockConfiguration (component AsynchroSerial)
**
**     Description :
**         This method changes the clock configuration.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void BT_SetClockConfiguration(LDD_TClockConfiguration ClockConfiguration)
{
  switch (ClockConfiguration) {
    case CPU_CLOCK_CONFIG_0:
      EnMode = TRUE;                   /* Set the flag "device enabled" in the actual speed CPU mode */
      break;
    default:
      EnMode = FALSE;                  /* Set the flag "device disabled" in the actual speed CPU mode */
      break;
  }
  if (EnMode) {                        /* Enable device? */
    HWEnDi();                          /* Enable/disable device according to status flags */
  }
}

#define ON_ERROR    0x01U
#define ON_FULL_RX  0x02U
#define ON_RX_CHAR  0x04U
/*
** ===================================================================
**     Method      :  BT_ASerialLdd2_OnBlockReceived (component AsynchroSerial)
**
**     Description :
**         This event is called when the requested number of data is 
**         moved to the input buffer.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void ASerialLdd2_OnBlockReceived(LDD_TUserData *UserDataPtr)
{
  register byte Flags = 0U;            /* Temporary variable for flags */

  (void)UserDataPtr;                   /* Parameter is not used, suppress unused argument warning */
  if (BT_InpLen < BT_INP_BUF_SIZE) {   /* Is number of bytes in the receive buffer lower than size of buffer? */
    BT_InpLen++;                       /* Increase number of chars in the receive buffer */
    InpBuffer[InpIndexW++] = (BT_TComData)BufferRead; /* Save received char to the receive buffer */
    if (InpIndexW >= BT_INP_BUF_SIZE) { /* Is the index out of the receive buffer? */
      InpIndexW = 0x00U;               /* Set index on the first item into the receive buffer */
    }
    Flags |= ON_RX_CHAR;               /* If yes then set the OnRxChar flag */
    if (BT_InpLen == BT_INP_BUF_SIZE) { /* Is number of bytes in the receive buffer equal to the size of buffer? */
      Flags |= ON_FULL_RX;             /* Set flag "OnFullRxBuf" */
    }
  } else {
    SerFlag |= FULL_RX;                /* Set flag "full RX buffer" */
    Flags |= ON_ERROR;                 /* Set the OnError flag */
  }
  if ((Flags & ON_ERROR) != 0U) {      /* Is any error flag set? */
    BT_OnError();                      /* Invoke user event */
  } else {
    if ((Flags & ON_RX_CHAR) != 0U) {  /* Is OnRxChar flag set? */
      BT_OnRxChar();                   /* Invoke user event */
    }
    if ((Flags & ON_FULL_RX) != 0U) {  /* Is OnTxChar flag set? */
      BT_OnFullRxBuf();                /* Invoke user event */
    }
  }
  (void)ASerialLdd2_ReceiveBlock(ASerialLdd2_DeviceDataPtr, &BufferRead, 1U); /* Receive one data byte */
}

#define ON_FREE_TX  0x01U
#define ON_TX_CHAR  0x02U
/*
** ===================================================================
**     Method      :  BT_ASerialLdd2_OnBlockSent (component AsynchroSerial)
**
**     Description :
**         This event is called after the last character from the output 
**         buffer is moved to the transmitter.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void ASerialLdd2_OnBlockSent(LDD_TUserData *UserDataPtr)
{
  word OnFlags = 0x00U;                /* Temporary variable for flags */

  (void)UserDataPtr;                   /* Parameter is not used, suppress unused argument warning */
  if ((SerFlag & RUNINT_FROM_TX) != 0U) { /* Is flag "running int from TX" set? */
    OnFlags |= ON_TX_CHAR;             /* Set flag "OnTxChar" */
  }
  OutIndexR++;
  if (OutIndexR >= BT_OUT_BUF_SIZE) {  /* Is the index out of the transmit buffer? */
    OutIndexR = 0x00U;                 /* Set index on the first item into the transmit buffer */
  }
  BT_OutLen--;                         /* Decrease number of chars in the transmit buffer */
  if (BT_OutLen != 0U) {               /* Is number of bytes in the transmit buffer greater then 0? */
    SerFlag |= RUNINT_FROM_TX;         /* Set flag "running int from TX"? */
    (void)ASerialLdd2_SendBlock(ASerialLdd2_DeviceDataPtr, (LDD_TData *)&OutBuffer[OutIndexR], 1U); /* Send one data byte */
  } else {
    SerFlag &= (byte)~(RUNINT_FROM_TX); /* Clear "running int from TX" and "full TX buff" flags */
    if (!(OnFreeTxBufSemaphore)) {     /* Is the OnFreeTXBuf flag blocked ?*/
      OnFlags |= ON_FREE_TX;           /* If not, set the OnFreeTxBuf flag */
    }
  }
  if ((OnFlags & ON_TX_CHAR) != 0x00U) { /* Is flag "OnTxChar" set? */
    BT_OnTxChar();                     /* If yes then invoke user event */
  }
  if ((OnFlags & ON_FREE_TX) != 0x00U) { /* Is flag "OnFreeTxBuf" set? */
    BT_OnFreeTxBuf();                  /* If yes then invoke user event */
  }
}

/*
** ===================================================================
**     Method      :  BT_ASerialLdd2_OnError (component AsynchroSerial)
**
**     Description :
**         This event is called when a channel error (not the error 
**         returned by a given method) occurs.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void ASerialLdd2_OnError(LDD_TUserData *UserDataPtr)
{
  LDD_SERIAL_TError SerialErrorMask;   /* Serial error mask variable */

  (void)UserDataPtr;                   /* Parameter is not used, suppress unused argument warning */
  (void)ASerialLdd2_GetError(ASerialLdd2_DeviceDataPtr, &SerialErrorMask); /* Get error state */
  if (SerialErrorMask != 0U) {
    SerFlag |= (((SerialErrorMask & LDD_SERIAL_PARITY_ERROR) != 0U ) ? PARITY_ERR : 0U);
    SerFlag |= (((SerialErrorMask & LDD_SERIAL_NOISE_ERROR) != 0U ) ? NOISE_ERR : 0U);
    SerFlag |= (((SerialErrorMask & LDD_SERIAL_RX_OVERRUN) != 0U ) ? OVERRUN_ERR : 0U);
    SerFlag |= (((SerialErrorMask & LDD_SERIAL_FRAMING_ERROR) != 0U ) ? FRAMING_ERR : 0U);
  }
  BT_OnError();                        /* Invoke user event */
}

/*
** ===================================================================
**     Method      :  BT_ASerialLdd2_OnBreak (component AsynchroSerial)
**
**     Description :
**         This event is called when a break occurs on the input channel.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void ASerialLdd2_OnBreak(LDD_TUserData *UserDataPtr)
{
  (void)UserDataPtr;                   /* Parameter is not used, suppress unused argument warning */
  SerFlag |= FRAMING_ERR;              /* Set framing error flag */
}


/* END BT. */

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
