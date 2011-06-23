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
   Module      : drv_tapi_tone.c
   Desription  : Contains TAPI Tone Services.
*******************************************************************************/


/* ============================= */
/* Includes                      */
/* ============================= */

#include "drv_tapi.h"

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

/* tone coefficient maximums */
#define MAX_TONE_TIME          64000
#define MAX_TONE_FREQ          32768
#define MIN_TONE_POWER         -300
#define MAX_TONE_POWER         0
#define MAX_CADENCE_TIME      64000   /* ms */
#define MAX_VOICE_TIME        64000   /* ms */


/* ============================= */
/* Local function declaration    */
/* ============================= */

IFX_LOCAL IFX_int32_t ComplexTone_Conf (TAPI_TONE_DATA* pData,
                                       COMPLEX_TONE *toneCoeffs,
                                        COMPLEX_TONE const *pTone);
/* ============================= */
/* Global function definition    */
/* ============================= */

/*******************************************************************************
Description:
   Sets the on-time for a tone with a specified duration.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   nTime           - Time in 0.25ms
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_Phone_Tone_Set_On_Time(TAPI_CONNECTION *pChannel,
                                        IFX_uint32_t nTime)
{
   /* set time only when no tone is played */
   if (pChannel->TapiTgToneData.nToneState != 0)
   {
      LOG(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: could not set on-time "
          "for channel %d\n\r", pChannel->nChannel));
      return IFX_ERROR;
   }

   pChannel->TapiTgToneData.nOnTime = nTime;

   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   Sets the off-time for a tone with a specified duration.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   nTime           - Time in 0.25ms
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_Phone_Tone_Set_Off_Time(TAPI_CONNECTION *pChannel,
                                         IFX_uint32_t nTime)
{
   /* set time only when no tone is played */
   if (pChannel->TapiTgToneData.nToneState != 0)
   {
      LOG(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: could not set off-time "
          "for channel %d\n\r", pChannel->nChannel));
      return IFX_ERROR;
   }

   pChannel->TapiTgToneData.nOffTime = nTime;

   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   Gets the on-time.
Arguments:
   pChannel        - handle to TAPI_CONNECTION
   pOnTime         - Tone On Time in 0.25ms
Return:
   IFX_SUCCESS
*******************************************************************************/
IFX_int32_t TAPI_Phone_Tone_Get_On_Time(TAPI_CONNECTION *pChannel,
                                        IFX_uint32_t *pOnTime)
{
   *pOnTime = pChannel->TapiTgToneData.nOnTime;
   return IFX_SUCCESS;
}


