/****************************************************************************
     Copyright (c) 2005, Infineon Technologies.  All rights reserved.

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
 ******************************************************************************
   Module      : drv_vinetic_int.c
   Description : This file contains the implementation of the interrupt handler
   Remarks     : The implementation assumes that multiple instances of this
                 interrupt handler cannot preempt each other, i.e. if there
                 is more than one Vinetic in your design all Vinetics are
                 expected to raise interrupts at the same priority level.
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_vinetic_api.h"
#include "drv_vinetic_hostapi.h"
#include "drv_vinetic_tapi.h"
#include "drv_vinetic_dcctl.h"
#include "drv_vinetic_cid.h"
#include "drv_vinetic_tone.h"

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */
/* Mask for voice mailbox events */
/*#define V2CPE_STAT_INT_VB_EVTS                         \
      (V2CPE_STAT_INT_VO_RDY | V2CPE_STAT_INT_VI_EMP) */
/* Mask for command mailbox events */
#define V2CPE_STAT_INT_CB_EVTS                           \
      (V2CPE_STAT_INT_CO_RDY | V2CPE_STAT_INT_CO_DATA |  \
       V2CPE_STAT_INT_CI_EMP | V2CPE_STAT_INT_CI_DATA)

/* ============================= */
/* Global function declarations  */
/* ============================= */

/* ============================= */
/* Local function declarations   */
/* ============================= */
#ifdef ASYNC_CMD
IFX_LOCAL IFX_void_t serve_command_box_events(VINETIC_DEVICE *pDev, IFX_uint16_t nStat_Int);
#endif
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
IFX_LOCAL IFX_void_t serve_voice_outbox_data(VINETIC_DEVICE *pDev);
#endif
IFX_LOCAL IFX_void_t serve_channel_events(VINETIC_DEVICE *pDev);
IFX_LOCAL IFX_void_t serve_LineX_Int(VINETIC_CHANNEL *pCh, IFX_uint16_t nLineX_Int);
IFX_LOCAL IFX_void_t serve_EDSPx_Int1(VINETIC_CHANNEL *pCh, IFX_uint16_t nEDSPx_Int1,
      IFX_uint16_t nEDSPx_Stat1);
IFX_LOCAL IFX_void_t serve_EDSPx_Int2(VINETIC_CHANNEL *pCh, IFX_uint16_t nEDSPx_Int2,
      IFX_uint16_t nEDSPx_Stat2);
IFX_LOCAL IFX_void_t serveErr_Int(VINETIC_DEVICE *pDev);

/* ============================= */
/* Local variable definitions    */
/* ============================= */
#ifdef ASYNC_CMD
/* 'Coder Channel Decoder Status' command used to clear status bit
 * EDSPx_STAT2.DEC_CHG */
IFX_LOCAL IFX_uint16_t pCmd_CodChDecStat[2] = { CMD1_EOP, ECMD_COD_CHDECSTAT | 0x1 };
#endif
/* 'Status Error Acknowledge' command used to clear status bits,
 * EDSPx_STAT2.[ETU_OF, PVPU_OF, DEC_ERR] */
IFX_LOCAL IFX_uint16_t pCmd_StatErrAck[3] = { CMD1_EOP, ECMD_ERR_ACK, 0 };
/* CERR Acknowledgement */
IFX_LOCAL IFX_uint16_t pCerr [2] = { CMD1_EOP, ECMD_CMDERR_ACK };
/* MIPS Overload Acknowledgement */
IFX_LOCAL IFX_uint16_t pMipsOl [3] = { CMD1_EOP, ECMD_POW_CTL, 0x1 };

