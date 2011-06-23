/******************************************************************************

                               Copyright (c) 2006
                            Infineon Technologies AG
                     Am Campeon 1-12; 81726 Munich, Germany

  THE DELIVERY OF THIS SOFTWARE AS WELL AS THE HEREBY GRANTED NON-EXCLUSIVE,
  WORLDWIDE LICENSE TO USE, COPY, MODIFY, DISTRIBUTE AND SUBLICENSE THIS
  SOFTWARE IS FREE OF CHARGE.

  THE LICENSED SOFTWARE IS PROVIDED "AS IS" AND INFINEON EXPRESSLY DISCLAIMS
  ALL REPRESENTATIONS AND WARRANTIES, WHETHER EXPRESS OR IMPLIED, INCLUDING
  WITHOUT LIMITATION, WARRANTIES OR REPRESENTATIONS OF WORKMANSHIP,
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, DURABILITY, THAT THE
  OPERATING OF THE LICENSED SOFTWARE WILL BE ERROR FREE OR FREE OF ANY THIRD
  PARTY CLAIMS, INCLUDING WITHOUT LIMITATION CLAIMS OF THIRD PARTY INTELLECTUAL
  PROPERTY INFRINGEMENT.

  EXCEPT FOR ANY LIABILITY DUE TO WILFUL ACTS OR GROSS NEGLIGENCE AND EXCEPT
  FOR ANY PERSONAL INJURY INFINEON SHALL IN NO EVENT BE LIABLE FOR ANY CLAIM
  OR DAMAGES OF ANY KIND, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

 ****************************************************************************/
/**
   \file drv_vinetic_main.c
      Main driver operations as initializations called in open driver
\remarks
      Device member initialization
      General basic functions
      OS independent
*/

/* ============================= */
/* Includes                      */
/* ============================= */

#include "drv_vinetic_api.h"
#include "drv_vinetic_main.h"
#include "drv_vinetic_stream.h"
#include "drv_vinetic_tone.h"
#include "drv_vinetic_con.h"

#ifdef VIN_POLL
   #include "lib_fifo.h"
#endif /* VIN_POLL */

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */


/* ============================= */
/* Global variable definition    */
/* ============================= */

/* Define trace output for vinetic
   OFF:    no output
   HIGH:   only important traces, as errors or some warnings
   NORMAL: including traces from high and general proceedings and
           possible problems
   LOW:    all traces and low level traces as basic chip access
           and interrupts, command data
   Traces can be completely switched off with the compiler switch
   ENABLE_TRACE to 0
*/
CREATE_TRACE_GROUP(VINETIC);
CREATE_LOG_GROUP(VINETIC);
#ifdef RUNTIME_TRACE
CREATE_TRACE_GROUP(VINETIC_RUNTIME_TRACE);
#endif /* RUNTIME_TRACE */

/** what string support, driver version string */
const IFX_char_t DRV_VINETIC_WHATVERSION[] = DRV_VINETIC_WHAT_STR;
#ifdef HAVE_CONFIG_H
/** which access mode is supported */
const IFX_char_t DRV_VINETIC_WHICHACCESS[] = VIN_ACCESS_MODE_STR;
/** which configure options were set */
const IFX_char_t DRV_VINETIC_WHICHCONFIG[] = VIN_CONFIGURE_STR;
#endif /* HAVE_CONFIG_H */

/* ============================= */
/* Global function declaration   */
/* ============================= */

/* ============================= */
/* Local function declaration    */
/* ============================= */

IFX_LOCAL IFX_void_t  InitSigCh     (VINETIC_SIGCH* pSig, IFX_uint8_t ch);
IFX_LOCAL IFX_void_t  InitAlmCh     (VINETIC_ALMCH* pAlm, IFX_uint8_t ch);
IFX_LOCAL IFX_void_t  InitPcmCh     (VINETIC_PCMCH* pPcm, IFX_uint8_t ch,
                                     IFX_uint8_t pcmCh);
IFX_LOCAL IFX_void_t  InitCodCh     (VINETIC_CODCH* pCod, IFX_uint8_t ch);
IFX_LOCAL IFX_int32_t initChMember    (VINETIC_CHANNEL* pCh);
IFX_LOCAL IFX_void_t  ResetDevState (VINETIC_DEVICE *pDev);
#if (VIN_CFG_FEATURES& VIN_FEAT_PACKET)
IFX_LOCAL IFX_return_t InitPacketCh (VINETIC_CHANNEL* pCh);
#endif /* (VIN_CFG_FEATURES& VIN_FEAT_PACKET) */
#ifdef TAPI
IFX_LOCAL IFX_int32_t initTapiDev   (VINETIC_DEVICE* pDev,
                                     IFX_uint8_t nMaxChannel);
#endif /* TAPI */
#ifdef FW_ETU
IFX_LOCAL IFX_int32_t MapEpouTriggerToRfc2833 (IFX_uint16_t epou);
#endif /* FW_ETU */

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* ============================= */
/* Local function definition     */
/* ============================= */

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
      rfcEvent = IFX_TAPI_PKT_EV_NUM_ANS;
   }
   else if (epou & SIG_EPOUTRIG_NANS)
   {
      /* /ANS tone received */
      rfcEvent = IFX_TAPI_PKT_EV_NUM_NANS;
   }
   else if (epou & SIG_EPOUTRIG_ANSAM)
   {
      /* ANSam tone received */
      rfcEvent = IFX_TAPI_PKT_EV_NUM_ANSAM;
   }
   else if (epou & SIG_EPOUTRIG_NANSAM)
   {
      /* /ANSam tone received */
      rfcEvent = IFX_TAPI_PKT_EV_NUM_NANSAM;
   }
   else if (epou & SIG_EPOUTRIG_CNG)
   {
      /* CNG tone received */
      rfcEvent = IFX_TAPI_PKT_EV_NUM_CNG;
   }
   else if (epou & SIG_EPOUTRIG_DIS)
   {
      /* DIS tone received */
      rfcEvent = IFX_TAPI_PKT_EV_NUM_DIS;
   }
   else
   {
      rfcEvent = (-1);
   }

   return rfcEvent;
}
#endif /* FW_ETU */

