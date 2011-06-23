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
#include "drv_vinetic_int.h"
#include "drv_vinetic_cid.h"
#include "drv_vinetic_gpio.h"
#include "drv_vinetic_tone.h"

#ifdef TAPI
#include "drv_vinetic_tapi.h"
#endif /* TAPI */

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

/* ============================= */
/* Global variable definition    */
/* ============================= */


/* ============================= */
/* Global function declaration   */
/* ============================= */

#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
IFX_void_t  serveMbxData   (VINETIC_DEVICE *pDev);
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
IFX_void_t  serveMbxEvt    (VINETIC_DEVICE *pDev, IFX_uint16_t nIBxsr1, IFX_uint16_t nIBxsr2);

/* ============================= */
/* Local function declaration    */
/* ============================= */
IFX_LOCAL IFX_void_t  serveHwEvt     (VINETIC_DEVICE *pDev);
IFX_LOCAL IFX_void_t  serveIsrEvt    (VINETIC_DEVICE *pDev, IFX_uint16_t nIr);
IFX_LOCAL IFX_void_t  serveHWSR1Evt  (VINETIC_DEVICE *pDev, IFX_uint16_t nIHwsr1);
IFX_LOCAL IFX_void_t  serveHWSR2Evt  (VINETIC_DEVICE *pDev, IFX_uint16_t nIHwsr2);
#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
IFX_LOCAL IFX_void_t  serveSRE1Evt   (VINETIC_CHANNEL *pCh, IFX_uint16_t nISRE1, IFX_uint16_t nSRE1);
IFX_LOCAL IFX_void_t  serveSRE2Evt   (VINETIC_CHANNEL *pCh, IFX_uint16_t nISRE2, IFX_uint16_t nSRE2);
IFX_LOCAL IFX_void_t  emptyMbx       (VINETIC_DEVICE *pDev);
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */
IFX_LOCAL IFX_void_t  serveSRS1Evt   (VINETIC_CHANNEL *pCh, IFX_uint16_t nISRS1, IFX_uint16_t nSRS1);
IFX_LOCAL IFX_void_t  serveSRS2Evt   (VINETIC_CHANNEL *pCh, IFX_uint16_t nISRS2, IFX_uint16_t nSRS2);

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* MIPS overload acknowledge, clears the bit, HWSR1 */
IFX_LOCAL IFX_uint16_t pMipsOl [3] = { CMD1_EOP, ECMD_POW_CTL, 0x1 };

#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
/* clear packet overflow bit, SRE2 */
IFX_LOCAL IFX_uint16_t pPvpuOf [3] = { CMD1_EOP, ECMD_ERR_ACK, SRE2_PVPUOF };
/* clear ETUOF bit, SRE2 */
IFX_LOCAL IFX_uint16_t pEtuOf  [3] = { CMD1_EOP, ECMD_ERR_ACK, SRE2_ETU_OF };
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */

/* command to read actual decoder status */
#ifdef ASYNC_CMD
IFX_LOCAL IFX_uint16_t pDecStatus [2] = { CMD1_EOP, ECMD_COD_CHDECSTAT | 0x1};
#endif /* ASYNC_CMD */

/* ============================= */
/* Local function definition     */
/* ============================= */

#if 0
#ifdef TAPI
/*******************************************************************************
Description:
   function to demonstrate how coder AAL switching works.
Arguments:
   pDev - pointer to the device interface
Return:
   none
Remarks:
   The function is actually out commented.
   Comment in this part of code to test the coder switching AAL functionality :

   Every time the downstreaming side receives a different
   packet than the one of coder programmed, a DEC Change interrupt occurs
   on the channel, and this function programms the new coder so that the
   voice connection isnt interrupted whens coder changes on one channel.
*******************************************************************************/
IFX_LOCAL IFX_void_t switchAalCoder (VINETIC_DEVICE *pDev)
{
   IFX_int32_t i;

   for (i = 0; i < pDev->nAnaChan; i++)
   {
      if (pDev->pCodCh[i].dec_update)
         TAPI_LL_Phone_Aal_SwitchCoder (((VINETIC_DEVICE*)pDev)->pChannel[i].pTapiCh);
   }
}
#endif /* TAPI */
#endif /* 0 */

#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
/*******************************************************************************
Description:
   Dump all data from Packet Mailbox
Arguments:
   pDev - pointer to the device
Return:
   none
*******************************************************************************/
IFX_LOCAL IFX_void_t emptyMbx (VINETIC_DEVICE *pDev)
{
   IFX_uint16_t pData[MAX_PACKET_WORD], nSize;
   IFX_int32_t err;

   err = SC_READ(SC_ROBXML, &nSize, 1);
   nSize &= OBXML_PDATA;
   if (err == IFX_SUCCESS && nSize > 0)
   {
      err = SC_READ(SC_RPOBX, pData, nSize);
   }
}
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */

/*******************************************************************************
Description :
   handle interrupts in HWSR1
Arguments   :
   pDev     - pointer to the device structure
   nIHwsr1  - current value of IHWSR1 register
Return      :
   none
Remarks     :
*******************************************************************************/
IFX_LOCAL IFX_void_t serveHWSR1Evt(VINETIC_DEVICE *pDev, IFX_uint16_t nIHwsr1)
{
   if (nIHwsr1 & ~HWSR1_WOKE_UP)
   {
      /* ignore CLK_FAIL according to errata sheet DS5 */
      if ((nIHwsr1 ==  HWSR1_CLK_FAIL) &&
          (pDev->nChipMajorRev == VINETIC_V1x))
      {
         return;
      }
      else if ((nIHwsr1 & HWSR1_SYNC_FAIL) &&
                (pDev->nDevState == DS_BASIC_INIT))
      {
         /* Do not show SYNC_FAIL error only if it happens immediately after
          * basic initialization. (Vinetic chip Errata)
          */
         return;
      }
      else
      {
         SET_ERROR (ERR_HW_ERR);
         LOG (VINETIC, DBG_LEVEL_NORMAL,
               ("Int: Hardware Error, IHWSR1 = 0x%04X\n", nIHwsr1));
      }
   }
}

/*******************************************************************************
Description :
   handles interrupts in HWSR2
Arguments   :
   pDev     - pointer to the device structure
   nIHwsr2  - current value of IHWSR2 register
Return      :
   none
Remarks     :
*******************************************************************************/
IFX_LOCAL IFX_void_t serveHWSR2Evt(VINETIC_DEVICE *pDev, IFX_uint16_t nIHwsr2)
{
   /* download ready event is polled - no handling necessary */

   /* Mips overload handling */
   if (nIHwsr2 & HWSR2_EDSP_MIPS_OL)
   {
      SET_ERROR (ERR_MIPS_OL);
#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
      {
         IFX_int32_t nCh;
         VINETIC_CHANNEL *pCh = NULL;
         for (nCh = 0; nCh < pDev->nAnaChan; nCh ++)
         {
            pCh = &pDev->pChannel [nCh];

            /* check status and do some actions */
            if (pCh->pTapiCh->TapiFaxStatus.nStatus &
                (TAPI_FAX_T38_DP_ON | TAPI_FAX_T38_TX_ON))
            {
               /* transmission stopped, so set error and raise exception */
               TAPI_FaxT38_Event_Update (pCh->pTapiCh, TAPI_FAX_T38_TX_OFF,
                                         TAPI_FAX_T38_MIPS_OVLD);
               TRACE (VINETIC, DBG_LEVEL_NORMAL,
                     ("FAX : MIPS Overloaded, Channel %ld\n\r", nCh));
            }
         }
      }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38) */
      /* clear EDSP */
      CmdWriteIsr (pDev, pMipsOl, 1);
      LOG (VINETIC, DBG_LEVEL_HIGH,
            ("Int: Mips Overloaded, IHWSR2 = 0x%04X\n\r", nIHwsr2));
   }

   /* EDSP Checksum failed */
   if (nIHwsr2 & HWSR2_EDSP_CS_FAIL)
   {
      SET_ERROR (ERR_EDSP_FAIL);
      LOG (VINETIC, DBG_LEVEL_HIGH,
            ("Int: EDSP Checksum failed, IHWSR2 = 0x%04X\n\r", nIHwsr2));
   }
}

