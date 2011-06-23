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
   Module      : drv_vinetic_gr909.c
   Description : This file contains the definition of the globals and local
                 defines and functions needed for Vinetic GR909 realtime
                 measurements.
   Remarks     : Driver should be compiled with -DTAPI_GR909 to support
                 this measurement. Feature is tested under Linux and VxWorks.
   Note        : Because floating points are not allowed in driver modules
                 under some OS, float values will be rounded to decimal values
                 in the whole file.
********************************************************************************/


/* ============================= */
/* includes                      */
/* ============================= */
#include "drv_vinetic_api.h"
#include "drv_vinetic_cram.h"
#include "drv_vinetic_gr909.h"

/*#define GRAPH*/
#if (VIN_CFG_FEATURES & VIN_FEAT_GR909)

/*#define GRAPH*/

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

/* general use defines */
/* to avoid 32 bits overflow while summing the DC values,
   this scaling factor will be used so that the end result
   still fits into a 32 bit variable. */
#define SCALING_FACTOR_32BIT 2
#define DEFAULT_SHIFT   0x0080 /* 128 */
#define GR909_RES_OVFLOW    0x7FFF
#define POW_2_15        32768
/* timeout waiting for LMRES.
   Note : For Vinetic 1.4, a timeout of at least 100 ms is needed.
                               This is still in clarification why!
                               For Vinetic 1.3, 10/20 ms are sufficient */
#define RES_TIMEOUT          150
/***************************/
/* Foreign Voltage defines */
/***************************/
#define LINE_HIRT       0x0030
#define DEFAULT_PERIOD  20 /* 20 ms -> 50 Hz */
#define MIN_PERIOD      5  /* 5 ms -> 200 Hz */
#define MAX_SAMPLES     100
#define MAX_RESULT      0x8000 /* -32768 */
#define MIN_RESULT      0x7FFF /* +32767 */
#define SAMPLE_PERIOD   1      /* 1 ms */

#ifdef CHECK_EXT_VOLT
/* conversion factor from DC to Input voltage */
/* Vinetic 1.3 : 118.36/(Vad*Ktr), Vad = 1.2, Ktr = 1
   values are rounded because float values arent allowed */
#define DC_TO_INPUT_V13 99
/* Vinetic 1.4 : 176.36/(Vad*Ktr), Vad = 1,28, Ktr = 1.066
   values are rounded because float values arent allowed */
#define DC_TO_INPUT_V14 129
#endif /* CHECK_EXT_VOLT */

/***************************/
/* Resistive Fault defines */
/***************************/
/* 15 mA constant current */
#define IK_15MA         15
/* 30 mA constant current */
#define IK_30MA         30
/* LM Value for 15 mA :
   It = LmVal/32768 * 88.2 => LmVal = (It*32768) / 88.2 */
#define LMVAL_10MA_V13  3715
/* LM Value for 15 mA :
   It = LmVal/32768 * 100.204 => LmVal = (It*32768) / 100.204 */
#define LMVAL_10MA_V14  3270

/****************************/
/* Receiver Offhook defines */
/****************************/

/***************************/
/* Ringer tests defines    */
/***************************/
#define FRQ_2_KHZ       2000
#define RINGER_AC_RMS   15  /* AC = 15 Vrms */
#define RINGER_DC_VOLT  10  /* DC = 10 V */
#define RINGER_FREQ     20  /* 20 Hz -> 100 samples */
#define IT_FACTOR_V13   88  /* Chip 1.3 : normally 88.2, but we can't work with
                              floating points in driver level */
#define IT_FACTOR_V14   100  /* Chip 1.4 : normally 100.204 = (94 * 1,066),
                                but we can't work with floating points
                                in driver level */
/***************************/
/* Functions defines       */
/***************************/

#define SET_STATE(state)      \
do                            \
{                             \
   pCh->Gr909.nState = state; \
} while (0)


#define GET_STATE() pCh->Gr909.nState

#define READ_SAMPLES(nr,samplebuf,ms)\
   {\
      IFX_int32_t  j = 0;\
      IFX_uint8_t  nCh = (pCh->nChannel - 1);\
      IFX_uint16_t nRes;\
      while (j < (nr))\
      {\
         RegRead  (pDev, (CMD1_SOP | nCh), &nRes, 1, SOP_OFFSET_LMRES);\
         samplebuf[j] = (IFX_int16_t)nRes;\
         IFXOS_DELAYMS (ms);\
         j++;\
      }\
   }

/***************************/
/*           Enums         */
/***************************/

/* save/restore enums */
enum
{
   SAVE_REG_CONF    = 0,
   RESTORE_REG_CONF = 1
};

/* ============================= */
/* Global function declaration   */
/* ============================= */

/* ============================= */
/* Local structures              */
/* ============================= */

/* ============================= */
/* Local function declaration    */
/* ============================= */
/* #define GRAPH */

#ifdef GRAPH
IFX_LOCAL IFX_void_t sampleGraph      (IFX_int16_t *pSamples, IFX_int16_t nMaxVal, IFX_int16_t nMinVal);
#endif /* GRAPH */
/* general functions */
IFX_LOCAL IFX_void_t resetGr909Struct (VINETIC_GR909 *pGr909);
IFX_LOCAL IFX_int32_t  DoLastStateJob (VINETIC_CHANNEL *pCh);
#ifdef ENABLE_TRACE
IFX_LOCAL IFX_void_t ParamMonitor     (IFX_uint8_t mode, IFX_int32_t *pParam);
#endif /* ENABLE_TRACE */

/* chip revision check */
IFX_LOCAL IFX_int32_t Gr909_ChipRev_Check (VINETIC_DEVICE *pDev);

/* general register operations */
IFX_LOCAL IFX_int32_t  GR909_Reg      (VINETIC_CHANNEL *pCh, IFX_int32_t nAction);
IFX_LOCAL IFX_int32_t  GR909_Preset   (VINETIC_CHANNEL *pCh);

/* general  measurements subfunctions */
IFX_LOCAL IFX_int32_t  DoOffsetCor    (VINETIC_CHANNEL *pCh, IFX_uint16_t nSel, IFX_uint16_t *pRes);
IFX_LOCAL IFX_int32_t  GetIntRes      (VINETIC_CHANNEL *pCh, IFX_int16_t *pRes, IFX_int32_t timeout);
IFX_LOCAL IFX_int32_t  GetOptimumRes  (VINETIC_CHANNEL *pCh, IFX_int32_t *pRes, IFX_int32_t  *pShift);
IFX_LOCAL IFX_int32_t  GetDcVal       (VINETIC_CHANNEL *pCh, IFX_int32_t *pIt, IFX_int32_t *pVdc);
IFX_LOCAL IFX_int16_t getPeriod       (IFX_int16_t *pSamples, IFX_int16_t *pStart);
IFX_LOCAL IFX_void_t  Calc_VoltParam  (VINETIC_CHANNEL *pCh, IFX_int16_t *pSamples, IFX_int32_t *pParam);

/* measurements state machines */
IFX_LOCAL  IFX_int32_t GR909_StateMachine_Voltage    (VINETIC_CHANNEL *pCh);
IFX_LOCAL  IFX_int32_t GR909_StateMachine_ResFault   (VINETIC_CHANNEL *pCh);
IFX_LOCAL  IFX_int32_t GR909_StateMachine_RecOffHook (VINETIC_CHANNEL *pCh);
IFX_LOCAL  IFX_int32_t GR909_StateMachine_Ringer     (VINETIC_CHANNEL *pCh);
typedef IFX_int32_t (* GR909_TEST)                (VINETIC_CHANNEL *pCh);

GR909_TEST Start_StateMachine [MAX_GR909_TESTS] =
{
   GR909_StateMachine_Voltage,
   GR909_StateMachine_Voltage,
   GR909_StateMachine_ResFault,
   GR909_StateMachine_RecOffHook,
   GR909_StateMachine_Ringer
};

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* allocate IFX_LOCAL memory for sample
   data on each analog channel */
IFX_LOCAL IFX_int16_t Samples [VINETIC_ANA_CH_NR][MAX_SAMPLES];

/* ============================= */
/* Local function definition     */
/* ============================= */

#ifdef GRAPH
/*******************************************************************************
Description:
   design sample graph
Arguments:
   pSample  - sample buffer
Return:
   NONE
Remarks:
   draws the voltage graph during Voltage measurement.
*******************************************************************************/
IFX_LOCAL IFX_void_t sampleGraph (IFX_int16_t *pSamples, IFX_int16_t nMaxVal,
                                  IFX_int16_t nMinVal)
{
   IFX_int32_t range, step = 0, i;

   Drv_Log ("\n\r\n\r\n\r\n\r");
   /* step */
   step = ((nMaxVal - nMinVal)/15);
   if (step == 0)
    step = 1;

   Drv_Log ("plot in interval ]%d, %d[\n\r\n\r", nMinVal, nMaxVal);

   for (range = nMaxVal; range >= (nMinVal - 1); range -= step)
   {
      Drv_Log (KERN_DEBUG "%6ld - ", range);
      for (i=0; i < MAX_SAMPLES; i++)
      {
         if ((pSamples [i] <= range) && (pSamples [i] > (range - step)))
            Drv_Log ("*");
         else
            Drv_Log (" ");
      }
      Drv_Log ("\n\r");
   }
   Drv_Log ("\n\r\n\r\n\r\n\r");
}
#endif /* GRAPH */

/*******************************************************************************
Description:
   reset GR909 data structure
Arguments:
   pGr909         - pointer to VINETIC_GR909 structure
Return:
   none
*******************************************************************************/
IFX_LOCAL IFX_void_t resetGr909Struct (VINETIC_GR909 *pGr909)
{
   memset (&pGr909->reg, 0, sizeof (VINETIC_GR909_REG));
   memset (&pGr909->Usr, 0, sizeof (VINETIC_IO_GR909));
   pGr909->bData     = IFX_FALSE;
   pGr909->nState    = 0;
   pGr909->Gr909Call = NULL;
}