/* ============================= */
/* Global function definitions   */
/* ============================= */
/*******************************************************************************
Description :
   VINETIC device interrupt service routine
Arguments   :
   *pDev       - pointer to device data structure
Return      :
   none
Remarks     :
*******************************************************************************/
IFX_void_t VINETIC_interrupt_routine(VINETIC_DEVICE *pDev)
{
   IFX_uint16_t nStat_Int;

   do
   {
      /* Read interrupt status register */
      REG_READ_UNPROT(pDev, V2CPE_STAT_INT, &nStat_Int);
      CHECK_HOST_ERR(pDev, return);

      if (!nStat_Int)
         return;

      /* Checking first if any system error events have been reported. Under
       * normal circumstances we no not expect any. */
      if (nStat_Int & V2CPE_STAT_INT_ERR)
         serveErr_Int(pDev);

#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
      /* Checking for voice outbox events, and serving if any. Incoming voice
       * packets are most frequent interrupt events, therefore we check for
       * them before anything else. */
      if ((nStat_Int & V2CPE_STAT_INT_VO_DATA) &&
            !(pDev->IrqPollMode & VIN_VOICE_POLL))
         serve_voice_outbox_data(pDev);
#endif /* VIN_CFG_FEATURES & VIN_FEAT_PACKET */

      /* Serving of non-voice box events */
      if (nStat_Int & (V2CPE_STAT_INT_CB_EVTS |
                     V2CPE_STAT_INT_CH |
                     V2CPE_STAT_INT_CRC_RDY |
                     V2CPE_STAT_INT_CRC_ERR |
                     V2CPE_STAT_INT_GPIO))
      {
#ifdef ASYNC_CMD
         /* Serving of command box events */
         if (nStat_Int & V2CPE_STAT_INT_CB_EVTS)
            serve_command_box_events(pDev, nStat_Int);
#endif /* ASYNC_CMD */
         /* Serving of ALM and EDSP events */
         if (nStat_Int & V2CPE_STAT_INT_CH)
            serve_channel_events(pDev);

         /* Check for CRC_RDY interrupt, mask it afterwards and enable
            CRC_ERR interrupt */
         if (nStat_Int & V2CPE_STAT_INT_CRC_RDY)
         {
            IFX_uint16_t nReg;
            /* check CRC_ERR Status */
            REG_READ_UNPROT(pDev, V2CPE_STAT, &nReg);
            CHECK_HOST_ERR(pDev, return);
            if (nReg & V2CPE_STAT_CRC_ERR)
            {
               /* show error */
               SET_ERROR(ERR_FWCRC_FAIL);
               VIN_REPORT_EVENT(pDev, V2CPE_DEV_STAT_INT,
                                V2CPE_STAT_INT_CRC_ERR, V2CPE_STAT_INT_CRC_ERR);
               TRACE(VINETIC, DBG_LEVEL_HIGH,
                    ("VIN%ld: IRQ: EDSP CRC RDY and STAT error, "
                     "STAT_INT=0x%04X, STAT=0x%04X!\n\r",
                     pDev->nDevNr,
                     nStat_Int,
                     nReg));
            }

            /* ... and unmask the CRC_ERR interrupt,
                         mask the CRC_RDY interrupt */
            V2CPE_STAT_IEN_CRC_RDY_SET(pDev->hostDev.nRegStatIen, 0);
            V2CPE_STAT_IEN_CRC_ERR_SET(pDev->hostDev.nRegStatIen, 1);
            REG_WRITE_UNPROT(pDev, V2CPE_STAT_IEN, pDev->hostDev.nRegStatIen);
            CHECK_HOST_ERR(pDev, return);
         }

         /* Check for CRC error upon firmware download end */
         if (nStat_Int & V2CPE_STAT_INT_CRC_ERR)
         {
            V2CPE_STAT_IEN_CRC_ERR_SET(pDev->hostDev.nRegStatIen, 0);
            REG_WRITE_UNPROT(pDev, V2CPE_STAT_IEN, pDev->hostDev.nRegStatIen);
            CHECK_HOST_ERR(pDev, return);

            /* show error */
            SET_ERROR(ERR_FWCRC_FAIL);
            VIN_REPORT_EVENT(pDev, V2CPE_DEV_STAT_INT,
                             V2CPE_STAT_INT_CRC_ERR, V2CPE_STAT_INT_CRC_ERR);
            TRACE(VINETIC, DBG_LEVEL_HIGH,
                 ("VIN%ld: IRQ: EDSP CRC error after CRC ready, "
                  "masking CRC_ERR (STAT_IEN 0x%04X)!\n\r",
                  pDev->nDevNr, pDev->hostDev.nRegStatIen));
         }
	
         /* Serving general purpose IO pins events */
         if (nStat_Int & V2CPE_STAT_INT_GPIO)
         {
            IFX_uint16_t nIO_Int;

            REG_READ_UNPROT(pDev, V2CPE_IO_INT, &nIO_Int);
            CHECK_HOST_ERR(pDev, return);

            VINETIC_GpioIntDispatch((IFX_int32_t)pDev, nIO_Int);
#ifdef EVALUATION
            {
               IFX_uint16_t nIO_in;
               REG_READ_UNPROT(pDev, V2CPE_IO_IN, &nIO_in);
               CHECK_HOST_ERR(pDev, return);
               Eval_serveSRGPIOEvt (pDev, nIO_Int, nIO_in);
            }
#endif /* EVALUATION */
            /* write IO_INT register with 0 to signal FW to update
               the register */
            REG_WRITE_UNPROT(pDev, V2CPE_IO_INT, 0);
            CHECK_HOST_ERR(pDev, return);
         }
         /* report WOKE_UP event */
         VIN_REPORT_EVENT(pDev, V2CPE_DEV_STAT_INT,
                          (nStat_Int & V2CPE_STAT_INT_WOKE_UP),
                          V2CPE_STAT_INT_WOKE_UP);
      }

      /* download ready event (V2CPE_STAT_DL_RDY) is polled,
       * no handling necessary */
   } while (1);

}

/*******************************************************************************
Description :
   Function issues a command to reset the voice outbox and waits for it
   until completed.
Arguments   :
   *pDev       - pointer to device data structure
Return      :
   none
Remarks     :
*******************************************************************************/
IFX_void_t reset_voice_outbox(VINETIC_DEVICE *pDev)
{
   IFX_uint16_t nRegBoxCmd;

   REG_WRITE_UNPROT(pDev, V2CPE_BOX_CMD, V2CPE_BOX_CMD_VO_RES);
   do
   {
      REG_READ_UNPROT(pDev, V2CPE_BOX_CMD, &nRegBoxCmd);

   } while (V2CPE_BOX_CMD_VO_RES_GET(nRegBoxCmd));
}