/*******************************************************************************
Description:
   Gets the off-time.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pOffTime        - Tone Off Time in 0.25ms
Return:
   IFX_SUCCESS
*******************************************************************************/
IFX_int32_t TAPI_Phone_Tone_Get_Off_Time(TAPI_CONNECTION *pChannel,
                                         IFX_uint32_t *pOffTime)
{
   *pOffTime = pChannel->TapiTgToneData.nOffTime;
   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   Plays a tone from the tone table.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   nToneIndex      - index in the tone table
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_Phone_Tone_Play(TAPI_CONNECTION *pChannel,
                                 IFX_int32_t nToneIndex, TAPI_TONE_DST dst)
{
   IFX_int32_t ret = IFX_SUCCESS;
   COMPLEX_TONE *pToneCoeff;
   IFX_int32_t src, res = 0;
   TAPI_DEV *pTapiDev = pChannel->pTapiDevice;
   TAPI_TONE_DATA* pData;

   /* filter out the source for playing out the tone */
   src = nToneIndex & (IFX_TAPI_TONE_SRC_TG | IFX_TAPI_TONE_SRC_DSP);
   nToneIndex &= ~(IFX_TAPI_TONE_SRC_TG | IFX_TAPI_TONE_SRC_DSP);
   /* resource selection */
   switch (src)
   {
   case IFX_TAPI_TONE_SRC_TG:
      res = 1;
      break;
   default:
      res = 0;
      break;
   }
   if (nToneIndex >= TAPI_MAX_TONE_CODE || nToneIndex < 0)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH,
            ("DRV_ERROR: Play complex tone code out of range\n\r"));
      return IFX_ERROR;
   }
   pData = &pChannel->TapiComplexToneData[res];
   /* Use the tone code as an index into internal tone coefficients table */
   pToneCoeff = &(pTapiDev->pToneTbl[nToneIndex]);
   if (nToneIndex >= IFX_TAPI_TONE_INDEX_MIN)
   {
      /* check if tone code is configured */
      if (pToneCoeff->type == TAPI_TONE_TYPE_NONE)
      {
         TRACE(TAPI_DRV, DBG_LEVEL_HIGH,
               ("\n\rDRV_ERROR: Reference to unconfigured tone code entry\n\r"));
         return IFX_ERROR;
      }
   }
   if (nToneIndex == 0)
   {
      do
      {
         /* stop all resources with direction "dst" */
         pData = &pChannel->TapiComplexToneData[res];
         if (pData->dst == dst)
         {
           /* Set tone generation back to initial state */
           pData->nToneState = TAPI_CT_IDLE;
           /* stop play out */
           ret = TAPI_LL_Phone_Tone_Stop (pChannel, res,
                                          &pTapiDev->pToneTbl[pData->nToneIndex]);
           /* reset index afterwards anyway */
           pData->nToneIndex = 0;
         }
         res++;
      }
      while ( /* pData->dst != dst  && */  res < TAPI_TONE_MAXRES  );
   }
   else
   {
      /* else: nToneIndex != 0 means: Play Tone! */
      do
      {
         /* search for available resources */
         pData = &pChannel->TapiComplexToneData[res];
         if  (pData->nToneState == TAPI_CT_IDLE)
         {
            /* preparation for complex tones */
            ComplexTone_Conf (pData, pTapiDev->pToneTbl, pToneCoeff);
            ret = TAPI_LL_Phone_Tone_Play(pChannel, res, pToneCoeff, dst);
            if (ret == IFX_SUCCESS)
            {   /* store the information */
               pData->dst = dst;
               pData->nToneIndex = nToneIndex;
            }
         }
         else
         {
            ret = IFX_ERROR;
         }
         res++;
      }
      while ( ret  != IFX_SUCCESS  &&  res < TAPI_TONE_MAXRES );

      if (ret != IFX_SUCCESS)
      {
         TRACE(TAPI_DRV, DBG_LEVEL_HIGH,
               ("DRV_ERROR: Tone play failed\n\r"));
         ret = IFX_ERROR;
      }
   }

   return ret;
}

/*******************************************************************************
Description:
   Sets the tone level of tone currently played
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   nToneIndex      - index in the tone table
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_Phone_Tone_Set_Level(TAPI_CONNECTION *pChannel,
                                      IFX_TAPI_PREDEF_TONE_LEVEL_t const *pToneLevel)
{
   IFX_int32_t ret = IFX_SUCCESS;

   ret = TAPI_LL_Phone_Tone_Set_Level(pChannel, pToneLevel);

   return ret;
}

/*******************************************************************************
Description:
   Gets the tone playing state.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pToneState      - 0: no tone is played
                     1: tone is played (on-time)
                     2: silence (off-time)
Return:
   IFX_SUCCESS
*******************************************************************************/
IFX_int32_t TAPI_Phone_Tone_Get_State(TAPI_CONNECTION *pChannel,
                                      IFX_uint32_t *pToneState)
{
   *pToneState = pChannel->TapiTgToneData.nToneState;
   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   Plays a dial tone.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_Phone_Tone_Dial(TAPI_CONNECTION *pChannel)
{
   IFX_int32_t ret;

   /* set times for dial tone */
   pChannel->TapiTgToneData.nOnTime  = 0xFFFFFF;
   pChannel->TapiTgToneData.nOffTime = 0;

   /* play dial tone */
   ret = TAPI_Phone_Tone_Play(pChannel, 25, TAPI_TONE_DST_LOCAL);

   return ret;
}

/*******************************************************************************
Description:
   Plays a ringback tone.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_Phone_Tone_Ringback(TAPI_CONNECTION *pChannel)
{
   IFX_int32_t ret;

   /* set times for dial tone */
   pChannel->TapiTgToneData.nOnTime  = 4000;
   pChannel->TapiTgToneData.nOffTime = 16000;
   ret = TAPI_Phone_Tone_Play (pChannel, 26, TAPI_TONE_DST_LOCAL);

   return ret;
}

/*******************************************************************************
Description:
   Plays a busy tone.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_Phone_Tone_Busy(TAPI_CONNECTION *pChannel)
{
   IFX_int32_t ret;

   /* set times for busy tone */
   pChannel->TapiTgToneData.nOnTime  = 2000;
   pChannel->TapiTgToneData.nOffTime = 2000;

   ret = TAPI_Phone_Tone_Play (pChannel, 27, TAPI_TONE_DST_LOCAL);

   return ret;
}

/*******************************************************************************
Description:
   Add simple tone to internal tone coefficients table
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pSimpleTone     - entry for internal tone table
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_Phone_Add_SimpleTone(COMPLEX_TONE* toneCoefficients,
                                      IFX_TAPI_TONE_SIMPLE_t const *pSimpleTone)
{
   IFX_TAPI_TONE_SIMPLE_t *pToneCfg = IFX_NULL;
   IFX_uint32_t i;

   if (pSimpleTone->index >= TAPI_MAX_TONE_CODE || pSimpleTone->index == 0)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH,
            ("DRV_ERROR: Simple tone code out of range\n\r"));
      return IFX_ERROR;
   }
   /* check on time to set */
   for (i=0; i < IFX_TAPI_TONE_STEPS_MAX; ++i)
   {
      if (pSimpleTone->cadence[i] > MAX_TONE_TIME)
      {
         TRACE(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Max on time "
               "time is %d ms\n\r", MAX_TONE_TIME));
         return IFX_ERROR;
      }
   }
   /* check for zero time on frequency A */
   if (pSimpleTone->cadence[0] == 0)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Max off/on time "
            "for first step may not be zero\n\r"));
      return IFX_ERROR;
   }
   /* check pause time to set */
   if (pSimpleTone->pause > MAX_CADENCE_TIME)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Max pause time "
            "is %d ms\n\r", MAX_CADENCE_TIME));
      return IFX_ERROR;
   }

   /* check frequencies to set */
   if (pSimpleTone->freqA > MAX_TONE_FREQ ||
       pSimpleTone->freqB > MAX_TONE_FREQ ||
       pSimpleTone->freqC > MAX_TONE_FREQ ||
       pSimpleTone->freqD > MAX_TONE_FREQ)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Max frequency "
            "is %d Hz\n\r", MAX_TONE_FREQ));
      return IFX_ERROR;
   }
   /* check power level to set */
   if (pSimpleTone->levelA < MIN_TONE_POWER ||
       pSimpleTone->levelA > MAX_TONE_POWER ||
       pSimpleTone->levelB < MIN_TONE_POWER ||
       pSimpleTone->levelB > MAX_TONE_POWER ||
       pSimpleTone->levelD < MIN_TONE_POWER ||
       pSimpleTone->levelD > MAX_TONE_POWER ||
       pSimpleTone->levelC < MIN_TONE_POWER ||
       pSimpleTone->levelC > MAX_TONE_POWER)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Min power "
            "level is %d Max power level is %d\n\r", MIN_TONE_POWER,
            MAX_TONE_POWER));
      return IFX_ERROR;
   }
   /* Set ptr to tone to be configured */
   pToneCfg = &toneCoefficients[pSimpleTone->index].tone.simple;
   /* Add entry to internal tone table */
   toneCoefficients[pSimpleTone->index].type = TAPI_TONE_TYPE_SIMPLE;
   /* Is the tone out of predefined tone range ? */
   if (pSimpleTone->index >= IFX_TAPI_TONE_INDEX_MIN)
   {
      *pToneCfg = *pSimpleTone;
   }
   else
   {
      /* add tone to tone table in case the tone if configured the first time */
      if (pToneCfg->frequencies [0] == IFX_TAPI_TONE_FREQNONE)
      {
         *pToneCfg = *pSimpleTone;
      }
      else
      {
         /* for predefined tones, adapt only cadence and level */
         pToneCfg->levelA      = pSimpleTone->levelA;
         pToneCfg->levelB      = pSimpleTone->levelB;
         /* tone on time */
         pToneCfg->cadence [0] = pSimpleTone->cadence [0];
         /* tone off time */
         pToneCfg->cadence [1] = pSimpleTone->cadence [1];
         /* set loop. 0 means continuous tone */
         pToneCfg->loop        = pSimpleTone->loop;
      }
   }

   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   Add composed tone to internal tone coefficients table
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pComposedTone   - entry for internal tone table
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_Phone_Add_ComposedTone (COMPLEX_TONE* toneCoefficients,
                                         IFX_TAPI_TONE_COMPOSED_t const *pComposedTone)
{
   IFX_int32_t ret = IFX_SUCCESS;
   IFX_uint32_t i;

   if (pComposedTone->index >= TAPI_MAX_TONE_CODE || pComposedTone->index == 0)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH,
            ("DRV_ERROR: Composed tone code out of range\n\r"));
      return IFX_ERROR;
   }

   /* check alternate voice time to set */
   if (pComposedTone->alternatVoicePath > MAX_VOICE_TIME)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Max alternate voice "
            "time is %d ms\n\r", MAX_VOICE_TIME));
      return IFX_ERROR;
   }

   /* check loop count against alternate voice time */
   if (pComposedTone->loop == 1 && pComposedTone->alternatVoicePath != 0)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Single repetition "
            "not allowed when alternate voice time is non-zero\n\r"));
      return IFX_ERROR;
   }

   /* Validate the number of simple tones allowed within composed tone */
   if (pComposedTone->count == 0 ||
       pComposedTone->count > IFX_TAPI_TONE_SIMPLE_MAX)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH,
            ("\n\rDRV_ERROR: Number of simple tone fields is out of range\n\r"));
      return IFX_ERROR;
   }

   /* Validate if all simple tone codes are configured within the composed tone */
   for (i = 0; i < pComposedTone->count; i++)
   {
      if (pComposedTone->tones[i] >= TAPI_MAX_TONE_CODE ||
         pComposedTone->tones[i] == 0)
      {
         TRACE(TAPI_DRV, DBG_LEVEL_HIGH,
            ("DRV_ERROR: Simple tone code within composed tone is out of range\n\r"));
         return IFX_ERROR;
      }
      if (toneCoefficients[pComposedTone->tones[i]].type == TAPI_TONE_TYPE_NONE)
      {
         TRACE(TAPI_DRV, DBG_LEVEL_HIGH,
            ("\n\rDRV_ERROR: Simple tone code %d within composed tone not configured\n\r",
            pComposedTone->tones[i]));
         return IFX_ERROR;
      }
   }

   /* Add entry to internal tone table */
   toneCoefficients[pComposedTone->index].type          = TAPI_TONE_TYPE_COMP;
   toneCoefficients[pComposedTone->index].tone.composed = *pComposedTone;

   return ret;
}

