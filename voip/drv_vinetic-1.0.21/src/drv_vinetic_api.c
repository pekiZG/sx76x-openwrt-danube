/****************************************************************************
                  Copyright © 2005  Infineon Technologies AG
                 St. Martin Strasse 53; 81669 Munich, Germany

   THE DELIVERY OF THIS SOFTWARE AS WELL AS THE HEREBY GRANTED NON-EXCLUSIVE,
   WORLDWIDE LICENSE TO USE, COPY, MODIFY, DISTRIBUTE AND SUBLICENSE THIS
   SOFTWARE IS FREE OF CHARGE.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" AND INFINEON EXPRESSLY DISCLAIMS
   ALL REPRESENTATIONS AND WARRANTIES, WHETHER EXPRESS OR IMPLIED, INCLUDING
   WITHOUT LIMITATION, WARRANTIES OR REPRESENTATIONS OF WORKMANSHIP,
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, DURABILITY, THAT THE
   OPERATING OF THE LICENSED SOFTWARE WILL BE ERROR FREE OR FREE OF ANY
   THIRD PARTY CLAIMS, INCLUDING WITHOUT LIMITATION CLAIMS OF THIRD PARTY
   INTELLECTUAL PROPERTY INFRINGEMENT.

   EXCEPT FOR ANY LIABILITY DUE TO WILFUL ACTS OR GROSS NEGLIGENCE AND
   EXCEPT FOR ANY PERSONAL INJURY INFINEON SHALL IN NO EVENT BE LIABLE FOR
   ANY CLAIM OR DAMAGES OF ANY KIND, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************
   Module      : drv_vinetic_api.c

   This file contains the implementation of the basic access
                 functions for the VINETIC access.                         */

/** \defgroup VinMbx VINETIC Mailbox API
\remarks
      Every access to the VINETIC has to be done via this module.
      Most functions are protected against reentrants and interrupts.
      Since functions ScRead and ScWrite can not be used in interrupt level
      macros for access are provided called SC_READ and SC_WRITE. They have to
      be used with care, manual protection from interrupts must be done!
      They should be used only in this and in the interrupt module.
*/

/* @{ */

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_vinetic_api.h"
#include "drv_vinetic_main.h"

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */
#if 0
IFX_LOCAL const IFX_char_t *const underline = "\033[4;30m";
IFX_LOCAL const IFX_char_t *const red       = "\033[0;31m";
IFX_LOCAL const IFX_char_t *const blue      = "\033[0;34m";
#endif
#ifdef RUNTIME_TRACE
IFX_LOCAL const IFX_char_t *const bold      = "\033[1;30m";
#endif /* RUNTIME_TRACE */
#ifdef ENABLE_TRACE
IFX_LOCAL const IFX_char_t *const boldred   = "\033[1;31m";
IFX_LOCAL const IFX_char_t *const normal    = "\033[0m";
#endif /* ENABLE_TRACE */

/* ============================= */
/* Global variable definition    */
/* ============================= */

/* ============================= */
/* Global function declaration   */
/* ============================= */

/* ============================= */
/* Local function declaration    */
/* ============================= */

#ifndef ASYNC_CMD
IFX_LOCAL IFX_int32_t ObxmlPoll       (VINETIC_DEVICE *pDev, IFX_uint8_t* count);
#endif /* ASYNC_CMD */
IFX_LOCAL IFX_int32_t waitMbxSpace    (VINETIC_DEVICE *pDev, IFX_uint8_t count);
#ifdef FW_ETU
IFX_LOCAL IFX_int32_t MapEpouTriggerToRfc2833(IFX_uint16_t epou);
#endif /* FW_ETU */
#ifdef ASYNC_CMD
IFX_LOCAL IFX_uint16_t* CmdAsyncFilterMsg(VINETIC_DEVICE *pDev, const IFX_uint16_t *pCmd);
#endif /* ASYNC_CMD */

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* ============================= */
/* Local function definition     */
/* ============================= */

#ifndef ASYNC_CMD
/**
  Polls CDATA-SIZE in OBXML to be not zero
\param
   pDev   - handle of the device structure
\param
   count  - minimum amount of data that must be available
\return
   IFX_SUCCESS if all or more data is available or IFX_ERROR
\remarks
   Polls the OBXML register for at least 'count' data. A maximum time of
   WAIT_POLLTIME * OBXML_POLL_LOOP is waited. The VINETIC should answer
   within 500 us. After 5 ms this  function signals a timeout.
   Timeout may occur in case of a HOST or CERR indicated in the mailbox
   register or the VINETIC does not answer.
   If an mailbox error occurs this function will also return an error

   The functions calling this function should protect it against concurrent
   tasks, so no task locking is done within this function.
   Protection against Interrupts must be done, as the OBXML register can be
   read from interrupt also.
*/
IFX_LOCAL IFX_int32_t ObxmlPoll (VINETIC_DEVICE *pDev, IFX_uint8_t* count)
{
   IFX_int32_t err = IFX_SUCCESS,
       /* loop counter */
       cnt = 0;
   IFX_uint16_t nObxml, nCount,
      /* size of data amount */
      nSize;

   /* shift count to match to bit position of OBXM_CDATA. Avoids shifting
      nSize each time OBXML was read  */
   nCount = *count << 8;
   /* read OBXML in timeout till enough data is available */
   while (cnt <= OBXML_POLL_LOOP)
   {
      /* here we use the macro to read the data, cause the mailbox is
         already protected by the caller and thus no protection in ScRead
         needs to be done
      */
      /* protect against interrupts */
      Vinetic_IrqLockDevice (pDev);
      err = SC_READ (SC_ROBXML, &nObxml, 1);
      /* release irq lock */
      Vinetic_IrqUnlockDevice (pDev);

      if (err != IFX_SUCCESS)
      {
         SET_ERROR (ERR_DEV_ERR);
         return IFX_ERROR;
      }
      else
      {
         if (pDev->IsrFlag [EV_MBX] &
             (BXSR2_HOST_ERR | BXSR2_CIBX_OF | BXSR1_CERR))
         {
            /* error occured and acknowledged already in
               interrupt level so clear flag */
            pDev->IsrFlag [EV_MBX] &= ~(BXSR1_CERR |
                                        BXSR2_HOST_ERR |
                                        BXSR2_CIBX_OF);
            return IFX_ERROR;
         }
      }
      /* calculate the size of the available data */
      nSize = nObxml & OBXML_CDATA;
      if (nSize >= nCount)
      {
         /* thats all, we got enough data */
         *count = nSize >> 8;
         return IFX_SUCCESS;
      }
      /* wait time, to not stress the interface */
      IFXOS_DELAYUS (WAIT_POLLTIME);
      cnt++;
   }
   /* time out: not enough data arrived */
   SET_ERROR (ERR_OBXML_ZERO);
   return IFX_ERROR;
}
#endif /* #ifndef ASYNC_CMD */

