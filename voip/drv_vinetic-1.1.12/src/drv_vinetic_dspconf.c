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
   Module      : drv_vinetic_dspconf.c
   Description : This file contains the implementations of vinetic dsp
                 related configurations
   Remarks:
      the functions implemented in this module must be protected from calling
      functions against concurrent access or interrupts susceptible to modify
      the contain of cached fw messages. The share variable mutex should be used
      for locking.
*******************************************************************************/

/** \file
   This file contains DSP specific functions like signal detection,
   tone generation.    */

/* ============================= */
/* includes                      */
/* ============================= */
#include "drv_vinetic_api.h"

#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M | VIN_FEAT_VIN_2CPE))

#include "drv_vinetic_dspconf.h"
#include "drv_vinetic_basic.h"

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */
/** Requested gap for tone holding to maximum 1000 ms sec (value / 10)*/
#define TONEHOLDING_RGAP (1000 / 10)
/** Requested gap for CED detection 120 ms sec (value / 10)*/
#define CED_RGAP         (120 / 10)
/** CED detection minimum phase shift for the phase reversal  */
#define CED_PHASE        0x23
/** DIS detection Number of Requested Repetition of the Preamble Sequence */
#define DIS_REPET        0x8
/** Toneholding detection minimum signal level */
#define TONEHOLDING_LEVEL 0x83
/** CED detection minimum signal level */
#define CED_LEVEL         0xCF
/** DIS detection minimum signal level */
#define DIS_LEVEL         0xCF
/** hamming constant for cpt level calculation */
#define CONST_CPT_HAMMING        (  610)
/** blackman constant for cpt level calculation */
#define CONST_CPT_BLACKMAN       (-1580)
/** define which constant is taken for cpt level calculation */
#define CONST_CPT                CONST_CPT_BLACKMAN

/* default detection level tolerance, -20dB */
#define CPTD_DETECTION_LEVEL_TOLERANCE  200
/* allow -42dB total power in a pause step */
#define CPTCOEFF_POW_PAUSE       0x0002
/* allow -10dB maximum frequency power to total power ratio for valid tone
   detection */
#define CPTCOEFF_FP_TP_R_DEFAULT 0x075D
/** minimum time for verification of continuos tone in ms */
#define CPTCOEFF_CNT_MINTIME     1000

/** return the maximum */
#define MAX(a, b) ( (a)>(b) ? (a) : (b) )


/* ============================= */
/*             Enums             */
/* ============================= */

/* ============================= */
/* Local structures              */
/* ============================= */

typedef struct
{
   IFX_uint16_t rising;
   IFX_uint16_t falling;
} VIN_IntMask_t;

/* ============================= */
/* Local function declaration    */
/* ============================= */

IFX_LOCAL IFX_uint8_t get_free_lec_res (VINETIC_DEVICE *pDev);

#ifdef TAPI
IFX_LOCAL IFX_int16_t cpt_calc_freq       (IFX_uint32_t    f);
IFX_LOCAL IFX_int16_t cpt_calc_level      (IFX_int32_t     level);
#if 0
IFX_LOCAL IFX_int16_t cid_calc_level      (IFX_int32_t     level);
#endif /* 0 */
IFX_LOCAL IFX_int32_t Atd_ConfCED         (VINETIC_CHANNEL *pCh,
                                           IFX_uint32_t     nSignal,
                                           IFX_uint8_t     *nAtd,
                                           VIN_IntMask_t   *pAtdIntMask);
IFX_LOCAL IFX_int32_t Atd_ConfToneHolding (VINETIC_CHANNEL *pCh,
                                           IFX_uint32_t     nSignal,
                                           IFX_uint8_t     *nAtd,
                                           VIN_IntMask_t   *pAtdIntMask);
IFX_LOCAL IFX_int32_t Atd_Activate        (VINETIC_CHANNEL *pCh,
                                           IFX_uint32_t     nSignal,
                                           IFX_uint8_t     *nAtd,
                                           IFX_uint16_t    *atdVal,
                                           VIN_IntMask_t   *pAtdIntMask);
IFX_LOCAL IFX_int32_t Utd_SetCoeff        (VINETIC_DEVICE  *pDev,
                                           IFX_uint32_t     freq,
                                           IFX_uint8_t      nUtd);
IFX_LOCAL IFX_int32_t Utd_CheckSignals    (VINETIC_CHANNEL *pCh,
                                           IFX_uint32_t     nSignal,
                                           IFX_uint8_t     *nUtd);
#endif /* TAPI */
IFX_LOCAL IFX_int32_t Atd_SetCoeff         (VINETIC_DEVICE *pDev,
                                            IFX_uint16_t nRgap,
                                            IFX_uint8_t nPhaseRep,
                                            IFX_uint16_t nLevel,
                                            IFX_uint8_t nAtd);

/* ============================= */
/* Local variable definition     */
/* ============================= */


/* ============================= */
/* Local function definitions    */
/* ============================= */

/******************************************************************************/
/**
   Get a free resource from the LEC resource management. It assumes that there
   is any resource available. The driver checks the availability of a
   resource before calling this function.
   The resource gets immediately allocated and reserved inside this function.

   \param
     pDev  -  Device handle
   \return
      Number of the LEC resource
   \remarks
      This function does not have any error handling!
*******************************************************************************/
IFX_LOCAL IFX_uint8_t get_free_lec_res (VINETIC_DEVICE *pDev)
{
   IFX_uint16_t   tmp = pDev->availLecRes;
   IFX_uint8_t    i;

   for (i = 0; i < pDev->nMaxRes; i++)
   {
      if (tmp & 1)
         break;

      tmp = tmp >> 1;
   }

   pDev->availLecRes &= ~((IFX_uint16_t)(1 << i));

   return i;
}

#ifdef TAPI
/**
   function to calculate the cpt frequency coefficients

   \param f - frequency in [Hz]
   \return
      cpt frequency coefficient
   \remarks
      none
*/
IFX_LOCAL IFX_int16_t cpt_calc_freq (IFX_uint32_t f)
{
   IFX_int16_t    coef = 0;

   /*
      The formula to calculate the cpt frequency is
         f_coef = 2 * cos ( 2*pi*f[Hz] / 8000) * 2^14
                = 2^15 * cos (pi*f[Hz] / 4000)

      Symmetrie:
         cos (phi) = sin (pi/2 - phi)
         --> we will use the taylor approximation for sin in the
             interval [pi/4 .. 3/4pi]

      with
              X = pi*f/4000

      Approximation with Taylor for cos in [-pi/4 .. +pi/4]
                = 2^15 * ( 1 - (X^2 /2) + (X^4 /24) )
                = 2^15 - (2^15 * X^2 / 2) + (2^15 * X^4 /24)
                = 2^15 - (2^15 * X^2 / 2) + A

                to ensure that f^4 does not overflow, use f/10
                A = 2^15 * X^4 /24
                  = 2^15 * (pi*f /4000)^4 /24
                  = 2^15 * (pi/4000)^4 * f^4 /24
                  = 2^15 * (pi/400)^4 *(f/10)^4 /24

      Approximation with Taylor for sin in [-pi/4 .. +pi/4]
                = 2^15 * ( X - (X^3 /6) + (X^5 /120) )
                = (2^15 * X) - (2^15 * X^3 /6) + (2^15 * X^5 /120)
                = (2^15 * X) - (2^15 * X^3 /6) + B

                to ensure that f^5 does not overflow, use f/20
                B = 2^15 * X^5 /120
                  = 2^15 * (pi*f /4000)^5 / 120
                  = 2^15 * (pi/4000)^5 * f^5 / 120
                  = 2^15 * (pi*20/4000)^5 * (f/20)^5 /120
                  = 2^15 * (pi/200)^5 * (f/20)^5 /120
   */

   if (f <= 1000)
   {
      /* cos approximation using the taylor algorithm */
      /* this formula covers the cos from 0 to pi/4 */
      coef =   (IFX_int16_t)(  (C2_15
                             - ((f*f*1011)/100000))
                             + ((((f/10)*(f/10)*(f/10)*(f/10)) / 192487)
                             - 1)
                             );
   }
   else if (f <= 2000)
   {
      /* sin approximation using the taylor algorithm */
      /* this formula covers the cos from pi/4 to pi/2 */
      f = 2000 - f;
      coef = (IFX_int16_t)( ((25736 * f)/1000
                           - (f*f*f/377948))
                           + ((f/20)*(f/20)*(f/20)*(f/20)*(f/20)/3829411)
                          );
   }
   else if (f <= 3000)
   {
      /* sin approximation using the taylor algorithm */
      /* this formula covers the cos from pi/2 to 3/4pi */
      f = f - 2000;
      coef = -(IFX_int16_t)(( (((25736 * f)/1000)
                              + ((-((IFX_int32_t)f))*f*f/377948))
                              + ((f/20)*(f/20)*(f/20)*(f/20)*(f/20)/3829411)
                           ));
   }
   else if (f <= 4000)
   {
      /* cos approximation using the taylor algorithm */
      /* this formula covers the cos from 3/4 pi to pi */
      f = 4000 - f;
      coef = -(IFX_int16_t)(( (C2_15
                             - ((f*f*1011)/100000))
                             + ((f/10)*(f/10)*(f/10)*(f/10) / 192487)
                           ));
   }
   else
   {
      IFXOS_ASSERT(IFX_FALSE);
   }

   return coef;
}

