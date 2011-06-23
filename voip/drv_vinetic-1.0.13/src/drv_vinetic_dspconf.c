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

#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))

#include "drv_vinetic_dspconf.h"
#include "drv_vinetic_cram.h"

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */
/** Requested gap for tone holding to maximum 1000 ms sec (value / 10)*/
#define TONEHOLDING_RGAP (1000 / 10)
/** Requested gap for CED detection 120 ms sec (value / 10)*/
#define CED_RGAP         (120 / 10)

/** hamming constant for cpt level calculation */
#define CONST_CPT_HAMMING        (  610)
/** blackman constant for cpt level calculation */
#define CONST_CPT_BLACKMAN       (-1580)
/** define which constant is taken for cpt level calculation */
#define CONST_CPT                CONST_CPT_BLACKMAN

/** CPTD default configuration: total power between 10 and 3400, frame length
    32 ms, hamming window */
#define CPTD_DEFAULT_CONF 0x0100

/* timing tolerance 200 ms */
#define CPTCOEFF_TIM_DEFAULT     0xC800
#define CPTCOEFF_POW_PAUSE       0x0001
#define CPTCOEFF_FP_TP_R_DEFAULT 0x075D
/** minimum time for verification of continuos tone in ms */
#define CPTCOEFF_CNT_MINTIME     1000

/* ============================= */
/*             Enums             */
/* ============================= */

/* ============================= */
/* Local function declaration    */
/* ============================= */

IFX_LOCAL IFX_int16_t cpt_calc_freq       (IFX_uint32_t f);
IFX_LOCAL IFX_int16_t cpt_calc_level      (IFX_int32_t level);
IFX_LOCAL IFX_uint8_t get_free_lec_res    (VINETIC_DEVICE *pDev);
IFX_LOCAL IFX_int32_t Atd_ConfCED         (VINETIC_DEVICE *pDev,
                                           VINETIC_CHANNEL *pCh,
                                           IFX_uint32_t nSignal,
                                           IFX_uint8_t* nAtd);
IFX_LOCAL IFX_int32_t Atd_ConfToneHolding (VINETIC_DEVICE *pDev,
                                           VINETIC_CHANNEL *pCh,
                                           IFX_uint32_t nSignal,
                                           IFX_uint8_t* nAtd);
IFX_LOCAL IFX_int32_t Utd_SetCoeff        (VINETIC_DEVICE *pDev,
                                           IFX_uint32_t freq,
                                           IFX_uint8_t nUtd);
IFX_LOCAL IFX_int32_t Utd_CheckSignals    (VINETIC_DEVICE *pDev,
                                           VINETIC_CHANNEL *pCh,
                                           IFX_uint32_t nSignal,
                                           IFX_uint8_t* nUtd);

/* ============================= */
/* Local variable definition     */
/* ============================= */


/* ============================= */
/* Local function definitions    */
/* ============================= */

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
      coef =   (IFX_int16_t)( C2_15
                    - ((f*f*1011)/100000)
                    + ((f/10)*(f/10)*(f/10)*(f/10) / 192487)
                    - 1 );
   }
   else if (f <= 2000)
   {
      /* sin approximation using the taylor algorithm */
      /* this formula covers the cos from pi/4 to pi/2 */
      f = 2000 - f;
      coef = (IFX_int16_t)(25736 * f)/1000
             - (f*f*f/377948)
             + ((f/20)*(f/20)*(f/20)*(f/20)*(f/20)/3829411);
   }
   else if (f <= 3000)
   {
      /* sin approximation using the taylor algorithm */
      /* this formula covers the cos from pi/2 to 3/4pi */
      f = f - 2000;
      coef = -((25736 * f)/1000
               + (-f*f*f/377948)
               + ((f/20)*(f/20)*(f/20)*(f/20)*(f/20)/3829411)
               );
   }
   else if (f <= 4000)
   {
      /* cos approximation using the taylor algorithm */
      /* this formula covers the cos from 3/4 pi to pi */
      f = 4000 - f;
      coef = -( C2_15
                    - ((f*f*1011)/100000)
                    + ((f/10)*(f/10)*(f/10)*(f/10) / 192487)
             );
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
   IFX_uint32_t exp, tenExp;
   IFX_uint32_t shift;
   IFX_int32_t  i;

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
      while (exp >= tens[i][0])
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
         exp -= tens[i][0];
      }
   }

   /* calculate the coefficient according to the specification... */
   return (IFX_int16_t) ROUND (((C2_14*shift)/(tenExp)));
}


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

   pDev->availLecRes &= ~(1 << i);

   return i;
}


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
   \param nAtd - number of ATD resource: 0 .. 7 to configure
   \return
      - IFX_ERROR if error
      - IFX_SUCCESS if successful
   \remarks
   This function is used in Dsp_AtdConf for CED settings
*/
IFX_LOCAL IFX_int32_t Atd_SetCoeff (VINETIC_DEVICE *pDev,
                                    IFX_uint16_t nRgap, IFX_uint8_t nAtd)
{
   IFX_uint16_t pCmd[CMD_SIG_ATDCOEFF_LEN + CMD_HEADER_CNT];

   pCmd [0] = CMD1_EOP | nAtd;
   pCmd [1] = ECMD_ATD_COEF;

   /* programming default */
   pCmd [2] = 0x0123;
   pCmd [3] = 0x2E6A;
   pCmd [4] = 0x4026;
   pCmd [5] = 0x00CF;
   pCmd [6] = 0x0A16;
   pCmd [7] = 0x0A00 | nRgap;

   return CmdWrite (pDev, pCmd, CMD_SIG_ATDCOEFF_LEN);
}