/**
   Checks by polling for free command data inbox space and waits a maximum time,
   if no space available
\param
   pDev   - pointer to the device structure
\param
   count  - required space to wait for. No check is done on this parameter!
\return
   IFX_SUCCESS or IFX_ERROR in case of an error. Follwing errors may occur:
   ERR_NO_FIBXMS and ERR_DEV_ERR
\remarks
   This function does not perform any lock to the vinetic. Thus the calling
   function must guard this function from interrupts
   If function returns with an error, the VINETIC access failed or the
   desired size was not reached.
   The function returns immediately if the desired size is greater
   as the maximum inbox size.
   Protection against tasks and interrupts must be done by the caller, as this
   function executes/resumes very fast and will not block interrupts.
*/
IFX_LOCAL IFX_int32_t waitMbxSpace (VINETIC_DEVICE *pDev, IFX_uint8_t count)
{
   IFX_LOCAL IFX_uint16_t pFibmx [] = { 0x0801, 0xA803, 0x01E0, 0x01E0, 0x01E0 };
   IFX_uint16_t nFibxms, nSpace,
      nCount; /* desired value in FIBXMS (count shifted by 8) */
   IFX_int32_t err, cnt = 0;

   /* shift count to match to bit position of FIBXMS_CBOX. Avoids shifting
      nSpace each time FIBXMS was read  */
   nCount = count << 8;
   /* read FIBMX in timeout till enough memory or till timeout */
   for (;;)
   {
      /* read free inbox space */
      err = SC_READ (SC_RFIBXMS, &nFibxms, 1);
      if (err == IFX_ERROR)
      {
          SET_ERROR (ERR_DEV_ERR);
          break;
      }
      /* calculate available space */
      nSpace = (nFibxms & FIBXMS_CBOX);
      /* check space */
      if (nSpace < nCount)
      {
         /* not enough space available, so wait by polling */
         IFXOS_DELAYUS (WAIT_POLLTIME);
         if (++cnt > (FIBXMS_POLL_LOOP))
         {
             err = pDev->write(pDev, pFibmx, sizeof(pFibmx) / 2);
             if (err == IFX_ERROR)
             {
                 SET_ERROR (ERR_DEV_ERR);
             }
             else
             {
                 SET_ERROR (ERR_NO_FIBXMS);
                 err = IFX_ERROR;
             }
             break;
         }
      }
      else
      {
         /* required space available so go out */
         break;
      }
   }
   return err;
}

/* ============================= */
/* Global function definition    */
/* ============================= */

/**
   write a short command to vinetic
\param
   pDev  - pointer to the device interface structure
\param
   cmd   - short command to write to the vinetic
\return
   IFX_SUCCESS if no error, otherwise IFX_ERROR
\remarks
   This function is protected against interrupts.
   Shared variables in this function must be protected against concurrent
   tasks access by the caller.
*/
IFX_int32_t ScWrite(VINETIC_DEVICE *pDev, IFX_uint16_t cmd)
{
   IFX_int32_t err;

   /* protect vinetic access against concurrent tasks */
   IFXOS_MutexLock (pDev->mbxAcc);
   /* protect vinetic access against interrupts */
   Vinetic_IrqLockDevice(pDev);
   err = pDev->write(pDev, &cmd, 1);
   /* release interrupt lock */
   Vinetic_IrqUnlockDevice(pDev);
   /* release task lock */
   IFXOS_MutexUnlock (pDev->mbxAcc);
#if defined(RUNTIME_TRACE)
   TRACE (VINETIC, DBG_LEVEL_LOW,
         ("ScWrite: cmd = 0x%04X\n\r", cmd));
#endif /* RUNTIME_TRACE */
   if (err != IFX_SUCCESS)
   {
      SET_ERROR (ERR_DEV_ERR);
   }
   else
   {
      /* check any error occuring on this command */
      if (pDev->IsrFlag [EV_MBX] &
          (BXSR2_HOST_ERR | BXSR2_CIBX_OF | BXSR1_CERR))
      {
         /* error occured and acknowledged already in interrupt
            level so clear flag */
         pDev->IsrFlag [EV_MBX] &= ~(BXSR1_CERR |
                                     BXSR2_HOST_ERR |
                                     BXSR2_CIBX_OF);
         err = IFX_ERROR;
      }
   }
#ifdef TESTING
   pDev->lastSC = cmd;
#endif /* TESTING */

   return err;
}

/**
   sends a short command and reads appropriate data from vinetic
\param
   pDev   - pointer to the device interface structure
\param
   cmd    - short command to read the vinetic
\param
   pData  - buffer containing the read data
\param
   count  - number of word awaited in reading
\return
   IFX_SUCCESS if no error, otherwise IFX_ERROR
\remarks
   This function is protected against interrupts.
   Shared variables in this function must be protected against concurrent
   tasks access by the caller.
*/
IFX_int32_t ScRead(VINETIC_DEVICE *pDev,IFX_uint16_t cmd, IFX_uint16_t* pData, IFX_uint8_t count)
{
   IFX_int32_t err;

   /* protect vinetic access against concurrent tasks */
   IFXOS_MutexLock (pDev->mbxAcc);
   /* protect vinetic access against interrupts */
   Vinetic_IrqLockDevice(pDev);
   err = pDev->read (pDev, cmd, pData, count);
   /* release interrupt lock */
   Vinetic_IrqUnlockDevice(pDev);
   /* release task lock */
   IFXOS_MutexUnlock (pDev->mbxAcc);
   if (err != IFX_SUCCESS)
   {
      SET_ERROR (ERR_DEV_ERR);
   }
   else
   {
      if (pDev->IsrFlag [EV_MBX] &
          (BXSR2_HOST_ERR | BXSR2_CIBX_OF | BXSR1_CERR))
      {
         /* error occured and acknowledged already in
            interrupt level so clear flag */
         pDev->IsrFlag [EV_MBX] &= ~(BXSR1_CERR |
                                     BXSR2_HOST_ERR |
                                     BXSR2_CIBX_OF);
         err = IFX_ERROR;
      }
   }
#ifdef TESTING
   pDev->lastSC = cmd;
#endif /* TESTING */

   return err;
}

