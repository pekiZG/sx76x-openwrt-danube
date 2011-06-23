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
   Module      : drv_vinetic_tone.c
   Date        : 2005-03-30
 \file This file contains the implementation  of the functions
       related to tone operations for TAPI
   \remarks
   The tone implementation uses the TAPI as interface. The VINETIC supports
   tone generation via a firmware signaling module called UTG and additionally
   vi an analog tone generator (TG, actually two). Most of the services are
   using the UTG, because is reduces the host utilization and gains more
   features. The TG is only used in very rare situation, when the UTG is not
   available. This is the case, when the coder is already connected to another
   analog part. The TG then can generate a busy tone.
   The resources are handled by the low level part of the driver. Thus when TAPI
   requests a resource, that is not available, the low level returns an error.
   Timer and state handling is also implemented in the low level part.
   The TAPI provides a maximum set of resources, that may not be available.
*/
/** \todo Provide capability for tone resources */

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_vinetic_api.h"
#include "drv_vinetic_tapi.h"
#include "drv_vinetic_tone.h"
#include "drv_tapi.h"
#include "drv_vinetic_basic.h"
#ifndef VIN_2CPE
/* for ALM Tone Generators support, not available in 2CPE */
#include "drv_vinetic_cram.h"
#endif


/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

/* ============================= */
/* Defines for UTG stop command   */
/* ============================= */

/* ============================= */
/* Global variable definition    */
/* ============================= */

/* ============================= */
/* Global function declaration   */
/* ============================= */

/* ============================= */
/* Local function declaration    */
/* ============================= */

#ifdef TAPI
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
IFX_LOCAL IFX_int16_t utg_calc_level (IFX_int32_t level);
IFX_LOCAL IFX_int32_t Utg_SetCoeff (IFX_TAPI_TONE_SIMPLE_t const *pTone, IFX_uint16_t *pCmd);
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
#ifndef VIN_2CPE
IFX_LOCAL TAPI_CMPLX_TONE_STATE TG_ToneStep (VINETIC_CHANNEL* pCh,
                                             IFX_TAPI_TONE_SIMPLE_t const *pTone,
                                             IFX_uint8_t res);
#endif /* VIN_2CPE*/

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* ============================= */
/* Local function definition     */
/* ============================= */

#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
/**
   function to calculate the utg level coefficient

   \param level - level in [0.1 dB]
   \return
      utg level coefficient
   \remarks
      none
*/
IFX_LOCAL IFX_int16_t utg_calc_level (IFX_int32_t level)
{
   long lvl;
   IFX_uint32_t  exp, tenExp;
   IFX_uint32_t  shift;
   IFX_int32_t   i;

   if (level == 0)
      return 0xFF;
   /* calclulate the desired level in mdB and the exponent */
   lvl = level * 100;
   exp = (IFX_uint32_t)((-lvl) / 20);

   /* set the initial shift factor according to the level */
   if (lvl <= -20000)
      shift  = 1000;
   else if (lvl <= -10000)
      shift  = 10000;
   else
      shift  = 10000;

   /* the initial value of the result (tenExp) is the shift factor
      which will be compensated in the last step (calculating the
      coefficient)
      return ((xxxxx * shift) / tenExp); */
   tenExp = shift;

   /* go over allelements in the tens array, starting with the
      largest entry and ... */
   for (i=27;i>=0;i--)
   {
      /* ... loop while the current tens[i][0] is part of the remaining exp */
      while (exp >= tens[i][0])
      {

         /* calulate part of the result for tens[i][0], check which accuracy
            of the tens[i][1] can be used to multiply tenExp without overflow */
         if ((C2_31 / tenExp) > tens[i][1])
         {
            /* use the unscaled tens[i][1] value to calculate
               the tenExp share */
            tenExp *= tens[i][1];
            tenExp /= 100000;
         }
         else if ( (C2_31 / tenExp) > ROUND_DIV10(tens[i][1]))
         {
            /* scale the tens[i][1] value by 10
               to calculate the tenExp share */
            tenExp *= ROUND_DIV10(tens[i][1]);
            tenExp /= 10000;
         }
         else if ( (C2_31 / tenExp) > ROUND_DIV100(tens[i][1]))
         {
            /* scale the tens[i][1] value by 100
               to calculate the tenExp share */
            tenExp *= ROUND_DIV100(tens[i][1]);
            tenExp /= 1000;
         }
         else
         {
            /* scale the tens[i][1] value by 1000
               to calculate the tenExp share */
            tenExp *= ROUND_DIV1000(tens[i][1]);
            tenExp /= 100;
         }

         /* calculate the remaining exp, i.e. subtract that part of exponent
            that has been calculated in this step */
         exp -= tens[i][0];
      }
   }

   /* calculate the coefficient according to the specification... */
   return (IFX_int16_t) ROUND (((C2_8*shift)/(tenExp)));
}