#ifdef TAPI
/**
   TAPI device structure initialization
\param
   pDev         - pointer to device
\param
   nMaxChannel  - defines maximum number of device channels
\return
   Returns IFX_ERROR in case of an error, otherwise returns IFX_SUCCESS.
\remarks
   Initializes all needed fifos, event queues, memory and sets all needed
   members.
*/
IFX_LOCAL IFX_int32_t initTapiDev(VINETIC_DEVICE* pDev, IFX_uint8_t nMaxChannel)
{
   IFX_uint8_t  nCh;
   TAPI_DEV     *pTapiDev   = &(pDev->TapiDev);
   TAPI_CONNECTION* pChannel;

   /* init tapi device */
   memset(pTapiDev, 0, sizeof(TAPI_DEV));
   /* allocate memory for the tapi structure */
   pTapiDev->pChannelCon =
      (TAPI_CONNECTION *)IFXOS_MALLOC(nMaxChannel * sizeof(TAPI_CONNECTION));
   if (!pTapiDev->pChannelCon)
   {
      SET_ERROR (ERR_NO_MEM);
      return IFX_ERROR;
   }
   /* Initialize channel data */
   memset(pTapiDev->pChannelCon, 0, nMaxChannel * sizeof(TAPI_CONNECTION));

   IFXOS_Init_WakeList(pDev->EventList);
   pDev->bNeedWakeup = IFX_TRUE;

   /* initialize the structure for the channel connections */
   for (nCh = 0; nCh < nMaxChannel; nCh++)
   {
      pChannel = &(pTapiDev->pChannelCon[nCh]);
      /* initialize the pointer to the device structure */
      pChannel->pDevice = (IFX_void_t *) pDev;
      /* initialize the pointer to the tapi device structure */
      pChannel->pTapiDevice = pTapiDev;
      /* initialize each channel with it's channel number */
      pChannel->nChannel = nCh;
      /* mask all exceptions -> disable exceptions */
      pChannel->TapiMiscData.nExceptionMask.Status = 0xFFFFFFFF;
      pDev->pChannel[nCh].pTapiCh = pChannel;
      /* set up FIFO for pulse dialing */
      Fifo_Init(&(pChannel->TapiDialData.TapiPulseFifo),
                pChannel->TapiDialData.pPulseStore,
                &pChannel->TapiDialData.pPulseStore[IFX_TAPI_PULSE_FIFO_SIZE - 1],
                sizeof(IFX_uint8_t));
      Fifo_Init(&(pChannel->TapiDialData.TapiDTMFFifo),
                pChannel->TapiDialData.pDtmfStore,
                &pChannel->TapiDialData.pDtmfStore[IFX_TAPI_DTMF_FIFO_SIZE - 1],
                sizeof(IFX_uint8_t));
#ifdef TAPI_CID
      /* allocate memory for caller id receiver data */
      {
         IFX_void_t *pEnd;
         IFX_void_t *pStart;

         pStart = IFXOS_MALLOC(IFX_TAPI_CID_RX_FIFO_SIZE * sizeof (IFX_TAPI_CID_RX_DATA_t));
         if (pStart == NULL)
         {
            SET_ERROR (ERR_NO_MEM);
            return IFX_ERROR;
         }
         pEnd = ((IFX_TAPI_CID_RX_DATA_t *)pStart) + IFX_TAPI_CID_RX_FIFO_SIZE - 1;
         Fifo_Init(&((pTapiDev->pChannelCon + nCh)->TapiCidRx.TapiCidRxFifo),
                pStart, pEnd, sizeof(IFX_TAPI_CID_RX_DATA_t));
      }
#endif
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
      pChannel->nEvtPT = DEFAULT_EVTPT;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
   }
   /* set the number of maximum channels */
   pTapiDev->nMaxChannel = nMaxChannel;

   return IFX_SUCCESS;
}
#endif /* TAPI */

/**
   OS independent initializations, called on first device open
\param
   pCh       - pointer to VINETIC channel structure
\return
   IFX_SUCCESS if ok, otherwise IFX_ERROR
*/
IFX_LOCAL IFX_int32_t initChMember(VINETIC_CHANNEL* pCh)
{
   IFX_int32_t ret;

#ifdef DEBUG
   pCh->magic = VCH_MAGIC;
#endif /* DEBUG */

   /* init channel lock */
   IFXOS_MutexInit (pCh->chAcc);
   /* select wait queue for reading data */
   IFXOS_Init_WakeList (pCh->SelRead);
   /* init Vinetic CID structure */
   memset (&pCh->cidSend, 0, sizeof (VINETIC_CID));
   /* init cid receiver temporary buffer */
   memset (&pCh->cidRxPacket, 0, sizeof (PACKET));
#ifdef TAPI
   /* allocate timer and status for tone resources */
   pCh->pToneRes = IFXOS_MALLOC(sizeof (VINETIC_TONERES) * TAPI_TONE_MAXRES);
   memset (pCh->pToneRes, 0, sizeof (VINETIC_TONERES) * TAPI_TONE_MAXRES);
#endif /* TAPI */
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
   {
      /* init Channel Fifo */
      pCh->pFifo = NULL;
      /* init queues*/
      IFXOS_InitEvent (pCh->wqRead);
      IFXOS_Init_WakeList (pCh->wqWrite);
      /* init read/write ptr */
      pCh->if_read  = VoIP_UpStream;
      pCh->if_write = VoIP_DownStream;
   }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   {
      pCh->bVoiceConnect = IFX_FALSE;
   }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
   {
      /* Initialization time : No Fax data request. */
      pCh->bFaxDataRequest = IFX_FALSE;

   }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38) */

   /* host channel member initialization */
   ret = VINETIC_Host_InitChMembers  (pCh);

   return ret;
}


