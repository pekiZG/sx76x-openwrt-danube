/****************************************************************************
                  Copyright (c) 2005  Infineon Technologies AG
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
   Module      : drv_tapi_misc.c
   Desription  : Contains TAPI Miscellaneous Services.
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */

#include "drv_tapi.h"

/* ============================= */
/* Local variable definition     */
/* ============================= */

IFX_LOCAL IFX_char_t TAPI_VersionString[] =
   "Product ID:  1000.1.1.1, Version ID: 1.2.0.1";
IFX_LOCAL IFX_int32_t GetChStatus (TAPI_CONNECTION * pChannel,
                                   TAPI_PHONE_STATUS * status);
IFX_LOCAL IFX_int32_t TAPI_PhoneCh_Init (TAPI_CONNECTION * pChannel);
IFX_LOCAL COMPLEX_TONE toneCoefficients[TAPI_MAX_TONE_CODE];

/* ============================= */
/* Global function definition    */
/* ============================= */

/*******************************************************************************
Description:
   Init the TAPI. This function is channel specific but the init is for the
   complete TAPI.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   pInit       - handle to TAPI_INIT structure
Return:
   error code: IFX_SUCCESS  -> init successful
               IFX_ERROR    -> init not successful
*******************************************************************************/
IFX_int32_t TAPI_Phone_Init (TAPI_CONNECTION * pChannel, TAPI_INIT const *pInit)
{
   TAPI_DEV        *pTapiDev = pChannel->pTapiDevice;
   TAPI_INIT       Init;
   IFX_int32_t     err = IFX_SUCCESS, i;

   /* number of tapi channel supported must be checked when first channel is
      opened. Check this. */
   if (pTapiDev->nMaxChannel == 0)
   {
      LOG (TAPI_DRV, DBG_LEVEL_HIGH,
          ("\n\rDRV_ERROR: TAPI maximum channels supported not set\n\r"));
      return IFX_ERROR;
   }
   if (pChannel->bInstalled == IFX_FALSE)
   {
      /* set the tone table pointer */
      pTapiDev->pToneTbl = &toneCoefficients[0];
   }
   /* we here initialize all channel member, if not already done. They could
      already be used for events, even if no channel specific initialization
      was done */
   for (i = 0; i < pTapiDev->nMaxChannel; i++)
   {
      if (pTapiDev->pChannelCon[i].bInstalled == IFX_FALSE)
      {
         IFXOS_MutexInit (pTapiDev->pChannelCon[i].TapiDataLock);
         IFXOS_InitEvent (pTapiDev->pChannelCon[i].TapiRingEvent);
         err = TAPI_PhoneCh_Init (&pTapiDev->pChannelCon[i]);
         if (err != IFX_SUCCESS)
            break;
      }
   }
   /* do low level initialization now */
   if (err == IFX_SUCCESS)
   {
      /* check if Init ptr is valid and set it if not. */
      if (pInit == NULL)
      {
         Init.nCountry = TAPI_COUNTRY_DEFAULT;
         Init.nMode    = TAPI_DEFAULT;
         Init.pProc    = NULL;
         pInit = &Init;
      }
      err = TAPI_LL_Phone_Init (pChannel, pInit);
   }
   return (err);
}

/*******************************************************************************
Description:
   Init one TAPI channel.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
Return:
   error code: IFX_SUCCESS  -> init successful
               IFX_ERROR      -> init not successful
*******************************************************************************/
IFX_LOCAL IFX_int32_t TAPI_PhoneCh_Init (TAPI_CONNECTION * pChannel)
{
   IFX_int32_t err = IFX_SUCCESS, i;
   TAPI_EXCEPTION ex;

#ifdef TAPI_CID
   /* set default data channel for ringing with CID */
   pChannel->TapiRingData.pDataCh = pChannel;
#endif /* TAPI_CID */
   /* stop ringing if already running */
   TAPI_Phone_Ring_Stop (pChannel);
   /* set default ringing cadence for all channels */
   TAPI_Phone_Ring_Cadence (pChannel, 0xF0F0F0F0);
   /* clear exception mask -> enable exceptions */
   ex.Status = 0;
   ex.Bits.feedLowBatt = 1;
   TAPI_Phone_Mask_Exception (pChannel, ex.Status);
#ifdef TAPI_CID
   /* default configuration for CID */
   pChannel->TapiCidConfig.nMode       = TAPI_CID_FSK_MODE;
   pChannel->TapiCidConfig.nAppearance = CID_BETWEEN_RINGBURST;
   pChannel->TapiCidConfig.nDelay      = TAPI_MIN_TIME_BEFORE_CID;
   pChannel->TapiCidConfig.nBurstCount = 1;
   /* default configuration for DTMF CID */
   pChannel->TapiDtmfCidConf.startTone      = TAPI_DTMF_DEFAULT_STARTTONE;
   pChannel->TapiDtmfCidConf.stopTone       = TAPI_DTMF_DEFAULT_STOPTONE;
   pChannel->TapiDtmfCidConf.infoStartTone  = TAPI_DTMF_DEFAULT_INFOSTARTTONE;
   pChannel->TapiDtmfCidConf.redirStartTone = TAPI_DTMF_DEFAULT_REDIRSTARTTONE;
   pChannel->TapiDtmfCidConf.digitTime      = TAPI_DTMF_DEFAULT_DIGITTIME;
   pChannel->TapiDtmfCidConf.interDigitTime = TAPI_DTMF_DEFAULT_INTERDIGITTIME;
   /* Default configuration for CID 2 */
   pChannel->TapiCid2.pPhoneCh                      = pChannel;
   pChannel->TapiCid2.conf.stdSelect                = TAPI_CID2_TIA;
   pChannel->TapiCid2.conf.std.defaultS.casDuration =
                                              TAPI_CID2_CASDURATION_DEFAULT;
   pChannel->TapiCid2.conf.std.defaultS.ackTone     =
                TAPI_LL_Phone_Dtmf_GetCode (TAPI_CID2_ACKTONE_DEFAULT);
   pChannel->TapiCid2.conf.std.defaultS.cas2ackTime =
                                              TAPI_CID2_CAS2ACK_DEFAULTTIME;
   pChannel->TapiCid2.conf.std.defaultS.ack2fskOutTime =
                                              TAPI_CID2_ACK2FSKOUT_DEFAULTTIME;
   IFXOS_InitEvent (pChannel->TapiCid2.asEvt);
#endif /* TAPI_CID */
   if (pChannel->TapiDialData.DialTimerID == 0)
   {
      /* initialize (create) dial timer */
      pChannel->TapiDialData.DialTimerID =
         TAPI_Create_Timer ((TIMER_ENTRY) TAPI_Phone_Dial_OnTimer,
                            (IFX_int32_t) pChannel);
      if (pChannel->TapiDialData.DialTimerID == 0)
      {
         err = IFX_ERROR;
      }
   }
   if (pChannel->TapiMiscData.GndkhTimerID == 0)
   {
      /* initialize (create) dial timer */
      pChannel->TapiMiscData.GndkhTimerID =
         TAPI_Create_Timer ((TIMER_ENTRY) TAPI_Gndkh_OnTimer,
                            (IFX_int32_t) pChannel);
      if (pChannel->TapiMiscData.GndkhTimerID == 0)
      {
         err = IFX_ERROR;
      }
   }
   if (pChannel->TapiRingData.RingTimerID == 0)
   {
      /* initialize (create) ring timer */
      pChannel->TapiRingData.RingTimerID =
         TAPI_Create_Timer ((TIMER_ENTRY) TAPI_Phone_Ring_OnTimer,
                            (IFX_int32_t) pChannel);
      if (pChannel->TapiRingData.RingTimerID == 0)
      {
         err = IFX_ERROR;
      }
   }
   if (pChannel->TapiMeterData.MeterTimerID == 0)
   {
      /* initialize (create) metering timer */
      pChannel->TapiMeterData.MeterTimerID =
         TAPI_Create_Timer ((TIMER_ENTRY) TAPI_Phone_Meter_OnTimer,
                            (IFX_int32_t) pChannel);
      if (pChannel->TapiMeterData.MeterTimerID == 0)
      {
         err = IFX_ERROR;
      }
   }
   /* Set defaults for validation timers */
   pChannel->TapiDigitLowTime.nMinTime   = TAPI_MIN_DIGIT_LOW;
   pChannel->TapiDigitLowTime.nMaxTime   = TAPI_MAX_DIGIT_LOW;
   pChannel->TapiDigitHighTime.nMinTime  = TAPI_MIN_DIGIT_HIGH;
   pChannel->TapiDigitHighTime.nMaxTime  = TAPI_MAX_DIGIT_HIGH;
   pChannel->TapiHookFlashTime.nMinTime  = TAPI_MIN_FLASH;
   pChannel->TapiHookFlashTime.nMaxTime  = TAPI_MAX_FLASH;
   pChannel->TapiHookFlashMakeTime.nMinTime  = TAPI_MIN_FLASH_MAKE;
   pChannel->TapiInterDigitTime.nMinTime = TAPI_MIN_INTERDIGIT;
   pChannel->TapiHookOffTime.nMinTime    = TAPI_MIN_OFF_HOOK;
   pChannel->TapiHookOnTime.nMinTime     = TAPI_MIN_ON_HOOK;

   /* Start hookstate FSM in onhook state */
   pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_ONHOOK;

   for (i = 0; i < TAPI_TONE_MAXRES; ++i)
   {
   /* Start complex tone generation in initialized state */
      pChannel->TapiComplexToneData[i].nToneState = TAPI_CT_IDLE;
   }
   pChannel->bInstalled = IFX_TRUE;
   return err;
}

/*******************************************************************************
Description:
   Returns the version string.
Arguments:
   pointer to buffer for version string
Return:
   length of version string
*******************************************************************************/
IFX_int32_t TAPI_Phone_Get_Version (IFX_char_t * version_string)
{
   IFX_int32_t len = (IFX_int32_t) strlen (TAPI_VersionString);
   strncpy (version_string, TAPI_VersionString, 80);
   return len;
}

/*******************************************************************************
Description:
   Returns the current exception status.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
Return:
   nException
*******************************************************************************/
IFX_uint32_t TAPI_Phone_Exception (TAPI_CONNECTION * pChannel)
{
   IFX_uint32_t   nException = 0;
   TAPI_EXCEPTION *ex;

   ex = &pChannel->TapiMiscData.nException;
   /* set fault bit */
   if (ex->Bits.ground_key || ex->Bits.ground_key_polarity ||
       ex->Bits.ground_key_high || ex->Bits.otemp)
   {
      ex->Bits.fault = IFX_TRUE;
   }
   /* get exception */
   nException = (~pChannel->TapiMiscData.nExceptionMask.Status) &
                 pChannel->TapiMiscData.nException.Status;
   /* delete hook bit */
   ex->Bits.hookstate = IFX_FALSE;
   /* delete cid rx status update bit */
   ex->Bits.cidrx_supdate = IFX_FALSE;
   /* delete flash hook bit */
   ex->Bits.flash_hook = IFX_FALSE;
   /* delete ground key bit */
   ex->Bits.ground_key = IFX_FALSE;
   /* delete ground key polarity bit */
   ex->Bits.ground_key_polarity = IFX_FALSE;
   /* delete ground key high bit */
   ex->Bits.ground_key_high = IFX_FALSE;
   ex->Bits.otemp = IFX_FALSE;
   /* delete fault bit */
   ex->Bits.fault = IFX_FALSE;
   /* clear FAX exceptions */
   ex->Bits.fax_ced     = IFX_FALSE;
   ex->Bits.fax_cng     = IFX_FALSE;
   ex->Bits.fax_supdate = IFX_FALSE;
   ex->Bits.fax_dis     = IFX_FALSE;
   ex->Bits.fax_ced_net = IFX_FALSE;
   ex->Bits.fax_cng_net = IFX_FALSE;
   /* clear gr909 exception bit */
   ex->Bits.gr909result = IFX_FALSE;
   ex->Bits.eventDetect = IFX_FALSE;
   /* clear runtime error bit */
   ex->Bits.runtime_error = IFX_FALSE;

   return nException;
}

/*******************************************************************************
Description:
   Masks the reporting of the exceptions.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   nException  - exception mask
Return:
   IFX_SUCCESS
*******************************************************************************/
IFX_int32_t TAPI_Phone_Mask_Exception (TAPI_CONNECTION * pChannel,
                                       IFX_uint32_t nException)
{
   pChannel->TapiMiscData.nExceptionMask.Status = nException;

   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   Reports the status information for one or more channels
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   status  - pointer to a list of TAPI_PHONE_STATUS structures, maybe only one
Return:
   IFX_SUCCESS
*******************************************************************************/
IFX_int32_t TAPI_Phone_GetStatus (TAPI_CONNECTION * pChannel,
                                  TAPI_PHONE_STATUS * status)
{
   IFX_uint8_t channels = status->channels;

   if (channels == 0)
   {
      /* return status for this channel */
      memset (status, 0, sizeof (TAPI_PHONE_STATUS));
      GetChStatus (pChannel, status);
   }
   else
   {
      /* more channels */
      TAPI_DEV *pTapiDev = pChannel->pTapiDevice;
      IFX_int32_t i;

      memset (status, 0, sizeof (TAPI_PHONE_STATUS) * channels);
      /* reduce the amount of copied data to the maximum supported channels */
      if (channels > pTapiDev->nMaxChannel)
         channels = pTapiDev->nMaxChannel;

      for (i = 0; i < channels; i++)
      {
         GetChStatus ((pTapiDev->pChannelCon + i), status++);
      }
   }
   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   Set status information for one channel
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   status  - pointer to a TAPI_PHONE_STATUS structure
Return:
   IFX_SUCCESS
*******************************************************************************/
IFX_LOCAL IFX_int32_t GetChStatus (TAPI_CONNECTION * pChannel,
                                   TAPI_PHONE_STATUS * status)
{
   if (pChannel->TapiMiscData.nException.Status)
   {
      IFX_uint8_t    *pFifo;
      TAPI_EXCEPTION ex;
      /* only do the event handling if there are event bits set ! */

      /* get the masked exception */
      ex.Status = pChannel->TapiMiscData.nException.Status &
                  ~pChannel->TapiMiscData.nExceptionMask.Status;

      /* clear exceptions */
      pChannel->TapiMiscData.nException.Status &=
         pChannel->TapiMiscData.nExceptionMask.Status;

      /* hook events */
      if (ex.Bits.hookstate)
      {
         if (pChannel->TapiOpControlData.bHookState)
            status->hook = TAPI_HOOKSTATUS_OFFHOOK | TAPI_HOOKSTATUS_HOOK;
         else
            status->hook = TAPI_HOOKSTATUS_HOOK;
      }
      if (ex.Bits.flash_hook)
         status->hook |= TAPI_HOOKSTATUS_FLASH;
      /* dial events. Do not read out fifo if masked */
      if (ex.Bits.dtmf_ready)
      {
         /* read DTMF digit from fifo */
            pFifo = Fifo_readElement (&(pChannel->TapiDialData.TapiDTMFFifo));
         if (pFifo != NULL)
         {
            status->digit = *pFifo;
            status->dialing = TAPI_DIALINGSTATUS_DTMF;
         }
      }
      /* Do not read out fifo if masked */
      if (ex.Bits.pulse_digit_ready)
      {
         status->dialing = TAPI_DIALINGSTATUS_PULSE;
         /* read pulse dial digit from fifo */
            pFifo = Fifo_readElement (&(pChannel->TapiDialData.TapiPulseFifo));
         if (pFifo != NULL)
         {
            status->digit = *pFifo;
         }
      }
      if (ex.Bits.ground_key)
         status->line |= TAPI_LINESTATUS_GNDKEY;
      if (ex.Bits.ground_key_high)
         status->line |= TAPI_LINESTATUS_GNDKEYHIGH;
      if (ex.Bits.ground_key_polarity)
         status->line |= TAPI_LINESTATUS_GNDKEYPOL;
      if (ex.Bits.otemp)
         status->line |= TAPI_LINESTATUS_OTEMP;
      if (ex.Bits.gr909result)
         status->line |= TAPI_LINESTATUS_GR909RES;
      if (ex.Bits.cidrx_supdate)
         status->line |= TAPI_LINESTATUS_CIDRX;

      /* fax status */
      if (ex.Bits.fax_ced || ex.Bits.fax_cng || ex.Bits.fax_dis ||
          ex.Bits.fax_supdate || ex.Bits.fax_ced_net || ex.Bits.fax_cng_net)
      status->line |= TAPI_LINESTATUS_FAX;

      if (ex.Bits.eventDetect)
         status->event = pChannel->TapiMiscData.event;
      else
         /* no event detected, set the value to "no event". Please note that
            'TAPI_RFC2833_EVENT_NO_EVENT' is unequal zero */
         status->event = (unsigned long)TAPI_RFC2833_EVENT_NO_EVENT;

      if (ex.Bits.ring_finished)
         status->line |= TAPI_LINESTATUS_RINGFINISHED;

      if (ex.Bits.feedLowBatt)
         status->line |= TAPI_LINESTATUS_FEEDLOWBATT;
   }
   else
      /* no event detected, set the value to "no event". Please note that
         'TAPI_RFC2833_EVENT_NO_EVENT' is unequal zero */
      status->event = (unsigned long)TAPI_RFC2833_EVENT_NO_EVENT;

   /* set signal detection status */
   status->signal = pChannel->TapiMiscData.signal;
   pChannel->TapiMiscData.signal = 0;

   /* set device status */
   status->device = pChannel->TapiMiscData.device;
   pChannel->TapiMiscData.device = IFX_FALSE;
   /* set runtime errors */
   status->error = pChannel->TapiMiscData.error;
   pChannel->TapiMiscData.error = 0;

   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   Returns error if the requested version is not supported
Arguments:
   pChannel - handle to TAPI_CONNECTION structure
   vers     - pointer to version structure
Return:
   IFX_SUCCESS if version is supported or IFX_ERROR
Remarks:
   Since an application is always build against one specific TAPI interface
   version it should check if it is supported. If not the application should
   abort. This interface checks if the current TAPI version supports a
   particular version. For example the TAPI versions 2.1 will support TAPI 2.0.
   But version 3.0 might not support 2.0.
*******************************************************************************/
IFX_int32_t TAPI_Phone_Check_Version (TAPI_PHONE_VERSION const *vers)
{
   switch (vers->major)
   {
   case  TAPI_MAJOR_VERS:
      switch (vers->minor)
      {
      case TAPI_MINOR_VERS:
         return IFX_SUCCESS;
         /* break; */
      default:
         break;
      }
      break;
   default:
      break;
   }
   return IFX_ERROR;
}