/**
   Set the coefficients for the UTG
\param
   pTone - internal simple tone table entry
\param
   pCmd  - command to be filled, it is not cleared in this function.
\return
   IFX_SUCCESS
\remark
   A memset must be done on pCmd and the command words have to be set
   by the calling function
*/
IFX_LOCAL IFX_int32_t Utg_SetCoeff (IFX_TAPI_TONE_SIMPLE_t const *pTone, IFX_uint16_t *pCmd)
{
   IFX_uint8_t step = 12, i;

   pCmd[6] = 0x008D; // modulation factor of 90 % 

   /* set frequency A for F_1 */
   pCmd [7] = (IFX_uint16_t)((8192 * pTone->freqA) / 1000);
   /* set frequency B  for F_2 */
   pCmd [8] = (IFX_uint16_t)((8192 * pTone->freqB) / 1000);
   /* set frequency C for F_3 */
   pCmd [9] = (IFX_uint16_t)((8192 * pTone->freqC) / 1000);
   /* set frequency C for F_3 */
   pCmd [10] = (IFX_uint16_t)((8192 * pTone->freqD) / 1000);

   /* set power level for LEV_1, LEV_2 and LEV_3 */
   pCmd [11] = ((IFX_uint8_t)utg_calc_level(pTone->levelA) << 8) |
                (IFX_uint8_t)utg_calc_level(pTone->levelB);
   pCmd [12] = ((IFX_uint8_t)utg_calc_level(pTone->levelC) << 8) |
                (IFX_uint8_t)utg_calc_level(pTone->levelD);

   /* set step times:  T_x */
   for (i = 0; i < IFX_TAPI_TONE_STEPS_MAX; ++i)
   {
      /* check for last cadence step */
      if (pTone->cadence[i] == 0)
      {
         /* special case: continous tone for the one and only cadence step.
            In that case a value FFFF must be programmed to the firmware */
         if (pTone->pause == 0 && pTone->loop == 0 && i == 1)
         {
            pCmd [step - 1] = 0xFFFF;
         }
         break;
      }
      /* increase step to timing. Step is initialized with 12 (one before
         the first timing setting */
      step++;
      /* the firmware uses a timebase of 0.5 ms */
      pCmd [step] = (IFX_uint16_t)(2 * pTone->cadence[i]);
      /* move step to step setting */
      step++;
      /* set mask for MSK_i - use frequency A. Initialize the field */
      if (pTone->frequencies[i] & IFX_TAPI_TONE_FREQA)
         pCmd [step] = ECMD_UTG_COEF_MSK_F1;
      else
         pCmd [step] = 0;
      /* set mask for MSK_i - use frequency B  */
      if (pTone->frequencies[i] & IFX_TAPI_TONE_FREQB)
         pCmd [step] |= ECMD_UTG_COEF_MSK_F2;
      /* set mask for MSK_i - use frequency C  */
      if (pTone->frequencies[i] & IFX_TAPI_TONE_FREQC)
         pCmd [step] |= ECMD_UTG_COEF_MSK_F3;
      /* set mask for MSK_i - use frequency D  */
      if (pTone->frequencies[i] & IFX_TAPI_TONE_FREQD)
         pCmd [step] |= ECMD_UTG_COEF_MSK_F4;
      if (pTone->modulation[i] & IFX_TAPI_TONE_MODULATION_ON)
         pCmd [step] |= ECMD_UTG_COEF_MSK_M12;
      /* set the next step */
      pCmd[step] |= (IFX_uint16_t)((i+1) << 12);
   }

   /* clear the next step to start at 0 */
   pCmd [step] &= ~ECMD_UTG_COEF_MSK_NXT;
   if (pTone->pause == 0 && pTone->loop)
   {
      /* The deactivation of the tone
         generator is delayed until the current tone generation step
         has been completely executed inclusive of the requested
         repetitions */
      pCmd [step] |= (IFX_uint16_t)((pTone->loop - 1) << 9) | ECMD_UTG_COEF_SA_01;
   }
   else
   {
      /* The deactivation of the tone
         generator is delayed until the current tone generation step
         has been completely executed. The deactivation is made
         immediately after the execution of the current tone
         generation step. */
      pCmd [step] |= ECMD_UTG_COEF_SA_10;
   }
   /* Always set the GO_1 and GO_2 coefficients to 0 dB (0x8000), i.e.
      tone level is defined only via the level settings above.
      If multiple tones are played out simultaneously the application has
      to adapt the levels to prevent clipping. */
   pCmd [25] = 0x8000;
   pCmd [26] = 0x8000;
   return IFX_SUCCESS;
}
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */


/**
   Function called from the teardown voice path timer.
   The timer expiry indicates the following:
     - Stop the voice path recording(teardown) in order for the next
       tone playing sequence step to be executed on that channel
\param
   Timer  - TimerID of timer that exipres
\param
   nArg   - Argument of timer including the VINETIC_CHANNEL structure
           (as integer pointer)
\return
*/
IFX_void_t Tone_OnTimer(Timer_ID Timer, IFX_int32_t nArg)
{
   VINETIC_TONERES *pRes = (VINETIC_TONERES*)nArg;
   VINETIC_CHANNEL* pCh = pRes->pCh;
   TAPI_DEV* pTapiDev = &pCh->pParent->TapiDev;
   TAPI_CONNECTION *pChannel = (TAPI_CONNECTION *) &(pTapiDev->pChannelCon[pCh->nChannel - 1]);
   IFX_uint32_t toneCode           = 0;
   IFX_TAPI_TONE_SIMPLE_t *pToneSimple;
   TAPI_TONE_DATA* pData;

   /* in this function resource number 0 is used. Resource 1 is not allowed to use
      this time -> This is still a problem: nArg must specify which tone resource */
   pData = &pChannel->TapiComplexToneData[pRes->nRes];

   /* check if mailbox supports all commands, otherwise return immediately */
   if (pData->src == IFX_TAPI_TONE_SRC_DSP)
   {
      IFX_uint8_t nCmdMbxSize;
      VINETIC_DEVICE* pDev = pCh->pParent;

      if (VINETIC_Host_GetCmdMbxSize (pDev, &nCmdMbxSize) == IFX_SUCCESS)
      {
         if (nCmdMbxSize < MAX_CMD_WORD)
         {
            TAPI_SetTime_Timer(pCh->pToneRes[pRes->nRes].Tone_Timer, 1, IFX_FALSE, IFX_FALSE);
            return;
         }
      }
      else
      {
         /** \todo raise runtime error */
         return;
      }
   }

   /* Get the simple tone code */
   if (pData->nType == TAPI_TONE_TYPE_SIMPLE)
   {
      toneCode = pData->nSimpleToneCode;
   }
   else if (pData->nType == TAPI_TONE_TYPE_COMP)
   {
      /* Get the simple tone to play within the composed tone */
      IFX_TAPI_TONE_COMPOSED_t *pComposedTone;

      pComposedTone = &pTapiDev->pToneTbl[pData->nComposedToneCode].
                      tone.composed;
      toneCode = pComposedTone->tones[pData->nToneCounter];
   }

   /* Any errors on the simple tone code index is taken care of here */
   if (toneCode >= TAPI_MAX_TONE_CODE || toneCode == 0)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH,
            ("DRV_ERROR: Repeat complex tone code out of range\n\r"));
      return;
   }

   /* Retrieve simple tone configuration from internal tone coefficients table */
   pToneSimple = &(pTapiDev->pToneTbl[toneCode].tone.simple);
   pData->nSimpleMaxReps      = pToneSimple->loop;
   pData->nPauseTime          = pToneSimple->pause;
   pData->nSimpleToneCode     = pToneSimple->index;
#ifndef VIN_2CPE
   if (pData->src == IFX_TAPI_TONE_SRC_TG)
   {
      /* tone has finished */
      pData->nToneState = TG_ToneStep (pCh, pToneSimple, pRes->nRes);
   }
   else
#endif /*VIN_2CPE*/
   {
      /* Activate the universal tone generator with simple tone sequence */
      Tone_UTG_Start(pChannel, pToneSimple, pData->dst, pRes->nRes);
   }

   return;
}

/**
   Simple tone deactivated event handling function.
\param
   pChannel     - handle to TAPI_CONNECTION structure
   utgNum       - tone resource 0 or 1
\remark
   This function is called from interrupt level. Is handles
   tone resource 0 or 1
\return
   none
*/
IFX_LOCAL IFX_void_t Event_SimpleTone(TAPI_CONNECTION *pChannel, IFX_uint8_t utgNum )
{
   VINETIC_DEVICE  *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   VINETIC_CHANNEL *pCh  = &pDev->pChannel[pChannel->nChannel];
   TAPI_TONE_DATA* pData;

   /* get context for this resource */
   pData = &pChannel->TapiComplexToneData[utgNum];

   /* One simple tone sequence has been applied on this channel */
   pData->nSimpleCurrReps++;

   /* Special case: A zero pause time was specified on this channel,
     therefore ignore the repetition count.
     The simple tone sequence must be applied only once.    */
   if (pData->nPauseTime == 0)
   {
      /* Set tone generation back to initial state */
      pData->nToneState = TAPI_CT_IDLE;
   }
   /* An infinite tone sequence repetition was requested
      on this channel or there are still simple tone sequences to repeat.
      Repeat tone sequence again after the voice path timer expires. */
   else if ((pData->nSimpleMaxReps == 0) ||
            (pData->nSimpleCurrReps < pData->nSimpleMaxReps)
           )
   {
      /* Start the voice path teardowm timer */
      TAPI_SetTime_Timer(pCh->pToneRes[utgNum].Tone_Timer,
                         pData->nPauseTime,
                         IFX_FALSE, IFX_FALSE);
   }
   /* This was the last simple tone sequence to be played */
   else
   {
      /* Set tone generation back to initial state */
      pData->nToneState = TAPI_CT_IDLE;
      /* Stop the voice path teardowm timer */
      TAPI_Stop_Timer(pCh->pToneRes[utgNum].Tone_Timer);
   }
}