/* ============================= */
/* Local function definitions    */
/* ============================= */
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
/*******************************************************************************
Description :
   Serves voice outbox data event, STAT_INT:VO_DATA
Arguments   :
   *pDev       - pointer to device data structure
Return      :
   none
Remarks     :
   The following function handles only STAT_INT.VO_DATA interrupt. On occurance
   of this interrupt, the voice outbox is scanned until there are no more data
   in it. Incomming voice packets are read and placed into the specific
   channel's FIFO's.
*******************************************************************************/
IFX_LOCAL IFX_void_t serve_voice_outbox_data(VINETIC_DEVICE *pDev)
{
   IFX_uint16_t nRegBoxVlen, nVoutboxCnt;
   IFX_uint8_t nCh = 0, nLen;
   VINETIC_CHANNEL *pCh;
   IFX_uint16_t pPktCmd[CMD_HEADER_CNT]; /* packet command header container */
   PACKET *pPkt; /* ptr to packet container */


   /* read voice box data count register (VBOX_CNT) */
   REG_READ_UNPROT(pDev, V2CPE_BOX_VLEN, &nRegBoxVlen);
   CHECK_HOST_ERR(pDev, return);

   /* get voice outbox data count in words, exit if none */
   if ((nVoutboxCnt = V2CPE_BOX_VLEN_RLEN_GET(nRegBoxVlen)) == 0)
      return;

   do
   {
         /* read packet command header, consisting of two words */
         VIN_LL_UNPROT_VOICE_MBX_READ (pDev, pPktCmd, CMD_HEADER_CNT);
         CHECK_HOST_ERR(pDev, return);

         nLen = pPktCmd[1] & CMD2_LEN; /* get packet length */
         nCh  = pPktCmd[0] & CMD1_CH;  /* get packet channel number */

         /* sanity check channel number */
         if (nCh >= VINETIC_2CPE_MAX_EDSP)
         {
            TRACE(VINETIC,DBG_LEVEL_HIGH,
                 ("VIN%ld, IRQ: corrupt voice outbox, outbox reseted"
                  " (Cmd1 : 0x%04X Cmd2: 0c%04X)\n\r",
                  pDev->nDevNr, pPktCmd[0], pPktCmd[1]));
            reset_voice_outbox(pDev);
            return;
         }
         /* if channel number is valid, get the channel context */
         pCh = &pDev->pChannel[nCh];

#ifdef TAPI_CID
         /* get the next available packet buffer */
         if ((pPktCmd[0] & CMD1_CIDRX) == CMD1_CIDRX)
         {
            pPkt = (PACKET *)&pCh->cidRxPacket;
         }
         else
#endif /* TAPI_CID */
         {
            pPkt = (PACKET *)Fifo_writeElement(&pCh->rdFifo);

            /* no packet buffers available */
            if (pPkt == NULL)
            {
               PACKET Pkt;

               /* dummy read of the rest of the packet */
               VIN_LL_UNPROT_VOICE_MBX_READ (pDev, &Pkt.pData[0], nLen);
               CHECK_HOST_ERR(pDev, return);

               return;
            }
         }

         /* copy packet command header into packet buffer */
         pPkt->cmd1 = pPktCmd[0];
         pPkt->cmd2 = pPktCmd[1];

         if (nLen)
         {
            /* read the rest of the packet into packet buffer */
            VIN_LL_UNPROT_VOICE_MBX_READ (pDev, pPkt->pData, nLen);
            CHECK_HOST_ERR(pDev, return);
         }

#ifdef TAPI_CID
         /* handle CID packet */
         if ((pPktCmd[0] & CMD1_CIDRX) == CMD1_CIDRX)
         {
            TAPI_LL_Phone_CidRxDataCollect (pCh->pTapiCh);
         }
#endif /* TAPI_CID */

      /* read voice box data count register (VBOX_CNT) */
      REG_READ_UNPROT(pDev, V2CPE_BOX_VLEN, &nRegBoxVlen);
      CHECK_HOST_ERR(pDev, return);

      /* get voice outbox data count in words, exit if none */
      nVoutboxCnt = V2CPE_BOX_VLEN_RLEN_GET(nRegBoxVlen);
   } while (nVoutboxCnt);


   /* reaching this point, we should have received packets and now need to
      wakeup further processing
   */
#ifdef QOS_SUPPORT
   /* schedule egress packet forwarding for packet redirection
    * at a safe time */
   if (pCh->QosCrtl.egressFlag == QOS_EGRESS_REDIR)
   {
      Qos_PktEgressSched ((IFX_uint32_t)pCh);
   }
   else
#endif /* QOS_SUPPORT */
   {
         if (!(pCh->nFlags & CF_NONBLOCK))
         {
            /* data available, wake up waiting upstream function */
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

#ifdef ASYNC_CMD
/*******************************************************************************
Description :
   Serves command data events
Arguments   :
   *pDev       - pointer to device data structure
   nStat_Int   - interrupt status register value (STAT_INT)
Return      :
   none
Remarks     :
   The following function currently handles only STAT_INT.CO_DATA interrupts.
*******************************************************************************/
IFX_LOCAL IFX_void_t serve_command_box_events(VINETIC_DEVICE *pDev, IFX_uint16_t nStat_Int)
{
   if (nStat_Int & V2CPE_STAT_INT_CO_DATA) /* command outbox data available */
   {
      if (pDev->nMbxState & DS_CMDREQ)
      {
         CmdReadIsr(pDev);
      }
   }
}
#endif /* ASYNC_CMD */

/*******************************************************************************
Description:
   serves ALM and EDSP events
Arguments:
   *pDev    - pointer to the device structure
Return   :
   none
Remarks  :
*******************************************************************************/
IFX_LOCAL IFX_void_t serve_channel_events(VINETIC_DEVICE *pDev)
{
   VINETIC_CHANNEL *pCh;
   IFX_uint16_t nStat_Ch, nEvt_Chs, nOffset;
   int i;

   REG_READ_UNPROT(pDev, V2CPE_STAT_CHAN, &nStat_Ch);
   CHECK_HOST_ERR(pDev, return);

   /* check for ALM events (all channels), and serve if any */
   if (nStat_Ch & V2CPE_STAT_CHAN_LINE_MASK)
   {
      /* line interrupt status register (one per ALM channel) */
      IFX_uint16_t nLineX_Int;

      /* get bitmask of channel(s) where ALM events occured */
      nEvt_Chs = (nStat_Ch & V2CPE_STAT_CHAN_LINE_MASK) >> 8;
      for (i = 0, nOffset = 0; i < VINETIC_MAX_ANA_CH_NR; i++, nOffset += 2)
      {
         if (nEvt_Chs & (1 << i))
         {
            pCh = &pDev->pChannel[i];

            REG_READ_UNPROT(pDev, V2CPE_LINE1_INT + nOffset, &nLineX_Int);
            CHECK_HOST_ERR(pDev, return);

            serve_LineX_Int(pCh, nLineX_Int);
         }
      }
   }

   /* check for EDSP events (all channels), and serve if any */
   if (nStat_Ch & V2CPE_STAT_CHAN_EDSP_MASK)
   {
      IFX_uint16_t nEdspX_Int[2];   /* EDSP interrupt status registers (two per EDSP channel) */
      IFX_uint16_t nEdspX_Stat[2];  /* EDSP status registers (two per EDSP channel) */

      /* get bitmask of channel(s) where EDSP events occured */
      nEvt_Chs = nStat_Ch & V2CPE_STAT_CHAN_EDSP_MASK;
      for (i = 0, nOffset = 0; i < VINETIC_2CPE_MAX_EDSP; i++, nOffset += 4)
      {
         if (nEvt_Chs & (1 << i))
         {
            pCh = &pDev->pChannel[i];

            REG_READ_UNPROT_MULTI(pDev, V2CPE_EDSP1_INT1 + nOffset, &nEdspX_Int[0], 2);
            CHECK_HOST_ERR(pDev, return);
            REG_READ_UNPROT_MULTI(pDev, V2CPE_EDSP1_STAT1 + nOffset, &nEdspX_Stat[0], 2);
            CHECK_HOST_ERR(pDev, return);

            if (nEdspX_Int[0])
            {
               serve_EDSPx_Int1(pCh, nEdspX_Int[0], nEdspX_Stat[0]);
            }
            if (nEdspX_Int[1])
            {
               serve_EDSPx_Int2(pCh, nEdspX_Int[1], nEdspX_Stat[1]);
            }
         }
      }
   }
}

/*******************************************************************************
Description :
   handles ALM interrupts reported through register LINEx_INT
Arguments   :
   pCh         - pointer to the channel structure
   nLineX_Int  - value of LINEx_INT register
Return      :
   none
Remarks     :
   This function serves phone events interrupts. Therefore, in case of tapi,
   an instance to data channel structure must be used for operation on
   data channel variables.
*******************************************************************************/
IFX_LOCAL IFX_void_t serve_LineX_Int(VINETIC_CHANNEL *pCh,
                                     IFX_uint16_t nLineX_Int)
{
#ifdef TAPI_CID
   VINETIC_CHANNEL *pCidCh;
   TAPI_CONNECTION *pTapiDataCh;
   IFX_uint8_t      data_ch;
#endif /* TAPI_CID */
   VINETIC_DEVICE *pDev = pCh->pParent;

#ifdef TAPI_CID
   /* get tapi data channel for operation on data channel */
   TAPI_Phone_Get_Data_Channel (pCh->pTapiCh, &data_ch);
   pTapiDataCh = &pCh->pTapiCh->pTapiDevice->pChannelCon [data_ch];
   pCidCh      = &((VINETIC_DEVICE *)pTapiDataCh->pDevice)->pChannel[data_ch];
#endif /* TAPI_CID*/

   /* serve LINEx_INT.ONHOOK/OFFHOOK events */
   if (nLineX_Int & (V2CPE_LINE1_INT_ONHOOK | V2CPE_LINE1_INT_OFFHOOK))
   {
      /* Stop DTMF transmission on any hook event */
      if (pCh->dtmfSend.nWords != 0)
      {
         VINETIC_DtmfStop(pCh, IFX_TRUE);
      }
      /* serve LINEx_INT.OFFHOOK event */
      if (nLineX_Int & V2CPE_LINE1_INT_OFFHOOK)
      {
#ifdef TAPI_CID
         /* for ongoing cid transmisson, call cid machine to handle off hook
            in case of CID type 1 (not applicable for NTT standard!) */
         if ((pCidCh->cidSend.nState == CID_TRANSMIT) &&
             (pCidCh->cidSend.nCidType == IFX_TAPI_CID_HM_ONHOOK) &&
             (pTapiDataCh->TapiCidConf.nStandard != IFX_TAPI_CID_STD_NTT)
         )
         {
            pCidCh->cidSend.nState = CID_OFFHOOK;
            VINETIC_CidFskMachine (pCidCh);
         }
#endif /* TAPI_CID */
      }
#ifdef TAPI
#ifdef TAPI_CID
      /* in case an CID onhook NTT is ongoing, this offhook is the aknowledge
         to the NTT alert routine  (CAR_NTT) */
      if ((nLineX_Int & V2CPE_LINE1_INT_OFFHOOK)                        &&
          (pTapiDataCh->TapiCidConf.nStandard == IFX_TAPI_CID_STD_NTT) &&
          (pTapiDataCh->TapiCidTx.bActive == IFX_TRUE)                 &&
          (pTapiDataCh->TapiCidTx.bAck == IFX_FALSE))
      {
         pTapiDataCh->TapiCidTx.bAck = IFX_TRUE;
      }
      else
#endif /* TAPI_CID */
      {
         /* raise TAPI exception */
         TAPI_Phone_Event_HookState(pCh->pTapiCh,
                                    ((nLineX_Int & V2CPE_LINE1_INT_OFFHOOK) ?
                                     IFX_TRUE : IFX_FALSE));
      }
#endif /* TAPI */
   }

   /* serve LINEx_INT.OVERTEMP event */
   if (nLineX_Int & V2CPE_LINE1_INT_OTEMP)
   {
      CMD_OpMode_t   opmod;

      memset (&opmod, 0, sizeof (CMD_OpMode_t));
      opmod.RW      = VIN_CMD_WR;
      opmod.CMD     = VIN_CMD_ALM;
      opmod.MOD     = VIN_MOD_DCCTL;
      opmod.ECMD    = VIN_ECMD_OPMOD;
      opmod.CH      = pCh->nChannel -1;
      opmod.LENGTH  = 1;
      opmod.REV_POL = VIN_OPMOD_POL_NORMAL;
      opmod.HOWLER  = VIN_OPMOD_HOWLER_OFF;
      opmod.OP_MODE = VIN_OPMOD_PDH;

      CmdWriteIsr (pDev, (IFX_uint16_t *)((IFX_void_t *) &opmod), 1);

#ifdef TAPI
      TAPI_Phone_Event_Otemp(pCh->pTapiCh);
#endif

      TRACE(VINETIC, DBG_LEVEL_HIGH,
           ("VIN%ld, Ch%d: IRQ: WARNING: overtemperature,"
            " setting line to power down\n\r",
            pDev->nDevNr, (pCh->nChannel -1) ));
   }

#if (VIN_CFG_FEATURES & VIN_FEAT_GR909)
   /* serve LINEx_INT.LTEST_FIN for GR909 line testing */
   if (nLineX_Int & V2CPE_LINE1_INT_LTEST_FIN)
   {
      TAPI_Phone_Event_Gr909 (pCh->pTapiCh);
   }
#endif /* VIN_CFG_FEATURES & VIN_FEAT_GR909 */

   VIN_REPORT_EVENT(pCh, V2CPE_CH_LINE_INT, nLineX_Int, nLineX_Int);
}

/*******************************************************************************
Description :
   handles EDSP interrupts reported through register EDSPx_INT1

Arguments   :
   pCh            - pointer to the channel structure
   nEDSPx_Int1    - value of EDSPx_INT1 register
   nEDSPx_Stat1   - value of EDSPx_STAT1 register

Return      :
   none
Remarks     :
*******************************************************************************/
IFX_LOCAL IFX_void_t serve_EDSPx_Int1(VINETIC_CHANNEL *pCh,
                                      IFX_uint16_t nEDSPx_Int1,
                                      IFX_uint16_t nEDSPx_Stat1)
{
#ifdef TAPI_DTMF
   IFX_uint8_t nKey = 0;
   VINETIC_DEVICE *pDev = pCh->pParent;
#endif

   /* DTMF transmitter events handling */
   if (nEDSPx_Int1 & (V2CPE_EDSP1_INT1_DTMFG_BUF |
                      V2CPE_EDSP1_INT1_DTMFG_REQ |
                      V2CPE_EDSP1_INT1_DTMFG_ACT))
   {
      /* DTMF generator activated */
      if (nEDSPx_Int1 & V2CPE_EDSP1_INT1_DTMFG_ACT)
      {
#ifdef TAPI
         if (pCh->pTapiCh->TapiComplexToneData[0].nToneState == TAPI_CT_ACTIVE)
         {
            /* utg resource is #0 */
            Tone_EventUTG_Deactivated(pCh->pTapiCh, 0);
         }
         else
#endif /* TAPI */
         {
            if (nEDSPx_Stat1 & V2CPE_EDSP1_STAT1_DTMFG_ACT)
               VINETIC_DtmfOnActivate(pCh, IFX_TRUE);
            else
               VINETIC_DtmfOnActivate(pCh, IFX_FALSE);
         }
      }
      /* DTMF generator (transmitter) underrun */
      if (nEDSPx_Int1 & V2CPE_EDSP1_INT1_DTMFG_BUF)
      {
         VINETIC_DtmfOnUnderrun(pCh);
      }
      /* DTMF generator data request (low data in transmitter) */
      if (nEDSPx_Int1 & V2CPE_EDSP1_INT1_DTMFG_REQ)
      {
         VINETIC_DtmfOnRequest(pCh);
      }
   }

#ifdef TAPI_DTMF
   /* DTMF key detected or probably detected */
   if (nEDSPx_Int1 & (V2CPE_EDSP1_INT1_DTMFR_DT |
                    V2CPE_EDSP1_INT1_DTMFR_PDT))
   {
      /* valid DTMF key detected */
      if (nEDSPx_Int1 & V2CPE_EDSP1_INT1_DTMFR_DT)
      {
         /* read detected key */
         nKey = V2CPE_EDSP1_STAT1_DTMFR_DTC_GET(nEDSPx_Stat1);
         /* call TAPI events for analog channels only */
         if ((pCh->nChannel - 1) < pDev->nSigCnt)
         {
#ifdef TAPI_CID
            if ((pCh->pTapiCh->TapiCidTx.bActive == IFX_TRUE) &&
                (pCh->pTapiCh->TapiCidTx.ackToneCode == nKey))
            {
               pCh->pTapiCh->TapiCidTx.bAck = IFX_TRUE;
            }
            else
#endif /* TAPI_CID */
            {
               VINETIC_Tapi_Event_Dtmf(pCh->pTapiCh, nKey);
            }
         }
      }
   }
#endif /* TAPI_DTMF */

#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   /* Universal tone detection receive */
   if (nEDSPx_Int1 & V2CPE_EDSP1_INT1_UTD1_OK)
   {
      TAPI_Signal_Event_Update(pCh->pTapiCh, pCh->nUtdSig[0]);
   }

   /* Universal tone detection transmit */
   if (nEDSPx_Int1 & V2CPE_EDSP1_INT1_UTD2_OK)
   {
      TAPI_Signal_Event_Update(pCh->pTapiCh, pCh->nUtdSig[1]);
   }
   /* Call progress tone detected */
   if (nEDSPx_Int1 & V2CPE_EDSP1_INT1_CPT)
   {
      TAPI_Signal_Event_Update (pCh->pTapiCh, IFX_TAPI_SIG_CPTD);
   }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
   /* CID transmitter events handling */
   if (nEDSPx_Int1 & (V2CPE_EDSP1_INT1_CIS_BUF |
                      V2CPE_EDSP1_INT1_CIS_REQ |
                      V2CPE_EDSP1_INT1_CIS_ACT))
   {
#ifdef TAPI
      /* second utg resource, if available */
      if ((nEDSPx_Int1 & V2CPE_EDSP1_INT1_CIS_ACT) &&
          (pDev->pSigCh[pCh->nChannel - 1].utg[1].bit.sm == 1 ||
           pDev->pSigCh[pCh->nChannel - 1].utg[1].bit.en == 1))
      {
         /* second utg, resource is #1 */
         Tone_EventUTG_Deactivated(pCh->pTapiCh, 1);
      }
      else
#endif /* TAPI */
      {
#ifdef TAPI_CID
         if ((nEDSPx_Int1 & V2CPE_EDSP1_INT1_CIS_ACT) &&
             ((nEDSPx_Stat1 & V2CPE_EDSP1_INT1_CIS_ACT) == 0))
         {
            /* Transition 1 -> 0 : Cid end of transmission and CIS disabled
               via autodeactivation if not offhook. */
            if (pCh->cidSend.nState != CID_OFFHOOK)
            {
               CIDTX_SET_STATE (CID_TRANSMIT_END);
               TAPI_Signal_Event_Update(pCh->pTapiCh, IFX_TAPI_SIG_CIDENDTX);
            }
         }
#endif /* TAPI_CID */
         /* CID transmit buffer underflow */
         if (nEDSPx_Int1 & V2CPE_EDSP1_INT1_CIS_BUF)
         {
            CIDTX_SET_STATE(CID_TRANSMIT_ERR);
         }
         /* Execute CID FSK state machine */
         if (pCh->cidSend.nState != CID_SETUP)
         {
            VINETIC_CidFskMachine(pCh);
         }
      }
   }

   VIN_REPORT_EVENT(pCh, V2CPE_CH_EDSP_INT1, nEDSPx_Int1, nEDSPx_Stat1);
}

/*******************************************************************************
Description :
   handles EDSP interrupts reported through register EDSPx_INT2

Arguments   :
   pCh            - pointer to the channel structure
   nEDSPx_Int2    - value of EDSPx_INT2 register
   nEDSPx_Stat2   - value of EDSPx_STAT2 register

Return      :
   none
Remarks     :
*******************************************************************************/
IFX_LOCAL IFX_void_t serve_EDSPx_Int2(VINETIC_CHANNEL *pCh,
                                      IFX_uint16_t nEDSPx_Int2,
                                      IFX_uint16_t nEDSPx_Stat2)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint16_t nCh = pCh->nChannel - 1;

   /* Handling of ATD1 events */
   if (nEDSPx_Int2 & (V2CPE_EDSP1_INT2_ATD1_DT |
                     V2CPE_EDSP1_INT2_ATD1_NPR_MASK |
                     V2CPE_EDSP1_INT2_ATD1_AM))
   {
      /* workaround for inability to clear bit V2CPE_EDSP1_STAT2_ATD1_AM
         upon ATD1_AM interrupt. */
      /* TODO check whether this workaround is needed for 2CPE */
      if ((nEDSPx_Stat2 & V2CPE_EDSP1_STAT2_ATD1_AM) &&
         !(pCh->hostCh.regEdspX_Stat2 & V2CPE_EDSP1_STAT2_ATD1_AM))
      {
#ifdef TAPI
         TAPI_Signal_Event_Update(pCh->pTapiCh, IFX_TAPI_SIG_AMMASK);
#endif /* TAPI */
         pCh->hostCh.regEdspX_Stat2 = nEDSPx_Stat2;
      }

      /* Answering tone/tone-end detected on receiver path (ATD1) */
      if (nEDSPx_Int2 & V2CPE_EDSP1_INT2_ATD1_DT)
      {
         if (nCh < pDev->nSigCnt)
         {
#ifdef TAPI
            if (nEDSPx_Stat2 & V2CPE_EDSP1_STAT2_ATD1_DT)
            {
               /* ATD, rising edge */
               /* event function selects what was configured: any CED,
                  tone holding end receive or both */
               TAPI_Signal_Event_Update(pCh->pTapiCh, IFX_TAPI_SIG_CEDMASK);
            }
            else
            {
               /* ATD, falling edge of ATD: end of tone or level */
               TAPI_Signal_Event_Update(pCh->pTapiCh, IFX_TAPI_SIG_CEDENDMASK |
                                                      IFX_TAPI_SIG_TONEHOLDING_ENDRX |
                                                      IFX_TAPI_SIG_TONEHOLDING_END);
            }
#endif /* TAPI */
         }
      }

      /* ATD1 phase reversal event */
      if (nEDSPx_Int2 & V2CPE_EDSP1_INT2_ATD1_NPR_MASK)
      {
#ifdef TAPI
         TAPI_Signal_Event_Update(pCh->pTapiCh, IFX_TAPI_SIG_PHASEREVMASK);
#endif /* TAPI */
      }
   }

   /* Handling of ATD2 events */
   if (nEDSPx_Int2 & (V2CPE_EDSP1_INT2_ATD2_DT |
                     V2CPE_EDSP1_INT2_ATD2_NPR_MASK |
                     V2CPE_EDSP1_INT2_ATD2_AM))
   {
      /* workaround for inability to clear bit V2CPE_EDSP1_STAT2_ATD2_AM
         upon ATD2_AM interrupt. */
      /* TODO check whether this is necessary for 2CPE */
      if ((nEDSPx_Stat2 & V2CPE_EDSP1_STAT2_ATD2_AM) &&
         !(pCh->hostCh.regEdspX_Stat2 & V2CPE_EDSP1_STAT2_ATD2_AM))
      {
#ifdef TAPI
         TAPI_Signal_Event_Update (pCh->pTapiCh, IFX_TAPI_SIG_AMMASK);
#endif
         pCh->hostCh.regEdspX_Stat2 = nEDSPx_Stat2;
      }

      /* Answering tone/tone-end detected on ATD2 */
      if (nEDSPx_Int2 & V2CPE_EDSP1_INT2_ATD2_DT)
      {
#ifdef TAPI
         /* event function selects what was configured: any DIS, tone holding
            end tranmit */
         TAPI_Signal_Event_Update(pCh->pTapiCh, IFX_TAPI_SIG_DISMASK |
                                   IFX_TAPI_SIG_TONEHOLDING_ENDTX);
#endif /* TAPI */
      }

      if (nEDSPx_Int2 & V2CPE_EDSP1_INT2_ATD2_NPR_MASK)
      {
#ifdef TAPI
         TAPI_Signal_Event_Update(pCh->pTapiCh, IFX_TAPI_SIG_PHASEREVMASK);
#endif /* TAPI */
      }
   }

   /* Event transmit unit overflow */
   if (nEDSPx_Int2 & V2CPE_EDSP1_INT2_ETU_OF)
   {
      /* perform ETU-OF acknowledge */
      pCmd_StatErrAck[0] = (pCmd_StatErrAck[0] & ~CMD1_CH) | nCh;
      pCmd_StatErrAck[2] = V2CPE_EDSP1_INT2_ETU_OF;
      CmdWriteIsr(pDev, pCmd_StatErrAck, 1);

      TRACE(VINETIC, DBG_LEVEL_LOW,
           ("VIN%ld, Ch%d: error, event transmit unit overflow\n\r",
            pDev->nDevNr, nCh));
   }

   /* Packetized voice protocol unit overflow */
   if (nEDSPx_Int2 & V2CPE_EDSP1_INT2_PVPU_OF)
   {
#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
      if (pCh->pTapiCh->TapiFaxStatus.nStatus &
          (IFX_TAPI_FAX_T38_DP_ON | IFX_TAPI_FAX_T38_TX_ON))
      {
         /* transmission stopped on this channel */
         TAPI_FaxT38_Event_Update (pCh->pTapiCh, IFX_TAPI_FAX_T38_TX_OFF,
                                   IFX_TAPI_FAX_T38_ERROR_DATAPUMP);
         TRACE(VINETIC, DBG_LEVEL_HIGH,
               ("Fax datapump Error (FDP_ERR), Channel %d\n\r", nCh));
	 printk("\nFax datapump Error (FDP_ERR), Channel %d\n\r", nCh);
      }
      else
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38) */
      {
         /* perform PVPU-OF acknowledge */
         pCmd_StatErrAck[0] = (pCmd_StatErrAck[0] & ~CMD1_CH) | nCh;
         pCmd_StatErrAck[2] = V2CPE_EDSP1_INT2_PVPU_OF;
         CmdWriteIsr(pDev, pCmd_StatErrAck, 1);
         reset_voice_outbox(pDev);
         TRACE(VINETIC, DBG_LEVEL_HIGH,
              ("VIN%ld, Ch%d: IRQ: error, packet outbox overflow\n\r",
               pDev->nDevNr, nCh));
      }
   }

   /* Decoder error or, decoder change */
   if (nEDSPx_Int2 & (V2CPE_EDSP1_INT2_DEC_ERR | V2CPE_EDSP1_INT2_DEC_CHG))
   {
      if (nEDSPx_Int2 & V2CPE_EDSP1_INT2_DEC_CHG)
      {  /* this bit has double meaning... */
#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
         /* 1. fax data pump request: FDP_REQ, overlaid with DEC_CHG */
         if (pCh->pTapiCh->TapiFaxStatus.nStatus & IFX_TAPI_FAX_T38_TX_ON)
         {
            pCh->bFaxDataRequest = IFX_TRUE;
            IFXOS_WakeUp (pCh->wqWrite, IFXOS_WRITEQ);
         }
         else
#endif /* VIN_FEAT_FAX_T38 */
         {
#ifdef ASYNC_CMD
            /* 2. decoder type or packet time change has occured, DEC_CHG */
            /* send command to read actual decoder status */
            /* On BXSR2[COBX], data will be read and filtered */
            pCmd_CodChDecStat[0] = ((pCmd_CodChDecStat[0] & ~CMD1_CH) |
                                    nCh | CMD1_RD);
            CmdWriteIsr(pDev, pCmd_CodChDecStat, 0);
            pDev->nMbxState |= DS_CMDREQ;
#endif /* ASYNC_CMD */
         }
      }
      if (nEDSPx_Int2 & V2CPE_EDSP1_INT2_DEC_ERR)
      {
         /* perform DEC-ERR acknowledge */
         pCmd_StatErrAck[0] = (pCmd_StatErrAck[0] & ~CMD1_CH) | nCh;
         pCmd_StatErrAck[2] = V2CPE_EDSP1_INT2_DEC_ERR;
         CmdWriteIsr(pDev, pCmd_StatErrAck, 1);

         TRACE(VINETIC, DBG_LEVEL_HIGH,
              ("VIN%ld, ch%d: IRQ: error, requested decoder cannot be activated\n\r",
               pDev->nDevNr, nCh));
      }
   }

   VIN_REPORT_EVENT(pCh, V2CPE_CH_EDSP_INT2, nEDSPx_Int2, nEDSPx_Stat2);
}

/*******************************************************************************
Description :
   Handle error interrupts reported through register ERR_INT
Arguments   :
   pDev        - pointer to the device structure
Return      :
   none
Remarks     :
*******************************************************************************/
IFX_LOCAL IFX_void_t serveErr_Int(VINETIC_DEVICE *pDev)
{
   IFX_uint16_t nErr_Int;

   /* Read error interrupt status register */
   REG_READ_UNPROT(pDev, V2CPE_ERR_INT, &nErr_Int);
   CHECK_HOST_ERR(pDev, return);

   /* Hardware error handling */
   if (nErr_Int & (V2CPE_ERR_INT_HW_ERR |
                  V2CPE_ERR_INT_MCLK_FAIL |
                  V2CPE_ERR_INT_SYNC_FAIL |
                  V2CPE_ERR_INT_PCMB_CRASH |
                  V2CPE_ERR_INT_PCMA_CRASH))
   {
      if ((nErr_Int == V2CPE_ERR_INT_SYNC_FAIL) &&
         (pDev->nDevState == DS_BASIC_INIT))
      {
         /* TODO Does this apply for 2CPE ? */
         /* Do not show SYNC_FAIL error only if it happens immediately after
          * basic initialization. (Vinetic chip Errata) */
         goto out;
      }
      SET_ERROR(ERR_HW_ERR);
      TRACE(VINETIC, DBG_LEVEL_HIGH,
           ("VIN%ld: IRQ: Hardware error, ERR_INT = 0x%04X\n",
            pDev->nDevNr, nErr_Int));
   }

   /* EDSP error handling */
   if (nErr_Int & (V2CPE_ERR_INT_MIPS_OL |
                  V2CPE_ERR_INT_WD_FAIL |
                  V2CPE_ERR_INT_CMD_ERR))
   {
      /* MIPS overload handling */
      if (nErr_Int & V2CPE_ERR_INT_MIPS_OL)
      {
         SET_ERROR(ERR_MIPS_OL);
#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
         {
            IFX_int32_t nCh;
            VINETIC_CHANNEL *pCh;
            for (nCh = 0; nCh < pDev->nAnaChan; nCh++)
            {
               pCh = &pDev->pChannel[nCh];

               /* check status and do some actions */
               if (pCh->pTapiCh->TapiFaxStatus.nStatus &
                   (IFX_TAPI_FAX_T38_DP_ON | IFX_TAPI_FAX_T38_TX_ON))
               {
                  /* transmission stopped, so set error and raise exception */
                  TAPI_FaxT38_Event_Update(pCh->pTapiCh, IFX_TAPI_FAX_T38_TX_OFF,
                                           IFX_TAPI_FAX_T38_ERROR_MIPS_OVLD);
                  TRACE(VINETIC,DBG_LEVEL_NORMAL,
                        ("VIN%ld, IRQ: MIPS overload, Ch%ld: FAX stopped\n\r",
                        pDev->nDevNr, nCh));
		  printk("\nVIN%ld, IRQ: MIPS overload, Ch%ld: FAX stopped\n\r",
				                          pDev->nDevNr, nCh);
               }
            }
         }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38) */
         CmdWriteIsr (pDev, pMipsOl, 1);
         TRACE(VINETIC, DBG_LEVEL_HIGH,
              ("VIN%ld: IRQ: MIPS overload\n\r", pDev->nDevNr));
      }

      if (nErr_Int & V2CPE_ERR_INT_WD_FAIL)
      {
         TRACE(VINETIC, DBG_LEVEL_HIGH,
              ("VIN%ld: IRQ: EDSP watchdog failure\n\r", pDev->nDevNr));
         SET_ERROR(ERR_EDSP_FAIL);
      }

      /* EDSP command error */
      if (nErr_Int & V2CPE_ERR_INT_CMD_ERR)
      {
         SET_ERROR(ERR_CERR);
         CmdWriteIsr (pDev, pCerr, 0);
         TRACE(VINETIC,DBG_LEVEL_HIGH,
              ("VIN%ld: IRQ: EDSP detected invalid command\n\r", pDev->nDevNr));
      }
   }

   /* Mailbox error handling */
   if (nErr_Int & (V2CPE_ERR_INT_VI_OV |
                  V2CPE_ERR_INT_CI_OV))
   {
      /* Voice inbox overflow */
      if (nErr_Int & V2CPE_ERR_INT_VI_OV)
      {
         TRACE(VINETIC,DBG_LEVEL_HIGH,
              ("VIN%ld: IRQ: Packet inbox overflow\n\r", pDev->nDevNr));
         SET_ERROR(ERR_PIBX_OF);
      }

      /* Command inbox overflow */
      if (nErr_Int & V2CPE_ERR_INT_CI_OV)
      {
         TRACE(VINETIC,DBG_LEVEL_HIGH,
              ("VIN%ld: IRQ: Command inbox overflow\n\r", pDev->nDevNr));
         SET_ERROR(ERR_CIBX_OF);
      }
   }

out:
   /* Clear error interrupt status register */
   REG_WRITE_UNPROT(pDev, V2CPE_ERR_INT, nErr_Int);
   VIN_REPORT_EVENT(pDev, V2CPE_DEV_ERR_INT, nErr_Int, nErr_Int);
}

