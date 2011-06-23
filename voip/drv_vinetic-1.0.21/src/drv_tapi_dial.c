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
   Module      : drv_tapi_dial.c
   Desription  : Contains TAPI Dial Services.
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */

#include "drv_tapi.h"

/* ============================= */
/* Global function definition    */
/* ============================= */

/*******************************************************************************
Description:
   Function called from the dial timer.
   The timer is used to verify the duration of various on- and offhook events:
     - Offhook long enough to be confirmed (not just line noise)
     - Offhook too long to be high pulse
     - Offhook long enough to indicate pulse digit completion (interdigit timer)
     - Onhook long enough to be confirmed (not just line noise)
     - Onhook too long to be a digit pulse (it may be flash or final onhook)
     - Onhook long enough to be a flash
     - Onhook too long to be a flash (it may be final onhook)
     - Onhook long enough to be a final onhook

Arguments:
   Timer - TimerID of timer that exipres
   nArg  - Argument of timer including the TAPI_CONNECTION structure
           (as integer pointer)
Return:
*******************************************************************************/
IFX_void_t TAPI_Phone_Dial_OnTimer(Timer_ID Timer, IFX_int32_t nArg)
{
   TAPI_CONNECTION *pChannel = (TAPI_CONNECTION *) nArg;
   IFX_uint8_t     nPulseDigit;
   IFX_uint8_t     *pFifo;

   switch (pChannel->TapiDialData.nHookState)
   {
      case TAPI_HOOK_STATE_OFFHOOK:
         /* Interdigit timer expiry indicates a complete pulse digit has
            been collected */
         nPulseDigit = pChannel->TapiDialData.nHookChanges;
         if (nPulseDigit == 10)
            nPulseDigit = 0xB;  /* digit 0 is represented by 0xB */

         /* write pulse dial digit in fifo and modify exception byte */
         pFifo = Fifo_writeElement (&(pChannel->TapiDialData.TapiPulseFifo));
         if (pFifo != NULL)
         {
            *pFifo = nPulseDigit;
         }
         pChannel->TapiMiscData.nException.Bits.pulse_digit_ready = IFX_TRUE;
         /* reset the pulse counter, ready for next digit */
         pChannel->TapiDialData.nHookChanges = 0;
         TAPI_WakeUp(pChannel);
         /* NB: the FSM remains in the same state, OFFHOOK */
         break;

      case TAPI_HOOK_STATE_PULSE_H_CONFIRM:
         /* digit_h_max expiry indicates offhook has lasted too long to be high pulse */
         TAPI_SetTime_Timer(pChannel->TapiDialData.DialTimerID,
                            pChannel->TapiInterDigitTime.nMinTime - pChannel->TapiDigitHighTime.nMaxTime,
                            IFX_FALSE,
                            IFX_FALSE);
         pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_OFFHOOK;
         break;

      case TAPI_HOOK_STATE_OFFHOOK_VAL:
         /* Timer indicates offhook has lasted long enough to be validated */
         pChannel->TapiMiscData.nException.Bits.hookstate = IFX_TRUE; /* IFX_TRUE => hook event */
         pChannel->TapiOpControlData.bHookState = IFX_TRUE; /* IFX_TRUE => nature of hook event is OFFhook */
         pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_OFFHOOK;
         /* stop blockig ring */
         IFXOS_WakeUpEvent(pChannel->TapiRingEvent);
         TAPI_WakeUp(pChannel);
         break;

      case TAPI_HOOK_STATE_PULSE_H_VAL:
         /* digit_h_min timer indicates offhook has lasted long enough to be validated */
         TAPI_SetTime_Timer(pChannel->TapiDialData.DialTimerID,
                            pChannel->TapiDigitHighTime.nMaxTime - pChannel->TapiDigitHighTime.nMinTime,
                            IFX_FALSE,
                            IFX_FALSE);
         /* A pulse has been detected (not yet a complete pulse digit) */
         pChannel->TapiDialData.nHookChanges++;
         pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_PULSE_H_CONFIRM;
         break;

      case TAPI_HOOK_STATE_PULSE_L_CONFIRM:
         /* Timer digit_l_max expires: onhook has lasted too long to be a pulse */
         /* reset the pulse counter: pulse digit collection, if in progress, has been aborted */
         pChannel->TapiDialData.nHookChanges = 0;
         if (pChannel->TapiHookFlashTime.nMinTime > pChannel->TapiDigitLowTime.nMaxTime)
         {
            TAPI_SetTime_Timer(pChannel->TapiDialData.DialTimerID,
                               pChannel->TapiHookFlashTime.nMinTime - pChannel->TapiDigitLowTime.nMaxTime,
                               IFX_FALSE,
                               IFX_FALSE);
            pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_FLASH_WAIT;
         }
         else
         {
            /* If TapiHookFlashTime.nMinTime <= TapiDigitLowTime.nMaxTime,
             * skip over state TAPI_HOOK_STATE_FLASH_WAIT.
             */
            TAPI_SetTime_Timer(pChannel->TapiDialData.DialTimerID,
                               pChannel->TapiHookFlashTime.nMaxTime - pChannel->TapiDigitLowTime.nMaxTime,
                               IFX_FALSE,
                               IFX_FALSE);
            pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_FLASH_CONFIRM;
         }
         break;

      case TAPI_HOOK_STATE_FLASH_WAIT:
         /* Timer flash_min expires: the onhook has lasted long enough to be a flash */
         TAPI_SetTime_Timer(pChannel->TapiDialData.DialTimerID,
                            pChannel->TapiHookFlashTime.nMaxTime - pChannel->TapiHookFlashTime.nMinTime,
                            IFX_FALSE,
                            IFX_FALSE);
         pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_FLASH_CONFIRM;
         break;

      case TAPI_HOOK_STATE_FLASH_CONFIRM:
         /* Timer flash_max expires: onhook has lasted too long to be a flash */
         if (pChannel->TapiHookOnTime.nMinTime > pChannel->TapiHookFlashTime.nMaxTime)
         {
            TAPI_SetTime_Timer(pChannel->TapiDialData.DialTimerID,
                               pChannel->TapiHookOnTime.nMinTime - pChannel->TapiHookFlashTime.nMaxTime,
                               IFX_FALSE,
                               IFX_FALSE);
            pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_ONHOOK_CONFIRM;
         }
         else
         {
            /* If TapiHookOnTime <= TapiHookFlashTime.nMaxTime,
             * skip over state TAPI_HOOK_STATE_ONHOOK_CONFIRM: offhook is already confirmed.
             */
            pChannel->TapiMiscData.nException.Bits.hookstate = IFX_TRUE; /* IFX_TRUE => hook event */
            pChannel->TapiOpControlData.bHookState = IFX_FALSE; /* IFX_FALSE => nature of hook event is ONhook */
            pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_ONHOOK;
            TAPI_WakeUp(pChannel);
         }
         break;

      case TAPI_HOOK_STATE_ONHOOK_CONFIRM:
         /* Timer onhook expires: onhook has lasted long enough to be final */
         pChannel->TapiMiscData.nException.Bits.hookstate = IFX_TRUE; /* IFX_TRUE => hook event */
         pChannel->TapiOpControlData.bHookState = IFX_FALSE; /* IFX_FALSE => nature of hook event is ONhook */
         pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_ONHOOK;
         TAPI_WakeUp(pChannel);
         break;

      case TAPI_HOOK_STATE_ONHOOK_VAL:
         /* digit_l_min expires: onhook has lasted long enough to be a low pulse (not noise) */
         /* reset the pulse counter, ready for next digit */
         pChannel->TapiDialData.nHookChanges = 0;
         /* NOTE: the "break" statement has been intentionally omitted */
         /*lint -fallthrough */
      case TAPI_HOOK_STATE_DIAL_L_VAL:
         /* digit_l_min expires: onhook has lasted long enough to be a low pulse (not noise) */
         TAPI_SetTime_Timer(pChannel->TapiDialData.DialTimerID,
                            pChannel->TapiDigitLowTime.nMaxTime - pChannel->TapiDigitLowTime.nMinTime,
                            IFX_FALSE,
                            IFX_FALSE);
         pChannel->TapiDialData.nHookState = TAPI_HOOK_STATE_PULSE_L_CONFIRM;

         break;

      default:
         /* all other states represent error cases */
         break;
   }

   return;
}