/**
   Composed tone deactivated event handling function.
\param
   pChannel     - handle to TAPI_CONNECTION structure
\return
   none
*/
IFX_LOCAL IFX_void_t Event_ComposedTone(TAPI_CONNECTION *pChannel, IFX_uint8_t utgNum)
{
   VINETIC_DEVICE  *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   VINETIC_CHANNEL *pCh  = &pDev->pChannel[pChannel->nChannel];
   TAPI_TONE_DATA* pData;

   pData = &pChannel->TapiComplexToneData[utgNum];
   /* One composed tone sequence has been applied on this channel */
   pData->nComposedCurrReps++;
   /* Special case: A zero alternate voice time was specified on this channel,
     therefore ignore the repetition count.
     The composed tone sequence must be applied only once.   */
   if (pData->nAlternateVoiceTime == 0)
   {
      /* Set tone generation back to initial state */
      pData->nToneState = TAPI_CT_IDLE;
   }
   /* An infinite tone sequence repetition was requested
      on this channel or there are still composed tone sequences to repeat.
      Repeat tone sequence again after the voice path timer expires.   */
   if ((pData->nComposedMaxReps == 0) ||
       (pData->nComposedCurrReps <= pData->nComposedMaxReps))
   {
      TAPI_SetTime_Timer(pCh->pToneRes[utgNum].Tone_Timer,
                         pData->nAlternateVoiceTime,
                         IFX_FALSE, IFX_FALSE);
   }

   /* This was the last composed tone sequence to be played */
   else
   {
      /* Set tone generation back to initial state */
      pData->nToneState = TAPI_CT_IDLE;
      /* Stop the voice path teardowm timer */
      TAPI_Stop_Timer(pCh->pToneRes[utgNum].Tone_Timer);
   }
}

#ifndef VIN_2CPE /* No tone generator on analog side for 2CPE */
/**
   Tone step of tone generator.
\param
   pCh   - handle to a VINETIC channel structure
\param
   pTone   - internal simple tone table entry
\return
   none
\remarks

*/
IFX_LOCAL TAPI_CMPLX_TONE_STATE TG_ToneStep(VINETIC_CHANNEL* pCh,
                                            IFX_TAPI_TONE_SIMPLE_t const *pTone,
                                            IFX_uint8_t res)
{
   if (pCh->nToneStep > IFX_TAPI_TONE_STEPS_MAX ||
       pTone->cadence[pCh->nToneStep] == 0)
   {
      pCh->nToneStep = 0;
      if (pTone->loop != 0)
      {
         pCh->nTone_Cnt--;
         if (pCh->nTone_Cnt == 0)
         {
            DSCR.value &= ~(DSCR_COR8 | DSCR_PTG | DSCR_TG1_EN | DSCR_TG2_EN);
            wrReg (pCh, DSCR_REG);
            return TAPI_CT_DEACTIVATED;
         }
      }
      if (pTone->loop == 0 || pTone->loop > 1)
      {
         if (pTone->pause > 0)
         {
            /* set tone timer */
            TAPI_SetTime_Timer (pCh->pToneRes[res].Tone_Timer, pTone->pause,
                                IFX_FALSE, IFX_TRUE);
            return TAPI_CT_ACTIVE;
         }
      }
   }
   DSCR.value &= ~(DSCR_COR8 | DSCR_PTG | DSCR_TG1_EN | DSCR_TG2_EN);
   if (pTone->frequencies[pCh->nToneStep] & IFX_TAPI_TONE_FREQA)
      /* data to start tone generator 1 */
      DSCR.value |= (DSCR_COR8 | DSCR_PTG | DSCR_TG1_EN);
   if (pTone->frequencies[pCh->nToneStep] & IFX_TAPI_TONE_FREQB)
      DSCR.value |= DSCR_TG2_EN;
   /* call target function to set tone generator(s) */
   wrReg (pCh, DSCR_REG);
   /* set tone timer */
   TAPI_SetTime_Timer (pCh->pToneRes[res].Tone_Timer,
                       pTone->cadence[pCh->nToneStep],
                       IFX_FALSE, IFX_TRUE);
   pCh->nToneStep++;
   return TAPI_CT_ACTIVE;
}
#endif /* VIN_2CPE */