/**
   Initalize the coder module and the cached firmware messages
\param
   pCod   - handle to the coder channel structure
   ch     - channel or resource number
*/
IFX_LOCAL IFX_void_t InitCodCh (VINETIC_CODCH* pCod, IFX_uint8_t ch)
{
   int i;

   pCod->signal.nModType = VINDSP_MT_COD;
   pCod->signal.nSignal  = ECMD_IX_COD_OUT0 + ch;
#ifdef DEBUG
   strcpy (pCod->signal.name, signalName[pCod->signal.nSignal]);
#endif /* DEBUG */
   for (i = 0; i < MAX_MODULE_SIGNAL_INPUTS; ++i)
      pCod->signal.in[i].pParent = &pCod->signal;
   /* COD CH */
   pCod->cod_ch.value[0] = CMD1_EOP | ch;
   pCod->cod_ch.value[1] = ECMD_COD_CH;
}

#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
/**
   Initalize the coder packet buffer
\param
   pCh     - channel handle
*/
IFX_LOCAL IFX_return_t InitPacketCh (VINETIC_CHANNEL* pCh)
{
   IFX_return_t err = IFX_SUCCESS;
   PACKET **pPacket = &pCh->pFifo;
   VINETIC_DEVICE* pDev = pCh->pParent;

   /* free ptrs if valid */
   if (pCh->pFifo != IFX_NULL)
      IFXOS_FREE(pCh->pFifo);

   /* allocate memory for channel fifo */
   *pPacket = (PACKET *)IFXOS_MALLOC(sizeof(PACKET) * POBX_FIFO_SIZE);
   if (*pPacket == NULL)
   {
      /* ERROR: No memory for Packet fifo */
      err = IFX_ERROR;
      SET_ERROR (ERR_NO_MEM);
   }
   if (err == IFX_SUCCESS)
   {
      /* initialize packet fifo */
      err = (IFX_return_t)Fifo_Init(&(pCh->rdFifo), (IFX_void_t *) pPacket[0],
                      (IFX_void_t *) &pPacket[0][POBX_FIFO_SIZE - 1],
                      sizeof(PACKET));
      if (err == IFX_ERROR)
      {
         /* ERROR: packet FIFO init failed, check fifo_init description */
         IFXOS_FREE(pCh->rdFifo.pStart);
         SET_ERROR(ERR_FUNC_PARM);
      }
   }
   return err;
}
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */


/**
   Initalize the PCM module and the cached firmware messages
\param
   pPcm   - handle to the PCM channel structure
   ch     - channel or resource number
   pcmCh  - the PCM channel resource to use
*/
IFX_LOCAL IFX_void_t InitPcmCh (VINETIC_PCMCH* pPcm, IFX_uint8_t ch, IFX_uint8_t pcmCh)
{
   int i;

   pPcm->signal.nModType = VINDSP_MT_PCM;
   pPcm->signal.nSignal  = ECMD_IX_PCM_OUT0 + pcmCh;
#ifdef DEBUG
   strcpy (pPcm->signal.name, signalName[pPcm->signal.nSignal]);
#endif /* DEBUG */
   for (i = 0; i < MAX_MODULE_SIGNAL_INPUTS; ++i)
      pPcm->signal.in[i].pParent = &pPcm->signal;
   /* PCM CH with logical resource nunber */
   memset (&pPcm->pcm_ch.value[CMD_HEADER_CNT], 0, CMD_PCM_CH_LEN * 2);
   pPcm->pcm_ch.value[0] = CMD1_EOP | pcmCh;
   pPcm->pcm_ch.value[1] = ECMD_PCM_CH;
   /* EN = 0, NLP = 0, LecNr = ch,
      OLDC = 1: keep filter coefficients,
      as they always will be programmed before activation */
   pPcm->pcm_nelec = PCM_OLDC_EN;
   /* set the default value inside the cached firmware message in order to
      use the PCM-Interface */
   pPcm->pcm_ch.bit.gain_1 = PCM_GAIN_0DB;
   pPcm->pcm_ch.bit.gain_2 = PCM_GAIN_0DB;
   pPcm->pcm_ch.bit.en = 0;
   pPcm->pcm_ch.bit.cod = 2; /* G711- ALaw */
   pPcm->pcm_ch.bit.codnr = ch;
}

/**
   Initalize the analog module and the cached firmware messages
\param
   pAlm   - handle to the analog channel structure
   ch     - channel or resource number
*/
IFX_LOCAL IFX_void_t InitAlmCh (VINETIC_ALMCH* pAlm, IFX_uint8_t ch)
{
   int i;

   /* ALI CH message */
   pAlm->ali_ch.value[0] = CMD1_EOP | ch;
   pAlm->ali_ch.value[1] = ECMD_ALM_CH;
   /* EN = 0, NLP = 0, LecNr is not set,
      OLDC = 1: keep filter coefficients,
      as they always will be programmed before activation.
      The LecNr resource is managed by a minimal resource management with the
      variable 'availLecRes'. The resource is shared with the PCM interface. */
   pAlm->ali_nelec = 0;
   /* ALM module connections */
   pAlm->signal.nModType = VINDSP_MT_ALM;
   pAlm->signal.nSignal  = ECMD_IX_ALM_OUT0 + ch;
   for (i = 0; i < MAX_MODULE_SIGNAL_INPUTS; ++i)
      pAlm->signal.in[i].pParent = &pAlm->signal;
#ifdef DEBUG
   strcpy (pAlm->signal.name, signalName[pAlm->signal.nSignal]);
#endif /* DEBUG */
}