#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
/*******************************************************************************
Description :
   handles interrupts in SRE1
Arguments   :
   pCh      - pointer to the channel structure
   nISRE1   - value of ISRE1 register
   nSRE1    - value of SRE1 register
Return      :
   none
Remarks     :
Remarks     :
   Nobody waits for not handled events actually.
*******************************************************************************/
IFX_LOCAL IFX_void_t serveSRE1Evt (VINETIC_CHANNEL *pCh, IFX_uint16_t nISRE1,
                                   IFX_uint16_t nSRE1)
{
#ifdef TAPI_DTMF
   IFX_uint8_t nKey = 0;
#endif /* TAPI_DTMF */
#ifdef TAPI_DTMF
   VINETIC_DEVICE *pDev =  pCh->pParent;
#endif /* TAPI_DTMF */

   /* save interrupt register value */
   pCh->IsrFlag [EV_SRE1] |= nISRE1;

   /* DTMF transmitter */
   if (nISRE1 & (SRE1_DTMFG_REQ | SRE1_DTMFG_BUF | SRE1_DTMFG_ACT))
   {
      /* DTMF generator activated */
      if (nISRE1 & SRE1_DTMFG_ACT)
      {
         /** \todo actually, UTG is used to generate alert signal for nttS CID2.
            But because the tone api isn't used for this purpose now, we check
            if NTT CID2 (japan) is ongoing. As soon as the tone api will be fully
            available, the wake up will be done by the appropriate tapi function*/
#ifdef TAPI_CID
         if (pCh->pTapiCh->TapiCid2.conf.stdSelect == TAPI_CID2_NTT)
         {
            /*PRINTF ("IFX_int32_t : CID NTT AS played, ch%d\n\r", pCh->pTapiCh->nChannel);*/
            pCh->pTapiCh->TapiCid2.bAStxOk = IFX_TRUE;
            IFXOS_WakeUpEvent (pCh->pTapiCh->TapiCid2.asEvt);
         }
#endif /* TAPI_CID */
         if (pCh->pTapiCh->TapiComplexToneData[0].nToneState == TAPI_CT_ACTIVE)
         {
            Tone_EventUTG_Deactivated(pCh->pTapiCh);
         }
         else
         {
            if (nSRE1 & SRE1_DTMFG_ACT)
               VINETIC_DtmfOnActivate(pCh, IFX_TRUE);
            else
               VINETIC_DtmfOnActivate(pCh, IFX_FALSE);
         }
      }
      /* DTMF generator underrun */
      if (nISRE1 & (SRE1_DTMFG_BUF))
      {
         VINETIC_DtmfOnUnderrun(pCh);
      }
      /* DTMF generator data request */
      if (nISRE1 & (SRE1_DTMFG_REQ))
      {
         VINETIC_DtmfOnRequest(pCh);
      }
   }

   /* DTMF key detected or probably detected? DTMFR-DT / DTMFR-PDT */
   if (nISRE1 & (SRE1_DTMFR_DT | SRE1_DTMFR_PDT))
   {
      /* valid DTMF key detected */
      if (nISRE1 & SRE1_DTMFR_DT)
      {
#ifdef TAPI_DTMF
         /* read detected key */
         nKey = ((nSRE1 & SRE1_DTMFR_DTC) >> 10);
         /* call TAPI events for analog channels only */
         if ((pCh->nChannel - 1) < pDev->nSigCnt)
         {
#ifdef TAPI_CID
            if ((pCh->pTapiCh->TapiCid2.bCid2IsOn == IFX_TRUE) &&
                (pCh->pTapiCh->TapiCid2.conf.std.defaultS.ackTone == nKey))
            {
               pCh->pTapiCh->TapiCid2.bAck = IFX_TRUE;
            }
            else
#endif /* TAPI_CID */
            {
               VINETIC_Tapi_Event_Dtmf (pCh->pTapiCh, nKey);
            }
         }
#endif /* TAPI_DTMF */
      }
   }
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   /* Universal Tone detection 1 UTD1-OK */
   if (nISRE1 & SRE1_UTD1_OK)
   {
      TAPI_Signal_Event_Update (pCh->pTapiCh, pCh->nUtdSig[0]);
   }

   /* Universal Tone detection 2 UTD2-OK */
   if (nISRE1 & SRE1_UTD2_OK)
   {
      TAPI_Signal_Event_Update (pCh->pTapiCh, pCh->nUtdSig[1]);
   }
   /* Call progress tone detected */
   if (nISRE1 & SRE1_CPT)
   {
      TAPI_Signal_Event_Update (pCh->pTapiCh, TAPI_SIGNAL_CPTD);
   }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */

   /* send CID data or handle buffer underflow if */
   if (nISRE1 & SRE1_CID_MASK)
   {
#ifdef TAPI_CID
      if ((nISRE1 & SRE1_CIS_ACT) && ((nSRE1 & SRE1_CIS_ACT) == 0))
      {
         /* Transition 1 -> 0 : Cid end of transmission and CIS disabled
            via autodeactivation. */
         pCh->cidSend.nState = CID_TRANSMIT_END;
         TAPI_Signal_Event_Update(pCh->pTapiCh, TAPI_SIGNAL_CIDENDTX);
      }
#endif /* TAPI_CID */
      if (nISRE1 & SRE1_CIS_BUF)
      {
         pCh->cidSend.nState = CID_TRANSMIT_ERR;
      }
      if (pCh->cidSend.nState != CID_SETUP)
      {
         VINETIC_CidFskMachine (pCh);
      }
   }

   /* CID receiver */
   if (nISRE1 & SRE1_CIDR_OF)
   {
#ifdef TAPI_CID
      /* \todo set error and raised an exeption : CID receiver must be stopped now */
      /*pCh->pTapiCh->TapiCidRx.stat.nError = TAPI_CIDRX_ERR_PACKETLOS;
      TAPI_Phone_Event_CidRx (pCh->pTapiCh);*/
#endif /* TAPI_CID */
   }
#ifdef EVALUATION
   Eval_serveSRE1Evt (pCh, nISRE1, nSRE1);
#endif /* EVALUATION */
}