/* ============================= */
/* Global function definition    */
/* ============================= */

#ifndef VIN_2CPE /* No tone generator on analog side for 2CPE */
/**
   Play a local tone on one or both tone generators.
\param
   pCh   - handle to a VINETIC channel structure
\param
   vers - chip revision
\param
   freqA - frequency A in mHz
\param
   freqB - frequency B in mHz, or zero if not applicable
\return
   IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t Tone_TG_SetCoeff (VINETIC_CHANNEL* pCh, IFX_uint8_t vers,
                         IFX_int32_t freqA, IFX_int32_t freqB)
{
   IFX_int32_t ret;
#ifdef VIN_V14_SUPPORT
   COEFF_TAB_ENTRY  tab_entry;
   IFX_uint16_t     pData [4] = {0};
#endif /* VIN_V14_SUPPORT */

   switch (vers)
   {
#ifdef VIN_V14_SUPPORT
      case VINETIC_V1x:
         tab_entry.quantVal = freqA;
         /* now get the coefficient from global frequency / coefficient table.
            result is returned in pEntry */
         ret = Cram_getUnitCoef (IOSET, &tab_entry, (COEFF_TAB_ENTRY *)
                                 VINETIC_CRAM_TGFreqQuantVal_Table);
         if (ret != IFX_SUCCESS)
            return IFX_ERROR;
         pData[0] = LOWWORD ((IFX_uint32_t)tab_entry.CRAMcoeff);
         pData[1] = HIGHWORD ((IFX_uint32_t)tab_entry.CRAMcoeff);
         /* c1 = tab_entry.CRAMcoeff; */
         if (freqB)
         {
            tab_entry.quantVal = (IFX_int32_t)freqB;
            ret = Cram_getUnitCoef (IOSET, &tab_entry,
                     (COEFF_TAB_ENTRY *) VINETIC_CRAM_TGFreqQuantVal_Table);
            if (ret != IFX_SUCCESS)
               return IFX_ERROR;
            pData[2] = LOWWORD  ((IFX_uint32_t)tab_entry.CRAMcoeff);
            pData[3] = HIGHWORD ((IFX_uint32_t)tab_entry.CRAMcoeff);
            /* c2 = tab_entry.CRAMcoeff; */
            ret = RegWrite (pCh->pParent, CMD1_COP | (pCh->nChannel - 1),
                            pData, 4, CRAM_PTG1);
         }
         else
         {
            ret = RegWrite (pCh->pParent, CMD1_COP | (pCh->nChannel - 1),
                            pData, 2, CRAM_PTG1);
         }
         break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
      case VINETIC_V2x:
         /* set the frequency for the first TG */
         ret = setRegVal (pCh, TG1F_REG, freqA);
         if (ret == IFX_SUCCESS)
            ret = wrReg (pCh, TG1F_REG);

         /* set the frequency for the second TG */
         if ((ret == IFX_SUCCESS) && (freqB != 0))
         {
            ret = setRegVal (pCh, TG2F_REG, freqB);
            if (ret == IFX_SUCCESS)
               ret = wrReg (pCh, TG2F_REG);
         }
         break;
#endif /* VIN_V21_SUPPORT */
      default:
         return IFX_ERROR;
   }
   if (ret == IFX_SUCCESS)
   {
      DSCR.value &= ~(DSCR_TG2_EN);
      /* data to start tone generator 1 */
      DSCR.value |= (DSCR_COR8 | DSCR_PTG | DSCR_TG1_EN);
      if (freqB)
         DSCR.value |= DSCR_TG2_EN;
      /* call target function to set tone generator(s) */
      ret = wrReg (pCh, DSCR_REG);
   }
   return ret;
}

/**
   Play a local tone on one or both tone generators.
\param
   pCh   - handle to a VINETIC channel structure
\return
   IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t Tone_TG_Play (VINETIC_CHANNEL* pCh)
{
   TAPI_DEV* pTapiDev = &pCh->pParent->TapiDev;
   VINETIC_DEVICE    *pDev     = (VINETIC_DEVICE *)pCh->pParent;
   TAPI_CONNECTION   *pChannel = &pDev->TapiDev.pChannelCon[pCh->nChannel-1];
   IFX_uint32_t toneCode;
   IFX_int32_t freqA, freqB;
   IFX_int32_t ret;
   IFX_TAPI_TONE_SIMPLE_t *pTone;
   TAPI_TONE_DATA* pData;

   /* in this function resource number 1 (TG) is used */
   pData = &pChannel->TapiComplexToneData[1];

   if (pData->nType == TAPI_TONE_TYPE_SIMPLE)
   {
      toneCode = pData->nSimpleToneCode;
      /* Retrieve simple tone configuration from internal tone coefficients table */
      pTone = &(pTapiDev->pToneTbl[toneCode].tone.simple);
   }
   else
   {
      SET_ERROR (ERR_INVALID_TONERES);
      return IFX_ERROR;
   }
   pCh->nToneStep = 0;
   pCh->nTone_Cnt = pTone->loop;
   freqA = (IFX_int32_t)pTone->freqA * 1000;
   if (pTone->frequencies[0] & IFX_TAPI_TONE_FREQB)
   {
      freqB = (IFX_int32_t)pTone->freqB * 1000;
      /* use the second tone generator */
      pChannel->TapiTgToneData.bUseSecondTG = IFX_TRUE;
   }
   else
   {
      freqB = 0;
      /* do not use the second tone generator */
      pChannel->TapiTgToneData.bUseSecondTG = IFX_FALSE;
   }
   /* program the coefficients */
   ret = Tone_TG_SetCoeff (pCh, pDev->nChipMajorRev, freqA, freqB);
   /* tone is playing now, so set the state */
   pData->nToneState = TAPI_CT_ACTIVE;

   return ret;
}

/**
   Function called from tone timer and switch the tone on/off.
\param
   Timer  - TimerID of timer that exipres
\param
   nArg   - Argument of timer including the TAPI_CONNECTION structure
           (as integer pointer)
\return
   none
\remarks
   Only for compability reasons
*/
IFX_void_t Tone_PredefOnTimer(Timer_ID Timer, IFX_int32_t nArg)
{
   VINETIC_CHANNEL *pCh = (VINETIC_CHANNEL *) nArg;
   VINETIC_DEVICE  *pDev     = (VINETIC_DEVICE *)pCh->pParent;
   TAPI_CONNECTION *pChannel = &pDev->TapiDev.pChannelCon[pCh->nChannel-1];
   IFX_uint32_t     nTime;
   IFX_uint16_t     nDscr;

   if (pChannel->TapiTgToneData.nToneState == TAPI_CT_IDLE)
   {
      /* error: tone already stopped */
      LOG(TAPI_DRV,DBG_LEVEL_HIGH,
         ("\n\rVINETIC IFX_ERROR: No Tone is played yet!\n\r"));
      return;
   }
   else if (pChannel->TapiTgToneData.nToneState == TAPI_CT_ACTIVE)
   {
      /* data to stop tone generator(s) (COR8_OFF) | (PTG_OFF) | (ALL_TG_OFF) */
      nDscr = 0;
      /* calculate off-time for timer */
      nTime = pChannel->TapiTgToneData.nOffTime / 4;
      if (nTime == 0)
      {
         nTime = pChannel->TapiTgToneData.nOnTime / 4;
         pChannel->TapiTgToneData.nToneState = TAPI_CT_ACTIVE;
      }
      else
      {
         pChannel->TapiTgToneData.nToneState = 2;
      }
   }
   else  /* pChannel->TapiTgToneData.nToneState == 2 */
   {
      /* data to start tone generator 1 */
      nDscr = (DSCR_COR8)|(DSCR_PTG)|(DSCR_TG1_EN);
      if (pChannel->TapiTgToneData.bUseSecondTG)
      {
         /* also start tone generator 2*/
         nDscr |= DSCR_TG2_EN;
      }
      /* calculate on-time for timer */
      nTime = pChannel->TapiTgToneData.nOnTime / 4;
      /* change tone state */
      pChannel->TapiTgToneData.nToneState = TAPI_CT_ACTIVE;
   }
   /* check time for smallest timer interval */
   if (nTime < 20)
   {
      nTime = 20;
   }
   if (pChannel->TapiTgToneData.nOffTime != 0)
   {
      /* call target function to set tone generator(s) */
      RegModify (pDev, CMD1_SOP|(pChannel->nChannel), SOP_OFFSET_DSCR,
                (DSCR_COR8)|(DSCR_PTG)|(DSCR_TG1_EN|DSCR_TG2_EN), nDscr);
   }
   /* set tone timer */
   TAPI_SetTime_Timer(Timer, nTime, IFX_FALSE, IFX_TRUE);
}

#endif /* VIN_2CPE */