/**
   Initalize the signaling module and the cached firmware messages
\param
   pSig   - handle to the signaling channel structure
   ch     - channel or resource number
*/
IFX_LOCAL IFX_void_t InitSigCh (VINETIC_SIGCH* pSig, IFX_uint8_t ch)
{
   int i;

   pSig->signal.nModType = VINDSP_MT_SIG;
   pSig->signal.nSignal  = ECMD_IX_SIG_OUTA0 + (IFX_uint8_t)(2*ch);
   pSig->signal.nSignal2 = ECMD_IX_SIG_OUTB0 + (IFX_uint8_t)(2*ch);
#ifdef DEBUG
   strcpy (pSig->signal.name, signalName[pSig->signal.nSignal]);
#endif /* DEBUG */
   for (i = 0; i < MAX_MODULE_SIGNAL_INPUTS; ++i)
      pSig->signal.in[i].pParent = &pSig->signal;
   /* SIG CH */
   pSig->sig_ch.value[0] = CMD1_EOP | ch;
   pSig->sig_ch.value[1] = ECMD_SIG_CH;
   pSig->utg[0].value[0]    = CMD1_EOP | ch;
   pSig->utg[0].value[1]    = ECMD_UTG1;
   pSig->utg[0].bit.utgnr   = ch;
   pSig->utg[1].value[0]    = CMD1_EOP | (ch +4);
   pSig->utg[1].value[1]    = ECMD_UTG2;
   pSig->utg[1].bit.utgnr   = ch;
   /* tone detectors */
   pSig->sig_atd1.value[0] = CMD1_EOP | ch;
   pSig->sig_atd1.value[1] = ECMD_SIG_ATD1;
   pSig->sig_atd2.value[0] = CMD1_EOP | ch;
   pSig->sig_atd2.value[1] = ECMD_SIG_ATD2;
   pSig->sig_utd1.value[0] = CMD1_EOP | ch;
   pSig->sig_utd1.value[1] = ECMD_SIG_UTD1;
   pSig->sig_utd2.value[0] = CMD1_EOP | ch;
   pSig->sig_utd2.value[1] = ECMD_SIG_UTD2;
   pSig->sig_mftd.value[0] = CMD1_EOP | ch;
   pSig->sig_mftd.value[1] = ECMD_SIG_MFTD;
   /* DTMF generator */
   pSig->sig_dtmfgen.value[0] = CMD1_EOP | ch;
   pSig->sig_dtmfgen.value[1] = ECMD_DTMF_GEN | CMD_SIG_DTMFGEN_LEN;
   /* CID Send */
   pSig->cid_sender [0] = CMD1_EOP | ch;
   pSig->cid_sender [1] = ECMD_CID_SEND;
}

/**
  Reset all driver state information
\param
   pDev   - handle to the device
\remarks
   Called upon reset of the device
*/
IFX_LOCAL IFX_void_t ResetDevState(VINETIC_DEVICE *pDev)
{
   IFX_int32_t      i;

   /* reset the states variable */
   pDev->nDevState = 0;
   /* reset error */
   pDev->err = ERR_OK;
   /* reset FW caches */
   for (i = 0; i < pDev->nSigCnt; ++i)
   {
      pDev->pSigCh[i].cid_sender[2] = 0;
   }
#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M | VIN_FEAT_VIN_2CPE))
   /* reset the version register to empty values */
   pDev->nEdspVers[0] = NOTVALID;
   for (i = 1; i < CMD_VERS_FEATURES_EXT_LEN; i++)
   {
      pDev->nEdspVers[i] = 0;
   }
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */
#ifdef TAPI
   for (i = 0; i < VINETIC_MAX_ANA_CH_NR; i ++)
   {
      TAPI_CONNECTION *pChannel = pDev->pChannel[i].pTapiCh;
      /* reset tapi flags */
      if (pChannel != NULL)
      {
         pChannel->bInstalled = IFX_FALSE;
         /* reset tapi exeption status */
         pChannel->TapiMiscData.nException.Status = 0;
         /* reset default ringmode */
         pChannel->TapiRingData.RingConfig.nMode =
                                     IFX_TAPI_RING_CFG_MODE_INTERNAL_BALANCED;
         pChannel->TapiRingData.RingConfig.nSubmode =
                                     IFX_TAPI_RING_CFG_SUBMODE_DC_RNG_TRIP_STANDARD;
      }
   }
   /* reset TS management flags */
   memset(pDev->PcmRxTs, 0, sizeof(pDev->PcmRxTs));
   memset(pDev->PcmTxTs, 0, sizeof(pDev->PcmTxTs));
#endif /* TAPI */
#ifdef TESTING
   /* last command and short command */
   memset(pDev->lastCmd, 0xFF, sizeof(pDev->lastCmd));
#endif /* TESTING */

   VINETIC_Host_ResetDevMembers (pDev);
}

/* ============================= */
/* Global function definition    */
/* ============================= */