/**
   Send a register write command to the VINETIC
\param
   pDev    - pointer to the device interface structure
\param
   cmd     - command to write to the vinetic, including register
            type, channel number or broadcast. Specifying the read/write
            bit is not neccessary.
\param
   pData   - register data buffer to write
\param
   count   - number of registers to write, refere to length field of register
            write command
\param
   offset  - parameter offset of register command message. Part of second
            command word.
\return
   IFX_SUCCESS if no error, otherwise IFX_ERROR.
\code
   write value 0x55 to the first register of channel 2
      IFX_uint16_t val = 0x55;
      RegWrite (pDev, CMD1_SOP | 2, &val, 1, 0);
\endcode
*/
IFX_int32_t RegWrite (VINETIC_DEVICE *pDev, IFX_uint16_t cmd,
              IFX_uint16_t *pData, IFX_uint8_t count, IFX_uint8_t offset)
{
   IFX_uint16_t pCmd [MAX_CMD_WORD];

   /* set write command header */
   pCmd[0] = cmd;
   pCmd[1] = offset << 8;

   /* add the data to write to command buffer */
   memcpy (&pCmd[CMD_HEADER_CNT], pData, count * 2);
   return CmdWrite (pDev, pCmd, count);
}

/**
  Read a register from the VINETIC
\param
   pDev    - pointer to the device interface structure
\param
   cmd     - command to write to the vinetic, including register
            type, channel number or broadcast. Specifying the read/write
            bit is not neccessary.
\param
   pData   - register data buffer to for read
\param
   count   - number of registers to read, refere to length field of register
            read command
\param
   offset  - parameter offset of register command message. Part of second
            command word.
\return
   IFX_ERROR if error in writing the command, otherwise IFX_SUCCESS
\remarks
   This function returns an error in the following cases:
   - count is too large, maximum is the size of the mailbox
   - error occured in the command access, see CmdRead
   - data length between read and desired does not match
\code
   //read the first register of channel 2
   IFX_uint16_t val, err;
   err = RegRead (pDev, CMD1_SOP | 2, &val, 1, 0);
   //check len ...
\endcode
*/
IFX_int32_t RegRead (VINETIC_DEVICE *pDev, IFX_uint16_t cmd,
             IFX_uint16_t *pData, IFX_uint8_t count, IFX_uint8_t offset)
{
   IFX_uint16_t pCmd[CMD_HEADER_CNT], pBuf [MAX_CMD_WORD];
   IFX_int32_t err;

   /* check command length */
   if (count > (MAX_CMD_WORD - CMD_HEADER_CNT))
   {
      SET_ERROR (ERR_FUNC_PARM);
      return IFX_ERROR;
   }

   /* set command header */
   pCmd[0] = cmd;
   pCmd[1] = offset << 8;

   /* read the register command */
   err = CmdRead (pDev, pCmd, pBuf, count);
   if (err != IFX_SUCCESS)
      return err;
   /* get read register count */
   if (count != (pBuf[1] & CMD2_LEN))
   {
      SET_ERROR (ERR_WRONGDATA);
      return IFX_ERROR;
   }
   /* fill data array */
   memcpy (pData, &pBuf[CMD_HEADER_CNT], count * 2);

   return IFX_SUCCESS;
}

/**
   Modify a VINETIC register
\param
   pDev     - pointer to the device interface structure
\param
   cmd      - command to write to the vinetic, including register
             type, channel number or broadcast. Specifying the read/write
             bit is not neccessary.
\param
   offset   - parameter offset of register command message. Part of second
             command word.
\param
   nMask     - set to one for bits to be changed by nSetBits
\param
   nSetBits  - value of masked bits to be set
\return
   IFX_ERROR if error in writing the command, otherwise IFX_SUCCESS
\code
   read the first register of channel 2
      IFX_uint16_t val, err;
      err = RegRead (pDev, CMD1_SOP | 2, &val, 1, 0);
   check len ...
\endcode
*/
IFX_int32_t RegModify (VINETIC_DEVICE *pDev, IFX_uint16_t cmd, IFX_uint8_t offset,
               IFX_uint16_t nMask, IFX_uint16_t nSetBits)
{
   IFX_int32_t err;
   IFX_uint16_t nVal;

   /* read register value */
   err = RegRead (pDev, cmd, &nVal, 1, offset);
   if (err >= IFX_SUCCESS)
   {
      /* modify register value */
      nVal = (nVal & ~nMask) | (nSetBits & nMask);
      /* write register new value */
      err = RegWrite (pDev, cmd, &nVal, 1, offset);
   }

   return err;
}