/**
   Do low level UTG (Universal Tone Generator) configuration and activation
\param
   pChannel      - handle to TAPI_CONNECTION structure
\param
   pSimpleTone   - internal simple tone table entry
\return
   IFX_SUCCESS/IFX_ERROR
\remarks
   The selected signalling channel has to be activated before this command can
   be sent.
   For the UTG, the coefficients have to be programmed with a seperate command,
   before the UTG can be activated. The UTG should be inactive when programming
   the coefficients.
   The DMTF / AT Generator is overlaid with the tone generator, which means
   the same resources for the DMTF / AT Generator and the UTG mustn't be active
   simultaneously! DTMF /AT will be deactivated to avoid simulatenous activation.

   It is assumed that the UTG is deactive before calling this function which
   programms UTG coefficients.
   In case the UTG event interrupt is needed, the calling function should make
   sure that the DTMD/AT Generator is deactivated before.
   This function is completely protected to concurrent member access
*/
IFX_int32_t Tone_UTG_Start (TAPI_CONNECTION *pChannel,
                            IFX_TAPI_TONE_SIMPLE_t const *pSimpleTone,
                            TAPI_TONE_DST dst,
                            IFX_uint8_t utgNum )
{
   IFX_int32_t      ret = IFX_ERROR;
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE  *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   VINETIC_CHANNEL *pCh  = &pDev->pChannel[pChannel->nChannel];
   FWM_SIG_UTG* pUtgCmd = &pDev->pSigCh[pChannel->nChannel].utg[utgNum];
   IFX_uint16_t     pCmdCoef [27] = {0};
   IFX_uint8_t      utgRes = 0, ch = pCh->nChannel - 1;

   /* calling function ensures valid parameter */
   IFXOS_ASSERT(pSimpleTone != IFX_NULL);

   /* check utgNum,
   please note that resources like "utgNum" are counted from #0 */
   if (utgNum >= pDev->VinCapabl.nUtgPerCh)
   {
      SET_ERROR (ERR_NORESOURCE);
      return IFX_ERROR;
   }

   /* choose utg resource according to the FW capabilities */
   if (utgNum == 0)
   {  /* resources #0..#3 for first utg */
      utgRes = (IFX_uint8_t)ch;
      /* Setup UTG1 interrupt masks.
      Note : Mask rising edge interrupts for VIN_UTG1_ACT_MASK.
             Allow falling edge interrupts for VIN_UTG1_ACT_MASK.
      Note: VIN_UTG2_ACT_MASK is overlaid with V2CPE_EDSP1_INT1_DTMFG_ACT
            For this reason the mask settings are set here within "Tone_UTG_Start"! */
      ret = VINETIC_Host_Set_EdspIntMask (pCh, VIN_EDSP_UTG1, IFX_FALSE, 0,
                                          VIN_UTG1_ACT_MASK);
      if (ret == IFX_ERROR)
      {
         return IFX_ERROR;
      }
   }
   else if (utgNum == 1)
   {
      /* second resource, UTG2, uses different resources as UTG1 */
      utgRes = (IFX_uint8_t)(ch + pDev->nSigCnt);
      /* check whether CID Generator is active,
      because the status bits for CIS and UTG2 are overlaid
      => UTG could not be used while CIS is active! */
      if (pDev->pSigCh[ch].cid_sender[2] & SIG_CID_EN)
      {
         SET_ERROR (ERR_CID_RUNNING);
         return (IFX_ERROR);
      }
      /* Setup UTG2 interrupt masks.
      Note : Mask rising edge interrupts for VIN_UTG2_ACT_MASK.
             Allow falling edge interrupts for VIN_UTG2_ACT_MASK.
      Note: VIN_UTG2_ACT_MASK is overlaid with V2CPE_EDSP1_INT1_CIS_ACT
            For this reason the mask settings are set here within "Tone_UTG_Start"! */
      ret = VINETIC_Host_Set_EdspIntMask (pCh, VIN_EDSP_UTG2, IFX_FALSE, 0,
                                          VIN_UTG2_ACT_MASK);
      if (ret == IFX_ERROR)
      {
         return IFX_ERROR;
      }
   }
   else
   {
      SET_ERROR (ERR_NORESOURCE);
      return IFX_ERROR;
   }
   memset (pCmdCoef, 0, sizeof(pCmdCoef));
   /* prepare command for coefficients */
   pCmdCoef [0] = CMD1_EOP | (utgRes & CMD1_CH);
   pCmdCoef [1] = ECMD_UTG_COEF;
   /* set the UTG coefficients */
   Utg_SetCoeff (pSimpleTone, pCmdCoef);
   ret = CmdWrite (pDev, pCmdCoef, 25);
   if (ret == IFX_ERROR)
      return IFX_ERROR;
   /* overwrite utg resource number, because here the "overall number" is used */
   pUtgCmd->bit.utgnr = (utgRes & CMD1_CH);
   /* Tone signal injection and muting the voice signal
     into adder 1 and/or adder 2 */
   switch (dst)
   {
      case  TAPI_TONE_DST_NET:
         /* put it to adder 1 to network */
         pUtgCmd->bit.a1 = SIG_UTG_MUTE;
         pUtgCmd->bit.a2 = SIG_UTG_NONE;
         break;
      case  TAPI_TONE_DST_NETLOCAL:
         /* put it to adder 1 to network and adder 2 for local */
         pUtgCmd->bit.a1 = SIG_UTG_MUTE;
         pUtgCmd->bit.a2 = SIG_UTG_MUTE;
         break;
      case  TAPI_TONE_DST_LOCAL:
      case  TAPI_TONE_DST_DEFAULT:
      default:
         /* play it locally: take adder 2 */
         pUtgCmd->bit.a1 = SIG_UTG_NONE;
         pUtgCmd->bit.a2 = SIG_UTG_MUTE;
         break;
   }
   /* activate the UTG with the tone action previously programmed */
   pUtgCmd->bit.sm = 0;
   pUtgCmd->bit.en = 1;
   ret = CmdWrite (pDev, pUtgCmd->value, 1);
   if (ret == IFX_SUCCESS)
   {
      /* tone is playing now, so set the state */
      pChannel->TapiComplexToneData[utgNum].nToneState = TAPI_CT_ACTIVE;

      if (pSimpleTone->loop > 0 || pSimpleTone->pause > 0)
      {
         /* auto stop after loop or after each generation step */
         pUtgCmd->bit.sm = 1;
         pUtgCmd->bit.en = 0;
         ret = CmdWrite (pDev, pUtgCmd->value, 1);
      }
   }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */

   return ret;
}