/**
   function to calculate the cpt level coefficient

   \param level - level in [0.1 dB]
   \return
      cpt level coefficient
   \remarks
      CONST_CPT has to be set by define to
      CONST_CPT_HAMMING or CONST_CPT_BLACKMAN
*/
IFX_LOCAL IFX_int16_t cpt_calc_level (IFX_int32_t level)
{
   long lvl;
   IFX_uint32_t tenExp;
   IFX_uint32_t shift;
   IFX_int32_t  exp, i;

   /* calculate the desired level in mdB and the exponent */
   lvl = level * 100;
   exp = (IFX_int32_t)(-lvl-CONST_CPT) / 10;

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

   /* go over all elements in the tens array, starting with the
      largest entry and ... */
   for (i=27;i>=0;i--)
   {
      /* ... loop while the current tens[i][0] is part of the remaining exp */
      while (exp >= ((IFX_int32_t)tens[i][0]))
      {
         /* calculate part of the result for tens[i][0], check which accuracy
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
         exp -= ((IFX_int32_t)tens[i][0]);
      }
   }

   /* calculate the coefficient according to the specification... */
   return (IFX_int16_t) ROUND (((C2_14*shift)/(tenExp)));
}

#if 0
/**
   function to calculate the cid level coefficient

   \param level - level in [0.1 dB]
   \return
      cid level coefficient
   \remarks
      used formula:
         coef = 32768 * 10 ** (level in dB/ 20)
*/
/* IFX_LOCAL */IFX_int16_t cid_calc_level (IFX_int32_t level)
{
   long           lvl;
   IFX_uint32_t   shift;
   IFX_uint32_t   exp, tenExp;
   IFX_int32_t    i;

   /* calclulate the desired level in mdB and the exponent */
   lvl = level * 100;
   exp = (IFX_uint32_t)((-lvl) / 20);

   /* set the initial shift factor according to the level */
   if (lvl <= -20000)
      shift  = 1000;
   else if (lvl <= -10000)
      shift  = 10000;
   else
      shift  = 100000;

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

   PRINTF("cid_calc_level: level=%ld, ret=%d/%04X, shift=%ld tenExp=%ld\r\n",
      level, ((IFX_int16_t) ROUND (((C2_15*shift)/(tenExp)))),
            ((IFX_int16_t) ROUND (((C2_15*shift)/(tenExp)))),
            shift, tenExp);
   /* calculate the coefficient according to the specification... */
   return (IFX_int16_t) ROUND (((C2_15*shift)/(tenExp)));
}
#endif

/**
   Helper function to set UTD coefficients

   \param pDev - pointer to the device structure
   \param freq - frequency: 1100 or 1300 Hz
   \param nUtd - number of UTD resource: 0 .. 7
   \return
      - IFX_ERROR if error
      - IFX_SUCCESS if successful
   \remarks
   This function is used in Dsp_AtdConf for CED settings
*/
IFX_LOCAL IFX_int32_t Utd_SetCoeff (VINETIC_DEVICE *pDev,
                                    IFX_uint32_t freq, IFX_uint8_t nUtd)
{
   IFX_uint16_t pCmd[CMD_SIG_UTDCOEFF_LEN + CMD_HEADER_CNT];

   pCmd [0] = CMD1_EOP | nUtd;
   pCmd [1] = ECMD_UTD_COEF;
   /* LEVELS = 0x0112 (-41.56 dB) */
   pCmd [2] = 0x0112;

   /* program CF according to freq */
   switch (freq)
   {
      case  1100:
         pCmd [3] = 0x5321;
      break;
      case  1300:
         pCmd [3] = 0x42E1;
      break;
      default:
         /* or calculate freq: f = 32768 * cos(360 * freq / 8000) */
         SET_ERROR(ERR_FUNC_PARM);
         return IFX_ERROR;
   }
   /* BW = 203.71 Hz */
   pCmd [4] = 0x1300;
   /* DEL and NLEV  */
   pCmd [5] = 0x4104;
   /* LEVELH  = -96 dB */
   pCmd [6] = 0x0000;
   /* AGAP = 0x1 4 ms */
   pCmd [7] = (0x1 << 8) & 0xFF00;
   /* RTIME = 400 ms */
   pCmd [7] |= 0x19;
   /* ABREAK = 0x4 (4 ms) */
   pCmd [8] = (0x4 << 8) & 0xFF00;
   /* RGAP = 0x32 (200 ms) */
   pCmd [8] |= 0x32;

   return CmdWrite (pDev, pCmd, CMD_SIG_UTDCOEFF_LEN);
}

/**
   Helper function to set ATD coefficients

   \param pDev - pointer to the device structure
   \param nRgap - value for RGAP
   \param nPhaseRep value for
           - CED detection minimum phase shift for the phase reversal
           - DIS detection Number of Requested Repetition of the Preamble
             Sequence
   \param nLevel - value for minimum signal level for CED, DIS and TONEHOLDING
   \param nAtd - number of ATD resource: 0 .. 7 to configure
   \return
      - IFX_ERROR if error
      - IFX_SUCCESS if successful
   \remarks
   This function is used in Dsp_AtdConf for CED, DIS and TONEHOLDING settings
*/
IFX_LOCAL IFX_int32_t Atd_SetCoeff (VINETIC_DEVICE *pDev,
                                    IFX_uint16_t nRgap, IFX_uint8_t nPhaseRep,
                                    IFX_uint16_t nLevel, IFX_uint8_t nAtd)
{
   IFX_uint16_t pCmd[CMD_SIG_ATDCOEFF_LEN + CMD_HEADER_CNT];

   pCmd [0] = CMD1_EOP | nAtd;
   pCmd [1] = ECMD_ATD_COEF;

   /* programming default */
   pCmd [2] = 0x0123;
   pCmd [3] = 0x2E00 | nPhaseRep;
   pCmd [4] = 0x4026;
   pCmd [5] = nLevel;
   pCmd [6] = 0x010A;
   pCmd [7] = 0x0100 | nRgap;

   return CmdWrite (pDev, pCmd, CMD_SIG_ATDCOEFF_LEN);
}

/**
   Helper function to set UTD coefficients.

   \param pCh - pointer to the channel structure retrieved from pDev in
                the calling function
   \param nSignal - signal definition from \ref IFX_TAPI_SIG_t related to UTD
   \param nUtd - returns the number of UTD resource that were configured
   \return
      - IFX_ERROR if error
      - IFX_SUCCESS if successful
   \remarks
   This function is used in Dsp_UtdConf for CED settings
*/
IFX_LOCAL IFX_int32_t Utd_CheckSignals (VINETIC_CHANNEL *pCh,
                                        IFX_uint32_t nSignal, IFX_uint8_t* nUtd)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint8_t     ch   = pCh->nChannel - 1,
                    /* signal input source to configure */
                   input;
   IFX_int32_t     err = IFX_SUCCESS;
   /* frequency to program */
   IFX_uint32_t    freq,
                   /* signal to configure for each loop */
                   tmpSig;

   *nUtd = 0;
   /* check signal until no further signal to be detect, maybe zero.
      For each run the current configured signal is removed from nSignal */
   while ((nSignal != 0) && err == IFX_SUCCESS && (*nUtd) < VIN_NUM_UTD)
   {
      tmpSig = 0;
      freq = 1100;
      /* configure both paths per default */
      input = VIN_ECMD_SIGUTD_IS_SIGINBOTH;
      /* check first signal and mask it from the signal given.
         Note: nSignal could contain more signals (also ATDs) to detect.
         The current signal for the UTD is stored in tmpSig */
      if (nSignal & IFX_TAPI_SIG_CNGFAXMASK)
      {
         /* get the current signal */
         if (nSignal & IFX_TAPI_SIG_CNGFAX)
         {
            tmpSig = nSignal & IFX_TAPI_SIG_CNGFAX;
            /* clear the signal */
            nSignal &= ~IFX_TAPI_SIG_CNGFAX;
         }
         else
         {
            /* detect only one specific path. UTD1 becomes transmit (ALI) */
            if (nSignal & IFX_TAPI_SIG_CNGFAXTX)
            {
               /* the last possibility is TX */
               tmpSig = nSignal & IFX_TAPI_SIG_CNGFAXTX;
               /* clear the signal */
               nSignal &= ~IFX_TAPI_SIG_CNGFAXTX;
               input = VIN_ECMD_SIGUTD_IS_SIGINA;
            }
            else
            {
               tmpSig = nSignal & IFX_TAPI_SIG_CNGFAXRX;
               /* clear the signal */
               nSignal &= ~IFX_TAPI_SIG_CNGFAXRX;
               input = VIN_ECMD_SIGUTD_IS_SIGINB;
            }
         }
      }
      else if (nSignal & IFX_TAPI_SIG_CNGMODMASK)
      {
         /* modem CNG signal detection */
         freq = 1300;
         /* get the current signal */
         if (nSignal & IFX_TAPI_SIG_CNGMOD)
         {
            tmpSig = nSignal & IFX_TAPI_SIG_CNGMOD;
            /* clear the signal */
            nSignal &= ~IFX_TAPI_SIG_CNGMOD;
         }
         else
         {
            /* detect only one specific path */
            if (nSignal & IFX_TAPI_SIG_CNGMODTX)
            {
               /* the last possibility is TX */
               tmpSig = nSignal & IFX_TAPI_SIG_CNGMODTX;
               /* clear the signal */
               nSignal &= ~IFX_TAPI_SIG_CNGMODTX;
               input = VIN_ECMD_SIGUTD_IS_SIGINA;
            }
            else
            {
               tmpSig = nSignal & IFX_TAPI_SIG_CNGMODRX;
               /* clear the signal */
               nSignal &= ~IFX_TAPI_SIG_CNGMODRX;
               input = VIN_ECMD_SIGUTD_IS_SIGINB;
            }
         }
      }
      /* set the input value and program the coefficients */
      if (tmpSig != 0)
      {
         /* enable appropriate paths. And check whether UTD is already active.
            When programming coefficients the UTD must be disabled.
            It will be enabled later on by the caller of this function */
         if ((*nUtd) == 0)
         {
            if (pDev->pSigCh[ch].sig_utd1.bit.en == 1)
            {
               pDev->pSigCh[ch].sig_utd1.bit.en = 0;
               err = CmdWrite (pDev, pDev->pSigCh[ch].sig_utd1.value, CMD_SIG_UTD_LEN);
            }
            pDev->pSigCh[ch].sig_utd1.bit.is = input;
         }
#ifndef VIN_2CPE
         else
         {
            if (pDev->pSigCh[ch].sig_utd2.bit.en == 1)
            {
               pDev->pSigCh[ch].sig_utd2.bit.en = 0;
               err = CmdWrite (pDev, pDev->pSigCh[ch].sig_utd2.value, CMD_SIG_UTD_LEN);
            }
            pDev->pSigCh[ch].sig_utd2.bit.is = input;
         }
#endif /* VIN_2CPE */
         /* set the signal type detection for the corresponding UTD */
         pCh->nUtdSig[*nUtd] = tmpSig;

         if ((err == IFX_SUCCESS) &&
             (Utd_SetCoeff (pDev, freq,
                   (IFX_uint8_t)(ch + pDev->nSigCnt * (*nUtd))) != IFX_SUCCESS)
         )
         {
             break;
         }
      }
      (*nUtd)++;
   } /* of while */
   return err;
}

/**
   Helper function to activate ATDs

   \param pCh         - pointer to VINETIC channel structure
   \param nSignal     - signal definition
   \param nAtd        - returns the number of ATDs used
   \param atdVal      - reference value of the ATD configuration from the
                        firmware structure given by the calling function.
                        Will be modified by this function.
   \param pAtdIntMask - Atd mask settings.
                        Will be modified by this function.
   \return
      - IFX_ERROR if error
      - IFX_SUCCESS if successful
   \remarks
   This function is used in Dsp_AtdConf for CED settings
*/
IFX_LOCAL IFX_int32_t Atd_Activate (VINETIC_CHANNEL *pCh,
                                    IFX_uint32_t nSignal, IFX_uint8_t* nAtd,
                                    IFX_uint16_t *atdVal,
                                    VIN_IntMask_t *pAtdIntMask)
{

   if (nSignal & IFX_TAPI_SIG_CEDMASK)
   {
      if (nSignal & IFX_TAPI_SIG_CEDENDMASK)
      {
         /* unmask setting for interrupt both edges */
         if ((*nAtd) == 0)
         {
               pAtdIntMask->rising  |= VIN_ATD1_DT_MASK;
               pAtdIntMask->falling |= VIN_ATD1_DT_MASK;
         }
         else
         {
            pAtdIntMask->rising  |= VIN_ATD2_DT_MASK;
            pAtdIntMask->falling |= VIN_ATD2_DT_MASK;
         }
      }
      else
      {
         /* mask setting for interrupt 1 -> 0 cause no end should
            be reported */
         if ((*nAtd) == 0)
         {
            pAtdIntMask->rising  |= VIN_ATD1_DT_MASK;
            pAtdIntMask->falling &= ~VIN_ATD1_DT_MASK;
         }
         else
         {
            pAtdIntMask->rising  |= VIN_ATD2_DT_MASK;
            pAtdIntMask->falling &= ~VIN_ATD2_DT_MASK;
         }
      }

      /* detection of phase reversals is always done
         For AM and phase reversal: no specific path can be set */
      if (nSignal & IFX_TAPI_SIG_AM)
      {
         /* enable amplitude modulation */
         VIN_ECMD_SIGATD_MD_SET ((*atdVal), VIN_ECMD_SIGATD_MD_ANS_PHREV_AM);
         if ((*nAtd) == 0)
         {
            pAtdIntMask->rising  |= VIN_ATD1_AM_MASK;
         }
         else
         {
            pAtdIntMask->rising  |= VIN_ATD2_AM_MASK;
         }
      }
      else
      {
         VIN_ECMD_SIGATD_MD_SET ((*atdVal), VIN_ECMD_SIGATD_MD_ANS_PHREV);
         if ((*nAtd) == 0)
         {
            pAtdIntMask->rising  &= ~VIN_ATD1_AM_MASK;
         }
         else
         {
            pAtdIntMask->rising  &= ~VIN_ATD2_AM_MASK;
         }
      }
      if (nSignal & IFX_TAPI_SIG_PHASEREV)
      {
         /* enable phase reversal interrupt */
         /* Interrupt will be generated on the first and third occurring phase reversal */
         if ((*nAtd) == 0)
         {
            pAtdIntMask->rising  |= VIN_ATD1_NPR_1_REV_MASK;
         }
         else
         {
            pAtdIntMask->rising  |= VIN_ATD2_NPR_1_REV_MASK;
         }
      }
      else
      {
         /* disable phase reversal interrupt */
         if ((*nAtd) == 0)
         {
            pAtdIntMask->rising  &= ~VIN_ATD1_NPR_MASK;
         }
         else
         {
            pAtdIntMask->rising  &= ~VIN_ATD2_NPR_MASK;
         }
      }
   }
   else
   {
      /* enable CED end only detection */
      VIN_ECMD_SIGATD_MD_SET ((*atdVal), VIN_ECMD_SIGATD_MD_ANS_PHREV);
      /* unmask setting for interrupt 1 -> 0  */
      if ((*nAtd) == 0)
      {
         pAtdIntMask->rising  &= ~VIN_ATD1_DT_MASK;
         pAtdIntMask->falling |= VIN_ATD1_DT_MASK;
      }
      else
      {
         pAtdIntMask->rising  &= ~VIN_ATD2_DT_MASK;
         pAtdIntMask->falling |= VIN_ATD2_DT_MASK;
      }
   }

   return IFX_SUCCESS;
}


/**
   Helper function to set CED and related settings

   \param pCh         - pointer to VINETIC channel structure
   \param nSignal     - signal definition
   \param nAtd        - number of ATDs used
   \param pAtdIntMask - Atd mask settings.
                        Will be modified by this function.
   \return
      - IFX_ERROR if error
      - IFX_SUCCESS if successful
   \remarks
   This function is used in Dsp_AtdConf for CED settings
*/
IFX_LOCAL IFX_int32_t Atd_ConfCED (VINETIC_CHANNEL *pCh,
                                   IFX_uint32_t nSignal, IFX_uint8_t* nAtd,
                                   VIN_IntMask_t *pAtdIntMask)
{
   /* signal to configure for each loop */
   IFX_uint32_t     tmpSig;
   IFX_uint16_t     atdVal;
   IFX_uint8_t      ch   = pCh->nChannel - 1;
   VINETIC_DEVICE  *pDev = pCh->pParent;

   /* check signal until no further signal to be detect, maybe zero.
      For each run the current configured signal is removed from nSignal */
   (*nAtd) = 0;
   while ((nSignal & (IFX_TAPI_SIG_CEDMASK | IFX_TAPI_SIG_CEDENDMASK)) != 0)
   {
      tmpSig = 0;
      if ((*nAtd) == 0)
         atdVal = pDev->pSigCh[ch].sig_atd1.value[CMD_HEADER_CNT];
      else
         atdVal = pDev->pSigCh[ch].sig_atd2.value[CMD_HEADER_CNT];

      if (nSignal & IFX_TAPI_SIG_CED || nSignal & IFX_TAPI_SIG_CEDEND)
      {
         /* enable both paths */
         VIN_ECMD_SIGATD_IS_SET (atdVal,
                                 VIN_ECMD_SIGATD_IS_SIGINBOTH);
         tmpSig = nSignal & (IFX_TAPI_SIG_CED | IFX_TAPI_SIG_CEDEND |
                             IFX_TAPI_SIG_PHASEREVMASK | IFX_TAPI_SIG_AMMASK);
         /* clear the signal */
         nSignal &= ~(IFX_TAPI_SIG_CED | IFX_TAPI_SIG_CEDEND);
      }
      else
      {
         /* enable only one path */
         if (nSignal & (IFX_TAPI_SIG_CEDENDTX | IFX_TAPI_SIG_CEDTX))
         {
            /* for transmit path input A is used. Selection of ATD needed */
            VIN_ECMD_SIGATD_IS_SET (atdVal, VIN_ECMD_SIGATD_IS_SIGINA);
            tmpSig = nSignal & (IFX_TAPI_SIG_CEDTX | IFX_TAPI_SIG_CEDENDTX |
                                IFX_TAPI_SIG_PHASEREVMASK | IFX_TAPI_SIG_AMMASK);
            /* clear the signal */
            nSignal &= ~(IFX_TAPI_SIG_CEDTX | IFX_TAPI_SIG_CEDENDTX);
         }
         else if (nSignal & (IFX_TAPI_SIG_CEDENDRX | IFX_TAPI_SIG_CEDRX))
         {
            /* for receive path input B is used. Only ATD1 is possible */
            VIN_ECMD_SIGATD_IS_SET (atdVal,
                                    VIN_ECMD_SIGATD_IS_SIGINB);
            tmpSig = nSignal & (IFX_TAPI_SIG_CEDRX | IFX_TAPI_SIG_CEDENDRX |
                             IFX_TAPI_SIG_PHASEREVMASK | IFX_TAPI_SIG_AMMASK);
            /* clear the signal */
            nSignal &= ~(IFX_TAPI_SIG_CEDRX | IFX_TAPI_SIG_CEDENDRX);
         }
      }
      Atd_Activate (pCh, tmpSig, nAtd, &atdVal, pAtdIntMask);

      if ((*nAtd) == 0)
         pDev->pSigCh[ch].sig_atd1.value[CMD_HEADER_CNT] = atdVal;
      else
         pDev->pSigCh[ch].sig_atd2.value[CMD_HEADER_CNT] = atdVal;
      /* detection time 210 ms */
      Atd_SetCoeff (pDev, CED_RGAP, CED_PHASE, CED_LEVEL,
                    (IFX_uint8_t)(ch + pDev->nSigCnt * (*nAtd)));
      (*nAtd)++;
   } /* of while */
   return IFX_SUCCESS;
}

/**
   Helper function to set signal level detection

   \param pCh         - pointer to VINETIC channel structure
   \param nSignal     - signal definition
   \param nAtd        - returns the number of the ATD, that were configured
   \param pAtdIntMask - Atd mask settings.
                        Will be modified by this function.
   \return
      - IFX_ERROR if error
      - IFX_SUCCESS if successful
   \remarks
   This function is used in Dsp_AtdConf for signal level, so called
   tone holding detection
*/
IFX_LOCAL IFX_int32_t Atd_ConfToneHolding (VINETIC_CHANNEL *pCh,
                                           IFX_uint32_t nSignal,
                                           IFX_uint8_t* nAtd,
                                           VIN_IntMask_t *pAtdIntMask)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint8_t     ch = pCh->nChannel - 1;
   IFX_uint16_t    atdVal;

   /* check signal until no further signal to be detect, maybe zero.
      For each run the current configured signal is removed from nSignal */
   (*nAtd) = 0;
   while (nSignal != 0)
   {
      if ((*nAtd) == 0)
         atdVal = pDev->pSigCh[ch].sig_atd1.value[CMD_HEADER_CNT];
      else
         atdVal = pDev->pSigCh[ch].sig_atd2.value[CMD_HEADER_CNT];

      VIN_ECMD_SIGATD_MD_SET (atdVal, VIN_ECMD_SIGATD_MD_TONEHOLDING);
      if ((nSignal & IFX_TAPI_SIG_TONEHOLDING_END) != 0)
      {
         VIN_ECMD_SIGATD_IS_SET (atdVal, VIN_ECMD_SIGATD_IS_SIGINBOTH);
         /* unmask setting for interrupt 1 -> 0 only for ATD1 */
         pAtdIntMask->rising  &= ~VIN_ATD1_DT_MASK;
         pAtdIntMask->falling |= VIN_ATD1_DT_MASK;
         /* clear the signal */
         nSignal &= ~(IFX_TAPI_SIG_TONEHOLDING_END);
      }
      else
      {
         /* disable 1 -> 0 interrupts */
         pAtdIntMask->falling &= ~(VIN_ATD1_DT_MASK | VIN_ATD2_DT_MASK);
         if (nSignal & IFX_TAPI_SIG_TONEHOLDING_ENDRX)
         {
            /* for receive path input B is used */
            VIN_ECMD_SIGATD_IS_SET (atdVal, VIN_ECMD_SIGATD_IS_SIGINB);
            /* enable 1 -> 0 interrupt */
            pAtdIntMask->falling |= VIN_ATD1_DT_MASK;
            /* clear the signal */
            nSignal &= ~(IFX_TAPI_SIG_TONEHOLDING_ENDRX);
         }
         else if (nSignal & IFX_TAPI_SIG_TONEHOLDING_ENDTX)
         {
            /* for transmit path input A is used */
            VIN_ECMD_SIGATD_IS_SET (atdVal, VIN_ECMD_SIGATD_IS_SIGINA);
            if (*nAtd == 0)
            {
               /* enable 1 -> 0 interrupt */
               pAtdIntMask->falling |= VIN_ATD1_DT_MASK;
            }
            else
            {
               /* enable 1 -> 0 interrupt */
               pAtdIntMask->falling |= VIN_ATD2_DT_MASK;
            }
            /* clear the signal */
            nSignal &= ~(IFX_TAPI_SIG_TONEHOLDING_ENDTX);
         }
      }
      if ((*nAtd) == 0)
         pDev->pSigCh[ch].sig_atd1.value[CMD_HEADER_CNT] = atdVal;
      else
         pDev->pSigCh[ch].sig_atd2.value[CMD_HEADER_CNT] = atdVal;
      /* detection time 1000 ms */
      Atd_SetCoeff (pDev, TONEHOLDING_RGAP, CED_PHASE, TONEHOLDING_LEVEL,
                    (IFX_uint8_t)(ch + pDev->nSigCnt * (*nAtd)));

      (*nAtd)++;
   }
   return IFX_SUCCESS;
}
#endif /* TAPI */