/*******************************************************************************
Description:
   Error Information
Arguments:
   pCh         - pointer to channel structure
Return:
   none
*******************************************************************************/
IFX_LOCAL IFX_void_t ErrorInfo (VINETIC_CHANNEL *pCh)
{
   IFX_char_t const *sErrInfo = "";

   switch (pCh->Gr909.Usr.mode)
   {
   case GR909_HAZARDOUS_VOLTAGE:
   case GR909_FOREIGN_FORCE:
      switch (GET_STATE())
      {
      case  TEST_OK_STATE:
         sErrInfo = "No Ground key event or proccessing OK";
         break;
      case TEST_NOTOK_STATE:
         sErrInfo = "IFX_ERROR: Operating Mode not suitable or"
                    " Tip/RING RMS value out of limit";
         break;
      case INTERNAL_ERR_STATE:
         sErrInfo =  "IFX_ERROR: Config error in Voltage tests proccessing";
         break;
      default:
         break;
      }
      break;
   case GR909_RESISTIVE_FAULT:
      switch (GET_STATE())
      {
      case TEST_OK_STATE:
         sErrInfo = "Resistive Fault tests proccessing OK";
         break;
      case TEST_NOTOK_STATE:
         sErrInfo = "IFX_ERROR: Measurement not allowed for Resistance range";
         break;
      case INTERNAL_ERR_STATE:
         sErrInfo =  "IFX_ERROR: Config error in Resistive Fault proccessing";
         break;
      default:
         break;
      }
      break;
   case GR909_RECEIVER_OFF_HOOK:
      switch (GET_STATE())
      {
      case TEST_OK_STATE:
         sErrInfo = "Receiver offhok: It under treshold value or proccessing OK";
         break;
      case INTERNAL_ERR_STATE:
         sErrInfo = "IFX_ERROR: Config error in Receiver Offhook tests proccessing";
         break;
      default:
         break;
      }
      break;
   case GR909_RINGER:
      switch (GET_STATE())
      {
      case TEST_OK_STATE:
         sErrInfo = "Ringer test processing OK";
         break;
      case TEST_NOTOK_STATE:
         sErrInfo = "IFX_ERROR: It under treshold value (1mA) for Ringer tets";
         break;
      case INTERNAL_ERR_STATE:
         sErrInfo = "IFX_ERROR: Config error in Ringer tests proccessing";
         break;
      default:
         break;
      }
      break;
   default:
      break;
   }
   TRACE (VINETIC, DBG_LEVEL_HIGH, ("%s\n\r", sErrInfo));
}

#ifdef ENABLE_TRACE
/*******************************************************************************
Description:
   Debug print of parameters set for user
Arguments:
   mode     - measurement mode
   pParam   - parameter buffer
Return:
   none
Remarks :
   Debug code used to display content of user values buffer
*******************************************************************************/
IFX_LOCAL IFX_void_t ParamMonitor (IFX_uint8_t mode, IFX_int32_t *pParam)
{
   IFX_int32_t i, count = 0;
   IFX_char_t const *sGr909 [5] = {"HAZARDEOUS VOLTAGE", "FOREIGN ELECTROMOTIVE FORCE",
                       "RESISTIVE FAULT", "RECEIVER OFFHOOK", "RINGER"};
   IFX_char_t const *sVolt [13]  = {"SCAL_AC","RG_DC","RG_DC_AC","RG_AC","RG_PER","TG_DC",
                        "TG_DC_AC","TG_AC", "TG_PER","TR_DC","TR_DC_AC",
                        "TR_AC","TR_PER"};
   IFX_char_t const *sResF [12] = {"IT_TR_NP", "V_TR_NP", "IT_TR_RP", "V_TR_RP",
                        "IT_TG_NP","V_TG_NP", "IT_TG_RP", "V_TG_RP",
                        "IT_RG_NP","V_RG_NP", "IT_RG_RP", "V_RG_RP"};
   IFX_char_t const *sRecO [4] = {"IT_20mA", "V_20_mA", "IT_30mA", "V_30_mA"};
   IFX_char_t const *sRng  [5] = {"AC_RMS", "DC_V", "SMPLNR", "SHIFT", "IT_RES"};
   IFX_char_t const **sMes      = NULL;


   TRACE(VINETIC, DBG_LEVEL_NORMAL,
        ("\n\r---------------- DRV PARAMETERS MONITORING ----------------\n\r"));
   TRACE(VINETIC, DBG_LEVEL_NORMAL, ("%s\n\r\n\r", sGr909 [mode]));

   switch (mode)
   {
   case GR909_HAZARDOUS_VOLTAGE :
   case GR909_FOREIGN_FORCE :
      count = 13;
      sMes  = sVolt;
      break;
   case GR909_RESISTIVE_FAULT :
      count = 12;
      sMes  = sResF;
      break;
   case GR909_RECEIVER_OFF_HOOK :
      count = 4;
      sMes  = sRecO;
      break;
   case GR909_RINGER :
      count = 5;
      sMes  = sRng;
      break;
   default:
      count = 0;
      break;
   };
   /* print parameters */
   if (sMes != NULL)
      for  (i = 0; i < count; i++)
         TRACE(VINETIC, DBG_LEVEL_NORMAL, ("%s\t\t\t\t=\t0x%08lX (%ld)\n\r",
            sMes [i], pParam [i], pParam [i]));

   TRACE(VINETIC, DBG_LEVEL_NORMAL,
        ("-----------------------------------------------------------\n\r"));
}
#endif /* ENABLE_TRACE */

/*******************************************************************************
Description:
   calculate the sample period
Arguments:
   pVolt        - pointer to parameter structure
   pStart       - period start point in sample buffer
Return:
   sample period
Remarks:
   this function looks for minima/maxima inside sample buffer to gat the period
   from that.
   The period is assumed to be the time spent from one minimum/maximum to
   the next one.
*******************************************************************************/
IFX_LOCAL IFX_int16_t getPeriod (IFX_int16_t *pSamples, IFX_int16_t *pStart)
{
   IFX_boolean_t bUp = IFX_TRUE;
   IFX_uint8_t   aIndexMin [2] = {0}, aIndexMax [2] = {0};
   IFX_int16_t   num = 0, nMaxRes = (IFX_int16_t)MAX_RESULT, nMinRes = (IFX_int16_t)MIN_RESULT,
                 nMaxVal = (IFX_int16_t)MAX_RESULT, nMinVal = (IFX_int16_t)MIN_RESULT,
                 nIndexMin = 0, nIndexMax = 0,
                 nStop = 0, nPeriod = DEFAULT_PERIOD;

   /* process sample buffer to get minima/maxima info :
      just 2 points are needed */
   while (num < MAX_SAMPLES)
   {
      TRACE(VINETIC, DBG_LEVEL_LOW, ("Samples [%2d]: %5d / 0x%08X ",
            num, pSamples [num], pSamples [num]));

      /* set max and min results if */
      if (nMaxRes < pSamples [num])
         nMaxRes = pSamples [num];
      if (nMinRes > pSamples [num])
         nMinRes = pSamples [num];
      /* look alternatively for maximas and minimas in the sample buffer */
      if (bUp == IFX_TRUE)
      {
         /* set maximum, assuming first sample can't be greather */
         if (nMaxVal <= pSamples [num])
            nMaxVal = pSamples [num];
         else
         {
            nMaxVal = (IFX_int16_t)MAX_RESULT;
            bUp = IFX_FALSE;
            /* save max index, assumeing the 2nd element is not
               in appropriate range */
            if (num != 1)
            {
               TRACE(VINETIC, DBG_LEVEL_NORMAL,
                     ("max (index %d = 0x%08X = %6d)\n\r",(num - 1),
                       pSamples [num], pSamples [num]));
               /* the previous sample was a maxima */
               aIndexMax[nIndexMax] = (IFX_uint8_t)(num - 1);
               nIndexMax ++;
            }
         }
      }
      else
      {
         /* set minimum, assuming first sample can't be lower */
         if (nMinVal >= pSamples [num])
            nMinVal = pSamples [num];
         else
         {
            nMinVal = MIN_RESULT;
            bUp = IFX_TRUE;
            /* save min index, assumeing the 2nd element is not in
               appropriate range */
            if (num != 1)
            {
               TRACE(VINETIC, DBG_LEVEL_NORMAL,
                     ("min (index %d = 0x%08X = %6d)\n\r",(num - 1),
                      pSamples [num], pSamples [num]));
               /* the previous sample was a minima */
               aIndexMin[nIndexMin] = (IFX_uint8_t)(num - 1);
               nIndexMin ++;
            }
         }
      }

      TRACE(VINETIC, DBG_LEVEL_NORMAL, ("\n\r"));
      if ((nIndexMin >= 2) && (nIndexMax >= 2))
         break;

      /* go to next sample */
      num ++;
   }
   /* set new period value if */
   if ((nIndexMin >= 2) || (nIndexMax >= 2))
   {
      /* get start/stop from mimimum points */
      if (nIndexMin >= 2)
      {
         *pStart = aIndexMin[0];
         nStop   = aIndexMin[1];
      }
      else
      {
         *pStart = aIndexMax[0];
         nStop   = aIndexMax[1];
      }
      /* calculate period */
      nPeriod = (nStop - pStart [0]);
      TRACE(VINETIC, DBG_LEVEL_NORMAL,("calculated Period: %d\n\r", nPeriod));
   }

#ifdef GRAPH
  sampleGraph    (pSamples , nMaxRes, nMinRes);
#endif /* GRAPH */

   return nPeriod;
}