/**
   OS independent initializations, called only on first device open
\param
   pDev      - pointer to VINETIC device structure
\return
   IFX_SUCCESS if ok, otherwise IFX_ERROR
\remarks
   Called in open when the device memory was allocated. This happens only
   once, cause the device structure is cached.
   Another open would not lead to this function cause the memory is already
   available.
*/
IFX_int32_t VINETIC_InitDevMember(VINETIC_DEVICE* pDev)
{
   IFX_int32_t ret, i;

#ifdef DEBUG
   pDev->magic = VDEV_MAGIC;
#endif /* DEBUG */
   /* initialize mailbox protection semaphore */
   IFXOS_MutexInit (pDev->mbxAcc);
   /* initialize share variables protection semaphore */
   IFXOS_MutexInit(pDev->memberAcc);
   /* set polling mode */
   pDev->IrqPollMode = VIN_VOICE_POLL | VIN_EVENT_POLL;
   /* mark for reading */
   pDev->nAnaChan   = 0;
#ifdef ADDED_SUPPORT
   /* Allocate and Initialize the Device Interrupt
      Error Status FIFO */
   if (InitErrFifo (pDev) == IFX_ERROR)
        return IFX_ERROR;
#endif /* ADDED_SUPPORT */
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
   /* set function pointer for data packets */
   pDev->getDataBuf = getPacketPtr;
   pDev->putDataBuf = FifoPut;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
#ifdef VIN_POLL
   /* Initialize the FIFO for polling */
   pDev->pFifo  = (IFX_void_t *)fifoInit(24);

   if (pDev->pFifo != NULL)
   {
      TRACE(VINETIC, DBG_LEVEL_NORMAL, ("FIFO initialized (size %d)\n\r",
            fifoSize((FIFO_ID *) pDev->pFifo)));
   }
   else
      TRACE(VINETIC, DBG_LEVEL_HIGH, ("ERROR initializing FIFO\n\r"));
#endif /* VIN_POLL */
#ifdef TAPI
   /* Initialize TAPI structure */
   if (initTapiDev(pDev, VINETIC_MAX_CH_NR) == IFX_ERROR)
      return IFX_ERROR;
#endif /* TAPI */
#ifdef TESTING
   pDev->lastCmd[0] = 0;
   pDev->lastCmd[1] = 0;
#endif /* TESTING */
#ifdef EVALUATION
      if (Eval_Init(pDev) == IFX_ERROR)
         return IFX_ERROR;
#endif /* EVALUATION */
   ret = VINETIC_Host_InitDevMembers (pDev);
   /* initialize all channel structures */
   if (ret == IFX_SUCCESS)
   {
      for (i = 0; i < VINETIC_MAX_CH_NR; i++)
      {
         pDev->pChannel[i].pParent = pDev;
         pDev->pChannel[i].nChannel = (IFX_uint8_t)i + 1;
         if (initChMember(&pDev->pChannel[i]) == IFX_ERROR)
            return IFX_ERROR;
      }
   }
#ifdef VIN_MB_ACC_TRACE
   /* Initialize the mailbox access trace buffer inside the driver */
   /* As default use 'VINETIC_MB_ACC_DEFAULT_ENTRIES' mailbox accesses for the
      size of the trace buffer.*/
   {
      VINETIC_MB_ACC_TRACE *pTrace;
      IFX_uint32_t len;

      len = sizeof(VINETIC_MB_ACC_TRACE) * VINETIC_MB_ACC_DEFAULT_ENTRIES;
      pTrace = malloc(len);
      if (pTrace == NULL)
         return IFX_ERROR;

      /* reset the memory to zero */
      memset(pTrace, 0, len);

      pDev->pMbAccTrStart = pTrace;
      pDev->pMbAccTrWrite = pTrace;
      pDev->pMbAccTrEnd = pTrace + VINETIC_MB_ACC_DEFAULT_ENTRIES;
   }
#endif /* VIN_MB_ACC_TRACE */

   return ret;
}

