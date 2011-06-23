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
#include "drv_vinetic_parallel.h"
#include "drv_vinetic_serial.h"
#include "drv_vinetic_dwnld.h"
#include "drv_vinetic_cram.h"
#include "drv_vinetic_tone.h"

#if (VIN_CFG_FEATURES & VIN_FEAT_LT)
   #include "drv_vinetic_lt.h"
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_LT) */

#include "drv_vinetic_con.h"

#ifdef VIN_POLL
   #include "lib_fifo.h"
#endif /* VIN_POLL */


/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

/* how often to try to read interrupt status register to
   be zero after a abort occurs */
#define MAX_IR_CHECK 10

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

/* ============================= */
/* Global function declaration   */
/* ============================= */

/* ============================= */
/* Local function declaration    */
/* ============================= */

IFX_LOCAL IFX_int32_t InitChMember    (VINETIC_CHANNEL* pCh);
IFX_LOCAL IFX_int32_t initChRegStruct (VINETIC_CHANNEL *pCh);
IFX_LOCAL IFX_int32_t InitDevRegStruct(VINETIC_DEVICE *pDev);
IFX_LOCAL IFX_int32_t clearInt        (VINETIC_DEVICE *pDev);
IFX_LOCAL void InitSigCh (VINETIC_SIGCH* pSig, IFX_uint8_t ch);
IFX_LOCAL void InitAlmCh (VINETIC_ALMCH* pAlm, IFX_uint8_t ch);
IFX_LOCAL void InitPcmCh (VINETIC_PCMCH* pPcm, IFX_uint8_t ch, IFX_uint8_t pcmCh);
IFX_LOCAL void InitCodCh (VINETIC_CODCH* pCod, IFX_uint8_t ch);
#if (VIN_CFG_FEATURES& VIN_FEAT_PACKET)
IFX_LOCAL IFX_return_t InitPacketCh (VINETIC_CHANNEL* pCh);
#endif /* (VIN_CFG_FEATURES& VIN_FEAT_PACKET) */

#ifdef TAPI
IFX_LOCAL IFX_int32_t initTapiDev     (VINETIC_DEVICE* pDev, IFX_uint8_t nMaxChannel);
static int initDevData     (VINETIC_DEVICE *pDev);
#endif /* TAPI */
IFX_LOCAL IFX_void_t UpdateMember(VINETIC_DEVICE* pDev, IFX_uint32_t nFlags);

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* ============================= */
/* Local function definition     */
/* ============================= */


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
   /*  default mode for ringing:
       pChannel->TapiRingData.RingConfig.nMode = INTERNAL_BALANCED = 0; */

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
      /* Initialize Tapi queue */
      /* set up FIFO for pulse dialing */
      Fifo_Init(&(pChannel->TapiDialData.TapiPulseFifo),
                pChannel->TapiDialData.pPulseStore,
                &pChannel->TapiDialData.pPulseStore[TAPI_DIAL_FIFO_SIZE - 1],
                sizeof(IFX_uint8_t));
      Fifo_Init(&(pChannel->TapiDialData.TapiDTMFFifo),
                pChannel->TapiDialData.pDtmfStore,
                &pChannel->TapiDialData.pDtmfStore[TAPI_DTMF_FIFO_SIZE - 1],
                sizeof(IFX_uint8_t));
#ifdef TAPI_CID
      /* allocate memory for caller id receiver data */
      {
         IFX_void_t *pEnd;
         IFX_void_t *pStart;

         pStart = IFXOS_MALLOC(TAPI_CIDRX_FIFO_SIZE * sizeof (TAPI_CIDRX_DATA));
         if (pStart == NULL)
         {
            SET_ERROR (ERR_NO_MEM);
            return IFX_ERROR;
         }
         pEnd = ((TAPI_CIDRX_DATA *)pStart) + TAPI_CIDRX_FIFO_SIZE - 1;
         Fifo_Init(&((pTapiDev->pChannelCon + nCh)->TapiCidRx.TapiCidRxFifo),
                   pStart, pEnd, sizeof(TAPI_CIDRX_DATA));
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

/* ============================= */
/* Global function definition    */
/* ============================= */

/**
   Updates all member of the device and all channel structures according
   to the flags
\param
   pDev      - pointer to VINETIC device structure
\param
   nFlags    - given operation flags for the device or channels, see CH_FLAGS
\remarks
   Sets the default streaming function
*/
IFX_LOCAL IFX_void_t UpdateMember(VINETIC_DEVICE* pDev, IFX_uint32_t nFlags)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
   IFX_int32_t      i;
   VINETIC_CHANNEL  *pCh;

   /* initialize flags for channel */
   for (i = 0; i < VINETIC_CH_NR; i++)
   {
      pCh = &pDev->pChannel[i];
      pCh->if_read  = VoIP_UpStream;
      pCh->if_write = VoIP_DownStream;
   }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
#ifdef TESTING
   pDev->lastSC = 0;
   pDev->lastCmd[0] = 0;
   pDev->lastCmd[1] = 0;
#endif /* TESTING */
}

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
   This function allocates all neccessary memory and initializes the fields
   by calling ResetDevMember.
*/
IFX_int32_t InitDevMember(VINETIC_DEVICE* pDev)
{
   IFX_int32_t i;

#ifdef DEBUG
   pDev->magic = VDEV_MAGIC;
#endif /* DEBUG */
   /* initialize mailbox protection semaphore */
   IFXOS_MutexInit (pDev->mbxAcc);
   /* initialize share variables protection semaphore */
   IFXOS_MutexInit(pDev->memberAcc);
   /* set polling mode */
   pDev->IrqPollMode = VIN_VOICE_POLL | VIN_EVENT_POLL;
   /* init fields first time, also the uC modes are set up here */
   if (ResetDevMember(pDev) == IFX_ERROR)
      return IFX_ERROR;
   /* initialize all channel structures */
   for (i = 0; i < VINETIC_CH_NR; i++)
   {
      pDev->pChannel[i].pParent = pDev;
      pDev->pChannel[i].nChannel = (IFX_uint8_t)i + 1;
      if (InitChMember(&pDev->pChannel[i]) == IFX_ERROR)
         return IFX_ERROR;
   }
   /* reset the driver states to default */
   UpdateMember(pDev, 0);

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

   InitDevRegStruct(pDev);
#ifdef TAPI
   /* Initialize TAPI structure */
   if (initTapiDev(pDev, VINETIC_CH_NR) == IFX_ERROR)
      return IFX_ERROR;
#endif /* TAPI */

   return IFX_SUCCESS;
}

/**
   OS independent initializations, called on first device open
\param
   pCh       - pointer to VINETIC channel structure
\return
   IFX_SUCCESS if ok, otherwise IFX_ERROR
*/
IFX_LOCAL IFX_int32_t InitChMember(VINETIC_CHANNEL* pCh)
{
#ifdef DEBUG
   pCh->magic = VCH_MAGIC;
#endif /* DEBUG */
   /* init channel lock */
   IFXOS_MutexInit (pCh->chAcc);
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
   /* init Channel Fifo */
   pCh->pFifo = NULL;
   /* init queues*/
   IFXOS_InitEvent (pCh->wqRead);
   IFXOS_Init_WakeList (pCh->wqWrite);
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
   IFXOS_InitEvent (pCh->wqLT_LM_OK);
   IFXOS_InitEvent (pCh->wqLT_RAMP_RDY);
   /* select wait queue for reading data */
   IFXOS_Init_WakeList (pCh->SelRead);
#ifdef TAPI
   /* reset line testing config structure */
   memset(&pCh->ltConf, 0, sizeof(VINETIC_LT_CONF));
#if (VIN_CFG_FEATURES & VIN_FEAT_GR909)
   /* GR909 initializations */
   IFXOS_InitEvent (pCh->Gr909.wqState);
   memset (&pCh->Gr909.reg, 0, sizeof (VINETIC_GR909_REG));
   memset (&pCh->Gr909.Usr, 0, sizeof (VINETIC_IO_GR909));
   pCh->Gr909.bData      = IFX_FALSE;
   pCh->Gr909.nState     = 0;
   pCh->Gr909.gr909tskId = -1;
   pCh->Gr909.Gr909Call  = NULL;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_GR909) */
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   pCh->bVoiceConnect = IFX_FALSE;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
#endif /* TAPI */
   /* init Vinetic CID structure */
   memset (&pCh->cidSend, 0, sizeof (VINETIC_CID));
   /* init cid receiver temporary buffer */
   memset (&pCh->cidRxPacket, 0, sizeof (PACKET));
   /* Set register init values */
#ifdef AM5120
   pCh->initIOCTL1 = 0x0001;
#else
   pCh->initIOCTL1 = 0x0000;
#endif /* AM5120 */
#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
   {
      /* Initialization time : No Fax data request. */
      pCh->bFaxDataRequest = IFX_FALSE;

   }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38) */
   /* DUP-OVT=9ms, DUP=10ms, DUP-IO=16.5ms */
   pCh->initIOCTL2 = 0x2094;
   pCh->initMF_SRS1 = 0xffff;
   pCh->initMR_SRS1 = 0xffff;
   /* allocate timer and status for tone resources */
   pCh->pToneRes = IFXOS_MALLOC(sizeof (VINETIC_TONERES) * TAPI_TONE_MAXRES);
   memset (pCh->pToneRes, 0, sizeof (VINETIC_TONERES) * TAPI_TONE_MAXRES);
   return IFX_SUCCESS;
}
/** Initializes all channel related timers
\param
   pCh   - handle to the channel structure
\return
   IFX_SUCCESS or IFX_ERROR, if no memory is available
*/
IFX_int32_t InitChTimers (VINETIC_CHANNEL* pCh)
{
#ifdef TAPI
   IFX_uint32_t i;
   for (i = 0; i < pCh->pParent->nToneGenCnt; ++i)
   {
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
      if (pCh->Tone_PredefTimer == 0)
      {
         /* create the tone timer */
         pCh->Tone_PredefTimer =
            TAPI_Create_Timer ((TIMER_ENTRY)Tone_PredefOnTimer,
                               (IFX_int32_t)pCh);
      }
   }
#endif /* TAPI */
   return IFX_SUCCESS;
}

/**
   OS independent field initializations, called on open for the
   first time.
\param
   pDev      - pointer to VINETIC device structure
\return
   IFX_SUCCESS if ok, otherwise IFX_ERROR
\remarks
   Called by InitDevMember to reset all fields
*/
IFX_int32_t ResetDevMember(VINETIC_DEVICE* pDev)
{
   /* mark for reading */
   pDev->nAnaChan   = 0;

#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
   /* set function pointer for data packets */
   pDev->getDataBuf = getPacketPtr;
   pDev->putDataBuf = FifoPut;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */

   /* set active mode to use on this device (system dependent)*/
   pDev->nActiveMode = SC_ACT_HIGH;

   return IFX_SUCCESS;
}

/**
   OS independent initializations for one channel of the parent
   device, called on channel first open
\param
   pCh       - pointer to VINETIC channel structure
\return
   IFX_SUCCESS if ok, otherwise IFX_ERROR
\remarks
   none
*/
IFX_int32_t ResetChMember(VINETIC_CHANNEL* pCh)
{
   IFX_int32_t err = IFX_SUCCESS;
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
   Fifo_Clear (&(pCh->rdFifo));
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */

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
IFX_int32_t  Device_Reset(VINETIC_DEVICE *pDev)
{
   IFX_uint32_t nDevState = pDev->nDevState;

   /* reset this device */
   ResetDevState(pDev);

   /* set back init bit */
   pDev->nDevState = (nDevState & DS_BASIC_INIT);

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
IFX_int32_t BasicDeviceInit(VINETIC_DEVICE *pDev,
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

   /* set access function's  ptr */
   if (err==IFX_SUCCESS)
   {
      err = VINETIC_Setup_Access_Function(pDev,pBasicDeviceInit->AccessMode);
   }
   /* set addresses, not SCI/SPI relevant */
   if ((err == IFX_SUCCESS) &&
       ((pBasicDeviceInit->AccessMode != VIN_ACCESS_SCI) &&
       (pBasicDeviceInit->AccessMode != VIN_ACCESS_SPI)))
   {
      err = VINETIC_Setup_Access_Pointer(pDev,pBasicDeviceInit->AccessMode);
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
   Setup the access functions (read,write,...) depending on the access mode
\param
   pDev     - pointer to the device structure
\param
   nAccess  - VINETIC access mode: VIN_ACCESS_PAR_16BIT, VIN_ACCESS_PAR_8BIT, VIN_ACCESS_SPI, VIN_ACCESS_SCI
\return
    IFX_SUCCESS if no error, otherwise IFX_ERROR
*/
IFX_int32_t VINETIC_Setup_Access_Function(VINETIC_DEVICE *pDev, VIN_ACCESS nAccess)
{
   switch (nAccess)
   {
#ifdef VIN_8BIT
#ifndef SIMULATION
   case VIN_ACCESS_PARINTEL_MUX8:
   case VIN_ACCESS_PARINTEL_DMUX8_LE:
      pDev->write       = parint8bit_write;
      pDev->read        = parint8bit_read;
      pDev->diop_read   = parint8bit_diop_read;
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
      pDev->packet_read = parint8bit_packet_read;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
      break;
#endif /* #ifndef SIMULATION */
   case VIN_ACCESS_PARINTEL_DMUX8_BE:
   case VIN_ACCESS_PAR_8BIT:
      pDev->write       = par8bit_write;
      pDev->read        = par8bit_read;
      pDev->diop_read   = par8bit_diop_read;
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
      pDev->packet_read = par8bit_packet_read;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
      break;
#endif /* VIN_8BIT */
   case VIN_ACCESS_PAR_8BIT_V2:
   case VIN_ACCESS_PAR_16BIT:
   case VIN_ACCESS_PARINTEL_MUX16:
      pDev->write       = parallel_write;
      pDev->read        = parallel_read;
      pDev->diop_read   = parallel_diop_read;
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
      pDev->packet_read = parallel_packet_read;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
      break;
#ifdef VIN_SPI
   case VIN_ACCESS_SPI:
   case VIN_ACCESS_SCI:
      pDev->write       = serial_write;
      pDev->read        = serial_read;
      pDev->diop_read   = serial_diop_read;
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
      pDev->packet_read = serial_read_packet;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
      break;
#endif  /* VIN_SPI */
   default:
      SET_ERROR (ERR_FUNC_PARM);
      return IFX_ERROR;
   }

   return IFX_SUCCESS;
}

/**
   Switch the access mode
\param
   pDev     - pointer to the device structure
\param
   nAccess  - VINETIC access mode: VIN_ACCESS_PAR_16BIT, VIN_ACCESS_PAR_8BIT, VIN_ACCESS_SPI, VIN_ACCESS_SCI
\return
    IFX_SUCCESS if no error, otherwise IFX_ERROR
*/
IFX_int32_t VINETIC_Setup_Access_Pointer(VINETIC_DEVICE *pDev, VIN_ACCESS nAccess)
{
   /* Switch address for EOM, cause in case of multiplexed it is different */
   if (nAccess == VIN_ACCESS_PARINTEL_MUX8 || nAccess == VIN_ACCESS_PARINTEL_MUX16)
   {
      /* set up multiplexed mode */
#if defined(VIN_V21_SUPPORT) && defined(VIN_V14_SUPPORT)
      switch (pDev->nChipMajorRev)
      {
#ifdef VIN_V14_SUPPORT
      case VINETIC_V1x:
         pDev->pNwd     = pDev->pBaseAddr + (VINETIC_NWD << VIN_1X_PARACC_MUX_SHIFT);
         pDev->pEom     = pDev->pBaseAddr + (VINETIC_EOM_MUX << VIN_1X_PARACC_MUX_SHIFT);
         pDev->pIntReg  = pDev->pBaseAddr + (VINETIC_ISR << VIN_1X_PARACC_MUX_SHIFT);
         break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
      case VINETIC_V2x:
         pDev->pNwd     = pDev->pBaseAddr + (VINETIC_NWD << VIN_2X_PARACC_MUX_SHIFT);
         pDev->pEom     = pDev->pBaseAddr + (VINETIC_EOM << VIN_2X_PARACC_MUX_SHIFT);
         pDev->pIntReg  = pDev->pBaseAddr + (VINETIC_ISR << VIN_2X_PARACC_MUX_SHIFT);
         break;
#endif /* VIN_V21_SUPPORT */
      default:
         TRACE (VINETIC, DBG_LEVEL_HIGH,
            ("ERROR: Access Mode %d not supported for chip version %d!\n\r",
               nAccess, pDev->nChipMajorRev));
         return IFX_ERROR;
      }
#endif /* defined(VIN_V21_SUPPORT) && defined(VIN_V14_SUPPORT) */
   }
   else
   {
      /* no mux mode */
      pDev->pNwd     = pDev->pBaseAddr + (VINETIC_NWD << VIN_PARACC_SHIFT);
      pDev->pEom     = pDev->pBaseAddr + (VINETIC_EOM << VIN_PARACC_SHIFT);
      pDev->pIntReg  = pDev->pBaseAddr + (VINETIC_ISR << VIN_PARACC_SHIFT);
   }
   return IFX_SUCCESS;
}

/**
  Initialize the VINETIC Chip
\param
   pDev      - pointer to the device structure
\param
   pCmd      - handle to the initialization structure
\return
    IFX_SUCCESS or IFX_ERROR
\remarks
   This function sets the initial configuration, downloads
   PRAM/DRAM Firmware to VINETIC, and starts the EDSP.
*/
IFX_int32_t VINETIC_Init(VINETIC_DEVICE *pDev, VINETIC_IO_INIT *pInit)
{
   IFX_int32_t err = IFX_SUCCESS;
   IFX_uint8_t i   = 0;
   /* for checking the revision */
   IFX_uint16_t pRvCmd[2] = {0x8801, 0x8001}, nRev = 0;
   /* update some pointers */
   UpdateMember(pDev, pInit->nFlags);
   /* check if previous initialization was done */
   if (pDev->nDevState & DS_DEV_INIT)
      return IFX_SUCCESS;


   Vinetic_IrqLockDevice (pDev);
   /* set Chip Revision and Number of Analog Channels */
   /* Note : According to chip revision, it will be
      decided if a PHI download is necessary or not */
   /* read out chip details, like version and supported channels  */
   err = pDev->diop_read (pDev, pRvCmd, &nRev, 1);
   if (err == IFX_SUCCESS)
   {
      pDev->nChipRev = VIN_DEV_REVISION_REV_GET(nRev);

      if ((pDev->nChipRev >= VINETIC_V13) && (pDev->nChipRev <= VINETIC_V16))
      {
#ifndef VIN_V14_SUPPORT
         pDev->nChipMajorRev = 0;
         TRACE(VINETIC, DBG_LEVEL_HIGH, ("Vinetic V1.x not supported!\n\r"));
#else
         pDev->nChipMajorRev = VINETIC_V1x;
#endif /* !VIN_V14_SUPPORT */
      }
      else
      /* last major version is V2.x. Assume next version are compatible */
      if (pDev->nChipRev >= VINETIC_V21)
      {
#ifndef VIN_V21_SUPPORT
         pDev->nChipMajorRev = 0;
         TRACE(VINETIC, DBG_LEVEL_HIGH, ("Vinetic V2.x not supported!\n\r"));
#else
         pDev->nChipMajorRev = VINETIC_V2x;
#endif /* !VIN_V21_SUPPORT */
      }
      else
         pDev->nChipMajorRev = 0;

      pDev->nAnaChan = VIN_DEV_REVISION_ANACHAN_GET(nRev);
      pDev->nChipType = VIN_DEV_REVISION_Type_GET(nRev);
      if (pDev->nAnaChan > VINETIC_CH_NR)
      {
         SET_ERROR (ERR_NO_VERSION);
         return IFX_ERROR;
      }
   }
   if (err != IFX_SUCCESS || pDev->nChipMajorRev == 0)
   {
      SET_ERROR (ERR_NO_VERSION);
      err = IFX_ERROR;
   }
   if (err == IFX_SUCCESS)
   {
      /* Initialize Channel Register Structure */
      for (i = 0; i < pDev->nAnaChan; i++)
      {
         if (err == IFX_SUCCESS)
            err = initChRegStruct (&pDev->pChannel[i]);
      }
   }
   if (err == IFX_SUCCESS)
   {
      for (i = 0; i < VINETIC_CH_NR; i++)
      {
         /* Define PCM channel resources:
            As the 16bit linear mode needs 2 channel resources on Vinetic V1.x,
            we use only every second one and set the capability to 8
            (instead of 16). For the Vinetic V2.x we can use all channels,
            but there are only 8!
         */
         switch (pDev->nChipMajorRev)
         {
#ifdef VIN_V14_SUPPORT
         case VINETIC_V1x:
            pDev->pChannel[i].nPcmCh = (IFX_uint8_t)(i*2);
            break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
         case VINETIC_V2x:
            pDev->pChannel[i].nPcmCh = (IFX_uint8_t)i;
            break;
#endif /* VIN_V21_SUPPORT */
         default:
            SET_ERROR (ERR_NO_VERSION);
            err = IFX_ERROR;
            break;
         }
      }
   }

   /* download PHI micro programm if desired */
   if (err == IFX_SUCCESS)
      err = Dwld_LoadPhi (pDev, pInit);

   /* do the rest only if the phi download was done */
   if (err == IFX_SUCCESS)
   {
      /* clear interrupt register and interupt status registers */
      err = clearInt(pDev);
      /* enable the the irq line */
      if (err == IFX_SUCCESS)
      {
         /* clear already pending interrupts */
         /* allow device interrupts */
         Vinetic_IrqUnlockDevice(pDev);
      }

      /* download firmware */
#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
      if (err == IFX_SUCCESS)
         err = Dwld_LoadFirmware(pDev, pInit);
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */
      if (err == IFX_SUCCESS)
         err = initDevData(pDev);
      /* Initialize VINETIC by setting mask registers to appropriate values */
      if ((err == IFX_SUCCESS) &&
         ((  (pInit->nFlags & NO_EDSP_START) == IFX_FALSE) ||
            (pDev->nChipType == VINETIC_TYPE_S)) )
      {
         err = InitVinetic(pDev);
#ifdef VIN_V14_SUPPORT
         /* set CRAM coefficients if EDSP was started succesfully */
         if (err == IFX_SUCCESS)
         {
            /* currently only available for V1.x chips */
            if ((pDev->nChipMajorRev == VINETIC_V1x) &&
               ((pInit->nFlags & NO_CRAM_DWLD) == 0))
               err = Dwld_LoadCram(pDev, pInit);
         }
#endif /* VIN_V14_SUPPORT */
      }
#ifdef VIN_V14_SUPPORT
      /* download DC Control */
      /* Note: Needs to be done after setting the reset value of BCR1 in
               InitVinetic! */
      if ((err == IFX_SUCCESS) && (pInit->nFlags & DC_DWLD))
      {
         err = Dwld_LoadFpiCode (pDev, pInit, DC_CODE);
      }
      /* download AC parameters */
      if ((err == IFX_SUCCESS) && ((pInit->nFlags & NO_AC_DWLD) == 0))
      {
         err = Dwld_LoadFpiCode (pDev, pInit, AC_CODE);
      }
#endif /* VIN_V14_SUPPORT */

#if (VIN_CFG_FEATURES & VIN_FEAT_LT)
      /* init linetesting configuration */
      if (err == IFX_SUCCESS)
         err = VINETIC_LT_Init ();
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_LT) */
   }
 /* 
   if (err == IFX_SUCCESS) 
	   printk("\npDev->nDevState |= DS_DEV_INIT\n");
   else
	   printk("\nerr = IFX_ERROR\n");
  */
   if (err == IFX_SUCCESS)
      pDev->nDevState |= DS_DEV_INIT;

   /* This function disables the interrupt line of the device or the global
      interrupts instead. This is done by calling 'Vinetic_IrqLockDevice'.
      In case of an error the interrupt line is not unlocked again, but it
      is necessary to unlock the global interrupt of the system. */
   if (err != IFX_SUCCESS)
      Vinetic_IrqUnlockGlobal (pDev);

   return err;
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
IFX_int32_t VINETIC_SelectCh (VINETIC_CHANNEL* pCh, IFX_int32_t node, IFX_int32_t opt)
{
   IFX_uint32_t  flags = 0;
   IFX_int32_t   ret = 0;

   /* calling function ensures != null  */
   IFXOS_ASSERT (pCh != IFX_NULL);
   IFXOS_SleepQueue(pCh->SelRead, node, opt);
#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
   IFXOS_SleepQueue (pCh->wqWrite, node, opt);
   if ((pCh->pTapiCh->TapiFaxStatus.nStatus & TAPI_FAX_T38_TX_ON) &&
       (pCh->bFaxDataRequest == IFX_TRUE))
      ret |= IFXOS_SYSWRITE;
      /* task should write a new packet now */
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
  Initialize the vinetic
\param
   pDev   - pointer to the device structure
\return
   Error code
\remarks
   Initializes the needed interrupt masks and default register values
*/
IFX_int32_t InitVinetic(VINETIC_DEVICE *pDev)
{
   VINETIC_CHANNEL *pCh;
   IFX_int32_t     err = IFX_SUCCESS;
   IFX_uint8_t i;
   IFX_uint16_t    pMaskR[2] = {0}, pMaskF[2] = {0}, nVal;

   /* Unmask rising edge of CERR.
   */
   pMaskR [0] = ~BXSR1_CERR;
   err = RegWrite(pDev, CMD1_IOP, pMaskR, 1, MRBXSR1);
   /* Mask MBX_EMPTY.
      Unmask rising edge of
      HOST-ERROR, POBX, COBX, PIBX-OF, CIBX-OF */
   pMaskR [0] = 0xFFC0 | BXSR2_MBX_EMPTY;
#ifndef ASYNC_CMD
   /* for async commands we need the signal to read data from the mailbox */
   pMaskR [0] |= BXSR2_COBX_DATA;
#endif /* ASYNC_CMD */
   /* Mask all  falling edge of BXSR2 */
   pMaskF [0] = 0xFFFF;
   if (err == IFX_SUCCESS)
      err = RegWrite(pDev, CMD1_IOP, pMaskR, 1, MRBXSR2);
   if (err == IFX_SUCCESS)
      err = RegWrite(pDev, CMD1_IOP, pMaskF, 1, MFBXSR2);

   /* mask falling edge of CLK-FAIL since not working correctely,
      V1.3 errata sheet DS5 */
   pMaskR [0] = 0xF7F6 | HWSR1_MCLK_FAIL;
   if (err == IFX_SUCCESS)
      err = RegWrite(pDev, CMD1_IOP, pMaskR, 1, MRHWSR1);

   /* the S-chip (or S-FW) does not use SRE1/2 */
   if ((err == IFX_SUCCESS) && (pDev->nChipType != VINETIC_TYPE_S))
   {
      /* Set all fields to 0xFFFF : disable states */
      memset(pMaskR, 0xff, sizeof(pMaskR));
      memset(pMaskF, 0xff, sizeof(pMaskF));
#ifdef TAPI
      /* Enable DTMF and CID rising egde interrupts in SRE1.
      Falling edges should be disabled. DTMFR-DT [0:3] are masked also */
      pMaskR [0] = 0x7FC0;
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
      /* Disable UTG activation rising edge 0 -> 1 interrupt */
      pMaskR [0] |= SRE1_UTG_ACT;
      /* Enable UTG deactivation falling edge 1 -> 0 interrupt */
      pMaskF [0] &= ~(SRE1_UTG_ACT);
      /* Enable UTD2_OK for Fax network tone detection */
      /* detect only 0 -> 1 transition */
      pMaskR[0] &= ~SRE1_UTD2_OK;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
#ifdef TAPI_CID
      /* Enable 0 -> 1 transition of CIDR_OF interrupt */
      pMaskR [0] &= ~SRE1_CIDR_OF;
#endif
#endif /* TAPI */
      /* enable packet overflow rising edge interrupt only */
      pMaskR[1] &= ~SRE2_PVPUOF;
#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
      /* Enable FDP_REQ and FDP_ERR */
      pMaskR [1] &= ~(SRE2_FDP_REQ | SRE2_FDP_ERR);
#elif defined (ASYNC_CMD)
      /* Enable DEC_CHG */
      pMaskR [1] &= ~SRE2_DEC_CHG;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38) / ASYNC_CMD*/
#if (VIN_CFG_FEATURES & (VIN_FEAT_VOICE))
#ifdef FW_ETU
      pMaskR [1] &= ~SRE2_EPOU_TRIG;
#endif /* FW_ETU */
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VOICE)) */
      /* set masks */
      for (i = 0; i < pDev->nMaxRes; i++)
      {
         /* Note: Since IOP commands do not work as broadcasts for
                  Vinetic 1.4 the initialisation has to be done in a loop */
         /* set rising edge masks */
         if (err == IFX_SUCCESS)
            err = RegWrite(pDev, (CMD1_IOP | i), pMaskR, 2, MRSRE1);
         /* set falling edge masks */
         if (err == IFX_SUCCESS)
            err = RegWrite(pDev, (CMD1_IOP | i), pMaskF, 2, MFSRE1);
         /* cache register values for later use */
         pDev->regMFSRE1[i] = pMaskF[0];
         pDev->regMFSRE2[i] = pMaskF[1];
         pDev->regMRSRE1[i] = pMaskR[0];
         pDev->regMRSRE2[i] = pMaskR[1];
      }

      if (pDev->nChipRev == VINETIC_V14)
      {
         /* set default (reset) values for ALM, see errata sheet V1.4*/
         nVal = 0x2000;
         if (err == IFX_SUCCESS)
            err = RegWrite(pDev, (CMD1_SOP | CMD1_BC), &nVal, 1, SOP_OFFSET_BCR1);
         nVal = 0x00C0;
         if (err == IFX_SUCCESS)
            err = RegWrite(pDev, (CMD1_SOP | CMD1_BC), &nVal, 1, SOP_OFFSET_TSTR2);
      }
   }

   /* set LM-SEL to DC current on IT per default
      to enable a correct DC characteristic */
   if (err == IFX_SUCCESS)
   {
      nVal = 0x0005;
      err = RegWrite(pDev, (CMD1_SOP | CMD1_BC), &nVal, 1, SOP_OFFSET_LMCR);
   }

   /* Initialize external analog multiplexer */
   /* The configuration might have been stored before using the ioctls
      FIO_VINETIC_DEV_GPIO_CFG and FIO_VINETIC_DEV_GPIO_SET. */
   if (err == IFX_SUCCESS)
      err = wrReg ((VINETIC_CHANNEL *) pDev, GCR1_REG);
   if (err == IFX_SUCCESS)
      err = wrReg ((VINETIC_CHANNEL *) pDev, GCR2_REG);
   if (err == IFX_SUCCESS)
      err = wrReg ((VINETIC_CHANNEL *) pDev, MRSRGPIO_REG);
   if (err == IFX_SUCCESS)
      err = wrReg ((VINETIC_CHANNEL *) pDev, MFSRGPIO_REG);

   for (i = 0; i < pDev->nAnaChan; i++)
   {
      pCh = &pDev->pChannel[i];
      /* Enable Hook and Ground Key rising and falling edge interrupts */
      MR_SRS1.value &= ~(SRS1_HOOK | SRS1_GNDK | SRS1_GNDKP);
      MF_SRS1.value &= ~(SRS1_HOOK | SRS1_GNDK | SRS1_GNDKP | SRS1_VTRLIM);
      /* enable OTEMP rising edge interrupt only */
      MR_SRS2.value &= ~(SRS2_OTEMP);
      if (err == IFX_SUCCESS)
      {
         err = wrReg (pCh, IOCTL2_REG);
      }
      if (err == IFX_SUCCESS)
      {
         err = wrReg (pCh, IOCTL1_REG);
      }
      if (err == IFX_SUCCESS)
      {
         err = wrReg (pCh, MF_SRS1_REG);
      }
      if (err == IFX_SUCCESS)
      {
         err = wrReg (pCh, MR_SRS1_REG);
      }
      if (err == IFX_SUCCESS)
         err = wrReg (pCh, MR_SRS2_REG);
   }
   return err;
}


/**
  Clears all interrupt
\param
   pDev  - handle to the device
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   This function must be save from interrupts
   If ISR gets not to zero this function returns an error code
*/
IFX_LOCAL IFX_int32_t clearInt(VINETIC_DEVICE *pDev)
{
   IFX_uint16_t nIr = 0, i = 0,
        pIr [4] = {0};
   IFX_int32_t err = IFX_SUCCESS;

   /* read with short command */
   err = SC_READ(SC_RIR, &nIr, 1);
   for (i = 0; i <= MAX_IR_CHECK; i ++)
   {
      if (nIr != 0)
      {
         /* clear hardware interrupts */
         if ((err == IFX_SUCCESS) && (nIr & IR_HW_STAT))
         {
            err = SC_READ (SC_RIHWSR, pIr, 2);
         }
         /* clear mbx interrupts */
         if ((err == IFX_SUCCESS) && (nIr & IR_MBX_EVT))
         {
            err = SC_READ (SC_RIBXSR, pIr, 2);
         }
         /* clear srgpio interrupts */
         if ((err == IFX_SUCCESS) && (nIr & IR_GPIO))
         {
            err = SC_READ (SC_RISRGPIO, pIr, 1);
         }
         /* clear reset interrupts */
         if ((err == IFX_SUCCESS) && (nIr & IR_RESET))
         {
            err = SC_READ (SC_RIR, pIr, 1);
         }
         /* check ir again one more time */
         if (err == IFX_SUCCESS)
         {
            err = SC_READ (SC_RIR, &nIr, 1);
         }
      }
      if (nIr == 0)
         break;
   }
   if ((err == IFX_SUCCESS) && (nIr != 0))
   {
      SET_ERROR (ERR_INTSTUCK);
      err = IFX_ERROR;
   }
   /* check status */
   if (err != IFX_SUCCESS)
      TRACE(VINETIC, DBG_LEVEL_HIGH,
           ("ERROR: While clearing interrupts, IR = 0x%04X(SC)\n\r", nIr));

   return err;
}

/**
  Reset all driver state information
\param
   pDev   - handle to the device
\remarks
   Called upon reset of the device
*/
IFX_void_t ResetDevState(VINETIC_DEVICE *pDev)
{
   IFX_int32_t i;

   /* reset register event flags */
   for (i = 0; i < MAX_IRQ_EVENTS ; i++)
   {
      pDev->IsrFlag [i] = 0;
   }
   /* clear all event semaphores */
   /* reset the states variable */
   pDev->nDevState = 0;
   pDev->err = ERR_OK;
#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
   pDev->nEdspVers = NOTVALID;
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */

#ifdef TAPI
   /* reset TS management flags */
   memset(pDev->PcmRxTs, 0, sizeof(pDev->PcmRxTs));
   memset(pDev->PcmTxTs, 0, sizeof(pDev->PcmTxTs));
#endif /* #ifdef TAPI */

#ifdef TESTING
   /* last command and short command */
   memset(pDev->lastCmd, 0xFF, sizeof(pDev->lastCmd));
   pDev->lastSC = 0xFFFF;
   /* set value in EDSP status register HWSR2 */
#endif /* TESTING */
#ifdef TAPI
   for (i = 0; i < VINETIC_ANA_CH_NR; i ++)
   {
      /* reset line testing configuration structure */
      memset(&pDev->pChannel[i].ltConf, 0, sizeof(VINETIC_LT_CONF));
      /* reset tapi flags */
      if (pDev->pChannel[i].pTapiCh != NULL)
      {
         pDev->pChannel[i].pTapiCh->bInstalled = IFX_FALSE;
         /* reset tapi exeption status */
         pDev->pChannel[i].pTapiCh->TapiMiscData.nException.Status = 0;
         /* reset default ringmode */
         pDev->pChannel[i].pTapiCh->TapiRingData.RingConfig.nMode = INTERNAL_BALANCED;
         pDev->pChannel[i].pTapiCh->TapiRingData.RingConfig.nSubmode = DC_RNG_TRIP_STANDARD;
      }
   }
#endif /* TAPI */
   for (i = 0; i < pDev->nSigCnt; ++i)
      pDev->pSigCh[i].cid_sender[2] = 0;
}

/**
  Frees all memory previously allocated in InitDevMember
\param
   pDev   - handle to the device
\remarks
   Called when device is released. Nobody uses this device
*/
IFX_void_t ExitDev(VINETIC_DEVICE *pDev)
{
    int i, j;
#ifdef TAPI
   VINETIC_CHANNEL *pCh;
   TAPI_DEV *pTapiDev = &(pDev->TapiDev);
   TAPI_CONNECTION *pChannel;
#endif /* TAPI */

   IFXOS_MutexDelete (pDev->mbxAcc);
   IFXOS_MutexDelete (pDev->memberAcc);
#ifdef TAPI
   for (i = 0; i < VINETIC_CH_NR; i++)
   {
      pCh = &pDev->pChannel[i];
      for (j = 0; j < pDev->nToneGenCnt; ++j)
      {
         if (pCh->pToneRes[j].Tone_Timer != 0)
            TAPI_Delete_Timer (pCh->pToneRes[j].Tone_Timer);
      }
      if (pCh->Tone_PredefTimer != 0)
         TAPI_Delete_Timer (pCh->Tone_PredefTimer);
      if (pCh->pToneRes != NULL)
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
   }
   IFXOS_FREE(pTapiDev->pChannelCon);
#endif /* TAPI */
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
   for (i = 0; i < pDev->nCoderCnt; ++i)
   {
      if (pDev->pChannel[i].pFifo != NULL)
         IFXOS_FREE (pDev->pChannel[i].pFifo);
   }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
   if (pDev->pAlmCh)
      IFXOS_FREE(pDev->pAlmCh);
   if (pDev->pCodCh)
      IFXOS_FREE(pDev->pCodCh);
   if (pDev->pPcmCh)
      IFXOS_FREE(pDev->pPcmCh);
   if (pDev->pSigCh)
      IFXOS_FREE(pDev->pSigCh);
   if (pDev->CapList != NULL)
      IFXOS_FREE (pDev->CapList);
}

/**
   Initalize the shadow channel registers
   This functions has to be called after vinetic hw revision was detected
   in VINETIC_Init.
\param
   pCh   - handle to the channel structure
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   none
*/
IFX_LOCAL IFX_int32_t initChRegStruct (VINETIC_CHANNEL *pCh)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t             err  = IFX_SUCCESS;

   /* Init Register structures */
   pCh->regLMRES.address = 0x03;
   pCh->regLMRES.regType = CMD1_SOP;
   pCh->regLMRES.length = 1;
   strncpy(pCh->regLMRES.name, "LMRES", 15);
   LMRES.value = 0x0000;
   pCh->regLMRES.setVal = NULL;
   pCh->regLMRES.getVal = NULL;
   pCh->regCCHKR.address = 0x04;
   pCh->regCCHKR.regType = CMD1_SOP;
   pCh->regCCHKR.length = 1;
   strncpy(pCh->regCCHKR.name, "CCHKR", 15);
   CCHKR.value = 0x0000;
   pCh->regCCHKR.setVal = NULL;
   pCh->regCCHKR.getVal = NULL;
   pCh->regIOCTL1.address = 0x05;
   pCh->regIOCTL1.regType = CMD1_SOP;
   pCh->regIOCTL1.length = 1;
   strncpy(pCh->regIOCTL1.name, "IOCTL1", 15);
   IOCTL1.value = pCh->initIOCTL1;
   pCh->regIOCTL1.setVal = NULL;
   pCh->regIOCTL1.getVal = NULL;
   pCh->regIOCTL2.address = 0x06;
   pCh->regIOCTL2.regType = CMD1_SOP;
   pCh->regIOCTL2.length = 1;
   strncpy(pCh->regIOCTL2.name, "IOCTL2", 15);
   IOCTL2.value = pCh->initIOCTL2;
   pCh->regIOCTL2.setVal = NULL;
   pCh->regIOCTL2.getVal = NULL;
   pCh->regBCR1.address = 0x07;
   pCh->regBCR1.regType = CMD1_SOP;
   pCh->regBCR1.length = 1;
   strncpy(pCh->regBCR1.name, "BCR1", 15);
   BCR1.value = 0x2000;
   pCh->regBCR1.setVal = NULL;
   pCh->regBCR1.getVal = NULL;
   pCh->regBCR2.address = 0x08;
   pCh->regBCR2.regType = CMD1_SOP;
   pCh->regBCR2.length = 1;
   strncpy(pCh->regBCR2.name, "BCR2", 15);
   BCR2.value = 0x0000;
   pCh->regBCR2.setVal = NULL;
   pCh->regBCR2.getVal = NULL;
   pCh->regDSCR.address = 0x09;
   pCh->regDSCR.regType = CMD1_SOP;
   pCh->regDSCR.length = 1;
   strncpy(pCh->regDSCR.name, "DSCR", 15);
   DSCR.value = 0x0000;
   pCh->regDSCR.setVal = NULL;
   pCh->regDSCR.getVal = NULL;
   pCh->regLMCR.address = 0x0a;
   pCh->regLMCR.regType = CMD1_SOP;
   pCh->regLMCR.length = 1;
   strncpy(pCh->regLMCR.name, "LMCR", 15);
   LMCR.value = 0x0200;
   pCh->regLMCR.setVal = NULL;
   pCh->regLMCR.getVal = NULL;
   pCh->regRTR.address = 0x0b;
   pCh->regRTR.regType = CMD1_SOP;
   pCh->regRTR.length = 1;
   strncpy(pCh->regRTR.name, "RTR", 15);
   RTR.value = 0x0000;
   pCh->regRTR.setVal = NULL;
   pCh->regRTR.getVal = NULL;
   pCh->regOFR.address = 0x0c;
   pCh->regOFR.regType = CMD1_SOP;
   pCh->regOFR.length = 1;
   strncpy(pCh->regOFR.name, "OFR", 15);
   OFR.value = 0x0000;
   pCh->regOFR.setVal = NULL;
   pCh->regOFR.getVal = NULL;
   pCh->regAUTOMOD.address = 0x0d;
   pCh->regAUTOMOD.regType = CMD1_SOP;
   pCh->regAUTOMOD.length = 1;
   strncpy(pCh->regAUTOMOD.name, "AUTOMOD", 15);
   AUTOMOD.value = 0x0000;
   pCh->regAUTOMOD.setVal = NULL;
   pCh->regAUTOMOD.getVal = NULL;
   pCh->regTSTR1.address = 0x0e;
   pCh->regTSTR1.regType = CMD1_SOP;
   pCh->regTSTR1.length = 1;
   strncpy(pCh->regTSTR1.name, "TSTR1", 15);
   TSTR1.value = 0x0000;
   pCh->regTSTR1.setVal = NULL;
   pCh->regTSTR1.getVal = NULL;
   pCh->regTSTR2.address = 0x0f;
   pCh->regTSTR2.regType = CMD1_SOP;
   pCh->regTSTR2.length = 1;
   strncpy(pCh->regTSTR2.name, "TSTR2", 15);
   TSTR2.value = 0x00c0;
   pCh->regTSTR2.setVal = NULL;
   pCh->regTSTR2.getVal = NULL;
   pCh->regTSTR3.address = 0x10;
   pCh->regTSTR3.regType = CMD1_SOP;
   pCh->regTSTR3.length = 1;
   strncpy(pCh->regTSTR3.name, "TSTR3", 15);
   TSTR3.value = 0x0000;
   pCh->regTSTR3.setVal = NULL;
   pCh->regTSTR3.getVal = NULL;
   pCh->regMR_SRS1.address = 0x02;
   pCh->regMR_SRS1.regType = CMD1_IOP;
   pCh->regMR_SRS1.length = 1;
   strncpy(pCh->regMR_SRS1.name, "MR-SRS1", 15);
   MR_SRS1.value = pCh->initMR_SRS1;
   pCh->regMR_SRS1.setVal = NULL;
   pCh->regMR_SRS1.getVal = NULL;
   pCh->regMR_SRS2.address = 0x03;
   pCh->regMR_SRS2.regType = CMD1_IOP;
   pCh->regMR_SRS2.length = 1;
   strncpy(pCh->regMR_SRS2.name, "MR-SRS2", 15);
   MR_SRS2.value = 0xffff;
   pCh->regMR_SRS2.setVal = NULL;
   pCh->regMR_SRS2.getVal = NULL;
   pCh->regMF_SRS1.address = 0x62;
   pCh->regMF_SRS1.regType = CMD1_IOP;
   pCh->regMF_SRS1.length = 1;
   strncpy(pCh->regMF_SRS1.name, "MF-SRS1", 15);
   MF_SRS1.value = pCh->initMF_SRS1;
   pCh->regMF_SRS1.setVal = NULL;
   pCh->regMF_SRS1.getVal = NULL;
   pCh->regMF_SRS2.address = 0x63;
   pCh->regMF_SRS2.regType = CMD1_IOP;
   pCh->regMF_SRS2.length = 1;
   strncpy(pCh->regMF_SRS2.name, "MF-SRS2", 15);
   MF_SRS2.value = 0xffff;
   pCh->regMF_SRS2.setVal = NULL;
   pCh->regMF_SRS2.getVal = NULL;
   pCh->regOPMOD_CUR.address = 0x21;
   pCh->regOPMOD_CUR.regType = CMD1_IOP;
   pCh->regOPMOD_CUR.length = 1;
   strncpy(pCh->regOPMOD_CUR.name, "OPMOD-CUR", 15);
   pCh->regOPMOD_CUR.address = 0x21;
   pCh->regOPMOD_CUR.regType = CMD1_IOP;
   pCh->regOPMOD_CUR.length = 1;
   strncpy(pCh->regOPMOD_CUR.name, "OPMOD-CUR", 15);

   OPMOD_CUR.value = 0x0000;
   pCh->regOPMOD_CUR.setVal = NULL;
   pCh->regOPMOD_CUR.getVal = NULL;

   switch (pDev->nChipMajorRev)
   {
#ifdef VIN_V14_SUPPORT
      case VINETIC_V1x:
         pCh->regTG1F.address = 0x18;
         pCh->regTG1F.regType = CMD1_COP;
         pCh->regTG1F.length = 1;
         strncpy(pCh->regTG1F.name, "TG1F", 15);
         TG1F.value = 0x0000;
         pCh->regTG1F.setVal = CRAM_getTGFreqHex;
         pCh->regTG1F.getVal = CRAM_getTGFreqVal;
         pCh->regBP1F.address = 0x19;
         pCh->regBP1F.regType = CMD1_COP;
         pCh->regBP1F.length = 1;
         strncpy(pCh->regBP1F.name, "BP1F", 15);
         BP1F.value = 0x0000;
         pCh->regBP1F.setVal = CRAM_getBPFreqHex;
         pCh->regBP1F.getVal = NULL;
         pCh->regTG2F.address = 0x1A;
         pCh->regTG2F.regType = CMD1_COP;
         pCh->regTG2F.length = 1;
         strncpy(pCh->regTG2F.name, "TG2F", 15);
         TG2F.value = 0x0000;
         pCh->regTG2F.setVal = CRAM_getTGFreqHex;
         pCh->regTG2F.getVal = CRAM_getTGFreqVal;
         pCh->regBP2F.address = 0x1B;
         pCh->regBP2F.regType = CMD1_COP;
         pCh->regBP2F.length = 1;
         strncpy(pCh->regBP2F.name, "BP2F", 15);
         BP2F.value = 0x0000;
         pCh->regBP2F.setVal = CRAM_getBPFreqHex;
         pCh->regBP2F.getVal = NULL;
         pCh->regTG1A.address = 0x1C;
         pCh->regTG1A.regType = CMD1_COP;
         pCh->regTG1A.length = 1;
         strncpy(pCh->regTG1A.name, "TG1A", 15);
         TG1A.value = 0x0000;
         pCh->regTG1A.setVal = CRAM_getTGAmpHex;
         pCh->regTG1A.getVal = CRAM_getTGAmpVal;
         pCh->regTG2A.address = 0x1D;
         pCh->regTG2A.regType = CMD1_COP;
         pCh->regTG2A.length = 1;
         strncpy(pCh->regTG2A.name, "TG2A", 15);
         TG2A.value = 0x0000;
         pCh->regTG2A.setVal = CRAM_getTGAmpHex;
         pCh->regTG2A.getVal = CRAM_getTGAmpVal;
         pCh->regBP12Q.address = 0x1E;
         pCh->regBP12Q.regType = CMD1_COP;
         pCh->regBP12Q.length = 1;
         strncpy(pCh->regBP12Q.name, "BP12Q", 15);
         BP12Q.value = 0x0000;
         pCh->regBP12Q.setVal = NULL;
         pCh->regBP12Q.getVal = NULL;
         pCh->regBP3F.address = 0x40;
         pCh->regBP3F.regType = CMD1_COP;
         pCh->regBP3F.length = 1;
         strncpy(pCh->regBP3F.name, "BP3F", 15);
         BP3F.value = 0x0000;
         pCh->regBP3F.setVal = CRAM_getBPFreqHex;
         pCh->regBP3F.getVal = NULL;
         pCh->regRGFR.address = 0x48;
         pCh->regRGFR.regType = CMD1_COP;
         pCh->regRGFR.length = 1;
         strncpy(pCh->regRGFR.name, "RGFR", 15);
         RGFR.value = 0x0000;
         pCh->regRGFR.setVal = CRAM_getRingFreqHex;
         pCh->regRGFR.getVal = CRAM_getRingFreqVal;
         pCh->regLMAC.address = 0x41;
         pCh->regLMAC.regType = CMD1_COP;
         pCh->regLMAC.length = 1;
         strncpy(pCh->regLMAC.name, "LMAC", 15);
         LM_AC.value = 0x0000;
         pCh->regLMAC.setVal = NULL;
         pCh->regLMAC.getVal = NULL;
         pCh->regLMDC.address = 0x68;
         pCh->regLMDC.regType = CMD1_COP;
         pCh->regLMDC.length = 1;
         strncpy(pCh->regLMDC.name, "LMDC", 15);
         LMDC.value = 0x0000;
         pCh->regLMDC.setVal = NULL;
         pCh->regLMDC.getVal = NULL;
         pCh->regV1.address = 0x49;
         pCh->regV1.regType = CMD1_COP;
         pCh->regV1.length = 1;
         strncpy(pCh->regV1.name, "V1", 15);
         V1.value = 0x0000;
         pCh->regV1.setVal = CRAM_getRingAmpHex;
         pCh->regV1.getVal = CRAM_getRingAmpVal;
         pCh->regRGTP.address = 0x4A;
         pCh->regRGTP.regType = CMD1_COP;
         pCh->regRGTP.length = 1;
         strncpy(pCh->regRGTP.name, "RGTP", 15);
         RGTP.value = 0x0000;
         pCh->regRGTP.setVal = NULL;
         pCh->regRGTP.getVal = NULL;
         pCh->regCREST.address = 0x4B;
         pCh->regCREST.regType = CMD1_COP;
         pCh->regCREST.length = 1;
         strncpy(pCh->regCREST.name, "CREST", 15);
         CREST.value = 0x0000;
         pCh->regCREST.setVal = NULL;
         pCh->regCREST.getVal = NULL;
         pCh->regRO1.address = 0x4C;
         pCh->regRO1.regType = CMD1_COP;
         pCh->regRO1.length = 1;
         strncpy(pCh->regRO1.name, "RO1", 15);
         RO1.value = 0x0000;
         pCh->regRO1.setVal = CRAM_getRingDcHex;
         pCh->regRO1.getVal = CRAM_getRingDcVal;
         pCh->regRO2.address = 0x4D;
         pCh->regRO2.regType = CMD1_COP;
         pCh->regRO2.length = 1;
         strncpy(pCh->regRO2.name, "RO2", 15);
         RO2.value = 0x0000;
         pCh->regRO2.setVal = CRAM_getRingDcHex;
         pCh->regRO2.getVal = CRAM_getRingDcVal;
         pCh->regRO3.address = 0x4E;
         pCh->regRO3.regType = CMD1_COP;
         pCh->regRO3.length = 1;
         strncpy(pCh->regRO3.name, "RO3", 15);
         RO3.value = 0x0000;
         pCh->regRO3.setVal = CRAM_getRingDcHex;
         pCh->regRO3.getVal = CRAM_getRingDcVal;
         pCh->regRGD.address = 0x4F;
         pCh->regRGD.regType = CMD1_COP;
         pCh->regRGD.length = 1;
         strncpy(pCh->regRGD.name, "RGD", 15);
         RGD.value = 0x0000;
         pCh->regRGD.setVal = CRAM_getRingDelayHex;
         pCh->regRGD.getVal = CRAM_getRingDelayVal;
         pCh->regI2.address = 0x53;
         pCh->regI2.regType = CMD1_COP;
         pCh->regI2.length = 1;
         strncpy(pCh->regI2.name, "I2", 15);
         I2.value = 0x0000;
         pCh->regI2.setVal = NULL;
         pCh->regI2.getVal = NULL;
         pCh->regVLIM.address = 0x54;
         pCh->regVLIM.regType = CMD1_COP;
         pCh->regVLIM.length = 1;
         strncpy(pCh->regVLIM.name, "VLIM", 15);
         VLIM.value = 0x0000;
         pCh->regVLIM.setVal = CRAM_getVlimHex;
         pCh->regVLIM.getVal = CRAM_getVlimVal;
         pCh->regVK1.address = 0x55;
         pCh->regVK1.regType = CMD1_COP;
         pCh->regVK1.length = 1;
         strncpy(pCh->regVK1.name, "VK1", 15);
         VK1.value = 0x0000;
         pCh->regVK1.setVal = CRAM_getVk1Hex;
         pCh->regVK1.getVal = CRAM_getVk1Val;
         pCh->regIK1.address = 0x56;
         pCh->regIK1.regType = CMD1_COP;
         pCh->regIK1.length = 1;
         strncpy(pCh->regIK1.name, "IK1", 15);
         IK1.value = 0x0000;
         pCh->regIK1.setVal = CRAM_getIk1Hex;
         pCh->regIK1.getVal = CRAM_getIk1Val;
         pCh->regVRTL.address = 0x5F;
         pCh->regVRTL.regType = CMD1_COP;
         pCh->regVRTL.length = 1;
         strncpy(pCh->regVRTL.name, "VRTL", 15);
         VRTL.value = 0x0000;
         pCh->regVRTL.setVal = NULL;
         pCh->regVRTL.getVal = NULL;
         pCh->regHPD.address = 0x60;
         pCh->regHPD.regType = CMD1_COP;
         pCh->regHPD.length = 1;
         strncpy(pCh->regHPD.name, "HPD", 15);
         HPD.value = 0x0000;
         pCh->regHPD.setVal = NULL;
         pCh->regHPD.getVal = NULL;
         pCh->regHACT.address = 0x61;
         pCh->regHACT.regType = CMD1_COP;
         pCh->regHACT.length = 1;
         strncpy(pCh->regHACT.name, "HACT", 15);
         HACT.value = 0x0000;
         pCh->regHACT.setVal = NULL;
         pCh->regHACT.getVal = NULL;
         pCh->regHRING.address = 0x62;
         pCh->regHRING.regType = CMD1_COP;
         pCh->regHRING.length = 1;
         strncpy(pCh->regHRING.name, "HRING", 15);
         HRING.value = 0x0000;
         pCh->regHRING.setVal = NULL;
         pCh->regHRING.getVal = NULL;
         pCh->regHACRT.address = 0x63;
         pCh->regHACRT.regType = CMD1_COP;
         pCh->regHACRT.length = 1;
         strncpy(pCh->regHACRT.name, "HACRT", 15);
         HACRT.value = 0x0000;
         pCh->regHACRT.setVal = NULL;
         pCh->regHACRT.getVal = NULL;
         pCh->regHLS.address = 0x64;
         pCh->regHLS.regType = CMD1_COP;
         pCh->regHLS.length = 1;
         strncpy(pCh->regHLS.name, "HLS", 15);
         HLS.value = 0x0000;
         pCh->regHLS.setVal = NULL;
         pCh->regHLS.getVal = NULL;
         pCh->regHMW.address = 0x65;
         pCh->regHMW.regType = CMD1_COP;
         pCh->regHMW.length = 1;
         strncpy(pCh->regHMW.name, "HMW", 15);
         HMW.value = 0x0000;
         pCh->regHMW.setVal = NULL;
         pCh->regHMW.getVal = NULL;
         pCh->regHAHY.address = 0x66;
         pCh->regHAHY.regType = CMD1_COP;
         pCh->regHAHY.length = 1;
         strncpy(pCh->regHAHY.name, "HAHY", 15);
         HAHY.value = 0x0000;
         pCh->regHAHY.setVal = NULL;
         pCh->regHAHY.getVal = NULL;
         pCh->regFRTR.address = 0x67;
         pCh->regFRTR.regType = CMD1_COP;
         pCh->regFRTR.length = 1;
         strncpy(pCh->regFRTR.name, "FRTR", 15);
         FRTR.value = 0x0000;
         pCh->regFRTR.setVal = NULL;
         pCh->regFRTR.getVal = NULL;
         pCh->regCRAMP.address = 0x6A;
         pCh->regCRAMP.regType = CMD1_COP;
         pCh->regCRAMP.length = 1;
         strncpy(pCh->regCRAMP.name, "CRAMP", 15);
         CRAMP.value = 0x0000;
         pCh->regCRAMP.setVal = CRAM_getRampSlopeHex;
         pCh->regCRAMP.getVal = CRAM_getRampSlopeVal;

         break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
      case VINETIC_V2x:
         pCh->regLMRES2.address = 0x12;
         pCh->regLMRES2.regType = CMD1_SOP;
         pCh->regLMRES2.length = 1;
         strncpy(pCh->regLMRES2.name, "LMRES2", 15);
         LMRES2.value = 0x0000;
         pCh->regLMRES2.setVal = NULL;
         pCh->regLMRES2.getVal = NULL;
         pCh->regLMCR2.address = 0x13;
         pCh->regLMCR2.regType = CMD1_SOP;
         pCh->regLMCR2.length = 1;
         strncpy(pCh->regLMCR2.name, "LMCR2", 15);
         LMCR2.value = 0x0000;
         pCh->regLMCR2.setVal = NULL;
         pCh->regLMCR2.getVal = NULL;
         pCh->regTG1F.address = 0xC4;
         pCh->regTG1F.regType = CMD1_COP;
         pCh->regTG1F.length = 1;
         strncpy(pCh->regTG1F.name, "TG1F", 15);
         TG1F.value = 0x0000;
         pCh->regTG1F.setVal = CRAM_getTGFreqHex;
         pCh->regTG1F.getVal = CRAM_getTGFreqVal;
         pCh->regTG2F.address = 0xC5;
         pCh->regTG2F.regType = CMD1_COP;
         pCh->regTG2F.length = 1;
         strncpy(pCh->regTG2F.name, "TG2F", 15);
         TG2F.value = 0x0000;
         pCh->regTG2F.setVal = CRAM_getTGFreqHex;
         pCh->regTG2F.getVal = CRAM_getTGFreqVal;
         pCh->regTG1A.address = 0xC6;
         pCh->regTG1A.regType = CMD1_COP;
         pCh->regTG1A.length = 1;
         strncpy(pCh->regTG1A.name, "TG1A", 15);
         TG1A.value = 0x0000;
         pCh->regTG1A.setVal = CRAM_getTGAmpHex;
         pCh->regTG1A.getVal = CRAM_getTGAmpVal;
         pCh->regTG2A.address = 0xC7;
         pCh->regTG2A.regType = CMD1_COP;
         pCh->regTG2A.length = 1;
         strncpy(pCh->regTG2A.name, "TG2A", 15);
         TG2A.value = 0x0000;
         pCh->regTG2A.setVal = CRAM_getTGAmpHex;
         pCh->regTG2A.getVal = CRAM_getTGAmpVal;
         pCh->regRGFR.address = 0xB8;
         pCh->regRGFR.regType = CMD1_COP;
         pCh->regRGFR.length = 1;
         strncpy(pCh->regRGFR.name, "RGFR", 15);
         RGFR.value = 0x0000;
         pCh->regRGFR.setVal = CRAM_getRingFreqHex;
         pCh->regRGFR.getVal = CRAM_getRingFreqVal;
         pCh->regV1.address = 0xB4;
         pCh->regV1.regType = CMD1_COP;
         pCh->regV1.length = 1;
         strncpy(pCh->regV1.name, "V1", 15);
         V1.value = 0x0000;
         pCh->regV1.setVal = CRAM_getRingAmpHex;
         pCh->regV1.getVal = NULL;
         pCh->regCREST.address = 0xB0;
         pCh->regCREST.regType = CMD1_COP;
         pCh->regCREST.length = 1;
         strncpy(pCh->regCREST.name, "CREST", 15);
         CREST.value = 0x0000;
         pCh->regCREST.setVal = NULL;
         pCh->regCREST.getVal = NULL;
         pCh->regRO1.address = 0xB5;
         pCh->regRO1.regType = CMD1_COP;
         pCh->regRO1.length = 1;
         strncpy(pCh->regRO1.name, "RO1", 15);
         RO1.value = 0x0000;
         pCh->regRO1.setVal = CRAM_getRingDcHex;
         pCh->regRO1.getVal = CRAM_getRingDcVal;
         pCh->regRO2.address = 0xB6;
         pCh->regRO2.regType = CMD1_COP;
         pCh->regRO2.length = 1;
         strncpy(pCh->regRO2.name, "RO2", 15);
         RO2.value = 0x0000;
         pCh->regRO2.setVal = CRAM_getRingDcHex;
         pCh->regRO2.getVal = CRAM_getRingDcVal;
         pCh->regRO3.address = 0xB7;
         pCh->regRO3.regType = CMD1_COP;
         pCh->regRO3.length = 1;
         strncpy(pCh->regRO3.name, "RO3", 15);
         RO3.value = 0x0000;
         pCh->regRO3.setVal = CRAM_getRingDcHex;
         pCh->regRO3.getVal = CRAM_getRingDcVal;
         pCh->regRGD.address = 0xB9;
         pCh->regRGD.regType = CMD1_COP;
         pCh->regRGD.length = 1;
         strncpy(pCh->regRGD.name, "RGD", 15);
         RGD.value = 0x0000;
         pCh->regRGD.setVal = CRAM_getRingDelayHex;
         pCh->regRGD.getVal = CRAM_getRingDelayVal;
         pCh->regVLIM.address = 0xAD;
         pCh->regVLIM.regType = CMD1_COP;
         pCh->regVLIM.length = 1;
         strncpy(pCh->regVLIM.name, "VLIM", 15);
         VLIM.value = 0x0000;
         pCh->regVLIM.setVal = CRAM_getVlimHex;
         pCh->regVLIM.getVal = CRAM_getVlimVal;
         pCh->regVK1.address = 0xAC;
         pCh->regVK1.regType = CMD1_COP;
         pCh->regVK1.length = 1;
         strncpy(pCh->regVK1.name, "VK1", 15);
         VK1.value = 0x0000;
         pCh->regVK1.setVal = CRAM_getVk1Hex;
         pCh->regVK1.getVal = CRAM_getVk1Val;
         pCh->regIK1.address = 0xA4;
         pCh->regIK1.regType = CMD1_COP;
         pCh->regIK1.length = 1;
         strncpy(pCh->regIK1.name, "IK1", 15);
         IK1.value = 0x0000;
         pCh->regIK1.setVal = CRAM_getIk1Hex;
         pCh->regIK1.getVal = CRAM_getIk1Val;
         pCh->regVRTL.address = 0xAE;
         pCh->regVRTL.regType = CMD1_COP;
         pCh->regVRTL.length = 1;
         strncpy(pCh->regVRTL.name, "VRTL", 15);
         VRTL.value = 0x0000;
         pCh->regVRTL.setVal = NULL;
         pCh->regVRTL.getVal = NULL;
         pCh->regCRAMP.address = 0xCA;
         pCh->regCRAMP.regType = CMD1_COP;
         pCh->regCRAMP.length = 1;
         strncpy(pCh->regCRAMP.name, "CRAMP", 15);
         CRAMP.value = 0x0000;
         pCh->regCRAMP.setVal = CRAM_getRampSlopeHex;
         pCh->regCRAMP.getVal = CRAM_getRampSlopeVal;
         pCh->regLM_ITIME.address = 0xCB;
         pCh->regLM_ITIME.regType = CMD1_COP;
         pCh->regLM_ITIME.length = 1;
         strncpy(pCh->regLM_ITIME.name, "LM_ITIME", 15);
         LM_ITIME.value = 0x0000;
         pCh->regLM_ITIME.setVal = NULL;
         pCh->regLM_ITIME.getVal = NULL;
         pCh->regLM_DELAY.address = 0xCC;
         pCh->regLM_DELAY.regType = CMD1_COP;
         pCh->regLM_DELAY.length = 1;
         strncpy(pCh->regLM_DELAY.name, "LM_DELAY", 15);
         LM_DELAY.value = 0x0000;
         pCh->regLM_DELAY.setVal = NULL;
         pCh->regLM_DELAY.getVal = NULL;
         pCh->regLM_SHIFT.address = 0xCD;
         pCh->regLM_SHIFT.regType = CMD1_COP;
         pCh->regLM_SHIFT.length = 1;
         strncpy(pCh->regLM_SHIFT.name, "LM_SHIFT", 15);
         LM_SHIFT.value = 0x0000;
         pCh->regLM_SHIFT.setVal = NULL;
         pCh->regLM_SHIFT.getVal = NULL;
         pCh->regLM_THRES.address = 0xCE;
         pCh->regLM_THRES.regType = CMD1_COP;
         pCh->regLM_THRES.length = 1;
         strncpy(pCh->regLM_THRES.name, "LM_THRES", 15);
         LM_THRES.value = 0x0000;
         pCh->regLM_THRES.setVal = NULL;
         pCh->regLM_THRES.getVal = NULL;
         pCh->regLM_RCLOWTHRES.address = 0xCF;
         pCh->regLM_RCLOWTHRES.regType = CMD1_COP;
         pCh->regLM_RCLOWTHRES.length = 1;
         strncpy(pCh->regLM_RCLOWTHRES.name, "LM_RCLOWTHRES", 15);
         LM_RCLOWTHRES.value = 0x0000;
         pCh->regLM_RCLOWTHRES.setVal = NULL;
         pCh->regLM_RCLOWTHRES.getVal = NULL;
         pCh->regLM_FILT1.address = 0xD0;
         pCh->regLM_FILT1.regType = CMD1_COP;
         pCh->regLM_FILT1.length = 1;
         strncpy(pCh->regLM_FILT1.name, "LM_FILT1", 15);
         LM_FILT1.value = 0x0000;
         pCh->regLM_FILT1.setVal = NULL;
         pCh->regLM_FILT1.getVal = NULL;
         pCh->regLM_FILT2.address = 0xD1;
         pCh->regLM_FILT2.regType = CMD1_COP;
         pCh->regLM_FILT2.length = 1;
         strncpy(pCh->regLM_FILT2.name, "LM_FILT2", 15);
         LM_FILT2.value = 0x0000;
         pCh->regLM_FILT2.setVal = NULL;
         pCh->regLM_FILT2.getVal = NULL;
         pCh->regLM_FILT3.address = 0xD2;
         pCh->regLM_FILT3.regType = CMD1_COP;
         pCh->regLM_FILT3.length = 1;
         strncpy(pCh->regLM_FILT3.name, "LM_FILT3", 15);
         LM_FILT3.value = 0x0000;
         pCh->regLM_FILT3.setVal = NULL;
         pCh->regLM_FILT3.getVal = NULL;
         pCh->regLM_FILT4.address = 0xD3;
         pCh->regLM_FILT4.regType = CMD1_COP;
         pCh->regLM_FILT4.length = 1;
         strncpy(pCh->regLM_FILT4.name, "LM_FILT4", 15);
         LM_FILT4.value = 0x0000;
         pCh->regLM_FILT4.setVal = NULL;
         pCh->regLM_FILT4.getVal = NULL;
         pCh->regLM_FILT5.address = 0xD4;
         pCh->regLM_FILT5.regType = CMD1_COP;
         pCh->regLM_FILT5.length = 1;
         strncpy(pCh->regLM_FILT5.name, "LM_FILT5", 15);
         LM_FILT5.value = 0x0000;
         pCh->regLM_FILT5.setVal = NULL;
         pCh->regLM_FILT5.getVal = NULL;
         break;
#endif /* VIN_V21_SUPPORT */
      default:
         SET_ERROR(ERR_UNKNOWN_VERSION);
         err = IFX_ERROR;
         break;
   }
   VINETIC_GpioInit((IFX_int32_t)pCh);
   return err;
}

/**
   Initalize the shadow channel registers
   This function is called on vinetic_open because it does not contain
   chip version dependant initialisation.
\param
   pDev   - handle to the device structure
\return
   IFX_SUCCESS
\remarks
   none
*/
IFX_LOCAL IFX_int32_t InitDevRegStruct (VINETIC_DEVICE *pDev)
{
   strncpy(pDev->regGCR1.name, "GCR1", 15);
   pDev->regGCR1.address   = 0x47;
   pDev->regGCR1.length    = 1;
   pDev->regGCR1.regType   = CMD1_IOP;
   pDev->regGCR1.setVal    = NULL;
   pDev->regGCR1.getVal    = NULL;
   pDev->regGCR1.value     = 0x0000;
   strncpy(pDev->regGCR2.name, "GCR2", 15);
   pDev->regGCR2.address   = 0x48;
   pDev->regGCR2.length    = 1;
   pDev->regGCR2.regType   = CMD1_IOP;
   pDev->regGCR2.setVal    = NULL;
   pDev->regGCR2.getVal    = NULL;
   pDev->regGCR2.value     = 0x0000;
   strncpy(pDev->regMRSRGPIO.name, "MRSRGPIO", 15);
   pDev->regMRSRGPIO.address   = 0x1A;
   pDev->regMRSRGPIO.length    = 1;
   pDev->regMRSRGPIO.regType   = CMD1_IOP;
   pDev->regMRSRGPIO.setVal    = NULL;
   pDev->regMRSRGPIO.getVal    = NULL;
   pDev->regMRSRGPIO.value     = 0xFFFF;
   strncpy(pDev->regMFSRGPIO.name, "MFSRGPIO", 15);
   pDev->regMFSRGPIO.address   = 0x7A;
   pDev->regMFSRGPIO.length    = 1;
   pDev->regMFSRGPIO.regType   = CMD1_IOP;
   pDev->regMFSRGPIO.setVal    = NULL;
   pDev->regMFSRGPIO.getVal    = NULL;
   pDev->regMFSRGPIO.value     = 0xFFFF;

   VINETIC_GpioInit((IFX_int32_t)pDev);
   return IFX_SUCCESS;
}

/**
   Initalize the shadow channel registers
   This function is called on vinetic_open because it does not contain
   chip version dependant initialisation.
\param
   pDev   - handle to the device structure
\return
   IFX_SUCCESS or IFX_ERROR, if no memory is available
*/
static int initDevData(VINETIC_DEVICE *pDev)
{
   IFX_uint8_t i;

   /* set default coder resource to 4 */
   pDev->nMaxRes = 4;
   pDev->nCoderCnt = 4;
   pDev->nSigCnt = 4;
   pDev->nPcmCnt = 8;
   pDev->nToneGenCnt = 2;
#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
   if (pDev->nEdspVers != NOTVALID)
   {
      /* set chip type and features if firmware EDSP version was read */
      pDev->nChipType = VINETIC_TYPE_VIP;
      switch (pDev->nEdspVers & ECMD_VERS_MV)
      {
      case ECMD_VERS_MV_4:
         switch (pDev->nEdspVers & ECMD_VERS_FEATURES)
         {
         case ECMD_VERS_FEATURES_S:
            pDev->nChipType = VINETIC_TYPE_S;
            break;
         case ECMD_VERS_FEATURES_C:
            pDev->nChipType = VINETIC_TYPE_C;
            break;
         default:
            /* M or VIP */
            break;
         }
         break;
      case ECMD_VERS_MV_8:
         pDev->nMaxRes = 8;
         pDev->nCoderCnt = 8;
      break;
      case 0x6000:
         pDev->nMaxRes = 6;
         pDev->nCoderCnt = 6;
         pDev->nSigCnt = 6;
      break;
      case ECMD_VERS_MV_4M:
         pDev->nChipType = VINETIC_TYPE_M;
         break;
      default:
         /* this version we don't know. So use defaults: VIP and 4 coders */
         break;
      }
   }
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */

   /* set features for chips not requiring FW download */
   switch (pDev->nChipType)
   {
   case VINETIC_TYPE_S:
      pDev->nMaxRes = 0;
      pDev->nCoderCnt = 0;
      pDev->nSigCnt = 0;
      pDev->nToneGenCnt = 1;
      break;
   case VINETIC_TYPE_C:
      pDev->nMaxRes = 0;
      pDev->nCoderCnt = 0;
      break;
   default:
      /* M or VIP */
      break;
   }

   /* set the available LEC resource in the firmware as free .
      One bit represent one available LEC resource */
   pDev->availLecRes = (1 << pDev->nMaxRes) - 1;

   if (pDev->pAlmCh!= NULL)
      IFXOS_FREE(pDev->pAlmCh);
   if (pDev->pCodCh != NULL)
      IFXOS_FREE(pDev->pCodCh);
   if (pDev->pPcmCh != NULL)
      IFXOS_FREE(pDev->pPcmCh);
   if (pDev->pSigCh != NULL)
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
      /* allocate memory for packet data when we got coder channel */
      InitPacketCh (&pDev->pChannel[i]);
#endif /* (VIN_CFG_FEATURES& VIN_FEAT_PACKET) */
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
   Initalize the coder module and the cached firmware messages
\param
   pSig   - handle to the coder channel structure
   ch     - channel or resource number
*/
IFX_LOCAL IFX_void_t InitCodCh (VINETIC_CODCH* pCod, IFX_uint8_t ch)
{
   int i;

   pCod->signal.nModType = VINDSP_MT_COD;
   pCod->signal.nSignal  = ECMD_IX_COD_OUT0 + ch;
#ifdef DEBUG
   pCod->signal.name = signalName[pCod->signal.nSignal];
#endif /* DEBUG */
   for (i = 0; i < MAX_MODULE_SIGNAL_INPUTS; ++i)
   {
      pCod->signal.in[i].pParent = &pCod->signal;
   }
   /* COD CH */
   pCod->cod_ch.value[0] = CMD1_EOP | ch;
   pCod->cod_ch.value[1] = ECMD_COD_CH;
}

/**
   Initalize the PCM module and the cached firmware messages
\param
   pSig   - handle to the PCM channel structure
   ch     - channel or resource number
   pcmCh  - the PCM channel resource to use
*/
IFX_LOCAL IFX_void_t InitPcmCh (VINETIC_PCMCH* pPcm, IFX_uint8_t ch, IFX_uint8_t pcmCh)
{
   int i;

   pPcm->signal.nModType = VINDSP_MT_PCM;
   pPcm->signal.nSignal  = ECMD_IX_PCM_OUT0 + pcmCh;
#ifdef DEBUG
   pPcm->signal.name = signalName[pPcm->signal.nSignal];
#endif /* DEBUG */
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
   for (i = 0; i < MAX_MODULE_SIGNAL_INPUTS; ++i)
      pPcm->signal.in[i].pParent = &pPcm->signal;
}

/**
   Initalize the analog module and the cached firmware messages
\param
   pSig   - handle to the analog channel structure
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
   pAlm->ali_nelec = ALM_OLDC_EN;
   /* ALM module connections */
   pAlm->signal.nModType = VINDSP_MT_ALM;
   pAlm->signal.nSignal  = ECMD_IX_ALM_OUT0 + ch;
   for (i = 0; i < MAX_MODULE_SIGNAL_INPUTS; ++i)
      pAlm->signal.in[i].pParent = &pAlm->signal;
#ifdef DEBUG
   pAlm->signal.name = signalName[pAlm->signal.nSignal];
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
   pSig->signal.name = signalName[pSig->signal.nSignal];
#endif /* DEBUG */
   for (i = 0; i < MAX_MODULE_SIGNAL_INPUTS; ++i)
   {
      pSig->signal.in[i].pParent = &pSig->signal;
   }
   /* SIG CH */
   pSig->sig_ch.value[0]            = CMD1_EOP | ch;
   pSig->sig_ch.value[1]            = ECMD_SIG_CH;
   pSig->utg.value[0]               = CMD1_EOP | ch;
   pSig->utg.value[1]               = ECMD_UTG;
   pSig->utg.bit.utgnr = ch;
   /* tone detectors */
   pSig->sig_atd1.value[0]   =  CMD1_EOP | ch;
   pSig->sig_atd1.value[1]   = ECMD_SIG_ATD1;
   pSig->sig_atd2.value[0]   = CMD1_EOP | ch;
   pSig->sig_atd2.value[1]   = ECMD_SIG_ATD2;
   pSig->sig_utd1.value[0]   = CMD1_EOP | ch;
   pSig->sig_utd1.value[1]   = ECMD_SIG_UTD1;
   pSig->sig_utd2.value[0]   = CMD1_EOP | ch;
   pSig->sig_utd2.value[1]   = ECMD_SIG_UTD2;
   /* DTMF generator */
   pSig->sig_dtmfgen.value[0] = CMD1_EOP | ch;
   pSig->sig_dtmfgen.value[1] = ECMD_DTMF_GEN | CMD_SIG_DTMFGEN_LEN;
   /* CID Send */
   pSig->cid_sender [0]              = CMD1_EOP | ch;
   pSig->cid_sender [1]              = ECMD_CID_SEND;
}

#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
/**
   Initalize the coder packet buffer
\param
   ch     - channel or resource number
*/
IFX_LOCAL IFX_return_t InitPacketCh (VINETIC_CHANNEL* pCh)
{
   IFX_return_t err = IFX_SUCCESS;
   PACKET **pPacket = &pCh->pFifo;
   VINETIC_DEVICE* pDev = pCh->pParent;

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
      if (err == -1)
      {
         /* ERROR: packet FIFO init failed, check fifo_init description */
         SET_ERROR(ERR_FUNC_PARM);
         err = IFX_ERROR;
      }
   }
   return err;
}
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