/*******************************************************************************
Description:
   calculate the DC,  RMS (square) value  and period of the sample data
Arguments:
   pCh      - handle to channel structure
   pSamples - Sample buffer
   pParam   - pointer to parameter structure
Return:
   NONE
Remarks:
   DC =  (SUM(samples) / period)
   RMS = (SUM (sample*sample) / period)
Note:
   Because of problems generated by a 64-bit division, the sampled values
   are divided by the defined  scaling factor to avoid 32-bit overflows.
   The application code should consider this factor during final calculations.
   In the result buffer given as parameter, the values are represented
   the following way:

   DC value     : pParam [0] = (Sum (Samples) / Period)
   DC value AC  : pParam [1] = (Sum (Samples/2^Scaling Factor) / Period)
   AC value     : pParam [2] = (Sum ((Samples/2^Scaling Factor)^2) / Period)
   Period       : pParam [3] = Period
*******************************************************************************/
IFX_LOCAL IFX_void_t Calc_VoltParam (VINETIC_CHANNEL *pCh, IFX_int16_t *pSamples,
                                     IFX_int32_t *pParam)
{
   IFX_int32_t    nDc = 0, nDcAc = 0, nRms = 0;
   IFX_int16_t    nPeriod = 0, i, nVal, nValAc, nStart = 0;
   VINETIC_DEVICE *pDev = pCh->pParent;

   /* read samples with 1 ms interval  */
   READ_SAMPLES (MAX_SAMPLES, pSamples, SAMPLE_PERIOD);
   /* get Sample period and start point */
   nPeriod =  getPeriod (pSamples, &nStart);
   /* set period = 50 Hz if too big frequency */
   if (nPeriod < MIN_PERIOD)
   {
      nStart   = 0;
      nPeriod  = DEFAULT_PERIOD;
   }
   /* make sum over one period  */
   for (i = nStart; i < (nStart + nPeriod); i++)
   {
      TRACE(VINETIC, DBG_LEVEL_LOW,
           ("Sample %d :  0x%08X\n\r", i, pSamples [i]));
      nVal     = pSamples [i];
      nValAc   = (nVal >> SCALING_FACTOR_32BIT);
      nDc     += nVal;
      nDcAc   += nValAc;
      nRms    += (nValAc * nValAc);
      /* Drv_Log("%d : (dc)0x%08lX (ac)0x%08lX\n\r",
               i, (IFX_int32_t)nVal, (IFX_int32_t)nValAc); */
   }
   /* map results in parameter buffer */
   pParam [0] = (nDc / nPeriod);
   pParam [1] = (nDcAc / nPeriod);
   pParam [2] = (nRms / nPeriod);
   pParam [3] = nPeriod;

   TRACE(VINETIC, DBG_LEVEL_NORMAL,
        ("DC part (DC only)\t\t:  %ld (= %ld/%d)\n\r", (IFX_uint32_t)pParam [0],
        (IFX_uint32_t)nDc, nPeriod));
   TRACE(VINETIC, DBG_LEVEL_NORMAL,
        ("DC part (DC and AC)\t\t:  %ld (= %ld/%d)\n\r", (IFX_uint32_t)pParam [1],
        (IFX_uint32_t)nDc, nPeriod));
   TRACE(VINETIC, DBG_LEVEL_NORMAL,
        ("RMS part\t\t:  %ld (= %ld/%d)\n\r", (IFX_uint32_t)pParam [2],
        (IFX_uint32_t)nRms, nPeriod));
}

/*******************************************************************************
Description:
   alert user that data are available or that error occured and restore
   register settings before measurement start
Arguments:
   pCh         - pointer to channel structure
Return:
   none
*******************************************************************************/
IFX_LOCAL IFX_int32_t DoLastStateJob (VINETIC_CHANNEL *pCh)
{
   /* data setup for user task */
   switch (GET_STATE())
   {
   case TEST_OK_STATE:
      /* measurement finished without errors */
      pCh->Gr909.Usr.status = GR909_TEST_OK;
      pCh->Gr909.bData = IFX_TRUE;
      break;
   case TEST_NOTOK_STATE:
      /* test failed */
      pCh->Gr909.bData = IFX_TRUE;
      pCh->Gr909.Usr.status = GR909_TEST_FAILED;
      break;
   case INTERNAL_ERR_STATE:
      /* configuration failed */
      pCh->Gr909.bData = IFX_TRUE;
      pCh->Gr909.Usr.status = GR909_MEASUREMENT_FAILED;
      break;
   default:
      pCh->Gr909.bData = IFX_FALSE;
      break;
   }
   /* error message */
   ErrorInfo (pCh);
#ifdef ENABLE_TRACE
   /* temporarily : print user parameters */
   ParamMonitor (pCh->Gr909.Usr.mode, (IFX_int32_t *)pCh->Gr909.Usr.values);
#endif /* ENABLE_TRACE */
   /* reset callback */
   pCh->Gr909.Gr909Call = NULL;
   /* protect channel from mutual access */
   IFXOS_MutexLock (pCh->chAcc);
   /* wake up user sleeping on GR99 exeption */
   TAPI_Phone_Event_Gr909 (pCh->pTapiCh);
   /* release channel */
   IFXOS_MutexUnlock (pCh->chAcc);
   /* restore values of registers  */
   return GR909_Reg (pCh, RESTORE_REG_CONF);
}


/****************************************************************************
Description :
   checks driver state according to chip version
Arguments   :
   pDev     - pointer to the device structure
Return      :
   IFX_SUCCESS or IFX_ERROR
Remarks     :
   This will be done at first when starting a measurement. If some required
   settings were not done, the measurement will not be started.
****************************************************************************/
IFX_LOCAL IFX_int32_t Gr909_ChipRev_Check (VINETIC_DEVICE *pDev)
{
   /* general driver state for any vinetic chip */
   if ((pDev->nDevState & (DS_FW_DLD | DS_CRAM_DLD)) !=
       (DS_FW_DLD | DS_CRAM_DLD))
   {
      TRACE(VINETIC, DBG_LEVEL_HIGH, ("IFX_ERROR: Firmware and CRAM coefs"
            " must be downloaded before any Gr909 measurement\n\r"));
      return IFX_ERROR;
   }
   switch (pDev->nChipRev)
   {
      case VINETIC_V13:
         if ((pDev->nDevState & DS_PHI_DLD) != DS_PHI_DLD)
         {
            TRACE(VINETIC, DBG_LEVEL_HIGH, ("IFX_ERROR: Phi Download is needer by "
                  "Vinetic 1.3 before processing any Gr909 measurement\n\r"));
            return IFX_ERROR;
         }
         break;
      case VINETIC_V14:
      case VINETIC_V15:
      case VINETIC_V16:
         if ((pDev->nDevState & (DS_DC_DWLD | DS_AC_DWLD)) !=
             (DS_DC_DWLD | DS_AC_DWLD))
         {
            TRACE(VINETIC, DBG_LEVEL_HIGH, ("IFX_ERROR: DC and AC Downloads are "
                  "mandatory\n\rfor Vinetic 1.4, 1.5, 1.6 before processing\n\r"
                  "any Gr909 measurement\n\r"));
            return IFX_ERROR;
         }
         break;
      case VINETIC_V21:
         /* ARC Download must be done in this case */
      default:
         TRACE(VINETIC, DBG_LEVEL_HIGH, ("IFX_ERROR: Vinetic 2.1 or greater "
               "are not yet supporting Gr909 measurements\n\r"));
         return IFX_ERROR;
   }

   return IFX_SUCCESS;
}