/**
   Initalize the shadow device Firmware modules (PCM/SIGNALLING/ALM/CODER)
\param
   pDev   - handle to the device structure
\return
   IFX_SUCCESS or IFX_ERROR, if no memory is available
*/
IFX_int32_t VINETIC_InitDevFWData(VINETIC_DEVICE *pDev)
{
   IFX_uint8_t i;
   IFX_int32_t ret;

   /* set default */
   pDev->nMaxRes     = 4;
   pDev->nCoderCnt   = 4;
   pDev->nSigCnt     = 4;
   pDev->nPcmCnt     = 4;
   pDev->nToneGenCnt = 2;
   pDev->VinCapabl.bWlec     = IFX_FALSE;
   pDev->VinCapabl.nUtgPerCh = 1;
   pDev->VinCapabl.nLecRes   = 4;
   /* call host fw init function, to overide above default values */
   ret = VINETIC_Host_InitDevFWData(pDev);
   if (ret == IFX_ERROR)
   {
      /* \todo specific error type */
      return IFX_ERROR;
   }
   /* set the available LEC resource in the firmware as free .
      One bit represent one available LEC resource */
   pDev->availLecRes = (1 << pDev->nMaxRes) - 1;
   /* free ptrs if valid */
   if (pDev->pAlmCh!= IFX_NULL)
      IFXOS_FREE(pDev->pAlmCh);
   if (pDev->pCodCh != IFX_NULL)
      IFXOS_FREE(pDev->pCodCh);
   if (pDev->pPcmCh != IFX_NULL)
      IFXOS_FREE(pDev->pPcmCh);
   if (pDev->pSigCh != IFX_NULL)
      IFXOS_FREE(pDev->pSigCh);
   if (pDev->nAnaChan > 0)
   {
      pDev->pAlmCh= IFXOS_MALLOC(sizeof(VINETIC_ALMCH) * pDev->nAnaChan);
      if (pDev->pAlmCh== NULL)
      {
         SET_ERROR (ERR_NO_MEM);
         return IFX_ERROR;
      }
      memset(pDev->pAlmCh, 0, sizeof(VINETIC_ALMCH) * pDev->nAnaChan);
   }
   if (pDev->nCoderCnt > 0)
   {
      pDev->pCodCh = IFXOS_MALLOC(sizeof(VINETIC_CODCH) * pDev->nCoderCnt);
      if (pDev->pCodCh == NULL)
      {
         SET_ERROR (ERR_NO_MEM);
         return IFX_ERROR;
      }
      memset(pDev->pCodCh, 0, sizeof(VINETIC_CODCH) * pDev->nCoderCnt);
   }
   if (pDev->nPcmCnt > 0)
   {
      pDev->pPcmCh = IFXOS_MALLOC(sizeof(VINETIC_PCMCH) * pDev->nPcmCnt);
      if (pDev->pPcmCh == NULL)
      {
         SET_ERROR (ERR_NO_MEM);
         return IFX_ERROR;
      }
      memset(pDev->pPcmCh, 0, sizeof(VINETIC_PCMCH) * pDev->nPcmCnt);
   }
   if (pDev->nSigCnt > 0)
   {
      pDev->pSigCh = IFXOS_MALLOC(sizeof(VINETIC_SIGCH) * pDev->nSigCnt);
      if (pDev->pSigCh == NULL)
      {
         SET_ERROR (ERR_NO_MEM);
         return IFX_ERROR;
      }
      memset(pDev->pSigCh, 0, sizeof(VINETIC_SIGCH) * pDev->nSigCnt);
   }
   for (i = 0; i < pDev->nAnaChan; ++i)
   {
      InitAlmCh (&pDev->pAlmCh[i], i);
   }
   for (i = 0; i < pDev->nCoderCnt; ++i)
   {
      InitCodCh (&pDev->pCodCh[i], i);
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
      /* allocate memory for packet data when we got coder channel,
         only if we aren't in voice polling mode */
      if ((pDev->IrqPollMode & VIN_VOICE_POLL) == 0)
      {
         InitPacketCh (&pDev->pChannel[i]);
      }
#endif /* (VIN_CFG_FEATURES& VIN_FEAT_PACKET) */
   }
   switch (pDev->nChipMajorRev)
   {
#ifdef VIN_V14_SUPPORT
      case VINETIC_V1x:
         {
            IFX_uint8_t j;
            /* Define PCM channel resources:
               As the 16bit linear mode needs 2 channel resources on
               Vinetic V1.x.
               The first half of the PCM channels we use is every
               second one (0,2,4,6,...).
               The second half of the used PCM channels are the one
               in between (1,3,5,7,...).
               The first half can be used for 8-Bit mode and 16-Bit mode.
               The upper half can only be used if the whole device is
               running in 8-Bit mode.
               The capability keeping all available PCM resources.
            */
            for (i = 0, j = 0; i < (pDev->nPcmCnt / 2); i++, j += 2)
            {
               pDev->pChannel[i].nPcmCh = (IFX_uint8_t)j;
               pDev->pChannel[i].nPcmMaxResolution = (IFX_uint8_t)16;
            }
            for (j = 1; i < pDev->nPcmCnt; i++, j += 2)
            {
               pDev->pChannel[i].nPcmCh = (IFX_uint8_t)j;
               pDev->pChannel[i].nPcmMaxResolution = (IFX_uint8_t)8;
            }
         }
         break;
#endif /* VIN_V14_SUPPORT */
#if (defined VIN_V21_SUPPORT) || (defined VIN_2CPE)
      case VINETIC_V2x:
         for (i = 0; i < pDev->nPcmCnt; i++)
         {
            pDev->pChannel[i].nPcmCh = (IFX_uint8_t)i;
            pDev->pChannel[i].nPcmMaxResolution = (IFX_uint8_t)16;
         }
         break;
#endif /* VIN_V21_SUPPORT */
      default:
         SET_ERROR (ERR_NO_VERSION);
         return IFX_ERROR;
         break;
   }

   for (i = 0; i < pDev->nPcmCnt; ++i)
   {
      InitPcmCh (&pDev->pPcmCh[i], i, pDev->pChannel[i].nPcmCh);
   }
   for (i = 0; i < pDev->nSigCnt; ++i)
   {
      InitSigCh (&pDev->pSigCh[i], i);
   }

   return IFX_SUCCESS;
}

/**
   Initalizes channel timers)
\param
   pCh   - handle to the channel structure
\return
   IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t VINETIC_InitChTimers (VINETIC_CHANNEL* pCh)
{
#ifdef TAPI
   IFX_uint32_t i;

   for (i = 0; i < pCh->pParent->nToneGenCnt; ++i)
   {
      /* safety check : memory allocated for only TAPI_TONE_MAXRES */
      if (i >= TAPI_TONE_MAXRES)
         break;
      if (pCh->pToneRes[i].Tone_Timer == 0)
      {
         /* initialize (create) voice path teardown timer */
         pCh->pToneRes[i].Tone_Timer = TAPI_Create_Timer((TIMER_ENTRY)Tone_OnTimer,
            (IFX_int32_t)&pCh->pToneRes[i]);
         pCh->pToneRes[i].pCh = pCh;
         pCh->pToneRes[i].nRes = i;
         if (pCh->pToneRes[i].Tone_Timer == 0)
         {
            return IFX_ERROR;
         }
      }
#ifndef VIN_2CPE /* No Tone generator on analog side for 2CPE */
      if (pCh->Tone_PredefTimer == 0)
      {
         /* create the tone timer */
         pCh->Tone_PredefTimer =
            TAPI_Create_Timer ((TIMER_ENTRY)Tone_PredefOnTimer,
                               (IFX_int32_t)pCh);
      }
#endif /* VIN_2CPE */
   }
