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
   Module      : drv_vinetic_stream.c
   Date        : 2002-11-08
   Description :
      This file contains the implementation of the voice streaming.
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_vinetic_api.h"

#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)

#include "drv_vinetic_int.h"
#include "drv_vinetic_stream.h"

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */


/* timeout for mailbox packet read */
/* used from File and VoIP upstream functions */
#define MBX_RD_TIMEOUT 0xFFFF

#if (VIN_CFG_FEATURES& VIN_FEAT_FAX_T38)
   /* Defines for Fax Upstream/Downstream */
   /* end of modulation data */
   #define END_MODLN    0x8000
   /* end of demodulation data */
   #define END_DEMODLN  0x8000
#endif /* (VIN_CFG_FEATURES& VIN_FEAT_FAX_T38) */

/* ============================= */
/* Global variable definition    */
/* ============================= */

/* ============================= */
/* Global function declaration   */
/* ============================= */

/* ============================= */
/* Local function declaration    */
/* ============================= */

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* ============================= */
/* Local function definition     */
/* ============================= */

/* ============================= */
/* Global function definition    */
/* ============================= */

/*******************************************************************************
Description	:
   Write RTP or AAL packets to the VINETIC
Arguments:
   pCh    -  device control structure, must be valid
   buf    -  destination buffer
   count  -  size of data bytes to write
Return Value:
   Actual written bytes
Remarks:
   the mailbox access must be protected against tasks and interrupt
*******************************************************************************/
IFX_int32_t VoIP_DownStream (VINETIC_CHANNEL *pCh, const IFX_uint8_t* buf,
                             IFX_int32_t count)
{
   IFX_uint16_t nFIBXMS, *pData;
   VINETIC_DEVICE* pDev = pCh->pParent;
   IFX_int32_t err;
   /* specifies the count to write to the device */
   IFX_uint16_t wrcnt = (IFX_uint16_t)count;

   /* check free packet in-box size */
   err = ScRead (pDev, SC_RFIBXMS, &nFIBXMS, 1);
   if (err == IFX_ERROR)
   {
      return IFX_ERROR;
   }
   /* not enough space available in the mailbox : write 0 bytes */
   if ((nFIBXMS & FIBXMS_PBOX) < ((wrcnt >> 1) + (wrcnt % 2)))
   {
      return 0;
   }
   /* map buffer pointer */
   pData = buf;
   /* setup command header */
   pData[0] = CMD1_WR | (pCh->nChannel - 1);
   /* check payload type in case of RTP */
   if ((pDev->nEdspVers & ECMD_VERS_EDSP_PRT) == ECMD_VERS_EDSP_PRT_RTP)
   {
      /* for dynamic event payload type assignment */
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
      /* check payload. pData is aligned to network order, big endian */
      if ((ntohs(pData[2]) & RTP_PT) == pCh->pTapiCh->nEvtPT)
      {
         pData[0] |= CMD1_EVT;
      }
      else
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
      {
         pData[0] |= CMD1_VOP;
      }
   }
   else
   {
      /* for AAL no event support currently implemented */
      pData[0] |= CMD1_VOP;
   }
   /* set ODD bit if odd byte count */
   if (wrcnt & 0x01)
   {
      pData[1] = CMD2_ODD;
      wrcnt += 1;
   }
   /* set count to word */
   wrcnt = wrcnt >> 1;
   /* set count in words */
   pData[1] |= wrcnt;
   /* protect write access againts tasks and interrupts */
   IFXOS_MutexLock (pDev->mbxAcc);
   Vinetic_IrqLockDevice(pDev);
   err = pDev->write (pDev, pData, wrcnt + 2);
   /* release locks */
   Vinetic_IrqUnlockDevice(pDev);
   IFXOS_MutexUnlock (pDev->mbxAcc);
   if (err == IFX_SUCCESS)
   {
      return count;
   }
   return err;
}