/**
   Helper function to set UTD coefficients
   \param pDev - pointer to the device structure
   \param pCh - pointer to the channel structure retrieved from pDev in
                the calling function
   \param nSignal - signal definition from \ref TAPI_SIGNAL related to UTD
   \param nUtd - returns the number of UTD resource that were configured
   \return
      - IFX_ERROR if error
      - IFX_SUCCESS if successful
   \remarks
   This function is used in Dsp_UtdConf for CED settings
*/
IFX_LOCAL IFX_int32_t Utd_CheckSignals (VINETIC_DEVICE *pDev, VINETIC_CHANNEL *pCh,
                                        IFX_uint32_t nSignal, IFX_uint8_t* nUtd)
{
   IFX_uint8_t ch = pCh->nChannel - 1,
                    /* signal input source to configure */
                    input;
   IFX_int32_t err = IFX_SUCCESS;
   /* frequency to program */
   IFX_uint32_t freq,
               /* signal to configure for each loop */
               tmpSig;

   *nUtd = 0;
   /* check signal until no further signal to be detect, maybe zero.
      For each run the current configured signal is removed from nSignal */
   while ((nSignal != 0) && err == IFX_SUCCESS && (*nUtd) < 2)
   {
      tmpSig = 0;
      freq = 1100;
      /* configure both paths per default */
      input = VIN_ECMD_SIGUTD_IS_SIGINBOTH;
      /* check first signal and mask it from the signal given.
         Note: nSignal could contain more signals (also ATDs) to detect.
         The current signal for the UTD is stored in tmpSig */
      if (nSignal & TAPI_SIGNAL_CNGFAXMASK)
      {
         /* get the current signal */
         if (nSignal & TAPI_SIGNAL_CNGFAX)
         {
            tmpSig = nSignal & TAPI_SIGNAL_CNGFAX;
            /* clear the signal */
            nSignal &= ~TAPI_SIGNAL_CNGFAX;
         }
         else
         {
            /* detect only one specific path. UTD1 becomes transmit (ALI) */
            if (nSignal & TAPI_SIGNAL_CNGFAXTX)
            {
               /* the last possibility is TX */
               tmpSig = nSignal & TAPI_SIGNAL_CNGFAXTX;
               /* clear the signal */
               nSignal &= ~TAPI_SIGNAL_CNGFAXTX;
               input = VIN_ECMD_SIGUTD_IS_SIGINA;
            }
            else
            {
               tmpSig = nSignal & TAPI_SIGNAL_CNGFAXRX;
               /* clear the signal */
               nSignal &= ~TAPI_SIGNAL_CNGFAXRX;
               input = VIN_ECMD_SIGUTD_IS_SIGINB;
            }
         }
      }
      else if (nSignal & TAPI_SIGNAL_CNGMODMASK)
      {
         /* modem CNG signal detection */
         freq = 1300;
         /* get the current signal */
         if (nSignal & TAPI_SIGNAL_CNGMOD)
         {
            tmpSig = nSignal & TAPI_SIGNAL_CNGMOD;
            /* clear the signal */
            nSignal &= ~TAPI_SIGNAL_CNGMOD;
         }
         else
         {
            /* detect only one specific path */
            if (nSignal & TAPI_SIGNAL_CNGMODTX)
            {
               /* the last possibility is TX */
               tmpSig = nSignal & TAPI_SIGNAL_CNGMODTX;
               /* clear the signal */
               nSignal &= ~TAPI_SIGNAL_CNGMODTX;
               input = VIN_ECMD_SIGUTD_IS_SIGINA;
            }
            else
            {
               tmpSig = nSignal & TAPI_SIGNAL_CNGMODRX;
               /* clear the signal */
               nSignal &= ~TAPI_SIGNAL_CNGMODRX;
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
         else
         {
            if (pDev->pSigCh[ch].sig_utd2.bit.en == 1)
            {
               pDev->pSigCh[ch].sig_utd2.bit.en = 0;
               err = CmdWrite (pDev, pDev->pSigCh[ch].sig_utd2.value, CMD_SIG_UTD_LEN);
            }
            pDev->pSigCh[ch].sig_utd2.bit.is = input;
         }
         /* set the signal type detection for the corresponding UTD */
         pCh->nUtdSig[*nUtd] = tmpSig;

         if (err == IFX_SUCCESS &&
             Utd_SetCoeff (pDev, freq,
                           (IFX_uint8_t)(ch + 4 * (*nUtd))) != IFX_SUCCESS)
             break;
      }
      (*nUtd)++;
   } /* of while */
   return err;
}

/**
   Helper function to activate ATDs

   \param pDev - pointer to the device structure
   \param pCh  - pointer to VINETIC channel structure
   \param nSignal - signal definition
   \param nAtd    - returns the number of ATDs used
   \param atdVal - reference value of the ATD configuration from the firmware
                   structure given by the calling function. Will be modified
                   by this function
   \return
      - IFX_ERROR if error
      - IFX_SUCCESS if successful
   \remarks
   This function is used in Dsp_AtdConf for CED settings
*/
IFX_LOCAL IFX_int32_t Atd_Activate (VINETIC_DEVICE *pDev, VINETIC_CHANNEL *pCh,
                                    IFX_uint32_t nSignal, IFX_uint8_t* nAtd, IFX_uint16_t *atdVal)
{
   IFX_uint8_t ch = pCh->nChannel - 1;

   if (nSignal & TAPI_SIGNAL_CEDMASK)
   {
      if (nSignal & TAPI_SIGNAL_CEDENDMASK)
      {
         /* unmask setting for interrupt both edges */
         if ((*nAtd) == 0)
         {
            pDev->regMFSRE2[ch] &= ~SRE2_ATD1_DT;
            pDev->regMRSRE2[ch] &= ~SRE2_ATD1_DT;
         }
         else
         {
            pDev->regMFSRE2[ch] &= ~SRE2_ATD2_DT;
            pDev->regMRSRE2[ch] &= ~SRE2_ATD2_DT;
         }
      }
      else
      {
         /* mask setting for interrupt 1 -> 0 cause no end should
            be reported */
         if ((*nAtd) == 0)
         {
            pDev->regMRSRE2[ch] &= ~SRE2_ATD1_DT;
            pDev->regMFSRE2[ch] |= SRE2_ATD1_DT;
         }
         else
         {
            pDev->regMRSRE2[ch] &= ~SRE2_ATD2_DT;
            pDev->regMFSRE2[ch] |= SRE2_ATD2_DT;
         }
      }

      /* detection of phase reversals is always done
         For AM and phase reversal: no specific path can be set */
      if (nSignal & TAPI_SIGNAL_AM)
      {
         /* enable amplitude modulation */
         VIN_ECMD_SIGATD_MD_SET ((*atdVal), VIN_ECMD_SIGATD_MD_ANS_PHREV_AM);
         if ((*nAtd) == 0)
            pDev->regMRSRE2[ch] &= ~SRE2_ATD1_AM;
         else
            pDev->regMRSRE2[ch] &= ~SRE2_ATD2_AM;
      }
      else
      {
         VIN_ECMD_SIGATD_MD_SET ((*atdVal), VIN_ECMD_SIGATD_MD_ANS_PHREV);
         if ((*nAtd) == 0)
            pDev->regMRSRE2[ch] |= SRE2_ATD1_AM;
         else
            pDev->regMRSRE2[ch] |= SRE2_ATD2_AM;
      }
      if (nSignal & TAPI_SIGNAL_PHASEREV)
      {
         /* enable phase reversal interrupt */
         /* Interrupt will be generated on the first and third occurring phase reversal */
         if ((*nAtd) == 0)
            pDev->regMRSRE2[ch] &= ~SRE2_ATD1_NPR_1_REV;
         else
            pDev->regMRSRE2[ch] &= ~SRE2_ATD2_NPR_1_REV;
      }
      else
      {
         /* disable phase reversal interrupt */
         if ((*nAtd) == 0)
            pDev->regMRSRE2[ch] |= SRE2_ATD1_NPR;
         else
            pDev->regMRSRE2[ch] |= SRE2_ATD2_NPR;
      }
   }
   else
   {
      /* enable CED end only detection */
      VIN_ECMD_SIGATD_MD_SET ((*atdVal), VIN_ECMD_SIGATD_MD_ANS_PHREV);
      /* unmask setting for interrupt 1 -> 0  */
      if ((*nAtd) == 0)
      {
         pDev->regMRSRE2[ch] |= SRE2_ATD1_DT;
         pDev->regMFSRE2[ch] &= ~SRE2_ATD1_DT;
      }
      else
      {
         pDev->regMRSRE2[ch] |= SRE2_ATD2_DT;
         pDev->regMFSRE2[ch] &= ~SRE2_ATD2_DT;
      }
   }
   return IFX_SUCCESS;
}


/**
   Helper function to set CED and related settings

   \param pDev - pointer to the device structure
   \param pCh  - pointer to VINETIC channel structure
   \param nSignal - signal definition
   \param nAtd    - number of ATDs used
   \return
      - IFX_ERROR if error
      - IFX_SUCCESS if successful
   \remarks
   This function is used in Dsp_AtdConf for CED settings
*/
IFX_LOCAL IFX_int32_t Atd_ConfCED (VINETIC_DEVICE *pDev, VINETIC_CHANNEL *pCh,
                                   IFX_uint32_t nSignal, IFX_uint8_t* nAtd)
{
   /* signal to configure for each loop */
   IFX_uint32_t tmpSig;
   IFX_uint16_t atdVal;
   IFX_uint8_t ch = pCh->nChannel - 1;

   /* check signal until no further signal to be detect, maybe zero.
      For each run the current configured signal is removed from nSignal */
   (*nAtd) = 0;
   while ((nSignal & (TAPI_SIGNAL_CEDMASK | TAPI_SIGNAL_CEDENDMASK)) != 0)
   {
      tmpSig = 0;
      if ((*nAtd) == 0)
         atdVal = pDev->pSigCh[ch].sig_atd1.value[CMD_HEADER_CNT];
      else
         atdVal = pDev->pSigCh[ch].sig_atd2.value[CMD_HEADER_CNT];

      if (nSignal & TAPI_SIGNAL_CED || nSignal & TAPI_SIGNAL_CEDEND)
      {
         /* enable both paths */
         VIN_ECMD_SIGATD_IS_SET (atdVal,
                                 VIN_ECMD_SIGATD_IS_SIGINBOTH);
         tmpSig = nSignal & (TAPI_SIGNAL_CED | TAPI_SIGNAL_CEDEND |
                             TAPI_SIGNAL_PHASEREVMASK | TAPI_SIGNAL_AMMASK);
         /* clear the signal */
         nSignal &= ~(TAPI_SIGNAL_CED | TAPI_SIGNAL_CEDEND);
      }
      else
      {
         /* enable only one path */
         if (nSignal & (TAPI_SIGNAL_CEDENDTX | TAPI_SIGNAL_CEDTX))
         {
            /* for transmit path input A is used. Selection of ATD needed */
            VIN_ECMD_SIGATD_IS_SET (atdVal, VIN_ECMD_SIGATD_IS_SIGINA);
            tmpSig = nSignal & (TAPI_SIGNAL_CEDTX | TAPI_SIGNAL_CEDENDTX |
                                TAPI_SIGNAL_PHASEREVMASK | TAPI_SIGNAL_AMMASK);
            /* clear the signal */
            nSignal &= ~(TAPI_SIGNAL_CEDTX | TAPI_SIGNAL_CEDENDTX);
         }
         else if (nSignal & (TAPI_SIGNAL_CEDENDRX | TAPI_SIGNAL_CEDRX))
         {
            /* for receive path input B is used. Only ATD1 is possible */
            VIN_ECMD_SIGATD_IS_SET (atdVal,
                                    VIN_ECMD_SIGATD_IS_SIGINB);
            tmpSig = nSignal & (TAPI_SIGNAL_CEDRX | TAPI_SIGNAL_CEDENDRX |
                             TAPI_SIGNAL_PHASEREVMASK | TAPI_SIGNAL_AMMASK);
            /* clear the signal */
            nSignal &= ~(TAPI_SIGNAL_CEDRX | TAPI_SIGNAL_CEDENDRX);
         }
      }
      Atd_Activate (pDev, pCh, tmpSig, nAtd, &atdVal);

      if ((*nAtd) == 0)
         pDev->pSigCh[ch].sig_atd1.value[CMD_HEADER_CNT] = atdVal;
      else
         pDev->pSigCh[ch].sig_atd2.value[CMD_HEADER_CNT] = atdVal;
      /* detection time 210 ms */
      Atd_SetCoeff (pDev, CED_RGAP, (IFX_uint8_t)(ch + 4 * (*nAtd)));
      (*nAtd)++;
   } /* of while */
   return IFX_SUCCESS;
}

/**
   Helper function to set signal level detection

   \param pDev - pointer to the device structure
   \param pCh  - pointer to VINETIC channel structure
   \param nSignal - signal definition
   \param nAtd - returns the number of the ATD, that were configured
   \return
      - IFX_ERROR if error
      - IFX_SUCCESS if successful
   \remarks
   This function is used in Dsp_AtdConf for signal level, so called
   tone holding detection
*/
IFX_LOCAL IFX_int32_t Atd_ConfToneHolding (VINETIC_DEVICE *pDev, VINETIC_CHANNEL *pCh,
                                           IFX_uint32_t nSignal, IFX_uint8_t* nAtd)
{
   IFX_int32_t  ch = pCh->nChannel - 1;
   IFX_uint16_t atdVal;

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
      if ((nSignal & TAPI_SIGNAL_TONEHOLDING_END) != 0)
      {
         VIN_ECMD_SIGATD_IS_SET (atdVal, VIN_ECMD_SIGATD_IS_SIGINBOTH);
         /* unmask setting for interrupt 1 -> 0 only for ATD1 */
         pDev->regMFSRE2[ch] &= ~SRE2_ATD1_DT;
         pDev->regMRSRE2[ch] |= SRE2_ATD1_DT;
         /* clear the signal */
         nSignal &= ~(TAPI_SIGNAL_TONEHOLDING_END);
      }
      else
      {
         /* enable both signals */
         pDev->regMFSRE2[ch] |= (SRE2_ATD1_DT | SRE2_ATD2_DT);
         if (nSignal & TAPI_SIGNAL_TONEHOLDING_ENDRX)
         {
            /* for receive path input B is used */
            VIN_ECMD_SIGATD_IS_SET (atdVal, VIN_ECMD_SIGATD_IS_SIGINB);
            pDev->regMFSRE2[ch] &= ~SRE2_ATD1_DT;
            /* clear the signal */
            nSignal &= ~(TAPI_SIGNAL_TONEHOLDING_ENDRX);
         }
         else if (nSignal & TAPI_SIGNAL_TONEHOLDING_ENDTX)
         {
            /* for transmit path input A is used */
            VIN_ECMD_SIGATD_IS_SET (atdVal, VIN_ECMD_SIGATD_IS_SIGINA);
            if (*nAtd == 0)
               pDev->regMFSRE2[ch] &= ~SRE2_ATD1_DT;
            else
               pDev->regMFSRE2[ch] &= ~SRE2_ATD2_DT;
            /* clear the signal */
            nSignal &= ~(TAPI_SIGNAL_TONEHOLDING_ENDTX);
         }
      }
      if ((*nAtd) == 0)
         pDev->pSigCh[ch].sig_atd1.value[CMD_HEADER_CNT] = atdVal;
      else
         pDev->pSigCh[ch].sig_atd2.value[CMD_HEADER_CNT] = atdVal;
      /* detection time 1000 ms */
      Atd_SetCoeff (pDev, TONEHOLDING_RGAP, (IFX_uint8_t)((pCh->nChannel - 1) + 4 * (*nAtd)));

      (*nAtd)++;
   }
   return IFX_SUCCESS;
}

/* ============================= */
/* Global function definitions   */
/* ============================= */


/**
   Disables or Enables CID Sender according to bEn

   \param   pCh  - pointer to VINETIC channel structure
   \param   bEn  - IFX_TRUE : enable / IFX_FALSE : disable

   \return
      IFX_SUCCESS or IFX_ERROR

*/
IFX_int32_t Dsp_SetCidSender (VINETIC_CHANNEL *pCh, IFX_boolean_t bEn)
{
   IFX_int32_t ret = IFX_SUCCESS;
   IFX_uint16_t pCmd [2] = {0}, pData [3] = {0};
   VINETIC_DEVICE *pDev = pCh->pParent;

   /* EOP Cmd */
   pCmd [0] = (CMD1_EOP | (pCh->nChannel - 1));
   /* CID SEND */
   pCmd [1] = ECMD_CID_SEND;
   ret = CmdRead (pDev, pCmd, pData, 1);
   if (ret == IFX_SUCCESS)
   {
      if ((bEn == IFX_TRUE) && !(pData [2] & SIG_CID_EN))
      {
         pData [2] |= SIG_CID_EN;
         ret = CmdWrite (pDev, pData, 1);
      }
      else if ((bEn == IFX_FALSE) && (pData [2] & SIG_CID_EN))
      {
         pData [2] &= ~SIG_CID_EN;
         ret = CmdWrite (pDev, pData, 1);
      }
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
      if (ret == IFX_SUCCESS && pDev->pSigCh[ch].sig_utd2.bit.en == 1)
      {
         pDev->pSigCh[ch].sig_utd2.bit.en = 0;
         ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_utd2.value, CMD_SIG_UTD_LEN);
      }
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
      if (ret == IFX_SUCCESS && pDev->pSigCh[ch].sig_utd2.bit.en == 0)
      {
         pDev->pSigCh[ch].sig_utd2.bit.en = 1;
         ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_utd2.value, CMD_SIG_UTD_LEN);
      }

   }
   return ret;
}