/**
   UTG deactivated event handling function. Is called from interrupt routine.
\param
   pChannel     - handle to TAPI_CONNECTION structure
\return
   none
\remarks
   This function does not deactivate the global signaling channel, because
   otherwise the switching function Dsp_SigActStatus must be protected
   against interrupts. Switching of signaling channel is not a must, but
   only a recommendation. In a typical scenario more sig resources would be
   used anyway, that the switching is not mandatory.
*/
IFX_void_t Tone_EventUTG_Deactivated (TAPI_CONNECTION *pChannel,
                                      IFX_uint8_t utgNum )
{
   VINETIC_DEVICE  *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   VINETIC_CHANNEL *pCh  = &pDev->pChannel[pChannel->nChannel];
   TAPI_TONE_DATA* pData;
   FWM_SIG_UTG* pUtgCmd = &pDev->pSigCh[pChannel->nChannel].utg[utgNum];

   /* possible utg resources, #0 and #1 */
   pData = &pChannel->TapiComplexToneData[utgNum];

   /* check utgNum,
   please note that resources like "utgNum" are counted from #0 */
   if  ( utgNum >= pDev->VinCapabl.nUtgPerCh)
   {
      SET_ERROR (ERR_NORESOURCE);
      return;
   }
   /* reset the cache state */
   pUtgCmd->bit.sm = 0;
   pUtgCmd->bit.en = 0;
   /* Tone generation is in deactivated state */
   pData->nToneState = TAPI_CT_DEACTIVATED;
   /* This is a simple tone so prepare for next repetition (if any)
      of a simple tone sequence to play    */
   if (pData->nType == TAPI_TONE_TYPE_SIMPLE)
   {
      Event_SimpleTone(pChannel, utgNum);
   }

   /* This is a composed tone so retrieve the next (if any) simple tone
      sequence to play    */
   else if (pData->nType == TAPI_TONE_TYPE_COMP)
   {
      /* Prepare for the for next repetition (if any) of a simple tone */
      Event_SimpleTone(pChannel, utgNum);

      if (pData->nToneState == TAPI_CT_IDLE)
      {
         if (pData->nPauseTime == 0)
         {
            /* Cadencde Time zero is not possible for repeting tones */
            return;
         }
         /* Init the simple tone repetition counter again */
         pData->nSimpleCurrReps = 0;
         /* Setup the next simple tone to play within the composed tone */
         pData->nToneCounter++;
         /* There are no more simple tones to play within the composed tone */
         if (pData->nToneCounter >= pData->nMaxToneCount)
         {
            /* Init the tone counter again to play repetative composed tones */
            pData->nToneCounter = 0;
            /* Prepare for the for next repetition (if any) of a composed tone */
            Event_ComposedTone(pChannel, utgNum);
         }

         /* Kick off the next simple tone sequence to play */
         else
         {
            /* Start the voice path teardowm timer */
            TAPI_SetTime_Timer (pCh->pToneRes[utgNum].Tone_Timer,
                                pData->nPauseTime,
                                IFX_FALSE, IFX_FALSE);
         }
      }
   }
   return;
}

#endif /* TAPI */