/*******************************************************************************
Description	:
   Read RTP or AAL packets from the VINETIC
Arguments   :
   pCh   -  device control structure, must be valid
   buf      -  destination buffer
   count   -  maximum data bytes to read
Result		:
   Actual read bytes maybe zero or error code. Error will be returned in case
   of count is smaller then the actual read bytes. If zero is returned no bytes
   are currently in the buffer and the read function can be called again
   later.
   The Fifo access must be protected against interrupt.
*******************************************************************************/
IFX_int32_t VoIP_UpStream (VINETIC_CHANNEL *pCh,  IFX_uint8_t* buf,
                           IFX_int32_t count)
{
   IFX_uint32_t len = 0;
   PACKET *pPacket = NULL;
   VINETIC_DEVICE *pDev = pCh->pParent;

#ifdef DEBUG
   CHK_CH_MAGIC;
#endif /* DEBUG */
   /* lock interrupts, as fifo can be changed within interrupt routine */
   Vinetic_IrqLockDevice (pDev);
   /* check if there is data in read Fifo */
   if (Fifo_isEmpty (&pCh->rdFifo) == IFX_TRUE)
   {
      /* non-blocking read call */
      /* if there is no data, it returns immediately with a return value of 0  */
      if (pCh->nFlags & CF_NONBLOCK)
      {
         Vinetic_IrqUnlockDevice (pDev);
         return 0;
      }
      /* blocking read call */
      /* waits MBX_RD_TIMEOUT milliseconds for data to arrive */
      else
      {
         /* timeout has expired without arrival of data */
         if (IFXOS_WaitEvent_timeout(pCh->wqRead, MBX_RD_TIMEOUT) == IFX_ERROR)
         {
            Vinetic_IrqUnlockDevice (pDev);
            return 0;
         }
      }
   }
   /* read data from the fifo if already available or if
      there is data arrival within timeout in blocking read call*/
   /* now data should be available, otherwise timeout occured */
   IFXOS_ASSERT (Fifo_isEmpty (&pCh->rdFifo) == IFX_FALSE);
   pPacket = (PACKET*) Fifo_readElement (&pCh->rdFifo);
   IFXOS_ASSERT (pPacket != NULL);
   if (pPacket == NULL)
   {
      Vinetic_IrqUnlockDevice (pDev);
      return 0;
   }
   /* get the length in Bytes */
   len = (pPacket->cmd2 & CMD2_LEN);
   /* check if the ODD bit is set */
   if (pPacket->cmd2 & CMD2_ODD)
      len = 2 * len - 1;
   else
      len = (2 * len);
   if (len > (IFX_uint32_t)count)
   {
      SET_ERROR (ERR_FUNC_PARM);
      Vinetic_IrqUnlockDevice (pDev);
      return IFX_ERROR;
   }
   /* unmap data */
   IFXOS_CPY_KERN2USR (buf, pPacket->pData, (IFX_uint32_t)len);
   Vinetic_IrqUnlockDevice (pDev);

   return (IFX_int32_t)len;
}

#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
/*******************************************************************************
Description	:
   Write Fax data to VINETIC EDSP Fax Datapump
Arguments:
   pCh    -  channel control structure, must be valid
   buf    -  data buffer
   count  -  size of data bytes to write
Return Value:
   Actual written bytes when success
   error code when error
Remarks:
   -  This function is not blocking and is invoked by the user when the fax
      datapump requires more data.
   - This function just send data out without checkin if the user has the rights
     to invoke the function now. Therefore the user must be aware of what he is
     doing
   -  The mailbox access must be protected against tasks and interrupt.
      All shared variables with interrupt routine must also be protected.
*******************************************************************************/
IFX_int32_t Fax_DownStream (VINETIC_CHANNEL *pCh, const IFX_uint8_t* buf,
                            IFX_int32_t count)
{
   VINETIC_DEVICE* pDev = pCh->pParent;
   IFX_uint16_t* pFaxBuf;
   IFX_int32_t err = IFX_SUCCESS;

   /* data already copied from user space with space for 2 command words.
      In case of FAX they have been transmitted */
   pFaxBuf = (IFX_uint16_t *)&buf[4];
   /* write data on user request */
   /* check size of buffer */
   if (count > (MAX_PACKET_WORD << 1))
      count = MAX_PACKET_WORD << 1;
   /* protect write access againts tasks and interrupts  */
   IFXOS_MutexLock (pDev->mbxAcc);
   Vinetic_IrqLockDevice(pDev);
   err = pDev->write (pDev, pFaxBuf, ((IFX_uint32_t)count >> 1));
   /* release locks */
   Vinetic_IrqUnlockDevice(pDev);
   IFXOS_MutexUnlock (pDev->mbxAcc);
   /* check error */
   if (err != IFX_SUCCESS)
   {
      /* set error and issue tapi exception */
      TAPI_FaxT38_Event_Update (pCh->pTapiCh, 0, TAPI_FAX_T38_WRITE_ERR);
      TRACE (VINETIC, DBG_LEVEL_HIGH,
            ("Error in Modulation ch%d\n\r", (pCh->nChannel - 1)));
      return IFX_ERROR;
   }

   return count;
}

/*******************************************************************************
Description	:
   Read Fax data from VINETIC EDSP Fax Datapump
Arguments   :
   pCh   -  channel control structure, must be valid
   buf   -  destination buffer
   count -  maximum data bytes to read
Result		:
    Actual read bytes when success
    error code when error
Remarks     :
   - This function is now implemented non blocking and will be invoked every time
     there are new data in Fifo. Error Conditions are :
        - amount of data to read greater than max packet size (256 Words)
        - no fifo available or Fifo read ptr is null
        - size read > size awaited (error case)
   -  The Fifo access must be protected against interrupt.
      All shared variables with interrupt must also be protected.
*******************************************************************************/
IFX_int32_t Fax_UpStream (VINETIC_CHANNEL *pCh, IFX_uint8_t* buf,
                          IFX_int32_t count)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   PACKET         *pFaxPacket = NULL;
   IFX_uint32_t   len = 0, pos = 0;
   IFX_int32_t    err = IFX_SUCCESS;

   /* protect against interrupts because fifo is filled
      within the interrupt routine */
   Vinetic_IrqLockDevice (pDev);
   /* check if Fifo isn't empty. Must be protected from interrupts */
   if (Fifo_isEmpty (&pCh->rdFifo) == IFX_FALSE)
   {
      /* read data from fifo ... Interrupts schould be locked */
      pFaxPacket = (PACKET*) Fifo_readElement (&pCh->rdFifo);
      if (pFaxPacket != NULL)
      {
         /* get and check the length in Bytes */
         len = ((pFaxPacket->cmd2 & CMD2_LEN)* 2);
         if (len > (count - 4))
            len = (count - 4);
         /* copy commands and data to user space and save line status if  */
         IFXOS_CPY_KERN2USR (buf, (&pFaxPacket->cmd1), 4);
         pos += 4;
         IFXOS_CPY_KERN2USR (&buf[pos], pFaxPacket->pData, (IFX_uint32_t)len);
         len += pos;
         /* end of demodulation. user should stop the datapum */
         if (((pFaxPacket->cmd1 & ~0xF) == CMD1_FRD) &&
              (ntohs(pFaxPacket->pData [0]) & END_DEMODLN))
         {
            TAPI_FaxT38_Event_Update (pCh->pTapiCh, TAPI_FAX_T38_TX_OFF,
                                      TAPI_FAX_T38_NO_ERR);
         }
      }
      else
      {
         TRACE (VINETIC, DBG_LEVEL_HIGH,
               ("Error : No Fifo element, ch%d\n\r", (pCh->nChannel - 1)));
         err = IFX_ERROR;
      }
   }
   /* release interrupt lock */
   Vinetic_IrqUnlockDevice (pDev);
   /* check errors */
   if (err == IFX_ERROR)
   {
      /* set error and issue tapi exception */
      TAPI_FaxT38_Event_Update (pCh->pTapiCh, 0, TAPI_FAX_T38_READ_ERR);
      TRACE (VINETIC, DBG_LEVEL_HIGH,
            ("Error in Demodulation ch%d\n\r", (pCh->nChannel - 1)));
      return IFX_ERROR;
   }

   return len;
}
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38) */