/* ============================= */
/* Global function definitions   */
/* ============================= */

/**
   Disables or Enables DSP Idle mode

   \param   pDev  - pointer to VINETIC device structure
   \param   bEn   - IFX_TRUE : enable / IFX_FALSE : disable

   \return
      IFX_SUCCESS or IFX_ERROR

*/
IFX_int32_t Dsp_SetIdleMode (VINETIC_DEVICE *pDev, IFX_boolean_t bEn)
{
   IFX_int32_t    ret;
   IFX_int16_t    pCmd[3] =  {0x0600, 0xE501, 0x0000};

   /* switch Idle Mode on or off */
   pCmd[2] = (bEn) ? 0x8000 : 0x0000;
   ret = CmdWrite (pDev, &pCmd[0], 1);
   return ret;
}


/**
   Disables or Enables CID Sender according to bEn

   \param   pCh  - pointer to VINETIC channel structure
   \param   bEn  - IFX_TRUE : enable / IFX_FALSE : disable

   \return
      IFX_SUCCESS or IFX_ERROR

*/
IFX_int32_t Dsp_SetCidSender (VINETIC_CHANNEL *pCh, IFX_boolean_t bEn)
{
   IFX_int32_t    ret   = IFX_SUCCESS;
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint8_t    ch    = (pCh->nChannel - 1);

   pDev->pSigCh[ch].cid_sender[0] &= ~CMD1_RD;
   if ((bEn == IFX_TRUE) && !(pDev->pSigCh[ch].cid_sender[2] & SIG_CID_EN))
   {
      pDev->pSigCh[ch].cid_sender[2] |= SIG_CID_EN;
      ret = CmdWrite (pDev, pDev->pSigCh[ch].cid_sender, 1);
   }
   else if ((bEn == IFX_FALSE) && (pDev->pSigCh[ch].cid_sender[2] & SIG_CID_EN))
   {
      pDev->pSigCh[ch].cid_sender[2] &= ~SIG_CID_EN;
      ret = CmdWrite (pDev, pDev->pSigCh[ch].cid_sender, 1);
   }

   return ret;
}


/**
   Disables or Enables Dtmf receiver according to bEn

   \param pCh  - pointer to VINETIC channel structure
   \param bEn  - IFX_TRUE : enable / IFX_FALSE : disable

   \return
      IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t Dsp_SetDtmfRec (VINETIC_CHANNEL *pCh, IFX_boolean_t bEn)
{
   IFX_int32_t ret = IFX_SUCCESS;
   IFX_uint16_t pCmd [2] = {0}, pData [3] = {0};
   VINETIC_DEVICE *pDev = pCh->pParent;

   /* EOP Cmd */
   pCmd [0] = (CMD1_EOP | (pCh->nChannel - 1));
   /* DTMF REC */
   pCmd [1] = ECMD_DTMF_REC;
   ret = CmdRead (pDev, pCmd, pData, 1);
   if (ret == IFX_SUCCESS)
   {
      if ((bEn == IFX_TRUE) && !(pData [2] & SIG_DTMFREC_EN))
      {
         pData [2] |= SIG_DTMFREC_EN;
         ret = CmdWrite (pDev, pData, 1);
      }
      else if ((bEn == IFX_FALSE) && (pData [2] & SIG_DTMFREC_EN))
      {
         pData [2] &= ~SIG_DTMFREC_EN;
         ret = CmdWrite (pDev, pData, 1);
      }
   }

   return ret;
}


/**
   Enable/Disable ATD1, ATD2 and UTD1 according to bEn

  \param pCh  - pointer to VINETIC channel structure
  \param bEn  - IFX_TRUE : enable / IFX_FALSE : disable Tone Detectors
   \return
   IFX_SUCCESS or IFX_ERROR
   \remarks
   If bEn is IFX_FALSE
   -  The function calling this function have to check if nFaxConnect is READY
   -  This function must be called after either an SRE1 [UTD1_OK] or
      SRE2 [ATD1_DT] or SRE2 [ATD2_DT] interrupt occurs.
   If bEn is IFX_TRUE
   -  The function is called by TAPI_LL_FaxT38_DisableDataPump to make sure
      that a next fax connection can take place after disabling the datapump
*/
IFX_int32_t Dsp_SetToneDetectors (VINETIC_CHANNEL *pCh, IFX_boolean_t bEn)
{
   IFX_int32_t   ret = IFX_SUCCESS;
   IFX_uint8_t   ch = pCh->nChannel - 1;
   VINETIC_DEVICE *pDev    = pCh->pParent;

   if (bEn == IFX_FALSE)
   {
      if (pDev->pSigCh[ch].sig_atd1.bit.en == 1)
      {
         pDev->pSigCh[ch].sig_atd1.bit.en = 0;
         ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_atd1.value, CMD_SIG_ATD_LEN);
      }
      if (ret == IFX_SUCCESS && pDev->pSigCh[ch].sig_atd2.bit.en == 1)
      {
         pDev->pSigCh[ch].sig_atd2.bit.en = 0;
         ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_atd2.value, CMD_SIG_ATD_LEN);
      }
      if (ret == IFX_SUCCESS && pDev->pSigCh[ch].sig_utd1.bit.en == 1)
      {
         pDev->pSigCh[ch].sig_utd1.bit.en = 0;
         ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_utd1.value, CMD_SIG_UTD_LEN);
      }