/**
   Enables signal detection on universal tone detector

   \param pCh  - pointer to VINETIC channel structure
   \param nSignal - signal definition from \ref TAPI_SIGNAL
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
             (TAPI_SIGNAL_CNGFAXMASK | TAPI_SIGNAL_CNGMODMASK);

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
  \param nSignal - signal to detect. combination of TAPI_SIGNAL_CNGFAX,
                  TAPI_SIGNAL_CNGFAXRX, TAPI_SIGNAL_CNGFAXTX,
                  TAPI_SIGNAL_CNGMOD, TAPI_SIGNAL_CNGMODRX and
                  TAPI_SIGNAL_CNGMODTX
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

   /* check the signals for both UTDs. The number of UTDs to configure
      is returned in nUtd  */
   err = Utd_CheckSignals (pDev, pCh, nSignal, &nUtd);
   /* program enable bits and interrupts. input signals are
      already programmed in the while loop */
   switch (nUtd)
   {
      case  0:
         /* no UTD resource used */
         pDev->pSigCh[ch].sig_utd1.bit.en = 0;
         pDev->pSigCh[ch].sig_utd2.bit.en = 0;
         /* unmask interrupt 0 -> 1  */
         pDev->regMRSRE1[ch] |= (SRE1_UTD1_OK | SRE1_UTD2_OK);
         break;
      case  1:
         /* first UTD used */
         pDev->pSigCh[ch].sig_utd1.bit.en = 1;
         pDev->pSigCh[ch].sig_utd2.bit.en = 0;
         /* unmask interrupt 0 -> 1  */
         pDev->regMRSRE1[ch] &= ~SRE1_UTD1_OK;
         pDev->regMRSRE1[ch] |= SRE1_UTD2_OK;
         break;
      default:
         /* both UTDs are used */
         pDev->pSigCh[ch].sig_utd1.bit.en = 1;
         pDev->pSigCh[ch].sig_utd2.bit.en = 1;
         /* unmask interrupt 0 -> 1  */
         pDev->regMRSRE1[ch] &= ~(SRE1_UTD1_OK | SRE1_UTD2_OK);
         break;
   }

   if (err == IFX_SUCCESS)
      err = RegWrite(pDev, CMD1_IOP | ch, &pDev->regMRSRE1[ch],1, MRSRE1);

   if (err == IFX_SUCCESS)
      err = CmdWrite (pDev, pDev->pSigCh[ch].sig_utd1.value, CMD_SIG_UTD_LEN);

   if (err == IFX_SUCCESS)
      err = CmdWrite (pDev, pDev->pSigCh[ch].sig_utd2.value, CMD_SIG_UTD_LEN);

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

   /* check ATD1 configuration **********************************************/
   /* CED detection */