/*******************************************************************************
Description :
   handles interrupts in SRE2
Arguments   :
   pCh      - pointer to the channel structure
   nISRE2   - value of ISRE2 register
   nSRE2    - value of SRE2 register
Return      :
   none
Remarks     :
   Nobody waits for not handled events actually.
*******************************************************************************/
IFX_LOCAL IFX_void_t serveSRE2Evt (VINETIC_CHANNEL *pCh, IFX_uint16_t nISRE2,
                                   IFX_uint16_t nSRE2)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint8_t            nCh  = (pCh->nChannel - 1);

   /* save interrupt register value */
   pCh->IsrFlag [EV_SRE2] |= nISRE2;

   if ((nSRE2 & SRE2_ATD1_AM) && !(pCh->regSRE2 & SRE2_ATD1_AM))
   {
      TAPI_Signal_Event_Update (pCh->pTapiCh, TAPI_SIGNAL_AMMASK);
      pCh->regSRE2 = nSRE2;
   }

   /* Answering Tone/Tone End detected ATD1-D1 */
   if (nISRE2 & SRE2_ATD1_DT)
   {
      if (nCh < pDev->nAnaChan)
      {
         if (nSRE2 & SRE2_ATD1_DT)
         {
            /* rising edge */
            /* event function selects what was configured: any CED,
               tone holding end receive or both */
            TAPI_Signal_Event_Update (pCh->pTapiCh, TAPI_SIGNAL_CEDMASK);
         }
         else
         {
            if ((pCh->pTapiCh->TapiMiscData.sigMask &
                 TAPI_SIGNAL_TONEHOLDING_ENDMASK) ==
                 TAPI_SIGNAL_TONEHOLDING_ENDTX)
               /* falling edge of ATD: end of tone or level */
               TAPI_Signal_Event_Update (pCh->pTapiCh, TAPI_SIGNAL_CEDENDMASK |
                                      TAPI_SIGNAL_TONEHOLDING_ENDTX);
            else
               /* falling edge of ATD: end of tone or level */
               TAPI_Signal_Event_Update (pCh->pTapiCh, TAPI_SIGNAL_CEDENDMASK |
                                         TAPI_SIGNAL_TONEHOLDING_ENDRX |
                                         TAPI_SIGNAL_TONEHOLDING_END);

         }
      }
   }
   if (nISRE2 & SRE2_ATD1_NPR)
   {
      TAPI_Signal_Event_Update (pCh->pTapiCh, TAPI_SIGNAL_PHASEREVMASK);
   }

   /* Workaround for not being able to clear bit SRE2_ATD2_AM upon AM interrupt. */
   if ((nSRE2 & SRE2_ATD2_AM) && !(pCh->regSRE2 & SRE2_ATD2_AM))
   {
      TAPI_Signal_Event_Update (pCh->pTapiCh, TAPI_SIGNAL_AMMASK);
      pCh->regSRE2 = nSRE2;
   }

   /* Answering Tone/Tone End detected - ATD2 ATD2-DT */
   if (nISRE2 & SRE2_ATD2_DT)
   {
      /* event function selects what was configured: any DIS, tone holding
         end tranmit */
      TAPI_Signal_Event_Update (pCh->pTapiCh, TAPI_SIGNAL_DISMASK |
                                TAPI_SIGNAL_TONEHOLDING_ENDTX);
   }

   if (nISRE2 & SRE2_ATD2_NPR)
   {
      TAPI_Signal_Event_Update (pCh->pTapiCh, TAPI_SIGNAL_PHASEREVMASK);
   }

   /* Event transmit Unit overflow ETU-OF */
   if (nISRE2 & SRE2_ETU_OF)
   {
      /* acknowledge channel ETU-OF */
      pEtuOf [0] = (pEtuOf [0] & ~CMD1_CH) | (nCh & CMD1_CH);
      CmdWriteIsr (pDev, pEtuOf, 1);
   }
   /* Packet Box overflow PVPUOF */
   if (nISRE2 & SRE2_PVPUOF)
   {
#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
      if (pCh->pTapiCh->TapiFaxStatus.nStatus &
          (TAPI_FAX_T38_DP_ON | TAPI_FAX_T38_TX_ON))
      {
         /* transmission stopped on this channel */
         TAPI_FaxT38_Event_Update (pCh->pTapiCh, TAPI_FAX_T38_TX_OFF,
                                   TAPI_FAX_T38_ERR);
	 printk("\nPVPUOF\n");
         TRACE (VINETIC, DBG_LEVEL_HIGH,
               ("Fax datapump Error (FDP_ERR), Channel %d\n\r", nCh));
      }
      else
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38) */
      {
         emptyMbx (pDev);
         /* clear channel status */
         pPvpuOf [0] = (pPvpuOf [0] & ~CMD1_CH) | (nCh & CMD1_CH);
         CmdWriteIsr (pDev, pPvpuOf, 1);
         LOG (VINETIC, DBG_LEVEL_NORMAL,
               ("Overflow in Packet Outbox, ch %d\n\r", nCh));
      }
   }
   /* This bit is set when the Event Play Out Unit receives a programmable
      Event (see Signalling Channel Config Command for programmable Events). */
   if (nISRE2 & SRE2_EPOU_TRIG)
   {
      IFX_uint16_t pEpouTrig[2];

      /* acknowledge channel ETU-OF */
      pEpouTrig [0] = (pCh->nChannel - 1) | CMD1_EOP | CMD1_RD;
      pEpouTrig [1] = ECMD_EPOU_TRIG | 1;
      CmdWriteIsr (pDev, pEpouTrig, 0);
   }
#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
   /*  Fax datapump request */
   if (nISRE2 & SRE2_FDP_REQ &&
       pCh->pTapiCh->TapiFaxStatus.nStatus & TAPI_FAX_T38_TX_ON)
   {
      pCh->IsrFlag[EV_SRE2] |= SRE2_FDP_REQ;
      pCh->bFaxDataRequest   = IFX_TRUE;
      IFXOS_WakeUp (pCh->wqWrite, IFXOS_WRITEQ);
      TRACE (VINETIC, DBG_LEVEL_LOW,
               ("Int: FDP_REQ set in ISRE2, ch %d\n\r", nCh));
   }
#elif defined (ASYNC_CMD)
   /* Decoder Change for AAL ? */
   if (nISRE2 & SRE2_DEC_CHG)
   {
      /* send command to read actual decoder status */
      /* On BXSR2 [COBX], data will be read and filtered */
      pDecStatus [0] = (pDecStatus [0] & ~CMD1_CH) | (nCh & CMD1_CH) | CMD1_RD;
      CmdWriteIsr (pDev, pDecStatus, 1);
      pDev->nMbxState |= DS_CMDREQ;
   }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38) / ASYNC_CMD */
}
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */

/*******************************************************************************
Description :
   handles interrupts in SRS1
Arguments   :
   pCh      - pointer to the channel structure
   nISRS1   - value of ISRS1 register
   nSRS1    - value of SRS1 register
Return      :
   none
Remarks     :
   Nobody waits for not handled events actually.
*******************************************************************************/
IFX_LOCAL IFX_void_t serveSRS1Evt (VINETIC_CHANNEL *pCh, IFX_uint16_t nISRS1,
                                   IFX_uint16_t nSRS1)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
#ifdef TAPI_CID
   TAPI_RING_DATA  *pRingData  = &pCh->pTapiCh->TapiRingData;
   TAPI_CID_DATA   *pCidData   = &pRingData->pDataCh->TapiCidTx;
   TAPI_CID_CONFIG *pCidConfig = &pRingData->pDataCh->TapiCidConfig;
#endif /* TAPI_CID */

   /* save interrupt register value */
   pCh->IsrFlag [EV_SRS1] |= nISRS1;
   /*  ramp generator reached end of ramp RAMP-RDY */
   if (nISRS1 & SRS1_RAMP_READY)
   {
      if (pDev->IrqPollMode & VIN_TAPI_WAKEUP)
      {
         IFXOS_WakeUpEvent (pCh->wqLT_RAMP_RDY);
      }
   }
   /* Hook SRS1_HOOK */
   if (nISRS1 & SRS1_HOOK)
   {
      /* Stop DTMF transmission on hook event */
      if (pCh->dtmfSend.nWords != 0)
      {
#ifdef TAPI_CID
         pCidData->nCidTxState = CID_TX_HOOK;
#endif
         VINETIC_DtmfStop(pCh, IFX_TRUE);
      }
      /* check if on hook occured */
      if (!(nSRS1 & SRS1_HOOK))
      {
         /* it was an onhook */
         pCh->IsrFlag [EV_SRS1] &= ~SRS1_HOOK;
      }
      /* do following for off-hook */
      else
      {
#ifdef TAPI_CID
         /* hook occurs on the phone channel. Get the CID channel from that */
         IFX_uint8_t cid_ch = pRingData->pDataCh->nChannel;
         VINETIC_CHANNEL *pCidCh =  &pDev->pChannel [cid_ch];

         /* for ongoing cid transmisson, call cid machine to handle off hook
            in case of CID type 1 (not applicable for NTT standard!) */
         if ((pCidCh->cidSend.nCidCnt) && (pCidCh->cidSend.nCidType == CID_TYPE_1))
         {
            pCidCh->cidSend.nState = CID_OFFHOOK;
            VINETIC_CidFskMachine (pCidCh);
         }
#endif /* TAPI_CID */
      }
#ifdef TAPI
#ifdef TAPI_CID
      /* in case an NTT CID1 is ongoing, this offhook is the aknowledge to the
         NTT alert routine */
      if ((nSRS1 & SRS1_HOOK) && (pCidConfig->nSpec == TAPI_CID_NTT)  &&
          (pCidData->bIsNewData == IFX_TRUE) &&
          (pCidData->bCidAlertEvt == IFX_FALSE))
      {
         pCidData->bCidAlertEvt = IFX_TRUE;
      }
      else
#endif /* TAPI_CID */
      {
         /* raise TAPI exception */
         TAPI_Phone_Event_HookState (pCh->pTapiCh,
                                     (nSRS1 & SRS1_HOOK) ? IFX_TRUE : IFX_FALSE);
      }
#endif /* TAPI */
   }
   /* Ground key event GNDK */
   if (nISRS1 & SRS1_GNDK)
   {
#ifdef TAPI
      /* Tapi Ground Key Event */
      TAPI_Phone_Event_GNK(pCh->pTapiCh);
#endif /* TAPI */
   }
   /*  Ground polarity GNDKP */
   if (nISRS1 & SRS1_GNDKP)
   {
#ifdef TAPI
      /* Tapi Ground Key Pol Event */
      TAPI_Phone_Event_GNKP (pCh->pTapiCh);
#endif /* TAPI */
   }
   /* High threshold for ground key GNDKH */
   if (nISRS1 & SRS1_GNDKH)
   {
#ifdef TAPI
      switch (pCh->pTapiCh->TapiMiscData.GndkhState)
      {
         case GNDKH_STATE_READY:
            SC_WRITE (SC_POW_DOWN | (pCh->nChannel - 1));
            break;
         case GNDKH_STATE_POWDN:
            /* apply the stored state back again */
            SC_WRITE (OPMOD_CUR.value | CMD1_SC | (pCh->nChannel - 1));
            break;
         case GNDKH_STATE_RECHECK:
            /* second ground key occured after reenabled line */
            SC_WRITE (SC_POW_DOWN | (pCh->nChannel - 1));
            break;
         default:
            break;
      }
      /* Tapi Ground Key High Event */
      TAPI_Phone_Event_GNKH (pCh->pTapiCh);
#endif /* TAPI */
   }
   /* level metering - measurement completed LM-OK */
   if (nISRS1 & SRS1_LM_OK)
   {
      if (pDev->IrqPollMode & VIN_TAPI_WAKEUP)
      {
         IFXOS_WakeUpEvent (pCh->wqLT_LM_OK);
      }
   }
   /* Low power battery possible. Only falling edge is masked! */
   if (nISRS1 & SRS1_VTRLIM)
      TAPI_Phone_Event_Line(pCh->pTapiCh, TAPI_LINESTATUS_FEEDLOWBATT);
   /* Channel IO pin interrupts */
   if (nISRS1 & SRS1_DU_IO)
   {
      VINETIC_GpioIntDispatch((IFX_int32_t)pCh, (nISRS1 & SRS1_DU_IO));
   }
}