/**
   Write VINETIC Command
\param
   pDev  - pointer to the device interface
\param
   pCmd  - pointer to write command Buffer. It contains the 2 command words
           needed and the data to be written
\param
   count  - number of data to write, without command header. May not be larger
           then maximum mailbox size
\return
   IFX_ERROR if count to large, no mailbox space is available, device not available
   or mailbox error occured (error codes ERR_NO_FIBXMS, ERR_DEV_ERR,
   ERR_CIBX_OF, ERR_HOST, ERR_CERR).
   Otherwise SUCESS
\remarks
   The Write bit is coded here in the first cmd whereas the length information
   is coded in the second cmd word.
   For security purposes, the fields RW in CMD1 and LENGTH in CMD2 are first
   cleared, and set to their new value.
   Shared variables in this function must be protected against concurrent
   access by the caller
*/
IFX_int32_t CmdWrite (VINETIC_DEVICE *pDev, IFX_uint16_t* pCmd, IFX_uint8_t count)
{
   IFX_int32_t err;

   /* check if maximal cmd oder packet Mbx size */
   if (count > (MAX_PACKET_WORD - CMD_HEADER_CNT))
   {
      SET_ERROR (ERR_FUNC_PARM);
      return IFX_ERROR;
   }
   /* for security, clear not desired bits */
   pCmd[0] &= ~(CMD1_SC);
   /* clear and fill the length field */
   pCmd[1]  &= ~CMD2_LEN;
   pCmd[1]  |= (IFX_uint16_t)count;
   if (pCmd[0] & CMD1_RD)
   {
      /* if read command issued just write command words */
      count = 0;
   }
   /* increase count with command header */
   count += CMD_HEADER_CNT;
   /* protect vinetic access against concurrent tasks */
   IFXOS_MutexLock (pDev->mbxAcc);
   /* protect vinetic access against interrupts */
   Vinetic_IrqLockDevice(pDev);
   /* wait for free space to write the command */
   err = waitMbxSpace (pDev, count);
   if (err == IFX_SUCCESS)
   {
      /* write the command sequence */
      err = pDev->write (pDev, (IFX_uint16_t*)pCmd, count);
      if (err == IFX_ERROR)
      {
         SET_ERROR (ERR_DEV_ERR);
      }
   }
   /* release interrupt lock */
   Vinetic_IrqUnlockDevice(pDev);
   if (err == IFX_SUCCESS)
   {
      if (pDev->IsrFlag [EV_MBX] &
          (BXSR2_HOST_ERR | BXSR2_CIBX_OF | BXSR1_CERR))
      {
         /* error occured and acknowledged already in
            interrupt level so clear flag */
         pDev->IsrFlag [EV_MBX] &= ~(BXSR1_CERR |
                                     BXSR2_HOST_ERR |
                                     BXSR2_CIBX_OF);
         err = IFX_ERROR;
      }
   }
   /* release task lock */
   IFXOS_MutexUnlock (pDev->mbxAcc);
#ifdef TESTING
   pDev->lastCmd[0] = pCmd[0];
   pDev->lastCmd[1] = pCmd[1];
#endif /* TESTING */
#if defined(RUNTIME_TRACE)
   if (err == IFX_SUCCESS)
   {
      IFX_int32_t i;
      TRACE (VINETIC, DBG_LEVEL_LOW,
            ("CmdWrite: cmd1 = 0x%04X, cmd2 = 0x%04X\n\r", pCmd[0], pCmd[1]));
      for (i = 0; i < (count - CMD_HEADER_CNT); i++)
      {
         TRACE (VINETIC, DBG_LEVEL_LOW,
             ("  %2li: 0x%04X \n\r", i+1, pCmd[2+i]));
      }
   }
#endif /* defined(RUNTIME_TRACE) */
   return err;
}

/**
   Write VINETIC Command without blocking, used only in interrupt context
\param
   pDev  - pointer to the device interface
\param
   pCmd  - pointer to write command Buffer. It contains the 2 command words
          needed and the data to be written.
\param
   count - number of data to write, without command header. May not be larger
           then maximum mailbox size
\return
   IFX_ERROR if no inbox space available or device error occured, otherwise IFX_SUCCESS
\remarks
   - The command words must be set properly before calling this function
     (length, read/write bit, ...).

   - The count schould take also the 2 commands words in count.

   - No security checks are done on the command fields
     This function was intended to be called from interrupt level. It must be
     protected from interrupts and other tasks if used at other places than in
     the interrupt routine.
*/
IFX_int32_t CmdWriteIsr (VINETIC_DEVICE *pDev, IFX_uint16_t* pCmd, IFX_uint8_t count)
{
   IFX_int32_t err;
   IFX_uint16_t nFibxms;

   count += CMD_HEADER_CNT;

   err = SC_READ (SC_RFIBXMS, &nFibxms, 1);
   if (err == IFX_SUCCESS)
   {
      if (((nFibxms & FIBXMS_CBOX) >> 8) < count)
      {
         SET_ERROR (ERR_NO_FIBXMS);
         return IFX_ERROR;
      }
      err = pDev->write (pDev, (IFX_uint16_t*)pCmd, count);
   }
   if (err != IFX_SUCCESS)
   {
      SET_ERROR (ERR_DEV_ERR);
   }

   return err;
}

