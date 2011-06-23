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

 ****************************************************************************

   Module      : drv_tapi_ring.c

   Contains TAPI Ringing Services.

   All operations done by functions in this module are phone
   related and assume a phone channel file descriptor.
   Caller of anyone of the functions must make sure that a phone
   channel is used. In case data channel functions are invoked here,
   an instance of the data channel must be passed.
*/

/* ============================= */
/* Includes                      */
/* ============================= */

#include "drv_tapi.h"

/* ============================= */
/* Local macros and definitions  */
/* ============================= */


/* ============================= */
/* Local function declaration    */
/* ============================= */

IFX_LOCAL IFX_uint32_t  ringbursttime_get    (TAPI_CONNECTION *pChannel, IFX_boolean_t bPeriodical);
IFX_LOCAL IFX_uint32_t  ringpausetime_get    (TAPI_CONNECTION *pChannel, IFX_boolean_t bPeriodical);
IFX_LOCAL IFX_int32_t   ringcadence_change   (TAPI_CONNECTION *pChannel, IFX_boolean_t bRingBurst);
IFX_LOCAL IFX_int32_t   ringcadence_play     (TAPI_CONNECTION *pChannel);
IFX_LOCAL IFX_boolean_t ringcadencebit_check (TAPI_CONNECTION *pChannel);

/* ============================= */
/* Local function declaration    */
/* ============================= */

/**
   gets the on time (burst time)
\param
   pChannel         - handle to TAPI_CONNECTION structure
\param
   bPeriodical      - IFX_TRUE=Periodical cadence/FALSE = Non periodical cadence
\return
   On time of periodical or non periodical cadence
\remarks
   every cadence bit set represents 50 ms on time.
*/
IFX_LOCAL IFX_uint32_t ringbursttime_get (TAPI_CONNECTION *pChannel,
                                          IFX_boolean_t bPeriodical)
{
   IFX_TAPI_RING_CADENCE_t *pRingCadence = &pChannel->TapiRingData.RingCadence;
   IFX_uint8_t  nByte, nBitsSet = 0, count;
   IFX_uint32_t nTimeOn;
   IFX_int32_t i, j;
   IFX_char_t *pBuf = NULL;

   /* set cadence buffer */
   if (bPeriodical == IFX_TRUE)
   {
      pBuf  = &pRingCadence->data [0];
      count = (IFX_uint8_t)(pRingCadence->nr / 8);
   }
   else
   {
      pBuf  = &pRingCadence->initial [0];
      count = (IFX_uint8_t)(pRingCadence->initialNr / 8);
   }
   /* calculate number of bits set in the initial cadence buffer */
   for (i = 0; i < count; i++)
   {
      nByte = (IFX_uint8_t)pBuf [i];
      /* count bits set only if at least one bit is set in this byte */
      if (nByte != 0)
      {
         for (j = 0; j < 8 ; j++)
         {
            if (nByte & 0x80)
            {
               nBitsSet ++;
            }
            nByte <<= 1;
         }
      }
      /* nByte == 0 : we are in the unset part of cadence buffer. stop here */
      else
         break;
   }
   /* calculate on time in ms. 1 bit set = 50 ms on time */
   nTimeOn = (nBitsSet * 50);

   return nTimeOn;
}

/**
   Calculates the off time for non periodic initial ringing
\param
   pChannel    - handle to TAPI_CONNECTION structure
\param
   bPeriodical - IFX_TRUE = Periodical cadence / FALSE = Non periodical cadence
\return
   Off time of periodical or non periodical cadence.
\remarks
   every cadence bit not set represents 50 ms off time.
*/
IFX_LOCAL IFX_uint32_t ringpausetime_get (TAPI_CONNECTION *pChannel,
                                          IFX_boolean_t bPeriodical)
{
   IFX_TAPI_RING_CADENCE_t *pRingCadence = &pChannel->TapiRingData.RingCadence;
   IFX_uint8_t nByte, nBitsUnset = 0, count;
   IFX_uint32_t nTimeOff;
   IFX_int32_t i, j;
   IFX_char_t *pBuf = NULL;

   /* set cadence buffer */
   if (bPeriodical == IFX_TRUE)
   {
      pBuf  = &pRingCadence->data [0];
      count = (IFX_uint8_t)(pRingCadence->nr / 8);
   }
   else
   {
      pBuf  = &pRingCadence->initial [0];
      count = (IFX_uint8_t)(pRingCadence->initialNr / 8);
   }

   /* calculate number of bits not set in the initial cadence buffer */
   for (i = 0; i < count; i++)
   {
      nByte = (IFX_uint8_t)pBuf [i];
      /* count bits unset only if at least one bit is unset in this byte */
      if (nByte != 0xFF)
      {
         for (j = 0; j < 8 ; j++)
         {
            if ((nByte & 0x80) == 0)
            {
               nBitsUnset ++;
            }
            nByte <<= 1;
         }
      }
      /* nByte == 0xFF : we are in the set part of cadence buffer. stop here */
      if ((nByte == 0xFF) && (nBitsUnset != 0))
         break;
   }
   /* calculate off time in ms. 1 bit set = 50 ms on time */
   nTimeOff = (nBitsUnset * 50);

   return nTimeOff;
}

/**
   Help function to check if linemode has to be changed.
\param
   pChannel  - handle to TAPI_CONNECTION structure
\return
   Return IFX_TRUE if the linemode has to change, otherwise IFX_FALSE
*/
IFX_LOCAL IFX_boolean_t ringcadencebit_check (TAPI_CONNECTION *pChannel)
{
   IFX_char_t      nCadenceByte;
   IFX_boolean_t   bChangeLineMode = IFX_FALSE;
   TAPI_RING_DATA *pRingData = &pChannel->TapiRingData;

   /* get actual cadence byte */
   nCadenceByte = pRingData->RingCadence.data [pRingData->nByteCounter];
   /* shift current bit to the front */
   nCadenceByte = (IFX_char_t)((IFX_uint8_t)nCadenceByte << pRingData->nBitCounter);
   /* switch linemode only when bit has changed */
   if ( ((nCadenceByte & 0x80) == 0x00) && (pRingData->bIsRingBurstState))
   {
      bChangeLineMode = IFX_TRUE;
      pRingData->bIsRingBurstState = IFX_FALSE;
   }
   if ( ((nCadenceByte & 0x80) == 0x80) && (!pRingData->bIsRingBurstState) )
   {
      bChangeLineMode = IFX_TRUE;
      pRingData->bIsRingBurstState = IFX_TRUE;
   }
   /* check if this bit is the last one of this byte,
      and increase it or set it to zero */
   if (pRingData->nBitCounter == 7)
   {
      if (pRingData->nByteCounter == ((pRingData->RingCadence.nr / 8) - 1))
      {
         pRingData->nByteCounter = 0;
      }
      else
      {
         pRingData->nByteCounter++;
      }
      pRingData->nBitCounter = 0;
   }
   else
   {
      pRingData->nBitCounter++;
   }

   return (bChangeLineMode);
}

/**
   manages the periodical cadence within the ringing timer
\param
   pChannel    - handle to TAPI_CONNECTION structure
\param
   bRingBurst  - IFX_TRUE : Ring Burst / IFX_FALSE: Ring Pause
\return
   IFX_SUCCESS / IFX_ERROR
*/
IFX_LOCAL IFX_int32_t ringcadence_change (TAPI_CONNECTION *pChannel,
                                          IFX_boolean_t bRingBurst)
{
   IFX_int32_t ret;
   TAPI_RING_DATA  *pRingData = &pChannel->TapiRingData;

   /* switch between ring burst and ring pause */
   if (bRingBurst == IFX_TRUE)
   {
      ret = TAPI_Phone_Set_Linefeed(pChannel, IFX_TAPI_LINE_FEED_RING_BURST);
   }
   else
   {
      /* Ring pause */
      ret = TAPI_Phone_Set_Linefeed(pChannel, IFX_TAPI_LINE_FEED_RING_PAUSE);
      /* check if blocking mode */
      if ((ret == IFX_SUCCESS) &&  (pRingData->nRingsLeft))
      {
         /* reduce number of rings */
         pRingData->nRingsLeft--;
         if (pRingData->nRingsLeft == 0)
         {
            IFXOS_WakeUpEvent(pChannel->TapiRingEvent);
         }
      }
   }

   return ret;
}

/**
   plays the periodical cadence as programmed
\param
   pChannel    - handle to TAPI_CONNECTION structure
\return
   IFX_SUCCESS / IFX_ERROR
*/
IFX_LOCAL IFX_int32_t ringcadence_play   (TAPI_CONNECTION *pChannel)
{
   IFX_int32_t ret = IFX_SUCCESS;

   /* channel i is in ringing mode, check for changing between
      ring and ring pause */
   if (ringcadencebit_check(pChannel))
   {
      ret = ringcadence_change (pChannel, pChannel->TapiRingData.bIsRingBurstState);
   }

   return ret;
}

/* ============================= */
/* Global function definition    */
/* ============================= */

/**
   Starts the timer for ringing
\param
   pChannel - handle to TAPI_CONNECTION structure
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   if a cadence is set for non periodic initial ringing, a one shot timer will
   be programmed for this, and later on, a periodic timer will be programmed for
   the cadence sequence. Otherwise the periodic timer will be programmed.
   The period of the periodic timer is 50 ms, which is the smallest unit for
   high resolution ring cadence.
   It should be verified whether the operating system can deal with such little
   periods.
*/
IFX_int32_t TAPI_Phone_Ringtimer_Start(TAPI_CONNECTION *pChannel)
{
   TAPI_RING_DATA *pRingData = &pChannel->TapiRingData;
   IFX_uint32_t nTime;

   pRingData->bIsRingBurstState = IFX_FALSE;
   pRingData->nRingTimerState   = TAPI_RING_STATE_IDLE;
   /* check if non periodic initial ringing should be done */
   if (pRingData->RingCadence.initialNr != 0)
   {
      nTime = ringbursttime_get (pChannel, IFX_FALSE);
      pRingData->nRingTimerState = TAPI_RING_STATE_INITIAL_RINGING;
      pRingData->bIsRingBurstState = IFX_TRUE;
      TAPI_Phone_Set_Linefeed (pChannel, IFX_TAPI_LINE_FEED_RING_BURST);
      TAPI_SetTime_Timer (pRingData->RingTimerID, nTime, IFX_FALSE, IFX_FALSE);
   }
   else
   {
      pRingData->nRingTimerState = TAPI_RING_STATE_PERIODICAL_RINGING;
      TAPI_SetTime_Timer (pRingData->RingTimerID, 50, IFX_TRUE, IFX_TRUE);
   }
   pRingData->bRingingMode = IFX_TRUE;

   return IFX_SUCCESS;
}

/**
   Function called from Timer and switches the Ringing status.
\param
   Timer - TimerID of timer that expires
\param
   nArg  - Argument of timer including the TAPI_CONNECTION structure
           (as integer pointer)
\return
   none.
*/
IFX_void_t TAPI_Phone_Ring_OnTimer(Timer_ID Timer, IFX_int32_t nArg)
{
   TAPI_CONNECTION *pChannel  = (TAPI_CONNECTION *) nArg;
   TAPI_RING_DATA  *pRingData = &pChannel->TapiRingData;
   IFX_uint32_t    nTime     = 0;

   switch (pRingData->nRingTimerState)
   {
   case  TAPI_RING_STATE_INITIAL_RINGING:
      if (pRingData->bIsRingBurstState == IFX_TRUE)
      {
         nTime = ringpausetime_get (pChannel, IFX_FALSE);
         pRingData->bIsRingBurstState = IFX_FALSE;
         TAPI_Phone_Set_Linefeed (pChannel, IFX_TAPI_LINE_FEED_RING_PAUSE);
         TAPI_SetTime_Timer (pRingData->RingTimerID, nTime,
                             IFX_FALSE, IFX_FALSE);
         /* set the event flag that the ringing phase has ended */
         pChannel->TapiMiscData.nException.Bits.ring_finished = IFX_TRUE;
         TAPI_WakeUp(pChannel);
      }
      else if (pRingData->RingCadence.nr)
      {
         /* parameter for cadence ringing is given to the driver.
            Initiate the cadence ringing */
         pRingData->nRingTimerState = TAPI_RING_STATE_PERIODICAL_RINGING;
         TAPI_SetTime_Timer (pRingData->RingTimerID, 50, IFX_TRUE, IFX_TRUE);
      }
      break;
   case  TAPI_RING_STATE_PERIODICAL_RINGING:
      ringcadence_play (pChannel);
      break;
   default:
      break;
   }
}

/**
   This service sets the ring cadence for the non-blocking ringing services
\param
   pChannel - handle to TAPI_CONNECTION structure
\param
   nCadence - contains the encoded cadence sequence
\return
   returns an error code:
               - IFX_SUCCESS  -> cadence is set
               - IFX_ERROR    -> cadence is NOT set
\remarks
   - This function codes the 32 cadence bits in 320 bits buffer (High resolution
     buffer with 40 bytes)
   - 1 cadence bit set = 0,5 s = 10 * 50 ms for high resolution.
*/
IFX_int32_t  TAPI_Phone_Ring_Cadence(TAPI_CONNECTION *pChannel,
                                     IFX_uint32_t nCadence)
{
   TAPI_RING_DATA *pRingData = &pChannel->TapiRingData;
   IFX_char_t  nBuffer      = 0,
               *pCadence    = pRingData->RingCadence.data;
   IFX_uint8_t nByteCounter = 0,
               nBitCounter  = 0;
   IFX_int8_t  i, n;

   /* check if telephone is ringing*/
   if (pRingData->bRingingMode == IFX_TRUE)
   {
      LOG(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: could not set cadence!"
          " channel is ringing\n\r"));
      return (IFX_ERROR);
   }
   /* return error if cadence is 0 */
   if (!nCadence)
   {
      return (IFX_ERROR);
   }
   for (i = 0; i < 32; i++)
   {
      if (nCadence & 0x80000000)
      {
         nBuffer = 0x01;
      }
      else
      {
         nBuffer = 0x00;
      }
      for (n = 0; n < 10; n++)
      {
			pCadence [nByteCounter]  = (IFX_char_t)((IFX_uint8_t)pCadence [nByteCounter] << 1);
			pCadence [nByteCounter] |= nBuffer;
         if (nBitCounter == 7)
         {
            nByteCounter++;
            nBitCounter = 0;
         }
         else
         {
            nBitCounter++;
         }
      }
      nCadence <<= 1;
   }
   /* no initial non periodic cadence */
   pRingData->RingCadence.initialNr = 0;
   /* length of periodic cadence : 320 bits = 40 Bytes */
   pRingData->RingCadence.nr = 320;

   return (IFX_SUCCESS);
}

/**
   This service sets the ring high resolution cadence for the non-blocking
   ringing services
\param
   pChannel - handle to TAPI_CONNECTION structure
\param
   pCadence - contains the pointer to encoded cadence sequence structure
\return
   returns an error code
   - IFX_SUCCESS  -> cadence is set
   - IFX_ERROR    -> cadence is NOT set
*/
IFX_int32_t  TAPI_Phone_Ring_Cadence_High_Res(TAPI_CONNECTION *pChannel,
                                              IFX_TAPI_RING_CADENCE_t const *pCadence)
{
   IFX_int32_t  i;
   IFX_uint32_t nBuffer = 0;

   /* check if telephone is ringing*/
   if (pChannel->TapiRingData.bRingingMode == IFX_TRUE)
   {
      LOG(TAPI_DRV, DBG_LEVEL_HIGH,
         ("\n\rDRV_ERROR: could not set cadence! channel is ringing\n\r"));
      return (IFX_ERROR);
   }
   /* check if number of data bits matches */
   if (((pCadence->nr + pCadence->initialNr) % 8) != 0)
   {
      LOG(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: could not set cadence!"
          " cadence bits number doesn't match\n\r"));
      return (IFX_ERROR);
   }
   /* check if cadence is 0 */
   for (i = 0; i < (pCadence->nr / 8); i++)
   {
      nBuffer += pCadence->data[i];
   }
   for (i = 0; (i < pCadence->initialNr / 8); i++)
   {
      nBuffer += pCadence->initial [i];
   }
   /* for cadence = 0 return error */
   if (!nBuffer)
   {
      return (IFX_ERROR);
   }
   /* copy cadence data into TAPIstructure */
   for (i = 0; i < (pCadence->nr / 8); i++)
   {
      pChannel->TapiRingData.RingCadence.data[i] = pCadence->data[i];
   }
   for (i = 0; (i < pCadence->initialNr / 8); i++)
   {
      pChannel->TapiRingData.RingCadence.initial [i] = pCadence->initial [i];
   }
   /* length of cadence data for non periodic one shot ringing timer */
   pChannel->TapiRingData.RingCadence.initialNr = pCadence->initialNr;
   /* length of cadence data for periodic ringing timer */
   pChannel->TapiRingData.RingCadence.nr        = pCadence->nr;

   return (IFX_SUCCESS);
}

/**
   Prepare ringing by checking current state, saving current line mode
\param
   pChannel - handle to TAPI_CONNECTION structure
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   - It is assumed that this function is called with holding the TapiDataLock!
   - operation is done on a phone channel. An instance to the data channel
     pointer should be used in case data channel variables are modified.
*/
IFX_int32_t TAPI_Phone_Ring_Prepare(TAPI_CONNECTION *pChannel)
{
   TAPI_RING_DATA *pRingData = &pChannel->TapiRingData;
   IFX_int32_t ret;

   /* check if cadence is set */
   if (!pRingData->RingCadence.nr)
   {
      LOG(TAPI_DRV,DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: ring cadence is not set\n\r"));
      return IFX_ERROR;
   }
   /* check if telephone isn't ringing */
   if (pRingData->bRingingMode)
   {
      LOG(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: channel is just ringing\n\r"));
      return IFX_ERROR;
   }
   /* check if metering is active */
   if (pChannel->TapiMeterData.bMeterActive)
   {
      LOG(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: metering is active\n\r"));
      return IFX_ERROR;
   }
#ifdef TAPI_CID
   {
      TAPI_CONNECTION *pDataCh;
      IFX_uint8_t      data_ch;

      TAPI_Phone_Get_Data_Channel (pChannel, &data_ch);
      pDataCh = &pChannel->pTapiDevice->pChannelCon [data_ch];

      if (pDataCh->TapiCidTx.bActive)
      {
         TRACE(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: caller id is "
                "active on data ch %d associated to this phone ch %d\n\r",
                data_ch, pChannel->nChannel));
         return IFX_ERROR;
      }
      pDataCh->TapiCidConf.cadenceRingBurst = ringbursttime_get(pChannel,
                                                                IFX_TRUE);
      pDataCh->TapiCidConf.cadenceRingPause = ringpausetime_get(pChannel,
                                                                IFX_TRUE);
   }
#endif /* TAPI_CID */

   /* disable automatic battery switch if enabled */
   ret = TAPI_LL_Phone_AutoBatterySwitch(pChannel);

   return ret;
}

/**
   Starts Ringing
\param
   pChannel - handle to TAPI_CONNECTION structure
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   To start ringing with caller id, the function "TAPI_Phone_CID_Seq_Tx" is used!
*/
IFX_int32_t TAPI_Phone_Ring_Start(TAPI_CONNECTION *pChannel)
{
   TAPI_RING_DATA *pRingData = &pChannel->TapiRingData;
   IFX_int32_t    ret;

   /* begin of protected area */
   IFXOS_MutexLock(pChannel->TapiDataLock);

   /* check if ring timer is available */
   if (pRingData->RingTimerID == 0)
   {
      LOG(TAPI_DRV,DBG_LEVEL_HIGH,
         ("\n\rDRV_ERROR: no ring timer available for this channel\n\r"));
      IFXOS_MutexUnlock(pChannel->TapiDataLock);
      return IFX_ERROR;
   }

   ret = TAPI_Phone_Ring_Prepare(pChannel);

   if (ret == IFX_SUCCESS)
   {
      /* start the timer */
      ret = TAPI_Phone_Ringtimer_Start(pChannel);
   }

   /* end of protected area */
   IFXOS_MutexUnlock(pChannel->TapiDataLock);

   return ret;
}

/**
   Stops the non-blocking ringing on the phone line
\param
   pChannel - handle to TAPI_CONNECTION structure
\return
   IFX_SUCCESS or IFX_ERROR
\remark
   - operation is done on a phone channel. An instance to the data channel
     pointer should be used in case data channel variables are modified.
*/
IFX_int32_t TAPI_Phone_Ring_Stop(TAPI_CONNECTION *pChannel)
{
   TAPI_RING_DATA *pRingData = &pChannel->TapiRingData;
   IFX_int32_t ret = IFX_SUCCESS;

   /* lock channel */
   IFXOS_MutexLock(pChannel->TapiDataLock);

#ifdef TAPI_CID
   {
      TAPI_CONNECTION *pDataCh;
      IFX_uint8_t      data_ch;

      TAPI_Phone_Get_Data_Channel (pChannel, &data_ch);
      pDataCh = &pChannel->pTapiDevice->pChannelCon [data_ch];
      /* in case CID tx is ongoing, disable starting of ringing */
      if (pDataCh->TapiCidTx.bActive == IFX_TRUE)
      {
         pDataCh->TapiCidTx.bRingStart = IFX_FALSE;
      }
   }
#endif /* TAPI_CID */
   /* stop only when phone is ringing */
   if (pRingData->bRingingMode)
   {
      pRingData->nRingTimerState     = TAPI_RING_STATE_IDLE;
      TAPI_Stop_Timer(pRingData->RingTimerID);
      /* reset ringing data */
      pRingData->bRingingMode        = IFX_FALSE;
      pRingData->bIsRingBurstState   = IFX_FALSE;
      pRingData->nByteCounter        = 0;
      pRingData->nBitCounter         = 0;

      /* switch off ringing */
      ret = TAPI_Phone_Set_Linefeed(pChannel, IFX_TAPI_LINE_FEED_STANDBY);
   }

   /* unlock channel */
   IFXOS_MutexUnlock(pChannel->TapiDataLock);

   return ret;
}

/**
   Sets the ring configuration for the non-blocking ringing services
\param
   pChannel        - handle to TAPI_CONNECTION structure
\param
   pRingConfig     - contains the ring modes (IFX_TAPI_RING_CFG_t)
\return
   IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t TAPI_Phone_Ring_Config (TAPI_CONNECTION *pChannel,
                                    IFX_TAPI_RING_CFG_t const *pRingConfig)
{
   TAPI_RING_DATA *pRingData = &pChannel->TapiRingData;
   IFX_int32_t ret;

   /* begin of protected area */
   IFXOS_MutexLock(pChannel->TapiDataLock);

   /* set only when channel ringing timer not running */
   if (pRingData->bRingingMode)
   {
      LOG(TAPI_DRV,DBG_LEVEL_HIGH,("\n\rDRV_ERROR: can't set ring configuration.\n\r"));
      IFXOS_MutexUnlock(pChannel->TapiDataLock);
      return IFX_ERROR;
   }

   /* save settings in TAPI structure */
   pRingData->RingConfig = *pRingConfig;

   ret = TAPI_LL_Phone_Ring_Config(pChannel, pRingConfig);

   /* end of protected area */
   IFXOS_MutexUnlock(pChannel->TapiDataLock);

   return ret;
}

/**
   Gets the ring configuration for the non-blocking ringing services
\param
   pTapiDev    - handle to TAPI_DEV structure
\param
   pRingConfig - contains the ring modes (IFX_TAPI_RING_CFG_t)
\return
   IFX_SUCCESS
*/
IFX_int32_t TAPI_Phone_Ring_GetConfig(TAPI_CONNECTION *pChannel, IFX_TAPI_RING_CFG_t *pRingConfig)
{
   /* begin of protected area */
   IFXOS_MutexLock(pChannel->TapiDataLock);

   *pRingConfig = pChannel->TapiRingData.RingConfig;

   /* end of protected area */
   IFXOS_MutexUnlock(pChannel->TapiDataLock);

   return IFX_SUCCESS;
}

/**
   Set the maximum number of blocking rings
\param
   pChannel    - handle to TAPI_CONNECTION structure
\param
   nMaxRings   - number of blocking rings
\return
   IFX_SUCCESS
*/
IFX_int32_t TAPI_Phone_Set_MaxRings(TAPI_CONNECTION *pChannel, IFX_uint32_t nMaxRings)
{
   pChannel->TapiRingData.nMaxRings = nMaxRings;
   return IFX_SUCCESS;
}

/**
   Starts the ringing in blocking mode
\param
   pChannel    - handle to TAPI_CONNECTION structure
\return
   IFX_SUCCESS - maximum number of rings reached
   1           - phone hooked off
   IFX_ERROR   - phone is actually off hook
*/
IFX_int32_t TAPI_Phone_Ring(TAPI_CONNECTION *pChannel)
{
   IFX_int32_t ret;

   if (pChannel->TapiOpControlData.bHookState)
   {
      return IFX_ERROR;
   }

   pChannel->TapiRingData.nRingsLeft = pChannel->TapiRingData.nMaxRings;

   TAPI_Phone_Ring_Start(pChannel);

   /* wait until wakeup -> maxrings have reached
                        -> hook off              */
   IFXOS_WaitEvent(pChannel->TapiRingEvent);

   if (pChannel->TapiRingData.bRingingMode)
      TAPI_Phone_Ring_Stop(pChannel);

   /* hook off */
   if (pChannel->TapiRingData.nRingsLeft)
   {
      pChannel->TapiRingData.nRingsLeft = 0;
      ret = 1;
   }
   /* maxrings have reached */
   else
   {
      ret = IFX_SUCCESS;
   }

   return ret;
}