/*******************************************************************************
Description:
   Add tone to internal tone coefficients table
Arguments:
   pChannel - handle to TAPI_CONNECTION structure
   pTone    - entry for internal tone table
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_Phone_Tone_TableConf (COMPLEX_TONE* toneCoefficients,
                                       IFX_TAPI_TONE_t const *pTone)
{
   IFX_int32_t ret = IFX_ERROR;
   switch (pTone->simple.format)
   {
      case  IFX_TAPI_TONE_TYPE_SIMPLE:
         ret = TAPI_Phone_Add_SimpleTone(toneCoefficients, &pTone->simple);
      break;
      case  IFX_TAPI_TONE_TYPE_COMPOSED:
         ret = TAPI_Phone_Add_ComposedTone(toneCoefficients, &pTone->composed);
      break;
      default:
         /* error */
         break;
   }
   return ret;
}

/*******************************************************************************
Description:
   Play complex (simple or composed) tone
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   nToneCode   - tone code reference for internal tone table
   pToneCoeff  -
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_LOCAL IFX_int32_t ComplexTone_Conf(TAPI_TONE_DATA* pData,
                                       COMPLEX_TONE *toneCoeffs,
                                       COMPLEX_TONE const *pToneCoeff)
{
   IFX_uint32_t firstToneCode = 0;

   /* Setup the tone code type */
   pData->nType = pToneCoeff->type;

   /* This is a composed tone so get the first simple tone sequence to play */
   if (pToneCoeff->type == TAPI_TONE_TYPE_COMP)
   {
      /* Retrieve composed configuration from internal tone coefficients table */
      pData->nComposedMaxReps    = pToneCoeff->tone.composed.loop;
      pData->nComposedCurrReps   = 0;
      pData->nAlternateVoiceTime = pToneCoeff->tone.composed.alternatVoicePath;
      pData->nComposedToneCode   = pToneCoeff->tone.composed.index;
      pData->nMaxToneCount       = pToneCoeff->tone.composed.count;
      pData->nToneCounter        = 0;

      /* First simple tone sequence to play */
      firstToneCode = pToneCoeff->tone.composed.tones[0];

      /* Use the simple tone code as an index into internal tone coefficients table */
      pToneCoeff = &toneCoeffs[firstToneCode];
   }

   /* Retrieve simple tone configuration from internal tone coefficients table */
   pData->nSimpleMaxReps      = pToneCoeff->tone.simple.loop;
   pData->nSimpleCurrReps     = 0;
   pData->nPauseTime          = pToneCoeff->tone.simple.pause;
   pData->nSimpleToneCode     = pToneCoeff->tone.simple.index;
   return IFX_SUCCESS;
}