/****************************************************************************
Description :
   save/restore register settings before/after measurement
Arguments   :
   pDev     - pointer to the device structure
   nAction  - save / restore register values
Return      :
   IFX_SUCCESS or IFX_ERROR
Remarks     :
   according to nAction, a local function pointer is set to RegRead or
   RegWrite and does the appropriate save/restore action.
***************************************************************************/
IFX_LOCAL IFX_int32_t GR909_Reg (VINETIC_CHANNEL *pCh, IFX_int32_t nAction)
{
   IFX_uint8_t nCh = (pCh->nChannel - 1);
   VINETIC_DEVICE *pDev = pCh->pParent;
   VINETIC_GR909_REG *pReg = &pCh->Gr909.reg;

   IFX_int32_t (* rwReg) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t cmd,
      IFX_uint16_t *pData, IFX_uint8_t count, IFX_uint8_t offset);

   /* read/write line mode and set register acces */
   if (nAction == SAVE_REG_CONF)
   {
      /* check if register contents was saved already */
      if (pReg->bReg == IFX_TRUE)
         return IFX_SUCCESS;
      /* set register access ptr */
      rwReg = RegRead;

      {
      /* read actual linemode */
         IFX_int32_t err =
            RegRead (pDev, (CMD1_IOP|nCh), &pReg->nOPMODE, 1, OPMOD_CUR_IOP);
      /* set and save actual line mode */
      pReg->nOPMODE = ((pReg->nOPMODE & 0x0FF0)|CMD1_WR|CMD1_SC|CMD1_OM|nCh);
      /* avoid new reading of registers */
      pReg->bReg = IFX_TRUE;

         if (err != IFX_SUCCESS)
            return IFX_ERROR;
      }
   }
   else
   {
      /* check if register contents was restored already */
      if (pReg->bReg == IFX_FALSE)
         return IFX_SUCCESS;
      /* set register access ptr */
      rwReg = RegWrite;
      /* restore operation mode without Hirt */
      SC_WRITE (pReg->nOPMODE & ~LINE_HIRT);
      /* avoid new writing of registers */
      pReg->bReg = IFX_FALSE;
   }

   /* save/restore register of general use : BCR2, LMCR, OFR, MRSRS1, MFSRS1 */
   if (nAction == SAVE_REG_CONF)
   {
      /* restore the BCR2 parameter in the G909 data structure. It is copied
         from the cached register of BCR2. The cached register should reflect
         the actual register value in the hardware. */
      pReg->nBCR2 = BCR2.value;
   }
   else
   {
      /* the parameter of the BCR2 register will be written to the register
         cache and then to the BCR2 register itself. */
      BCR2.value = pReg->nBCR2;
      if (wrReg (pCh, BCR2_REG) != IFX_SUCCESS)
         return IFX_ERROR;
   }

   if (rwReg (pDev, (CMD1_SOP | nCh), &pReg->nLMCR, 1, SOP_OFFSET_LMCR)
      != IFX_SUCCESS)
      return IFX_ERROR;

   if (rwReg (pDev, (CMD1_SOP | nCh), &pReg->nOFR,  1, SOP_OFFSET_OFR)
      != IFX_SUCCESS)
      return IFX_ERROR;

   if (rwReg (pDev, (CMD1_IOP | nCh), &pReg->pMSRS [0], 1, MRSRS1)
      != IFX_SUCCESS)
      return IFX_ERROR;

   if (rwReg (pDev, (CMD1_IOP | nCh), &pReg->pMSRS [1], 1, MFSRS1)
      != IFX_SUCCESS)
      return IFX_ERROR;

   /* save register according to measurement */
   switch (pCh->Gr909.Usr.mode)
   {
   case GR909_HAZARDOUS_VOLTAGE:
   case GR909_FOREIGN_FORCE:
      /* IOCTL1 */
      if (rwReg (pDev, (CMD1_SOP | nCh), &pReg->nIOCTL1, 1, SOP_OFFSET_IOCTL1)
         != IFX_SUCCESS)
         return IFX_ERROR;
      break;
   case GR909_RECEIVER_OFF_HOOK:
      /* BCR1, IK1 */
      if (rwReg (pDev, (CMD1_SOP|nCh), &pReg->nBCR1,  1, SOP_OFFSET_BCR1)
         != IFX_SUCCESS)
         return IFX_ERROR;
      if (rwReg (pDev, (CMD1_COP|nCh), &pReg->nIK1,  1, CRAM_IK1)
         != IFX_SUCCESS)
         return IFX_ERROR;
      break;
   case GR909_RESISTIVE_FAULT:
      /* IK1, RTR, */
      if (rwReg (pDev, (CMD1_COP|nCh), &pReg->nIK1,  1, CRAM_IK1)
         != IFX_SUCCESS)
         return IFX_ERROR;
      if (rwReg (pDev, (CMD1_SOP|nCh), &pReg->nRTR,  1, SOP_OFFSET_RTR)
         != IFX_SUCCESS)
         return IFX_ERROR;
      break;
   case GR909_RINGER:
      /* AC RMS, LMDC, RGFR, RTR, RO1 */
      if (rwReg (pDev, (CMD1_COP | nCh), &pReg->nRMS, 1, CRAM_RINGF + 1)
         != IFX_SUCCESS)
         return IFX_ERROR;
      if (rwReg (pDev, (CMD1_SOP|nCh), &pReg->nRTR,  1, SOP_OFFSET_RTR)
         != IFX_SUCCESS)
         return IFX_ERROR;
      if (rwReg (pDev, (CMD1_COP|nCh), &pReg->nRO1, 1, CRAM_RINGO)
         != IFX_SUCCESS)
         return IFX_ERROR;
      if (rwReg (pDev, (CMD1_COP|nCh), &pReg->nRGFR, 1, CRAM_RINGF)
         != IFX_SUCCESS)
         return IFX_ERROR;
      if (rwReg (pDev, (CMD1_COP|nCh), &pReg->nLMDC, 1, CRAM_RAMPF)
         != IFX_SUCCESS)
         return IFX_ERROR;
      break;
   default:
      return IFX_ERROR;
      /*break;*/
   }

   return IFX_SUCCESS;
}