#endif /* TAPI */

   return IFX_SUCCESS;
}

/**
   Basic initializations of the device.
\param
   pDev              - pointer to the device structure
\param
   pBasicDeviceInit  - pointer to the device init parameters
\return
   IFX_SUCCESS if ok, otherwise IFX_ERROR
\remarks
   Must be the first action after installing the driver and
   before any access to the hardware (corresponding vinetic chip or channel).
*/
IFX_int32_t VINETIC_BasicDeviceInit(VINETIC_DEVICE *pDev,
                                    VINETIC_BasicDeviceInit_t *pBasicDeviceInit)
{
   IFX_int32_t err = IFX_SUCCESS;

   /* reset this device */
   ResetDevState(pDev);
   /* initialize base address in case the access isn't via SPI/SCI */
   if ((pBasicDeviceInit->AccessMode != VIN_ACCESS_SCI) &&
       (pBasicDeviceInit->AccessMode != VIN_ACCESS_SPI))
   {
      err = OS_Init_Baseadress(pDev, pBasicDeviceInit->nBaseAddress);
   }
#ifdef VIN_2CPE
   else
   {
#ifdef VIN_SPI
      if (pBasicDeviceInit->AccessMode == VIN_ACCESS_SPI)
      {
         pDev->hostDev.nSPIDevAddr = (pBasicDeviceInit->nBaseAddress &
                                      V2CPE_SPI_DEVADDR_MASK);
      }
      else
#endif /* VIN_SPI */
      {
         SET_ERROR(ERR_FUNC_PARM);
         err = IFX_ERROR;
      }
   }
#endif /* VIN_2CPE */
   /* set access methods (functions, pointers)*/
   if (err == IFX_SUCCESS)
   {
      err = VINETIC_Host_SetupAccess (pDev, pBasicDeviceInit->AccessMode);
   }
   if (err==IFX_SUCCESS)
   {
      if (pBasicDeviceInit->nIrqNum < 0)
      {
         pDev->IrqPollMode = VIN_VOICE_POLL | VIN_EVENT_POLL;
      }
      else
      {
         OS_Install_VineticIRQHandler(pDev, pBasicDeviceInit->nIrqNum);

         if (pDev->pIrq == IFX_NULL)
            pDev->IrqPollMode = VIN_VOICE_POLL | VIN_EVENT_POLL;
         else
            pDev->IrqPollMode = VIN_TAPI_WAKEUP;
      }
   }
   if (err == IFX_SUCCESS)
   {
      pDev->nDevState |= DS_BASIC_INIT;
   }

   return err;
}

/**
   Reset of the device.
\param
   pDev              - pointer to the device structure
\return
   IFX_SUCCESS if ok, otherwise IFX_ERROR
\remarks
   Can be used to reset device internal state after a hard reset from
   application in case the whole device initialization done before is
   still valid.
*/
IFX_int32_t  VINETIC_DeviceReset(VINETIC_DEVICE *pDev)
{
   IFX_uint32_t nDevState = pDev->nDevState;

   /* reset this device */
   ResetDevState(pDev);

   /* set back init bit */
   pDev->nDevState = (nDevState & DS_BASIC_INIT);

   return IFX_SUCCESS;
}


/**
   Executes the select for the channel fd
\param
   pCh  - handle to channel control structure
\param
   node   - node list
\param
   opt    - optinal argument, which contains needed information for
           IFXOS_SleepQueue
\return
   System event qualifier. Either 0 or IFXOS_SYSREAD
\remarks
   This function needs operating system services, that are hidden by
   IFXOS macros.
*/
IFX_int32_t VINETIC_SelectCh (VINETIC_CHANNEL* pCh, IFX_int32_t node,
                              IFX_int32_t opt)
{
   IFX_uint32_t  flags = 0;
   IFX_int32_t   ret = 0;

   IFXOS_SleepQueue(pCh->SelRead, node, opt);
#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
   IFXOS_SleepQueue (pCh->wqWrite, node, opt);
   if ((pCh->pTapiCh->TapiFaxStatus.nStatus & IFX_TAPI_FAX_T38_TX_ON) &&
       (pCh->bFaxDataRequest == IFX_TRUE))
   {
      /* task should write a new packet now */
      ret |= IFXOS_SYSWRITE;
   }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38) */
   /* select on a voice channel -- only implemented for TAPI */
   flags |= CF_NEED_WAKEUP;
   /* clear flags first, then apply new flags */
   Vinetic_IrqLockDevice (pCh->pParent);
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
   if (!Fifo_isEmpty (&pCh->rdFifo))
   {
      flags |= CF_WAKEUPSRC_STREAM;
      flags &= ~CF_NEED_WAKEUP;
      ret |= IFXOS_SYSREAD;
   }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
   pCh->nFlags &= ~(CF_WAKEUPSRC_GR909 | CF_WAKEUPSRC_STREAM |
                       CF_WAKEUPSRC_TAPI | CF_NEED_WAKEUP);
   pCh->nFlags |= flags;
   Vinetic_IrqUnlockDevice (pCh->pParent);

   return ret;
}

/**
   Updates specific channel members on channel open.
\param
   pCh       - pointer to VINETIC channel structure
\return
   IFX_SUCCESS if ok, otherwise IFX_ERROR
\remarks
   none
*/
IFX_int32_t VINETIC_UpdateChMember(VINETIC_CHANNEL* pCh)
{
   IFX_int32_t err = IFX_SUCCESS;

#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
   Fifo_Clear (&(pCh->rdFifo));
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */

   return err;
}