/*******************************************************************************
Description :
   handles interrupts in SRS2
Arguments   :
   pCh      - pointer to the channel structure
   nISRS2   - value of ISRS2 register
   nSRS2    - value of SRS2 register
Return      :
   none
Remarks     :
   Nobody waits for not handled events actually.
*******************************************************************************/
IFX_LOCAL IFX_void_t serveSRS2Evt (VINETIC_CHANNEL *pCh, IFX_uint16_t nISRS2,
                                   IFX_uint16_t nSRS2)
{
   IFX_uint8_t            nCh  = (pCh->nChannel - 1);
   /* needed for SC_WRITE */
   VINETIC_DEVICE *pDev = pCh->pParent;

   /* save interrupt register value */
   pCh->IsrFlag [EV_SRS2] |= nISRS2;
   /* Thermal Overload */
   if (nISRS2 & SRS2_OTEMP)
   {
#ifdef TAPI
      /* stop tapi meterring  */
      if (pCh->pTapiCh->TapiMeterData.bMeterActive == IFX_TRUE)
      {
         LOG (VINETIC, DBG_LEVEL_HIGH, ("Stop Metering\n\r"));
         TAPI_Phone_Meter_Stop(pCh->pTapiCh);
      }
      TAPI_Phone_Event_Otemp (pCh->pTapiCh);
#endif /* TAPI */
      /* set mode power down */
      SC_WRITE (SC_POW_DOWN | nCh);
      LOG (VINETIC, DBG_LEVEL_HIGH, ("WARNING Over temperature in ch %d."
             " Line mode set to Power Down\n\r", nCh));
   }
}

/****************************************************************************
Description :
   Serves the mailbox event interrupts
Arguments   :
   *pDev    - pointer to the device structure
Remarks     :
   Reacts on the appropriate mailbox event, except data event,
   done directly vinetic_interrupt_routine.
****************************************************************************/
IFX_void_t serveMbxEvt (VINETIC_DEVICE *pDev, IFX_uint16_t nIBxsr1, IFX_uint16_t nIBxsr2)
{
   IFX_int32_t err;
   IFX_uint16_t pBxsr[2];
   IFX_uint16_t nRPHIERR = 0,
        nBxsr2Err = (BXSR2_PIBX_OF | BXSR2_HOST_ERR | BXSR2_CIBX_OF);
   IFX_LOCAL IFX_uint16_t pCerr [] = { CMD1_EOP, ECMD_CMDERR_ACK };

   /* read mailbox status to clear interrupt */
   err = SC_READ (SC_RBXSR, pBxsr, 2);
   if (err == IFX_ERROR)
   {
      LOG (VINETIC, DBG_LEVEL_HIGH, ("ERROR: Cannot read RBXSR !\n\r"));
      return;
   }
   /* CERR in BXSR1 ? Currently it is the only bit */
   if (nIBxsr1 & BXSR1_CERR)
   {
      SET_ERROR (ERR_CERR);
      pDev->IsrFlag [EV_MBX] |= BXSR1_CERR;
      CmdWriteIsr (pDev, pCerr, 0);
   }
   if (nIBxsr2)
   {
      if (nIBxsr2 & nBxsr2Err)
      {
         LOG (VINETIC, DBG_LEVEL_NORMAL, ("BXSR2 Error %08X!\n\r",
               (nIBxsr2 & nBxsr2Err)));
         pDev->IsrFlag [EV_MBX] |= (nIBxsr2 & nBxsr2Err);
         /* Command In-Box Overflow CIBX-OF */
         if (nIBxsr2 & BXSR2_CIBX_OF)
            SET_ERROR (ERR_CIBX_OF);
         if (nIBxsr2 & BXSR2_HOST_ERR)
            SET_ERROR (ERR_HOST);
         if (nIBxsr2 & BXSR2_PIBX_OF)
            SET_ERROR (ERR_PIBX_OF);
         /* acknowledge the error in the PHI. Resets the error bits */
         if (pDev->nChipRev == VINETIC_V13)
         {
            err = SC_READ (SC_RPHIERR, &nRPHIERR, 1);
         }
         else
         {
            SC_WRITE (SC_WPHIERR);
         }
      }
#ifdef ASYNC_CMD
      /* Command Out-Box Event COBX_DATA */
      if (nIBxsr2 & BXSR2_COBX_DATA)
      {
         if (pBxsr[1] & BXSR2_COBX_DATA)
         {
            if (CmdReadIsr (pDev) > 0)
            {
               /* the CmdReadIsr() has read out some messages from the mailbox.
                  Parse these messages to dispatchCmd by using
                  DispatchCmdAsyncBuffer() */
               DispatchCmdAsyncBuffer(pDev);
            }
         }
      }
#endif /* ASYNC_CMD */
   }
}