/****************************************************************************
Description :
   Do general registers pre settings
Arguments   :
   pCh - handler to the channel structure
Return      :
   IFX_SUCCESS or IFX_ERROR
Remarks     :
   Before any measurement, a set of registers must be initialized with
   specific values.
****************************************************************************/
IFX_LOCAL IFX_int32_t GR909_Preset (VINETIC_CHANNEL *pCh)
{
   IFX_uint16_t   nReg = 0;
   IFX_uint8_t    nCh = (pCh->nChannel - 1);
   VINETIC_DEVICE *pDev  = pCh->pParent;

   /* disable hook and LM_OK interrupt */
   if (RegModify (pDev, (CMD1_IOP | nCh), MRSRS1, (SRS1_LM_OK | SRS1_HOOK),
      (SRS1_LM_OK | SRS1_HOOK)) != IFX_SUCCESS)
      return IFX_ERROR;

   /* Modify the cached value of BCR2 inside the driver and write the modified
      cached value to the device mailbox  */
   {
      IFX_uint16_t value;

      value = BCR2.value & ~(BCR2_SOFT_DIS | BCR2_REV_POL);
      BCR2.value = value | BCR2_CRAM_EN;
   }

   if (wrReg (pCh, BCR2_REG) != IFX_SUCCESS)
      return IFX_ERROR;

   /* write 0 in OFR register */
   nReg = 0;
   if (RegWrite (pDev, (CMD1_SOP | nCh), &nReg,  1, SOP_OFFSET_OFR)
      != IFX_SUCCESS)
      return IFX_ERROR;

   /* unset DC Hold */
   if (RegModify (pDev, (CMD1_SOP | nCh), SOP_OFFSET_BCR1, BCR1_DC_HOLD, 0)
      != IFX_SUCCESS)
      return IFX_ERROR;

   /* unset LM_EN , AD16 Gain , LM_ONCE : continuous measurement */
   if (RegModify (pDev, (CMD1_SOP | nCh), SOP_OFFSET_LMCR,
      (LMCR_LM_EN | LMCR_LM_ONCE | LMCR_DC_AD16), 0)
      != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
}

/****************************************************************************
Description :
   Subfunction for DC prefi offset correction
Arguments   :
   pCh      - pointer to the channel structure
   nSel     - Level meter unit for the offset correction
   pRes     - result to compensate if needed from calling function
Return      :
   IFX_SUCCESS or IFX_ERROR
Remarks     :
   Do Dc prefi offset correction for Foreign voltage
****************************************************************************/
IFX_LOCAL IFX_int32_t DoOffsetCor  (VINETIC_CHANNEL *pCh, IFX_uint16_t nSel,
                                    IFX_uint16_t *pRes)
{
   IFX_int32_t    err = IFX_SUCCESS;
   IFX_uint16_t   nRes = 0, nCor = 0;
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint8_t    nCh = (pCh->nChannel - 1);

   /* set unit to test */
   err = RegModify (pDev, (CMD1_SOP|nCh), SOP_OFFSET_LMCR,
                    LMCR_LM_SEL_MASK, nSel);
   /* Wait mode to take effect */
   IFXOS_DELAYMS (5);
   /* read result */
   if (err == IFX_SUCCESS)
      err = RegRead  (pDev, (CMD1_SOP | nCh), &nRes, 1, SOP_OFFSET_LMRES);
   /* do offset compensation */
   if (err == IFX_SUCCESS)
   {
      /* set offset correction value */
      nCor = (0xFFFF - nRes);
      /* write compensation value to offset register */
      err = RegWrite (pDev, (CMD1_SOP | nCh), &nCor,  1, SOP_OFFSET_OFR);
   }
   if (err == IFX_ERROR)
      SET_STATE (INTERNAL_ERR_STATE);
   else if (pRes != NULL)
      *pRes = nRes;

   TRACE (VINETIC, DBG_LEVEL_NORMAL, ("Offset Correction : OFR = 0x%04X(%d),"
          " RES = 0x%04X(%d)\n\r", nCor, nCor, nRes, nRes));

   return err;
}


/****************************************************************************
Description :
   Integration Measurement result.
Arguments   :
   pCh      - pointer to the channel structure
   pRes     - result ptr
   timeout  - allowed timeout
Return      :
   IFX_SUCCESS or IFX_ERROR
Remarks     :
   This function is usefull only for integration measurements.
   It :
   - enables LM_OK interrupt.
   - enable integration measurement (LMCR [LM_EN] = 1).
   - waits for measurement result and sets back everything.
****************************************************************************/
IFX_LOCAL IFX_int32_t GetIntRes (VINETIC_CHANNEL *pCh, IFX_int16_t *pRes,
                                 IFX_int32_t timeout)
{
   VINETIC_DEVICE *pDev   = pCh->pParent;
   IFX_uint8_t    nCh = (pCh->nChannel - 1);
   IFX_int32_t    err;

   /* set MRSRS1 [LM_OK] = 0 : raising edge interrupt for changes 0->1 */
   err = RegModify (pDev, (CMD1_IOP | nCh), MRSRS1, SRS1_LM_OK, 0);
   /* set LMCR [LM_EN] = 1: integration */
   if (err == IFX_SUCCESS)
      err = RegModify (pDev, (CMD1_SOP | nCh), SOP_OFFSET_LMCR,
                       LMCR_LM_EN, LMCR_LM_EN);
   /* wait for LM_OK interrupt within a time out */
   if (err == IFX_SUCCESS)
   {
      /* Wait at least 1 ms before reading LMRES (non blocking!) */
      IFXOS_DELAYMS (1);
      /* sleep on queue and wait */
      err = IFXOS_WaitEvent_timeout(pCh->wqLT_LM_OK, timeout);
      /* check if event or timeout */
      if (err == IFX_ERROR)
         TRACE(VINETIC, DBG_LEVEL_HIGH, ("IFX_ERROR: Timeout => no SRS1[LM_OK] \n\r"));
   }
   /* read result from LMRES */
   if (err == IFX_SUCCESS)
      err = RegRead (pDev, (CMD1_SOP | nCh), (IFX_uint16_t*)pRes, 1, SOP_OFFSET_LMRES);
   /* set LMCR [LM_EN] = 0: no integration more*/
   if (err == IFX_SUCCESS)
      err = RegModify (pDev, (CMD1_SOP | nCh), SOP_OFFSET_LMCR, LMCR_LM_EN, 0);
   /* set MRSRS1 [LM_OK] = 1 */
   if (err == IFX_SUCCESS)
      err = RegModify (pDev, (CMD1_IOP | nCh), MRSRS1, SRS1_LM_OK, SRS1_LM_OK);

   return err;
}

/****************************************************************************
Description :
   Subfunction for optimum result in integration measurements
Arguments   :
   pCh      - pointer to the channel structure
   pRes     - ptr to result value
Return      :
   IFX_SUCCESS or IFX_ERROR
Remarks     :
   This function sets the correct shift factor according to measured value
   and read the LMRES rtegister
****************************************************************************/
IFX_LOCAL IFX_int32_t GetOptimumRes (VINETIC_CHANNEL *pCh, IFX_int32_t *pRes,
                                     IFX_int32_t  *pShift)
{
   IFX_int32_t    err = IFX_SUCCESS, factor = 1;
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint8_t    nCh = (pCh->nChannel - 1);
   IFX_int16_t    nRes = 0;
   IFX_uint16_t   nShift = 0;
   IFX_int32_t    nMaxRes = 0;
   IFX_boolean_t  bGain = IFX_FALSE;

   /* set default Shift Factor */
   nShift = DEFAULT_SHIFT;
   err = RegWrite (pDev, (CMD1_COP|nCh), (IFX_uint16_t *)&nShift, 1, CRAM_RAMPF);
   /* read Result after Integration */
   if (err == IFX_SUCCESS)
   {
      err = GetIntRes (pCh, &nRes, RES_TIMEOUT);
      /* result absolute value */
      if (nRes < 0)
         factor = -1;
   }
   /* change sample number by adding the gain if result is
      too small to be shifted till overflow */
   if ((err == IFX_SUCCESS) && ((nRes*factor) < (GR909_RES_OVFLOW / DEFAULT_SHIFT)))
   {
      bGain = IFX_TRUE;
      /* Set gain to 16 */
      err = RegModify (pDev, (CMD1_SOP|nCh), SOP_OFFSET_LMCR,
                       LMCR_DC_AD16, LMCR_DC_AD16);
      if (err == IFX_SUCCESS)
         err = GetIntRes (pCh, &nRes, RES_TIMEOUT);
      if (nRes < 0)
         factor = -1;
   }
   /* Calculate shift factor. Shift as long as possible result fits
      into signed 16 bit:
      Estimated inaccuracy of multiplied single measurements: +-4096 */
   if (err == IFX_SUCCESS)
   {
      nMaxRes = (IFX_int32_t)(nRes*factor / DEFAULT_SHIFT);
      if (nMaxRes >= (GR909_RES_OVFLOW - 4096))
      {
         nShift  = 0x1;
         while (nMaxRes >= (GR909_RES_OVFLOW - 4096))
         {
            nMaxRes = (((IFX_uint32_t)nMaxRes) >> 1);
            nShift  <<= 1;
         }

         if (nShift > DEFAULT_SHIFT)
            nShift = DEFAULT_SHIFT;
         /* set shift factor and do measurement */
         err = RegWrite (pDev, (CMD1_COP|nCh), (IFX_uint16_t *)&nShift, 1, CRAM_RAMPF);
         /* read Result after Integration */
         if (err == IFX_SUCCESS)
            err = GetIntRes (pCh, &nRes, RES_TIMEOUT);
      }
   }
   if (err == IFX_SUCCESS)
   {
      if (bGain == IFX_TRUE)
      {
         nRes = (((IFX_uint16_t)nRes) >> 4);
         err = RegModify (pDev, (CMD1_SOP|nCh), SOP_OFFSET_LMCR, LMCR_DC_AD16, 0);
      }
      *pRes   = (IFX_int32_t)nRes;
      *pShift = (IFX_int32_t)nShift;
      TRACE (VINETIC, DBG_LEVEL_NORMAL,
            ("shift = 0x%04X, RESULT = 0x%04X\n\r", nShift, nRes));
   }

   return err;
}

/****************************************************************************
Description :
   get transversal current and dc voltage values
Arguments   :
   pCh      - pointer to the channel structure
   pIt      - buffer for transversal current value
   pVdc     - buffer for dc voltage value
Return      :
   IFX_SUCCESS or IFX_ERROR
Remarks     :
   none
****************************************************************************/
IFX_LOCAL IFX_int32_t GetDcVal (VINETIC_CHANNEL *pCh, IFX_int32_t *pIt,
                                IFX_int32_t *pVdc)
{
   IFX_int32_t     err = IFX_SUCCESS;
   IFX_uint8_t     nCh = (pCh->nChannel - 1);
   VINETIC_DEVICE  *pDev = pCh->pParent;
   IFX_int16_t     nRes = 0;
   IFX_int32_t     nDummy;


   /* check if ptrs are valid */
   if (pIt == NULL)
      pIt = &nDummy;
   if (pVdc == NULL)
      pVdc = &nDummy;

   /* Set It Mode */
   err = RegModify (pDev, (CMD1_SOP | nCh), SOP_OFFSET_LMCR,
                    LMCR_LM_SEL_MASK, LMCR_LM_SEL_I_DC_IT);
   /* read It result */
   if (err == IFX_SUCCESS)
   {
      /* wait AT LEAST 80 ms to allow line to settle
         and pick out suitable sample */
      IFXOS_DELAYMS (100);
      err = RegRead  (pDev, (CMD1_SOP | nCh), (IFX_uint16_t *)&nRes, 1, SOP_OFFSET_LMRES);
      *pIt = nRes;
   }
   /* set unit = DCN-DCP */
   if (err == IFX_SUCCESS)
   {
       err = RegModify (pDev, (CMD1_SOP|nCh), SOP_OFFSET_LMCR,
                        LMCR_LM_SEL_MASK, LMCR_LM_SEL_U_DC_DCN_DCP);
       /* wait AT LEAST 1 ms to allow line to settle */
      IFXOS_DELAYMS (10);
   }
   /* read DCN-DCP Result */
   if (err == IFX_SUCCESS)
   {
       err = RegRead  (pDev, (CMD1_SOP | nCh), (IFX_uint16_t *)&nRes, 1, SOP_OFFSET_LMRES);
       *pVdc = nRes;
   }

   return err;
}

#ifdef CHECK_EXT_VOLT
/****************************************************************************
Description :
   checks if an external voltage is pending
Arguments   :
   pCh      - pointer to the channel structure
   bExtVolt - IFX_TRUE: external voltage pending
              IFX_FALSE : No external voltage pending
Return      :
   IFX_SUCCESS or IFX_ERROR
Remarks     :
   This routine will be executed during foreing voltage tests if there is no
   ground key event. The check is done as follows:
   1- Ring Offset 1 is set to 20 V
   2- Ring Pause mode is set and 20 ms are waited
   3- Tip/Ring Voltage is measured
   4- DCN-DCP Voltage is measured
   5- The check is done: if both voltages are equal +- 5%, bExtVolt = IFX_TRUE
Note        :
   This solution was not fully evaluated and is available only with compiler
   flag CHECK_EXT_VOLT
****************************************************************************/
IFX_LOCAL IFX_int32_t Check_ExtVolt(VINETIC_CHANNEL *pCh, IFX_boolean_t *bExtVolt)
{
   IFX_int32_t  err   = IFX_SUCCESS, nDc2Input = 0;
   IFX_uint8_t  nCh   = (pCh->nChannel - 1);
   IFX_int16_t  nVdc  = 0, nVinput = 0;
   IFX_uint16_t nCoef = 0;
   VINETIC_DEVICE *pDev = pCh->pParent;

   if (pCh->pParent->nChipRev == VINETIC_V13)
   {
      nDc2Input = DC_TO_INPUT_V13;
   }
   else if ((pCh->pParent->nChipRev == VINETIC_V14) ||
       (pCh->pParent->nChipRev == VINETIC_V15)      ||
       (pCh->pParent->nChipRev == VINETIC_V16))
   {
      nDc2Input = DC_TO_INPUT_V14;
   }
   else
   {
      err = IFX_ERROR;
   }
   if (err == IFX_SUCCESS)
   {
      /* reset flag */
      *bExtVolt = IFX_FALSE;
      /* set DC Ring offset = 20 V */
      nCoef = CRAM_getRingDcHex (pDev, 20 * 1000);
      err = RegWrite (pDev, (CMD1_COP|nCh), &nCoef, 1, CRAM_RINGO);
   }
   /* select Ring Offset = RO1 */
   if (err == IFX_SUCCESS)
   {
      err = RegModify (pDev, (CMD1_SOP|nCh), SOP_OFFSET_RTR,
                       RTR_RING_OFFSET, RTR_RO1);
   }
   if (err == IFX_SUCCESS)
   {
      /* set ring pause mode */
      SC_WRITE (SC_RNG_PAUSE | nCh);
      /* allow line to settle by waiting 20 ms */
      IFXOS_DELAYMS (50);
      /* do offset correction for tip/ring voltage */
      err = DoOffsetCor (pCh, LMCR_LM_SEL_DC_PREFI_OFFSET, NULL);
   }
   if (err == IFX_SUCCESS)
   {
      /* measure Tip/Ring input voltage */
      err = RegModify (pDev, (CMD1_SOP | nCh), SOP_OFFSET_IOCTL1,
                       0xFFFF, (IOCTL1_INEN_IO3 | IOCTL1_INEN_IO4));
   }
   /* set LM-SEL = IO3 */
   if (err == IFX_SUCCESS)
   {
      err = RegModify (pDev, (CMD1_SOP | nCh), SOP_OFFSET_LMCR,
            LMCR_LM_SEL_MASK, LMCR_LM_SEL_U_IO4_IO3);
      /* allow line to settle */
      IFXOS_DELAYMS (20);
   }
   /* read Result */
   if (err == IFX_SUCCESS)
   {
      err = RegRead  (pDev, (CMD1_SOP | nCh), (IFX_uint16_t *)&nVinput, 1,
                      SOP_OFFSET_LMRES);
      /* Drv_Log ("\r\n       TIP/RING Voltage : 0x%08X\r\n", (IFX_int32_t)nVinput);*/
   }
   /* set unit = DCN-DCP */
   if (err == IFX_SUCCESS)
   {
       err = RegModify (pDev, (CMD1_SOP|nCh), SOP_OFFSET_LMCR,
                        LMCR_LM_SEL_MASK, LMCR_LM_SEL_U_DC_DCN_DCP);
       /* wait AT LEAST 1 ms to allow line to settle */
       IFXOS_DELAYMS (20);
   }
   /* read DCN-DCP Result */
   if (err == IFX_SUCCESS)
   {
      err = RegRead  (pDev, (CMD1_SOP | nCh), (IFX_uint16_t *)&nVdc, 1, SOP_OFFSET_LMRES);
      /* Drv_Log ("          DCN-DCP Voltage : 0x%08X\r\n", (IFX_int32_t)nVdc);*/
   }
   if (nVinput != (nVdc*nDc2Input))
   {
      /*Drv_Log ("\n\r-------------->External Voltage"
              " is pending. bExtVolt = IFX_TRUE\r\n");*/
      *bExtVolt = IFX_TRUE;
   }

   return err;
}
#endif /* CHECK_EXT_VOLT */

/****************************************************************************
Description :
   state machine for Hazadeous voltage and FEMF measurements
Arguments   :
   pCh      - pointer to the channel structure
Return      :
   IFX_SUCCESS or IFX_ERROR
Remarks     :
   this machine defines states needed for a Voltage measurement and actions
   to be done for each state.
****************************************************************************/
IFX_LOCAL IFX_int32_t GR909_StateMachine_Voltage (VINETIC_CHANNEL *pCh)
{
   IFX_int32_t     err = IFX_SUCCESS;
   IFX_uint32_t    nextState = 0;
   VINETIC_DEVICE  *pDev = pCh->pParent;
   IFX_uint16_t    nIo = 0, nSel = 0;
   IFX_int32_t     *pParam = (IFX_int32_t *)pCh->Gr909.Usr.values, *pGet = NULL;
   IFX_uint8_t     nCh = (pCh->nChannel - 1);
   IFX_boolean_t   bCommon = IFX_TRUE, bOffComp = IFX_TRUE;
#ifdef CHECK_EXT_VOLT
   IFX_boolean_t	bExtVolt = IFX_FALSE;
#endif /* CHECK_EXT_VOLT */

   TRACE (VINETIC, DBG_LEVEL_NORMAL,
         ("GR909_StateMachine_Voltage. State = %ld\n\r", (IFX_uint32_t)GET_STATE()));

   /* protect channel from mutual access */
   IFXOS_MutexLock (pCh->chAcc);
   /* voltage measurement state machine */
   switch (GET_STATE())
   {
   case VOLT_PRESET:
      Vinetic_IrqLockDevice (pDev);
      /* check ground key, sign of an external voltage */
      if ((pCh->IsrFlag [EV_SRS1] & (SRS1_GNDK | SRS1_GNDKP)) != IFX_FALSE)
      {
#ifdef CHECK_EXT_VOLT
         pCh->IsrFlag [EV_SRS1] &= ~(SRS1_GNDK | SRS1_GNDKP);
#endif /* CHECK_EXT_VOLT */
         err = IFX_SUCCESS;
         Vinetic_IrqUnlockDevice (pDev);
      }
      else
      {
         Vinetic_IrqUnlockDevice (pDev);
#ifdef CHECK_EXT_VOLT
         /* No Ground Key event detected : Check if there is  external voltage */
         err = Check_ExtVolt(pCh, &bExtVolt);
         if ((err == IFX_SUCCESS) && (bExtVolt == IFX_FALSE))
         {
            SET_STATE (TEST_OK_STATE);
            bCommon = IFX_FALSE;
            break;
         }
         /* Offset Compensation was done already while checking the voltages */
         bOffComp = IFX_FALSE;
#else  /* CHECK_EXT_VOLT */
         SET_STATE (TEST_OK_STATE);
         bCommon = IFX_FALSE;
         break;
#endif /* CHECK_EXT_VOLT */
      }
#ifdef CHECK_EXT_VOLT
      if (err == IFX_SUCCESS)
#endif /* CHECK_EXT_VOLT */
      {
         /* set line to Hirt */
         SC_WRITE(SC_ACT_HIRT | nCh);
         IFXOS_DELAYMS (5);
         /* Do DC offset Correction if not done before */
         if (bOffComp == IFX_TRUE)
            err = DoOffsetCor (pCh, LMCR_LM_SEL_DC_PREFI_OFFSET, NULL);
         /* set scaling factor for AC */
         pParam [HFV_AC_SCALFACT] = SCALING_FACTOR_32BIT;
         /* set next state */
         nextState = VOLT_RING_GND;
      }
      break;

   case VOLT_RING_GND:
      /* set IO = IO3 */
      nIo = IOCTL1_INEN_IO3;
      /* set LM-SEL = IO3 */
      nSel = LMCR_LM_SEL_U_IO3;
      /* set read ptr */
      pGet = &pParam [HFV_RG_DC];
      /* set next state */
      nextState = VOLT_TIP_GND;
      break;
   case VOLT_TIP_GND:
      /* set IO = IO4 */
      nIo = IOCTL1_INEN_IO4;
      /* set LM-SEL = IO3 */
      nSel = LMCR_LM_SEL_U_IO4;
      /* set read ptr */
      pGet = &pParam [HFV_TG_DC];
      /* set next state */
      nextState = VOLT_TIP_RNG;
      break;
   case VOLT_TIP_RNG:
      /* set IO = IO3 & IO4 */
      nIo = (IOCTL1_INEN_IO3 | IOCTL1_INEN_IO4);
      /* set LM-SEL = IO4 - IO3 */
      nSel = LMCR_LM_SEL_U_IO4_IO3;
      /* set read ptr */
      pGet = &pParam [HFV_TR_DC];
      /* set next state */
      nextState = TEST_OK_STATE;
      break;
   default:
      /* release channel */
      IFXOS_MutexUnlock (pCh->chAcc);
      /* end of measurements . alert user and restore values
         of registers used for Voltage measurements */
      return DoLastStateJob (pCh);
   }

   if (GET_STATE() == VOLT_PRESET)
      SET_STATE (nextState);
   /* do common job here */
   else if ((err == IFX_SUCCESS) &&  (bCommon == IFX_TRUE))
   {
      err = RegModify (pDev, (CMD1_SOP | nCh), SOP_OFFSET_IOCTL1, 0xFFFF, nIo);
      /* set LM-SEL = IO3 */
      if (err == IFX_SUCCESS)
      {
         err = RegModify (pDev, (CMD1_SOP | nCh), SOP_OFFSET_LMCR,
                          LMCR_LM_SEL_MASK, nSel);
         /* allow line to settle */
         IFXOS_DELAYMS (5);
      }
      /* read samples and calculate DC, RMS value and Period */
      if (err == IFX_SUCCESS)
         Calc_VoltParam (pCh, Samples [nCh], pGet);
      /* next state  */
      SET_STATE (nextState);
      if ((GET_STATE() == VOLT_TIP_RNG) &&
          ((IFX_uint32_t)pParam [HFV_TR_AC] > 0x7FFFFFFF))
      {
         SET_STATE (TEST_NOTOK_STATE);
      }
   }
   /* handle error case */
   if (err == IFX_ERROR)
      SET_STATE (INTERNAL_ERR_STATE);
   /* release channel */
   IFXOS_MutexUnlock (pCh->chAcc);
   /* wake up for next state processing */
   IFXOS_WakeUpEvent (pCh->Gr909.wqState);

   return err;
}

/****************************************************************************
Description :
   state machine for resistive faults measurements
Arguments   :
   pCh      - pointer to the channel structure
Return      :
   IFX_SUCCESS or IFX_ERROR
Remarks     :
   this machine defines states needed for a resistive fault measurement and
   actions to be done for each state.
****************************************************************************/
IFX_LOCAL IFX_int32_t GR909_StateMachine_ResFault (VINETIC_CHANNEL *pCh)
{
   IFX_int32_t    err = IFX_SUCCESS, nIndex = 0;
   IFX_uint32_t   nextState = 0;
   IFX_int32_t    *pParam = (IFX_int32_t *)pCh->Gr909.Usr.values/*, *pGet = NULL*/;
   IFX_uint8_t    nCh = (pCh->nChannel - 1);
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint16_t   nCoef = 0;

   TRACE (VINETIC, DBG_LEVEL_NORMAL, ("GR909_StateMachine_ResFault :"
          " State = %ld\n\r", (IFX_uint32_t)GET_STATE()));
   /* protect channel from mutual access */
   IFXOS_MutexLock (pCh->chAcc);
   /* resistive fault state machine */
   switch (GET_STATE())
   {
   case RESFAULT_PRESET:
      /* set active Hirt for Offset Correction */
      SC_WRITE(SC_ACT_HIRT | nCh);
      /* do offset correction */
      err = DoOffsetCor  (pCh, LMCR_LM_SEL_I_DC_IT, NULL);
      /* set IK1 = 30 mA for Tip/Ring  */
      nCoef = CRAM_getIk1Hex (pDev, IK_30MA);

      if (err != IFX_SUCCESS)
         break;

      err =  RegWrite (pDev, (CMD1_COP | nCh), &nCoef, 1, CRAM_IK1);
      if (err != IFX_SUCCESS)
         break;

      /* select ring Offset 0 */
      err = RegModify (pDev, (CMD1_SOP | nCh), SOP_OFFSET_RTR,
                       RTR_RING_OFFSET, RTR_RO0);
      if (err != IFX_SUCCESS)
         break;

      /* set soft reverse polarity
         Modify the cached value inside the driver and write the modified
         cached value to the device mailbox */
      BCR2.value |= BCR2_SOFT_DIS;
      err = wrReg (pCh, BCR2_REG);

      /* set next state */
      nextState = RESFAULT_TIP_RNG;
      break;
   case RESFAULT_TIP_RNG:
      /* set line active boost */
      SC_WRITE(SC_ACT_BOOST | nCh);
      /* set ptr index */
      nIndex = RF_IT_TR_NP;
      /*pGet = &pParam [RF_IT_TR_NP];*/
      /* set next state */
      nextState = RESFAULT_TIP_GND;
      break;
   case RESFAULT_TIP_GND:
      /* set active HIR */
      SC_WRITE (SC_ACT_HIR | nCh);
      /* set IK1 = 15 mA for Tip/Ring  */
      nCoef = CRAM_getIk1Hex (pDev, IK_15MA);
      err =  RegWrite (pDev, (CMD1_COP | nCh), &nCoef, 1, CRAM_IK1);
      /* set ptr index */
      nIndex = RF_IT_TG_NP;
      /*pGet = &pParam [RF_IT_TG_NP];*/
      /* set next state */
      nextState = RESFAULT_RING_GND;
      break;
   case RESFAULT_RING_GND:
      /* set active HIT */
      SC_WRITE (SC_ACT_HIT | nCh);
      /* set ptr index */
      nIndex = RF_IT_RG_NP;
      /*pGet = &pParam [RF_IT_RG_NP];*/
      /* set next state */
      nextState = TEST_OK_STATE;
      break;
   default:
      /* release channel */
      IFXOS_MutexUnlock (pCh->chAcc);
      /* end of measurements . alert user and restore values
         of registers used for this measurements */
      return DoLastStateJob (pCh);
   }
   if (GET_STATE() == RESFAULT_PRESET)
      SET_STATE (nextState);
   /* do common job here */
   else if (err == IFX_SUCCESS)
   {
      /* allow line to settle */
      IFXOS_DELAYMS (30);
      /* set normal polarity */

      /* Modify the cached value inside the driver and write the modified
         cached value to the device mailbox */
      BCR2.value &= ~BCR2_REV_POL;
      err = wrReg (pCh, BCR2_REG);

      /* switch time */
      IFXOS_DELAYMS (5);
      /* get It and Vdcn-dcp in normal polarity */
      if (err == IFX_SUCCESS)
         err = GetDcVal (pCh, &pParam [nIndex], &pParam [nIndex + 1]);

      /* set reverse polarity */
      if (err == IFX_SUCCESS)
      {
         /* Modify the cached value inside the driver and write the modified
            cached value to the device mailbox */
         BCR2.value |= BCR2_REV_POL;
         err = wrReg (pCh, BCR2_REG);

         /* Wait some time */
         IFXOS_DELAYMS (15);
      }

      /* get It and Vdcn-dcp in reverse polarity */
      if (err == IFX_SUCCESS)
         err = GetDcVal (pCh, &pParam [nIndex + 2], &pParam [nIndex + 3]);
      /* set next state */
      SET_STATE (nextState);
   }
   /* handle error case */
   if (err == IFX_ERROR)
      SET_STATE (INTERNAL_ERR_STATE);
   /* release channel */
   IFXOS_MutexUnlock (pCh->chAcc);
   /* wake up for next state processing */
   IFXOS_WakeUpEvent (pCh->Gr909.wqState);

   return err;
}

/****************************************************************************
Description :
   state machine for receiver offhook measurements
Arguments   :
   pCh      - pointer to the channel structure
Return      :
   IFX_SUCCESS or IFX_ERROR
Remarks     :
   this machine defines states needed for a receiver offhook measurement and
   actions to be done for each state.
   It and Vdc are measured for 15 mA and 30 mA.
****************************************************************************/
IFX_LOCAL IFX_int32_t GR909_StateMachine_RecOffHook (VINETIC_CHANNEL *pCh)
{
   IFX_int32_t     err = IFX_SUCCESS;
   IFX_uint8_t     nCh = (pCh->nChannel - 1);
   VINETIC_DEVICE  *pDev = pCh->pParent;
   IFX_int32_t     *pParam = (IFX_int32_t *)pCh->Gr909.Usr.values,
                   nLmVal = 0;
   IFX_uint16_t    nCoef = 0;

   TRACE (VINETIC, DBG_LEVEL_NORMAL, ("GR909_StateMachine_RecOffHook."
          " State = %ld\n\r", (IFX_uint32_t)GET_STATE()));
   /* protect channel from mutual access */
   IFXOS_MutexLock (pCh->chAcc);
   /* Receiver Ofhook measurement state machine */
   switch (GET_STATE())
   {
   case RECOFFHOOK_PRESET:
      /* set active mode */
      SC_WRITE(pDev->nActiveMode | nCh);
      /* set IK1 = 15 mA, measure It */
      nCoef = CRAM_getIk1Hex (pDev, IK_15MA);
      err =  RegWrite (pDev, (CMD1_COP | nCh), &nCoef, 1, CRAM_IK1);
      if (err == IFX_ERROR)
      {
         SET_STATE (INTERNAL_ERR_STATE);
      }
      else
         SET_STATE (RECOFFHOOK_WITH15MA);
      break;
   case RECOFFHOOK_WITH15MA:
      /* Get It and Vdc for 15 mA */
      err = GetDcVal (pCh, &pParam [RO_15MA_IT], &pParam [RO_15MA_VOLT]);
      /* compare with treshold = 10 mA */
      if (pCh->pParent->nChipRev == VINETIC_V13)
      {
         nLmVal = LMVAL_10MA_V13;
      }
      else if ((pCh->pParent->nChipRev == VINETIC_V14) ||
          (pCh->pParent->nChipRev == VINETIC_V15) ||
          (pCh->pParent->nChipRev == VINETIC_V16) )
      {
         nLmVal = LMVAL_10MA_V14;
      }
      else
      {
         err = IFX_ERROR;
      }
      if ((err == IFX_SUCCESS) && (pParam [RO_15MA_IT] < nLmVal))
      {
         /* IT < 10 mA : phone Onhook. Stop */
         pParam [RO_15MA_IT]   = 0;
         pParam [RO_15MA_VOLT] = 0;
         SET_STATE (TEST_OK_STATE);
         break;
      }
      if (err == IFX_ERROR)
         SET_STATE (INTERNAL_ERR_STATE);
      else
         SET_STATE (RECOFFHOOK_WITH30MA);
      break;
   case RECOFFHOOK_WITH30MA:
      /* set IK1 = 30 mA */
      nCoef = CRAM_getIk1Hex (pDev, IK_30MA);
      err =  RegWrite (pDev, (CMD1_COP | nCh), &nCoef, 1, CRAM_IK1);
      /* Get It and Vdcp-dcn for 30 mA*/
      if (err == IFX_SUCCESS)
         err = GetDcVal (pCh, &pParam [RO_30MA_IT], &pParam [RO_30MA_VOLT]);
      if (err == IFX_ERROR)
         SET_STATE (INTERNAL_ERR_STATE);
      else
         SET_STATE (TEST_OK_STATE);
      break;
   default:
      /* release channel */
      IFXOS_MutexUnlock (pCh->chAcc);
      /* end of measurements . alert user and restore values
         of registers used for this measurements */
      return DoLastStateJob (pCh);
   }
   /* release channel */
   IFXOS_MutexUnlock (pCh->chAcc);
   /* wake up for next state processing */
   IFXOS_WakeUpEvent (pCh->Gr909.wqState);

   return err;
}

/****************************************************************************
Description :
   state machine for ringer tests
Arguments   :
   pCh      - pointer to the channel structure
Return      :
   IFX_SUCCESS or IFX_ERROR
Remarks     :
   this machine defines states needed for a Ringer measurement and actions
   to be done for each state.
****************************************************************************/
IFX_LOCAL IFX_int32_t GR909_StateMachine_Ringer (VINETIC_CHANNEL *pCh)
{
   IFX_int32_t    err = IFX_SUCCESS, nItFactor = 1;
   IFX_int32_t    *pParam = (IFX_int32_t *)pCh->Gr909.Usr.values;
   IFX_int16_t    nRes = 0;
   IFX_uint16_t   nCoef = 0;
   IFX_uint8_t    nCh = (pCh->nChannel - 1);
   VINETIC_DEVICE *pDev = pCh->pParent;

   TRACE (VINETIC, DBG_LEVEL_NORMAL,
         ("GR909_StateMachine_Ringer : State = %ld\n\r", (IFX_uint32_t)GET_STATE()));

   /* protect channel from mutual access */
   IFXOS_MutexLock (pCh->chAcc);
   /* ringer state machine */
   switch (GET_STATE())
   {
   case RINGER_PRESET:
      /* Save AC rms, DC Volt and sample nr used for this measurement */
      pParam [RNG_AC_RMS]  = RINGER_AC_RMS;
      pParam [RNG_DC_VOLT] = RINGER_DC_VOLT;
      pParam [RNG_SAMPLNR] = (FRQ_2_KHZ / RINGER_FREQ);
      /* set LM-ONCE = 1 in LMCR */
      err = RegModify (pDev, (CMD1_SOP|nCh), SOP_OFFSET_LMCR,
                              LMCR_LM_ONCE, LMCR_LM_ONCE);
      /* set ring frequence  for samplenr = 100  in
         case of measurement with integration */
      if (err == IFX_SUCCESS)
      {
         nCoef = CRAM_getRingFreqHex (pDev, RINGER_FREQ * 1000);
         err =  RegWrite (pDev, (CMD1_COP|nCh), &nCoef, 1, CRAM_RINGF);
      }
      /* set appropriate low pass frequency */
      if (err == IFX_SUCCESS)
      {
         nCoef = CRAM_getRingLpFreqHex (pDev, RINGER_FREQ * 1000);
         err =  RegWrite (pDev, (CMD1_COP|nCh), &nCoef, 1, CRAM_RINGF + 2);
      }
      /* set DC Ring offset = 10 V */
      if (err == IFX_SUCCESS)
      {
         /* get hex value of quantum in mVolt */
         nCoef = CRAM_getRingDcHex (pDev, RINGER_DC_VOLT * 1000);
         err = RegWrite (pDev, (CMD1_COP|nCh), &nCoef, 1, CRAM_RINGO);
      }
      /* set Ring Amplitude = 15 Vrms */
      if (err == IFX_SUCCESS)
      {
         nCoef = CRAM_getRingAmpHex (pDev, RINGER_AC_RMS * 1000);
         err = RegWrite (pDev, (CMD1_COP|nCh), &nCoef, 1, (CRAM_RINGF + 1));
      }
      /* select Ring Offset = RO1 */
      if (err == IFX_SUCCESS)
      {
         err = RegModify (pDev, (CMD1_SOP|nCh), SOP_OFFSET_RTR,
                          RTR_RING_OFFSET, RTR_RO1);
      }
      if (err == IFX_SUCCESS)
      {
         /* Wait a little bit */
         IFXOS_DELAYMS (5);
         SET_STATE (RINGER_CURR_DC);
      }
      else
         SET_STATE (INTERNAL_ERR_STATE);
      break;
   case RINGER_CURR_DC:
      /* set ring pause mode */
      SC_WRITE (SC_RNG_PAUSE | nCh);
      /* allow line to settle */
      IFXOS_DELAYMS (50);
      /* Measure IT and do offset correction */
      err = DoOffsetCor  (pCh, LMCR_LM_SEL_I_DC_IT, (IFX_uint16_t *)&nRes);
      /* check IT value and set appropriate IT factor */
      if (pCh->pParent->nChipRev == VINETIC_V13)
      {
         nItFactor = IT_FACTOR_V13;
      }
      else if ((pCh->pParent->nChipRev == VINETIC_V14) ||
          (pCh->pParent->nChipRev == VINETIC_V15) ||
          (pCh->pParent->nChipRev == VINETIC_V16) )
      {
         nItFactor = IT_FACTOR_V14;
      }
      else
      {
         err = IFX_ERROR;
      }
      if ((err == IFX_SUCCESS) && (nRes > (POW_2_15/nItFactor)))
      {
         SET_STATE (TEST_NOTOK_STATE);
         break;
      }
      if (err == IFX_SUCCESS)
         SET_STATE (RINGER_CURR_AC);
      break;
   case RINGER_CURR_AC:
      /* set Ring Mode */
      SC_WRITE (SC_ACT_RNG | nCh);
      /* allow line to settle */
      IFXOS_DELAYMS (50);
      /* Set Rectifier and measure It */
      err = RegModify (pDev, (CMD1_SOP|nCh), SOP_OFFSET_LMCR,
                      (LMCR_LM_ONCE | LMCR_LM_RECT | LMCR_LM_SEL_MASK),
                      (LMCR_LM_ONCE | LMCR_LM_RECT| LMCR_LM_SEL_I_DC_IT));
      if (err == IFX_SUCCESS)
         err = GetOptimumRes (pCh, &pParam [RNG_IT_RESULT], &pParam [RNG_SHIFT]);
      if (err == IFX_SUCCESS)
         SET_STATE (TEST_OK_STATE);
      else
         SET_STATE (INTERNAL_ERR_STATE);
      break;
   default:
      /* release channel */
      IFXOS_MutexUnlock (pCh->chAcc);
      /* end of measurements . alert user and restore values
         of registers used for this measurements */
      return DoLastStateJob (pCh);
   }
   /* release channel */
   IFXOS_MutexUnlock (pCh->chAcc);
   /* wake up for next state processing */
   IFXOS_WakeUpEvent (pCh->Gr909.wqState);

   return err;
}

/* ============================= */
/* Global function definition    */
/* ============================= */

/******************************************************************************/
/**
   read GR909 current results if available
\param
   pCh      - pointer to the channel structure
\param
   pLtest   - pointer to the GR909 IO structure
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   none
*/
/******************************************************************************/
IFX_int32_t VINETIC_GR909_Result (VINETIC_CHANNEL *pCh, VINETIC_IO_GR909 *pResult)
{
   IFX_int32_t err = IFX_SUCCESS;
   VINETIC_DEVICE *pDev  = pCh->pParent;

   /* protect channel from mutual access */
   IFXOS_MutexLock (pCh->chAcc);
   /* no data available */
   if (pCh->Gr909.bData == IFX_TRUE)
   {
      /* copy data to user space if available */
      IFXOS_CPY_KERN2USR (pResult, &pCh->Gr909.Usr, sizeof (VINETIC_IO_GR909));
      /* reset internal GR909 data stuctures */
      resetGr909Struct (&pCh->Gr909);
      /* reset GR909 event flag */
      pCh->pTapiCh->TapiMiscData.nException.Bits.gr909result = IFX_FALSE;
   }
   else
   {
      SET_ERROR (ERR_GR909);
      TRACE (VINETIC, DBG_LEVEL_HIGH, ("IFX_ERROR: No Data for User Space\n\r"));
      err = IFX_ERROR;
   }
   /* release channel */
   IFXOS_MutexUnlock (pCh->chAcc);

   return err;
}

/******************************************************************************/
/**
   Starts a/all GR909 test(s)
\param
   pCh     - pointer to the channel structure
\param
   ptest   - pointer to the GR909 IO structure
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   a gr909 task is created and started here after every check and settings
   are ok. The task implementation is OS-specific.
*/
/******************************************************************************/
IFX_int32_t VINETIC_GR909_Start (VINETIC_CHANNEL *pCh, VINETIC_IO_GR909 *pTest)
{
   IFX_int32_t    err = IFX_SUCCESS;
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint32_t   initState [MAX_GR909_TESTS] = {VOLT_PRESET, VOLT_PRESET,
                                                 RESFAULT_PRESET, RECOFFHOOK_PRESET,
                                                 RINGER_PRESET};

   /* protect channel from mutual access */
   IFXOS_MutexLock (pCh->chAcc);
   /* start measurement if no measurement on going */
   if (pCh->Gr909.Usr.status == GR909_TEST_RUNNING)
   {
      SET_ERROR (ERR_GR909);
      TRACE (VINETIC, DBG_LEVEL_HIGH, ("IFX_ERROR: Measurement ongoing \n\r"));
      err = IFX_ERROR;
   }
   /* check if measurement is allowed */
   if ((err == IFX_SUCCESS) && (pTest->mode > GR909_RINGER))
   {
      SET_ERROR (ERR_FUNC_PARM);
      TRACE (VINETIC, DBG_LEVEL_HIGH,
            ("IFX_ERROR: Measurement mode not allowed\n\r"));
      err = IFX_ERROR;
   }
   if (err == IFX_SUCCESS)
   {
      /* setup chip version */
      err = Gr909_ChipRev_Check (pDev);
      if (err == IFX_SUCCESS)
      {
         /* fill GR909 test structure */
         pCh->Gr909.Usr = *pTest;
         /* save chip version */
         pCh->Gr909.Usr.nChipRev = pDev->nChipRev;
         /* save register contents */
         err = GR909_Reg (pCh, SAVE_REG_CONF);
      }
      /* do first general settings */
      if (err == IFX_SUCCESS)
         err = GR909_Preset (pCh);
      /* call state machine */
      if (err == IFX_SUCCESS)
      {
         /* reset event at start */
         pCh->pTapiCh->TapiMiscData.nException.Bits.gr909result = IFX_FALSE;
         /* set state where to start */
         SET_STATE(initState [pTest->mode]);
#ifndef CHECK_EXT_VOLT
         /* Workaround for foreign voltage: Ground Key Flag will be cleared
            only if the measurement was successfull */
         if ((pTest->bAll == IFX_TRUE)                     &&
             ((pTest->mode != GR909_HAZARDOUS_VOLTAGE) ||
              (pTest->mode != GR909_HAZARDOUS_VOLTAGE)))
         {
            Vinetic_IrqLockDevice (pDev);
            pCh->IsrFlag [EV_SRS1] &= ~(SRS1_GNDK | SRS1_GNDKP);
            Vinetic_IrqUnlockDevice (pDev);
         }
#endif /* CHECK_EXT_VOLT */
         /* set call back if needed */
         pCh->Gr909.Gr909Call = Start_StateMachine [pTest->mode];
         /* start gr909 internal task */
         VINETIC_GR909_OSCreateTask (pCh);
      }
      else
         TRACE (VINETIC, DBG_LEVEL_HIGH,
               ("IFX_ERROR: Initialization for Gr909 failed\n\r"));
   }
   /* release channel */
   IFXOS_MutexUnlock (pCh->chAcc);

   return err;
}

/******************************************************************************/
/**
   stop running GR909 test
\param
   pCh         - pointer to the channel structure
   nStopFlag   - 0 : stop tasks if existing / 1 : clear flags
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   User invokes this function to reset internal states when something wen wrong.
   The task created at start will be deleted here. The implementation of task
   deletion is OS-Specific.
*/
/******************************************************************************/
IFX_void_t VINETIC_GR909_Stop  (VINETIC_CHANNEL *pCh, IFX_int32_t nStopFlag)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t    status [3] = {GR909_TEST_OK, GR909_TEST_FAILED,
                                GR909_MEASUREMENT_FAILED};

   /* protect channel from mutual access */
   IFXOS_MutexLock (pCh->chAcc);
   if (nStopFlag == 0)
   {
      /* set actual state in case of intermediate state */
      if ((pCh->Gr909.nState != TEST_OK_STATE) &&
          (pCh->Gr909.nState != INTERNAL_ERR_STATE))
      {
         pCh->Gr909.nState = TEST_NOTOK_STATE;
      }
      /* set actual status */
      pCh->Gr909.Usr.status = status [pCh->Gr909.nState - TEST_OK_STATE];
      /* reset callback after actual status is changed */
      pCh->Gr909.Gr909Call = NULL;
      /* user can read actual status ... manually*/
      pCh->Gr909.bData = IFX_TRUE;
      /* no event at wakeup */
      pCh->pTapiCh->TapiMiscData.nException.Bits.gr909result = IFX_FALSE;
      /* delete gr909 task */
      VINETIC_GR909_OSDeleteTask (pCh);
   }
   else
   {
      Vinetic_IrqLockDevice (pDev);
      pCh->IsrFlag [EV_SRS1] &= ~(SRS1_GNDK | SRS1_GNDKP);
      Vinetic_IrqUnlockDevice (pDev);
   }
   /* release channel */
   IFXOS_MutexUnlock (pCh->chAcc);
   /* what's the error status ? */
   /* ErrorInfo (pCh); */
}

#endif /* (VIN_CFG_FEATURES & VIN_FEAT_GR909) */