/*******************************************************************************
Description:
   Detects a tone based on the complex (simple or composed) tone
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   nToneCode   - tone code reference for internal tone table
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   The function returns an error, when the tone is not previously defined
*******************************************************************************/
IFX_int32_t TAPI_Phone_DetectToneStart (TAPI_CONNECTION *pChannel,
                                        IFX_TAPI_TONE_CPTD_t const *signal)
{
   COMPLEX_TONE *pToneCoeff;
   IFX_int32_t ret = IFX_SUCCESS;
   TAPI_DEV* pTapiDev = pChannel->pTapiDevice;

   /* Use the tone code as an index into internal tone coefficients table */
   pToneCoeff = &(pTapiDev->pToneTbl[signal->tone]);

   /* check if tone code is configured */
   if (pToneCoeff->type == TAPI_TONE_TYPE_NONE)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH,
            ("\n\rDRV_ERROR: Reference to unconfigured tone code entry\n\r"));
      return IFX_ERROR;
   }
   if (signal->tone >= TAPI_MAX_TONE_CODE || signal->tone <= 0)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH,
            ("DRV_ERROR: Detect complex tone code out of range\n\r"));
      return IFX_ERROR;
   }
   if (pToneCoeff->type == TAPI_TONE_TYPE_COMP)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH,
            ("\n\rDRV_ERROR: Reference to unsupported tone code entry\n\r"));
      return IFX_ERROR;
   }

   ret = TAPI_LL_CPTD_Start (pChannel, &pToneCoeff->tone.simple, signal->signal);
   if (ret == IFX_SUCCESS)
   {
      /* store it to the signals */
      pChannel->TapiMiscData.sigMask |= IFX_TAPI_SIG_CPTD;
   }
   return ret;
}

/*******************************************************************************
Description:
   Stops the detection a tone based on the complex (simple or composed) tone
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS
*******************************************************************************/
IFX_int32_t TAPI_Phone_DetectToneStop (TAPI_CONNECTION *pChannel)
{
   /* removed it from the signals */
   pChannel->TapiMiscData.sigMask &= ~IFX_TAPI_SIG_CPTD;
   TAPI_LL_CPTD_Stop (pChannel);
   return IFX_SUCCESS;
}


/*******************************************************************************
Description:
   Stops Tone Generation, only in connection with "IFX_TAPI_TONE_STOP"
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   nToneIndex      - tone to be stopped
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_Phone_Tone_Stop(TAPI_CONNECTION *pChannel,
                                 IFX_int32_t nToneIndex)
{
   TAPI_TONE_DATA* pData;
   IFX_int32_t res = 0;
   IFX_int32_t ret = IFX_ERROR;

   if (nToneIndex == 0)
   {
      /* for backwards compability:
      if nToneIndex is 0, stop tones played on UTG and TG */
      do
      {
         pData = &pChannel->TapiComplexToneData[res];
         ret = TAPI_Phone_Tone_Play(pChannel, 0, pData->dst );
         res++;
      }
      while (res < TAPI_TONE_MAXRES);
   }
   else
   {
      do /* stop all resources with nToneIndex == nToneIndex */
      {
         pData = &pChannel->TapiComplexToneData[res];
         if (pData->nToneIndex == nToneIndex)
         {
            ret = TAPI_Phone_Tone_Play(pChannel, 0, pData->dst );
         }
         res++;
      }
      while ( /* pData->dst != dst  && */  res < TAPI_TONE_MAXRES  );
   }
   return (ret);
}
