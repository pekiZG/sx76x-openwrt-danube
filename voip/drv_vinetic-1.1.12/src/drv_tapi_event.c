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
   Module      : drv_tapi_event.c
   Desription  : Contains TAPI Event Handling.
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */

#include "drv_tapi.h"

/* ============================= */
/* Global variable definition    */
/* ============================= */

/* ============================= */
/* Global function definition    */
/* ============================= */


/*******************************************************************************
Description:
   Event handling function. Is called from interrupt routine.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   bHookState  - IFX_TRUE : off hook
                 IFX_FALSE: on hook
Return:
   none
Remarks:
   to be done : Compare Flextronics code with former code and check if old
   functionality is still fully implemented. old code is removed with #if 0
   actually.
*******************************************************************************/
IFX_void_t TAPI_Phone_Event_HookState (TAPI_CONNECTION *pChannel,
                                 IFX_uint8_t bHookState)
{
   if (bHookState == IFX_TRUE)
   {
      /* phone has gone offhook */
      switch (pChannel->TapiDialData.nHookState)
      {
         case TAPI_HOOK_STATE_ONHOOK:
            /* Set timer to verify this offhook isn't just some line noise */
            TAPI_SetTime_Timer(pChannel->TapiDialData.DialTimerID,
                               pChannel->TapiHookOffTime.nMinTime,
                               IFX_FALSE,
                               IFX_FALSE);
            pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_OFFHOOK_VAL;
            break;

         case TAPI_HOOK_STATE_PULSE_L_CONFIRM:
            /* Offhook may indicate completion of low pulse:
               set timer to validate */
            TAPI_SetTime_Timer(pChannel->TapiDialData.DialTimerID,
                               pChannel->TapiDigitHighTime.nMinTime,
                              IFX_FALSE, IFX_TRUE);
            pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_PULSE_H_VAL;
            break;
         case TAPI_HOOK_STATE_PULSE_L_FLASH_CONFIRM:
            /* overlap of hook flash min and pulse max time. Confirmation needed:
               - interdigit time and hook flash min time ended: report digit 1
                 and flash hook
               - next pulse occurs: report only digit   */
            TAPI_SetTime_Timer(pChannel->TapiDialData.DialTimerID,
                              pChannel->TapiDigitHighTime.nMinTime,
                              IFX_FALSE, IFX_TRUE);
            pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_PULSE_H_VAL;
            pChannel->TapiDialData.state.bProbablyFlash = IFX_TRUE;
            break;
         case TAPI_HOOK_STATE_FLASH_WAIT:
            /* Offhook arrives too soon for flash: go to offhook (no event) */
            TAPI_Stop_Timer(pChannel->TapiDialData.DialTimerID);
            pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_OFFHOOK;
            break;

         case TAPI_HOOK_STATE_FLASH_CONFIRM:
            if (pChannel->TapiHookFlashMakeTime.nMinTime == 0)
            {
               /* Offhook indicates completion of flash */
               pChannel->TapiMiscData.nException.Bits.flash_hook = IFX_TRUE;
               TAPI_Stop_Timer(pChannel->TapiDialData.DialTimerID);
               pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_OFFHOOK;
               TAPI_WakeUp(pChannel);
            }
            else
            {
               /* validation time specified so validate */
               TAPI_SetTime_Timer(pChannel->TapiDialData.DialTimerID,
                                 pChannel->TapiHookFlashMakeTime.nMinTime,
                                 IFX_FALSE, IFX_TRUE);
               pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_FLASH_VAL;
            }
            break;

         case TAPI_HOOK_STATE_ONHOOK_CONFIRM:
            /* Offhook arrives too soon for final onhook: go to offhook (no event) */
            TAPI_Stop_Timer(pChannel->TapiDialData.DialTimerID);
            pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_OFFHOOK;
            break;

         case TAPI_HOOK_STATE_ONHOOK_VAL:
            /* Offhook while validating onhook: stop validation timer and return to OFFHOOK */
            /* If collecting digits, abort */
            pChannel->TapiDialData.nHookChanges = 0;
            TAPI_Stop_Timer(pChannel->TapiDialData.DialTimerID);
            pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_OFFHOOK;
            break;

         case TAPI_HOOK_STATE_DIAL_L_VAL:
            TAPI_SetTime_Timer(pChannel->TapiDialData.DialTimerID,
                               pChannel->TapiDigitHighTime.nMinTime,
                               IFX_FALSE,
                               IFX_TRUE);
            pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_PULSE_H_VAL;
            break;

         default:
            /* all other states represent an error condition */
            break;
      }
   }
   else
   {
      /* phone has gone onhook */
      switch (pChannel->TapiDialData.nHookState)
      {
         case TAPI_HOOK_STATE_FLASH_VAL:
         case TAPI_HOOK_STATE_OFFHOOK:
            /* Set timer to confirm whether this onhook is not just result of line noise */
            /* Restart=IFX_TRUE because interdigit timer could be running */
            TAPI_SetTime_Timer(pChannel->TapiDialData.DialTimerID,
                               pChannel->TapiDigitLowTime.nMinTime,
                               IFX_FALSE,
                               IFX_TRUE);
            pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_ONHOOK_VAL;
            break;

         case TAPI_HOOK_STATE_PULSE_H_CONFIRM:
            /* Set timer to confirm whether this onhook is not just result of line noise */
            /* Restart=IFX_TRUE because DigitHigh.nMaxTime timer is running */
            TAPI_SetTime_Timer(pChannel->TapiDialData.DialTimerID,
                               pChannel->TapiDigitLowTime.nMinTime,
                               IFX_FALSE, IFX_TRUE);
            pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_DIAL_L_VAL;
            break;

         case TAPI_HOOK_STATE_PULSE_H_VAL:
            /* Pulse duration too short */
            TAPI_SetTime_Timer(pChannel->TapiDialData.DialTimerID,
                               pChannel->TapiDigitLowTime.nMinTime,
                               IFX_FALSE,
                               IFX_TRUE);
            pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_DIAL_L_VAL;
            break;

         case TAPI_HOOK_STATE_OFFHOOK_VAL:
            TAPI_Stop_Timer(pChannel->TapiDialData.DialTimerID);
            pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_ONHOOK;
            break;

         default:
            /* all other states represent an error condition */
         break;
      }
   }

   return;
}

#ifdef TAPI_DTMF
/*******************************************************************************
Description:
   Event handling function. Is called from interrupt routine.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   nKey        - detected key
Return:
   none
*******************************************************************************/
IFX_void_t  TAPI_Phone_Event_DTMF (TAPI_CONNECTION *pChannel, IFX_uint8_t nKey)
{
   IFX_uint8_t  *pFifo;

   /* write dtmf digit in fifo and modify exception byte */
   pFifo = Fifo_writeElement (&(pChannel->TapiDialData.TapiDTMFFifo));
   if (pFifo != NULL)
   {
      *pFifo = nKey;
   }
   pChannel->TapiMiscData.nException.Bits.dtmf_ready = IFX_TRUE;
   TAPI_WakeUp(pChannel);

   return;
}
#endif /* TAPI_DTMF */

/*******************************************************************************
Description:
   Event handling function. Is called from interrupt routine.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
Return:
   none
*******************************************************************************/
IFX_void_t  TAPI_Phone_Event_GNK(TAPI_CONNECTION *pChannel)
{
   /* set the TAPI line mode to power down. The TAPI will not signal hook
      changes to the client application anymore till the mode is changed
      again */
   pChannel->TapiOpControlData.bFaulCond = IFX_TRUE;
   pChannel->TapiMiscData.nException.Bits.ground_key = IFX_TRUE;
   TAPI_WakeUp(pChannel);
   return;
}

#ifdef TAPI_GR909
/*******************************************************************************
Description:
   Event handling function. Is called from any GR909 state machine when
   measurement processing is over.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   device      - device specific signals. Set bits are ored to the existing
                 ones
Return:
   none
*******************************************************************************/
IFX_void_t  TAPI_Phone_Event_Gr909 (TAPI_CONNECTION *pChannel)
{
   pChannel->TapiMiscData.nException.Bits.gr909result = IFX_TRUE;
   TAPI_WakeUp(pChannel);
   return;
}
#endif /* TAPI_GR909 */