/**
   Dispatches the command read, by copying it into the appropriate pDev/pCh
   command caching buffer.
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
IFX_void_t VINETIC_DispatchCmd (VINETIC_DEVICE *pDev, IFX_uint16_t const *pData)
{
   IFX_uint8_t i, len, ch;

   ch = pData[0] & CMD1_CH;
   switch (pData[0] & CMD1_CMD)
   {
   case  CMD1_EOP:
      len = (pData[1] & CMD2_LEN);
      switch ((pData[1] & ~CMD2_LEN) & (CMD2_ECMD | CMD2_MOD) )
      {
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
            pDev->pAlmCh[ch].ali_ch.value[CMD_HEADER_CNT + i] = pData[CMD_HEADER_CNT + i];
         break;
      case ECMD_PCM_CH:
         /* check if ch matches to number of analog ressources */
         if (ch >= pDev->nPcmCnt)
            break;
         for (i = 0; (i < len) && (i < CMD_PCM_CH_LEN); i++)
            pDev->pPcmCh[ch].pcm_ch.value[CMD_HEADER_CNT + i] = pData[CMD_HEADER_CNT + i];
         break;
      case ECMD_COD_CH:
         /* check if ch matches to number of coder ressources */
         if (ch >= pDev->nCoderCnt)
            break;
         for (i = 0; (i < len) && (i < CMD_COD_CH_LEN); i++)
            pDev->pCodCh[ch].cod_ch.value[CMD_HEADER_CNT + i] = pData[CMD_HEADER_CNT + i];
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

/**
  Frees all memory previously allocated in VINETIC_InitDevMember
\param
   pDev   - handle to the device
\remarks
   Called when device is released. Nobody uses this device
*/
IFX_void_t VINETIC_ExitDev(VINETIC_DEVICE *pDev)
{
    int i, j;
#ifdef TAPI
   VINETIC_CHANNEL *pCh;
   TAPI_DEV *pTapiDev = &(pDev->TapiDev);
   TAPI_CONNECTION *pChannel;
#endif /* TAPI */

   /* delete device mutexes */
   IFXOS_MutexDelete (pDev->mbxAcc);
   IFXOS_MutexDelete (pDev->memberAcc);

   /* delete channel mutexes */
   for (i = 0; i < VINETIC_MAX_CH_NR; i++)
   {
      IFXOS_MutexDelete (pDev->pChannel[i].chAcc);
   }

   /* free fw caches */
   IFXOS_FREE(pDev->pAlmCh);
   IFXOS_FREE(pDev->pCodCh);
   IFXOS_FREE(pDev->pPcmCh);
   IFXOS_FREE(pDev->pSigCh);
#ifdef TAPI
   for (i = 0; i < VINETIC_MAX_CH_NR; i++)
   {
      pCh = &pDev->pChannel[i];
      for (j = 0; j < pDev->nToneGenCnt; ++j)
      {
         if (pCh->pToneRes[j].Tone_Timer != 0)
            TAPI_Delete_Timer (pCh->pToneRes[j].Tone_Timer);
      }
      if (pCh->Tone_PredefTimer != 0)
         TAPI_Delete_Timer (pCh->Tone_PredefTimer);
      /* free tone resource */
      IFXOS_FREE(pCh->pToneRes);
   }
   for (i= 0; i < pTapiDev->nMaxChannel; i++)
   {
      pChannel = &pTapiDev->pChannelCon[i];
#ifdef TAPI_CID
      IFXOS_FREE(pChannel->TapiCidRx.TapiCidRxFifo.pStart);
#endif /* TAPI_CID */
      if (pChannel->TapiMeterData.MeterTimerID != NULL)
      {
         TAPI_Delete_Timer (pChannel->TapiMeterData.MeterTimerID);
         pChannel->TapiMeterData.MeterTimerID = 0;
      }
      if (pChannel->TapiMiscData.GndkhTimerID != NULL)
      {
         TAPI_Delete_Timer (pChannel->TapiMiscData.GndkhTimerID);
         pChannel->TapiMiscData.GndkhTimerID = 0;
      }
      if (pChannel->TapiDialData.DialTimerID != NULL)
      {
         TAPI_Delete_Timer (pChannel->TapiDialData.DialTimerID);
         pChannel->TapiDialData.DialTimerID = 0;
      }
      if (pChannel->TapiRingData.RingTimerID)
      {
         TAPI_Delete_Timer (pChannel->TapiRingData.RingTimerID);
         pChannel->TapiRingData.RingTimerID = 0;
      }
#ifdef TAPI_CID
      if (pChannel->TapiCidTx.CidTimerID)
      {
         TAPI_Delete_Timer (pChannel->TapiCidTx.CidTimerID);
         pChannel->TapiCidTx.CidTimerID = 0;
      }
#endif /* TAPI_CID */
   }
   /* free tapi connection structure */
   IFXOS_FREE(pTapiDev->pChannelCon);
   /* free capability structure */
   IFXOS_FREE (pDev->CapList);
#endif /* TAPI */
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
   for (i = 0; i < VINETIC_MAX_CH_NR; i++)
   {
      IFXOS_FREE (pDev->pChannel[i].pFifo);
   }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
#ifdef EVALUATION
#ifndef VIN_2CPE
   IFXOS_FREE (((VINETIC_EVAL*)pDev->pEval)->irqFifo.pStart);
   for (i = 0; i < VINETIC_MAX_CH_NR; i++)
   {
      IFXOS_FREE (pDev->pChannel[i].pEval);
   }
#endif /* VIN_2CPE */
   IFXOS_FREE (pDev->pEval);
#endif /* EVALUATION */
#ifdef ADDED_SUPPORT
   /* Deallocate the Device Interrupt */
   /* Error Status FIFO               */
    FreeErrFifo (pDev);
#endif /* ADDED_SUPPORT */
}