#if 0
   no secrurity check to enable phase rev and am later on
   if (nSignal & (TAPI_SIGNAL_AM | TAPI_SIGNAL_PHASEREV) &&
       !((nSignal & TAPI_SIGNAL_CEDTX) || (nSignal & TAPI_SIGNAL_CEDRX)))
      /* amplitude modulation and phase reversals require CED detection.
         no CED detection was enabled, so we enable both paths */
      nSignal |= TAPI_SIGNAL_CED;
#endif /* 0 */

   /* set temporary mask for sanity checks */
   sigMask |= nSignal;
   /* check if CED receive and other transmit should be set */
   if ( (sigMask & (TAPI_SIGNAL_CEDRX | TAPI_SIGNAL_AMRX |
                    TAPI_SIGNAL_PHASEREVRX | TAPI_SIGNAL_CEDENDRX)) &&
        (sigMask & (TAPI_SIGNAL_CEDTX | TAPI_SIGNAL_AMTX |
                    TAPI_SIGNAL_PHASEREVTX | TAPI_SIGNAL_CEDENDTX)) )
   {
      SET_ERROR (ERR_NOTSUPPORTED);
      return IFX_ERROR;
   }

   /* check if tone holding or CED is to be enabled */
   if ((sigMask & TAPI_SIGNAL_TONEHOLDING_ENDMASK) &&
       (sigMask & TAPI_SIGNAL_CEDMASK))
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
IFX_int32_t Dsp_AtdConf (VINETIC_CHANNEL *pCh, IFX_uint32_t nSignal)
{
   IFX_int32_t err = IFX_SUCCESS;
   IFX_uint8_t ch = pCh->nChannel - 1;
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint8_t nAtd;

   nAtd = 0;
   if (!(nSignal & (TAPI_SIGNAL_CEDMASK | TAPI_SIGNAL_CEDENDMASK |
                   TAPI_SIGNAL_PHASEREVMASK | TAPI_SIGNAL_AMMASK |
                   TAPI_SIGNAL_TONEHOLDING_ENDMASK)))
   {
      /* no ATD1 use */
      pDev->regMFSRE2[ch] |= (SRE2_ATD1_DT | SRE2_ATD1_NPR | SRE2_ATD1_AM);
      pDev->regMRSRE2[ch] |= (SRE2_ATD1_DT | SRE2_ATD1_NPR | SRE2_ATD1_AM);
      pDev->pSigCh[ch].sig_atd1.bit.en = 0;
   }
   else
   {
      /* as a workaround of a CERR here we first disable the ATDs
         before enabling them again */
      pDev->pSigCh[ch].sig_atd1.bit.en = 0;
      err = CmdWrite (pDev, (IFX_uint16_t*)&pDev->pSigCh[ch].sig_atd1, CMD_SIG_ATD_LEN); /* disable ADT1 */

      pDev->pSigCh[ch].sig_atd2.bit.en = 0;
      err = CmdWrite (pDev, (IFX_uint16_t*)&pDev->pSigCh[ch].sig_atd2, CMD_SIG_ATD_LEN); /* disable ADT2 */
   }

   if (((nSignal & TAPI_SIGNAL_CEDMASK) != 0) ||
       ((nSignal & TAPI_SIGNAL_CEDENDMASK) != 0))
   {
      /* modify ATDs and interrupt masks according nSingal */
      err = Atd_ConfCED(pDev, pCh, nSignal, &nAtd);
   }
   /* handling for tone holding */
   if ((nSignal & TAPI_SIGNAL_TONEHOLDING_ENDMASK) != 0)
   {
      if (nAtd == 2)
      {
         SET_ERROR (ERR_NORESOURCE);
         err = IFX_ERROR;
      }
      else
         err = Atd_ConfToneHolding (pDev, pCh,
                                    nSignal & TAPI_SIGNAL_TONEHOLDING_ENDMASK,
                                    &nAtd);
   }
   /* if both used, set to enable option 3 (means both).
      Atd_ConfToneHolding and Atd_ConfCED return the number of used
      ATDs. Here a switching to bit wise use is done, to make enabling easier */
   if (nAtd == 2)
      nAtd = 3;
   /* currently DIS is only detected on ATD2 */
   if ((nSignal & TAPI_SIGNAL_DISMASK) != 0)
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

         if (nSignal & TAPI_SIGNAL_DIS)
            pDev->pSigCh[ch].sig_atd2.bit.is = VIN_ECMD_SIGATD_IS_SIGINBOTH;
         else
         {
            if (nSignal & TAPI_SIGNAL_DISRX)
               /* for receive path input B is used */
               pDev->pSigCh[ch].sig_atd2.bit.is = VIN_ECMD_SIGATD_IS_SIGINB;
            if (nSignal & TAPI_SIGNAL_DISTX)
               /* for transmit path input A is used */
               pDev->pSigCh[ch].sig_atd2.bit.is = VIN_ECMD_SIGATD_IS_SIGINA;
         }
         /* unmask setting for interrupt 0 -> 1  */
         pDev->regMRSRE2[ch] &= ~SRE2_ATD2_DT;
      }
   }
   else
   {
      /* no DIS and no other on ATD2 */
      if (nAtd == 1)
         pDev->regMRSRE2[ch] |= SRE2_ATD2_DT;
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
   /* write interrupt masks */
   if (err == IFX_SUCCESS)
      err = RegWrite(pDev, CMD1_IOP | ch, &pDev->regMRSRE2[ch],1, MRSRE2);
   if (err == IFX_SUCCESS)
      err = RegWrite(pDev, CMD1_IOP | ch, &pDev->regMFSRE2[ch],1, MFSRE2);
   /* write ATD configuration */
   if (err == IFX_SUCCESS)
      err = CmdWrite (pDev, pDev->pSigCh[ch].sig_atd1.value, CMD_SIG_ATD_LEN);
   if (err == IFX_SUCCESS)
      err = CmdWrite (pDev, pDev->pSigCh[ch].sig_atd2.value, CMD_SIG_ATD_LEN);

   /* Saved SRE2 is cleared upon ATD submodule reconfiguration, i.e. execution
    * of either  IFXPHONE_DISABLE_SIGDETECT or IFXPHONE_ENABLE_SIGDETECT. It is
    * the responsibility of the developer using the signaling functionality to
    * reconfigure the ATD submodule in order to clear the cached SRE2 value. This
    * is part of a workaround needed, since bits ATD1_AM and ATD2_AM of register
    * SRE2 cannot be cleared through reading SRE2 register.
    */
   pCh->regSRE2 = 0;

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
IFX_int32_t Dsp_Cptd_SetCoeff (VINETIC_CHANNEL *pCh, TAPI_SIMPLE_TONE const *pTone, IFX_uint16_t *pCmd)
{
   IFX_uint8_t nr = 0;
   IFX_uint8_t step = 10, i;

   /* default settings */
   /* program allowed twist to 10.28 dB */
   pCmd[10] = 0x6666;
   pCmd[19] = CPTCOEFF_TIM_DEFAULT;
   pCmd[20] = CPTCOEFF_POW_PAUSE;
   pCmd[21] = CPTCOEFF_FP_TP_R_DEFAULT;

   /* set frequency and level A for F_1. Freq A is always set. Tone API
      assures it */
   pCmd [2] = (IFX_uint16_t)cpt_calc_freq   (pTone->freqA);
   pCmd [6] = (IFX_uint16_t)cpt_calc_level  (pTone->levelA);
   /* set frequency and level B  for F_2 */
   if (pTone->freqB)
   {
      pCmd [3] = (IFX_uint16_t)cpt_calc_freq   (pTone->freqB);
      pCmd [7] = (IFX_uint16_t)cpt_calc_level  (pTone->levelB);
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
      pCmd [8] = (IFX_uint16_t)cpt_calc_level  (pTone->levelC);
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
      pCmd [9] = (IFX_uint16_t)cpt_calc_level  (pTone->levelD);
   }
   else
   {
      pCmd [5] = VIN_ECMD_SIGCPTCOEFF_FX_0HZ;
      pCmd [9] = VIN_ECMD_SIGCPTCOEFF_LEVEL_0DB;
   }
   /* set step times:  T_x */
   for (i = 0; i < TAPI_TONE_MAXSTEPS; ++i)
   {
      /* check for last cadence step */
      if (pTone->cadence[i] == 0)
         break;
      /* increase step to timing. Step is initialized with 12 (one before
         the first timing setting */
      step++;
      pCmd [step] = (IFX_uint16_t)(pTone->cadence[i]);
      /* move step to step setting */
      step++;
      if ((pTone->frequencies[i] & TAPI_TONE_FREQALL) == 0)
      {
         pCmd [step] = VIN_ECMD_SIGCPTCOEFF_P;
      }
      else
      {
         /* set mask for MSK_i - use frequency A. Initialize the field */
         if (pTone->frequencies[i] & TAPI_TONE_FREQA)
            pCmd [step] = VIN_ECMD_SIGCPTCOEFF_F1;
         else
            pCmd [step] = 0;
         /* set mask for MSK_i - use frequency B  */
         if (pTone->frequencies[i] & TAPI_TONE_FREQB)
            pCmd [step] |= VIN_ECMD_SIGCPTCOEFF_F2;
         /* set mask for MSK_i - use frequency C  */
         if (pTone->frequencies[i] & TAPI_TONE_FREQC)
            pCmd [step] |= VIN_ECMD_SIGCPTCOEFF_F3;
         /* set mask for MSK_i - use frequency D  */
         if (pTone->frequencies[i] & TAPI_TONE_FREQD)
            pCmd [step] |= VIN_ECMD_SIGCPTCOEFF_F4;
      }
      nr++;
   }
   if (pTone->cadence[1] == 0 && pTone->pause == 0)
   {
      /* continuos tone, program time for verification */
      pCmd [step-1] = CPTCOEFF_CNT_MINTIME;
      /* return because nothing more to do */
      return IFX_SUCCESS;
   }
   /* set end of steps */
   pCmd[step] |= VIN_ECMD_SIGCPTCOEFF_E;
   pCmd [19] |= nr;

   return IFX_SUCCESS;
}

/**
   Configures the call progress tone detector
   \param pCh  - pointer to VINETIC channel structure
   \param pTone - pointer to the complex tone to detect
   \param pCmd - pointer to the command structure to fill
   \return
      IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t Dsp_Cptd_Conf (VINETIC_CHANNEL *pCh, TAPI_SIMPLE_TONE const *pTone,
                   IFX_int32_t signal, IFX_uint16_t *pCmd)
{
   VINETIC_DEVICE    *pDev    = (VINETIC_DEVICE *)pCh->pParent;

   if (pDev->pCodCh[3].cod_ch.bit.en == 1 &&
       pDev->nChipMajorRev == VINETIC_V1x)
   {
      SET_ERROR(ERR_NORESOURCE);
      return IFX_ERROR;
   }
   /* Set up CPT command  */
   pCmd[2] = CPTD_DEFAULT_CONF;
   /* set resource */
   VIN_ECMD_SIGCPT_CPTNR_SET (pCmd[2], pCh->nChannel - 1);
   /* CPTD is enabled */
   VIN_ECMD_SIGCPT_EN_SET (pCmd[2], 1);
   /* check for continuos tone */
   if (pTone->cadence[1] == 0)
   {
      VIN_ECMD_SIGCPT_CNT_SET (pCmd[2], 1);
   }
   switch (signal)
   {
      case  TAPI_CPT_SIGNALRX:
         /* for receive path input B is used */
         VIN_ECMD_SIGCPT_IS_SET (pCmd[2], VIN_ECMD_SIGCPT_IS_SIGINB);
      break;
      case  TAPI_CPT_SIGNALTX:
         VIN_ECMD_SIGCPT_IS_SET (pCmd[2], VIN_ECMD_SIGCPT_IS_SIGINA);
      break;
      default:
         SET_ERROR(ERR_FUNC_PARM);
         return IFX_ERROR;
   }
   /* remember CPT settings and enable status for resource management */
   pDev->pSigCh[pCh->nChannel - 1].nCpt = pCmd[2];
   return IFX_SUCCESS;
}