#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
/*******************************************************************************
Description :
   Serves the mailbox data events
Arguments   :
   *pDev    - pointer to the device structure
Return      :
   none
Remarks     :
  Function reads until no more data is indicated in OBXML.
  If data is available, it reads the command words first to get frame
  length and channel. Then it retrieves fifo memory and reads out the
  data.
*******************************************************************************/
IFX_void_t serveMbxData(VINETIC_DEVICE  *pDev)
{
   IFX_int32_t cnt = 0, nSize, err;
   IFX_uint16_t nObxml;

   /* read number of WORDs available in the mailbox */
   err = SC_READ (SC_ROBXML, &nObxml, 1);
   if (err == IFX_ERROR)
      return;
   nSize = nObxml & OBXML_PDATA;
   while (nSize > 0)
   {
      /* read all available packets from vinetic, max. nSize words */
      nSize = pDev->packet_read (pDev, nSize);
      if (nSize == 0)
      {
         /* Ok, we read all data now; check if further data arrived because
            we wouldn't get another edge interrupt */
         err = SC_READ (SC_ROBXML, &nObxml, 1);
         if (err == IFX_ERROR)
            return;
         nSize = nObxml & OBXML_PDATA;
         cnt ++;
      }
      else if (nSize == IFX_ERROR)
      {
         SET_ERROR (ERR_DEV_ERR);
         return;
      }
      if (cnt > VINETIC_CH_NR)
      {
         IFXOS_ASSERT(cnt > VINETIC_CH_NR);
         return;
      }
   }
}
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */


/*******************************************************************************
Description:
   serves hardware envent interrupts
Arguments  :
   *pDev - pointer to the device structure
Return     :
   none
Remarks    :
   Handle HW Events by reading the HW Interrupt Status Register and
   HW Status register.
*******************************************************************************/
IFX_LOCAL IFX_void_t serveHwEvt (VINETIC_DEVICE *pDev)
{
   IFX_int32_t err;
   IFX_uint16_t pIHwsr[2], pHwsr[2];

   /* read IHWSRs */
   err = SC_READ (SC_RIHWSR, pIHwsr, 2);
   if (err == IFX_ERROR)
      return;
   /* read HWSRs */
   err = SC_READ (SC_RHWSR, pHwsr,  2);
   if (err == IFX_ERROR)
      return;

   /* dispatch to HW interrupt handlers */
   if (pIHwsr[0])
      serveHWSR1Evt (pDev,  pIHwsr[0]);
   if (pIHwsr[1])
      serveHWSR2Evt (pDev,  pIHwsr[1]);
}

/*******************************************************************************
Description:
   serves Status Register Event interrupt
Arguments:
   *pDev - pointer to the device structure
Return   :
   channel if Channel SRE1 Event or NOTVALID
Remarks  :
   reacts on the appropriate SR event. Please check SRSs 0 to 7!!
*******************************************************************************/
IFX_LOCAL IFX_void_t serveIsrEvt (VINETIC_DEVICE *pDev, IFX_uint16_t nIr)
{
   VINETIC_CHANNEL *pCh;
   IFX_uint16_t pIr[4] = {0}, pSr[4] = {0};
   IFX_uint32_t i, lastCh;
   IFX_int32_t err;
   IFX_uint8_t nCh = 0;

   if (pDev->nChipMajorRev == VINETIC_V2x)
      lastCh = IR_ISR3;
   else
      lastCh = IR_ISRE7;
   for (i = IR_ISR0; i < (lastCh << 1); i = i << 1)
   {
      if (nIr & i)
      {
         /* read iSRSs/SRSs for chan x if ISRx event */
         pCh    = &pDev->pChannel [nCh];
         if (i < IR_ISRE4)
         {
            /* the lower 4 channels contain also SRS events */
            err = SC_READ (SC_RISR | nCh, pIr, 4);
            if (err == IFX_ERROR)
               return;
            err = SC_READ (SC_RSR  | nCh, pSr, 4);
            if (err == IFX_ERROR)
               return;
            /* dispatch to SRS handlers */
            if (pIr[2])
               serveSRS1Evt (pCh, pIr[2], pSr[2]);
            if (pIr[3])
               serveSRS2Evt (pCh, pIr[3], pSr[3]);
         }
         else
         {
            /* the upper 4 channels contain only EDSP events */
            err = SC_READ (SC_RISR | nCh, pIr, 2);
            if (err == IFX_ERROR)
               return;
            err = SC_READ (SC_RSR  | nCh, pSr, 2);
            if (err == IFX_ERROR)
               return;
         }
#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
         /* dispatch to SRE handlers (not necessary for VIN_S) */
         if (pIr[0])
            serveSRE1Evt (pCh, pIr[0], pSr[0]);
         if (pIr[1])
            serveSRE2Evt (pCh, pIr[1], pSr[1]);
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */
      }
      nCh++;
   }
}


/* ============================= */
/* Global function definition    */
/* ============================= */