/*******************************************************************************
Description:
   Event handling function. Is called from interrupt routine.
Arguments:
   pChannel - handle to TAPI_CONNECTION structure
   devEvt   - device specific signals. Set bits are ored to the existing ones
Return:
   none
*******************************************************************************/
IFX_void_t  TAPI_Phone_Event_Device(TAPI_CONNECTION *pChannel, IFX_uint32_t devEvt)
{
   pChannel->TapiMiscData.nException.Bits.device = IFX_TRUE;
   pChannel->TapiMiscData.device |= devEvt;
   TAPI_WakeUp(pChannel);
   return;
}

/*******************************************************************************
Description:
   Event handling function. Is called from interrupt routine.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
Return:
   none
*******************************************************************************/
IFX_void_t TAPI_Phone_Event_Otemp(TAPI_CONNECTION *pChannel)
{
   pChannel->TapiOpControlData.nLineMode = IFX_TAPI_LINE_FEED_DISABLED;
   pChannel->TapiMiscData.nException.Bits.otemp = IFX_TRUE;
   TAPI_WakeUp(pChannel);
   return;
}

/*******************************************************************************
Description:
   Event handling function. Is called from interrupt routine.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
Return:
   none
*******************************************************************************/
IFX_void_t TAPI_Phone_Event_Line(TAPI_CONNECTION *pChannel, IFX_TAPI_LINE_STATUS_t stat)
{
   switch (stat)
   {
      case  IFX_TAPI_LINE_STATUS_FEEDLOWBATT:
         /* Event occurs when the line mode may be switched to
            TAPI_LINEFEED_NORMAL_LOW to save power */
         if (pChannel->TapiMiscData.nExceptionMask.Bits.feedLowBatt == IFX_FALSE)
         {
            pChannel->TapiMiscData.nException.Bits.feedLowBatt = IFX_TRUE;
            TAPI_WakeUp(pChannel);
         }
      break;
      default:
         break;
   }
   return;
}


/*******************************************************************************
Description:
   Event handling function. Is called from interrupt routine.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
Return:
   none
*******************************************************************************/
IFX_void_t  TAPI_Phone_Event_GNKH(TAPI_CONNECTION *pChannel)
{
   /* set the TAPI line mode to power down. The TAPI will not signal hook
      changes to the client application anymore till the mode is changed
      again */

   switch (pChannel->TapiMiscData.GndkhState)
   {
      case GNDKH_STATE_READY:
         /* start of state machine */
         TAPI_SetTime_Timer(pChannel->TapiMiscData.GndkhTimerID, GNDKH_RECHECKTIME1,
                            IFX_FALSE, IFX_TRUE);
         pChannel->TapiMiscData.GndkhState = GNDKH_STATE_POWDN;
         break;
      case GNDKH_STATE_POWDN:
         /* ground key in state power down should not be possible */
         break;
      case GNDKH_STATE_RECHECK:
         /* ground key occured second time -> raise error */
         pChannel->TapiMiscData.GndkhState = GNDKH_STATE_READY;
         pChannel->TapiMiscData.nException.Bits.ground_key_high = IFX_TRUE;
         pChannel->TapiOpControlData.bFaulCond = IFX_TRUE;
         TAPI_WakeUp(pChannel);
         break;
      default:
         break;
   }

   return;
}

/*******************************************************************************
Description:
   Event handling function. Is called from interrupt routine.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
Return:
   none
*******************************************************************************/
IFX_void_t  TAPI_Phone_Event_GNKP(TAPI_CONNECTION *pChannel)
{
   pChannel->TapiMiscData.nException.Bits.ground_key_polarity = IFX_TRUE;
   TAPI_WakeUp(pChannel);
   return;
}

#ifdef TAPI_CID
/*******************************************************************************
Description:
   Event handling function. Is called from interrupt routine.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
Return:
   none
*******************************************************************************/
IFX_void_t  TAPI_Phone_Event_CidRx(TAPI_CONNECTION *pChannel)
{
   pChannel->TapiMiscData.nException.Bits.cidrx_supdate = IFX_TRUE;
   TAPI_WakeUp(pChannel);
   return;
}

#endif /* TAPI_CID */