/**
   Read the VINETIC Command
\param
   pDev   - pointer to the device interface
\param
   pCmd   - command for the read access
\param
   pData  - memory pointer for Data to read including two command words
\param
   count  - amount of data to read, without command header
\return
   IFX_SUCCESS if no error or IFX_ERROR if any error. Following errors may occur:
   ERR_OBXML_ZERO, ERR_DEV_ERR, ERR_CIBX_OF, ERR_HOST, ERR_CERR
   and ERR_NO_FIBXMS
\remarks
   First the content of the 2 command words in pCmd are written
   This function codes the read bit and the length information into the
   commands words. For security purposes, the fields RW in CMD1 and
   LENGTH in CMD2 are first cleared and set to their new value.
   If data is available, the register OBXML gives the amount of data
   present in the mailbox, which must be equal to or greater than the amount
   given as parameter (count parameter).
   Note: A checking for free outbox space for the responds is not done, since
   the driver always empties the outbox.
   Shared variables in this function must be protected against concurrent
   access by the caller
*/
IFX_int32_t CmdRead (VINETIC_DEVICE *pDev, IFX_uint16_t* pCmd, IFX_uint16_t* pData, IFX_uint8_t count)
{
   IFX_int32_t err;
#ifdef ASYNC_CMD
   IFX_uint16_t *pBuffer;
   IFX_int32_t i;
#endif /* ASYNC_CMD */

   /* protect vinetic access against concurrent tasks */
   IFXOS_MutexLock (pDev->mbxAcc);
   /* protect vinetic access against interrupts */
   Vinetic_IrqLockDevice(pDev);

   err = waitMbxSpace (pDev, CMD_HEADER_CNT);
   if (err == IFX_ERROR)
   {
      /* release irq lock */
      Vinetic_IrqUnlockDevice(pDev);
      /* release task lock */
      IFXOS_MutexUnlock(pDev->mbxAcc);
      return IFX_ERROR;
   }
   /* Set the read bit */
   pCmd[0] |= CMD1_RD;
   /*for security, clear indesirable bits */
   pCmd[0] &= ~CMD1_SC;
   /*fill the length field */
   pCmd[1]  &= ~CMD2_LEN;
   pCmd[1] |= (IFX_uint16_t)count;

#ifdef ASYNC_CMD
   /* mark the local buffer as empty and reset the fill counter
      variable. The content of the buffer was used by the interrupt or
      previous calls from the task level */
   pDev->nCmdReadCnt = 0;
   /* set the flag that the task mode is going to wait for response */
   pDev->bAsyncReq = IFX_TRUE;
#endif /* ASYNC_CMD */


   err = pDev->write (pDev, (IFX_uint16_t*)pCmd, CMD_HEADER_CNT);
   /* release irq lock */
   Vinetic_IrqUnlockDevice (pDev);
#ifdef TESTING
   pDev->lastCmd[0] = pCmd[0];
   pDev->lastCmd[1] = pCmd[1];
#endif /* TESTING */
   if (err != IFX_SUCCESS)
   {
#ifdef ASYNC_CMD
      /* reset the flag that the task is requesting a message
         because of error condition. This modification does not
         have to be protected against interrupt, because ISR is 
         modifiying it */
      pDev->bAsyncReq = IFX_FALSE;
#endif /* ASYNC_CMD */
      SET_ERROR (ERR_DEV_ERR);
      /* release task lock */
      IFXOS_MutexUnlock(pDev->mbxAcc);
      return IFX_ERROR;
   }
#if defined(RUNTIME_TRACE)
   TRACE (VINETIC, DBG_LEVEL_LOW,
           ("CmdRead: cmd1 = 0x%04X, cmd2 = 0x%04X\n\r", pCmd[0], pCmd[1]));
#endif /* defined(RUNTIME_TRACE) */

#ifdef ASYNC_CMD
   for (i = 0;i < OBXML_POLL_LOOP * 1000; i++)
   {
      if (pDev->nCmdReadCnt == 0)
      {
         /* in order to get the response, disable the interrupts and poll the
            command mailbox. In order to read directly from the mailbox we call
            the function CmdReadIsr() as it is used inside ISR */
         Vinetic_IrqLockDevice(pDev);
         if (CmdReadIsr (pDev) > 0)
         {
            /* the CmdReadIsr() has read out some messages from the mailbox.
               Parse these messages to dispatchCmd by using
               DispatchCmdAsyncBuffer() */
            DispatchCmdAsyncBuffer(pDev);
         }
         Vinetic_IrqUnlockDevice (pDev);
      }

      if (pDev->nCmdReadCnt > 0)
      {
         /* we got something from the device. Go and check if our
            data are in the buffer. We do not have to disable the
            interrupt for it, because the interrupt ('CmdReadIsr')
            does not modify the content of the buffer if 
            'nCmdReadCnt' and 'bAsyncReq' are set. */
      
         pBuffer = CmdAsyncFilterMsg(pDev, pCmd);

         if (pBuffer != NULL)
         {
            /* found the message, go and copy */
            memcpy(pData, pBuffer, (count * 2) + 4);
            /* reset the flag because we got a message */
            pDev->bAsyncReq = IFX_FALSE;
            break;
         }
         else
         {
            pDev->nCmdReadCnt = 0;
         }
      }
   }

   if (pBuffer == NULL)
   {
      /* if the pointer is still zero, set an error */
      err = IFX_ERROR;
   }
#else
   /* poll for data */
   err = ObxmlPoll (pDev, &count);
   if (err == IFX_SUCCESS)
   {
      /* protect against interrupts */
      Vinetic_IrqLockDevice (pDev);
      /* read expected data */
      err = SC_READ (SC_RCOBX, pData, count);
      /* release interrupts lock */
      Vinetic_IrqUnlockDevice (pDev);
      if (err != IFX_SUCCESS)
      {
         SET_ERROR (ERR_DEV_ERR);
      }
   }
#endif /* #else ASYNC_CMD */
   /* release tasks lock */
   IFXOS_MutexUnlock(pDev->mbxAcc);
   /* clear read flag to use it afterwards in CmdWrite */
   pData[0] &= ~CMD1_RW;
#if defined(RUNTIME_TRACE)
   if (err != IFX_SUCCESS)
      return IFX_ERROR;
   {
      IFX_int32_t j;
      for (j = 0; j < (pData[1] & CMD2_LEN); j++)
      {
         TRACE (VINETIC, DBG_LEVEL_LOW,
             ("  %2li: 0x%04X \n\r", j+1, pData[2+j]));
      }
   }
#endif /* defined(RUNTIME_TRACE) */

   return err;
}

#ifdef FW_ETU
/**
   This function maps the EPOU field of the VINETIC firmware message
   "Read_EPOU_Trigger (RTP Support)" to an equivalent RFC2833 event number.

\param
    epou - EPOU field of the VINETIC firmware message
\return
      returns the RFC2833 event number. Returns (-1) if unknown event
\remarks
*/
IFX_LOCAL IFX_int32_t MapEpouTriggerToRfc2833(IFX_uint16_t epou)
{
   IFX_int32_t rfcEvent;

   if (epou & SIG_EPOUTRIG_DTMF)
   {
      /* DTMF tone received */
      rfcEvent = (epou & SIG_EPOUTRIG_DTMFCODE) >> SIG_EPOUTRIG_DTMFBIT;
   }
   else if (epou & SIG_EPOUTRIG_ANS)
   {
      /* ANS tone received */
      rfcEvent = TAPI_RFC2833_EVENT_ANS;
   }
   else if (epou & SIG_EPOUTRIG_NANS)
   {
      /* /ANS tone received */
      rfcEvent = TAPI_RFC2833_EVENT_NANS;
   }
   else if (epou & SIG_EPOUTRIG_ANSAM)
   {
      /* ANSam tone received */
      rfcEvent = TAPI_RFC2833_EVENT_ANSAM;
   }
   else if (epou & SIG_EPOUTRIG_NANSAM)
   {
      /* /ANSam tone received */
      rfcEvent = TAPI_RFC2833_EVENT_NANSAM;
   }
   else if (epou & SIG_EPOUTRIG_CNG)
   {
      /* CNG tone received */
      rfcEvent = TAPI_RFC2833_EVENT_CNG;
   }
   else if (epou & SIG_EPOUTRIG_DIS)
   {
      /* DIS tone received */
      rfcEvent = TAPI_RFC2833_EVENT_DIS;
   }
   else
   {
      rfcEvent = (-1);
   }

   return rfcEvent;
}
#endif /* FW_ETU */

/**
   Dispatches the command read by copying it into appropriate buffer
\param
   pDev      - pointer to the device
\param
   pData     - data containing the command
\return
   none
\remarks
   this function must be protected from calling function against concurent
   access or interrupts susceptible to modify the contain of cached fw messages
   The firmware semaphore should be used for locking.
*/
IFX_void_t DispatchCmd (VINETIC_DEVICE *pDev, IFX_uint16_t const *pData)
{
   IFX_uint8_t i, len, ch;

   ch = pData[0] & CMD1_CH;
   switch (pData[0] & CMD1_CMD)
   {
   case  CMD1_EOP:
      len = (pData[1] & CMD2_LEN);
      switch ((pData[1] & ~CMD2_LEN) & (CMD2_ECMD | CMD2_MOD) )
      {
#if 0
      case ECMD_COD_CHDECSTAT:
         /* interested in Decoder change status ! */
         /* check if ch matches to number of coder ressources */
         if (ch >= pDev->nCoderCnt)
            break;
         pDev->pCodCh[ch].dec_update = IFX_TRUE;
         pDev->pCodCh[ch].dec_status [CMD_HEADER_CNT] = pData[CMD_HEADER_CNT];
         break;
#endif /* 0 */
      case ECMD_CID_SEND:
         /* check if ch matches to number of signalling ressources */
         if (ch >= pDev->nSigCnt)
            break;
         pDev->pSigCh[ch].cid_sender [CMD_HEADER_CNT] = pData[CMD_HEADER_CNT];
         break;
      case ECMD_SIG_CH:
         /* check if ch matches to number of signalling ressources */
         if (ch >= pDev->nSigCnt)
            break;
         /* This message has a minimum length of one data word... */
         pDev->pSigCh[ch].sig_ch.value[CMD_HEADER_CNT] = pData[CMD_HEADER_CNT];
         break;
      case ECMD_ALM_CH:
         /* check if ch matches to number of analog ressources */
         if (ch >= pDev->nAnaChan)
            break;
         for (i = 0; (i < len) && (i < CMD_ALM_CH_LEN); i++)
            pDev->pAlmCh[ch].ali_ch.value[CMD_HEADER_CNT + i] = pData[i];
         break;
      case ECMD_PCM_CH:
         /* check if ch matches to number of analog ressources */
         if (ch >= pDev->nPcmCnt)
            break;
         for (i = 0; (i < len) && (i < CMD_PCM_CH_LEN); i++)
            pDev->pPcmCh[ch].pcm_ch.value[CMD_HEADER_CNT + i] = pData[i];
         break;
      case ECMD_COD_CH:
         /* check if ch matches to number of coder ressources */
         if (ch >= pDev->nCoderCnt)
            break;
         for (i = 0; (i < len) && (i < CMD_COD_CH_LEN); i++)
            pDev->pCodCh[ch].cod_ch.value[CMD_HEADER_CNT + i] = pData[i];
         break;
      case ECMD_COD_CHRTCP:
         /* check if ch matches to number of coder ressources */
         if (ch >= pDev->nCoderCnt)
            break;
         for (i = 0; (i < len) && (i < CMD_COD_RTCP_LEN); i++)
            pDev->pCodCh[ch].rtcp[i] = pData[CMD_HEADER_CNT + i];
         /* generate event ? */
         pDev->pCodCh[ch].rtcp_update = IFX_TRUE;
         break;
#ifdef FW_ETU
      case ECMD_EPOU_TRIG:
         /* check if ch matches to number of tapi ressources */
         if (ch >= pDev->TapiDev.nMaxChannel)
            break;
         TAPI_Rfc2833_Event (&pDev->TapiDev.pChannelCon[ch],
            (IFX_uint32_t)MapEpouTriggerToRfc2833(pData[CMD_HEADER_CNT]));
         break;
#endif /* FW_ETU */
      default:
         /* nothing of interest */
         break;
      }
      break;
   default:
      break;
   }
}

#ifdef ASYNC_CMD
/**
   Reads command in Isr context and dispatch them, if needed
\param
   pDev  - pointer to the device
\return
   none
\remarks
   Some firmware commands aren't directly read via CmdRead.
   Instead, a read command is written when the interrupt event occurs, and
   later on, when COBX event occurs, it is checked wether the read command is
   the one awaited, and if yes, the command is stored in the device or channel
   structure for a later use.
   Two applications of this are:
      - Coder Switching AAL (on SRE2 [DEC_CHG])
      - RTCP statistics
   After reading and dispatching the data, the flag DS_CMDREQ  will be cleared
   and depending on a DS_RDCMD_SENT the flag DS_CMDREAD is set, to indicate
   that probably the requested data is available in the device mailbox */
IFX_int32_t  CmdReadIsr (VINETIC_DEVICE *pDev)
{
   IFX_uint16_t nObxml;
   IFX_uint8_t nRead = 0;

   if ((pDev->nCmdReadCnt != 0) && (pDev->bAsyncReq != IFX_FALSE))
   {
      /* if CmdRead() has placed a request 'bAsyncReq' and the local buffer
         still contains information 'nCmdReadCnt': Do not overwrite
         the local buffer content till CmdRead() has read out all information
         from the local buffer. CmdRead() will reset 'nCmdReadCnt' if
         all firmware messages in the local buffer are checked or the function
         returns */
         return 0;
   }

   for (;;)
   {
      IFX_int32_t err;
      /* read number of command data available in the mailbox */
      /* it is actually assumed that the command mailbox contains data only
         for one command.*/
      err = SC_READ (SC_ROBXML, &nObxml, 1);
      if (err == IFX_ERROR)
         break;
      if (nObxml & OBXML_CDATA)
      {
         IFX_uint8_t nSize, nMaxRead;
         nSize = ((nObxml & OBXML_CDATA) >> 8);

         /* calculate the fill level of the local buffer after the
            mailbox will be read out. Check that this read out will
            not exceed the size of the buffer */
         nMaxRead = nSize + nRead;

         if (nMaxRead >= MAX_CMD_WORD)
            break;

         err = SC_READ (SC_RCOBX, pDev->pCmdData + nRead , nSize);
         if (err == IFX_ERROR)
            break;

         /* update the variable about the current fill level of the local
            buffer */
         nRead = nMaxRead;
      }
      else
         break;
   } /* for */

   /* update the number of read words in the device structure for the local 
      buffer */
   pDev->nCmdReadCnt = nRead;
   return nRead;
}

/**
   Dispatch all messages that where placed inside the local buffer by using
   CmdReadIsr
\param
   pDev  - pointer to the device
\return
   none
\remarks
*/
IFX_void_t DispatchCmdAsyncBuffer(VINETIC_DEVICE *pDev)
{
   IFX_int32_t nCmdReadCnt = pDev->nCmdReadCnt;
   IFX_uint16_t *pCmdData = pDev->pCmdData;

   /* parse buffer and dispatch message accordingly */
   while (nCmdReadCnt > 0)
   {
      IFX_uint8_t size;

      /* there is something of interest, which was requested before */
      DispatchCmd (pDev, pCmdData);

      /* get the length of the firmware message */
      size = (*(pCmdData + 1) & CMD2_LEN) + CMD_HEADER_CNT;

      /* increase the pointer by the size of the message in order to point
         to the next message */
      pCmdData += size;
      /* reduce the amount of words that have to be parsed */
      nCmdReadCnt -= size;
   }
}

/**
   Run through the local buffer and have a look for a message response
   This message response should fit to the message request
\param
   pDev  - pointer to the device
   pCmd  - pointer to the request message to compare 
\return
   pointer to the firmware message in the local buffer. Zero if message not
   found
\remarks
*/
IFX_LOCAL IFX_uint16_t* CmdAsyncFilterMsg(VINETIC_DEVICE *pDev, const IFX_uint16_t *pCmd)
{
   IFX_int32_t nCmdReadCnt = pDev->nCmdReadCnt;
   IFX_uint16_t *pCmdData = pDev->pCmdData;

   /* parse buffer and dispatch message accordingly */
   while (nCmdReadCnt > 0)
   {
      IFX_uint8_t size;

      if ((pCmd[0] == pCmdData[0]) && (pCmd[1] == pCmdData[1]))
      {
         /* found the message inside the local buffer */
         return pCmdData;
      }
      
      /* get the length of the firmware message */
      size = (*(pCmdData + 1) & CMD2_LEN) + CMD_HEADER_CNT;

      /* increase the pointer by the size of the message in order to point
         to the next message */
      pCmdData += size;
      /* reduce the amount of words that have to be parsed */
      nCmdReadCnt -= size;
   }
   return NULL;
}
#endif /* ASYNC_CMD */


/**
   Trace the data from shadow register.
\param
   reg       - pointer to the register structure
\return
   None
\remarks
   None
\code
   prReg(BCR1_REG);
\endcode
*/
IFX_void_t prReg (IFX_void_t *reg)
{
#ifdef RUNTIME_TRACE
   REG_TYPE *r = (REG_TYPE *) reg;
   TRACE(VINETIC_RUNTIME_TRACE, DBG_LEVEL_NORMAL,
         ("print   %10s value=0x%04x\n", r->name, r->value));
#endif /* RUNTIME_TRACE */
}

/**
   Write data from shadow register to the Vinetic.
\param
   pCh       - pointer to the channel structure
\param
   reg       - pointer to the register structure
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   None
\code
   wrReg(pCh, BCR1_REG);
\endcode
*/
IFX_int32_t wrReg (VINETIC_CHANNEL *pCh, IFX_void_t *reg)
{
   REG_TYPE *r = (REG_TYPE *) reg;
   IFX_int32_t      err;

   if (!((r->address == OPMOD_CUR_IOP) && ( r->regType == CMD1_IOP )))
   {
#ifdef RUNTIME_TRACE
         TRACE(VINETIC_RUNTIME_TRACE, DBG_LEVEL_NORMAL,
              ("write   %10s value=0x%04x\n", r->name, r->value));
#endif /* RUNTIME_TRACE */
      if (pCh->nChannel == 0) /* pDev */
      {
         err = RegWrite ((VINETIC_DEVICE *)pCh, r->regType,
                         &r->value, 1, r->address);
      }
      else
      {
         err = RegWrite (pCh->pParent,
                         (r->regType | (pCh->nChannel-1)),
                         &r->value, 1, r->address);
      }
   }
   else
   {
#ifdef RUNTIME_TRACE
      TRACE(VINETIC_RUNTIME_TRACE, DBG_LEVEL_NORMAL,
            ("write     SC_OPMOD value=0x%04x\n",
            CMD1_SC | CMD1_OM | r->value | (pCh->nChannel-1)));
#endif /* RUNTIME_TRACE */
      err = ScWrite  (pCh->pParent, CMD1_SC | CMD1_OM | r->value | (pCh->nChannel-1));
   }
   return err;
}

/**
   Read data from the Vinetic to the shadow register.
\param
   pCh       - pointer to the channel structure
\param
   reg       - pointer to the register structure
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   None
\code
   rdReg(pCh, BCR1_REG);
\endcode
*/
IFX_int32_t rdReg (VINETIC_CHANNEL *pCh, IFX_void_t *reg)
{
   REG_TYPE *r = (REG_TYPE *) reg;
   IFX_int32_t      err;

   if (pCh->nChannel == 0) /* pDev */
   {
      err = RegRead  ((VINETIC_DEVICE *) pCh, r->regType,
                     &r->value, 1, r->address);
   }
   else  /* pCh */
   {
      err = RegRead  (pCh->pParent, (r->regType | (pCh->nChannel-1)),
                     &r->value, 1, r->address);
   }
#ifdef RUNTIME_TRACE
   TRACE(VINETIC_RUNTIME_TRACE, DBG_LEVEL_NORMAL,
        ("%sread    %10s value=0x%04x%s\n", boldred, r->name, r->value, normal));
#endif /* RUNTIME_TRACE */
   return err;
}

/**
   Set the value of the shadow register coefficient.
\param
   pCh       - pointer to the channel structure
\param
   reg       - pointer to the register structure
\param
   val       - value
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   None
\code
   setRegVal(pCh, IK1, 15);  [mA in this case]
   wrReg(pCh, IK1_REG);
\endcode
*/
IFX_int32_t setRegVal (VINETIC_CHANNEL *pCh, IFX_void_t *reg, IFX_int32_t val)
{
   REG_TYPE *r = (REG_TYPE *) reg;

   if (r->setVal != NULL)
   {
      r->value = r->setVal (pCh->pParent, val);
#ifdef RUNTIME_TRACE
      TRACE(VINETIC_RUNTIME_TRACE, DBG_LEVEL_NORMAL,
              ("%sset     %10s value=0x%04X <= [%ld]%s\n",
               bold, r->name, r->value, val, normal));
#endif /* RUNTIME_TRACE */
   }
   else
   {
#ifdef RUNTIME_TRACE
      TRACE(VINETIC_RUNTIME_TRACE, DBG_LEVEL_HIGH,
              ("%sset     %10s value=UNDEFINED <= [%ld]  ERROR, r->setVal==NULL%s\n",
               boldred, r->name, val, normal));
#else
      TRACE(VINETIC, DBG_LEVEL_HIGH,
              ("%sERROR function pointer is NULL%s\n", boldred, normal));
#endif /* RUNTIME_TRACE */
   }
   if (r->value == 0xFFFF)
      return IFX_ERROR;

   return IFX_SUCCESS;
}

/**
   Get the value of the shadow register coefficient.
\param
   pCh       - pointer to the channel structure
\param
   reg       - pointer to the register structure
\return
   value of the shadow register coefficient
\remarks
   None
\code
   rdReg(pCh, IK1_REG);        [optional]
   val = getRegVal(pCh, IK1);  [returns the IK1 in mA]
\endcode
*/
IFX_int32_t getRegVal (VINETIC_CHANNEL *pCh, IFX_void_t *reg)
{
   REG_TYPE *r = (REG_TYPE *) reg;
   IFX_int32_t      val;

   if (r->getVal != NULL)
   {
      val = r->getVal (pCh->pParent, r->value);
#ifdef RUNTIME_TRACE
      TRACE(VINETIC_RUNTIME_TRACE, DBG_LEVEL_NORMAL,
            ("%sget     %10s value=0x%04X => [%ld]%s\n",
            bold, r->name, r->value, val, normal));
#endif /* RUNTIME_TRACE */
   }
   else
   {
#ifdef RUNTIME_TRACE
      TRACE(VINETIC_RUNTIME_TRACE, DBG_LEVEL_HIGH,
            ("%sget     %10s value=0x%04X => [UNDEFINED]  ERROR, r->getVal==NULL%s\n",
            boldred, r->name, r->value, normal));
#else
      TRACE(VINETIC, DBG_LEVEL_HIGH,
              ("%sERROR function pointer is NULL%s\n", boldred, normal));
#endif /* RUNTIME_TRACE */
      val = IFX_ERROR;
   }

   return val;
}

/**
   Store the register value to restore it later on...
\param
   pCh       - pointer to the channel structure
\param
   reg       - pointer to the register structure
\return
   IFX_ERROR or IFX_SUCCESS
\remarks
   None
\code
   storeRegVal(pCh, IK1_REG);
\endcode
*/
IFX_int32_t storeRegVal (VINETIC_CHANNEL *pCh, IFX_void_t *reg)
{
   REG_TYPE *r = (REG_TYPE *) reg;
   IFX_int32_t      err;

   err = RegRead  (pCh->pParent,
                  (r->regType | (pCh->nChannel-1)),
                  &r->value, 1, r->address);

   r->store = r->value;
#ifdef RUNTIME_TRACE
   TRACE(VINETIC_RUNTIME_TRACE, DBG_LEVEL_NORMAL,
        ("%sstore   %10s value=0x%04x%s\n", bold, r->name, r->value, normal));
#endif /* RUNTIME_TRACE */
   return err;
}

/**
   Restore the register value (if stored previously)
\param
   pCh       - pointer to the channel structure
\param
   reg       - pointer to the register structure
\return
   IFX_ERROR or IFX_SUCCESS
\remarks
   None
\code
   restoreRegVal(pCh, IK1_REG);
\endcode
*/
IFX_int32_t restoreRegVal (VINETIC_CHANNEL *pCh, IFX_void_t *reg)
{
   REG_TYPE *r = (REG_TYPE *) reg;
   IFX_int32_t      err;

   r->value = r->store;

   if (!((r->address == OPMOD_CUR_IOP) && ( r->regType == CMD1_IOP )))
   {
#ifdef RUNTIME_TRACE
      TRACE(VINETIC_RUNTIME_TRACE, DBG_LEVEL_NORMAL,
           ("%srestore %10s value=0x%04x%s\n", bold, r->name, r->value, normal));
#endif /* RUNTIME_TRACE */
      err = RegWrite (pCh->pParent,
                     (r->regType | (pCh->nChannel-1)),
                     &r->value, 1, r->address);
   }
   else
   {
#ifdef RUNTIME_TRACE
      TRACE(VINETIC_RUNTIME_TRACE, DBG_LEVEL_NORMAL,
            ("%srestore   SC_OPMOD value=0x%04x%s\n",
            bold, CMD1_SC | CMD1_OM | r->value | (pCh->nChannel-1), normal));
#endif /* RUNTIME_TRACE */
      err = ScWrite  (pCh->pParent, CMD1_SC | CMD1_OM | r->value | (pCh->nChannel-1));
   }

   return err;
}

/* {@ */