/*******************************************************************************
Description	:
   get Data and Fifo handler for packet handling
Arguments   :
   pDev           -  device control structure, must be valid
   pPacketCmd     -  first 2 command words
   pFifo          -  Fifo handler
   pPacket        -  pointer to a buffer handler, currently not used
Result		:
   remaining length of the current packet to be read
Remarks:
   This function is only invoked within the interrupt and should not block.
*******************************************************************************/
IFX_uint8_t getPacketPtr (VINETIC_DEVICE *pDev, IFX_uint16_t *pPacketCmd, FIFO **pFifo,
                          PACKET **pPacket)
{
   IFX_uint8_t nCh = 0, nLen = 0;
   VINETIC_CHANNEL *pCh = NULL;

   /* read channel */
   nCh = pPacketCmd [0] & CMD1_CH;
   IFXOS_ASSERT (nCh < VINETIC_CH_NR);

   /* get channel pointer */
   pCh = &pDev->pChannel [nCh];
   /* read length */
   nLen = (pPacketCmd [1] & CMD2_LEN);
   /* get Buffer handler */
   if ((pPacketCmd [0] & CMD1_CIDRX) == CMD1_CIDRX)
   {
      *pPacket = (PACKET*)&pCh->cidRxPacket;
   }
   else
   {
      *pPacket = (PACKET*) Fifo_writeElement (&pCh->rdFifo);
   }
   if (*pPacket != NULL)
   {
      /* write first data into buffer */
      ((PACKET*)pPacket [0])->cmd1 = pPacketCmd [0];
      ((PACKET*)pPacket [0])->cmd2 = pPacketCmd [1];
   }
   /* in future : set fifo ptr here */
   /* *pFifo = (FIFO *)*pPacket; */

   return nLen;
}

/*******************************************************************************
Description	:
   Put Data into  Fifo
Arguments   :
   pDev      -  device control structure, must be valid
   nCmd1     -  1st read command including channel info
   pFifo     -  Fifo handler
   pPacket   -  Buffer pointer
Result		:
   none
Remarks     :
   This function sets the fifo pointer and wakes up reading queues
   waiting for data.
   For future use for different fifo implementations

   This function is only invoked within the interrupt and should not block.
*******************************************************************************/
IFX_void_t FifoPut (VINETIC_DEVICE *pDev, IFX_uint16_t nCmd1, FIFO **pFifo,
                    PACKET **pPacket)
{
   IFX_uint8_t nCh = 0;
   VINETIC_CHANNEL *pCh = NULL;

   /* read channel */
   nCh = nCmd1 & CMD1_CH;
   /* invalid channel number */
   if (nCh >= VINETIC_CH_NR)
      return;
   /* get channel pointer */
   pCh = &(pDev->pChannel [nCh]);
#ifdef TAPI_CID
   if ((nCmd1 & CMD1_CIDRX) == CMD1_CIDRX)
   {
      TAPI_LL_Phone_CidRxDataCollect (pCh->pTapiCh);
   }
   else
#endif /* TAPI_CID */
#ifdef QOS_SUPPORT
   if (pCh->QosCrtl.egressFlag == QOS_EGRESS_REDIR)
   {
      Qos_PktEgressSched ((IFX_uint32_t)pCh);
   }
   else
#endif /* QOS_SUPPORT */
   {
      if (!(pCh->nFlags & CF_NONBLOCK))
      {
         /* ok, data available, wake up waiting upstream function */
         IFXOS_WakeUpEvent (pCh->wqRead);
      }
      /* if a non-blocking read should be performed just wake up once */
      if (pCh->nFlags & CF_NEED_WAKEUP)
      {
         pCh->nFlags |= CF_WAKEUPSRC_STREAM;
         /* don't wake up any more */
         pCh->nFlags &= ~CF_NEED_WAKEUP;
         IFXOS_WakeUp (pCh->SelRead, IFXOS_READQ);
      }
   }
}

#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */


