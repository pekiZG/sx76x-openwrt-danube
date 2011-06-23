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
   Module      : drv_tapi_ring.c
   Desription  : Contains TAPI Ringing Services.
   Remarks     :

   All operations done by functions in this module are phone
   related and assumes a phone channel file descriptor.
   Caller of anyone of the functions must make sure that a phone
   channel is used. In case data channel functions are invoked here,
   an instance of the data channel must be passed.
*******************************************************************************/

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

#ifdef TAPI_CID
IFX_LOCAL IFX_void_t   ringpausetimecid_add  (TAPI_CONNECTION *pChannel);
IFX_LOCAL IFX_int32_t  ringalertcid_default  (TAPI_CONNECTION *pChannel);
IFX_LOCAL IFX_int32_t  ringalertcid_ntt      (TAPI_CONNECTION *pChannel);
IFX_LOCAL IFX_int32_t  ringcidtxend_default  (TAPI_CONNECTION *pChannel);
#endif /* TAPI_CID */

IFX_LOCAL IFX_uint32_t  ringbursttime_get    (TAPI_CONNECTION *pChannel, IFX_boolean_t bPeriodical);
IFX_LOCAL IFX_uint32_t  ringpausetime_get    (TAPI_CONNECTION *pChannel, IFX_boolean_t bPeriodical);
IFX_LOCAL IFX_int32_t   ringtimer_start      (TAPI_CONNECTION *pChannel);
IFX_LOCAL IFX_int32_t   ringcadence_change   (TAPI_CONNECTION *pChannel, IFX_boolean_t bRingBurst);
IFX_LOCAL IFX_int32_t   ringcadence_play     (TAPI_CONNECTION *pChannel);
IFX_LOCAL IFX_boolean_t ringcadencebit_check (TAPI_CONNECTION *pChannel);

/* ============================= */
/* Local function declaration    */
/* ============================= */

#ifdef TAPI_CID

/*******************************************************************************
Description:
   Sets additionnal time for CID phase according to delay and cadence off time
Arguments:
   pChannel       - handle to TAPI_CONNECTION structure, phone channel
Return:
   none
Remark:
   The timer which triggers the sending of CID will be reschedule with this
   additional time before doing the next operation.
   CID configuration and data are stored in the data channel.
*******************************************************************************/
IFX_LOCAL IFX_void_t ringpausetimecid_add (TAPI_CONNECTION *pChannel)
{
   TAPI_RING_DATA  *pRingData  = &pChannel->TapiRingData;
   TAPI_CID_DATA   *pCidData   = &pRingData->pDataCh->TapiCidTx;
   TAPI_CID_CONFIG *pCidConfig = &pRingData->pDataCh->TapiCidConfig;

   /* set additional time according to appearance */
   switch (pCidConfig->nAppearance)
   {
   case CID_AFTER_DELAY:
   case CID_BETWEEN_RINGBURST:
      /* calculate rest of time after initial delay */
      if (pCidConfig->nAppearance == CID_AFTER_DELAY)
      {
         pCidData->nAdditionalTime  = ringpausetime_get (pChannel, IFX_FALSE);
      }
      else
      {
         pCidData->nAdditionalTime  = ringpausetime_get (pChannel, IFX_TRUE);
      }
      if (pCidData->nAdditionalTime > pCidConfig->nDelay)
      {
         /* wait only the rest of cadence off time */
         pCidData->nAdditionalTime -= pCidConfig->nDelay;
      }
      else
      {
         /* cadence off time is less than delay already consumed.
         No additionnal time */
         pCidData->nAdditionalTime = 0;
      }
      break;
   case CID_BEFORE_RINGBURST:
   default:
      pCidData->nAdditionalTime = 0;
      break;
   }
}

/*******************************************************************************
Description:
   Ringing alert machine  according to NTT standard
Arguments:
   pChannel       - handle to TAPI_CONNECTION structure, phone channel
Return:
   IFX_SUCCESS/IFX_ERROR
Remark:

*******************************************************************************/
IFX_LOCAL IFX_int32_t ringalertcid_ntt (TAPI_CONNECTION *pChannel)
{
   TAPI_RING_DATA  *pRingData  = &pChannel->TapiRingData;
   TAPI_CID_DATA   *pCidData   = &pRingData->pDataCh->TapiCidTx;
   TAPI_CID_CONFIG *pCidConfig = &pRingData->pDataCh->TapiCidConfig;

   /* special case NTT (Japan)*/
   switch (pRingData->nRingTimerState)
   {
   case  TAPI_RING_STATE_IDLE:
      /*PRINTF ("NTT Alert: reversal & 500 ms timer\r\n");*/
      /* set line reversal mode */
      TAPI_Phone_Set_Linefeed (pChannel, TAPI_LINEFEED_REVERSED);
      /* do periodical ringing with 500 ms period  */
      pRingData->nRingTimerState = TAPI_RING_STATE_SALERT;
      TAPI_SetTime_Timer (pRingData->RingTimerID, 500, IFX_TRUE, IFX_TRUE);
      /* reset offhook state flags */
      pChannel->TapiMiscData.nException.Bits.hookstate = IFX_FALSE;
      pChannel->TapiOpControlData.bHookState = IFX_FALSE;
      break;
   case TAPI_RING_STATE_SALERT:
      if (pRingData->bIsRingBurstState == IFX_FALSE)
      {
         pRingData->bIsRingBurstState = IFX_TRUE;
      }
      else
      {
         pRingData->bIsRingBurstState = IFX_FALSE;
      }
      ringcadence_change (pChannel, pRingData->bIsRingBurstState);
      /* in case offhook occured, send CID after delay now */
      if ((pRingData->bIsRingBurstState == IFX_FALSE) &&
          (pCidData->bCidAlertEvt == IFX_TRUE))
      {
         /*PRINTF ("NTT offhook: send CID\r\n");*/
         pRingData->nRingTimerState = TAPI_RING_STATE_CID_START;
         TAPI_SetTime_Timer (pRingData->RingTimerID, pCidConfig->nDelay,
                             IFX_FALSE, IFX_FALSE);
      }
      else if (pCidData->nBurstCnt == pCidConfig->nBurstCount)
      {
         LOG(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_WARN: No offhook during "
             "the short ringing cadence as specified by NTT\n\r"));
         /* number of bursts before cid reached without offhook. Forget about
            CID sending */
         pCidData->bIsNewData = IFX_FALSE;
         /* setup periodical timer */
         pRingData->nRingTimerState = TAPI_RING_STATE_PERIODICAL_RINGING;
         TAPI_SetTime_Timer (pRingData->RingTimerID, 50, IFX_TRUE, IFX_TRUE);
      }
      break;
   default:
      break;
   }

   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   Ringing alert machine according to user configuration (No NTT)
Arguments:
   pChannel       - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS/IFX_ERROR
Remark:

*******************************************************************************/
IFX_LOCAL IFX_int32_t ringalertcid_default (TAPI_CONNECTION *pChannel)
{
   TAPI_RING_DATA  *pRingData  = &pChannel->TapiRingData;
   TAPI_CID_DATA   *pCidData   = &pRingData->pDataCh->TapiCidTx;
   TAPI_CID_CONFIG *pCidConfig = &pRingData->pDataCh->TapiCidConfig;
   IFX_uint32_t nTime;

   /* set the next step to go */
   switch (pCidConfig->nAppearance)
   {
   case CID_BEFORE_RINGBURST:
      pRingData->nRingTimerState = TAPI_RING_STATE_CID_START;
      nTime = pCidConfig->nDelay;
      pRingData->bIsRingBurstState = IFX_FALSE;
      TAPI_Phone_Set_Linefeed (pChannel, TAPI_LINEFEED_NORMAL);
      TAPI_SetTime_Timer (pRingData->RingTimerID, nTime, IFX_FALSE, IFX_FALSE);
      break;
   case CID_AFTER_DELAY:
      if (pRingData->bIsRingBurstState == IFX_FALSE)
      {
         pRingData->nRingTimerState = TAPI_RING_STATE_INITIAL_RINGING;
         nTime = ringbursttime_get (pChannel, IFX_FALSE);
         pRingData->bIsRingBurstState = IFX_TRUE;
         TAPI_Phone_Set_Linefeed (pChannel, TAPI_LINEFEED_RING_BURST);
      }
      else
      {
         pRingData->nRingTimerState = TAPI_RING_STATE_CID_START;
         nTime = pCidConfig->nDelay;
         pRingData->bIsRingBurstState = IFX_FALSE;
         TAPI_Phone_Set_Linefeed (pChannel, TAPI_LINEFEED_NORMAL);
      }
      TAPI_SetTime_Timer (pRingData->RingTimerID, nTime, IFX_FALSE, IFX_FALSE);
      break;
   case CID_BETWEEN_RINGBURST:
      if ((pCidData->nBurstCnt == pCidConfig->nBurstCount)
          && (pRingData->bIsRingBurstState == IFX_FALSE))
      {
         pRingData->nRingTimerState = TAPI_RING_STATE_CID_START;
         nTime = pCidConfig->nDelay;
         TAPI_SetTime_Timer (pRingData->RingTimerID, nTime, IFX_FALSE, IFX_FALSE);
      }
      break;
   default:
      break;
   }

   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   ringing machine to trigger at the end of CID sending
Arguments:
   pChannel       - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS/IFX_ERROR
Remark:

*******************************************************************************/
IFX_LOCAL IFX_int32_t ringcidtxend_default (TAPI_CONNECTION *pChannel)
{
   TAPI_RING_DATA  *pRingData  = &pChannel->TapiRingData;
   TAPI_CID_CONFIG *pCidConfig = &pRingData->pDataCh->TapiCidConfig;
   IFX_uint32_t     nTime;

   /* set the next step to go */
   switch (pCidConfig->nAppearance)
   {
   case CID_BEFORE_RINGBURST:
      if (pRingData->RingCadence.initialNr != 0)
      {
         nTime = ringbursttime_get (pChannel, IFX_FALSE);
         pRingData->nRingTimerState = TAPI_RING_STATE_INITIAL_RINGING;
         pRingData->bIsRingBurstState = IFX_TRUE;
         TAPI_Phone_Set_Linefeed (pChannel, TAPI_LINEFEED_RING_BURST);
         TAPI_SetTime_Timer (pRingData->RingTimerID, nTime, IFX_FALSE, IFX_FALSE);
      }
      else
      {
         pRingData->nRingTimerState = TAPI_RING_STATE_PERIODICAL_RINGING;
         TAPI_SetTime_Timer (pRingData->RingTimerID, 50, IFX_TRUE, IFX_TRUE);
      }
      break;
   case CID_AFTER_DELAY:
   case CID_BETWEEN_RINGBURST:
   default:
      pRingData->nRingTimerState = TAPI_RING_STATE_PERIODICAL_RINGING;
      TAPI_SetTime_Timer (pRingData->RingTimerID, 50, IFX_TRUE, IFX_TRUE);
      break;
   }

   return IFX_SUCCESS;
}

#endif /* TAPI_CID */

/*******************************************************************************
Description:
   gets the on time (burst time)
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   bPeriodical     - IFX_TRUE=Periodical cadence/FALSE = Non periodical cadence
Return:
   On time of periodical or non periodical cadence
Remarks
   every cadence bit set represents 50 ms on time.
*******************************************************************************/
IFX_LOCAL IFX_uint32_t ringbursttime_get (TAPI_CONNECTION *pChannel,
                                          IFX_boolean_t bPeriodical)
{
   TAPI_RING_CADENCE *pRingCadence = &pChannel->TapiRingData.RingCadence;
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

/*******************************************************************************
Description:
   Calculates the off time for non periodic initial ringing
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   bPeriodical     - IFX_TRUE=Periodical cadence/FALSE = Non periodical cadence
Return:
   Off time of periodical or non periodical cadence.
Remarks
   every cadence bit not set represents 50 ms off time.
*******************************************************************************/
IFX_LOCAL IFX_uint32_t ringpausetime_get (TAPI_CONNECTION *pChannel,
                                          IFX_boolean_t bPeriodical)
{
   TAPI_RING_CADENCE *pRingCadence = &pChannel->TapiRingData.RingCadence;
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
   }
   /* calculate off time in ms. 1 bit set = 50 ms on time */
   nTimeOff = (nBitsUnset * 50);

   return nTimeOff;
}

/*******************************************************************************
Description:
   Helpfunction to check if linemode has to be changed.
Arguments:
   pChannel - handle to TAPI_CONNECTION structure
Return:
   Return IFX_TRUE if the linemode has to change, otherwise IFX_FALSE
*******************************************************************************/
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

/*******************************************************************************
Description:
   manages the periodical cadence within the ringing timer
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   bRingBurst  - IFX_TRUE : Ring Burst / IFX_FALSE: Ring Pause
Return:
   IFX_SUCCESS / IFX_ERROR
*******************************************************************************/
IFX_LOCAL IFX_int32_t ringcadence_change (TAPI_CONNECTION *pChannel,
                                          IFX_boolean_t bRingBurst)
{
   IFX_int32_t ret;
   TAPI_RING_DATA  *pRingData = &pChannel->TapiRingData;
#ifdef TAPI_CID
   TAPI_CID_DATA   *pCidData   = &pRingData->pDataCh->TapiCidTx;
   TAPI_CID_CONFIG *pCidConfig = &pRingData->pDataCh->TapiCidConfig;
#endif /* TAPI_CID */

   /* switch between ring burst and ring pause */
   if (bRingBurst == IFX_TRUE)
   {
      ret = TAPI_Phone_Set_Linefeed(pChannel, TAPI_LINEFEED_RING_BURST);
#ifdef TAPI_CID
      if ((ret == IFX_SUCCESS) &&
          (pCidData->nBurstCnt < pCidConfig->nBurstCount))
      {
         pCidData->nBurstCnt++;
      }
#endif /* TAPI_CID */
   }
   else
   {
      /* Ring pause */
      ret = TAPI_Phone_Set_Linefeed(pChannel, TAPI_LINEFEED_NORMAL);
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

/*******************************************************************************
Description:
   plays the periodical cadence as programmed
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS / IFX_ERROR
*******************************************************************************/
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

/*******************************************************************************
Description:
   Starts the timer for ringing
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks
   if a cadence is set for non periodic initial ringing, a one shot timer will
   be programmed for this, and later on, a periodic timer will be programmed for
   the cadence sequence. Otherwise the periodic timer will be programmed.
   The period of the periodic timer is 50 ms, which is the smallest unit for
   high resolution ring cadence.
   It should be verified wheter the operating system can deal with such little
   periods.

   The ring timer should manage following cases according to configuration
   at ring start:

     1.                          2.                  3.
   - CID                -> Periodical Ringing
   - CID                -> Oneshot Ringing     -> Periodical Ringing
   - Oneshot Ringing    -> CID                 -> Periodical Ringing
   - Oneshot Ringing    -> Periodical Ringing  -> CID
   - Periodical Ringing -> CID
   - Periodical Ringing -> CID                 -> Periodical Ringing (NTT)
   - Oneshot Ringing    -> Periodical Ringing
   - Periodical Ringing
*******************************************************************************/
IFX_LOCAL IFX_int32_t ringtimer_start(TAPI_CONNECTION *pChannel)
{
   TAPI_RING_DATA  *pRingData  = &pChannel->TapiRingData;
   TAPI_CID_DATA   *pCidData   = &pRingData->pDataCh->TapiCidTx;
   TAPI_CID_CONFIG *pCidConfig = &pRingData->pDataCh->TapiCidConfig;
   IFX_uint32_t nTime;

   pRingData->bIsRingBurstState = IFX_FALSE;
   pRingData->nRingTimerState   = TAPI_RING_STATE_IDLE;
#ifdef TAPI_CID
   if (pCidData->bIsNewData == IFX_TRUE)
   {
      if (pCidConfig->nSpec == TAPI_CID_NTT)
      {
         TAPI_Phone_CID1_OpControl (pChannel, TAPI_CID1_ALERT, NULL,
                                  ringalertcid_ntt, NULL);
      }
      else
      {
         TAPI_Phone_CID1_OpControl (pChannel, TAPI_CID1_ALERT, NULL,
                                  ringalertcid_default, NULL);
      }
   }
#endif /* TAPI_CID */
   if (pRingData->nRingTimerState == TAPI_RING_STATE_IDLE)
   {
      /* check if non periodic initial ringing should be done */
      if (pRingData->RingCadence.initialNr != 0)
      {
         nTime = ringbursttime_get (pChannel, IFX_FALSE);
         pRingData->nRingTimerState = TAPI_RING_STATE_INITIAL_RINGING;
         pRingData->bIsRingBurstState = IFX_TRUE;
         TAPI_Phone_Set_Linefeed (pChannel, TAPI_LINEFEED_RING_BURST);
         TAPI_SetTime_Timer (pRingData->RingTimerID, nTime, IFX_FALSE, IFX_FALSE);
      }
      else
      {
         pRingData->nRingTimerState = TAPI_RING_STATE_PERIODICAL_RINGING;
         TAPI_SetTime_Timer (pRingData->RingTimerID, 50, IFX_TRUE, IFX_TRUE);
      }
   }

   return IFX_SUCCESS;
}

/* ============================= */
/* Global function definition    */
/* ============================= */

/*******************************************************************************
Description:
   Function called from Timer and switches the Ringing status.
Arguments:
   Timer - TimerID of timer that exipres
   nArg  - Argument of timer including the TAPI_CONNECTION structure
           (as integer pointer)
Return:
   none.
Remarks:
   The ring timer should manage following cases according to configuration
   at ring start:

     1.                          2.                  3.
   - CID                -> Periodical Ringing
   - CID                -> Oneshot Ringing     -> Periodical Ringing
   - Oneshot Ringing    -> CID                 -> Periodical Ringing
   - Oneshot Ringing    -> Periodical Ringing  -> CID
   - Periodical Ringing -> CID
   - Periodical Ringing -> CID                 -> Periodical Ringing (NTT)
   - Oneshot Ringing    -> Periodical Ringing
   - Periodical Ringing
*******************************************************************************/
IFX_void_t TAPI_Phone_Ring_OnTimer(Timer_ID Timer, IFX_int32_t nArg)
{
   TAPI_CONNECTION *pChannel   = (TAPI_CONNECTION *) nArg;
   TAPI_RING_DATA  *pRingData  = &pChannel->TapiRingData;
   TAPI_CID_DATA   *pCidData   = &pRingData->pDataCh->TapiCidTx;
   TAPI_CID_CONFIG *pCidConfig = &pRingData->pDataCh->TapiCidConfig;
   IFX_uint32_t     nTime      = 0;
   IFX_int32_t      ret        = IFX_SUCCESS;

   switch (pRingData->nRingTimerState)
   {
#ifdef TAPI_CID
   case TAPI_RING_STATE_CID_START:
      pRingData->nRingTimerState = TAPI_RING_STATE_CID_END;
      ret = TAPI_Phone_CID1_OpControl (pChannel, TAPI_CID1_SEND,
                                       Timer, NULL, NULL);
      if (ret == IFX_ERROR)
      {
         TAPI_Phone_Ring_Stop(pChannel);
      }
      break;
   case TAPI_RING_STATE_CID_END:
      TAPI_Phone_CID1_OpControl (pChannel, TAPI_CID1_END, NULL,
                                 NULL, ringcidtxend_default);
      break;
   case  TAPI_RING_STATE_SALERT:
      /* special case for NTT (japan) */
      if (pCidData->bIsNewData == IFX_TRUE)
      {
         TAPI_Phone_CID1_OpControl (pChannel, TAPI_CID1_ALERT,
                                    NULL, ringalertcid_ntt, NULL);
      }
      break;
#endif /* TAPI_CID */
   case  TAPI_RING_STATE_INITIAL_RINGING:
#ifdef TAPI_CID
      /* check if there are cid data to send after  non periodical ringing */
      if ((pCidData->bIsNewData == IFX_TRUE) &&
          (pCidConfig->nAppearance == CID_AFTER_DELAY))
      {
         TAPI_Phone_CID1_OpControl (pChannel, TAPI_CID1_ALERT, NULL,
                                    ringalertcid_default, NULL);
      }
      else
#endif /* TAPI_CID */
      {
         if (pRingData->bIsRingBurstState == IFX_TRUE)
         {
            nTime = ringpausetime_get (pChannel, IFX_FALSE);
            pRingData->bIsRingBurstState = IFX_FALSE;
            TAPI_Phone_Set_Linefeed (pChannel, TAPI_LINEFEED_NORMAL);
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
            TAPI_SetTime_Timer (pRingData->RingTimerID, 50,
                                IFX_TRUE, IFX_TRUE);
         }
      }
      break;
   case  TAPI_RING_STATE_PERIODICAL_RINGING:
      ringcadence_play (pChannel);
#ifdef TAPI_CID
      if (pCidData->bIsNewData == IFX_TRUE)
      {
         TAPI_Phone_CID1_OpControl (pChannel, TAPI_CID1_ALERT,
                                    NULL, ringalertcid_default, NULL);
      }
#endif /* TAPI_CID */
      break;
   default:
      break;
   }
}

/*******************************************************************************
Description:
   This service sets the ring cadence for the non-blocking ringing services
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   nCadence    - contains the encoded cadence sequence
Return:
   returns an error code:
               - IFX_SUCCESS  -> cadence is set
               - IFX_ERROR    -> cadence is NOT set
Remarks
   - This function codes the 32 cadence bits in 320 bits buffer (High resolution
     buffer with 40 bytes)
   - 1 cadence bit set = 0,5 s = 10 * 50 ms for high resolution.
*******************************************************************************/
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

/*******************************************************************************
Description:
   This service sets the ring high resolution cadence for the non-blocking
   ringing services
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   pCadence    - contains the pointer to encoded cadence sequence structure
Return:
   returns an error code
   - IFX_SUCCESS  -> cadence is set
   - IFX_ERROR    -> cadence is NOT set
*******************************************************************************/
IFX_int32_t  TAPI_Phone_Ring_Cadence_High_Res(TAPI_CONNECTION *pChannel,
                                              TAPI_RING_CADENCE const *pCadence)
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

/*******************************************************************************
Description:
   Starts Ringing
Arguments:
   pChannel   - handle to TAPI_CONNECTION structure
   pPhoneCid  - ptr to TAPI_PHONE_CID structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks
   - This operation is done on a phone channel. In case CID processing should
      be done together with ringing, care must be taken that CID is sent on a
      data channel mapped to this phone channel.
   -  The (un)mapping of any data channel to any phone channel is a user task.
*******************************************************************************/
IFX_int32_t TAPI_Phone_Ring_Start(TAPI_CONNECTION *pChannel,
                                  TAPI_PHONE_CID const *pPhoneCid)
{
   TAPI_RING_DATA *pRingData = &pChannel->TapiRingData;
   IFX_int32_t ret = IFX_SUCCESS;

   /* begin of protected area */
   IFXOS_MutexLock(pChannel->TapiDataLock);

   /* check if ring timer is available */
   if(pRingData->RingTimerID == 0)
   {
      LOG(TAPI_DRV,DBG_LEVEL_HIGH,
         ("\n\rDRV_ERROR: no ring timer available for this channel\n\r"));
      IFXOS_MutexUnlock(pChannel->TapiDataLock);
      return IFX_ERROR;
   }

   /* check if telephone isn't ringing */
   if (pRingData->bRingingMode)
   {
      LOG(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: channel is just ringing\n\r"));
      IFXOS_MutexUnlock(pChannel->TapiDataLock);
      return IFX_ERROR;
   }
   /* check if metering is active */
   if (pChannel->TapiMeterData.bMeterActive)
   {
      LOG(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: metering is active\n\r"));
      IFXOS_MutexUnlock(pChannel->TapiDataLock);
      return IFX_ERROR;
   }
   /* save line mode, polarity and automatic battery switch */
   pRingData->nLastLineMode  = pChannel->TapiOpControlData.nLineMode;
   /* disable automatic battery switch if enabled */
   if (pChannel->TapiOpControlData.nBatterySw)
   {
      pChannel->TapiOpControlData.nBatterySw = 0x00;
      ret = TAPI_LL_Phone_AutoBatterySwitch(pChannel);
   }
#ifdef TAPI_CID
   if ((ret == IFX_SUCCESS) && (pPhoneCid != NULL))
   {
      IFX_uint8_t data_ch = 0;

      TAPI_Phone_Get_Data_Channel (pChannel, &data_ch);
      pRingData->pDataCh = &pChannel->pTapiDevice->pChannelCon [data_ch];
      /* process phone cid data*/
      ret = TAPI_Phone_CID_SetData (pRingData->pDataCh/*pChannel*/, pPhoneCid);
      if ((ret == IFX_SUCCESS) && /*pChannel*/
          (pRingData->pDataCh->TapiCidConfig.nSpec != TAPI_CID_NTT))
      {
         ringpausetimecid_add (pChannel);
      }
   }
#endif /* TAPI_CID */
   if (ret == IFX_SUCCESS)
   {
      /* start the timer */
      ret = ringtimer_start(pChannel);
   }
   if (ret == IFX_SUCCESS)
   {
      /* channel is now in ringing mode */
      pRingData->bRingingMode   = IFX_TRUE;
   }

   /* end of protected area */
   IFXOS_MutexUnlock(pChannel->TapiDataLock);

   return ret;
}

/*******************************************************************************
Description:
   Stops the non-blocking ringing on the phone line
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks

*******************************************************************************/
IFX_int32_t TAPI_Phone_Ring_Stop(TAPI_CONNECTION *pChannel)
{
   TAPI_RING_DATA *pRingData = &pChannel->TapiRingData;
   IFX_int32_t ret = IFX_SUCCESS;

   /* lock channel */
   IFXOS_MutexLock(pChannel->TapiDataLock);

   /* stop only when phone is ringing */
   if (pRingData->bRingingMode)
   {
      TAPI_Stop_Timer(pRingData->RingTimerID);
      /* reset ringing data */
      pRingData->bRingingMode        = IFX_FALSE;
      pRingData->bIsRingBurstState   = IFX_FALSE;
      pRingData->nByteCounter        = 0;
      pRingData->nBitCounter         = 0;
#ifdef TAPI_CID
      pRingData->pDataCh->TapiCidTx.bIsNewData  = IFX_FALSE;
#endif /* TAPI_CID */
      /* recover the last line mode */
      ret = TAPI_Phone_Set_Linefeed(pChannel, pRingData->nLastLineMode);
   }

   /* unlock channel */
   IFXOS_MutexUnlock(pChannel->TapiDataLock);

   return ret;
}

/*******************************************************************************
Description:
   Sets the ring configuration for the non-blocking ringing services
Arguments:
   pChannel       - handle to TAPI_CONNECTION structure
   pRingConfig    - contains the ring modes (TAPI_RING_CONFIG)
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_Phone_Ring_Config (TAPI_CONNECTION *pChannel,
                                    TAPI_RING_CONFIG const *pRingConfig)
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

/*******************************************************************************
Description:
   Gets the ring configuration for the non-blocking ringing services
Arguments:
   pTapiDev       - handle to TAPI_DEV structure
   pRingConfig    - contains the ring modes (TAPI_RING_CONFIG)
Return:
   IFX_SUCCESS
*******************************************************************************/
IFX_int32_t TAPI_Phone_Ring_GetConfig(TAPI_CONNECTION *pChannel, TAPI_RING_CONFIG *pRingConfig)
{
   /* begin of protected area */
   IFXOS_MutexLock(pChannel->TapiDataLock);

   *pRingConfig = pChannel->TapiRingData.RingConfig;

   /* end of protected area */
   IFXOS_MutexUnlock(pChannel->TapiDataLock);

   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   Set the maximum number of blocking rings
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   nMaxRings   - number of blocking rings
Return:
   IFX_SUCCESS
*******************************************************************************/
IFX_int32_t TAPI_Phone_Set_MaxRings(TAPI_CONNECTION *pChannel, IFX_uint32_t nMaxRings)
{
   pChannel->TapiRingData.nMaxRings = nMaxRings;
   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   Starts the ringing in blocking mode
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS - maximum number of rings reached
   1           - phone hooked off
   IFX_ERROR   - phone is actually off hook
*******************************************************************************/
IFX_int32_t TAPI_Phone_Ring(TAPI_CONNECTION *pChannel)
{
   IFX_int32_t ret;

   if (pChannel->TapiOpControlData.bHookState)
   {
      return IFX_ERROR;
   }

   pChannel->TapiRingData.nRingsLeft = pChannel->TapiRingData.nMaxRings;

   TAPI_Phone_Ring_Start(pChannel, NULL);

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