/**
   Firmware DTMF generator configuration

   \param
      pChannel - handle to VINETIC_CHANNEL structure
   \param
      nLevel1 - Gain level for frequency 1, Level = 128 * 10^(Level[dB]/20)
                with -20dB < Level < 0dB
   \param
      nLevel2 - Gain level for frequency 2, Level = 128 * 10^(Level[dB]/20)
                with -20dB < Level < 0dB
   \param
      nTIMT - DTMF tone on time, TIM=TIM[ms]*2 with 0ms < TIM < 127ms
   \param
      nTIMP - DTMF tone pause, TIM=TIM[ms]*2 with 0ms < TIM < 127ms
   \param
      nAttA1 - Output attenuation for adder 1, ATT=ATT[dB]
               with -127dB <= ATT <= 0dB
   \param
      nAttA2 - Output attenuation for adder 2 with -127dB <= ATT <= 0dB
   \return
      IFX_SUCCESS or IFX_ERROR
   \remarks
      Disables DTMF generator during configuration and leave values set to 0xFF
      untouched. See description of DTMF/AT Generator, Coefficients message for
      more details on parameters.
*/
IFX_int32_t Dsp_DtmfConf(VINETIC_CHANNEL *pCh, IFX_uint8_t nLevel1,
                         IFX_uint8_t nLevel2, IFX_uint8_t nTIMT,
                         IFX_uint8_t nTIMP, IFX_uint8_t nAttA1,
                         IFX_uint8_t nAttA2)
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
      if (nLevel1 != 0xFF)
         pData[2] = (pData[2] & 0x00FF) | (nLevel1 << 8);
      if (nLevel2 != 0xFF)
         pData[2] = (pData[2] & 0xFF00) | nLevel2;
      if (nTIMT != 0xFF)
         pData[3] = (pData[3] & 0x00FF) | (nTIMT << 8);
      if (nTIMP != 0xFF)
         pData[3] = (pData[3] & 0xFF00) | nTIMP;
      if (nAttA1 != 0xFF)
         pData[4] = (pData[4] & 0x00FF) | (nAttA1 << 8);
      if (nAttA2 != 0xFF)
         pData[4] = (pData[4] & 0xFF00) | nAttA2;
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
      pDev  - pointer to the device interface
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
   \todo Use of function pointer to CmdWrite or CmdWriteIsr
*/
IFX_int32_t Dsp_DtmfGen (VINETIC_CHANNEL *pCh, IFX_uint16_t nMode,
                         IFX_uint16_t nMask, IFX_boolean_t nIsr)
{
   IFX_int32_t    err      = IFX_SUCCESS;
   VINETIC_DEVICE *pDev    = pCh->pParent;
   IFX_uint8_t ch = pCh->nChannel - 1;

   pDev->pSigCh[ch].sig_dtmfgen.value[CMD_HEADER_CNT] =
      (pDev->pSigCh[ch].sig_dtmfgen.value[CMD_HEADER_CNT] & ~nMask) | (nMode & nMask);
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
   IFX_int32_t ret = IFX_SUCCESS;
   IFX_uint16_t pCmd [7] = {0};
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint8_t ch = (pCh->nChannel - 1), count;

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

   ret = CmdWrite (pDev, pCmd, count);

#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
   /* set fax datapump status in case of success */
   if (ret == IFX_SUCCESS)
   {
      pCh->pTapiCh->TapiFaxStatus.nStatus |=
                ((bEn == IFX_TRUE) ? TAPI_FAX_T38_DP_ON : TAPI_FAX_T38_DP_OFF);
   }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38) */

   return ret;
}


/**
   Configures the Near LEC in the analog module
   \param   pCh        - pointer to VINETIC channel structure
   \param   bEnLec     - IFX_FALSE : disable / IFX_TRUE : enable LEC
   \param   bEnNlp     - IFX_FALSE : disable / IFX_TRUE : enable NLP

   \return
      IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t Dsp_SetNeLec_Alm(VINETIC_CHANNEL *pCh,
                              IFX_boolean_t bEnLec,
                              IFX_boolean_t bEnNlp)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint16_t   ch = (pCh->nChannel - 1);
   IFX_uint16_t   pCmd1[3];
   static const IFX_uint16_t alm_nelec_coefs[5] =
   {
      ECMD_NELEC_COEFS, 0x1048, 0x1808, 0x6060, 0x0060
   };
   static const IFX_uint16_t alm_nelec_nlp[10] =
   {
      ECMD_NELEC_NLP, 0x1502, 0x4540, 0x0C40, 0x4180,
      0x1106, 0x5050, 0x0D10, 0x6480, 0x0810
   };

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
         IFX_uint8_t    lec_res;
         IFX_uint16_t   pCmd2[11];

         /* LEC reource required */
         if (pDev->availLecRes == 0)
         {
            SET_ERROR (ERR_NORESOURCE);
            return IFX_ERROR;
         }

         /* get the LEC resource */
         lec_res = get_free_lec_res(pDev);

         pCmd1 [2] = pDev->pAlmCh[ch].ali_nelec | lec_res | ALM_NLEC_EN;

         /* set channel */
         pCmd2 [0] = CMD1_EOP | lec_res;

         /* set NLEC coefficients defaults for ALM */
         memcpy(&pCmd2[1], alm_nelec_coefs, sizeof(alm_nelec_coefs));

         if (CmdWrite (pDev, pCmd2, 4) != IFX_SUCCESS)
            return IFX_ERROR;

         /* set NLP coefficients defaults for ALM */
         memcpy(&pCmd2[1], alm_nelec_nlp, sizeof(alm_nelec_nlp));
         if (CmdWrite (pDev, pCmd2, 9) != IFX_SUCCESS)
            return IFX_ERROR;

      }
      else
      {
         /* no LEC resource is required, because LEC is running already */
         pCmd1 [2] = pDev->pAlmCh[ch].ali_nelec;
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

   \return
      IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t Dsp_SetNeLec_Pcm(VINETIC_CHANNEL *pCh,
                              IFX_boolean_t bEnLec,
                              IFX_boolean_t bEnNlp)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint16_t   ch = (pCh->nChannel - 1);
   IFX_uint16_t   pCmd1[3];
   static const IFX_uint16_t pcm_nelec_coefs[5] =
   {
      ECMD_NELEC_COEFS, 0x1048, 0x0808, 0x6060, 0x0060
   };
   static const IFX_uint16_t pcm_nelec_nlp[10] =
   {
      ECMD_NELEC_NLP, 0x1502, 0x4540, 0x0C40, 0x4180,
      0x1106, 0x4647, 0x0D10, 0x6480, 0x0810
   };

   if (ch >= pDev->nPcmCnt)
   {
      /* no pcm channel! */
      SET_ERROR (ERR_FUNC_PARM);
      return IFX_ERROR;
   }

   /* set channel */
   pCmd1 [0] = CMD1_EOP | ch;

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
         IFX_uint8_t    lec_res;
         IFX_uint16_t   pCmd2[11];

         /* LEC reource required */
         if (pDev->availLecRes == 0)
         {
            SET_ERROR (ERR_NORESOURCE);
            return IFX_ERROR;
         }

         /* get the LEC resource */
         lec_res = get_free_lec_res(pDev);

         pCmd1 [2] = pDev->pPcmCh[ch].pcm_nelec | lec_res | PCM_NLEC_EN;

         /* set channel */
         pCmd2 [0] = CMD1_EOP | lec_res;

         /* set NLEC coefficients defaults for PCM */
         memcpy(&pCmd2[1], pcm_nelec_coefs, sizeof(pcm_nelec_coefs));

         if (CmdWrite (pDev, pCmd2, 4) != IFX_SUCCESS)
            return IFX_ERROR;

         /* set NLP coefficients defaults for PCM */
         memcpy(&pCmd2[1], pcm_nelec_nlp, sizeof(pcm_nelec_nlp));
         if (CmdWrite (pDev, pCmd2, 9) != IFX_SUCCESS)
            return IFX_ERROR;


      }
      else
      {
         /* no LEC resource is required, because LEC is running already */
         pCmd1 [2] = pDev->pPcmCh[ch].pcm_nelec;
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

#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */

