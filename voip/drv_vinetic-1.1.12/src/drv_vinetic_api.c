/*       Copyright (c) 2003, Infineon Technologies.  All rights reserved.

                               No Warranty
   Because the program is licensed free of charge, there is no warranty for
   the program, to the extent permitted by applicable law.  Except when
   otherwise stated in writing the copyright holders and/or other parties
   provide the program "as is" without warranty of any kind, either
   expressed or implied, including, but not limited to, the implied
   warranties of merchantability and fitness for a particular purpose. The
   entire risk as to the quality and performance of the program is with
   you.  should the program prove defective, you assume the cost of all
   necessary servicing, repair or correction.

   In no event unless required by applicable law or agreed to in writing
   will any copyright holder, or any other party who may modify and/or
   redistribute the program as permitted above, be liable to you for
   damages, including any general, special, incidental or consequential
   damages arising out of the use or inability to use the program
   (including but not limited to loss of data or data being rendered
   inaccurate or losses sustained by you or third parties or a failure of
   the program to operate with any other programs), even if such holder or
   other party has been advised of the possibility of such damages.
 ****************************************************************************
   Module      : drv_vinetic_api.c

   This file contains the implementation of basic access
                 functions for VINETIC access.                         */

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
#include "drv_vinetic_host.h"
#include "drv_vinetic_api.h"
#include "drv_vinetic_main.h"

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

/* ============================= */
/* Local function declaration    */
/* ============================= */
IFX_LOCAL IFX_int32_t wait_coutbox_data(VINETIC_DEVICE *pDev, IFX_uint8_t *count);
IFX_LOCAL IFX_int32_t wait_cinbox_space(VINETIC_DEVICE *pDev, IFX_uint8_t count);

/* ============================= */
/* Local function definitions    */
/* ============================= */

/**
   Polls for sufficient amount of data in command-outbox until the data is
   available, or a maximum amount of time has expired.

\param
   pDev     - Handle of the device structure
\param
   *pCount  - Value/result argument. On function call, it contains the amount
              of command outbox data expected. In case this amount is available,
              upon function return it contains the current amount of data
              present in the command outbox.
\return
   IFX_SUCCESS if all or more data is available or IFX_ERROR
\remarks
   Polls the field RLEN of register BOX_CLEN until the *pCount amount of
   words  become available in the command-outbox. A maximum time of
   (WAIT_POLLTIME * OBXML_POLL_LOOP)us can be waited in the process. The VINETIC
   should answer within 500 us. After 5 ms this function signals a timeout.
   Timeout may occur in case of a HOST or CERR indicated in the mailbox
   register or the VINETIC does not answer.
   < !!! If an mailbox error occurs this function will also return an error >

   This function does not perform any protection against concurrent accesses
   (either interrupt or multi-task). It is an obligation to the calling function
   to provide such protection.
*/
IFX_LOCAL IFX_int32_t wait_coutbox_data(VINETIC_DEVICE *pDev, IFX_uint8_t *pCount)
{
   IFX_uint8_t nCmdOutSize;
   IFX_uint16_t nRegBoxClen;
   IFX_int32_t nWaitCnt = 0;

   do
   {
      REG_READ_UNPROT(pDev, V2CPE_BOX_CLEN, &nRegBoxClen);
      if ((nCmdOutSize = V2CPE_BOX_CLEN_RLEN_GET(nRegBoxClen)) >= *pCount)
      {
         /* sufficient amount of data available in command-outbox, write
          * the current amount to the caller */
         *pCount = nCmdOutSize;
         return IFX_SUCCESS;
      }
      /* not enough data available in command-outbox, we wait until data
       * arrives ot timeout occurs */
      IFXOS_DELAYUS(WAIT_POLLTIME);
   } while (++nWaitCnt < OBXML_POLL_LOOP);

   /* !!! TODO : The interrupt routine must check and set mailbox Errors */

   SET_ERROR(ERR_OBXML_ZERO);

   return IFX_ERROR;
}

/**
   Polls for adequate amount of command-inbox space within a certain time.
   The polling stops when space is available or the timeout expires.
\param
   pDev   - Pointer to the device structure
\param
   nCount  - Required space in words to wait for.
            No check is done on this parameter!
\return
   IFX_SUCCESS on success, IFX_ERROR in case of error.
   On error, ERR_NO_FIBXMS is logged.
\remarks
   Polls the field WLEN of register BOX_CLEN until the space of count words
   become available in the command-inbox. A maximum time of WAIT_POLLTIME *
   FIBXMS_POLL_LOOP can be waited in the process. If the requested command-inbox
   space does not become available withing this time, the function returns an error.
   The function returns a success value immediately if the desired command-inbox
   space is available.

   This function does not perform any protection against concurrent accesses
   (either interrupt or multi-task). It is an obligation to the calling function
   to provide such protection.
*/
IFX_LOCAL IFX_int32_t wait_cinbox_space(VINETIC_DEVICE *pDev, IFX_uint8_t nCount)
{
   IFX_int32_t nWaitCnt = 0;
   IFX_uint16_t nReg;

   do
   {
      REG_READ_UNPROT(pDev, V2CPE_BOX_CLEN, &nReg);
      if (V2CPE_BOX_CLEN_WLEN_GET(nReg) >= nCount)
      {
         /* sufficient free space in command-inbox, we carry on */
         return IFX_SUCCESS;
      }
      /* no enough free space in command-inbox, we wait until
       * space of 'nCount' words is available or timeout occurs */
      IFXOS_DELAYUS(WAIT_POLLTIME);
   } while ((++nWaitCnt < FIBXMS_POLL_LOOP) && (pDev->err == ERR_OK));

   /* exceeded maximum wait time, we reset command-inbox */
   nReg = 0;
   V2CPE_BOX_CMD_CI_RES_SET(nReg, 1);
   REG_WRITE_UNPROT(pDev, V2CPE_BOX_CMD, nReg);

   /* poll the BOX_CMD.CI_RES bit until cleared */
   do
   {
      REG_READ_UNPROT(pDev, V2CPE_BOX_CLEN, &nReg);
   } while (nReg & V2CPE_BOX_CMD_CI_RES);

   SET_ERROR(ERR_NO_FIBXMS);

   return IFX_ERROR;
}

/* ============================= */
/* Global function definitions   */
/* ============================= */

/**
   Write VINETIC Command
\param
   pDev  - pointer to the device interface
\param
   pCmd  - pointer to command buffer. It contains the 2 command words
           needed and the data to be written
\param
   nCount - number of data words to write, without command header. Must not be
            larger then maximum mailbox size
\return
   IFX_SUCCESS on success, else IFX_ERROR on error.
   Error can occur if command-inbox space is not available or if low level
   function/macros fail.
   The following error codes can be set: ERR_NO_FIBXMS.
\remarks
   The length information is coded in the second cmd word.
   For security purposes, only the fields RW, BC, CMD and CH are allowed in
   command header 1 (CMD1).
   Shared variables in this function must be protected against concurrent
   access by the caller.
   Protection against tasks and interrupts is done ONLY for written commands.
   So in case this function is used for Read Commands, Command Read Routine
   must do the protections.
*/
IFX_int32_t CmdWrite(VINETIC_DEVICE *pDev, IFX_uint16_t *pCmd, IFX_uint8_t nCount)
{
   IFX_int32_t err = IFX_SUCCESS;

   IFXOS_ASSERT(!(pCmd[0] & CMD1_BC));
   IFXOS_ASSERT(nCount <= (MAX_PACKET_WORD - CMD_HEADER_CNT));

   /* if read command issued just write command words */
   if ((pCmd[0] & CMD1_RD) == 0)
   {
     /* protect only write accesses against concurrent task access
       and VINETIC interrupt access */
      VIN_HOST_PROTECT (pDev);
      /* for write accesses clear and fill the length field */
      pCmd[1] = ((pCmd[1] & ~CMD2_LEN) | (IFX_uint16_t)nCount);
   }
   else
   {
      /* to request data (read) just write the command header */
      nCount = 0;
   }

   /* increase count with command header */
   nCount += CMD_HEADER_CNT;

   /* wait for free space in command inbox and write data
      Note: This action should not take too long, so that interrupts
            will not get lost. */
   if ((err = wait_cinbox_space(pDev, nCount)) == IFX_ERROR)
   {
      IFX_uint8_t i;
      /* polling for inbox space timedout, the command inbox was reset */
      TRACE(VINETIC, DBG_LEVEL_HIGH,("VIN%ld: no enough space in command "
            "inbox for following command:\n\r", pDev->nDevNr));
      for (i = 0; i < nCount; i++)
         TRACE(VINETIC, DBG_LEVEL_HIGH,("%d : 0x%04X\n\r", i, pCmd [i]));
   }
   else
   {
      V2CPE_CMD_MBX_SEM_TAKE(pDev);
      VIN_LL_UNPROT_CMD_MBX_WRITE (pDev, pCmd, nCount);
      V2CPE_CMD_MBX_SEM_GIVE(pDev);

      CHECK_HOST_ERR(pDev, err = IFX_ERROR);
   }
   /* release concurrent task and VINETIC interrupt protection
      for write commands only */
   if ((pCmd[0] & CMD1_RD) == 0)
      VIN_HOST_RELEASE (pDev);

#ifdef EVALUATION
   {
      IFX_uint8_t i;

      TRACE(VINETIC, DBG_LEVEL_LOW,("CmdWrite\n\r"));
      for (i = 0; i < nCount; i++)
         TRACE(VINETIC, DBG_LEVEL_LOW,("%d : 0x%04X\n\r", i, pCmd [i]));
   }
#endif /* EVALUATION */

   /* The following log macro only logs write commands i.e. !(pCmd[0] & CMD1_RD).
    * Read commands are assumed to be always written through function CmdRead(),
    * and they are logged in that function. */
   LOG_WR_CMD(pCmd, nCount, !err ? err : pDev->err);
   return err;
}

/**
   Write VINETIC command without blocking, intended for use within interrupt
   context or highest priority task only.
\param
   pDev   - pointer to the device interface
\param
   pCmd   - pointer to write command Buffer. It contains the 2 command words
            needed and the data to be written.
\param
   nCount - number of data to write, without command headers
\return
   IFX_ERROR if no command-inbox space available, otherwise IFX_SUCCESS
\remarks
   - The command words must be set properly before calling this function
     (length, read/write bit, ...).

   - No security checks are done on the command fields
     This function was intended to be called from interrupt or highest
     priority task context. If otherwise used, it must be protected from
     interrupts and concurrent task accesses.
*/
IFX_int32_t CmdWriteIsr (VINETIC_DEVICE *pDev, IFX_uint16_t* pCmd,
                         IFX_uint8_t nCount)
{
   IFX_uint16_t reg_box;

   nCount += CMD_HEADER_CNT;

   REG_READ_UNPROT(pDev, V2CPE_BOX_CLEN, &reg_box);
   if (pDev->err != ERR_OK)
   {
      return IFX_ERROR;
   }
   if (V2CPE_BOX_CLEN_WLEN_GET(reg_box) < nCount)
   {
      /* No space available and no time to poll, as we are in the
         highest priority task or in interrupt. */
      SET_ERROR (ERR_NO_FIBXMS);
      return IFX_ERROR;
   }

   V2CPE_CMD_MBX_SEM_TAKE(pDev);
   VIN_LL_UNPROT_CMD_MBX_WRITE (pDev, pCmd, nCount);
   V2CPE_CMD_MBX_SEM_GIVE(pDev);

   CHECK_HOST_ERR(pDev, return IFX_ERROR);

   return IFX_SUCCESS;
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
   nCount  - amount of data to read in words, without command header
\return
   IFX_SUCCESS on success, else IFX_ERROR on error
   Following errors may occur: ERR_NO_FIBXMS, ERR_OBXML_ZERO
\remarks
   Command Write routine is used to write the read command. In this case,
   the protection against interupts and concurent tasks is done by Command Read
   routine it self. Refer to Command Write routine for more details about
   writing commands.

   If the command write is successfull, command outbox is polled for an amount
   of data specified by nCount. If less or more data are available in the mailbox,
   IFX_ERROR is returned. Otherwise data are read from the command outbox.
*/
IFX_int32_t CmdRead(VINETIC_DEVICE *pDev, IFX_uint16_t *pCmd,
      IFX_uint16_t *pData, IFX_uint8_t nCount)
{
   IFX_int32_t err = IFX_SUCCESS;
   IFX_uint8_t nCnt;

   IFXOS_ASSERT(!(pCmd[0] & CMD1_BC));

   /* protect against concurrent task access and VINETIC interrupts */
   VIN_HOST_PROTECT(pDev);

   /* write read command : Only command header is written. */
   pCmd[0] |= CMD1_RD;
   /* clear and fill the length field */
   pCmd[1] = ((pCmd[1] & ~CMD2_LEN) | (IFX_uint16_t)nCount);

   /* increase count with command header */
   nCount += CMD_HEADER_CNT;
   if ((err = CmdWrite(pDev, pCmd, 0)) == IFX_ERROR)
   {
      /* release concurrent task and VINETIC interrupt protection */
      VIN_HOST_RELEASE(pDev);
      TRACE(VINETIC, DBG_LEVEL_HIGH, ("VIN%ld: writing read command fails\n\r",
            pDev->nDevNr));
      goto error;
   }
   nCnt = nCount;

   /* wait for data availability in command outbox and, read data.
      Note: This action should not take too long, so that interrupts
      will not get lost. */
   if ((err = wait_coutbox_data(pDev, &nCnt)) == IFX_SUCCESS)
   {
      if (nCnt > nCount)
      {
         err = IFX_ERROR;
         TRACE (VINETIC, DBG_LEVEL_HIGH, ("VIN%ld: more data in command outbox(%d) than expected(%d)\n\r",
               pDev->nDevNr, nCnt, nCount));
      }
      else
      {
         VIN_LL_UNPROT_CMD_MBX_READ(pDev, pData, nCount);
         if (pDev->err != ERR_OK)
	 {
            err = IFX_ERROR;
	 }
      }
   }
   else
   {
      /* polling for outbox data timedout */
      TRACE (VINETIC, DBG_LEVEL_HIGH, ("VIN%ld: expected read command data not available\n\r",
            pDev->nDevNr));
   }
   /* clear read flag to use it afterwards in CmdWrite */
   pData[0] &= ~CMD1_RW;
   /* release concurrent task and VINETIC interrupt protection */
   VIN_HOST_RELEASE(pDev);

#ifdef EVALUATION
   {
      IFX_uint8_t i;

      TRACE(VINETIC, DBG_LEVEL_LOW,("CmdRead\n\r"));
      for (i = 0; i < nCount; i++)
         TRACE(VINETIC, DBG_LEVEL_LOW,("%d : 0x%04X\n\r", i, pCmd [i]));
   }
#endif /* EVALUATION */

error:
   LOG_RD_CMD(pCmd, pData, nCount, !err ? err : (pDev->err ? pDev->err : err));
   return err;
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
   later on, when COBX event occurs, it is checked whether the read command is
   the one awaited, and if yes, the command is stored in the device or channel
   structure for a later use.
   Two applications of this are:
      - Coder Switching AAL (on SRE2 [DEC_CHG])
      - RTCP statistics
   Every time a command is requested from interrupt context without waitin on
   it, in pDev->nMbxState DS_CMDREQ is set. DS_RDCMD_SENT indicates that
   CmdRead itself requested data.
   After reading and dispatching the data, the flag DS_CMDREQ  will be cleared
   and depending on a DS_RDCMD_SENT the flag DS_CMDREAD is set, to indicate
   that probably the requested data is available in the device mailbox
   !! TODO: measure time spent in this function !!
*/
IFX_void_t  CmdReadIsr(VINETIC_DEVICE *pDev)
{
   IFX_uint16_t nBoxClen;
   IFX_uint8_t nSize = 0, nPos = 0;
   IFX_int32_t i;

   for (;;)
   {
      REG_READ_UNPROT(pDev, V2CPE_BOX_CLEN, &nBoxClen);
      if ((nSize = V2CPE_BOX_CLEN_RLEN_GET(nBoxClen)))
      {
         VIN_LL_UNPROT_READ_MULTI(pDev, V2CPE_BOX_CDATA, pDev->pCmdData + nPos, nSize);
         if (pDev->err != ERR_OK)
         {
            TRACE(VINETIC, DBG_LEVEL_HIGH,
               ("VIN%ld: error while reading command outbox\n\r", pDev->nDevNr));
            return;
         }
         nPos += nSize;
      }
      else
         break;
   }

   nSize = nPos;
   if (nSize)
   {
      pDev->nMbxState &= ~DS_CMDREQ;
      pDev->nCmdReadCnt = nSize;
      /* we read the data, in one case when CmdRead also expects and
         the data is not already in the outobox this field may not be zero */
      pDev->nExpCmdData -= nSize;
      /* if CmdRead already sent a command, now we probably read it out */
      if (pDev->nMbxState & DS_RDCMD_SENT)
      {
         pDev->nMbxState |= DS_CMDREAD;
      }
   }
   /* parse buffer and dispatch message accordingly */
   i = 0;
   while (i < nSize)
   {
      /* there is something of interest, which was requested before */
      VINETIC_DispatchCmd(pDev, &pDev->pCmdData[i]);
      /* set to next command */
      i += (pDev->pCmdData[i + 1] & CMD2_LEN) + CMD_HEADER_CNT;
   }
}
#endif /* ASYNC_CMD */

/* }@ */