/*******************************************************************************
Description:
   Reads the dial pulse digit out of the receive buffer.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pPulseDigit     - dial pulse digit
Return:
   IFX_SUCCESS
*******************************************************************************/
IFX_int32_t TAPI_Phone_Get_Pulse (TAPI_CONNECTION *pChannel, IFX_int32_t *pPulseDigit)
{
   IFX_uint8_t *pFifo;

   /* read pulse dial digit from fifo */
   pFifo = Fifo_readElement(&(pChannel->TapiDialData.TapiPulseFifo));
   if (pFifo != NULL)
      *pPulseDigit = *pFifo;
   else
      *pPulseDigit = 0;

   /* clear "puls digit" exception if fifo empty */
   if (Fifo_isEmpty(&(pChannel->TapiDialData.TapiPulseFifo)))
   {
      pChannel->TapiMiscData.nException.Bits.pulse_digit_ready = IFX_FALSE;
   }

   return IFX_SUCCESS;
}


/*******************************************************************************
Description:
   Reads the ASCII character representing the dial pulse digit
   out of the receive buffer.
Arguments:
   pChannel         - handle to TAPI_CONNECTION structure
   pPulseDigit     - dial pulse digit in ASCII form
Return:
   IFX_SUCCESS
*******************************************************************************/
IFX_int32_t TAPI_Phone_Get_Pulse_ASCII (TAPI_CONNECTION *pChannel,
                                        IFX_int32_t *pPulseDigit)
{
   TAPI_Phone_Get_Pulse(pChannel, pPulseDigit);

   if (*pPulseDigit != 0)
   {
      if (*pPulseDigit  == 0x0B)
      {
         *pPulseDigit = 0x30;
      }
      else
      {
         *pPulseDigit = *pPulseDigit | 0x30;
      }
   }

   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   Checks if a dial pulse digit was received.
Arguments:
   pChannel         - handle to TAPI_CONNECTION structure
   pPulseReady      - Pulse status :
                      IFX_TRUE  -> dial pulse digit was received
                      IFX_FALSE -> no dial pulse digit was received
Return:
   IFX_SUCCESS
*******************************************************************************/
IFX_int32_t TAPI_Phone_Pulse_Ready (TAPI_CONNECTION *pChannel,
                                    IFX_int32_t *pPulseReady)
{
   if (Fifo_isEmpty(&(pChannel->TapiDialData.TapiPulseFifo)))
   {
      /* no dial pulse digit was received */
      *pPulseReady = IFX_FALSE;
   }
   else
   {
      /* dial pulse digit was received */
      *pPulseReady = IFX_TRUE;
   }

   return IFX_SUCCESS;
}

#ifdef TAPI_DTMF
/*******************************************************************************
Description:
   Reads the DTMF digit out of the receive buffer.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pDtmfDigit      - dial DTMF digit
Return:
   IFX_SUCCESS / IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_Phone_Get_DTMF (TAPI_CONNECTION *pChannel,
                                 IFX_int32_t *pDtmfDigit)
{
   IFX_uint8_t    *pFifo;

   /* read DTMF digit from fifo */
   pFifo = Fifo_readElement(&(pChannel->TapiDialData.TapiDTMFFifo));
   if (pFifo != NULL)
   {
      *pDtmfDigit = *pFifo;
   }
   else
   {
      *pDtmfDigit = -1;
      TRACE (TAPI_DRV,DBG_LEVEL_NORMAL, ("No digits available\n\r"));
      return IFX_ERROR;
   }
   /* clear "DTMF digit" exception if fifo empty */
   if (Fifo_isEmpty(&(pChannel->TapiDialData.TapiDTMFFifo)))
   {
      pChannel->TapiMiscData.nException.Bits.dtmf_ready = IFX_FALSE;
   }

   return IFX_SUCCESS;
}


/*******************************************************************************
Description:
   Reads the ASCII character representing the dial pulse digit
   out of the receive buffer.
Arguments:
   pChannel         - handle to TAPI_CONNECTION structure
   pDtmfAscii        - dial DTMF digit in ASCII form
Return:
   IFX_SUCCESS
*******************************************************************************/
IFX_int32_t TAPI_Phone_Get_DTMF_ASCII (TAPI_CONNECTION *pChannel,
                                       IFX_int32_t *pDtmfAscii)
{
   TAPI_Phone_Get_DTMF(pChannel, pDtmfAscii);
   switch ( *pDtmfAscii )
   {
      case 0x00: break; /* return 0 as error code */
      case 0x0A: *pDtmfAscii = '*'; break;
      case 0x0B: *pDtmfAscii = '0'; break;
      case 0x0C: *pDtmfAscii = '#'; break;
      case 0x1C: *pDtmfAscii = 'A'; break;
      case 0x1D: *pDtmfAscii = 'B'; break;
      case 0x1E: *pDtmfAscii = 'C'; break;
      case 0x1F: *pDtmfAscii = 'D'; break;
      default:
         *pDtmfAscii += '0';  /* '1'--'9' */
         break;
   };

   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   Checks if a dtmf digit was received.
Arguments:
   pChannel         - handle to TAPI_CONNECTION structure
   pDtmfReady       - status of the fifo: IFX_TRUE  -> dtmf digit was received
                                   IFX_FALSE -> no dtmf digit was received
Return:
   IFX_SUCCESS
*******************************************************************************/
IFX_int32_t TAPI_Phone_DTMF_Ready (TAPI_CONNECTION *pChannel,
                                   IFX_int32_t *pDtmfReady)
{

   if (Fifo_isEmpty(&(pChannel->TapiDialData.TapiDTMFFifo)))
   {
      /* no dial pulse digit was received */
      *pDtmfReady = IFX_FALSE;
   }
   else
   {
      /* dial pulse digit was received */
      *pDtmfReady = IFX_TRUE;
   }

   return IFX_SUCCESS;
}
#endif /* TAPI_DTMF */

/*******************************************************************************
Description:
   Sets the validation timers for hook, pulse digit and hook flash during
   system startup.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pTime           - type of validation setting, min and max time in milliseconds
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   For timers that do not distinguish between a min and max value, both
   min and max values are set equal.
*******************************************************************************/
IFX_int32_t TAPI_Phone_Validation_Time(TAPI_CONNECTION *pChannel,
                                       TAPI_VALIDATION_TIMES const *pTime)
{

   /* zero timer values are not allowed */
   if (pTime->nMinTime == 0 || pTime->nMaxTime == 0)
   {
      TRACE(TAPI_DRV,DBG_LEVEL_HIGH,
            ("DRV_ERROR: zero validation timer values not allowed for channel %d\n",
                                     pChannel->nChannel));
      return IFX_ERROR;
   }

   /* check whether min timer value > max timer value */
   if (pTime->nMinTime > pTime->nMaxTime)
   {
      TRACE(TAPI_DRV,DBG_LEVEL_HIGH,
           ("DRV_ERROR: min value %ld > max value %ld for channel %d\n",
                                     pTime->nMinTime,pTime->nMaxTime,pChannel->nChannel));
      return IFX_ERROR;
   }

   /* configure the validation timers at system startup */
   switch (pTime->nType)
   {
      case HOOKOFF_TIME:
         pChannel->TapiHookOffTime.nType    = pTime->nType;
         pChannel->TapiHookOffTime.nMinTime = pTime->nMinTime;
         pChannel->TapiHookOffTime.nMaxTime = pTime->nMaxTime;
         break;

      case HOOKON_TIME:
         pChannel->TapiHookOnTime.nType    = pTime->nType;
         pChannel->TapiHookOnTime.nMinTime = pTime->nMinTime;
         pChannel->TapiHookOnTime.nMaxTime = pTime->nMaxTime;
         break;

      case HOOKFLASH_TIME:
         pChannel->TapiHookFlashTime.nType    = pTime->nType;
         pChannel->TapiHookFlashTime.nMinTime = pTime->nMinTime;
         pChannel->TapiHookFlashTime.nMaxTime = pTime->nMaxTime;
         break;

      case DIGITLOW_TIME:
         pChannel->TapiDigitLowTime.nType    = pTime->nType;
         pChannel->TapiDigitLowTime.nMinTime = pTime->nMinTime;
         pChannel->TapiDigitLowTime.nMaxTime = pTime->nMaxTime;
         break;

      case DIGITHIGH_TIME:
         pChannel->TapiDigitHighTime.nType    = pTime->nType;
         pChannel->TapiDigitHighTime.nMinTime = pTime->nMinTime;
         pChannel->TapiDigitHighTime.nMaxTime = pTime->nMaxTime;
         break;

      case INTERDIGIT_TIME:
         pChannel->TapiInterDigitTime.nType    = pTime->nType;
         pChannel->TapiInterDigitTime.nMinTime = pTime->nMinTime;
         pChannel->TapiInterDigitTime.nMaxTime = pTime->nMaxTime;
         break;

      default:
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,
               ("DRV_ERROR: unknown validation type 0x%x\n",pTime->nType));
         return IFX_ERROR;
      }

   return IFX_SUCCESS;
}