#ifndef VIN_2CPE
      if (ret == IFX_SUCCESS && pDev->pSigCh[ch].sig_utd2.bit.en == 1)
      {
         pDev->pSigCh[ch].sig_utd2.bit.en = 0;
         ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_utd2.value, CMD_SIG_UTD_LEN);
      }
#endif /* VIN_2CPE */
   }
   else
   {
      if (pDev->pSigCh[ch].sig_atd1.bit.en == 0)
      {
         pDev->pSigCh[ch].sig_atd1.bit.en = 1;
         ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_atd1.value, CMD_SIG_ATD_LEN);
      }
      if (ret == IFX_SUCCESS && pDev->pSigCh[ch].sig_atd2.bit.en == 0)
      {
         pDev->pSigCh[ch].sig_atd2.bit.en = 1;
         ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_atd2.value, CMD_SIG_ATD_LEN);
      }
      if (ret == IFX_SUCCESS && pDev->pSigCh[ch].sig_utd1.bit.en == 0)
      {
         pDev->pSigCh[ch].sig_utd1.bit.en = 1;
         ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_utd1.value, CMD_SIG_UTD_LEN);
      }
#ifndef VIN_2CPE
      if (ret == IFX_SUCCESS && pDev->pSigCh[ch].sig_utd2.bit.en == 0)
      {
         pDev->pSigCh[ch].sig_utd2.bit.en = 1;
         ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_utd2.value, CMD_SIG_UTD_LEN);
      }
#endif /* VIN_2CPE */
   }
   return ret;
}

#ifdef TAPI
/**
   Enables signal detection on universal tone detector

   \param pCh  - pointer to VINETIC channel structure
   \param nSignal - signal definition from \ref IFX_TAPI_SIG_t
   \return
      IFX_SUCCESS or IFX_ERROR
   \remarks
      Checks are done if configuration is valid
*/
IFX_int32_t Dsp_UtdSigEnable (VINETIC_CHANNEL *pCh, IFX_uint32_t nSignal)
{
   IFX_int32_t  err = IFX_SUCCESS;
   IFX_uint32_t sigMask;

   sigMask = (nSignal | pCh->pTapiCh->TapiMiscData.sigMask) &
             (IFX_TAPI_SIG_CNGFAXMASK | IFX_TAPI_SIG_CNGMODMASK);

   err = Dsp_UtdConf(pCh, sigMask);
   if (err == IFX_SUCCESS)
   {
      pCh->pTapiCh->TapiMiscData.sigMask |= nSignal;
   }

   return err;
}

/**
   Configuration of the universal tone detector

  \param pCh  - pointer to VINETIC channel structure
  \param nSignal - signal to detect. combination of IFX_TAPI_SIG_CNGFAX,
                  IFX_TAPI_SIG_CNGFAXRX, IFX_TAPI_SIG_CNGFAXTX,
                  IFX_TAPI_SIG_CNGMOD, IFX_TAPI_SIG_CNGMODRX and
                  IFX_TAPI_SIG_CNGMODTX
   \return
      IFX_SUCCESS or IFX_ERROR
   \remarks
      coefficients are fixed to
       - Bandwidth 203.71 Hz
       - Levels -41.56 dB
       - detection time 400 ms
       - universal tone detection always
*/
IFX_int32_t Dsp_UtdConf (VINETIC_CHANNEL *pCh, IFX_uint32_t nSignal)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t    err = IFX_SUCCESS;
   IFX_uint8_t    nUtd;
   IFX_uint8_t    ch = (pCh->nChannel - 1);
   IFX_uint16_t   nRising = 0;

   /* check the signals for both UTDs. The number of UTDs to configure
      is returned in nUtd  */
   err = Utd_CheckSignals (pCh, nSignal, &nUtd);
   /* program enable bits and interrupts. input signals are
      already programmed in the while loop */
   switch (nUtd)
   {
      case  0:
         /* Mask UTD interrupts */
         nRising = 0;
         /* no UTD resource used */
         pDev->pSigCh[ch].sig_utd1.bit.en = 0;
#ifndef VIN_2CPE
         pDev->pSigCh[ch].sig_utd2.bit.en = 0;
#endif /* VIN_2CPE */
         break;
      case  1:
         /* unmask UTD1 interrupt 0 -> 1  */
         nRising |= VIN_UTD1_OK_MASK;
         /* first UTD used */
         pDev->pSigCh[ch].sig_utd1.bit.en = 1;
#ifndef VIN_2CPE
         pDev->pSigCh[ch].sig_utd2.bit.en = 0;
#endif /* VIN_2CPE */
         break;
      default:
         /* all UTDs are used : unmask interrupt 0 -> 1 for all utds */
         nRising |= VIN_UTD1_OK_MASK;
         /*  */
         pDev->pSigCh[ch].sig_utd1.bit.en = 1;
#ifndef VIN_2CPE
         pDev->pSigCh[ch].sig_utd2.bit.en = 1;
         nRising |= VIN_UTD2_OK_MASK;
#endif /* VIN_2CPE */
         break;
   }
   if (err == IFX_SUCCESS)
      err = VINETIC_Host_Set_EdspIntMask (pCh, VIN_EDSP_UTD, IFX_FALSE,
                                          nRising, 0);
   if (err == IFX_SUCCESS)
      err = CmdWrite (pDev, pDev->pSigCh[ch].sig_utd1.value, CMD_SIG_UTD_LEN);
#ifndef VIN_2CPE
   if (err == IFX_SUCCESS)
      err = CmdWrite (pDev, pDev->pSigCh[ch].sig_utd2.value, CMD_SIG_UTD_LEN);
#endif /* VIN_2CPE */

   return err;
}

/**
   Enables signal detection on answering tone detector

   \param pCh  - pointer to VINETIC channel structure
   \param nSignal - signal definition
   \return
      IFX_SUCCESS or IFX_ERROR
   \remarks
      Checks are done if configuration is valid
*/
IFX_int32_t Dsp_AtdSigEnable (VINETIC_CHANNEL *pCh, IFX_uint32_t nSignal)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t err = IFX_SUCCESS;
   IFX_uint32_t sigMask;

   sigMask = pCh->pTapiCh->TapiMiscData.sigMask;

   /* set temporary mask for sanity checks */
   sigMask |= nSignal;
   /* check if CED receive and other transmit should be set */
   if ( (sigMask & (IFX_TAPI_SIG_CEDRX | IFX_TAPI_SIG_AMRX |
                    IFX_TAPI_SIG_PHASEREVRX | IFX_TAPI_SIG_CEDENDRX)) &&
        (sigMask & (IFX_TAPI_SIG_CEDTX | IFX_TAPI_SIG_AMTX |
                    IFX_TAPI_SIG_PHASEREVTX | IFX_TAPI_SIG_CEDENDTX)) )
   {
      SET_ERROR (ERR_NOTSUPPORTED);
      return IFX_ERROR;
   }

   /* check if tone holding or CED is to be enabled */
   if ((sigMask & IFX_TAPI_SIG_TONEHOLDING_ENDMASK) &&
       (sigMask & IFX_TAPI_SIG_CEDMASK))
   {
      /* resource is in use */
      SET_ERROR (ERR_NORESOURCE);
      return IFX_ERROR;
   }
   /* check ATD2 configuration **********************************************/

   err = Dsp_AtdConf(pCh, sigMask);
   if (err == IFX_SUCCESS)
   {
      pCh->pTapiCh->TapiMiscData.sigMask |= nSignal;
   }
   return err;
}

/**
   Configures signal detection on answering tone detector

   \param pCh  - pointer to VINETIC channel structure
   \param nSignal - signal definition
   \return
      IFX_SUCCESS or IFX_ERROR
   \remarks
      no checks are done if configuration is valid
      CED must be set, if any related detection is required. CED end detection
      is separately set.
*/
/** \todo This function does not set the interrupts for ATDx_DT, ATDx_NPR
  and ATDx_AM individually because VINETIC_Host_Set_EdspIntMask does not
  support it. Instead even when ATD1 is activated ATD2 interrupts are
  also activated.
 */
IFX_int32_t Dsp_AtdConf (VINETIC_CHANNEL *pCh, IFX_uint32_t nSignal)
{
   IFX_int32_t     err  = IFX_SUCCESS;
   IFX_uint8_t     ch   = pCh->nChannel - 1;
   VINETIC_DEVICE *pDev = pCh->pParent;
   VIN_IntMask_t   atdMask;
   IFX_uint8_t     nAtd;


   memset (&atdMask, 0, sizeof (atdMask));
   /* disable interrupts to avoid unwanted interrupts */
   if (err == IFX_SUCCESS)
      err = VINETIC_Host_Set_EdspIntMask (pCh, VIN_EDSP_ATD, IFX_FALSE,
                                          atdMask.rising, atdMask.falling);
   nAtd = 0;
   if (!(nSignal & (IFX_TAPI_SIG_CEDMASK | IFX_TAPI_SIG_CEDENDMASK |
                   IFX_TAPI_SIG_PHASEREVMASK | IFX_TAPI_SIG_AMMASK |
                   IFX_TAPI_SIG_TONEHOLDING_ENDMASK))
   )
   {
      /* no ATD1 use .
         Note : Mask registers bits are by default unset
                via previous memset */
      pDev->pSigCh[ch].sig_atd1.bit.en = 0;
   }
   else
   {
      /* as a workaround of a CERR here we first disable the ATDs
         before enabling them again */
      pDev->pSigCh[ch].sig_atd1.bit.en = 0;
      err = CmdWrite (pDev, (IFX_uint16_t*)&pDev->pSigCh[ch].sig_atd1,
                      CMD_SIG_ATD_LEN); /* disable ATD1 */
      if (err == IFX_SUCCESS)
      {
         pDev->pSigCh[ch].sig_atd2.bit.en = 0;
         err = CmdWrite (pDev, (IFX_uint16_t*)&pDev->pSigCh[ch].sig_atd2,
                         CMD_SIG_ATD_LEN); /* disable ATD2 */
      }
   }
   if (((nSignal & IFX_TAPI_SIG_CEDMASK) != 0) ||
       ((nSignal & IFX_TAPI_SIG_CEDENDMASK) != 0))
   {
      /* modify ATDs and interrupt masks according to nSignal */
      err = Atd_ConfCED(pCh, nSignal, &nAtd, &atdMask);
   }
   /* handling for tone holding */
   if ((nSignal & IFX_TAPI_SIG_TONEHOLDING_ENDMASK) != 0)
   {
      if (nAtd == 2)
      {
         SET_ERROR (ERR_NORESOURCE);
         err = IFX_ERROR;
      }
      else
         err = Atd_ConfToneHolding (pCh,
                                    nSignal & IFX_TAPI_SIG_TONEHOLDING_ENDMASK,
                                    &nAtd, &atdMask);
   }
   /* if both used, set to enable option 3 (means both).
      Atd_ConfToneHolding and Atd_ConfCED return the number of used
      ATDs. Here a switching to bit wise use is done, to make enabling easier */
   if (nAtd == 2)
      nAtd = 3;
   /* currently DIS is only detected on ATD2 */
   if ((nSignal & IFX_TAPI_SIG_DISMASK) != 0)
   {
      if (nAtd == 3)
      {
         SET_ERROR (ERR_NORESOURCE);
         err = IFX_ERROR;
      }
      else
      {
         /* detect DIS on ATD2 */
         nAtd |= 0x2;
         pDev->pSigCh[ch].sig_atd2.bit.md = VIN_ECMD_SIGATD_MD_DIS;

         if (nSignal & IFX_TAPI_SIG_DIS)
            pDev->pSigCh[ch].sig_atd2.bit.is = VIN_ECMD_SIGATD_IS_SIGINBOTH;
         else
         {
            if (nSignal & IFX_TAPI_SIG_DISRX)
               /* for receive path input B is used */
               pDev->pSigCh[ch].sig_atd2.bit.is = VIN_ECMD_SIGATD_IS_SIGINB;
            if (nSignal & IFX_TAPI_SIG_DISTX)
               /* for transmit path input A is used */
               pDev->pSigCh[ch].sig_atd2.bit.is = VIN_ECMD_SIGATD_IS_SIGINA;
         }
         Atd_SetCoeff (pDev, CED_RGAP, DIS_REPET, DIS_LEVEL,
                       (IFX_uint8_t)(ch + pDev->nSigCnt));
         /* unmask setting for interrupt 0 -> 1  */
         atdMask.rising |= VIN_ATD2_DT_MASK;
      }
   }
   else
   {
      /* no DIS and no other on ATD2 */
      pDev->pSigCh[ch].sig_atd2.bit.en = 0;
      if (nAtd == 1)
      {
         /* disable 1 -> 0 interrupt */
         atdMask.rising &= ~VIN_ATD2_DT_MASK;
      }
   }
   if (nAtd & 0x1)
   {
      /* ATD1 is enabled */
      pDev->pSigCh[ch].sig_atd1.bit.en = 1;
   }
   if (nAtd & 0x2)
   {
      pDev->pSigCh[ch].sig_atd2.bit.en = 1;
   }
   /* write ATD configuration */
   if (err == IFX_SUCCESS)
      err = CmdWrite (pDev, pDev->pSigCh[ch].sig_atd1.value, CMD_SIG_ATD_LEN);
   if (err == IFX_SUCCESS)
      err = CmdWrite (pDev, pDev->pSigCh[ch].sig_atd2.value, CMD_SIG_ATD_LEN);
   /* write interrupt masks */
   if (err == IFX_SUCCESS)
      err = VINETIC_Host_Set_EdspIntMask (pCh, VIN_EDSP_ATD, IFX_FALSE,
                                          atdMask.rising, atdMask.falling);
   /* Saved SRE2 is cleared upon ATD submodule reconfiguration, i.e. execution
    * of either  IFX_TAPI_SIG_DETECT_ENABLE or IFX_TAPI_SIG_DETECT_DISABLE.
    * It is the responsibility of the developer using the signaling functionality
    * to reconfigure the ATD submodule in order to clear the cached SRE2 value.
    * This is part of a workaround needed, since bits ATD1_AM and ATD2_AM of
    * register SRE2/EdspX_Stat2 cannot be cleared through reading SRE2 register.
    */
#ifdef VIN_2CPE
   pCh->hostCh.regEdspX_Stat2 = 0;
#else
   pCh->hostCh.regSRE2 = 0;
#endif /* VIN_2CPE */

   return err;
}

/**
   Configures call progress tone detection coefficients
   \param pCh   - pointer to VINETIC channel structure
   \param pTone - tone definition
   \param pCmd - command to fill
   \return
      IFX_SUCCESS
   \remarks
*/
IFX_int32_t Dsp_Cptd_SetCoeff (VINETIC_CHANNEL *pCh,
                               IFX_TAPI_TONE_SIMPLE_t const *pTone,
                               IFX_uint16_t *pCmd,
                               DSP_CPTD_FL_t *pFrameLength)
{
   IFX_uint8_t nr = 0;
   IFX_uint8_t step = 10, i;
   int         nTenPercentTimeTolerance, 
               nMaxTenPercentTimeTolerance = 0,
               nAbsoluteTimeTolerance;

   /* set frame length, in case no step is <= 200ms we use a frame lenth of
      32ms, otherwise 16ms */
   *pFrameLength = DSP_CPTD_FL_32;
   for (i = 0; i < IFX_TAPI_TONE_STEPS_MAX; ++i)
   {
      /* check for last cadence step */
      if (pTone->cadence[i] == 0)
         break;
      /* if one of the steps is shorter or eaqual to 200ms we reduce the
         frame length to 16 ms */
      if (pTone->cadence[i] <= 200)
         *pFrameLength = DSP_CPTD_FL_16;
   }

   /* set nAbsoluteTimeTolerance to roughly 1.2 times the frame length */
   switch (*pFrameLength)
   {
      case DSP_CPTD_FL_16:
         nAbsoluteTimeTolerance = 20;
         break;
      case DSP_CPTD_FL_64:
         nAbsoluteTimeTolerance = 76;
         break;
      case DSP_CPTD_FL_32:
      default:
         nAbsoluteTimeTolerance = 40;
         break;
   }

   /* default settings */
   /* program allowed twist to 6 dB */
   pCmd[10] = 0x2020;
   pCmd[20] = CPTCOEFF_POW_PAUSE;
   pCmd[21] = CPTCOEFF_FP_TP_R_DEFAULT;

   /* set frequency and level A for F_1. Freq A is always set. Tone API
      assures it, the detection level is always below the defined tone. */
   pCmd [2] = (IFX_uint16_t)cpt_calc_freq   (pTone->freqA);
   pCmd [6] = (IFX_uint16_t)cpt_calc_level  (pTone->levelA - 
                                             CPTD_DETECTION_LEVEL_TOLERANCE);
   /* set frequency and level B  for F_2 */
   if (pTone->freqB)
   {
      pCmd [3] = (IFX_uint16_t)cpt_calc_freq   (pTone->freqB);
      pCmd [7] = (IFX_uint16_t)cpt_calc_level  (pTone->levelB -
                                                CPTD_DETECTION_LEVEL_TOLERANCE);
   }
   else
   {
      pCmd [3] = VIN_ECMD_SIGCPTCOEFF_FX_0HZ;
      pCmd [7] = VIN_ECMD_SIGCPTCOEFF_LEVEL_0DB;
   }
   /* set frequency and level C for F_3 */
   if (pTone->freqC)
   {
      pCmd [4] = (IFX_uint16_t)cpt_calc_freq   (pTone->freqC);
      pCmd [8] = (IFX_uint16_t)cpt_calc_level  (pTone->levelC -
                                                CPTD_DETECTION_LEVEL_TOLERANCE);
   }
   else
   {
      pCmd [4] = VIN_ECMD_SIGCPTCOEFF_FX_0HZ;
      pCmd [8] = VIN_ECMD_SIGCPTCOEFF_LEVEL_0DB;
   }
   /* set frequency and level D for F_4 */
   if (pTone->freqD)
   {
      pCmd [5] = (IFX_uint16_t)cpt_calc_freq   (pTone->freqD);
      pCmd [9] = (IFX_uint16_t)cpt_calc_level  (pTone->levelD -
                                                CPTD_DETECTION_LEVEL_TOLERANCE);
   }
   else
   {
      pCmd [5] = VIN_ECMD_SIGCPTCOEFF_FX_0HZ;
      pCmd [9] = VIN_ECMD_SIGCPTCOEFF_LEVEL_0DB;
   }
   /* set step times:  T_x */
   for (i = 0; i < IFX_TAPI_TONE_STEPS_MAX; ++i)
   {
      /* check for last cadence step */
      if (pTone->cadence[i] == 0)
         break;
      /* increase step to timing. Step is initialized with 12 (one before
         the first timing setting */
      step++;

      /* to allow +/- 10% deviation in the time, we'll reduce each time 
         by 10% and program +TIM_TOL to 2 * MAX ( cadence [i] / 10 ).
         In addition we check if the tolerance for each step as well as
         +TIM_TOL is smaller than nAbsoluteTimeTolerance, if so we use the
         latter one.
       */
      nTenPercentTimeTolerance = pTone->cadence[i] / 10;
      if (nTenPercentTimeTolerance > nMaxTenPercentTimeTolerance)
         nMaxTenPercentTimeTolerance = nTenPercentTimeTolerance;

      pCmd [step] = (IFX_uint16_t)(pTone->cadence[i] -
                        MAX(nAbsoluteTimeTolerance, nTenPercentTimeTolerance) );

      /* move step to step setting */
      step++;
      if ((pTone->frequencies[i] & IFX_TAPI_TONE_FREQALL) == 0)
      {
         pCmd [step] = VIN_ECMD_SIGCPTCOEFF_P;
      }
      else
      {
         /* set mask for MSK_i - use frequency A. Initialize the field */
         if (pTone->frequencies[i] & IFX_TAPI_TONE_FREQA)
            pCmd [step] = VIN_ECMD_SIGCPTCOEFF_F1;
         else
            pCmd [step] = 0;
         /* set mask for MSK_i - use frequency B  */
         if (pTone->frequencies[i] & IFX_TAPI_TONE_FREQB)
            pCmd [step] |= VIN_ECMD_SIGCPTCOEFF_F2;
         /* set mask for MSK_i - use frequency C  */
         if (pTone->frequencies[i] & IFX_TAPI_TONE_FREQC)
            pCmd [step] |= VIN_ECMD_SIGCPTCOEFF_F3;
         /* set mask for MSK_i - use frequency D  */
         if (pTone->frequencies[i] & IFX_TAPI_TONE_FREQD)
            pCmd [step] |= VIN_ECMD_SIGCPTCOEFF_F4;
      }
      nr++;
   }
   if (pTone->cadence[1] == 0 && pTone->pause == 0)
   {
      /* continuos tone, program time for verification */
      pCmd [step-1] = CPTCOEFF_CNT_MINTIME;
      /* set tolerance +TIM_TOL */
      pCmd [19] = (2 * MAX(nAbsoluteTimeTolerance, nMaxTenPercentTimeTolerance)) << 8;
      /* return because nothing more to do */
      return IFX_SUCCESS;
   }
   /* set end of steps */
   pCmd[step] |= VIN_ECMD_SIGCPTCOEFF_E;

   /* set tolerance +TIM_TOL and NR of successfully fulfilled timing 
      requirement steps required to pass */
   pCmd[19] = (2 * MAX(nAbsoluteTimeTolerance, nMaxTenPercentTimeTolerance)) << 8 | nr;

   return IFX_SUCCESS;
}

/**
   Configures the call progress tone detector
   \param pCh    - pointer to VINETIC channel structure
   \param pTone  - pointer to the complex tone to detect
   \param signal - signal as specified in IFX_TAPI_TONE_CPTD_DIRECTION_t
   \param pCmd   - pointer to the command structure to fill
   \return
      IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t Dsp_Cptd_Conf (VINETIC_CHANNEL *pCh, IFX_TAPI_TONE_SIMPLE_t const *pTone,
                           IFX_int32_t signal, IFX_uint16_t *pCmd,
                           DSP_CPTD_FL_t frameLength)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *)pCh->pParent;
   IFX_uint8_t    ch    = (pCh->nChannel - 1);

   if ((pDev->pCodCh[3].cod_ch.bit.en == 1) &&
       (pDev->nChipMajorRev == VINETIC_V1x))
   {
      SET_ERROR(ERR_NORESOURCE);
      return IFX_ERROR;
   }
   /* clean cmd data word */
   pCmd[2] = 0x0000;

   /* set frame length */
   VIN_ECMD_SIGCPT_FL_SET (pCmd[2], frameLength);
   /* set total power */
   VIN_ECMD_SIGCPT_TP_SET (pCmd[2], DSP_CPTD_TP_250_3400_HZ);
   /* select window type */
   VIN_ECMD_SIGCPT_WS_SET (pCmd[2], VIN_ECMD_SIGCPT_WS_HAMMING);
   /* set resource number */
   VIN_ECMD_SIGCPT_CPTNR_SET (pCmd[2], ch);
   /* enable CPTD */
   VIN_ECMD_SIGCPT_EN_SET (pCmd[2], 1);

   /* check for continuos tone */
   if (pTone->cadence[1] == 0)
   {
      VIN_ECMD_SIGCPT_CNT_SET (pCmd[2], 1);
   }
   switch (signal)
   {
      case  IFX_TAPI_TONE_CPTD_DIRECTION_RX:
         /* for receive path input B is used */
         VIN_ECMD_SIGCPT_IS_SET (pCmd[2], VIN_ECMD_SIGCPT_IS_SIGINB);
      break;
      case  IFX_TAPI_TONE_CPTD_DIRECTION_TX:
         VIN_ECMD_SIGCPT_IS_SET (pCmd[2], VIN_ECMD_SIGCPT_IS_SIGINA);
      break;
      default:
         SET_ERROR(ERR_FUNC_PARM);
         return IFX_ERROR;
   }
   /* remember CPT settings and enable status for resource management */
   pDev->pSigCh[ch].nCpt = pCmd[2];
   return IFX_SUCCESS;
}

/**
   Stops the UTG
   \param   pCh    - pointer to VINETIC channel structure

   \return
      IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t Dsp_Utg_Stop (VINETIC_CHANNEL *pCh, IFX_uint8_t utgNum)
{
   IFX_int32_t     ret = IFX_SUCCESS;
   VINETIC_DEVICE *pDev = pCh->pParent;
   FWM_SIG_UTG    *pUtgCmd = &pDev->pSigCh[pCh->nChannel - 1].utg[utgNum];
   IFX_uint8_t     utgRes = 0;

   /* check utgNum,
   please note that resources like "utgNum" are counted from #0 */
   if  (utgNum >= pDev->VinCapabl.nUtgPerCh)
   {
      SET_ERROR (ERR_NORESOURCE);
      return IFX_ERROR;
   }

   /* choose utg resource according to the FW capabilities */
   if (utgNum == 0)
   {  /* resources #0..#3 for first utg */
      utgRes = (IFX_uint8_t)(pCh->nChannel - 1);
   }
   else if (utgNum == 1)
   {  /* second utg */
      utgRes = (IFX_uint8_t)(pCh->nChannel - 1) + 4; /* resources #4..#7 */
   }
   else
   {
      SET_ERROR (ERR_NORESOURCE);
      return IFX_ERROR;
   }

   /* overwrite utg resource number, because here the "overall number" is used */
   pUtgCmd->bit.utgnr = (utgRes & CMD1_CH);
   /* Stop tone playing for tones which is using the UTG Force the UTG to
      stop immediately if already disabled */
   if (pUtgCmd->bit.sm == 1 && pUtgCmd->bit.en == 0)
   {
      /* enable the UTG first because it is already disabled and auto stop
         is activated. So first enabled it and set auto stop to false */
      pUtgCmd->bit.en = 1;
      pUtgCmd->bit.sm = 0;
      ret = CmdWrite (pDev, pUtgCmd->value, 1);
   }
   if (pUtgCmd->bit.en == 1)
   {
      pUtgCmd->bit.en = 0;
      pUtgCmd->bit.sm = 0;
      ret = CmdWrite (pDev, pUtgCmd->value, 1);
   }


   if (utgNum == 0)
   {  /* second utg */
      ret = VINETIC_Host_Set_EdspIntMask (pCh, VIN_EDSP_UTG1, IFX_FALSE, 0, 0);
   }
   else if (utgNum == 1)
   {  /* second utg */
      ret = VINETIC_Host_Set_EdspIntMask (pCh, VIN_EDSP_UTG2, IFX_FALSE, 0, 0);
   }
#ifndef FW_ETU
   /* reenable the DTMF module */
   if (ret == IFX_SUCCESS)
   {
      pDev->pSigCh[pCh->nChannel - 1].sig_dtmfgen.bit.en = 1;
      ret = CmdWrite (pDev,
                      pDev->pSigCh[pCh->nChannel - 1].sig_dtmfgen.value,
                      CMD_SIG_DTMFGEN_LEN);
   }
#endif /* FW_ETU */

   return ret;
}

/**
   Sets Coder payload type (RTP) in the streaming direction given

   \param  pCh    - pointer to VINETIC channel structure
   \param  bUp    - IFX_TRUE : upstream / IFX_FALSE : downstream

   \return
      IFX_SUCCESS or IFX_ERROR
   \remark
      This function assumes that the array pPTvalues is at least
      IFX_TAPI_ENC_TYPE_MAX big and supports all coders specified in
      \ref IFX_TAPI_ENC_TYPE_t which is used as index.
*/
IFX_int32_t Dsp_SetRTPPayloadType (VINETIC_CHANNEL *pCh, IFX_boolean_t bUp,
                                   IFX_uint8_t *pPTvalues)
{
   IFX_int32_t     ret = IFX_SUCCESS;
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint8_t     ch    = (pCh->nChannel - 1), count = 0;
   IFX_uint16_t    pData [20] = {0}, *pPtype = IFX_NULL;

   /* upstream direction */
   if (bUp == IFX_TRUE)
   {
      /* set commands data words using cached value to avoid reading
         the coder channel configuration */
      pData [0] = (CMD1_EOP | ch);
      pData [1] = ECMD_COD_CHRTP_UP;
      pData [2] = HIGHWORD (pDev->pCodCh[ch].nSsrc);
      pData [3] = LOWWORD  (pDev->pCodCh[ch].nSsrc);
      pData [4] = pDev->pCodCh[ch].nSeqNr;
      /* set ptr to right place in RTP config data buffer */
      pPtype = &pData [5];
      /* set number of data to write */
      count  = 18;
   }
   else /* downstream direction */
   {

      /* set command to write the whole message */
      pData [0] = (CMD1_EOP | ch);
      pData [1] = ECMD_COD_CHRTP_DOWN;
      /* set ptr to right place in RTP config data buffer */
      pPtype    = &pData [2];
      /* set number of data to write */
      count     = 15;
   }
   /* G711, IFX_TAPI_ENC_TYPE_ALAW, 64 KBps */
   pPtype [0]   =   pPTvalues [IFX_TAPI_ENC_TYPE_ALAW] << 8;
   /* G711, uLAW, 64 KBps */
   pPtype [0]   |=  pPTvalues [IFX_TAPI_ENC_TYPE_MLAW] | 0x8080;
   /* G726, 16 KBps */
   pPtype [1]   =   pPTvalues [IFX_TAPI_ENC_TYPE_G726_16] << 8;
   /* G726, 24 KBps */
   pPtype [1]   |=  pPTvalues [IFX_TAPI_ENC_TYPE_G726_24] | 0x8080;
   /* G726, 32 KBps */
   pPtype [2]   =   pPTvalues [IFX_TAPI_ENC_TYPE_G726_32] << 8;
   /* G726, 40 KBps */
   pPtype [2]   |=  pPTvalues [IFX_TAPI_ENC_TYPE_G726_40] | 0x8080;
   /* IFX_TAPI_ENC_TYPE_G728, 16 KBps */
   pPtype [7]    =  pPTvalues[IFX_TAPI_ENC_TYPE_G728] << 8;
   /* IFX_TAPI_ENC_TYPE_G729, 8KBps */
   pPtype [8]    =  (pPTvalues[IFX_TAPI_ENC_TYPE_G729] << 8) |
                     pPTvalues[IFX_TAPI_ENC_TYPE_G729_E];
   /** iLBC, 15.2 kBit/s */
   pPtype [12]   =  (pPTvalues [IFX_TAPI_ENC_TYPE_ILBC_152] << 8);
   /** iLBC, 13.3 kBit/s */
   pPtype [12]  |=  pPTvalues [IFX_TAPI_ENC_TYPE_ILBC_133];
   /* G723, 5,3 KBps */
   pPtype [13]   =  (pPTvalues [IFX_TAPI_ENC_TYPE_G723_53] << 8);
   /* G723, 6,3 KBps */
   pPtype [13]  |=  pPTvalues [IFX_TAPI_ENC_TYPE_G723_63];

   ret = CmdWrite (pDev, pData, count);

   return ret;
}

#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
/**
   Enables or disables the signalling interface modules :
   DTMF gen, DTMF rec, CID, Tone generators

   \param  pCh    - pointer to VINETIC channel structure
   \param  bOn    - IFX_TRUE : switch on / IFX_FALSE : switch off

   \return
      IFX_SUCCESS or IFX_ERROR
   \remark
      This function can be called to disable the signalling interface without
      disabling the signalling channel itself and thus to avoid disturbance of an
      ongoing conference, as the input signals of signalling channel are connected
      in this process.
*/
IFX_int32_t Dsp_SwitchSignModules (VINETIC_CHANNEL *pCh, IFX_boolean_t bOn)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t ret, ch = pCh->nChannel - 1;

   ret = Dsp_SetToneDetectors (pCh, bOn);
   if (ret == IFX_SUCCESS)
      ret = Dsp_SetDtmfRec (pCh, bOn);
   if (ret == IFX_SUCCESS)
   {
      if (bOn)
         pDev->pSigCh[ch].sig_dtmfgen.bit.en = 1;
      else
         pDev->pSigCh[ch].sig_dtmfgen.bit.en = 0;
      ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_dtmfgen.value,
                      CMD_SIG_DTMFGEN_LEN);
   }
   if (ret == IFX_SUCCESS)
      ret = Dsp_SetCidSender (pCh, bOn);

   return ret;
}
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38) */

#ifdef TAPI_CID
/**
   Set the CID sender coefficients
\param
   pCh         - pointer to VINETIC channel structure
\param
   cidHookMode - cid hook mode as specified in IFX_TAPI_CID_HOOK_MODE_t
\param
   pFskConf    - handle to IFX_TAPI_CID_FSK_CFG_t structure
\param
   nSize       - size of CID data to send, used to set the BRS level.
\return
   IFX_SUCCESS or Error Code
\remark
   - The CID sender must be disabled before programming the coefficients, what
      is done in this function
   - According to CID type (onhook=Type1, Offhook=Type2), different
      Mark/Seizure configuration apply according to specifications:
      GR-30-CORE : Type 1 (Onhook)  => Seizure = 300 bits, Mark = 180 bits
                   Type 2 (Offhook) => Seizure = 0 bits, Mark = 80 bits
      ETSI       : Type 1 (Onhook)  => Seizure = 300 bits, Mark = 180+/-25 bits
                   Type 2 (Offhook) => Seizure = 0 bits, Mark =  80+/-25bits
      SIN227     : Type 1 (Onhook)  => Seizure = 96 to 315 bits, Mark >= 55 bits
                   Type 2 (Offhook) => Seizure = 0 bits, Mark >= 55 bits

      To meet all these specifications, following is set as default:
         Type 1 (Onhook)  => Seizure = 300 bits, Mark = 180 bits
         Type 2 (Offhook) => Seizure =   0 bits, Mark = 80 bits
   - Here are some dB levels for reference :
      (formula: level=32768*10^(dBval / 20)
      -5dB = 0x47FA, -10 dB = 0x287A, - 15 dB = 0x16C3, -20dB = 0xCCC ,
      -25 dB = 0x732, -30 dB = 0x40C , -35 dB = 0x246, -40 dB = 0x147
*/
IFX_int32_t Dsp_SetCIDCoeff (VINETIC_CHANNEL          *pCh,
                             IFX_TAPI_CID_HOOK_MODE_t  cidHookMode,
                             IFX_TAPI_CID_FSK_CFG_t   *pFskConf,
                             IFX_uint8_t               nSize)
{
   IFX_int32_t    err = IFX_SUCCESS;
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint16_t   ch = (pCh->nChannel - 1);
   IFX_uint16_t   pCidCoefs [6] = {0};

   /* not allowed while cid sender is active */
   err = Dsp_SetCidSender (pCh, IFX_FALSE);
   if (err == IFX_SUCCESS)
   {
      /* set the new coefficients */
      pCidCoefs[0] = (CMD1_EOP | ch);
      pCidCoefs[1] = ECMD_CID_COEF;
      /* level ~= -5 dB */
      /** FIXME: calculate level value */
      pCidCoefs[2] = 0x47FA;
      /* set seizure and mark according to CID type */
      switch (cidHookMode)
      {
      /* offhook CID, called CID type 2, NTT inclusive */
      case  IFX_TAPI_CID_HM_OFFHOOK:
         /* set Seizure */
         pCidCoefs[3] = pFskConf->seizureTX;
         /* set Mark */
         pCidCoefs[4] = pFskConf->markTXOffhook;
         break;
      /* onhook CID, called CID type 1, NTT inclusive */
      case IFX_TAPI_CID_HM_ONHOOK:
         /* set Seizure */
         pCidCoefs[3] = pFskConf->seizureTX;
         /* set Mark */
         pCidCoefs[4] = pFskConf->markTXOnhook;
         break;
      }
      /* BRS <= MAX_CID_LOOP_DATA*/
      if (nSize >= MAX_CID_LOOP_DATA)
         pCidCoefs [5] = MAX_CID_LOOP_DATA;
      else
         pCidCoefs [5] = nSize;

      err = CmdWrite (pDev, pCidCoefs, 4);
   }
   if (err != IFX_SUCCESS)
      TRACE (VINETIC,DBG_LEVEL_HIGH, ("CID Coefficients setting failed\n\r"));

   return err;
}

/**
  Disables or Enables Cid Receiver according to bEn.

\param
   pCh  - pointer to VINETIC channel structure
\param
   bEn  - IFX_TRUE : enable / IFX_FALSE : disable cid receiver

\return
   IFX_SUCCESS or IFX_ERROR
\remark
   The signalling channel must be enabled before this command is issued.
*******************************************************************************/
IFX_int32_t Dsp_SetCidRec (VINETIC_CHANNEL const *pCh, IFX_boolean_t bEn)
{
   IFX_int32_t     ret      = IFX_SUCCESS;
   IFX_uint16_t    pCmd [2] = {0}, pData [3] = {0};
   VINETIC_DEVICE *pDev     = pCh->pParent;

   /* EOP Cmd */
   pCmd [0] = (CMD1_EOP | (pCh->nChannel - 1));
   /* CID RX */
   pCmd [1] = ECMD_CIDRX;
   ret = CmdRead (pDev, pCmd, pData, 1);
   if (ret == IFX_SUCCESS)
   {
      if ((bEn == IFX_TRUE) && !(pData [2] & SIG_CIDRX_EN))
      {
         pData [2] |= SIG_CIDRX_EN;
         ret = CmdWrite (pDev, pData, 1);
      }
      else if ((bEn == IFX_FALSE) && (pData [2] & SIG_CIDRX_EN))
      {
         pData [2] &= ~SIG_CIDRX_EN;
         ret = CmdWrite (pDev, pData, 1);
      }
   }

   return ret;
}

#endif /* TAPI_CID */
#endif /* TAPI */

/**
   Firmware DTMF generator configuration

   \param
      pCh     - handle to VINETIC_CHANNEL structure
   \param
      nTIMT   - DTMF tone on time, TIM=TIM[ms]*2 with 0ms < TIM < 127ms
   \param
      nTIMP   - DTMF tone pause, TIM=TIM[ms]*2 with 0ms < TIM < 127ms
   \return
      IFX_SUCCESS or IFX_ERROR
   \remarks
      Disables DTMF generator during configuration and leave values set to 0xFF
      untouched. See description of DTMF/AT Generator, Coefficients message for
      more details on parameters.
*/
IFX_int32_t Dsp_SetDtmfCoeff(VINETIC_CHANNEL *pCh, IFX_uint8_t nTIMT,
                             IFX_uint8_t nTIMP)
{
   IFX_int32_t       err, enable;
   VINETIC_DEVICE    *pDev    = (VINETIC_DEVICE *)pCh->pParent;
   IFX_uint16_t      pCmd[2]  = {0}, ch = pCh->nChannel - 1;
   IFX_uint16_t      pData[5] = {0};

   /* Disable DTMF/AT generator */
   enable = pDev->pSigCh[ch].sig_dtmfgen.bit.en;
   pDev->pSigCh[ch].sig_dtmfgen.bit.en = 0;
   err = CmdWrite (pDev, pDev->pSigCh[ch].sig_dtmfgen.value,
                   CMD_SIG_DTMFGEN_LEN);
   if (err == IFX_ERROR)
      return IFX_ERROR;

   /* Prepare and send DTMF/AT generator coefficients */
   pCmd [0] = (CMD1_EOP | ch);
   pCmd [1] = ECMD_DTMF_COEF;
   err = CmdRead(pDev, pCmd, pData, 3);

   if (err == IFX_SUCCESS)
   {
      /* Only modify values not set to 0xFF */
      if (nTIMT != 0xFF)
         pData[3] = (pData[3] & 0x00FF) | (nTIMT << 8);
      if (nTIMP != 0xFF)
         pData[3] = (pData[3] & 0xFF00) | nTIMP;
      err = CmdWrite(pDev, pData, 3);
   }
   if (enable)
   {
      pDev->pSigCh[ch].sig_dtmfgen.bit.en = 1;
      err = CmdWrite (pDev, pDev->pSigCh[ch].sig_dtmfgen.value,
                      CMD_SIG_DTMFGEN_LEN);
   }
   return err;
}

/**
   Firmware DTMF/AT generator control

   \param
      pCh   - pointer to the channel interface
   \param
      nMode - mode field according to DTMF/AT generator message
   \param
      nMask - only bits set to one will be modified
   \param
      nIsr  - access to VINETIC during normal execution (IFX_FALSE) or interrupt (IFX_TRUE)
   \return
      IFX_SUCCESS/IFX_ERROR
   \remarks
      See description of DTMF/AT Generator message for more details on parameter.
*/
IFX_int32_t Dsp_DtmfGen (VINETIC_CHANNEL *pCh, IFX_uint16_t nMode,
                         IFX_uint16_t nMask, IFX_boolean_t nIsr)
{
   IFX_int32_t    err   = IFX_SUCCESS;
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint8_t     ch   = pCh->nChannel - 1;

   pDev->pSigCh[ch].sig_dtmfgen.value[CMD_HEADER_CNT] =
       ((pDev->pSigCh[ch].sig_dtmfgen.value[CMD_HEADER_CNT] & ~nMask) |
       (nMode & nMask));
   if (nIsr == 0)
   {
      err = CmdWrite (pDev, pDev->pSigCh[ch].sig_dtmfgen.value,
                      CMD_SIG_DTMFGEN_LEN);
   }
   else
   {
      err = CmdWriteIsr (pDev, pDev->pSigCh[ch].sig_dtmfgen.value,
                         CMD_SIG_DTMFGEN_LEN);
   }
   return err;
}

/**
   Activates ALI NLP

   \param  pCh        - pointer to VINETIC channel structure

   \return
      IFX_SUCCESS or IFX_ERROR

   \remark
      In case of fax t38 transmission, NLP should be enabled once fax call is
      over
*/
IFX_int32_t Dsp_AlmNlpEnable (VINETIC_CHANNEL *pCh)
{
   IFX_int32_t ret;
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint16_t pCmd [2] = {0}, pData [3] = {0};

   /* EOP Cmd */
   pCmd[0] = CMD1_EOP | (pCh->nChannel -1);
   pCmd [1] = ECMD_ALM_NEARLEC;
   ret = CmdRead(pDev, pCmd, pData, 1);
   if(ret == IFX_SUCCESS)
   {
      /*EN = 1, NLP = 1 */
      pData [2] |= (ALM_NLEC_EN | ALM_NLP_EN);
      /* write command */
      ret = CmdWrite (pDev, pData, 1);
   }

   return ret;
}

/**
   Configures the fax datapump demodulator

   \param  pCh        - pointer to VINETIC channel structure
   \param  nSt1       - standard 1
   \param  nSt2       - Standard 2
   \param  nEq        - Equalizer
   \param  nTr        - Training

   \return
      IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t Dsp_SetDPDemod (VINETIC_CHANNEL *pCh, IFX_uint8_t nSt1,
                            IFX_uint8_t nSt2, IFX_uint8_t nEq,
                            IFX_uint8_t nTr)
{
   IFX_uint16_t pCmd [3] = {0};
   VINETIC_DEVICE *pDev = pCh->pParent;

   pCmd [0] = (CMD1_EOP | (pCh->nChannel - 1));
   pCmd [1] = ECMD_COD_CHFAXDEMOD;
   pCmd [2] = ((nSt1 & COD_DEMOD_ST1)        |
               ((nSt2 << 5) & COD_DEMOD_ST2) |
               ((nEq << 14) & COD_DEMOD_EQ)  |
               ((nTr << 15) & COD_DEMOD_TRN));

   return CmdWrite (pDev, pCmd, 1);
}

/**
   Configures the Datapump modulator

   \param   pCh        - pointer to VINETIC channel structure
   \param   nSt        - standard
   \param   nLen       - signal length
   \param   nDbm       - level
   \param   nTEP       - TEP
   \param   nTr        - Training

   \return
      IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t Dsp_SetDPMod (VINETIC_CHANNEL *pCh, IFX_uint8_t nSt,
                                     IFX_uint16_t nLen, IFX_uint8_t nDbm,
                                     IFX_uint8_t nTEP, IFX_uint8_t nTr)
{
   IFX_uint16_t pCmd [4] = {0};
   VINETIC_DEVICE *pDev = pCh->pParent;

   /* set command to configure the  modulator */
   pCmd [0] = (CMD1_EOP | (pCh->nChannel - 1));
   pCmd [1] = ECMD_COD_CHFAXMOD;
   pCmd [2] = (nSt | (nTr << 5) | (nTEP << 6) |
               ((nDbm << 8) & COD_MOD_DBM));
   if (nLen <= COD_CHFDP_MODMAXSGLEN)
      pCmd [3] = (nLen & COD_MOD_SGLEN);
   else
      pCmd [3] = COD_CHFDP_MODMAXSGLEN;

   return CmdWrite (pDev, pCmd, 2);
}

/**
   Set data pump according to input parameters

   \param   pCh        - pointer to VINETIC channel structure
   \param   bEn        - IFX_FALSE : disable / IFX_TRUE : enable datapump
   \param   bMod       - 0 = Modulator / 1 = Demodulator
   \param   gain       - gain to be applied
   \param   mod_start  - level for start modulation
   \param   mod_req    - level for request more data
   \param   demod_send - level for send data

   \return
      IFX_SUCCESS or IFX_ERROR
   \remark
      This function disables datapump if bEn is IFX_FALSE. Otherwise it enables the
      datapump and set modulator or demodulator according to bMod
*/
IFX_int32_t Dsp_SetDatapump (VINETIC_CHANNEL *pCh, IFX_boolean_t bEn,
                             IFX_boolean_t bMod, IFX_uint16_t gain,
                             IFX_uint16_t mod_start, IFX_uint16_t mod_req,
                             IFX_uint16_t demod_send)
{
   IFX_uint16_t    pCmd [7] = {0};
   VINETIC_DEVICE *pDev     = pCh->pParent;
   IFX_uint8_t     ch       = (pCh->nChannel - 1), count;

   /* setup cmd 1 */
   pCmd [0] = (CMD1_EOP | ch);
   /* set Coder Channel Fax data Pump */
   pCmd [1] = ECMD_COD_CHFAXDP;

   /** \todo  To check: Because the ALM isn't directly connected to
     CODER anymore, how shall we connect the datapump input?
     Where is coming the signal from? Now we use the coder configuration
     on this channel, as the coder is disabled.       */
   pCmd [2] = (ch << 8) | pDev->pCodCh[ch].cod_ch.bit.i1;

   /* Disable Datapump if  EN = 1 */
   if (bEn == IFX_FALSE)
   {
      count =1;
   }
   else
   {
      count = 5;
      /* Set DPNR = Ch , input 1 = Coder input I1 setting */
      /* Note: The coder channel should be disabled to do this, so that
               we can use this resource. */
      pCmd [2] |= COD_CHFDP_EN;

      switch (bMod)
      {
         /* configure data for modulation */
         case IFX_FALSE:
            /* Modulation gain */
            pCmd [3] = 0x6000 | gain;
            /* MOBSM: level for start modulation */
            pCmd [4] = mod_start;
            /* MOBRD: level for request more data */
            pCmd [5] = mod_req;
            /* set unused fields to valid values */
            pCmd [6] = 1;
            break;
         /* configure data for demodulation */
         case IFX_TRUE:
            /* MD = 1, */
            pCmd [2] |= COD_CHFDP_MD;
            /* Demodulation gain */
            pCmd [3] = 0x60 | ((IFX_uint16_t)gain << 8);
            /* set unused fields to valid values */
            pCmd [4] = 1;
            pCmd [5] = 1;
            /* DMBSD: send data level */
            pCmd [6] = demod_send;
            break;
      }
   }

   return (CmdWrite (pDev, pCmd, count));
}


/**
   Configures the Near LEC in the analog module
   \param   pCh        - pointer to VINETIC channel structure
   \param   bEnLec     - IFX_FALSE : disable / IFX_TRUE : enable LEC
   \param   bEnNlp     - IFX_FALSE : disable / IFX_TRUE : enable NLP
   \param   bEnWLec    - IFX_FALSE : disable / IFX_TRUE : enable bEnWLec, requires enable LEC

   \return
      IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t Dsp_SetNeLec_Alm(VINETIC_CHANNEL *pCh,
                             IFX_boolean_t bEnLec,
                             IFX_boolean_t bEnNlp,
                             IFX_boolean_t bEnWLec)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint16_t   ch = (pCh->nChannel - 1);
   IFX_uint16_t   pCmd1[3];
   IFX_uint8_t    lec_res;
   IFX_uint16_t   pCmd2[11];
   static const IFX_uint16_t alm_nelec_coefs[5] =
   {
      ECMD_NELEC_COEFS, 0x1048, 0x1808, 0x6060, 0x0060
   };
   /* should be used in connection with WLEC-FW only! */
   static const IFX_uint16_t alm_nelec_wlec_coefs[7] =
   {  /* Recommended WLEC Coefficient Command Parameters:
            LEN         |   POWR       = 0x103D
            DELTAP      |   DELTAQ     = 0x0808
            GAIN_XI     |   GAIN_XO    = 0x6060
            **********  |   Gain_RI    = 0x0060
            LEN_FIX_WIN |   PMW_POWR   = 0x0834
            PMW_DELTAP  |   PMW_DELTAQ = 0x0008 */
      ECMD_NELEC_COEFS, 0x103D, 0x0808, 0x6060, 0x0060, 0x0834, 0x0008
   };
   static const IFX_uint16_t alm_nelec_nlp[10] =
   {
      ECMD_NELEC_NLP, 0x1502, 0x4540, 0x0C40, 0x4180,
      0x1106, 0x5050, 0x0D10, 0x6480, 0x0810
   };
   /* ---------------------------------------------------- */

   /* if WLEC should be used, check WLEC capability and parameters */
   if ( ((bEnWLec == IFX_TRUE) && (pDev->VinCapabl.bWlec != IFX_TRUE))    /* WLEC is not available! */
        || ((bEnWLec == IFX_TRUE) && (bEnLec != IFX_TRUE))              /* enable bEnWLec, requires enable LEC */
      )
   {  /* => invalid parameter, error code ERR_FUNC_PARM */
      SET_ERROR (ERR_FUNC_PARM);
      return IFX_ERROR;
   }

   if (ch >= pDev->nAnaChan)
   {
      /* no analog channel! */
      SET_ERROR (ERR_FUNC_PARM);
      return IFX_ERROR;
   }
   /* set channel */
   pCmd1 [0] = CMD1_EOP | ch;

   /* disable ali nelec if required by application */
   if (bEnLec == IFX_FALSE)
   {
      if (pDev->pAlmCh[ch].ali_nelec & ALM_NLEC_EN)
      {
         /* update cached value and release LEC resource back to the minimal
            resource management. */
         pDev->availLecRes |= (1 << (pDev->pAlmCh[ch].ali_nelec & ALM_LECNR));
         pDev->pAlmCh[ch].ali_nelec &= ~(ALM_NLEC_EN | ALM_NLP_EN | ALM_LECNR);

         /* set ALI NELEC command */
         pCmd1 [1] = ECMD_ALM_NEARLEC;
         pCmd1 [2] = pDev->pAlmCh[ch].ali_nelec;

         return CmdWrite (pDev, pCmd1, 1);
      }
   }
   else
   {
      /* configure the LEC, get a LEC resource and enable accordingly */

      if (!(pDev->pAlmCh[ch].ali_nelec & ALM_NLEC_EN))
      {
         /* LEC reource required */
         if (pDev->availLecRes == 0)
         {
            SET_ERROR (ERR_NORESOURCE);
            return IFX_ERROR;
         }

         /* get the LEC resource */
         lec_res = get_free_lec_res(pDev);
         /* check capabilities */
         if (lec_res >= pDev->VinCapabl.nLecRes)
            return IFX_ERROR;
         /* set channel in lec/nlp coefficient command */
         pCmd2 [0] = CMD1_EOP | lec_res;
         /* set channel in lec command */
         pCmd1 [2] = pDev->pAlmCh[ch].ali_nelec | lec_res | ALM_NLEC_EN;
         /* set NLP coefficients defaults for ALM,
         only needed in this case! */
         memcpy(&pCmd2[1], alm_nelec_nlp, sizeof(alm_nelec_nlp));
         if (CmdWrite (pDev, pCmd2, 9) != IFX_SUCCESS)
            return IFX_ERROR;
      }
      else
      {  /* no LEC resource is required, because LEC is running already */
         pCmd1 [2] = pDev->pAlmCh[ch].ali_nelec;
         /* get the LEC resource which is used for this channel */
         lec_res = pDev->pAlmCh[ch].ali_nelec & ALM_LECNR;
         /* check capabilities */
         if (lec_res >= pDev->VinCapabl.nLecRes)
            return IFX_ERROR;
         /* set channel in lec coefficient command */
         pCmd2 [0] = CMD1_EOP | lec_res;
      }

      if ( bEnWLec == IFX_TRUE )
      {
         /* activate WLEC feature */
         /* Note: WLEC capability check already done!
         WLEC Configuration: Normal operation mode for the window based LEC.
         The window position as well as the filter coefficients are adapted by the LEC.
         OLDP = 0, OLDC = 0
         DCF = 1,
         MW = 1, The de-correlation filter is part of the new WLEC only
         and should be activated. */
         pCmd1 [2] &= ~(ALM_OLDC_EN | ALM_OLDP);
         pCmd1 [2] |= ALM_MW_EN | ALM_DCF_EN;
         /* set NLEC coefficients defaults for ALM */
         TRACE (VINETIC, DBG_LEVEL_LOW, ("Dsp_SetNeLec_Alm, set WLEC coefficients for ALM, ch: %d \r\n", ch));
         memcpy(&pCmd2[1], alm_nelec_wlec_coefs, sizeof(alm_nelec_wlec_coefs));
         /* write NLEC coefficients command for ALM */
         if (CmdWrite (pDev, pCmd2, 6) != IFX_SUCCESS)
            return IFX_ERROR;
      }
      else
      {
         /* set NLEC coefficients defaults for ALM */
         memcpy(&pCmd2[1], alm_nelec_coefs, sizeof(alm_nelec_coefs));
         /* write NLEC coefficients command for ALM */
         if (CmdWrite (pDev, pCmd2, 4) != IFX_SUCCESS)
            return IFX_ERROR;
      }

      if (bEnNlp == IFX_FALSE)
         pCmd1 [2] &= ~ALM_NLP_EN;
      else
         pCmd1 [2] |= ALM_NLP_EN;

      /* only write to the device if the value is different */
      if (pCmd1 [2] != pDev->pAlmCh[ch].ali_nelec)
      {
         /* set ALI NELEC command */
         pCmd1 [1] = ECMD_ALM_NEARLEC;
         if (CmdWrite (pDev, pCmd1, 1) != IFX_SUCCESS)
            return IFX_ERROR;
         pDev->pAlmCh[ch].ali_nelec = pCmd1 [2];
      }
   }
   return IFX_SUCCESS;
}

/**
   Configures the Near LEC in the PCM module
   \param   pCh        - pointer to VINETIC channel structure
   \param   bEnLec     - IFX_FALSE : disable / IFX_TRUE : enable LEC
   \param   bEnNlp     - IFX_FALSE : disable / IFX_TRUE : enable NLP
   \param   bEnWLec    - IFX_FALSE : disable / IFX_TRUE : enable bEnWLec, requires enable LEC

   \return
      IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t Dsp_SetNeLec_Pcm(VINETIC_CHANNEL *pCh,
                             IFX_boolean_t bEnLec,
                             IFX_boolean_t bEnNlp,
                             IFX_boolean_t bEnWLec)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint16_t   ch = (pCh->nChannel - 1);
   IFX_uint16_t   pCmd1[3];
   IFX_uint8_t    lec_res;
   IFX_uint16_t   pCmd2[11];
   static const IFX_uint16_t pcm_nelec_coefs[5] =
   {
      ECMD_NELEC_COEFS, 0x1048, 0x0808, 0x6060, 0x0060
   };
   /* should be used in connection with WLEC-FW only! */
   static const IFX_uint16_t pcm_nelec_wlec_coefs[7] =
   {  /* Recommended WLEC Coefficient Command Parameters:
            LEN         |   POWR       = 0x103D
            DELTAP      |   DELTAQ     = 0x0808
            GAIN_XI     |   GAIN_XO    = 0x6060
            **********  |   Gain_RI    = 0x0060
            LEN_FIX_WIN |   PMW_POWR   = 0x0834
            PMW_DELTAP  |   PMW_DELTAQ = 0x0008 */
      ECMD_NELEC_COEFS, 0x103D, 0x0808, 0x6060, 0x0060, 0x0834, 0x0008
   };
   static const IFX_uint16_t pcm_nelec_nlp[10] =
   {
      ECMD_NELEC_NLP, 0x1502, 0x4540, 0x0C40, 0x4180,
      0x1106, 0x4647, 0x0D10, 0x6480, 0x0810
   };

   /* if WLEC should be used, check WLEC capability and parameters */
   /* WLEC is not available! */
   if ( ((bEnWLec == IFX_TRUE) && (pDev->VinCapabl.bWlec != IFX_TRUE))
        /* enable bEnWLec, requires enable LEC */
        || ((bEnWLec == IFX_TRUE) && (bEnLec != IFX_TRUE))
      )
   {  /* => invalid parameter, error code ERR_FUNC_PARM */
      SET_ERROR (ERR_FUNC_PARM);
      return IFX_ERROR;
   }
   if (ch >= pDev->nPcmCnt)
   {
      /* no pcm channel! */
      SET_ERROR (ERR_FUNC_PARM);
      return IFX_ERROR;
   }

   /* set channel */
   pCmd1 [0] = CMD1_EOP | pCh->nPcmCh;

   /* disable ali nelec if required by application */
   if (bEnLec == IFX_FALSE)
   {
      if (pDev->pPcmCh[ch].pcm_nelec & PCM_NLEC_EN)
      {
         /* update cached value and release LEC resource back to the minimal
            resource management. */
         pDev->availLecRes |= (1 << (pDev->pPcmCh[ch].pcm_nelec & PCM_LECNR));
         pDev->pPcmCh[ch].pcm_nelec &= ~(PCM_NLEC_EN | PCM_NLP_EN | PCM_LECNR);

         /* set PCM NELEC command */
         pCmd1 [1] = ECMD_PCM_NEARLEC;
         pCmd1 [2] = pDev->pPcmCh[ch].pcm_nelec;

         return CmdWrite (pDev, pCmd1, 1);
      }
   }
   else
   {
      /* configure the LEC, get a LEC resource and enable accordingly */
      if (!(pDev->pPcmCh[ch].pcm_nelec & PCM_NLEC_EN))
      {
         /* LEC reource required */
         if (pDev->availLecRes == 0)
         {
            SET_ERROR (ERR_NORESOURCE);
            return IFX_ERROR;
         }

         /* get the LEC resource */
         lec_res = get_free_lec_res(pDev);
         /* check capabilities */
         if (lec_res >= pDev->VinCapabl.nLecRes)
            return IFX_ERROR;
         /* set channel in lec command */
         pCmd1 [2] = pDev->pPcmCh[ch].pcm_nelec | lec_res | PCM_NLEC_EN;
         /* set channel in lec/nlp coefficient command */
         pCmd2 [0] = CMD1_EOP | lec_res;
         /* set NLP coefficients defaults for ALM,
         only needed in this case! */
         memcpy(&pCmd2[1], pcm_nelec_nlp, sizeof(pcm_nelec_nlp));
         if (CmdWrite (pDev, pCmd2, 9) != IFX_SUCCESS)
            return IFX_ERROR;
      }
      else
      {
         /* no LEC resource is required, because LEC is running already */
         pCmd1 [2] = pDev->pPcmCh[ch].pcm_nelec;
         /* get the LEC resource which is used for this channel */
         lec_res = pDev->pPcmCh[ch].pcm_nelec & PCM_LECNR;
         /* check capabilities */
         if (lec_res >= pDev->VinCapabl.nLecRes)
            return IFX_ERROR;
         /* set channel in lec coefficient command */
         pCmd2 [0] = CMD1_EOP | lec_res;
      }
      if ( bEnWLec == IFX_TRUE )    /* activate WLEC feature */
      {  /* Note: WLEC capability check already done!
         WLEC Configuration: Normal operation mode for the window based LEC.
         The window position as well as the filter coefficients are adapted by the LEC.
         OLDP = 0, OLDC = 0
         DCF = 1,
         MW = 1, The de-correlation filter is part of the new WLEC only
         and should be activated. */
         pCmd1 [2] &= ~(PCM_OLDC_EN | PCM_OLDP);
         pCmd1 [2] |= PCM_MW_EN | PCM_DCF_EN;

         /* set NLEC coefficients defaults for PCM */
         memcpy(&pCmd2[1], pcm_nelec_wlec_coefs, sizeof(pcm_nelec_wlec_coefs));
         /* write NLEC coefficients command for ALM */
         if (CmdWrite (pDev, pCmd2, 6) != IFX_SUCCESS)
            return IFX_ERROR;
      }
      else
      {
         /* set NLEC coefficients defaults for PCM */
         memcpy(&pCmd2[1], pcm_nelec_coefs, sizeof(pcm_nelec_coefs));
         if (CmdWrite (pDev, pCmd2, 4) != IFX_SUCCESS)
             return IFX_ERROR;
      }

      if (bEnNlp == IFX_FALSE)
         pCmd1 [2] &= ~PCM_NLP_EN;
      else
         pCmd1 [2] |= PCM_NLP_EN;

      /* only write to the device if the value is different */
      if (pCmd1 [2] != pDev->pPcmCh[ch].pcm_nelec)
      {
         /* set ALI NELEC command */
         pCmd1 [1] = ECMD_PCM_NEARLEC;
         if (CmdWrite (pDev, pCmd1, 1) != IFX_SUCCESS)
            return IFX_ERROR;
         pDev->pPcmCh[ch].pcm_nelec = pCmd1 [2];
      }
   }
   return IFX_SUCCESS;
}

#endif /* (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M | VIN_FEAT_VIN_2CPE) */