#ifdef TAPI_FAX_T38
/*******************************************************************************
Description:
   Fax Status update Event handling function. Is called from interrupt routine
   or elsewhere a fax related error occurs.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   status      - Fax status
   error       - Fax error
Return:
   none
*******************************************************************************/
IFX_void_t  TAPI_FaxT38_Event_Update (TAPI_CONNECTION *pChannel,
                                      IFX_uint8_t status, IFX_uint8_t error)
{
   pChannel->TapiFaxStatus.nStatus |= status;
   pChannel->TapiFaxStatus.nError   = error;
   pChannel->TapiMiscData.nException.Bits.fax_supdate = IFX_TRUE;
   TAPI_WakeUp(pChannel);
   return;
}
#endif /* TAPI_FAX_T38 */

/*******************************************************************************
Description:
   Fax Status update Event handling function. Is called from interrupt routine
   or elsewhere a fax related error occurs.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
Return:
   none
*******************************************************************************/
IFX_void_t TAPI_Signal_Event_Update (TAPI_CONNECTION *pChannel, IFX_uint32_t signal)
{
   /* remove unwanted signals */
   signal &= (pChannel->TapiMiscData.sigMask);
   if (signal != 0)
   {
      pChannel->TapiMiscData.signal |= signal;
      pChannel->TapiMiscData.nException.Bits.signal = 1;
      TAPI_WakeUp(pChannel);
   }
   return;
}

/*******************************************************************************
Description:
   Signal update Event handling function. Is called from interrupt routine.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   event       - event number played out
Return:
   none
*******************************************************************************/
IFX_void_t TAPI_Rfc2833_Event (TAPI_CONNECTION *pChannel, IFX_uint32_t event)
{
   if (pChannel->TapiMiscData.nExceptionMask.Bits.eventDetect == 0)
   {
      pChannel->TapiMiscData.event = event;
      pChannel->TapiMiscData.nException.Bits.eventDetect = 1;
      TAPI_WakeUp(pChannel);
   }
   return;
}

/*******************************************************************************
Description:
   Runtime errors update.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   error       - error according to IFX_TAPI_RUNTIME_ERROR_t enum.
Return:
   none
*******************************************************************************/
IFX_void_t TAPI_Error_Event_Update (TAPI_CONNECTION *pChannel, IFX_uint32_t error)
{
   /* update errors */
   if (error != IFX_TAPI_RT_ERROR_NONE)
   {
      pChannel->TapiMiscData.error |= error;
      pChannel->TapiMiscData.nException.Bits.runtime_error = 1;
      TAPI_WakeUp(pChannel);
   }

   return;
}

/*******************************************************************************
Description:
   Ground key high recheck timer entry.
Arguments:
   Timer - timer ID
   nArg    - handle to TAPI_CONNECTION structure
Return:
   none
Remarks:
   To ignore invalid ground key high interrupts the line is switched off on
   the first GNDKH and this timer is started. After the first expiry the
   line is switched on again and checked again for GNDKH. If the GNDKH
   is signaled again it is handled as an fault.
*******************************************************************************/
IFX_void_t TAPI_Gndkh_OnTimer(Timer_ID Timer, IFX_int32_t nArg)
{
   TAPI_CONNECTION *pChannel = (TAPI_CONNECTION *) nArg;

   switch (pChannel->TapiMiscData.GndkhState)
   {
      case GNDKH_STATE_READY:
         /* can not occur */
         break;
      case GNDKH_STATE_POWDN:
         /* recheck the line with origin state */
         TAPI_LL_Restore_FaultLine (pChannel);
         pChannel->TapiMiscData.GndkhState = GNDKH_STATE_RECHECK;
         TAPI_SetTime_Timer(pChannel->TapiMiscData.GndkhTimerID, GNDKH_RECHECKTIME2,
                            IFX_FALSE, IFX_TRUE);
         break;
      case GNDKH_STATE_RECHECK:
         /* no further ground key occured within time thus no error */
         pChannel->TapiMiscData.GndkhState = GNDKH_STATE_READY;
         pChannel->TapiOpControlData.bFaulCond = IFX_FALSE;
         TAPI_Stop_Timer(pChannel->TapiMiscData.GndkhTimerID);
         break;
      default:
         break;
   }
}