/*******************************************************************************
Description:
   VINETIC Interrupt Rountine
Arguments:
   pDev - pointer to the device interface
Return:
   none
Remarks:
   If real time interrupt routine is used, serve real time events
   here according to flags set in real time interrupt routine
*******************************************************************************/
IFX_void_t VINETIC_interrupt_routine(VINETIC_DEVICE *pDev)
{
   IFX_int32_t  err;
   IFX_uint32_t cnt = 0;
   IFX_uint16_t nIr, nIGpio, nGpio;
   IFX_uint16_t pIBxsr[2];
#if 0
#ifdef TAPI
   switchAalCoder (pDev);
#endif /* TAPI */
#endif /* 0 */

   for(;;)
   {
     /* read the Vinetic Interrupt Register (IR) */
#if defined (VIN_8BIT) || defined (VIN_SPI)
      err = SC_READ (SC_RIR, &nIr, 1);
      if (err == IFX_ERROR)
      {
         LOG(VINETIC,DBG_LEVEL_HIGH,
              ("\n\rRead IR not working on Device %ld. Interrupts "
               "will be disabled if working in interrupt mode\n\r",
               pDev->nDevNr));
         /* disbale the interrupt line of this device. If no interrupt line is
            connected, the global interrupt is disabled.
            In the next step the global interrupt are enabled again. If an
            interrupt line of this device was disabled, it stays disabled */
         Vinetic_IrqLockDevice (pDev);
         Vinetic_IrqUnlockGlobal (pDev);
         return;
      }
#else
      nIr = *pDev->pIntReg;
#endif /* ((!defined VIN_8BIT) && (!defined VIN_SPI)) */
#ifdef VIN_V21_SUPPORT
      /* mask reserved bits */
      nIr &= ~(0x60);
#endif /* VIN_V21_SUPPORT */

      /* break when no more interrupts */
      if (!(nIr & ~(IR_RDYQ)))
         break;

      if (pDev->err == ERR_DEV_ERR)
      {
         LOG(VINETIC,DBG_LEVEL_HIGH,
              ("\n\rDevice Error on Device %ld. Interrupts "
               "will be disabled if working in interrupt mode\n\r",
               pDev->nDevNr));
         /* disbale the interrupt line of this device. If no interrupt line is
            connected, the global interrupt is disabled.
            In the next step the global interrupt are enabled again. If an
            interrupt line of this device was disabled, it stays disabled */
         Vinetic_IrqLockDevice (pDev);
         Vinetic_IrqUnlockGlobal (pDev);
         break;
      }
      /* Reset Event IR-RESET (can only be reseted by ShortCmd!) */
      if (nIr & IR_RESET)
      {
         IFX_uint8_t nCh;
#if ((!defined VIN_8BIT) && (!defined VIN_SPI))
         IFX_uint16_t  ir;
         SC_READ (SC_RIR, &ir, 1);
#endif /* ((!defined VIN_8BIT) && (!defined VIN_SPI)) */
         for (nCh = 0; nCh < pDev->TapiDev.nMaxChannel; nCh++)
            TAPI_Phone_Event_Device (&(pDev->TapiDev.pChannelCon[nCh]),
                                     TAPI_DEVRESET);
      }
      /* Timeout check ... */
      if (++cnt > 100)
      {
         /* disbale the interrupt line of this device. If no interrupt line is
            connected, the global interrupt is disabled.
            In the next step the global interrupt are enabled again. If an
            interrupt line of this device was disabled, it stays disabled */
         Vinetic_IrqLockDevice (pDev);
         Vinetic_IrqUnlockGlobal (pDev);
         SET_ERROR(ERR_INTSTUCK);
         break;
      }

      /* if any other bit except the IR_MBX_EVT or IR_RDYQ bit is set ... */
      if (nIr & ~(IR_MBX_EVT | IR_RDYQ))
      {
         /* dispatch for ISR Events */
         if (nIr & IR_ISRCH)
            serveIsrEvt (pDev, nIr);

         /* dispatch for HW Events */
         if (nIr & IR_HW_STAT)
         {
            serveHwEvt(pDev);
         }

         /* dispatch for GPIO Interrupts */
         if (nIr & IR_GPIO)
         {
            err = SC_READ (SC_RISRGPIO, &nIGpio, 1);
            if (err == IFX_ERROR)
               return;
            err = SC_READ (SC_RSRGPIO, &nGpio, 1);
            if (err == IFX_ERROR)
               return;
            VINETIC_GpioIntDispatch((IFX_int32_t)pDev, nIGpio);
         }
      }

      /* MBX Event */
      if (nIr & IR_MBX_EVT)
      {
         /* read BXSR1 & BXSR2 */
         err = SC_READ (SC_RIBXSR, pIBxsr, 2);
         if (err == IFX_ERROR)
            return;

#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
         /* In polled mode, packet outbox is not serviced in interrupt
            routine in case client uses other routines for this. */
         if (!(pDev->IrqPollMode & VIN_VOICE_POLL))
         {
            /* POBX DATA Event */
            if (pIBxsr[1] & BXSR2_POBX_DATA)
               serveMbxData (pDev);
         }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */

         /* all other events */
         if ((pIBxsr[0] != 0) || (pIBxsr[1] & ~BXSR2_POBX_DATA))
            serveMbxEvt (pDev, pIBxsr[0], pIBxsr[1]);
      }
   } /* end of for loop */
}

