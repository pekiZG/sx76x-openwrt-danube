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
   Module      : drv_vinetic_lt.c
   Description : This file contains the VINETIC linetesting implementation
*******************************************************************************/


/* ============================= */
/* Includes                      */
/* ============================= */

#include "drv_vinetic_api.h"
#include "drv_vinetic_cram.h"
#include "drv_vinetic_lt.h"

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

/* defines for Ring Offset Registers to be set in RTR register */
#define LT_RO1                                      1
#define LT_RO2                                      2
/* default integration time 100 ms = 10000 mHz = 200 Samples */
#define LT_T_DEFAULT_INTEGRATION                10000 /* mHz */
/* a safety factor is required to ensure that the timeout does not occure
   before the interrupt is risen at the end of the integration time */
#define LT_T_SAFETY_FACTOR                          2
/* set the timeout for DC Levelmeter measurements, based on the default
   integration time and the above safety factor */
#define LT_DC_TIMEOUT  (LT_T_SAFETY_FACTOR * 1000000 / LT_T_DEFAULT_INTEGRATION)

/* set the timeout for the RAMP_READY interrupt to occur */
#define LT_RAMP_TIMEOUT                          2000 /* ms */
/* set the default shift factor for the Offset Calibration (OC) */
#define LT_DEFAULT_SHFIT_OC                    0x01d0
/* set the default integration time for the Offset Calibration (Vinetic 1.4) */
#define LT_T_DEFAULT_INTEGRATION_OC             20000 /* 20 Hz = 100 Samples */
/* set the time to wait on a linemode change, this time varies of cause
   based on the specific line conditions. 500ms should be fine in most
   cases but could be much too long in certain cases... */
#define LT_T_LINEMODE_CHANGE                      500 /* ms */

/* set the start voltage for the ramp used in the capacitance measurement */
#define VINETIC_CAPACITANCE_START_VOLTAGE       50000 /* mV */
/* set the stop voltage for the ramp used in the capacitance measurement */
#define VINETIC_CAPACITANCE_STOP_VOLTAGE       -50000 /* mV */

/* set the current used in the resistance measurement to 20 mA */
#define LT_RES_NORMAL_CURRENT                      20 /* mA */
/* set the current used in the resistance measurement against ground to 15 mA */
#define LT_RES_GROUND_CURRENT                      10 /* mA */

/* set the voltage for Vlim and Vk used in the resistance measurement to 20V */
#define LT_RES_NORMAL_VOLTAGE                      20 /* V */
#define LT_RES_GROUND_VOLTAGE                      40 /* V */

#ifdef VIN_V14_SUPPORT
/* define overflow values and thresholds for DC Optimum Result Routine */
#define RES_OVERFLOW                            32767 /* max. value */
#define NEG_OVERFLOW                           -32768 /* min. value */
#define LT_OPTRES_THRESHOLD                     10000 /* see OptimumResult */
#define LT_OPTRES_THRESHOLD_AC                   6000
/* set the default shift factor to 128 */
#define LT_DEFAULT_SHIFT                       0x0080 /* == 128 */
/* set several parameters for the AC optimum result routine */
#define LT_AC_MIN_ITIME                          0x03 /* 64 ms = 512 samples */
#define LT_AC_START_ITIME_SQUARER                0x15
#define LT_AC_LMAC_DEFAULT_SQUARER                0x6
#define LT_AC_LMAC_DEFAULT_RECTIFIER              0x7
/* set several thresholds to disable automatism during measurements */
#define LT14_MAX_THRESHOLD_HACT  0x8000
#define LT14_MAX_THRESHOLD_HAHY  0x0000
#define LT14_MAX_THRESHOLD_VRTL  0x8000
#define LT14_MAX_THRESHOLD_HRING 0x7FFF
#define LT14_MAX_THRESHOLD_HACRT 0x0000
#define LT14_MAX_THRESHOLD_FRTR  0x7FFF
#define LT14_MAX_THRESHOLD_HPD   0x7FFF
/* set a margin to check the success of the offset calibration to 0.027 mA */
#define LT14_OFFSET_MARGIN 0x00FF
#endif /* VIN_V14_SUPPORT */

#ifdef VIN_V21_SUPPORT
/* set the default integration time for the Offset Calibration (Vinetic 2.1) */
#define LT21_T_DEFAULT_INTEGRATION_OC             100 /* ms */

/* use LM_PREGAIN amplifier for the measurement of small currents */
#define LT21_USE_LM_PREGAIN
#define LT21_DEFAULT_SHIFT                          0 /* = shift factor 1 */

/* set time to wait after setting the RAMP_EN bit [ms],
   at least 2 ms as defined in the Prel. User's Manual,
   System Description */
#define LT21_WAIT_AFTER_RAMP_EN                     3 /* ms */

/* Vinetic 2.1 maximum supported integration time and resulting timeout */
#define LT21_MAX_INTEGRATION_TIME                1000 /* ms */
#define LT21_TIMEOUT_SAFETY_TIME                  250 /* ms */
#define LT21_DC_TIMEOUT  (LT21_MAX_INTEGRATION_TIME + LT21_TIMEOUT_SAFETY_TIME)
#define LT21_RC_RESISTANCE_TIMEOUT              10000 /* ms */

/* defines for the levelmeter operating modes (2kHz and 8kHz) */
#define LT21_LM_FREQ_2KHZ   2000
#define LT21_LM_FREQ_8KHZ   8000

/* set tone generator settling time [ms] */
#define LT21_TG_SETTLING_TIME                      30 /* ms */

/* define margin to check the success of the offset calibration to
   0.027 mA @ 200 samples
   LMval = 0.027 mA * samples * 0.9414 / 94.12mA * 2^23 = 453081 */
#define  LT21_OFFSET_MARGIN 0x0006E9D9

/* define 5Hz Highpass for DC Levelmeter AC measurement, for 2kHz Samples */
                                     /* filt1,  filt2,  filt3,  filt4,  filt5 */
const IFX_uint16_t VIN21_HP5Hz[] = { 0x7F9E, 0x8092, 0x7FFE, 0x8031, 0x7FFC };

#endif /* VIN_V21_SUPPORT */

/* set factor to calculate the filter adaptation time */
#define LT_AC_FILTER_ADAPTION_FACTOR              600
#define LT21_AC_FILTER_ADAPTION_TIME              400 /* ms */

/* defines the filter adaptation time for a 5Hz highpass filter */
#define LT21_5HZ_ADAPTATION_TIME                  200 /* ms */

/* set the timeout for AC measurements in ms */
#define LT_AC_TIMEOUT                             500 /* ms */
#define LT_2KHZ                               2000000 /* mHz */

/* define some constants for 2 to the power of x */
#define CONST_2_14                              16384
#ifdef VIN_V21_SUPPORT
#define CONST_2_15                              32768
#define CONST_2_23                            8388608
#endif /* VIN_V21_SUPPORT */

typedef enum _VINETIC_SLIC_TYPES
{
   VINETIC_SLIC_TYPE_LCP                      = 0x08,
   VINETIC_SLIC_TYPE_LCP_CURRENT_LIMITED      = 0x09
} VINETIC_SLIC_TYPES;

/* declare a structure of voltage slope coefficients for
   the capacitance measurement */
typedef struct _VINETIC_LT_SLOPE_COEFS
{
   IFX_uint32_t                  slope;
   IFX_uint32_t                  delay_ms;
#ifdef VIN_V14_SUPPORT
   IFX_uint32_t                  ringf;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
   IFX_uint32_t                  itime_ms;
#endif /* VIN_V21_SUPPORT */
} VINETIC_LT_SLOPE_COEFS;

/* define a voltage slope coefficient table for
   the capacitance measurement */
VINETIC_LT_SLOPE_COEFS slopeCoefs[2] =
   {
      {
         200000, /* slope mV/s */
         85,     /* lm_delay_ms   */
#ifdef VIN_V14_SUPPORT
         3000,   /* ringf mHz  */
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
         330     /* lm_itime_ms */
#endif /* VIN_V21_SUPPORT */
      },
      {
         2000000, /* slope mV/s */
         5,       /* lm_delay_ms   */
#ifdef VIN_V14_SUPPORT
         25000,   /* ringf mHz  */
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
         40      /* lm_itime_ms */
#endif /* VIN_V21_SUPPORT */
      }
   };

/* define indicies for the above slope coefficient table */
typedef enum _VINETIC_LT_CAPACITANCE_SLOPE
{
  VINETIC_LT_CAPACITANCE_SLOPE_FLAT,
  VINETIC_LT_CAPACITANCE_SLOPE_STEEP
} VINETIC_LT_CAPACITANCE_SLOPE;

/* map the calibration modes which can be called via the ioctl interface to
   the internally available calibration modes. This is required because the
   ioctl interface provides a possibility to calibrate all offsets which is
   internally spread to the following calibration modes. */
typedef enum _VINETIC_OFFSET_CALIBRATION_MODE
{
   VINETIC_OFFSET_CALIBRATION_MODE_DC_SLIC   =
                            VINETIC_IO_OFFSET_CALIBRATION_MODE_DC_SLIC,
   VINETIC_OFFSET_CALIBRATION_MODE_DC_PREFI =
                            VINETIC_IO_OFFSET_CALIBRATION_MODE_DC_PREFI
#ifdef VIN_V21_SUPPORT
                                                                       ,
   VINETIC_OFFSET_CALIBRATION_MODE_DC_SLIC_PREGAIN =
                            VINETIC_IO_OFFSET_CALIBRATION_MODE_DC_SLIC_PREGAIN
#endif /* VIN_V21_SUPPORT */

} VINETIC_OFFSET_CALIBRATION_MODE;

/* map the PCM4 measurements which can be called via the ioctl interface to
   the internal measurement names. This is required because the SNR
   measurement consists of two single measurements. */
typedef enum _VINETIC_LT_AC_MODE
{
   VINETIC_LT_AC_MODE_LEVEL          = VINETIC_IO_LT_AC_MODE_LEVEL,
   VINETIC_LT_AC_MODE_GAINTRACKING   = VINETIC_IO_LT_AC_MODE_GAINTRACKING,
   VINETIC_LT_AC_MODE_TRANSHYBRID    = VINETIC_IO_LT_AC_MODE_TRANSHYBRID,
   VINETIC_LT_AC_MODE_IDLENOISE      = VINETIC_IO_LT_AC_MODE_IDLENOISE,
   VINETIC_LT_AC_MODE_SNR_SIGNAL     = VINETIC_IO_LT_AC_MODE_SNR,
   VINETIC_LT_AC_MODE_SNR_NOISE,
   VINETIC_LT_AC_MODE_DIRECT
} VINETIC_LT_AC_MODE;

/* provide an interface for an universal AC measurement which can handle
   predefined PCM4 measurements as well as AC measurements where all
   parameters can be configured by the application */
typedef struct _VINETIC_LT_AC
{
   VINETIC_LT_AC_MODE                  mode;
   union
   {
      VINETIC_IO_LT_AC_MEASUREMENTS    *pAC;
      VINETIC_IO_LT_AC_DIRECT          *pAC_direct;
   } p;
} VINETIC_LT_AC;

#ifdef TAPI
#if (VIN_CFG_FEATURES & VIN_FEAT_LT)
/* ============================= */
/* Global variable declaration   */
/* ============================= */
VINETIC_LT_CONFIG      ltCfg;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_LT) */
#endif /* TAPI */

/* ============================= */
/* Local function declaration    */
/* ============================= */
#ifdef VIN_V14_SUPPORT
IFX_LOCAL IFX_int32_t LT14_IntegratedMeasurement    (VINETIC_CHANNEL *pCh,
                                                     IFX_int32_t *pResult,
                                                     IFX_int32_t timeout);
IFX_LOCAL IFX_int32_t LT14_DC_OffsetCal             (VINETIC_DEVICE *pDev,
                                                     VINETIC_OFFSET_CALIBRATION_MODE mode,
                                                     VINETIC_IO_OFFSET_CALIBRATION *pCal);
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
IFX_LOCAL IFX_int32_t LT21_LM_IntegrationCfg        (VINETIC_CHANNEL *pCh,
                                                     LMCR2_LM_DCTX8K  dctx8k,
                                                     LMCR2_LM_ITS     its,
                                                     IFX_uint32_t     lm_itime_ms,
                                                     IFX_uint32_t     lm_delay_ms );
IFX_LOCAL IFX_uint16_t LT21_LM_getSamples           (VINETIC_CHANNEL *pCh);
IFX_LOCAL IFX_int32_t LT21_LM_getShift              (VINETIC_CHANNEL *pCh);
IFX_LOCAL IFX_int32_t LT21_IntegratedMeasurement    (VINETIC_CHANNEL      *pCh,
                                                     VINETIC_IO_LT_RESULT *pRes,
                                                     IFX_int32_t          timeout);
IFX_LOCAL IFX_int32_t LT21_DC_OffsetCal             (VINETIC_DEVICE *pDev,
                                                     VINETIC_OFFSET_CALIBRATION_MODE mode,
                                                     VINETIC_IO_OFFSET_CALIBRATION *pCal);
#endif /* VIN_V21_SUPPORT */

#ifdef TAPI
#if (VIN_CFG_FEATURES & VIN_FEAT_LT)
IFX_LOCAL IFX_int32_t LT_AC_Store                   (VINETIC_CHANNEL *pCh);
IFX_LOCAL IFX_int32_t LT_DC_Store                   (VINETIC_CHANNEL *pCh);
IFX_LOCAL IFX_int32_t LT_AC_Restore                 (VINETIC_CHANNEL *pCh);
IFX_LOCAL IFX_int32_t LT_DC_Restore                 (VINETIC_CHANNEL *pCh);
IFX_LOCAL IFX_int32_t LT_AC_GeneralSettings         (VINETIC_CHANNEL *pCh);
IFX_LOCAL IFX_int32_t LT_DC_GeneralSettings         (VINETIC_CHANNEL *pCh);
IFX_LOCAL IFX_int32_t LT_DC_SilentActive            (VINETIC_CHANNEL *pCh);
IFX_LOCAL IFX_int32_t LT_DC_AC_Handling             (VINETIC_CHANNEL *pCh);
IFX_LOCAL IFX_int32_t LT_DC_Cap_InterleavedCurrent  (VINETIC_CHANNEL *pCh,
                                                     VINETIC_IO_LT_CAPACITANCE *pIO,
                                                     VINETIC_LT_CAPACITANCE_SLOPE slopeType);

#ifdef VIN_V14_SUPPORT
IFX_LOCAL IFX_int32_t LT14_DC_OptimumResult         (VINETIC_CHANNEL *pCh,
                                                     VINETIC_IO_LT_RESULT *pRes);
IFX_LOCAL IFX_int32_t LT14_DC_Cap_OptimumResult     (VINETIC_CHANNEL *pCh,
                                                     VINETIC_IO_LT_CAPACITANCE *pIO);
IFX_LOCAL IFX_int32_t LT14_AC_OptimumResult         (VINETIC_CHANNEL *pCh,
                                                     VINETIC_IO_LT_RESULT *pRes);
IFX_LOCAL IFX_int32_t LT14_AC_LM_FilterType         (VINETIC_CHANNEL *pCh,
                                                     VINETIC_IO_LT_AC_LM_FILTER_TYPE lmft);
IFX_LOCAL IFX_int32_t LT14_AC_Level                 (VINETIC_CHANNEL *pCh,
                                                     VINETIC_LT_AC *pPar,
                                                     VINETIC_IO_LT_RESULT *pRes);
IFX_LOCAL IFX_int32_t LT14_DC_SilentThreshold       (VINETIC_CHANNEL *pCh);
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
IFX_LOCAL IFX_int32_t LT21_LM_FilterType            (VINETIC_CHANNEL *pCh,
                                                     VINETIC_IO_LT_AC_LM_FILTER_TYPE lmft);
IFX_LOCAL IFX_int32_t LT21_LM_FilterFreq            (VINETIC_CHANNEL *pCh, IFX_int32_t freq);
IFX_LOCAL IFX_int32_t LT21_AC_Level                 (VINETIC_CHANNEL *pCh,
                                                     VINETIC_LT_AC *pPar,
                                                     VINETIC_IO_LT_RESULT *pRes);
#endif /* VIN_V21_SUPPORT */

IFX_LOCAL IFX_int32_t LT_DC_Current                 (VINETIC_CHANNEL *pCh,
                                                     VINETIC_IO_LT_CURRENT_MODE mode,
                                                     VINETIC_IO_LT_CURRENT_SUBMODE submode,
                                                     VINETIC_IO_LT_RESULT *pRes);
IFX_LOCAL IFX_int32_t LT_DC_Voltage                 (VINETIC_CHANNEL *pCh,
                                                     VINETIC_IO_LT_VOLTAGE_MODE mode,
                                                     VINETIC_IO_LT_VOLTAGE_SUBMODE submode,
                                                     VINETIC_IO_LT_RESULT *pRes);
IFX_LOCAL IFX_int32_t LT_AC_TG_Config               (VINETIC_CHANNEL *pCh,
                                                     VINETIC_IO_LT_TG nTG,
                                                     VINETIC_IO_LT_AC_TG *pCfg);
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_LT) */
#endif /* TAPI */

#ifdef VIN_V14_SUPPORT
/*******************************************************************************
   Description:
   Local function to execute an integrated measurement
Arguments:
   pCh     - pointer to the channel structure
   pResult - 16bit result register value
   timeout - time to wait for LM_OK interrupt
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   To be used only with Vinetic revision < 2.1
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT14_IntegratedMeasurement (VINETIC_CHANNEL *pCh,
                                                  IFX_int32_t *pResult,
                                                  IFX_int32_t timeout)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t             err  = IFX_SUCCESS;

   /* if the Ramp generator is enabled, set the new target voltage */
   if (pCh->ltConf.bRampEnable)
   {
      VIN_CH_RTR_RING_OFFSET_MODIFY(RTR.value,
                                   (pCh->ltConf.bRampReverse)? LT_RO1 : LT_RO2);
      err = wrReg (pCh, RTR_REG);
   }

   IFXOS_DELAYUS (DCDSP_DELAY);    /* be sure all settings are in the DCDSP */

   /* unmask LM_OK interrupt for changes 0->1 */
   if (err == IFX_SUCCESS)
   {
      VIN_CH_MR_SRS1_M_LM_OK_SET(MR_SRS1.value, 0);
      err = wrReg (pCh, MR_SRS1_REG);
   }
   /* start the integration */
   if (err == IFX_SUCCESS)
   {
      VIN_CH_LMCR_LM_EN_SET(LMCR.value, 1);
      err = wrReg (pCh, LMCR_REG);
   }
   /* wait for LM_OK interrupt within a time out */
   if (err == IFX_SUCCESS)
   {
      IFXOS_Wait (1); /* Wait at least 1 ms before reading LMRES */
      if ( IFXOS_WaitEvent_timeout (pCh->wqLT_LM_OK, timeout) == IFX_ERROR )
      {
         TRACE(VINETIC, DBG_LEVEL_HIGH, ("ERROR timeout waiting for LM_OK\n"));
         SET_ERROR (ERR_LT_TIMEOUT_LM_OK);
         err = IFX_ERROR;
      }
   }
   /* read out the result */
   if (err == IFX_SUCCESS)
   {
      err = rdReg(pCh, LMRES_REG);
      *pResult = (IFX_int32_t) LMRES.value;
   }

   /* stop integrated measurement */
   if (err == IFX_SUCCESS)
   {
      VIN_CH_LMCR_LM_EN_SET(LMCR.value, 0);
      err = wrReg (pCh, LMCR_REG);
   }
   /* mask LM_OK */
   if (err == IFX_SUCCESS)
   {
      VIN_CH_MR_SRS1_M_LM_OK_SET(MR_SRS1.value, 1);
      err = wrReg (pCh, MR_SRS1_REG);
   }

   if (pCh->ltConf.bRampEnable)
   {
      pCh->ltConf.bRampReverse = !pCh->ltConf.bRampReverse;
      /* Wait for Ramp to complete */
      if (IFXOS_WaitEvent_timeout (pCh->wqLT_RAMP_RDY, LT_RAMP_TIMEOUT) == IFX_ERROR)
      {
         TRACE(VINETIC, DBG_LEVEL_HIGH, ("ERROR timeout waiting for RAMP_RDY\n"));
         SET_ERROR (ERR_LT_TIMEOUT_LM_RAMP_RDY);
         err = IFX_ERROR;
      }
   }
   return err;
}

/* *****************************************************************************
Description :
   Offset calibration
Arguments   :
   pDev     - pointer to the device structure
   mode     - select offset compensation to be executed
              VINETIC_OFFSET_CALIBRATION_MODE_DC_SLIC,
              VINETIC_OFFSET_CALIBRATION_MODE_DC_PREFI
   pCal     - pointer to the calibration parameter structure
              nChannel = 0..n channel n, 0xff all
              error    = IFX_SUCCESS, IFX_ERROR
Return      :
   IFX_SUCCESS or IFX_ERROR
Remarks     :
   To be used only with Vinetic 1.4.
***************************************************************************** */
IFX_LOCAL IFX_int32_t LT14_DC_OffsetCal (VINETIC_DEVICE *pDev,
                                         VINETIC_OFFSET_CALIBRATION_MODE mode,
                                         VINETIC_IO_OFFSET_CALIBRATION *pCal)
{
   VINETIC_CHANNEL            *pCh;
   VINETIC_IO_OFFSET_VALUE    *pOFR;
   IFX_int32_t                 err[VINETIC_CH_NR] = {IFX_SUCCESS},
                               calibrationError = IFX_SUCCESS,
                               nCh = 0;
   IFX_uint8_t                 nLmSel,
                               startCh,
                               stopCh;
   IFX_int32_t                 res = 0;

   /* initialize pCal->error */
   pCal->error = IFX_SUCCESS;

   /* Select calibration mode */
   switch (mode)
   {
      case VINETIC_OFFSET_CALIBRATION_MODE_DC_PREFI:
         nLmSel = LMCR_LM_SEL_DC_PREFI_OFFSET;
         pOFR   = &pCal->nPrefi;
         break;
      case VINETIC_OFFSET_CALIBRATION_MODE_DC_SLIC:
      default:
         nLmSel = LMCR_LM_SEL_I_DC_IT;
         pOFR   = &pCal->nSlic;
         break;
   }

   /* Select channels to calibrate */
   if (pCal->nChannel < pDev->nAnaChan)
   {
      startCh = pCal->nChannel;
      stopCh  = pCal->nChannel+1;
   }
   else
   {
      startCh = 0;
      stopCh  = pDev->nAnaChan;
   }
   /* ***********************************************************************
    * Measure
    * *********************************************************************** */
   for (nCh = startCh; nCh < stopCh; nCh++)
   {
      pCh = &pDev->pChannel[nCh];
      /* set LevelMeter configuration */
      VIN_CH_LMCR_LM_SEL_MODIFY(LMCR.value, nLmSel);
      VIN_CH_LMCR_DC_AD16_SET  (LMCR.value, 1);
      VIN_CH_LMCR_LM_EN_SET    (LMCR.value, 0);
      VIN_CH_LMCR_LM_RECT_SET  (LMCR.value, 0);
      VIN_CH_LMCR_LM_ONCE_SET  (LMCR.value, 0);
      err[nCh] = wrReg (pCh, LMCR_REG);
      /* reset OFR */
      if (err[nCh] == IFX_SUCCESS)
      {
         OFR.value = 0x0000;
         err[nCh] = wrReg (pCh, OFR_REG);
      }
      /* Measure offset ***************************************************** */
      /* wait for interrupt and read LM-RES */
      if (err[nCh] == IFX_SUCCESS)
         err[nCh] = LT14_IntegratedMeasurement (pCh, &res, LT_DC_TIMEOUT);
#ifdef DEBUG
      TRACE(VINETIC, DBG_LEVEL_LOW, ("ch %ld res=0x%04hlX\n\r", nCh, res));
#endif /* DEBUG */
      /* calculate the two's complement */
      res = (~res) + 1;
      /* Calculate OFR = (res * shift_scale) / (100 Samples * shift * 16) */
      res = (IFX_int16_t) ((res * CONST_2_14) / (100 * LMDC.value * 16));
      /* set OFR */
      if (err[nCh] == IFX_SUCCESS)
      {
         OFR.value = res;
         err[nCh] = wrReg (pCh, OFR_REG);
      }
      pOFR->nOFR[nCh] = res;
      switch (mode)
      {
         case VINETIC_OFFSET_CALIBRATION_MODE_DC_SLIC:
            pCh->offset.nSlic = res;
            break;
         case VINETIC_OFFSET_CALIBRATION_MODE_DC_PREFI:
            pCh->offset.nPrefi = res;
            break;
#ifdef VIN_V21_SUPPORT
         case VINETIC_OFFSET_CALIBRATION_MODE_DC_SLIC_PREGAIN:
            /* not supported by Vinetic Version < 2.1 */
            break;
#endif /* VIN_V21_SUPPORT */
      }
      pCh->offset.bInitialized = IFX_TRUE;
   }

   /* ***********************************************************************
    * Check calibration
    * *********************************************************************** */
   for (nCh = startCh; nCh < stopCh; nCh++)
   {
      pCh = &pDev->pChannel[nCh];
      /* Check offset calibration - measure again *************************** */
      /* wait for interrupt and read LM-RES */
      if (err[nCh] == IFX_SUCCESS)
         err[nCh] = LT14_IntegratedMeasurement (pCh, &res, LT_DC_TIMEOUT);
#ifdef DEBUG
      TRACE(VINETIC, DBG_LEVEL_LOW, ("ch %ld resCal=0x%04hlX\n\r", nCh, res));
#endif /* DEBUG */

      /* Check for errors *************************************************** */
      if (err[nCh] != IFX_SUCCESS)
      {
#ifdef DEBUG
         TRACE(VINETIC, DBG_LEVEL_HIGH,
              ("ERROR DCOffsetCalibration failed for ch %ld\n\r",nCh));
#endif /* DEBUG */
         pCal->error |= (1 << nCh);
         calibrationError = IFX_ERROR;
      }

      /* Check if calibration succeeded, if not set OFR to 0x0000 */
      if (res & 0x8000)
         res = (~res) + 1;     /* two's complement */

      if ((err[nCh] == IFX_SUCCESS) && (res > LT14_OFFSET_MARGIN))
      {
         TRACE(VINETIC, DBG_LEVEL_HIGH, ("ERROR DCOffsetCalibration "
               "offset not calibrated correctly ch=%ld res was 0x%04lX\n\r", nCh, res));
         pOFR->nOFR[nCh] = 0x0000;
         OFR.value = 0x0000;
         err[nCh] = wrReg (pCh, OFR_REG);
         switch (mode)
         {
            case VINETIC_OFFSET_CALIBRATION_MODE_DC_SLIC:
               pCh->offset.nSlic = 0x0000;
               break;
            case VINETIC_OFFSET_CALIBRATION_MODE_DC_PREFI:
               pCh->offset.nPrefi = 0x0000;
               break;
#ifdef VIN_V21_SUPPORT
            case VINETIC_OFFSET_CALIBRATION_MODE_DC_SLIC_PREGAIN:
               /* do nothing */
               break;
#endif /* VIN_V21_SUPPORT */
         }
         pCh->offset.bInitialized = IFX_FALSE;
         err[nCh] = IFX_ERROR;
         pCal->error |= (1 << nCh);
         calibrationError = IFX_ERROR;
      }
   }

   return calibrationError;
}
#endif /* VIN_V14_SUPPORT */

#ifdef VIN_V21_SUPPORT
/*******************************************************************************
   Description:
   Local function to calculate the number of samples
   in one integration period.
Arguments:
   pCh     - pointer to the channel structure
Return:
   the number of samples
Remarks:
   To be used with Vinetic revision >= 2.1
*******************************************************************************/
IFX_LOCAL IFX_uint16_t LT21_LM_getSamples (VINETIC_CHANNEL *pCh)
{
   IFX_uint16_t samples      = 0;
   IFX_uint16_t samplingRate = 0;

   switch (VIN21_CH_LMCR2_LM_ITS_GET(LMCR2.value))
   {
      case LMCR2_LM_ITS_LM_ITIME:
         return LM_ITIME.value;
         break;
      case LMCR2_LM_ITS_RING_PERIOD:
         switch (VIN21_CH_LMCR2_LM_DCTX8K_GET(LMCR2.value))
         {
            case LMCR2_LM_DCTX8K_2K:
               samplingRate = LT21_LM_FREQ_2KHZ;
               break;
            case LMCR2_LM_DCTX8K_8K:
               samplingRate = LT21_LM_FREQ_8KHZ;
               break;
         }
         samples = (samplingRate) / (getRegVal (pCh, RGFR_REG)/1000);
         break;
   }
   return samples;
}

/*******************************************************************************
   Description:
   Local function to calculate the shift factor
Arguments:
   pCh     - pointer to the channel structure
Return:
   the number of samples
Remarks:
   To be used with Vinetic revision >= 2.1
   Shift factors in the range 0..1 are represented by negative shift
   values, i.e. if the shift value is negative you have to use it
   1/abs(shift).
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT21_LM_getShift (VINETIC_CHANNEL *pCh)
{
   IFX_int32_t shiftCoef = LM_SHIFT.value;
   IFX_int32_t shiftVal  = 1;

   if (shiftCoef < 16)
   {
      shiftVal = 1 << shiftCoef;
   }
   else if ((shiftCoef > 16) && (shiftCoef <= 31))
   {
      shiftVal = - (1 << (shiftCoef-16));
   }

   return shiftVal;
}

/*******************************************************************************
   Description:
   Local function to execute an integrated measurement
Arguments:
   pCh     - pointer to the channel structure
   pResult - pointer to the result structure
   timeout - time to wait for LM_OK interrupt
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   To be used with Vinetic revision >= 2.1
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT21_IntegratedMeasurement (VINETIC_CHANNEL      *pCh,
                                                  VINETIC_IO_LT_RESULT *pRes,
                                                  IFX_int32_t           timeout)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t     err  = IFX_SUCCESS;

   /* if the Ramp generator is enabled, set the new target voltage */
   if (pCh->ltConf.bRampEnable)
   {
      VIN_CH_RTR_RING_OFFSET_MODIFY(RTR.value,
                                   (pCh->ltConf.bRampReverse)? LT_RO1 : LT_RO2);
      err = wrReg (pCh, RTR_REG);
   }
   if (err == IFX_SUCCESS)
   {
      /* unmask LM_OK interrupt for changes 0->1 */
      VIN21_CH_MR_SRS1_M_LM_OK_SET(MR_SRS1.value, 0);
      err = wrReg (pCh, MR_SRS1_REG);
   }

   if (err == IFX_SUCCESS)
   {
      /* be sure all settings are in the DCDSP */
      IFXOS_DELAYUS (DCDSP_DELAY_V21);
      /* trigger measurement */
      VIN21_CH_LMCR_LM_INT_MODIFY(LMCR.value, LMCR_LM_INT_ONCE);
      err = wrReg (pCh, LMCR_REG);
   }
   /* wait for LM_OK interrupt within a time out */
   if (err == IFX_SUCCESS)
   {
      IFXOS_DELAYMS (1);  /* Wait at least 1 ms before reading LMRES, LMRES2 */

      if ( IFXOS_WaitEvent_timeout (pCh->wqLT_LM_OK, timeout) == IFX_ERROR )
      {
         TRACE(VINETIC, DBG_LEVEL_HIGH, ("ERROR timeout waiting for LM_OK\n"));
         SET_ERROR (ERR_LT_TIMEOUT_LM_OK);
         err = IFX_ERROR;
      }
   }

   /* concat 32bit LMRES */
   if (err == IFX_SUCCESS)
   {
      err = rdReg(pCh, LMRES_REG);
      pRes->nLMRES   = LMRES.value << 16;
   }
   if (err == IFX_SUCCESS)
   {
      err = rdReg(pCh, LMRES2_REG);
      pRes->nLMRES  |= LMRES2.value;
      pRes->nGain    = 1; /* unused for Vin V21 */
      pRes->nSamples = LT21_LM_getSamples (pCh);
      pRes->nShift   = LT21_LM_getShift (pCh);
      switch (VIN21_CH_LMCR_LM_MAG_GET(LMCR.value))
      {
      case LMCR_LM_MAG_SQUARE:
         pRes->rs = VINETIC_IO_LT_RS_SQUARER;
         break;
      case LMCR_LM_MAG_RECITFY:
         pRes->rs = VINETIC_IO_LT_RS_RECTIFIER;
         break;
      case LMCR_LM_MAG_OFF:
         pRes->rs = VINETIC_IO_LT_RS_NONE;
         break;
      }
   }

   if (err == IFX_SUCCESS)
   {  /* set LM_INT off */
      VIN21_CH_LMCR_LM_INT_MODIFY(LMCR.value, LMCR_LM_INT_OFF);
      err = wrReg (pCh, LMCR_REG);
   }
   if (err == IFX_SUCCESS)
   {  /* mask LM_OK */
      VIN21_CH_MR_SRS1_M_LM_OK_SET(MR_SRS1.value, 1);
      err = wrReg (pCh, MR_SRS1_REG);
   }

   if (pCh->ltConf.bRampEnable)
   {
      pCh->ltConf.bRampReverse = !pCh->ltConf.bRampReverse;
      /* Wait for Ramp to complete */
      if (IFXOS_WaitEvent_timeout (pCh->wqLT_RAMP_RDY, LT_RAMP_TIMEOUT) == IFX_ERROR)
      {
         TRACE(VINETIC, DBG_LEVEL_HIGH, ("ERROR timeout waiting for RAMP_RDY\n"));
         SET_ERROR (ERR_LT_TIMEOUT_LM_RAMP_RDY);
         err = IFX_ERROR;
      }
   }
   return err;
}

/*******************************************************************************
   Description:
   Local function to configure the Vinetic 2.1 integration time and delay
Arguments:
   pCh     - pointer to the channel structure
   dctx8k  - select sample rate for level meter
             LMCR2_LM_DCTX8K_2K,
             LMCR2_LM_DCTX8K_8K
   its     - select integration time source
             LMCR2_LM_ITS_LM_ITIME,
             LMCR2_LM_ITS_RING_PERIOD
   lm_itime_ms - integration time in ms (for its == LMCR2_LM_ITS_LM_ITIME)
   lm_delay_ms - level meter delay in ms
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   To be used with Vinetic revision >= 2.1
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT21_LM_IntegrationCfg ( VINETIC_CHANNEL *pCh,
                                               LMCR2_LM_DCTX8K  dctx8k,
                                               LMCR2_LM_ITS     its,
                                               IFX_uint32_t     lm_itime_ms,
                                               IFX_uint32_t     lm_delay_ms )
{
   IFX_uint32_t samplingRate = 0;
   IFX_int32_t err;

   /* select sampling rate, integration time source */
   VIN21_CH_LMCR2_LM_DCTX8K_SET(LMCR2.value, dctx8k);
   VIN21_CH_LMCR2_LM_ITS_SET   (LMCR2.value, its);
   err = wrReg (pCh, LMCR2_REG);

   /* set lm_itime if not equal to a ring period */
   if ( (err == IFX_SUCCESS) && (its == LMCR2_LM_ITS_LM_ITIME) )
   {
      switch (dctx8k)
      {
      case LMCR2_LM_DCTX8K_2K:
         samplingRate = LT21_LM_FREQ_2KHZ;
         break;
      case LMCR2_LM_DCTX8K_8K:
         samplingRate = LT21_LM_FREQ_8KHZ;
         break;
      }
      LM_ITIME.value = lm_itime_ms * (samplingRate / 1000 /* ms -> s */);
      err = wrReg (pCh, LM_ITIME_REG);
   }
   if (err == IFX_SUCCESS)
   {  /* Note: ring delay is independent of the sampling rate
               and based on the 2kHz clock */
      LM_DELAY.value = lm_delay_ms * (2000 /* Hz */ / 1000 /* ms -> s */);
      err = wrReg (pCh, LM_DELAY_REG);
   }
   return err;
}

/* *****************************************************************************
Description :
   Offset calibration for Vinetic 2.1
Arguments   :
   pDev     - pointer to the device structure
   mode     - select offset compensation to be executed
              VINETIC_OFFSET_CALIBRATION_MODE_DC_SLIC,
              VINETIC_OFFSET_CALIBRATION_MODE_DC_PREFI
   pCal     - pointer to the calibration parameter structure
              nChannel = 0..n channel n, 0xff all
              error    = IFX_SUCCESS, IFX_ERROR
Return      :
   IFX_SUCCESS or IFX_ERROR
Remarks     :
   None
***************************************************************************** */
IFX_LOCAL IFX_int32_t LT21_DC_OffsetCal (VINETIC_DEVICE *pDev,
                                         VINETIC_OFFSET_CALIBRATION_MODE mode,
                                         VINETIC_IO_OFFSET_CALIBRATION *pCal)
{
   VINETIC_CHANNEL            *pCh;
   VINETIC_IO_OFFSET_VALUE    *pOFR;
   IFX_int32_t                 err[VINETIC_CH_NR] = {IFX_SUCCESS},
                               calibrationError = IFX_SUCCESS,
                               nCh = 0;
   IFX_uint8_t                 nLmSel,
                               startCh,
                               stopCh;
   IFX_int16_t                 ofr = 0;
   VINETIC_IO_LT_RESULT        res;

   /* initialize pCal->error */
   pCal->error = IFX_SUCCESS;

   /* Select channels to calibrate */
   if (pCal->nChannel < pDev->nAnaChan)
   {
      startCh = pCal->nChannel;
      stopCh  = pCal->nChannel+1;
   }
   else
   {
      startCh = 0;
      stopCh  = pDev->nAnaChan;
   }

   /* Select calibration mode */
   switch (mode)
   {
      case VINETIC_OFFSET_CALIBRATION_MODE_DC_PREFI:
         nLmSel = LMCR_LM_SEL_DC_PREFI_OFFSET;
         pOFR   = &pCal->nPrefi;
         break;
      case VINETIC_OFFSET_CALIBRATION_MODE_DC_SLIC_PREGAIN:
         nLmSel = LMCR_LM_SEL_I_DC_IT;
         pOFR   = &pCal->nSlicPregain;
         /* set LM_PREGAIN */
         for (nCh = startCh; nCh < stopCh; nCh++)
         {
            pCh = &pDev->pChannel[nCh];
            if (err[nCh] == IFX_SUCCESS)
            {
               VIN21_CH_LMCR_LM_PRE_GAIN_SET (LMCR.value, 1);
               err[nCh] = wrReg(pCh, LMCR_REG);
            }
         }
         break;
      default:
      case VINETIC_OFFSET_CALIBRATION_MODE_DC_SLIC:
         nLmSel = LMCR_LM_SEL_I_DC_IT;
         pOFR   = &pCal->nSlic;
         break;
   }

   /* ***********************************************************************
    * Measure
    * *********************************************************************** */
   for (nCh = startCh; nCh < stopCh; nCh++)
   {
      pCh = &pDev->pChannel[nCh];
      /* set LevelMeter configuration */
      VIN21_CH_LMCR_LM_SEL_MODIFY(LMCR.value, nLmSel);
      err[nCh] = wrReg (pCh, LMCR_REG);
      /* reset OFR */
      if (err[nCh] == IFX_SUCCESS)
      {
         OFR.value = 0x0000;
         err[nCh] = wrReg (pCh, OFR_REG);
      }
      /* Measure offset ***************************************************** */
      /* wait for interrupt and read LM-RES */
      if (err[nCh] == IFX_SUCCESS)
         err[nCh] = LT21_IntegratedMeasurement (pCh, &res, LT_DC_TIMEOUT);

      if (err[nCh] == IFX_SUCCESS)
      {
#ifdef DEBUG
         TRACE(VINETIC, DBG_LEVEL_LOW, ("ch %ld res    = 0x%08lX == %ld samples=%ld\n\r", nCh, res.nLMRES, res.nLMRES, res.nSamples));
#endif /* DEBUG */
         /* calculate the two's complement */
         res.nLMRES = (~res.nLMRES) + 1;
         /* Calculate OFR = (res) / (samples * shift) */
         ofr = (IFX_int16_t) ((res.nLMRES / (res.nSamples * (CONST_2_23/CONST_2_15))));
#ifdef DEBUG
         TRACE(VINETIC, DBG_LEVEL_LOW, ("ch %ld ofr    = 0x%lX\n\r", nCh, ofr));
#endif /* DEBUG */
         /* set OFR */
         OFR.value = ofr;
         err[nCh] = wrReg (pCh, OFR_REG);
         pOFR->nOFR[nCh] = ofr;
         switch (mode)
         {
            case VINETIC_OFFSET_CALIBRATION_MODE_DC_SLIC:
               pCh->offset.nSlic = ofr;
               break;
            case VINETIC_OFFSET_CALIBRATION_MODE_DC_PREFI:
               pCh->offset.nPrefi = ofr;
               break;
            case VINETIC_OFFSET_CALIBRATION_MODE_DC_SLIC_PREGAIN:
               pCh->offset.nSlicPregain = ofr;
               break;
         }
         pCh->offset.bInitialized = IFX_TRUE;
      }
   }

   /* ***********************************************************************
    * Check calibration
    * *********************************************************************** */
   for (nCh = startCh; nCh < stopCh; nCh++)
   {
      pCh = &pDev->pChannel[nCh];
      /* Check offset calibration - measure again *************************** */
      /* wait for interrupt and read LM-RES */
      if (err[nCh] == IFX_SUCCESS)
         err[nCh] = LT21_IntegratedMeasurement (pCh, &res, LT_DC_TIMEOUT);
#ifdef DEBUG
      TRACE(VINETIC, DBG_LEVEL_LOW, ("ch %ld resCal = 0x%08hlX\n\r",
            nCh, res.nLMRES));
#endif /* DEBUG */

      /* Check for errors *************************************************** */
      if (err[nCh] != IFX_SUCCESS)
      {
#ifdef DEBUG
         TRACE(VINETIC, DBG_LEVEL_HIGH,
            ("ERROR DCOffsetCalibration failed for ch %ld\n\r",nCh));
#endif /* DEBUG */
         pCal->error |= (1 << nCh);
         calibrationError = IFX_ERROR;
      }

      if ((err[nCh] == IFX_SUCCESS) &&
         (mode == VINETIC_OFFSET_CALIBRATION_MODE_DC_SLIC_PREGAIN))
      {
         /* switch LM_PREGAIN off */
         VIN21_CH_LMCR_LM_PRE_GAIN_SET (LMCR.value, 0);
         err[nCh] = wrReg (pCh, LMCR_REG);
      }

      /* Check if calibration succeeded, if not set OFR to 0x0000 */
      if (res.nLMRES < 0)
         res.nLMRES *= -1;

      if ((err[nCh] == IFX_SUCCESS) && (res.nLMRES > LT21_OFFSET_MARGIN))
      {
         TRACE(VINETIC, DBG_LEVEL_HIGH, ("ERROR DCOffsetCalibration "
            "offset not calibrated correctly ch=%ld res was 0x%08X\n\r",
            nCh, res.nLMRES));
         pOFR->nOFR[nCh] = 0x0000;
         OFR.value = 0x0000;
         err[nCh] = wrReg (pCh, OFR_REG);
         switch (mode)
         {
            case VINETIC_OFFSET_CALIBRATION_MODE_DC_SLIC:
               pCh->offset.nSlic        = 0x0000;
               break;
            case VINETIC_OFFSET_CALIBRATION_MODE_DC_SLIC_PREGAIN:
               pCh->offset.nSlicPregain = 0x0000;
               break;
            case VINETIC_OFFSET_CALIBRATION_MODE_DC_PREFI:
               pCh->offset.nPrefi       = 0x0000;
               break;
         }
         pCh->offset.bInitialized = IFX_FALSE;
         err[nCh] = IFX_ERROR;
         pCal->error |= (1 << nCh);
         calibrationError = IFX_ERROR;
      }
   }
   return calibrationError;
}
#endif /* VIN_V21_SUPPORT */

/* *****************************************************************************
Description :
   Initial Offset calibration during VINETIC_Init
Arguments   :
   pDev     - pointer to the device structure
   pCal     - pointer to the calibration parameter structure
              nChannel = 0..n channel n, 0xff all
              error    = IFX_SUCCESS, IFX_ERROR
Return      :
   IFX_SUCCESS or IFX_ERROR
Remarks     :
   None
***************************************************************************** */
IFX_int32_t VINETIC_DC_Offset_Calibration (VINETIC_DEVICE *pDev,
                                           VINETIC_IO_OFFSET_CALIBRATION *pCal)
{
   VINETIC_CHANNEL  *pCh;
   IFX_int32_t       err[VINETIC_CH_NR] = {IFX_SUCCESS},
                     calibrationError = IFX_SUCCESS,
                     nCh = 0;
   IFX_uint8_t       startCh,
                     stopCh,
                     nLogCh = 0;
   IFX_char_t        sOfr [9 * VINETIC_CH_NR] = {0};
   IFX_int32_t       (* fpDC_OffsetCal)     (VINETIC_DEVICE *pDev,
                                             VINETIC_OFFSET_CALIBRATION_MODE mode,
                                             VINETIC_IO_OFFSET_CALIBRATION *pCal) = NULL;

   /* Select channels to calibrate */
   if (pCal->nChannel < pDev->nAnaChan)
   {
      startCh = pCal->nChannel;
      stopCh  = pCal->nChannel+1;
   }
   else
   {
      startCh = 0;
      stopCh  = pDev->nAnaChan;
   }

   /* ***********************************************************************
    * Initialisation
    * *********************************************************************** */
   switch (pDev->nChipMajorRev)
   {
#ifdef VIN_V14_SUPPORT
      case VINETIC_V1x:
         for (nCh = startCh; nCh < stopCh; nCh++)
         {
            pCh = &pDev->pChannel[nCh];
            err[nCh] = IFX_SUCCESS;
            /* Store settings *********************************************** */
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = storeRegVal (pCh, OPMOD_CUR_REG);
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = storeRegVal (pCh, LMCR_REG);
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = storeRegVal (pCh, BCR1_REG);
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = storeRegVal (pCh, BCR2_REG);
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = storeRegVal (pCh, RGFR_REG);
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = storeRegVal (pCh, MR_SRS1_REG);
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = storeRegVal (pCh, MF_SRS1_REG);

            /* Configure Measurement **************************************** */
            /* mask hook interrupts */
            if (err[nCh] == IFX_SUCCESS)
            {
               VIN_CH_MR_SRS1_M_HOOK_SET(MR_SRS1.value, 1);
               err[nCh] = wrReg (pCh, MR_SRS1_REG);
            }
            if (err[nCh] == IFX_SUCCESS)
            {
               VIN_CH_MF_SRS1_M_HOOK_SET(MF_SRS1.value, 1);
               err[nCh] = wrReg (pCh, MF_SRS1_REG);
            }
            /* set linemode Active HIRT */
            if (err[nCh] == IFX_SUCCESS)
            {
               VIN_CH_OPMOD_CUR_MODE_CUR_MODIFY   (OPMOD_CUR.value,
                                                   OPMOD_MODE_CUR_ACTIVE);
               VIN_CH_OPMOD_CUR_SUBMODE_CUR_MODIFY(OPMOD_CUR.value,
                                                   OPMOD_SUBMOD_CUR_HIRT);
               err[nCh] = wrReg (pCh, OPMOD_CUR_REG);
            }
            /* be sure that CRAM-EN is set */
            if (err[nCh] == IFX_SUCCESS)
            {
               VIN_CH_BCR2_CRAM_EN_SET(BCR2.value, 1);
               err[nCh] = wrReg (pCh, BCR2_REG);
            }
            /* set integration time */
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = setRegVal (pCh, RGFR_REG, LT_T_DEFAULT_INTEGRATION_OC);
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = wrReg (pCh, RGFR_REG);
            /* set shift factor */
            if (err[nCh] == IFX_SUCCESS)
            {
               LMDC.value = LT_DEFAULT_SHFIT_OC;
               err[nCh] = wrReg (pCh, LMDC_REG);
            }
            /* set calibrationError in case of channel error */
            if (err[nCh] != IFX_SUCCESS)
            {
               calibrationError = IFX_ERROR;
            }
         }
         fpDC_OffsetCal = LT14_DC_OffsetCal;
         break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
      case VINETIC_V2x:
         for (nCh = startCh; nCh < stopCh; nCh++)
         {
         pCh = &pDev->pChannel[nCh];
         err[nCh] = IFX_SUCCESS;
         /* Store settings *********************************************** */
         if (err[nCh] == IFX_SUCCESS)
            err[nCh] = storeRegVal (pCh, OPMOD_CUR_REG);
         if (err[nCh] == IFX_SUCCESS)
            err[nCh] = storeRegVal (pCh, LMCR_REG);
         if (err[nCh] == IFX_SUCCESS)
            err[nCh] = storeRegVal (pCh, LMCR2_REG);
         if (err[nCh] == IFX_SUCCESS)
            err[nCh] = storeRegVal (pCh, BCR1_REG);
         if (err[nCh] == IFX_SUCCESS)
            err[nCh] = storeRegVal (pCh, BCR2_REG);
         if (err[nCh] == IFX_SUCCESS)
            err[nCh] = storeRegVal (pCh, LM_ITIME_REG);
         if (err[nCh] == IFX_SUCCESS)
            err[nCh] = storeRegVal (pCh, LM_DELAY_REG);
         if (err[nCh] == IFX_SUCCESS)
            err[nCh] = storeRegVal (pCh, LM_SHIFT_REG);
         if (err[nCh] == IFX_SUCCESS)
            err[nCh] = storeRegVal (pCh, MR_SRS1_REG);
         if (err[nCh] == IFX_SUCCESS)
            err[nCh] = storeRegVal (pCh, MF_SRS1_REG);

         /* Configure Measurement **************************************** */
         /* mask hook interrupts */
         if (err[nCh] == IFX_SUCCESS)
         {
            VIN_CH_MR_SRS1_M_HOOK_SET(MR_SRS1.value, 1);
            err[nCh] = wrReg (pCh, MR_SRS1_REG);
         }
         if (err[nCh] == IFX_SUCCESS)
         {
            VIN_CH_MF_SRS1_M_HOOK_SET(MF_SRS1.value, 1);
            err[nCh] = wrReg (pCh, MF_SRS1_REG);
            }
            /* set linemode Active HIRT */
            if (err[nCh] == IFX_SUCCESS)
            {
               VIN_CH_OPMOD_CUR_MODE_CUR_MODIFY   (OPMOD_CUR.value,
                                                   OPMOD_MODE_CUR_ACTIVE);
               VIN_CH_OPMOD_CUR_SUBMODE_CUR_MODIFY(OPMOD_CUR.value,
                                                   OPMOD_SUBMOD_CUR_HIRT);
               err[nCh] = wrReg (pCh, OPMOD_CUR_REG);
            }
            /* activate levelmeter, disable ramp generator */
            if (err[nCh] == IFX_SUCCESS)
            {  /* activate levelmeter */
               VIN21_CH_LMCR_LM_ACT_SET(LMCR.value,  1);
               /* no ramp */
               VIN21_CH_LMCR_RAMP_EN_SET(LMCR.value, 0);
               /* no rectifier, no squarer */
               VIN21_CH_LMCR_LM_MAG_MODIFY(LMCR.value, LMCR_LM_MAG_OFF);
               err[nCh] = wrReg (pCh, LMCR_REG);
            }
            /* set integration time and lm filter */
            if (err[nCh] == IFX_SUCCESS)
            {
               /* bypass level meter filter */
               VIN21_CH_LMCR2_LM_FILT_SEL_MODIFY (LMCR2.value,
                                                  LMCR2_LM_FILT_SEL_BYPASS);
               err[nCh] = LT21_LM_IntegrationCfg (pCh, LMCR2_LM_DCTX8K_2K,
                                                  LMCR2_LM_ITS_LM_ITIME,
                                                  LT21_T_DEFAULT_INTEGRATION_OC,
                                                  0 /* ms */);
            }
            /* set shift factor = 1 */
            if (err[nCh] == IFX_SUCCESS)
            {
               LM_SHIFT.value = 0;
               err[nCh] = wrReg (pCh, LM_SHIFT_REG);
            }
            /* set calibrationError in case of channel error */
            if (err[nCh] != IFX_SUCCESS)
            {
               calibrationError = IFX_ERROR;
            }
         }
         fpDC_OffsetCal = LT21_DC_OffsetCal;
         break;
#endif /* VIN_V21_SUPPORT */
      default:
         SET_ERROR(ERR_UNKNOWN_VERSION);
         err[nCh] = IFX_ERROR;
         break;
   }
   IFXOS_Wait (100); /* allow line to settle (! > 75 ms) */

   /* do the offset calibration */
   switch (pCal->nMode)
   {
      case VINETIC_IO_OFFSET_CALIBRATION_MODE_DC_PREFI:
         if ((calibrationError == IFX_SUCCESS) && (fpDC_OffsetCal != NULL))
         {
            calibrationError = fpDC_OffsetCal (pDev,
                                     VINETIC_OFFSET_CALIBRATION_MODE_DC_PREFI,
                                               pCal);
         }
         break;
      case VINETIC_IO_OFFSET_CALIBRATION_MODE_DC_SLIC:
         if ((calibrationError == IFX_SUCCESS) && (fpDC_OffsetCal != NULL))
         {
            calibrationError = fpDC_OffsetCal (pDev,
                                     VINETIC_OFFSET_CALIBRATION_MODE_DC_SLIC,
                                               pCal);
         }
         break;
#ifdef VIN_V21_SUPPORT
      case VINETIC_IO_OFFSET_CALIBRATION_MODE_DC_SLIC_PREGAIN:
         if ((calibrationError == IFX_SUCCESS) && (fpDC_OffsetCal != NULL))
         {
            calibrationError = fpDC_OffsetCal (pDev,
                              VINETIC_OFFSET_CALIBRATION_MODE_DC_SLIC_PREGAIN,
                                               pCal);
         }
         break;
#endif /* VIN_V21_SUPPORT */
      case VINETIC_IO_OFFSET_CALIBRATION_MODE_ALL:
      default:
         if (fpDC_OffsetCal != NULL)
         {
            if (calibrationError == IFX_SUCCESS)
            {
               calibrationError = fpDC_OffsetCal (pDev,
                                     VINETIC_OFFSET_CALIBRATION_MODE_DC_PREFI,
                                                  pCal);
            }
#ifdef VIN_V21_SUPPORT
            switch (pDev->nChipMajorRev)
            {
               case VINETIC_V2x:
                  if (calibrationError == IFX_SUCCESS)
                  {
                     calibrationError = fpDC_OffsetCal (pDev,
                                VINETIC_OFFSET_CALIBRATION_MODE_DC_SLIC_PREGAIN,
                                                        pCal);
                  }
                  break;
               default:
                  /* nothing more to do */
                  break;
            }
#endif /* VIN_V21_SUPPORT */
            /* be sure the DC_SLIC calibration is done at the end so that
               the OFR register contains the correct compensation for the
               standard active mode... */
            if (calibrationError == IFX_SUCCESS)
            {
               calibrationError = fpDC_OffsetCal (pDev,
                                     VINETIC_OFFSET_CALIBRATION_MODE_DC_SLIC,
                                                  pCal);
            }
         }
         break;
   }

   /* set error code to the different channels (from pCal->error) */
   if (calibrationError != IFX_SUCCESS)
   {
      for (nCh = startCh; nCh < stopCh; nCh++)
      {
         /* if channel error bit is set, set channel error in local array */
         if (pCal->error & (1 << nCh))
         {
            err[nCh] = IFX_ERROR;
         }
      }
   }

   for (nCh = startCh; nCh < stopCh; nCh++)
   {
      pCh = &pDev->pChannel[nCh];
      /* Restore Settings *************************************************** */
      switch (pDev->nChipMajorRev)
      {
#ifdef VIN_V14_SUPPORT
         case VINETIC_V1x:
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = restoreRegVal(pCh, LMCR_REG);
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = restoreRegVal(pCh, BCR1_REG);
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = restoreRegVal(pCh, BCR2_REG);
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = restoreRegVal(pCh, RGFR_REG);
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = restoreRegVal(pCh, OPMOD_CUR_REG);
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = restoreRegVal(pCh, MR_SRS1_REG);
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = restoreRegVal(pCh, MF_SRS1_REG);
            break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
         case VINETIC_V2x:
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = restoreRegVal (pCh, LMCR_REG);
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = restoreRegVal (pCh, LMCR2_REG);
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = restoreRegVal (pCh, BCR1_REG);
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = restoreRegVal (pCh, BCR2_REG);
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = restoreRegVal (pCh, LM_ITIME_REG);
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = restoreRegVal (pCh, LM_DELAY_REG);
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = restoreRegVal (pCh, LM_SHIFT_REG);
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = restoreRegVal (pCh, OPMOD_CUR_REG);
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = restoreRegVal(pCh, MR_SRS1_REG);
            if (err[nCh] == IFX_SUCCESS)
               err[nCh] = restoreRegVal(pCh, MF_SRS1_REG);
            break;
#endif /* VIN_V21_SUPPORT */
         default:
            SET_ERROR(ERR_UNKNOWN_VERSION);
            err[nCh] = IFX_ERROR;
            break;
      }
      sprintf(&sOfr[9 * nLogCh++], "%ld:0x%04X ", nCh, OFR.value);
      if (err[nCh] != IFX_SUCCESS)
         calibrationError = IFX_ERROR;
   }

   TRACE(VINETIC, DBG_LEVEL_NORMAL, ("OFR %s\n\r",sOfr));

   pCal->nHwRevision = pDev->nChipRev;
   return calibrationError;
}

#ifdef TAPI
#if (VIN_CFG_FEATURES & VIN_FEAT_LT)
/*******************************************************************************
Description:
   Local function to store register values before the measurement starts
Arguments:
   pCh     - pointer to the channel structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   None
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT_AC_Store (VINETIC_CHANNEL *pCh)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t     err;

   err = storeRegVal (pCh, OPMOD_CUR_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, AUTOMOD_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, MR_SRS1_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, BCR1_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, BCR2_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, TG1F_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, TG1A_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, TG2F_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, TG2A_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, LMCR_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, DSCR_REG);

   switch (pDev->nChipMajorRev)
   {
#ifdef VIN_V14_SUPPORT
      case VINETIC_V1x:
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, LMAC_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, BP1F_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, BP2F_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, BP3F_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, BP12Q_REG);
         break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
      case VINETIC_V2x:
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, LMCR2_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, LM_ITIME_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, LM_DELAY_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, LM_SHIFT_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, LM_FILT1_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, LM_FILT2_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, LM_FILT3_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, LM_FILT4_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, LM_FILT5_REG);
         break;
#endif /* VIN_V21_SUPPORT */
      default:
         SET_ERROR(ERR_UNKNOWN_VERSION);
         err = IFX_ERROR;
         break;
   }

#ifdef RUNTIME_TRACE
   TRACE(VINETIC_RUNTIME_TRACE, DBG_LEVEL_NORMAL,
        ("-------------------------------\n\r"));
#endif /* RUNTIME_TRACE */
   return err;
}

/*******************************************************************************
Description:
   Local function to store register values before the measurement starts
Arguments:
   pCh     - pointer to the channel structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   None
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT_DC_Store (VINETIC_CHANNEL *pCh)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t     err;

   err = storeRegVal (pCh, OPMOD_CUR_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, MR_SRS1_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, MR_SRS2_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, AUTOMOD_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, RO1_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, RO2_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, RTR_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, LMCR_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, BCR1_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, BCR2_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, RGFR_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, IK1_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, VLIM_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, VK1_REG);
   if (err == IFX_SUCCESS)
      err = storeRegVal (pCh, OFR_REG);

   switch (pDev->nChipMajorRev)
   {
#ifdef VIN_V14_SUPPORT
   case VINETIC_V1x:
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, HACT_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, HAHY_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, VRTL_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, HRING_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, HACRT_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, FRTR_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, HPD_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, TSTR3_REG);
         break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
      case VINETIC_V2x:
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, LMCR2_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, LM_ITIME_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, LM_DELAY_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, LM_SHIFT_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, LM_FILT1_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, LM_FILT2_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, LM_FILT3_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, LM_FILT4_REG);
         if (err == IFX_SUCCESS)
            err = storeRegVal (pCh, LM_FILT5_REG);
         break;
#endif /* VIN_V21_SUPPORT */
      default:
         SET_ERROR(ERR_UNKNOWN_VERSION);
         err = IFX_ERROR;
         break;
   }

#ifdef RUNTIME_TRACE
   TRACE(VINETIC_RUNTIME_TRACE, DBG_LEVEL_NORMAL,
        ("-------------------------------\n\r"));
#endif /* RUNTIME_TRACE */
   return err;
}

/*******************************************************************************
Description:
   Local function to restore register values after the measurement completed
Arguments:
   pCh     - pointer to the channel structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   None
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT_AC_Restore (VINETIC_CHANNEL *pCh)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t     err;

#ifdef RUNTIME_TRACE
   TRACE(VINETIC_RUNTIME_TRACE, DBG_LEVEL_NORMAL,
        ("-------------------------------\n\r"));
#endif /* RUNTIME_TRACE */

      err = restoreRegVal (pCh, OPMOD_CUR_REG);
   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, MR_SRS1_REG);
   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, BCR1_REG);
   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, BCR2_REG);
   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, TG1F_REG);
   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, TG1A_REG);
   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, TG2F_REG);
   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, TG2A_REG);
   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, LMCR_REG);
   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, DSCR_REG);
   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, AUTOMOD_REG);

   switch (pDev->nChipMajorRev)
   {
#ifdef VIN_V14_SUPPORT
      case VINETIC_V1x:
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, LMAC_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, BP1F_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, BP2F_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, BP3F_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, BP12Q_REG);
      break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
      case VINETIC_V2x:
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, LMCR2_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, LM_ITIME_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, LM_DELAY_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, LM_SHIFT_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, LM_FILT1_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, LM_FILT2_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, LM_FILT3_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, LM_FILT4_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, LM_FILT5_REG);
         break;
#endif /* VIN_V21_SUPPORT */
      default:
         SET_ERROR(ERR_UNKNOWN_VERSION);
         err = IFX_ERROR;
         break;
   }

   return err;
}

/*******************************************************************************
Description:
   Local function to restore register values after the DC measurement completed
Arguments:
   pCh     - pointer to the channel structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   None
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT_DC_Restore (VINETIC_CHANNEL *pCh)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t     err;

#ifdef RUNTIME_TRACE
   TRACE(VINETIC_RUNTIME_TRACE, DBG_LEVEL_NORMAL,
        ("-------------------------------\n\r"));
#endif /* RUNTIME_TRACE */

   err = restoreRegVal (pCh, LMCR_REG);
   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, IK1_REG);
   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, VLIM_REG);
   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, VK1_REG);
   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, BCR2_REG);
   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, OFR_REG);
   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, RTR_REG);
   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, RO1_REG);
   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, RO2_REG);
   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, MR_SRS1_REG);
   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, MR_SRS2_REG);
   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, RGFR_REG);

   switch (pDev->nChipMajorRev)
   {
#ifdef VIN_V14_SUPPORT
      case VINETIC_V1x:
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, VRTL_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, HRING_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, HACRT_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, FRTR_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, HPD_REG);

         /* IMPORTANT:
            Give the DC-TX decimation filters time to adapt before
            DC-Hold is switched off! About 3 2kHz frames should be
            sufficient.
         */
         IFXOS_Wait (50); /* ms */

         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, TSTR3_REG);
         break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
      case VINETIC_V2x:
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, LMCR2_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, LM_ITIME_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, LM_DELAY_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, LM_SHIFT_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, LM_FILT1_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, LM_FILT2_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, LM_FILT3_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, LM_FILT4_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, LM_FILT5_REG);
         break;
#endif /* VIN_V21_SUPPORT */
      default:
         SET_ERROR(ERR_UNKNOWN_VERSION);
         err = IFX_ERROR;
         break;
   }

   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, BCR1_REG);

   switch (pDev->nChipMajorRev)
   {
#ifdef VIN_V14_SUPPORT
      case VINETIC_V1x:
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, HACT_REG);
         if (err == IFX_SUCCESS)
            err = restoreRegVal (pCh, HAHY_REG);
         break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
      case VINETIC_V2x:
         /* nothing more to do */
         break;
#endif /* VIN_V21_SUPPORT */
      default:
         SET_ERROR(ERR_UNKNOWN_VERSION);
         err = IFX_ERROR;
         break;
   }

   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, OPMOD_CUR_REG);
   if (err == IFX_SUCCESS)
      err = restoreRegVal (pCh, AUTOMOD_REG);

   return err;
}

/*******************************************************************************
Description:
   Do general settings for AC measurements
Arguments:
   pCh     - pointer to the channel structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   This function is usually called before the measurement.
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT_AC_GeneralSettings (VINETIC_CHANNEL *pCh)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t     err;

   /* the TTX_EN bit is at the same position for Vinetic 1.4 and 2.1 */
   VIN_CH_BCR2_TTX_EN_SET(BCR2.value, 0);
   err = wrReg (pCh, BCR2_REG);

   switch (pDev->nChipMajorRev)
   {
#ifdef VIN_V14_SUPPORT
      case VINETIC_V1x:
         if (err == IFX_SUCCESS)
         {
            VIN_CH_AUTOMOD_PDOT_DIS_SET(AUTOMOD.value, 1);
            err = wrReg (pCh, AUTOMOD_REG);
         }
         break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
      case VINETIC_V2x:
         if (err == IFX_SUCCESS)
         {
            VIN21_CH_AUTOMOD_PDOT_DIS_SET(AUTOMOD.value, 1);
            err = wrReg (pCh, AUTOMOD_REG);
         }
         if (err == IFX_SUCCESS)
         {  /* activate levelmeter */
            VIN21_CH_LMCR_LM_ACT_SET(LMCR.value, 1);
            /* no ramp */
            VIN21_CH_LMCR_RAMP_EN_SET(LMCR.value, 0);
            err = wrReg(pCh, LMCR_REG);
         }
         if (err == IFX_SUCCESS)
         {  /* set shift factor = 1 */
            LM_SHIFT.value = LT21_DEFAULT_SHIFT;
            err = wrReg (pCh, LM_SHIFT_REG);
         }
         break;
#endif /* VIN_V21_SUPPORT */
      default:
         SET_ERROR(ERR_UNKNOWN_VERSION);
         err = IFX_ERROR;
         break;
   }

   return err;
}

/*******************************************************************************
Description:
   Do general settings for DC measurements
Arguments:
   pCh     - pointer to the channel structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   This function is usually called before the measurement.
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT_DC_GeneralSettings (VINETIC_CHANNEL *pCh)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t     err;

   switch (pDev->nChipMajorRev)
   {
#ifdef VIN_V14_SUPPORT
      case VINETIC_V1x:
         VIN_CH_MR_SRS2_M_OTEMP_SET(MR_SRS2.value, 1);
         err = wrReg (pCh, MR_SRS2_REG);
         if (err == IFX_SUCCESS)
         {
            VIN_CH_AUTOMOD_PDOT_DIS_SET(AUTOMOD.value, 1);
            err = wrReg (pCh, AUTOMOD_REG);
         }
         if (err == IFX_SUCCESS)
         {
            VIN_CH_LMCR_LM_ONCE_SET(LMCR.value, 1);
            VIN_CH_LMCR_LM_RECT_SET(LMCR.value, 0);
            VIN_CH_LMCR_RAMP_EN_SET(LMCR.value, 0);
            err = wrReg (pCh, LMCR_REG);
         }
         break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
      case VINETIC_V2x:
         VIN_CH_MR_SRS2_M_OTEMP_SET(MR_SRS2.value, 1);
         err = wrReg (pCh, MR_SRS2_REG);
         if (err == IFX_SUCCESS)
         {
            VIN21_CH_AUTOMOD_PDOT_DIS_SET(AUTOMOD.value, 1);
            err = wrReg (pCh, AUTOMOD_REG);
         }
         /* activate levelmeter, disable ramp generator, disable squarer/rect */
         if (err == IFX_SUCCESS)
         {
            /* activate levelmeter */
            VIN21_CH_LMCR_LM_ACT_SET(LMCR.value,  1);
            /* no ramp */
            VIN21_CH_LMCR_RAMP_EN_SET(LMCR.value, 0);
            /* no rectifier, no squarer */
            VIN21_CH_LMCR_LM_MAG_MODIFY(LMCR.value, LMCR_LM_MAG_OFF);
            /* be sure integration is switched off */
            VIN21_CH_LMCR_LM_INT_MODIFY(LMCR.value, LMCR_LM_INT_OFF);
            err = wrReg (pCh, LMCR_REG);
         }
         if (err == IFX_SUCCESS)
         {
            /* disable level meter filter */
            VIN21_CH_LMCR2_LM_FILT_SEL_MODIFY(LMCR2.value,
               LMCR2_LM_FILT_SEL_BYPASS);
            err = wrReg (pCh, LMCR2_REG);
         }
         /* set shift factor = 1 */
         if (err == IFX_SUCCESS)
         {
            LM_SHIFT.value = 0;
            err = wrReg (pCh, LM_SHIFT_REG);
         }
         break;
#endif /* VIN_V21_SUPPORT */
      default:
         SET_ERROR(ERR_UNKNOWN_VERSION);
         err = IFX_ERROR;
         break;
   }

   return err;
}

/*******************************************************************************
Description:
   Local function to set the BCR1.DC_Hold bit if the linemode is either Active
   or Active Boosted.
Arguments:
   pCh     - pointer to the channel structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   None
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT_DC_SilentActive (VINETIC_CHANNEL *pCh)
{
   IFX_int32_t err = IFX_SUCCESS;

   /* "ActiveSilent" - set DC-Hold if in active mode */
   if ((VIN_CH_OPMOD_CUR_MODE_CUR_GET(OPMOD_CUR.value) == OPMOD_MODE_CUR_ACTIVE) ||
       (VIN_CH_OPMOD_CUR_MODE_CUR_GET(OPMOD_CUR.value) == OPMOD_MODE_CUR_ACTIVE_BOOSTED))
   {
      /* Note: Although the bitfield in the BCR1 register has changed
               between different chip revisions, the position of the
               DC-HOLD bit is still the same. */
      VIN_CH_BCR1_DC_HOLD_SET(BCR1.value, 1);
      err = wrReg(pCh, BCR1_REG);
      IFXOS_DELAYUS (DCDSP_DELAY);
   }
   return err;
}

/*******************************************************************************
Description:
   Local function to write the DC part of the result to the OFR register
Arguments:
   pCh     - pointer to the channel structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   None
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT_DC_AC_Handling (VINETIC_CHANNEL *pCh)
{
   VINETIC_DEVICE         *pDev = pCh->pParent;
   IFX_int32_t             err;
#ifdef VIN_V14_SUPPORT
   VINETIC_IO_LT_RESULT    res;
#endif /* VIN_V14_SUPPORT */

   switch (pDev->nChipMajorRev)
   {
#ifdef VIN_V14_SUPPORT
      case VINETIC_V1x:
         OFR.value  = 0;
         err = wrReg(pCh, OFR_REG);
         if (err == IFX_SUCCESS)
         {
            err = LT14_DC_OptimumResult (pCh, &res);
            /* calculate DC offset, see System Reference Manual for details */
            OFR.value  = (IFX_int16_t) ((- CONST_2_14 * res.nLMRES) /
                                       (res.nGain * res.nShift * res.nSamples));
         }
         if (err == IFX_SUCCESS)
            err = wrReg (pCh, OFR_REG);
         if (err == IFX_SUCCESS)
         {
            VIN_CH_LMCR_LM_RECT_SET (LMCR.value, 1);
            err =  wrReg (pCh, LMCR_REG);
         }
         break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
      case VINETIC_V2x:
         /* set filter frequency */
         LM_FILT1.value = VIN21_HP5Hz[0];
         LM_FILT2.value = VIN21_HP5Hz[1];
         LM_FILT3.value = VIN21_HP5Hz[2];
         LM_FILT4.value = VIN21_HP5Hz[3];
         LM_FILT5.value = VIN21_HP5Hz[4];
         err = wrReg (pCh, LM_FILT1_REG);
         if (err == IFX_SUCCESS)
            err = wrReg (pCh, LM_FILT2_REG);
         if (err == IFX_SUCCESS)
            err = wrReg (pCh, LM_FILT3_REG);
         if (err == IFX_SUCCESS)
            err = wrReg (pCh, LM_FILT4_REG);
         if (err == IFX_SUCCESS)
            err = wrReg (pCh, LM_FILT5_REG);
         /* Select filter structure */
         if (err == IFX_SUCCESS)
         {
            VIN21_CH_LMCR2_LM_FILT_SEL_MODIFY(LMCR2.value,
                                              LMCR2_LM_FILT_SEL_HIGHPASS);
            err = wrReg(pCh, LMCR2_REG);
         }
         if (err == IFX_SUCCESS)
         {
            VIN21_CH_LMCR_LM_MAG_MODIFY(LMCR.value, LMCR_LM_MAG_RECITFY);
            err =  wrReg (pCh, LMCR_REG);
            IFXOS_Wait ( LT21_5HZ_ADAPTATION_TIME );
         }
         break;
#endif /* VIN_V21_SUPPORT */
      default:
         SET_ERROR(ERR_UNKNOWN_VERSION);
         err = IFX_ERROR;
         break;
   }
   return err;
}

/*******************************************************************************
Description:
   Local function to measure the current on two consecutive ramps.
   The slope of the voltage ramp is a parameter to this function;
   it is assigned to a set of predefined coefficients.
Arguments:
   pCh       - pointer to the channel structure
   pIO       - pointer to the capacitance parameter/result structure
   slopeType - slope of the voltage ramp (flat or steep)
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   This function iterates two sets of LM-RESults as well as their according
   (independent!) shift and gain values using the Optimum Result routine.
   To achieve this, a series of raising and falling ramps is used to measures
   the two coefficient sets in an interleaved way.
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT_DC_Cap_InterleavedCurrent (VINETIC_CHANNEL *pCh,
                                                    VINETIC_IO_LT_CAPACITANCE *pIO,
                                                    VINETIC_LT_CAPACITANCE_SLOPE slopeType)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t     err  = IFX_SUCCESS;

   /* Set Ramp Slope */
   err = setRegVal (pCh, CRAMP_REG, slopeCoefs[slopeType].slope);
   if (err == IFX_SUCCESS)
   {
      /* SLIC specific adations */
      if (err == IFX_SUCCESS)
      {
         switch (VIN_CH_BCR1_SEL_SLIC_GET(BCR1.value))
         {
             /* slic LCP/LCP with current limitation; adapted to gain */
             case VINETIC_SLIC_TYPE_LCP:
             case VINETIC_SLIC_TYPE_LCP_CURRENT_LIMITED:
                CRAMP.value = CRAMP.value << 1;
                if (err == IFX_SUCCESS)
                  err = wrReg (pCh, CRAMP_REG);
                CRAMP.value = CRAMP.value >> 1;
                break;
             default:
                /* just write */
                if (err == IFX_SUCCESS)
                   err = wrReg (pCh, CRAMP_REG);
                break;
         }
      }
   }

   /* Set Offset to precalibrated value */
   if ((err == IFX_SUCCESS) && (pCh->offset.bInitialized))
   {
      OFR.value = pCh->offset.nSlic;
      err = wrReg (pCh, OFR_REG);
   }
   else
      TRACE(VINETIC, DBG_LEVEL_HIGH, ("Warning: Offsets not calibrated\n"));

   pCh->ltConf.bRampEnable  = IFX_TRUE;
   switch (pDev->nChipMajorRev)
   {
#ifdef VIN_V14_SUPPORT
      case VINETIC_V1x:
         if (err == IFX_SUCCESS)
            err = setRegVal (pCh, RGFR_REG, slopeCoefs[slopeType].ringf);
         if (err == IFX_SUCCESS)
            err = wrReg (pCh, RGFR_REG);
         if (err == IFX_SUCCESS)
            err = setRegVal (pCh, RGD_REG, slopeCoefs[slopeType].delay_ms);
         if (err == IFX_SUCCESS)
            err = wrReg (pCh, RGD_REG);
         if (err == IFX_SUCCESS)
            err = LT14_DC_Cap_OptimumResult (pCh, pIO);
         pIO->slope = getRegVal (pCh, CRAMP_REG);
         break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
      case VINETIC_V2x:
         if (err == IFX_SUCCESS)
         {
            VIN21_CH_LMCR_RAMP_EN_SET(LMCR.value, 1);
            err = wrReg (pCh, LMCR_REG);
         }
         IFXOS_Wait(LT21_WAIT_AFTER_RAMP_EN);

         if (err == IFX_SUCCESS)
            err = LT21_LM_IntegrationCfg (pCh, LMCR2_LM_DCTX8K_2K,
                                          LMCR2_LM_ITS_LM_ITIME,
                                          slopeCoefs[slopeType].itime_ms,
                                          slopeCoefs[slopeType].delay_ms);
         if (err == IFX_SUCCESS)
            err = LT21_IntegratedMeasurement(pCh, &pIO->res[0], LT_RAMP_TIMEOUT);
         if (err == IFX_SUCCESS)
            err = LT21_IntegratedMeasurement(pCh, &pIO->res[1], LT_RAMP_TIMEOUT);
         pIO->slope = slopeCoefs[slopeType].slope;
         break;
#endif /* VIN_V21_SUPPORT */
      default:
         SET_ERROR(ERR_UNKNOWN_VERSION);
         err = IFX_ERROR;
         break;
   }
   pCh->ltConf.bRampEnable  = IFX_FALSE;
   pCh->ltConf.bRampReverse = IFX_FALSE;

   return err;
}

#ifdef VIN_V14_SUPPORT
/*******************************************************************************
Description:
   Local function containing the "Optimum Result Routine"
Arguments:
   pCh     - pointer to the channel structure
   pResult - pointer to the result structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   To be used only with Vinetic revision < 2.1
Note:
   Shift and gain factors are stored in the shadow registers and have to be
   stored in the result structure by the calling function, since this function
   has several chances to return...
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT14_DC_OptimumResult (VINETIC_CHANNEL *pCh,
                                             VINETIC_IO_LT_RESULT *pRes)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t     res  = 0;
   IFX_int32_t     err;

   /* Stage 1 *************************************************************** */
   /*         first shoot                                                     */
   LMDC.value = LT_DEFAULT_SHIFT;
   err = wrReg(pCh, LMDC_REG);

   if (err == IFX_SUCCESS)
   {
      VIN_CH_LMCR_DC_AD16_SET(LMCR.value, 0);
      err = wrReg (pCh, LMCR_REG);
   }
   if (err == IFX_SUCCESS)
      err = LT14_IntegratedMeasurement(pCh, &res, LT_DC_TIMEOUT);
   if ((err == IFX_SUCCESS) &&
       (abs(res) > (RES_OVERFLOW-LT_OPTRES_THRESHOLD)) &&
       (res != RES_OVERFLOW) && (res != NEG_OVERFLOW))
   {
      goto LT_DC_OPTRES_EXIT;
   }

   /* Stage 2 *************************************************************** */
   /*         test for gain 16                                                */
   /*         Note: don't use gain16 if in ring burst mode because the        */
   /*               internal result register might clip.                      */
   if ((err == IFX_SUCCESS) &&
       (VIN_CH_OPMOD_CUR_MODE_CUR_GET(OPMOD_CUR.value) != OPMOD_MODE_CUR_RINGING) &&
       ((abs(res) < (RES_OVERFLOW / 32))))/* 32 == gain16 * 2 (safety margin) */
   {
      VIN_CH_LMCR_DC_AD16_SET(LMCR.value, 1);
      err = wrReg (pCh, LMCR_REG);
      if (err == IFX_SUCCESS)
         err = LT14_IntegratedMeasurement(pCh, &res, LT_DC_TIMEOUT);

      if ((err == IFX_SUCCESS) &&
          (abs(res) > (RES_OVERFLOW-LT_OPTRES_THRESHOLD)) &&
          (res != RES_OVERFLOW) && (res != NEG_OVERFLOW))
      {
         goto LT_DC_OPTRES_EXIT;
      }
   }

   /* Stage 3 *************************************************************** */
   /*         calc optimum shift                                              */
   if (err == IFX_SUCCESS)
   {
      LMDC.value =  (IFX_int16_t) ( LMDC.value *
                            ((RES_OVERFLOW-LT_OPTRES_THRESHOLD)) / abs(res) );
      err = wrReg (pCh, LMDC_REG);
   }
   if (err == IFX_SUCCESS)
      err = LT14_IntegratedMeasurement(pCh, &res, LT_DC_TIMEOUT);

   /* Stage 4 *************************************************************** */
   /*         overflow check                                                  */
   while ( (res == RES_OVERFLOW) || (res ==  NEG_OVERFLOW) )
   {
      if (LMDC.value == 0)
      {
         if (VIN_CH_LMCR_DC_AD16_GET(LMCR.value))
         {
            VIN_CH_LMCR_DC_AD16_SET(LMCR.value, 0);
            err = wrReg (pCh, LMCR_REG);
         }
         else
         {
            TRACE(VINETIC, DBG_LEVEL_HIGH,
                 ("IFX_ERROR LT14_DC_OptimumResult GIVING UP....\n\r"));
            SET_ERROR(ERR_LT_OPTRES_FAILED);
            return IFX_ERROR;
         }
      }
      else
      {
         /* reduce the shift factor (* 0.75) */
         LMDC.value = (LMDC.value / 4) * 3;
         err = wrReg (pCh, LMDC_REG);
      }
      err = LT14_IntegratedMeasurement(pCh, &res, LT_DC_TIMEOUT);
   }

LT_DC_OPTRES_EXIT:
   if (err == IFX_SUCCESS)
   {
      pRes->nLMRES   = res;
      pRes->nSamples = (IFX_uint16_t)(LT_2KHZ / getRegVal (pCh, RGFR_REG));
      pRes->nGain    = (VIN_CH_LMCR_DC_AD16_GET(LMCR.value)) ? 16 : 1;
      pRes->nShift   = LMDC.value;
      pRes->bRevPol  = VIN_CH_BCR2_REVPOL_GET(BCR2.value);
      pRes->rs       = VINETIC_IO_LT_RS_NONE;
   }

   return err;
}

/*******************************************************************************
Description:
   Local function containing the "Optimum Result Routine"
   for the Capacitance measurement
Arguments:
   pCh     - pointer to the channel structure
   pIO     - pointer to the capacitance parameter/result structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   To be used only with Vinetic revision < 2.1
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT14_DC_Cap_OptimumResult (VINETIC_CHANNEL *pCh,
                                                 VINETIC_IO_LT_CAPACITANCE *pIO)
{
   IFX_boolean_t   overflow[2]   = {IFX_FALSE};
   IFX_int16_t     slope         = 0;
   IFX_int32_t     res           = 0;
   IFX_uint16_t    samples;
   IFX_int32_t     err;

   pIO->res[0].nGain   = 1;
   pIO->res[1].nGain   = 1;
   pIO->res[0].nShift  = LT_DEFAULT_SHIFT;
   pIO->res[1].nShift  = LT_DEFAULT_SHIFT;
   pIO->res[0].bRevPol = VIN_CH_BCR2_REVPOL_GET(BCR2.value);
   pIO->res[1].bRevPol = VIN_CH_BCR2_REVPOL_GET(BCR2.value);

   /* Stage 1 *************************************************************** */
   /*         set default shift and gain                                      */
   LMDC.value = LT_DEFAULT_SHIFT;
   err = wrReg (pCh, LMDC_REG);
   VIN_CH_LMCR_DC_AD16_SET(LMCR.value, 0);
   if (err == IFX_SUCCESS)
      err = wrReg (pCh, LMCR_REG);

   for (slope = 0; slope <= 1 ; slope++)
   {
      if (err == IFX_SUCCESS)
      {
         err = LT14_IntegratedMeasurement(pCh,
                                         (IFX_int32_t*)&pIO->res[slope].nLMRES,
                                         LT_RAMP_TIMEOUT);
      }
   }

   /* Stage 2 *************************************************************** */
   /*         test for gain 16                                                */
   for (slope = 0; slope <= 1; slope++)
   {
      /* check if GAIN16 can be set. GAIN16*2 (safety margin) = 32 */
      if (abs (pIO->res[slope].nLMRES) < (RES_OVERFLOW / 32))
      {
         pIO->res[slope].nGain = 16;
      }
      /* so far the shift factor is identical for both ramps -
         just set the Gain */
      if (err == IFX_SUCCESS)
      {
         VIN_CH_LMCR_DC_AD16_SET(LMCR.value,
                                (pIO->res[slope].nGain == 1) ? 0 : 1);
         err = wrReg (pCh, LMCR_REG);
      }
      /* start another measurement */
      if (err == IFX_SUCCESS)
      {
         err = LT14_IntegratedMeasurement(pCh,
                                         (IFX_int32_t*)&pIO->res[slope].nLMRES,
                                         LT_RAMP_TIMEOUT);
      }
   }

   /* Stage 3 *************************************************************** */
   /*         calc optimum shift                                              */
   for (slope = 0; slope <= 1 ; slope++)
   {
      pIO->res[slope].nShift = (IFX_int16_t) ( (RES_OVERFLOW - LT_OPTRES_THRESHOLD) *
                                                pIO->res[slope].nShift /
                                                abs(pIO->res[slope].nLMRES) );
      if (err == IFX_SUCCESS)
      {
         LMDC.value = pIO->res[slope].nShift;
         err = wrReg (pCh, LMDC_REG);
      }
      if (err == IFX_SUCCESS)
      {
         VIN_CH_LMCR_DC_AD16_SET(LMCR.value,
                                (pIO->res[slope].nGain == 1) ? 0 : 1);
         err = wrReg (pCh, LMCR_REG);
      }
      if (err == IFX_SUCCESS)
      {
         err = LT14_IntegratedMeasurement(pCh,
                                         (IFX_int32_t*)&pIO->res[slope].nLMRES,
                                         LT_RAMP_TIMEOUT);
      }
      /* overflow check */
      if ( (pIO->res[slope].nLMRES == RES_OVERFLOW) ||
           (pIO->res[slope].nLMRES == (IFX_int16_t) NEG_OVERFLOW) )
         overflow[slope] = IFX_TRUE;
      else
         overflow[slope] = IFX_FALSE;
   }

   /* Stage 4 *************************************************************** */
   /*         overflow handling                                               */
   slope = 0;
   while ((err == IFX_SUCCESS) &&
          ((overflow[0] == IFX_TRUE) || (overflow[1] == IFX_TRUE)))
   {
      if (overflow[slope])
      {
         if ( (pIO->res[slope].nShift == 0) && (pIO->res[slope].nGain == 16) )
         {
            pIO->res[slope].nGain = 1;
         }
         else if ((pIO->res[slope].nShift == 0) && !(pIO->res[slope].nGain == 16))
         {
            TRACE(VINETIC, DBG_LEVEL_HIGH, ("Giving up...\n"));
            err = IFX_ERROR;
         }
         else
         {
            pIO->res[slope].nShift /= 4;
            pIO->res[slope].nShift *= 3;
         }
      }
      if (err == IFX_SUCCESS)
      {
         LMDC.value = pIO->res[slope].nShift;
         err = wrReg (pCh, LMDC_REG);
      }
      if (err == IFX_SUCCESS)
      {
         VIN_CH_LMCR_DC_AD16_SET(LMCR.value,
                                (pIO->res[slope].nGain == 1) ? 0 : 1);
         err = wrReg (pCh, LMCR_REG);
      }
      /* The ramp has to be executed in any case... */
      if (err == IFX_SUCCESS)
      {
         err = LT14_IntegratedMeasurement (pCh, &res, LT_RAMP_TIMEOUT);
      }
      /* ... while the result check has to be performed only if the
         last  measurement was an overflow */
      if ((err == IFX_SUCCESS) && (overflow[slope] == IFX_TRUE))
      {
         pIO->res[slope].nLMRES = res;
         if ( (res != RES_OVERFLOW) && (res != NEG_OVERFLOW) )
            overflow[slope] = IFX_FALSE;
      }
      /* loop control */
      slope = !slope;
   }

   if (err == IFX_SUCCESS)
   {
      samples                = (IFX_uint16_t) (LT_2KHZ / getRegVal (pCh, RGFR_REG));
      pIO->res[0].nSamples   = samples;
      pIO->res[1].nSamples   = samples;
   }
   return err;
}

/*******************************************************************************
Description:
   Local function containing the AC "Optimum Result Routine"
Arguments:
   pCh     - pointer to the channel structure
   pResult - 16bit result register value, measured with optimized
             shift and gain factors
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   To be used with Vinetic revision < 2.1
Note:
   Shift and gain factors are stored in the shadow registers.
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT14_AC_OptimumResult (VINETIC_CHANNEL *pCh,
                                             VINETIC_IO_LT_RESULT *pRes)
{
   IFX_int32_t          res           = 0;
   IFX_uint32_t         bigres;
   IFX_uint16_t         shift;
   IFX_uint16_t         iTime;
   IFX_int16_t          gainMargin;
   IFX_int32_t          err = IFX_SUCCESS;

   /* Stage 1 *************************************************************** */
   /*         set default shift and gain                                      */
   iTime = (VIN_CH_BCR1_LMABS_EN_GET(BCR1.value) ==
            VINETIC_IO_LT_RS_SQUARER) ?
            LT_AC_START_ITIME_SQUARER    :
            LT_AC_MIN_ITIME;
   VIN_CH_LMCR_LM_ITIME_MODIFY(LMCR.value, iTime);
   err = wrReg (pCh, LMCR_REG);
   if (err == IFX_SUCCESS)
   {
      shift = (VIN_CH_BCR1_LMABS_EN_GET(BCR1.value) ==
               VINETIC_IO_LT_RS_SQUARER) ?
               LT_AC_LMAC_DEFAULT_SQUARER   :
               LT_AC_LMAC_DEFAULT_RECTIFIER;
      VIN_CH_LMAC_LMAC_MODIFY(LM_AC.value, shift);
      err = wrReg (pCh, LMAC_REG);
   }
   if (err == IFX_SUCCESS)
   {
      VIN_CH_BCR1_LMAC64_SET(BCR1.value, 0);
      err = wrReg (pCh, BCR1_REG);
   }
   if (err == IFX_SUCCESS)
      err = LT14_IntegratedMeasurement (pCh, &res, LT_AC_TIMEOUT);
   /* Stage 2 *************************************************************** */
   /*         gain check                                                      */
   gainMargin = RES_OVERFLOW / (64*4);
   if (VIN_CH_BCR1_LMABS_EN_GET(BCR1.value) == VINETIC_IO_LT_RS_SQUARER)
      gainMargin /= 64;

   if ((err == IFX_SUCCESS) && (abs(res) < gainMargin))
   {
      VIN_CH_BCR1_LMAC64_SET(BCR1.value, 1);
      err = wrReg (pCh, BCR1_REG);
      if (err == IFX_SUCCESS)
         err = LT14_IntegratedMeasurement (pCh, &res, LT_AC_TIMEOUT);
   }
   /* Stage 3 *************************************************************** */
   /*         optimize integration time / number of samples                   */
   if (err == IFX_SUCCESS)
   {
      iTime = (IFX_uint16_t) ( VIN_CH_LMCR_LM_ITIME_GET(LMCR.value) *
              ( (IFX_uint32_t) (RES_OVERFLOW-LT_OPTRES_THRESHOLD_AC) /
                (IFX_uint32_t) abs(res) ) );
      if (iTime > 15)
         iTime = 15;
      if (iTime < LT_AC_MIN_ITIME)
         iTime = LT_AC_MIN_ITIME;
      VIN_CH_LMCR_LM_ITIME_MODIFY(LMCR.value, iTime);
      err = wrReg (pCh, LMCR_REG);
   }
   if (err == IFX_SUCCESS)
      err = LT14_IntegratedMeasurement (pCh, &res, LT_AC_TIMEOUT);
   /* Stage 4 *************************************************************** */
   /*         optimize shift factor                                           */
   if (err == IFX_SUCCESS)
   {
      bigres = ((IFX_uint32_t) abs(res)) << VIN_CH_LMAC_LMAC_GET(LM_AC.value);
      shift = 0;
      while ((bigres >= (RES_OVERFLOW - 4096)) && (shift < 7))
      {
         bigres >>= 1;
         shift ++;
      }
      rdReg(pCh, LMAC_REG);
      VIN_CH_LMAC_LMAC_MODIFY(LM_AC.value, shift);
      err = wrReg (pCh, LMAC_REG);
   }
   if (err == IFX_SUCCESS)
      err = LT14_IntegratedMeasurement (pCh, &res, LT_AC_TIMEOUT);
   /* Stage 5 *************************************************************** */
   /*         overflow check                                                  */
   while ((err == IFX_SUCCESS) && ((res == RES_OVERFLOW) ||
                               (res == (IFX_int16_t) NEG_OVERFLOW)))
   {
      shift = VIN_CH_LMCR_LM_ITIME_GET(LMCR.value);
      if (shift)
      {
         shift -= 1;
         VIN_CH_LMCR_LM_ITIME_MODIFY(LMCR.value, shift);
         err = wrReg (pCh, LMCR_REG);
      }
      if (err == IFX_SUCCESS)
         err = LT14_IntegratedMeasurement (pCh, &res, LT_AC_TIMEOUT);
   }

   if (err == IFX_SUCCESS)
   {
      pRes->nLMRES   = res;
      pRes->rs       = (VINETIC_IO_LT_RS) VIN_CH_BCR1_LMABS_EN_GET(BCR1.value);
      pRes->nGain    = (VIN_CH_BCR1_LMAC64_GET(BCR1.value)) ?
                           (VIN_CH_BCR1_LMABS_EN_GET(BCR1.value) ? 64 : 4096) :
                            1;
      pRes->nSamples = ((VIN_CH_LMCR_LM_ITIME_GET(LMCR.value)+1)*16 * 8);
      pRes->nShift   = 1 << VIN_CH_LMAC_LMAC_GET(LM_AC.value);
   }
   return err;
}

/*******************************************************************************
Description:
   Local function to configure the LM Filter type
Arguments:
   pCh     - pointer to the channel structure
   lmft    - Level Meter filter type
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   None.
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT14_AC_LM_FilterType (VINETIC_CHANNEL *pCh,
                                             VINETIC_IO_LT_AC_LM_FILTER_TYPE lmft)
{
   VINETIC_DEVICE *pDev = pCh->pParent;

   switch (lmft)
   {
      case VINETIC_IO_LT_AC_LM_FILTER_OFF:
         VIN_CH_LMCR_LM_FILT_SET    (LMCR.value, 0);
         VIN_CH_LMCR_LM_FILT_SEL_SET(LMCR.value, 0);
         break;
      case VINETIC_IO_LT_AC_LM_FILTER_BP:
         VIN_CH_LMCR_LM_FILT_SET    (LMCR.value, 1);
         VIN_CH_LMCR_LM_FILT_SEL_SET(LMCR.value, 1);
         break;
      case VINETIC_IO_LT_AC_LM_FILTER_NOTCH:
         VIN_CH_LMCR_LM_FILT_SET    (LMCR.value, 1);
         VIN_CH_LMCR_LM_FILT_SEL_SET(LMCR.value, 0);
         break;
      case VINETIC_IO_LT_AC_LM_FILTER_IGNORE:
         /* nothing to change */
         break;
      default:
         SET_ERROR(ERR_LT_UNKNOWN_PARAM);
         return IFX_ERROR;
   }
   return wrReg (pCh, LMCR_REG);
}

/*******************************************************************************
Description:
   AC Measurements
   Level Measurement, Transhybrid Measurement, SNR Measurement, Gain Tracking
   and Idle Noise Measurement
Arguments:
   pCh     - pointer to the channel structure
   pPar    - pointer to the parameter structure
   pRes    - pointer to the result structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   For Vinetic < 2.1
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT14_AC_Level (VINETIC_CHANNEL *pCh,
                                     VINETIC_LT_AC *pPar,
                                     VINETIC_IO_LT_RESULT *pRes)
{
   VINETIC_IO_LT_AC_TG  tg;
   IFX_int32_t          err        = IFX_SUCCESS;

   /* set linemode ********************************************************* */
   VIN_CH_OPMOD_CUR_MODE_CUR_MODIFY   (OPMOD_CUR.value, OPMOD_MODE_CUR_ACTIVE);
   VIN_CH_OPMOD_CUR_SUBMODE_CUR_MODIFY(OPMOD_CUR.value, OPMOD_SUBMOD_CUR_NORMAL);
   err = wrReg (pCh, OPMOD_CUR_REG);

   if (err == IFX_SUCCESS) /* Squarer / Recftifier ************************* */
   {
      switch (pPar->mode)
      {
         case VINETIC_LT_AC_MODE_LEVEL:
         case VINETIC_LT_AC_MODE_GAINTRACKING:
         case VINETIC_LT_AC_MODE_TRANSHYBRID:
         case VINETIC_LT_AC_MODE_SNR_NOISE:
         case VINETIC_LT_AC_MODE_SNR_SIGNAL:
            VIN_CH_BCR1_LMABS_EN_SET(BCR1.value, 0); /* Squarer */
            break;
         case VINETIC_LT_AC_MODE_IDLENOISE:
            VIN_CH_BCR1_LMABS_EN_SET(BCR1.value, 1); /* Rectifier */
            break;
         case VINETIC_LT_AC_MODE_DIRECT:
            VIN_CH_BCR1_LMABS_EN_SET(BCR1.value, pPar->p.pAC_direct->rsSelect);
            break;
      }
      err = wrReg (pCh, BCR1_REG);
   }
   if (err == IFX_SUCCESS) /* TH filter ************************************ */
   {
      switch (pPar->mode)
      {
         case VINETIC_LT_AC_MODE_LEVEL:
         case VINETIC_LT_AC_MODE_GAINTRACKING:
         case VINETIC_LT_AC_MODE_IDLENOISE:
         case VINETIC_LT_AC_MODE_SNR_SIGNAL:
         case VINETIC_LT_AC_MODE_SNR_NOISE:
            VIN_CH_BCR2_TH_DIS_SET(BCR2.value, 1);
            break;
         case VINETIC_LT_AC_MODE_TRANSHYBRID:
            VIN_CH_BCR2_TH_DIS_SET(BCR2.value, 0);
            break;
         case VINETIC_LT_AC_MODE_DIRECT:
            VIN_CH_BCR2_TH_DIS_SET(BCR2.value, pPar->p.pAC_direct->thFilter);
            break;
      }
      err = wrReg (pCh, BCR2_REG);
   }
   if (err == IFX_SUCCESS) /* TG configuration ***************************** */
   {
      switch (pPar->mode)
      {
         case VINETIC_LT_AC_MODE_IDLENOISE:
            tg.state = VINETIC_IO_LT_AC_TG_STATE_OFF;
            tg.freq  = 0;
            err = LT_AC_TG_Config (pCh, VINETIC_IO_LT_TG_1, &tg);
            if (err == IFX_SUCCESS)
               err = LT_AC_TG_Config (pCh, VINETIC_IO_LT_TG_2, &tg);
            IFXOS_Wait (30);
            break;
         case VINETIC_LT_AC_MODE_LEVEL:
         case VINETIC_LT_AC_MODE_GAINTRACKING:
         case VINETIC_LT_AC_MODE_TRANSHYBRID:
         case VINETIC_LT_AC_MODE_SNR_SIGNAL:
            tg.state = VINETIC_IO_LT_AC_TG_STATE_ON;
            tg.freq  = pPar->p.pAC->freq;
            tg.level = pPar->p.pAC->level;
            if (pPar->p.pAC->freq > LT_2KHZ) /* f > 2 kHz */
               tg.qual = VINETIC_IO_LT_BP_QUAL_32;
            else
               tg.qual = VINETIC_IO_LT_BP_QUAL_128;
            err = LT_AC_TG_Config (pCh, VINETIC_IO_LT_TG_1, &tg);
            /* return quantized values */
            pPar->p.pAC->freq  = tg.freq;
            pPar->p.pAC->level = tg.level;
            break;
         case VINETIC_LT_AC_MODE_DIRECT:
            err = LT_AC_TG_Config (pCh, VINETIC_IO_LT_TG_1,
                                   &pPar->p.pAC_direct->tg[VINETIC_IO_LT_TG_1]);
            if (err == IFX_SUCCESS)
               err = LT_AC_TG_Config (pCh, VINETIC_IO_LT_TG_2,
                                   &pPar->p.pAC_direct->tg[VINETIC_IO_LT_TG_2]);
            break;
         case VINETIC_LT_AC_MODE_SNR_NOISE:
            /* no TG changes required
            - settings were done for VINETIC_LT_AC_MODE_SNR_SIGNAL */
            break;
      }
   }
   if (err == IFX_SUCCESS) /* LM Sel, LM filter type **************************** */
   {
      VIN_CH_LMCR_LM_SEL_MODIFY(LMCR.value, LMCR_LM_SEL_AC_TX);
      switch (pPar->mode)
      {
         case VINETIC_LT_AC_MODE_LEVEL:
         case VINETIC_LT_AC_MODE_GAINTRACKING:
         case VINETIC_LT_AC_MODE_TRANSHYBRID:
         case VINETIC_LT_AC_MODE_SNR_SIGNAL:
            err = LT14_AC_LM_FilterType (pCh, VINETIC_IO_LT_AC_LM_FILTER_BP);
            break;
         case VINETIC_LT_AC_MODE_IDLENOISE:
            err = LT14_AC_LM_FilterType (pCh, VINETIC_IO_LT_AC_LM_FILTER_OFF);
            break;
         case VINETIC_LT_AC_MODE_SNR_NOISE:
            err = LT14_AC_LM_FilterType (pCh, VINETIC_IO_LT_AC_LM_FILTER_NOTCH);
            break;
         case VINETIC_LT_AC_MODE_DIRECT:
            switch (pPar->p.pAC_direct->lmSelect)
            {
               case VINETIC_IO_LT_AC_LM_SELECT_TX:
                  /* nothing more to do */
                  break;
               case VINETIC_IO_LT_AC_LM_SELECT_RX:
                  VIN_CH_LMCR_LM_SEL_MODIFY(LMCR.value, LMCR_LM_SEL_AC_RX);
                  break;
               case VINETIC_IO_LT_AC_LM_SELECT_RX_TX:
                  VIN_CH_LMCR_LM_SEL_MODIFY(LMCR.value, LMCR_LM_SEL_AC_RX_TX);
                  break;
            }
            err = LT14_AC_LM_FilterType (pCh, pPar->p.pAC_direct->lmFilter.type);
            break;
      }
      /* NOTE: The LMCR register is written in LT14_AC_LM_FilterType (!) */
   }
   if (err == IFX_SUCCESS) /* LM filter quality ********************************* */
   {
      if (pPar->mode != VINETIC_LT_AC_MODE_DIRECT)
      {
         if (pPar->p.pAC->freq > LT_2KHZ) /* f > 2 kHz */
            VIN_CH_LMAC_BP3Q_MODIFY(LM_AC.value, VINETIC_IO_LT_BP_QUAL_32);
         else
            VIN_CH_LMAC_BP3Q_MODIFY(LM_AC.value, VINETIC_IO_LT_BP_QUAL_128);
      }
      else
         VIN_CH_LMAC_BP3Q_MODIFY(LM_AC.value, pPar->p.pAC_direct->lmFilter.qual);
      err = wrReg (pCh, LMAC_REG);
   }
   if (err == IFX_SUCCESS) /* LM filter frequency ******************************* */
   {
      if (pPar->mode != VINETIC_LT_AC_MODE_DIRECT)
      {
         if (pPar->p.pAC->freq != 0)
            err = setRegVal (pCh, BP3F_REG, pPar->p.pAC->freq);
      }
      else
      {
         if (pPar->p.pAC_direct->lmFilter.freq !=0)
            err = setRegVal (pCh, BP3F_REG, pPar->p.pAC_direct->lmFilter.freq);
      }
      if (err == IFX_SUCCESS)
         err = wrReg (pCh, BP3F_REG);
   }

   IFXOS_Wait (((pPar->p.pAC->freq/1000)*LT_AC_FILTER_ADAPTION_FACTOR/1000));

   if (err == IFX_SUCCESS)
      err = LT14_AC_OptimumResult (pCh, pRes);

   return err;
}

/*******************************************************************************
Description:
   Local function to set threshold values to maximum values
   (depending on linemode)
Arguments:
   pCh     - pointer to the channel structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   For use with Vinetic < 2.1 only
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT14_DC_SilentThreshold (VINETIC_CHANNEL *pCh)
{
   IFX_int32_t             err;

   /* Switch off lp1 and lp2 to reduce noise on the line during the
      measurement (LM-SEL switch) */
   VIN_CH_TSTR3_DC_LP1_DIS_SET (TSTR3.value, 1);
   VIN_CH_TSTR3_DC_LP2_DIS_SET (TSTR3.value, 1);
   err = wrReg (pCh, TSTR3_REG);
   /* Activate Test Regitster settings */
   if (err == IFX_SUCCESS)
   {
      VIN_CH_BCR1_TEST_EN_SET  (BCR1.value, 1);
      err = wrReg (pCh, BCR1_REG);
   }

   switch (VIN_CH_OPMOD_CUR_MODE_CUR_GET(OPMOD_CUR.value))
   {
      case OPMOD_MODE_CUR_ACTIVE:
      case OPMOD_MODE_CUR_ACTIVE_BOOSTED:
         HACT.value = LT14_MAX_THRESHOLD_HACT;
         HAHY.value = LT14_MAX_THRESHOLD_HAHY;
         if (err == IFX_SUCCESS)
            err = wrReg (pCh, HACT_REG);
         if (err == IFX_SUCCESS)
            err = wrReg (pCh, HAHY_REG);
         break;
      case OPMOD_MODE_CUR_RINGING:
         VRTL.value = LT14_MAX_THRESHOLD_VRTL;
         HRING.value= LT14_MAX_THRESHOLD_HRING;
         HACRT.value= LT14_MAX_THRESHOLD_HACRT;
         FRTR.value = LT14_MAX_THRESHOLD_FRTR;
         if (err == IFX_SUCCESS)
            err = wrReg (pCh, VRTL_REG);
         if (err == IFX_SUCCESS)
            err = wrReg (pCh, HRING_REG);
         if (err == IFX_SUCCESS)
            err = wrReg (pCh, HACRT_REG);
         if (err == IFX_SUCCESS)
            err = wrReg (pCh, FRTR_REG);
         break;
      case OPMOD_MODE_CUR_PDNR:
         HPD.value = LT14_MAX_THRESHOLD_HPD;
         if (err == IFX_SUCCESS)
            err = wrReg (pCh, HPD_REG);
         break;
      case OPMOD_MODE_CUR_RING_PAUSE:
         /* TBD set thresholds for Ring Pause Mode, not yet required... */
         err = IFX_SUCCESS;
         break;
      default:
         /* do nothing */
         err = IFX_SUCCESS;
         break;
   }

   return err;
}
#endif /* VIN_V14_SUPPORT */


#ifdef VIN_V21_SUPPORT
/*******************************************************************************
Description:
   Local function to configure the LM Filter type
Arguments:
   pCh     - pointer to the channel structure
   lmft    - Level Meter filter type
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   None.
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT21_LM_FilterType (VINETIC_CHANNEL *pCh,
                                          VINETIC_IO_LT_AC_LM_FILTER_TYPE lmft)
{
   switch (lmft)
   {
      case VINETIC_IO_LT_AC_LM_FILTER_OFF:
         VIN21_CH_LMCR2_LM_FILT_SEL_MODIFY (LMCR2.value,
                                            LMCR2_LM_FILT_SEL_BYPASS);
         break;
      case VINETIC_IO_LT_AC_LM_FILTER_BP:
         VIN21_CH_LMCR2_LM_FILT_SEL_MODIFY (LMCR2.value,
                                            LMCR2_LM_FILT_SEL_BANDPASS);
         break;
      case VINETIC_IO_LT_AC_LM_FILTER_NOTCH:
         VIN21_CH_LMCR2_LM_FILT_SEL_MODIFY (LMCR2.value,
                                            LMCR2_LM_FILT_SEL_NOTCH);
         break;
      case VINETIC_IO_LT_AC_LM_FILTER_LP:
         VIN21_CH_LMCR2_LM_FILT_SEL_MODIFY (LMCR2.value,
                                            LMCR2_LM_FILT_SEL_LOWPASS);
         break;
      case VINETIC_IO_LT_AC_LM_FILTER_HP:
         VIN21_CH_LMCR2_LM_FILT_SEL_MODIFY (LMCR2.value,
                                            LMCR2_LM_FILT_SEL_HIGHPASS);
         break;
      case VINETIC_IO_LT_AC_LM_FILTER_IGNORE:
         /* do not change */
         break;
   }
   return wrReg (pCh, LMCR2_REG);
}

/*******************************************************************************
Description:
   Level Meter Filter Frequency Configuration
   Set the filter frequency for the Vinetic 2.1 Level Meter Filter.
Arguments:
   pCh     - pointer to the channel structure
   freq    - frequency [mHz]
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   For Vinetic 2.1
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT21_LM_FilterFreq (VINETIC_CHANNEL *pCh, IFX_int32_t freq)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t     f    = 0;
   IFX_int32_t     err  = IFX_ERROR;

   /* convert frequency from mHz to Hz */
   freq /= 1000;
   /* find accourate filter coefficients */
   while ((VINETIC_V21_Filter_Table[f].freq != -1) &&
          (freq >= VINETIC_V21_Filter_Table[f].freq))
   {
      if (freq == VINETIC_V21_Filter_Table[f].freq)
      {
         LM_FILT1.value = VINETIC_V21_Filter_Table[f].filt1;
         LM_FILT2.value = VINETIC_V21_Filter_Table[f].filt2;
         LM_FILT3.value = VINETIC_V21_Filter_Table[f].filt3;
         LM_FILT4.value = VINETIC_V21_Filter_Table[f].filt4;
         LM_FILT5.value = VINETIC_V21_Filter_Table[f].filt5;
         err = IFX_SUCCESS;
      }
      f++;
   }
   if (err != IFX_SUCCESS)
      SET_ERROR(ERR_LT_UNKNOWN_PARAM);

   if (err == IFX_SUCCESS)
      err = wrReg (pCh, LM_FILT1_REG);
   if (err == IFX_SUCCESS)
      err = wrReg (pCh, LM_FILT2_REG);
   if (err == IFX_SUCCESS)
      err = wrReg (pCh, LM_FILT3_REG);
   if (err == IFX_SUCCESS)
      err = wrReg (pCh, LM_FILT4_REG);
   if (err == IFX_SUCCESS)
      err = wrReg (pCh, LM_FILT5_REG);

   return err;
}

/*******************************************************************************
   Description:
   Local function to execute an non integrated measurement
Arguments:
   pCh     - pointer to the channel structure
   pResult - pointer to the result structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   To be used with Vinetic revision >= 2.1
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT21_NonIntegratedMeasurement (VINETIC_CHANNEL      *pCh,
                                                     VINETIC_IO_LT_RESULT *pRes)
{
   IFX_int32_t             err;

   IFXOS_DELAYUS (DCDSP_DELAY_V21); /* be sure all settings are in the DCDSP */

   /* concat 32bit LMRES */
   err = rdReg(pCh, LMRES_REG);
   /* shift result by 8 bits, so that the formula for a 32bit result applies */
   pRes->nLMRES   = LMRES.value << 8;

   if (err == IFX_SUCCESS)
   {
      pRes->nGain    = 1; /* unused for Vin V21 */
      pRes->nSamples = 1;
      pRes->nShift   = LT21_LM_getShift (pCh);
      pRes->bRevPol  = VIN21_CH_BCR2_REVPOL_GET(BCR2.value);
      switch (VIN21_CH_LMCR_LM_MAG_GET(LMCR.value))
      {
         case LMCR_LM_MAG_SQUARE:
            pRes->rs = VINETIC_IO_LT_RS_SQUARER;
            break;
         case LMCR_LM_MAG_RECITFY:
            pRes->rs = VINETIC_IO_LT_RS_RECTIFIER;
            break;
         case LMCR_LM_MAG_OFF:
            pRes->rs = VINETIC_IO_LT_RS_NONE;
            break;
      }
   }
   return err;
}

/*******************************************************************************
   Description:
   Local function to execute a non integrated measurement to measure the
   time between two thresholds. Although this is a non integrated measurement
   we have to wait for the LM-OK interrupt, indicating that the lower
   threshold was reached
   used only in the resistance measurement via a RC network
Arguments:
   pCh     - pointer to the channel structure
   pResult - pointer to the result structure
   timeout - time to wait for LM_OK interrupt
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   To be used with Vinetic revision >= 2.1
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT21_TimeMeasurement (VINETIC_CHANNEL      *pCh,
                                            VINETIC_IO_LT_RESULT *pRes,
                                            IFX_int32_t           timeout)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t     err  = IFX_SUCCESS;

   /* wait for LM_OK interrupt within a time out */
   IFXOS_DELAYMS (1);  /* Wait at least 1 ms before reading LMRES, LMRES2 */
   /* check if event occured already */
   if (! (pCh->IsrFlag [EV_SRS1] & SRS1_LM_OK))
      if ( IFXOS_WaitEvent_timeout (pCh->wqLT_LM_OK, timeout) == IFX_ERROR )
      {
         TRACE(VINETIC, DBG_LEVEL_HIGH, ("ERROR timeout waiting for LM_OK\n"));
         SET_ERROR (ERR_LT_TIMEOUT_LM_OK);
         err = IFX_ERROR;
      }

   /* concat 32bit LMRES */
   if (err == IFX_SUCCESS)
   {
      err = rdReg(pCh, LMRES_REG);
      pRes->nLMRES   = LMRES.value << 16;
   }
   if (err == IFX_SUCCESS)
   {
      err = rdReg(pCh, LMRES2_REG);
      pRes->nLMRES  |= LMRES2.value;
      pRes->nGain    = 1; /* unused for Vin V21 */
      pRes->nSamples = LT21_LM_getSamples (pCh);
      pRes->nShift   = LT21_LM_getShift (pCh);
      switch (VIN21_CH_LMCR_LM_MAG_GET(LMCR.value))
      {
         case LMCR_LM_MAG_SQUARE:
            pRes->rs = VINETIC_IO_LT_RS_SQUARER;
            break;
         case LMCR_LM_MAG_RECITFY:
            pRes->rs = VINETIC_IO_LT_RS_RECTIFIER;
            break;
         case LMCR_LM_MAG_OFF:
            pRes->rs = VINETIC_IO_LT_RS_NONE;
            break;
      }
   }

   if (err == IFX_SUCCESS)
   {  /* mask LM_OK */
      VIN21_CH_MR_SRS1_M_LM_OK_SET(MR_SRS1.value, 1);
      err = wrReg (pCh, MR_SRS1_REG);
   }

   return err;
}

/*******************************************************************************
Description:
   AC Measurements
   Level Measurement, Transhybrid Measurement, SNR Measurement, Gain Tracking
   and Idle Noise Measurement
Arguments:
   pCh     - pointer to the channel structure
   pPar    - pointer to the parameter structure
   pRes    - pointer to the result structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   For Vinetic 2.1
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT21_AC_Level (VINETIC_CHANNEL *pCh,
                                     VINETIC_LT_AC *pPar,
                                     VINETIC_IO_LT_RESULT *pRes)
{
   VINETIC_IO_LT_AC_TG             tg;
   VINETIC_IO_LT_AC_LM_FILTER_TYPE lm_filter = VINETIC_IO_LT_AC_LM_FILTER_OFF;
   IFX_int32_t                     err       = IFX_SUCCESS;

   /* set linemode ************************************** */
   VIN_CH_OPMOD_CUR_MODE_CUR_MODIFY   (OPMOD_CUR.value, OPMOD_MODE_CUR_ACTIVE);
   VIN_CH_OPMOD_CUR_SUBMODE_CUR_MODIFY(OPMOD_CUR.value, OPMOD_SUBMOD_CUR_NORMAL);
   err = wrReg (pCh, OPMOD_CUR_REG);
   if (err == IFX_SUCCESS) /* TG configuration ***************************** */
   {
      switch (pPar->mode)
      {
         case VINETIC_LT_AC_MODE_IDLENOISE:
            tg.state = VINETIC_IO_LT_AC_TG_STATE_OFF;
            tg.freq  = 0;
            err = LT_AC_TG_Config (pCh, VINETIC_IO_LT_TG_1, &tg);
            if (err == IFX_SUCCESS)
               err = LT_AC_TG_Config (pCh, VINETIC_IO_LT_TG_2, &tg);
            IFXOS_Wait (LT21_TG_SETTLING_TIME);
            break;
         case VINETIC_LT_AC_MODE_LEVEL:
         case VINETIC_LT_AC_MODE_GAINTRACKING:
         case VINETIC_LT_AC_MODE_TRANSHYBRID:
         case VINETIC_LT_AC_MODE_SNR_SIGNAL:
            tg.state = VINETIC_IO_LT_AC_TG_STATE_ON;
            tg.freq  = pPar->p.pAC->freq;
            tg.level = pPar->p.pAC->level;
            err = LT_AC_TG_Config (pCh, VINETIC_IO_LT_TG_1, &tg);
            /* return quantized values */
            /* since we calculate coefficients for the Vinetic 2.1 it's not
               required to return quantized values... */
            /* pPar->p.pAC->freq  = tg.freq; pPar->p.pAC->level = tg.level; */
            break;
         case VINETIC_LT_AC_MODE_DIRECT:
            err = LT_AC_TG_Config (pCh, VINETIC_IO_LT_TG_1,
                                   &pPar->p.pAC_direct->tg[VINETIC_IO_LT_TG_1]);
            if (err == IFX_SUCCESS)
               err = LT_AC_TG_Config (pCh, VINETIC_IO_LT_TG_2,
                                   &pPar->p.pAC_direct->tg[VINETIC_IO_LT_TG_2]);
            break;
         case VINETIC_LT_AC_MODE_SNR_NOISE:
            /* no TG changes required
            - settings were done for VINETIC_LT_AC_MODE_SNR_SIGNAL */
            break;
      }
   }
   if (err == IFX_SUCCESS) /* Squarer / Rectifier ************************** */
   {
      switch (pPar->mode)
      {
         case VINETIC_LT_AC_MODE_LEVEL:
         case VINETIC_LT_AC_MODE_TRANSHYBRID:
            VIN21_CH_LMCR_LM_MAG_MODIFY (LMCR.value, LMCR_LM_MAG_SQUARE);
            break;
         case VINETIC_LT_AC_MODE_SNR_NOISE:
         case VINETIC_LT_AC_MODE_SNR_SIGNAL:
         case VINETIC_LT_AC_MODE_GAINTRACKING:
         case VINETIC_LT_AC_MODE_IDLENOISE:
            VIN21_CH_LMCR_LM_MAG_MODIFY (LMCR.value, LMCR_LM_MAG_RECITFY);
            break;
         case VINETIC_LT_AC_MODE_DIRECT:
            switch (pPar->p.pAC_direct->rsSelect)
            {
               case VINETIC_IO_LT_RS_SQUARER:
                  VIN21_CH_LMCR_LM_MAG_MODIFY (LMCR.value, LMCR_LM_MAG_SQUARE);
                  break;
               case VINETIC_IO_LT_RS_RECTIFIER:
                  VIN21_CH_LMCR_LM_MAG_MODIFY (LMCR.value, LMCR_LM_MAG_RECITFY);
                  break;
               case VINETIC_IO_LT_RS_NONE:
                  VIN21_CH_LMCR_LM_MAG_MODIFY (LMCR.value, LMCR_LM_MAG_OFF);
                  break;
            }
            break;
      }
      /* LM Sel, LM filter type **************************************** */
      VIN21_CH_LMCR_LM_SEL_MODIFY(LMCR.value, LMCR_LM_SEL_AC_TX);

      switch (pPar->mode)
      {
         case VINETIC_LT_AC_MODE_SNR_SIGNAL:
         case VINETIC_LT_AC_MODE_GAINTRACKING:
         case VINETIC_LT_AC_MODE_LEVEL:
         case VINETIC_LT_AC_MODE_TRANSHYBRID:
            lm_filter = VINETIC_IO_LT_AC_LM_FILTER_BP;
            break;
         case VINETIC_LT_AC_MODE_IDLENOISE:
            lm_filter = VINETIC_IO_LT_AC_LM_FILTER_OFF;
            break;
         case VINETIC_LT_AC_MODE_SNR_NOISE:
            lm_filter = VINETIC_IO_LT_AC_LM_FILTER_NOTCH;
            break;
         case VINETIC_LT_AC_MODE_DIRECT:
            switch (pPar->p.pAC_direct->lmSelect)
            {
               case VINETIC_IO_LT_AC_LM_SELECT_TX:
                  /* nothing more to do */
                  break;
               case VINETIC_IO_LT_AC_LM_SELECT_RX:
                  VIN21_CH_LMCR_LM_SEL_MODIFY(LMCR.value, LMCR_LM_SEL_AC_RX);
                  break;
               case VINETIC_IO_LT_AC_LM_SELECT_RX_TX:
                  VIN21_CH_LMCR_LM_SEL_MODIFY(LMCR.value, LMCR_LM_SEL_AC_RX_TX);
                  break;
            }
            lm_filter = pPar->p.pAC_direct->lmFilter.type;
            break;
      }
      err = wrReg(pCh, LMCR_REG);
   }

   if (err == IFX_SUCCESS)
   {
      /* LM filter frequency (if not off) ****************************** */
      if ((lm_filter != VINETIC_IO_LT_AC_LM_FILTER_OFF) &&
          (lm_filter != VINETIC_IO_LT_AC_LM_FILTER_IGNORE))
      {
         if (pPar->mode != VINETIC_LT_AC_MODE_DIRECT)
         {
            if (pPar->p.pAC->freq != 0)
               err = LT21_LM_FilterFreq (pCh, pPar->p.pAC->freq);
         }
         else
         {
            if (pPar->p.pAC_direct->lmFilter.freq != 0)
               err = LT21_LM_FilterFreq (pCh, pPar->p.pAC_direct->lmFilter.freq);
         }
         /* TBD decide about handling of missing filter coefficients */
         /*     so far, switch filter off and continue */
         if (err != IFX_SUCCESS)
         {
            lm_filter = VINETIC_IO_LT_AC_LM_FILTER_OFF;
            TRACE(VINETIC, DBG_LEVEL_HIGH, ("WARNING, switching off lm filter"
                  " - no coefficient found\n"));
            err = IFX_SUCCESS;
         }
      }
      if (err == IFX_SUCCESS)
         err = LT21_LM_FilterType (pCh, lm_filter);
   }

   if (err == IFX_SUCCESS) /* TH filter ************************************ */
   {
      switch (pPar->mode)
      {
         case VINETIC_LT_AC_MODE_LEVEL:
         case VINETIC_LT_AC_MODE_GAINTRACKING:
         case VINETIC_LT_AC_MODE_IDLENOISE:
         case VINETIC_LT_AC_MODE_SNR_SIGNAL:
         case VINETIC_LT_AC_MODE_SNR_NOISE:
            VIN21_CH_BCR2_TH_DIS_SET(BCR2.value, 1);
            break;
         case VINETIC_LT_AC_MODE_TRANSHYBRID:
            VIN21_CH_BCR2_TH_DIS_SET(BCR2.value, 0);
            break;
         case VINETIC_LT_AC_MODE_DIRECT:
            VIN21_CH_BCR2_TH_DIS_SET(BCR2.value, pPar->p.pAC_direct->thFilter);
            break;
      }
      err = wrReg (pCh, BCR2_REG);
   }

   /* Wait for the digital filters to adapt to the new coefficients... */
   if ((lm_filter != VINETIC_IO_LT_AC_LM_FILTER_OFF) &&
       (lm_filter != VINETIC_IO_LT_AC_LM_FILTER_IGNORE))
      IFXOS_Wait (LT21_AC_FILTER_ADAPTION_TIME);

   if (err == IFX_SUCCESS)
   {
      err = LT21_LM_IntegrationCfg (pCh, LMCR2_LM_DCTX8K_8K,
                                    LMCR2_LM_ITS_LM_ITIME,
                                    100 /* ms */, 0 /* ms */);
   }
   if (err == IFX_SUCCESS)
      err = LT21_IntegratedMeasurement (pCh, pRes, LT_AC_TIMEOUT);

   return err;
}
#endif /* VIN_V21_SUPPORT */

/*******************************************************************************
Description:
   Local function to measure a current as part of a measurement.
Arguments:
   pCh     - pointer to the channel structure
   mode    - defines how to configure the LM-SEL multiplexer
             VINETIC_IO_LT_CURRENT_MODE_IT,
             VINETIC_IO_LT_CURRENT_MODE_IL
   submode - defines whether the AC or DC part shall be measured
             VINETIC_IO_LT_CURRENT_SUBMODE_DC,
             VINETIC_IO_LT_CURRENT_SUBMODE_AC
   pRes    - pointer to a result structure containing all required coefficients
             to calculate the current on application level
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   None
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT_DC_Current (VINETIC_CHANNEL *pCh,
                                     VINETIC_IO_LT_CURRENT_MODE mode,
                                     VINETIC_IO_LT_CURRENT_SUBMODE submode,
                                     VINETIC_IO_LT_RESULT *pRes)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t     err;

   switch (mode)
   {
      case VINETIC_IO_LT_CURRENT_MODE_IT:
         VIN_CH_LMCR_LM_SEL_MODIFY(LMCR.value, LMCR_LM_SEL_I_DC_IT);
         break;
      case VINETIC_IO_LT_CURRENT_MODE_IL:
         VIN_CH_LMCR_LM_SEL_MODIFY(LMCR.value, LMCR_LM_SEL_I_DC_IL);
         break;
   }
   err = wrReg (pCh, LMCR_REG);

   /* Set Offset to precalibrated value */
   if ((err == IFX_SUCCESS) && (pCh->offset.bInitialized))
   {
      OFR.value = pCh->offset.nSlic;
      err = wrReg (pCh, OFR_REG);
   }
   else
      TRACE(VINETIC, DBG_LEVEL_HIGH, ("Warning: Offsets not calibrated\n"));

   switch (submode)
   {
      case VINETIC_IO_LT_CURRENT_SUBMODE_DC:
         /* nothing additional to do */
         break;
      case VINETIC_IO_LT_CURRENT_SUBMODE_AC:
         if (err == IFX_SUCCESS)
            err = LT_DC_AC_Handling (pCh);
         break;
   }

   if (err == IFX_SUCCESS)
   {
      switch (pDev->nChipMajorRev)
      {
#ifdef VIN_V14_SUPPORT
         case VINETIC_V1x:
            err = LT14_DC_OptimumResult (pCh, pRes);
            break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
         case VINETIC_V2x:
            if (pCh->ltConf.nIntegrationTime)
               err = LT21_IntegratedMeasurement (pCh, pRes, LT21_DC_TIMEOUT);
            else
               err = LT21_NonIntegratedMeasurement (pCh, pRes);

#ifdef LT21_USE_LM_PREGAIN
            /* for DC current only */
            /* check if the LM-Pregain +20dB should be activated */
            /* TBD add shift factor to formula,
            set a define for 6.99 mA = 0x8F358 */
            if ((err == IFX_SUCCESS) &&
                (submode == VINETIC_IO_LT_CURRENT_SUBMODE_DC) &&
                (abs(pRes->nLMRES) / (pRes->nSamples) < 0x8F358))
            {
               VIN21_CH_LMCR_LM_PRE_GAIN_SET(LMCR.value, 1);
               err = wrReg (pCh, LMCR_REG);
               /* set OFR for IT measurement with LM_PREGAIN */
               if ((err == IFX_SUCCESS) && (pCh->offset.bInitialized))
               {
                  OFR.value = pCh->offset.nSlicPregain;
                  err = wrReg(pCh, OFR_REG);
               }
               /* start another measurement and scale the result */
               if (err == IFX_SUCCESS)
               {
                  if (pCh->ltConf.nIntegrationTime)
                     err = LT21_IntegratedMeasurement (pCh, pRes, LT21_DC_TIMEOUT);
                  else
                     err = LT21_NonIntegratedMeasurement (pCh, pRes);

                  /* divide result by 10 (-20dB) */
                  pRes->nLMRES /= 10;
                  /* pRes->nSamples *= 10; */
               }
               /* switch off pregain for consecutive measurements */
               if (err == IFX_SUCCESS)
               {
                  VIN21_CH_LMCR_LM_PRE_GAIN_SET(LMCR.value, 0);
                  err = wrReg (pCh, LMCR_REG);
               }
            }
#endif /* LT21_USE_LM_PREGAIN */
            break;
#endif /* VIN_V21_SUPPORT */
         default:
            SET_ERROR(ERR_UNKNOWN_VERSION);
            err = IFX_ERROR;
            break;
      }
   }
   return err;
}

/*******************************************************************************
Description:
   Local function to measure a voltage as part of a measurement.
Arguments:
   pCh     - pointer to the channel structure
   mode    - defines how to configure the LM-SEL multiplexer
             VINETIC_IO_LT_VOLTAGE_MODE_DCN_DCP,
             VINETIC_IO_LT_VOLTAGE_MODE_IO2,
             VINETIC_IO_LT_VOLTAGE_MODE_IO3,
             VINETIC_IO_LT_VOLTAGE_MODE_IO4,
             VINETIC_IO_LT_VOLTAGE_MODE_IO4_IO3,
             VINETIC_IO_LT_VOLTAGE_MODE_IO4_IO2,
             VINETIC_IO_LT_VOLTAGE_MODE_IO3_IO2,
             VINETIC_IO_LT_VOLTAGE_MODE_VDD
   submode - defines whether the AC or DC part shall be measured
             VINETIC_IO_LT_VOLTAGE_SUBMODE_DC,
             VINETIC_IO_LT_VOLTAGE_SUBMODE_AC
   pRes    - pointer to a result structure containing all required coefficients
             to calculate the voltage on application level
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   None
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT_DC_Voltage (VINETIC_CHANNEL *pCh,
                                     VINETIC_IO_LT_VOLTAGE_MODE mode,
                                     VINETIC_IO_LT_VOLTAGE_SUBMODE submode,
                                     VINETIC_IO_LT_RESULT *pRes)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t     err;

   switch (mode)
   {
      case VINETIC_IO_LT_VOLTAGE_MODE_DCN_DCP:
         switch (pDev->nChipMajorRev)
         {
#ifdef VIN_V14_SUPPORT
            case VINETIC_V1x:
               VIN_CH_LMCR_DC_AD16_SET(LMCR.value, 0);
               VIN_CH_LMCR_LM_ONCE_SET(LMCR.value, 0);
               break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
            case VINETIC_V2x:
               /* nothing version specific */
               break;
#endif /* VIN_V21_SUPPORT */
            default:
               SET_ERROR(ERR_UNKNOWN_VERSION);
               err = IFX_ERROR;
               break;
         }
         VIN_CH_LMCR_LM_SEL_MODIFY(LMCR.value, LMCR_LM_SEL_U_DC_DCN_DCP);
         break;
      case VINETIC_IO_LT_VOLTAGE_MODE_IO2:
         VIN_CH_LMCR_LM_SEL_MODIFY(LMCR.value, LMCR_LM_SEL_U_IO2);
         break;
      case VINETIC_IO_LT_VOLTAGE_MODE_IO3:
         VIN_CH_LMCR_LM_SEL_MODIFY(LMCR.value, LMCR_LM_SEL_U_IO3);
         break;
      case VINETIC_IO_LT_VOLTAGE_MODE_IO4:
         VIN_CH_LMCR_LM_SEL_MODIFY(LMCR.value, LMCR_LM_SEL_U_IO4);
         break;
      case VINETIC_IO_LT_VOLTAGE_MODE_IO4_IO3:
         VIN_CH_LMCR_LM_SEL_MODIFY(LMCR.value, LMCR_LM_SEL_U_IO4_IO3);
         break;
      case VINETIC_IO_LT_VOLTAGE_MODE_IO4_IO2:
         VIN_CH_LMCR_LM_SEL_MODIFY(LMCR.value, LMCR_LM_SEL_U_IO4_IO2);
         break;
      case VINETIC_IO_LT_VOLTAGE_MODE_IO3_IO2:
         VIN_CH_LMCR_LM_SEL_MODIFY(LMCR.value, LMCR_LM_SEL_U_IO3_IO2);
         break;
      case VINETIC_IO_LT_VOLTAGE_MODE_VDD:
         VIN_CH_LMCR_LM_SEL_MODIFY(LMCR.value, LMCR_LM_SEL_VDD);
         break;
   }
   err = wrReg (pCh, LMCR_REG);

   /* Set Offset to precalibrated value */
   if ((err == IFX_SUCCESS) && (pCh->offset.bInitialized))
   {
      OFR.value = pCh->offset.nPrefi;
      err = wrReg (pCh, OFR_REG);
   }
   else
      TRACE(VINETIC, DBG_LEVEL_HIGH, ("Warning: Offsets not calibrated\n"));

   switch (submode)
   {
      case VINETIC_IO_LT_VOLTAGE_SUBMODE_DC:
         /* nothing additional to do */
         break;
      case VINETIC_IO_LT_VOLTAGE_SUBMODE_AC:
         if (err == IFX_SUCCESS)
            err = LT_DC_AC_Handling (pCh);
         break;
      default:
         SET_ERROR(ERR_LT_UNKNOWN_PARAM);
         return IFX_ERROR;
   }

   if (err == IFX_SUCCESS)
   {
      switch (pDev->nChipMajorRev)
      {
#ifdef VIN_V14_SUPPORT
         case VINETIC_V1x:
            if (mode != VINETIC_IO_LT_VOLTAGE_MODE_DCN_DCP)
            {
               err = LT14_DC_OptimumResult (pCh, pRes);
            }
            else
            {
               /* Simplified V_DCN_DCP measurement,
               result is not measured, i.e. it can be read out. */
               IFXOS_DELAYUS (DCDSP_DELAY);
               err = rdReg(pCh, LMRES_REG);
               if (err == IFX_SUCCESS)
               {
                  pRes->nLMRES   = (IFX_int32_t) LMRES.value;
                  pRes->nSamples = 1;
                  pRes->nGain    = (VIN_CH_LMCR_DC_AD16_GET(LMCR.value)) ? 16 : 1;
                  pRes->nShift   = LMDC.value;
                  pRes->bRevPol  = VIN_CH_BCR2_REVPOL_GET(BCR2.value);
                  pRes->rs       = VINETIC_IO_LT_RS_NONE;
               }
            }
            break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
         case VINETIC_V2x:
            /* do an integrated measurement if not Vdcn-dcp or
            the user requested an non integrated measurement */
            if (mode != VINETIC_IO_LT_VOLTAGE_MODE_DCN_DCP)
            {
               if (pCh->ltConf.nIntegrationTime)
                  err = LT21_IntegratedMeasurement (pCh, pRes, LT21_DC_TIMEOUT);
               else
                  err = LT21_NonIntegratedMeasurement (pCh, pRes);
            }
            else
            {
               /* Simplified V_DCN_DCP measurement,
               result is not measured, i.e. it can be read out. */
               IFXOS_DELAYUS (DCDSP_DELAY_V21);
               err = rdReg(pCh, LMRES_REG);
               if (err == IFX_SUCCESS)
               {
                  pRes->nLMRES   = (IFX_int32_t) LMRES.value;
                  pRes->nSamples = 1;
                  pRes->nGain    = 1;
                  pRes->nShift   = LT21_LM_getShift (pCh);
                  pRes->bRevPol  = VIN21_CH_BCR2_REVPOL_GET(BCR2.value);
                  pRes->rs       = VINETIC_IO_LT_RS_NONE;
               }
            }
            break;
#endif /* VIN_V21_SUPPORT */
            default:
               SET_ERROR(ERR_UNKNOWN_VERSION);
               err = IFX_ERROR;
               break;
         }
   }

   /* Set Offset to precalibrated value */
   if ((err == IFX_SUCCESS) && (pCh->offset.bInitialized))
   {
      OFR.value = pCh->offset.nSlic;
      err = wrReg (pCh, OFR_REG);
   }
   /* set LM-SEL back to IT */
   if (err == IFX_SUCCESS)
   {
      VIN_CH_LMCR_LM_SEL_MODIFY(LMCR.value, LMCR_LM_SEL_I_DC_IT);
      err = wrReg (pCh, LMCR_REG);
   }
   return err;
}

/*******************************************************************************
Description:
   Local function to configure the TGs
Arguments:
   pCh     - pointer to the channel structure
   nTG     - defines which TG shall be configured
             VINETIC_LT_TG_1,
             VINETIC_LT_TG_2
   pCfg    - pointer to the tone generator configuration structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   None.
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT_AC_TG_Config (VINETIC_CHANNEL *pCh, VINETIC_IO_LT_TG nTG,
                                       VINETIC_IO_LT_AC_TG *pCfg)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_void_t     *pTGA = NULL;
   IFX_void_t     *pTGF = NULL;
   IFX_void_t     *pBPF = NULL;
   IFX_int32_t     err  = IFX_SUCCESS;

   /* If IGNORE is set, don't change TG settings ... */
   if (pCfg->state == VIENTIC_IO_LT_AC_TG_STATE_IGNORE)
      return IFX_SUCCESS;

   switch (nTG)
   {
      case VINETIC_IO_LT_TG_1:
         pTGA = TG1A_REG;
         pTGF = TG1F_REG;
         pBPF = BP1F_REG;
         break;
      case VINETIC_IO_LT_TG_2:
         pTGA = TG2A_REG;
         pTGF = TG2F_REG;
         pBPF = BP2F_REG;
         break;
   }

   if (pCfg->freq != 0) /* TG frequency *************** */
   {
      err = setRegVal (pCh, pTGF, pCfg->freq);
      if (err == IFX_SUCCESS)
      {
         err = wrReg (pCh, pTGF);
      }
   }
   if ((err == IFX_SUCCESS) && (pCfg->freq != 0)) /* TG amplitude ********** */
   {
      err = setRegVal (pCh, pTGA, pCfg->level);
      if (err == IFX_SUCCESS)
      {
         err = wrReg (pCh, pTGA);
      }
   }

   switch (pDev->nChipMajorRev)
   {
#ifdef VIN_V14_SUPPORT
      case VINETIC_V1x:
         /* pCfg->freq = getRegVal (pCh, pTGF); */ /* TBD not implemented yet */
         pCfg->level = getRegVal (pCh, pTGA);
         if ((err == IFX_SUCCESS) && (pCfg->freq != 0)) /* BP frequency **** */
         {
            err = setRegVal (pCh, pBPF, pCfg->freq);
            if (err == IFX_SUCCESS)
               err = wrReg (pCh, pBPF);
         }
         if (err == IFX_SUCCESS) /* BP filter quality ********************** */
         {
            switch (nTG)
            {
               case VINETIC_IO_LT_TG_1:
                  VIN_CH_BP12Q_BP1Q_MODIFY(BP12Q.value, pCfg->qual);
                  break;
               case VINETIC_IO_LT_TG_2:
                  VIN_CH_BP12Q_BP2Q_MODIFY(BP12Q.value, pCfg->qual);
                  break;
            }
            err = wrReg (pCh, BP12Q_REG);
         }
         break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
      case VINETIC_V2x:
         /* do nothing */
         break;
#endif /* VIN_V21_SUPPORT */
      default:
         SET_ERROR(ERR_UNKNOWN_VERSION);
         err = IFX_ERROR;
         break;
   }

   if (err == IFX_SUCCESS) /* configure PTG and TG ****************************** */
   {
      switch (nTG)
      {
         case VINETIC_IO_LT_TG_1:
            VIN_CH_DSCR_TG1_EN_SET(DSCR.value, pCfg->state);
            break;
         case VINETIC_IO_LT_TG_2:
            VIN_CH_DSCR_TG2_EN_SET(DSCR.value, pCfg->state);
            break;
      }
      if ( VIN_CH_DSCR_TG1_EN_GET(DSCR.value) ||
           VIN_CH_DSCR_TG2_EN_GET(DSCR.value) )
      {
         /* cut off receive 8kHz = open RX path before TG */
         VIN_CH_DSCR_COR8_SET(DSCR.value, 1);
         VIN_CH_DSCR_PTG_SET (DSCR.value, 1);
      }
      else
      {
         VIN_CH_DSCR_COR8_SET(DSCR.value, 0);
         VIN_CH_DSCR_PTG_SET (DSCR.value, 0);
      }
      err = wrReg (pCh, DSCR_REG);
   }
   return err;
}



/* ============================= */
/* Global function definition    */
/* ============================= */

/*******************************************************************************
Description:
   Measurement of Currents with the DC levelmeter
Arguments:
   pCh     - pointer to the channel structure
   pIO     - pointer to the parameter / results structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   Channel is protected against concurrent tasks
*******************************************************************************/
IFX_int32_t VINETIC_LT_Current (VINETIC_CHANNEL *pCh, VINETIC_IO_LT_CURRENT *pIO)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t     err;
#ifdef VIN_V21_SUPPORT
   LMCR2_LM_ITS    its;
#endif /* VIN_V21_SUPPORT */

   IFXOS_MutexLock (pCh->chAcc);
   err = LT_DC_Store (pCh);
   if (VIN_CH_OPMOD_CUR_MODE_CUR_GET(OPMOD_CUR.value) == OPMOD_MODE_CUR_PDNH)
   {
      IFXOS_MutexUnlock (pCh->chAcc);
      SET_ERROR (ERR_LT_LINE_IS_PDNH);
      return IFX_ERROR;
   }

   if (err == IFX_SUCCESS)
      err = LT_DC_GeneralSettings (pCh);
   if (err == IFX_SUCCESS)
      err = LT_DC_SilentActive (pCh);

   switch (pDev->nChipMajorRev)
   {
#ifdef VIN_V14_SUPPORT
      case VINETIC_V1x:
         if (err == IFX_SUCCESS)
            err = LT14_DC_SilentThreshold (pCh);
         /* Set Integration Time */
         if ((err == IFX_SUCCESS) &&
             (pIO->submode == VINETIC_IO_LT_CURRENT_SUBMODE_DC) &&
             ( VIN_CH_OPMOD_CUR_MODE_CUR_GET(OPMOD_CUR.value) !=
                                                        OPMOD_MODE_CUR_RINGING )
            )
         {
            err = setRegVal (pCh, RGFR_REG, LT_T_DEFAULT_INTEGRATION);
            if (err == IFX_SUCCESS)
               err = wrReg (pCh, RGFR_REG);
         }
         break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
      case VINETIC_V2x:
         if (err == IFX_SUCCESS)
         {
            if ( VIN_CH_OPMOD_CUR_MODE_CUR_GET(OPMOD_CUR.value) !=
                                                        OPMOD_MODE_CUR_RINGING )
               its = LMCR2_LM_ITS_LM_ITIME;
            else
               its = LMCR2_LM_ITS_RING_PERIOD;
            err = LT21_LM_IntegrationCfg (pCh, LMCR2_LM_DCTX8K_2K,
                                          its, ltCfg.tIntCurrent, 0 /* ms */);
            /* store integration time to channel structure for later use */
            pCh->ltConf.nIntegrationTime = ltCfg.tIntCurrent;
         }
         break;
#endif /* VIN_V21_SUPPORT */
      default:
         SET_ERROR(ERR_UNKNOWN_VERSION);
         err = IFX_ERROR;
         break;
   }

   if (err == IFX_SUCCESS)
      err = LT_DC_Current (pCh, pIO->mode, pIO->submode, &pIO->res);

   if (err == IFX_SUCCESS)
   {
      pIO->cfg.nLineMode    = OPMOD_CUR.value;
      pIO->cfg.nHwRevision  = pCh->pParent->nChipRev;
   }

   if (err == IFX_SUCCESS)
      err = LT_DC_Restore (pCh);

   /* release channel lock */
   IFXOS_MutexUnlock (pCh->chAcc);
   return err;
}

/*******************************************************************************
Description:
   Measurement of Voltages with the DC levelmeter
Arguments:
   pCh     - pointer to the channel structure
   pIO     - pointer to the parameter / results structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   channel is protected against concurrent tasks
*******************************************************************************/
IFX_int32_t VINETIC_LT_Voltage (VINETIC_CHANNEL *pCh, VINETIC_IO_LT_VOLTAGE *pIO)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t     err;
#ifdef VIN_V21_SUPPORT
   LMCR2_LM_ITS    its;
#endif /* VIN_V21_SUPPORT */

   /* protect channel against concurrent tasks */
   IFXOS_MutexLock (pCh->chAcc);

   err = LT_DC_Store (pCh);
   if ((err == IFX_SUCCESS) &&
       (VIN_CH_OPMOD_CUR_MODE_CUR_GET(OPMOD_CUR.value) == OPMOD_MODE_CUR_PDNH))
   {
      IFXOS_MutexUnlock (pCh->chAcc);
      SET_ERROR (ERR_LT_LINE_IS_PDNH);
      return IFX_ERROR;
   }

   if (err == IFX_SUCCESS)
      err = LT_DC_GeneralSettings (pCh);
   if (err == IFX_SUCCESS)
      err = LT_DC_SilentActive (pCh);

   switch (pDev->nChipMajorRev)
   {
#ifdef VIN_V14_SUPPORT
      case VINETIC_V1x:
         if (err == IFX_SUCCESS)
            err = LT14_DC_SilentThreshold (pCh);
         /* Modify integration time (ring frequency) only if not ringing
         and if doing a DC or Foreign Voltage measurement */
         if ((err == IFX_SUCCESS) &&
            ( (pIO->submode == VINETIC_IO_LT_VOLTAGE_SUBMODE_DC) ||
            (pIO->bForeignVoltage) ) &&
            (VIN_CH_OPMOD_CUR_MODE_CUR_GET(OPMOD_CUR.value) != OPMOD_MODE_CUR_RINGING)
            )
         {
            err = setRegVal (pCh, RGFR_REG, LT_T_DEFAULT_INTEGRATION);
            if (err == IFX_SUCCESS)
               err = wrReg (pCh, RGFR_REG);
         }
         break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
      case VINETIC_V2x:
         if (err == IFX_SUCCESS)
         {
            if ( VIN_CH_OPMOD_CUR_MODE_CUR_GET(OPMOD_CUR.value) !=
                                                        OPMOD_MODE_CUR_RINGING )
               its = LMCR2_LM_ITS_LM_ITIME;
            else
               its = LMCR2_LM_ITS_RING_PERIOD;

            /* select 8 kHz Level Meter for non integrated measuerements */
            if (ltCfg.tIntVoltage != 0)
            {
               err = LT21_LM_IntegrationCfg (pCh, LMCR2_LM_DCTX8K_2K,
                                             its, ltCfg.tIntVoltage, 0 /* ms */);
            }
            else
            {
               err = LT21_LM_IntegrationCfg (pCh, LMCR2_LM_DCTX8K_8K,
                                             its, ltCfg.tIntVoltage, 0 /* ms */);
            }
            /* store integration time to channel structure for later use */
            pCh->ltConf.nIntegrationTime = ltCfg.tIntVoltage;
         }
         break;
#endif /* VIN_V21_SUPPORT */
      default:
         SET_ERROR(ERR_UNKNOWN_VERSION);
         err = IFX_ERROR;
         break;
   }

   if ((err == IFX_SUCCESS) && (pIO->bForeignVoltage))
   {
      VIN_CH_OPMOD_CUR_SUBMODE_CUR_MODIFY(OPMOD_CUR.value, OPMOD_SUBMOD_CUR_HIRT);
      err = wrReg(pCh, OPMOD_CUR_REG);
      IFXOS_Wait (LT_T_LINEMODE_CHANGE);
   }

   if (err == IFX_SUCCESS)
      err = LT_DC_Voltage (pCh, pIO->mode, pIO->submode, &pIO->res);

   if (err == IFX_SUCCESS)
   {
      pIO->cfg.nLineMode    = OPMOD_CUR.value;
      pIO->cfg.nHwRevision  = pCh->pParent->nChipRev;
   }
   if (err == IFX_SUCCESS)
      err = LT_DC_Restore (pCh);
   /* release channel */
   IFXOS_MutexUnlock (pCh->chAcc);
   return err;
}


/*******************************************************************************
Description:
   Measurement of Resistances
   first configure the measurement point, measure current and voltage
   \param pCh   - pointer to channel structure
   \param rev   - false for normal polarity, true for reverse polarity
   \param vlim  - set vlim for measurement point [V]
   \param ik    - set ik for measurement point [mA]
   \param io    - set which io pin to use for the measurement
   \param pU    - pointer to voltage result structure
   \param pI    - pointer to current result structure
   \return      - IFX_SUCCESS or ERROR in case of a failure
*******************************************************************************/
IFX_LOCAL IFX_int32_t LT_DC_Res_MeasurementPoint (VINETIC_CHANNEL *pCh,
                                                 IFX_boolean_t rev, int vlim,
                                                 int ik,
                                                 VINETIC_IO_LT_VOLTAGE_MODE io,
                                                 VINETIC_IO_LT_RESULT *pU,
                                                 VINETIC_IO_LT_RESULT *pI)
{
   IFX_int32_t err = IFX_SUCCESS;

   /* deactivate test regitster, unset DC-Hold, settle DC characteristic **** */
   if (err == IFX_SUCCESS)
   {
      if (pCh->pParent->nChipMajorRev == VIN_V14_SUPPORT)
         VIN_CH_BCR1_TEST_EN_SET (BCR1.value, 0);
      VIN_CH_BCR1_DC_HOLD_SET (BCR1.value, 0);
      err = wrReg (pCh, BCR1_REG);
   }

   /* configure new measurement point  ************************************** */
   if (err == IFX_SUCCESS)
   {  /* set ik */
      err = setRegVal (pCh, IK1_REG, ik);
      if (err == IFX_SUCCESS)
         err = wrReg (pCh, IK1_REG);
   }
   if (err == IFX_SUCCESS)
   {  /* set Vk1 */
      err = setRegVal (pCh, VK1_REG, vlim);
      if (err == IFX_SUCCESS)
         err = wrReg (pCh, VK1_REG);
   }
   if (err == IFX_SUCCESS)
   {  /* set Vlim */
      err = setRegVal (pCh, VLIM_REG, vlim);
      if (err == IFX_SUCCESS)
         err = wrReg (pCh, VLIM_REG);
   }
   if (err == IFX_SUCCESS)
   {  /* reverse pol. */
      VIN_CH_BCR2_REVPOL_SET(BCR2.value, rev);
      err = wrReg (pCh, BCR2_REG);
   }

   /* set DC-Hold, freeze dc characteristic ********************************* */
   if (err == IFX_SUCCESS)
   {
      /* before freezing, wait until DC characteristic has settled */
      IFXOS_Wait (2*LT_T_LINEMODE_CHANGE);
      VIN_CH_BCR1_DC_HOLD_SET(BCR1.value, 1);
      err = wrReg (pCh, BCR1_REG);
   }
   /* activate Test Regitster settings */
   if ((pCh->pParent->nChipMajorRev == VIN_V14_SUPPORT) && (err == IFX_SUCCESS))
   {
      VIN_CH_BCR1_TEST_EN_SET  (BCR1.value, 1);
      err = wrReg (pCh, BCR1_REG);
   }

   /* execute measurement *************************************************** */
   if (err == IFX_SUCCESS)
   {
      err = LT_DC_Current (pCh, VINETIC_IO_LT_CURRENT_MODE_IT,
                           VINETIC_IO_LT_CURRENT_SUBMODE_DC, pI);
   }
   if (err == IFX_SUCCESS)
   {
      err = LT_DC_Voltage (pCh, io,
                           VINETIC_IO_LT_VOLTAGE_SUBMODE_DC, pU);
   }

   return err;
}


/*******************************************************************************
Description:
   Measurement of Resistances
Arguments:
   pCh     - pointer to the channel structure
   pIO     - pointer to the parameter / results structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   The pIO structure contains a submode telling on which IO pins the
   voltage should be measured - i.e. which pins are TIP/RING, since
   the driver implementation is board independant.

   channel is protected against concurrent tasks
*******************************************************************************/
IFX_int32_t VINETIC_LT_Resistance (VINETIC_CHANNEL *pCh, VINETIC_IO_LT_RESISTANCE *pIO)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t     err;

   /* protect channel against concurrent tasks */
   IFXOS_MutexLock (pCh->chAcc);

   err = LT_DC_Store (pCh);
   if (err == IFX_SUCCESS)
      err = LT_DC_GeneralSettings (pCh);

   /* set linemode ********************************************************** */
   if (err == IFX_SUCCESS)
   {
      VIN_CH_OPMOD_CUR_MODE_CUR_MODIFY(OPMOD_CUR.value, OPMOD_MODE_CUR_ACTIVE);
      switch (pIO->mode)
      {
         case VINETIC_IO_LT_RESISTANCE_MODE_RING_TIP:
            VIN_CH_OPMOD_CUR_SUBMODE_CUR_MODIFY(OPMOD_CUR.value,
                                                OPMOD_SUBMOD_CUR_NORMAL);
            break;
         case VINETIC_IO_LT_RESISTANCE_MODE_RING_GROUND:
            VIN_CH_OPMOD_CUR_SUBMODE_CUR_MODIFY(OPMOD_CUR.value,
                                                OPMOD_SUBMOD_CUR_HIT);
            break;
         case VINETIC_IO_LT_RESISTANCE_MODE_TIP_GROUND:
            VIN_CH_OPMOD_CUR_SUBMODE_CUR_MODIFY(OPMOD_CUR.value,
                                                OPMOD_SUBMOD_CUR_HIR);
            break;
      }
      err = wrReg (pCh, OPMOD_CUR_REG);
   }

   switch (pDev->nChipMajorRev)
   {
#ifdef VIN_V14_SUPPORT
      case VINETIC_V1x:
         if ((err == IFX_SUCCESS) && (pIO->submode == VINETIC_IO_LT_VOLTAGE_MODE_DCN_DCP))
         {
            VIN_CH_BCR1_DC_HOLD_SET(BCR1.value, 1);
            err = wrReg (pCh, BCR1_REG);
            IFXOS_DELAYUS (DCDSP_DELAY);
         }
         if (err == IFX_SUCCESS)
            err = LT14_DC_SilentThreshold (pCh);
         /* Set Integration Time ******************************************** */
         if (err == IFX_SUCCESS)
            err = setRegVal (pCh, RGFR_REG, LT_T_DEFAULT_INTEGRATION);
         if (err == IFX_SUCCESS)
            err = wrReg (pCh, RGFR_REG);
         break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
      case VINETIC_V2x:
         if (err == IFX_SUCCESS)
         {
            /* select 8 kHz Level Meter for short integration periods */
            if (ltCfg.tIntVoltage != 0)
            {
               err = LT21_LM_IntegrationCfg (pCh, LMCR2_LM_DCTX8K_2K,
                                             LMCR2_LM_ITS_LM_ITIME,
                                             ltCfg.tIntResistance, 0 /* ms */);
            }
            else
            {
               err = LT21_LM_IntegrationCfg (pCh, LMCR2_LM_DCTX8K_8K,
                                             LMCR2_LM_ITS_LM_ITIME,
                                             ltCfg.tIntResistance, 0 /* ms */);
            }
            /* store integration time to channel structure for later use */
            pCh->ltConf.nIntegrationTime = ltCfg.tIntResistance;
         }
         break;
#endif /* VIN_V21_SUPPORT */
      default:
         SET_ERROR(ERR_UNKNOWN_VERSION);
         err = IFX_ERROR;
         break;
   }

   /* start measurement sequence */
   switch (pIO->mode)
   {
      case VINETIC_IO_LT_RESISTANCE_MODE_RING_TIP:
         if (pIO->submode == VINETIC_IO_LT_VOLTAGE_MODE_DCN_DCP)
         {
            if (err == IFX_SUCCESS)
            {
               err = LT_DC_Current (pCh, VINETIC_IO_LT_CURRENT_MODE_IT,
                                    VINETIC_IO_LT_CURRENT_SUBMODE_DC, &pIO->res_I1);
            }
            if (err == IFX_SUCCESS)
            {
               err = LT_DC_Voltage (pCh, pIO->submode,
                                    VINETIC_IO_LT_VOLTAGE_SUBMODE_DC, &pIO->res_U1);
            }
         }
         else
         {
            if (err == IFX_SUCCESS)
            {
               err = LT_DC_Res_MeasurementPoint (pCh, IFX_FALSE, 20, 20,
                                                 pIO->submode,
                                                 &pIO->res_U1, &pIO->res_I1);
            }
            if (err == IFX_SUCCESS)
            {
               err = LT_DC_Res_MeasurementPoint (pCh, IFX_TRUE, 20, 20,
                                                 pIO->submode,
                                                 &pIO->res_U2, &pIO->res_I2);
            }
         }
         break;
      case VINETIC_IO_LT_RESISTANCE_MODE_TIP_GROUND:
         if (err == IFX_SUCCESS)
         {
            err = LT_DC_Res_MeasurementPoint (pCh, IFX_TRUE, 0, 10,
                                              pIO->submode,
                                              &pIO->res_U1, &pIO->res_I1);
         }
         if (err == IFX_SUCCESS)
         {
            err = LT_DC_Res_MeasurementPoint (pCh, IFX_TRUE, 40, 10,
                                              pIO->submode,
                                              &pIO->res_U2, &pIO->res_I2);
         }
         break;
      case VINETIC_IO_LT_RESISTANCE_MODE_RING_GROUND:
         if (err == IFX_SUCCESS)
         {
            err = LT_DC_Res_MeasurementPoint (pCh, IFX_FALSE, 0, 10,
                                              pIO->submode,
                                              &pIO->res_U1, &pIO->res_I1);
         }
         if (err == IFX_SUCCESS)
         {
            err = LT_DC_Res_MeasurementPoint (pCh, IFX_FALSE, 40, 10,
                                              pIO->submode,
                                              &pIO->res_U2, &pIO->res_I2);
         }
         break;
    }

   if (err == IFX_SUCCESS)
   {
      pIO->cfg.nLineMode     = OPMOD_CUR.value;
      pIO->cfg.nHwRevision   = pCh->pParent->nChipRev;
   }
   if (err == IFX_SUCCESS)
      err = LT_DC_Restore (pCh);

   /* release tasks lock */
   IFXOS_MutexUnlock (pCh->chAcc);
   return err;
}

#ifdef VIN_V21_SUPPORT
/*******************************************************************************
Description:
   Measurement of Resistances via a RC network
Arguments:
   pCh     - pointer to the channel structure
   pIO     - pointer to the parameter / results structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   The pIO structure contains a submode telling on which IO pins the
   voltage should be measured - i.e. which pins are TIP/RING, since
   the driver implementation is board independant.

   channel is protected against concurrent tasks
*******************************************************************************/
IFX_int32_t VINETIC_LT_Resistance_RC (VINETIC_CHANNEL *pCh,
                                      VINETIC_IO_LT_RESISTANCE_RC *pIO)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t             err;

   /* protect channel against concurrent tasks */
   IFXOS_MutexLock (pCh->chAcc);

   err = LT_DC_Store (pCh);

   if (err == IFX_SUCCESS)
      err = LT_DC_GeneralSettings (pCh);

   /* Switch Rectifier on and set lm select */
   if (err == IFX_SUCCESS)
   {
      VIN21_CH_LMCR_LM_MAG_MODIFY(LMCR.value, LMCR_LM_MAG_RECITFY);
      switch (pIO->iopin)
      {
         case VINETIC_IO_LT_VOLTAGE_MODE_IO2:
            VIN21_CH_LMCR_LM_SEL_MODIFY(LMCR.value, LMCR_LM_SEL_U_IO2);
            break;
         case VINETIC_IO_LT_VOLTAGE_MODE_IO3:
            VIN21_CH_LMCR_LM_SEL_MODIFY(LMCR.value, LMCR_LM_SEL_U_IO3);
            break;
         case VINETIC_IO_LT_VOLTAGE_MODE_IO4:
            VIN21_CH_LMCR_LM_SEL_MODIFY(LMCR.value, LMCR_LM_SEL_U_IO4);
            break;
         case VINETIC_IO_LT_VOLTAGE_MODE_IO4_IO3:
            VIN_CH_LMCR_LM_SEL_MODIFY(LMCR.value, LMCR_LM_SEL_U_IO4_IO3);
            break;
         case VINETIC_IO_LT_VOLTAGE_MODE_IO4_IO2:
            VIN_CH_LMCR_LM_SEL_MODIFY(LMCR.value, LMCR_LM_SEL_U_IO4_IO2);
            break;
         case VINETIC_IO_LT_VOLTAGE_MODE_IO3_IO2:
            VIN_CH_LMCR_LM_SEL_MODIFY(LMCR.value, LMCR_LM_SEL_U_IO3_IO2);
            break;
         default:
            SET_ERROR (ERR_LT_UNKNOWN_PARAM);
            err = IFX_ERROR;
            break;
      }
      if (err == IFX_SUCCESS)
         err = wrReg (pCh, LMCR_REG);
   }
   /* Set Ring Pause Linemode and apply 30 V */
   if (err == IFX_SUCCESS)
   {
      setRegVal(pCh, RO1_REG, 80000);
      err = wrReg (pCh, RO1_REG);
   }
   if (err == IFX_SUCCESS)
   {
      VIN_CH_RTR_RING_OFFSET_MODIFY (RTR.value, LT_RO1);
      err = wrReg (pCh, RTR_REG);
   }
   /* Switch DC Control to 8kHz mode */
   if (err == IFX_SUCCESS)
   {
      VIN21_CH_LMCR2_LM_DCTX8K_SET(LMCR2.value, LMCR2_LM_DCTX8K_8K);
      err = wrReg (pCh, LMCR2_REG);
   }

   /* set linemode ********************************************************** */
   if (err == IFX_SUCCESS)
   {
      VIN_CH_OPMOD_CUR_MODE_CUR_MODIFY    (OPMOD_CUR.value,
                                           OPMOD_MODE_CUR_RING_PAUSE);
      VIN_CH_OPMOD_CUR_SUBMODE_CUR_MODIFY (OPMOD_CUR.value,
                                           OPMOD_SUBMOD_CUR_NORMAL);
      err = wrReg (pCh, OPMOD_CUR_REG);
   }

   /* Set RC low threshold */
   if (err == IFX_SUCCESS)
   {
      LM_RCLOWTHRES.value = 0x4000; /* scale 0.5 */
      err = wrReg (pCh, LM_RCLOWTHRES_REG);
      /* store the scale value for later calculations */
      pIO->reciprocal_scale  = 2;
   }
   /* wait for line to settle */
   IFXOS_Wait (500);

   /* start calculating of thresholds for RC measurement */
   if (err == IFX_SUCCESS)
   {
      VIN21_CH_LMCR_LM_RCMEAS_SET(LMCR.value, 1);
      err = wrReg (pCh, LMCR_REG);
   }

   /* reset LM-OK flag */
   Vinetic_IrqLockDevice(pDev);
   pCh->IsrFlag [EV_SRS1] &= ~SRS1_LM_OK;
   Vinetic_IrqUnlockDevice(pDev);

   /* unmask LM-OK interrupt */
   if (err == IFX_SUCCESS)
   {
      /* unmask LM_OK interrupt for changes 0->1 */
      VIN21_CH_MR_SRS1_M_LM_OK_SET(MR_SRS1.value, 0);
      err = wrReg (pCh, MR_SRS1_REG);
   }

   /* generate start event: switch to HIRT/HIT/HIR mode*/
   if (err == IFX_SUCCESS)
   {
      switch (pIO->mode)
      {
         case VINETIC_IO_LT_RESISTANCE_MODE_RING_TIP:
            VIN_CH_OPMOD_CUR_SUBMODE_CUR_MODIFY (OPMOD_CUR.value,
                                                 OPMOD_SUBMOD_CUR_HIRT);
            break;
         case VINETIC_IO_LT_RESISTANCE_MODE_RING_GROUND:
            VIN_CH_OPMOD_CUR_SUBMODE_CUR_MODIFY (OPMOD_CUR.value,
                                                 OPMOD_SUBMOD_CUR_HIR);
            break;
         case VINETIC_IO_LT_RESISTANCE_MODE_TIP_GROUND:
            VIN_CH_OPMOD_CUR_SUBMODE_CUR_MODIFY (OPMOD_CUR.value,
                                                 OPMOD_SUBMOD_CUR_HIT);
            break;
      }
      err = wrReg (pCh, OPMOD_CUR_REG);
   }

   /* measure the time interval between the two thresholds */
   if (err == IFX_SUCCESS)
   {
      err = LT21_TimeMeasurement (pCh, &pIO->res, LT21_RC_RESISTANCE_TIMEOUT);
      pIO->cfg.nLineMode     = OPMOD_CUR.value;
      pIO->cfg.nHwRevision   = pCh->pParent->nChipRev;
   }

   /* restore the settings in case of success */
   if (err == IFX_SUCCESS)
      err = LT_DC_Restore (pCh);
   /* or if we ran into a timeout - in thiscase report the
      timeout error anyway... */
   else if (pDev->err == ERR_LT_TIMEOUT_LM_OK)
      LT_DC_Restore (pCh);

   /* release lock */
   IFXOS_MutexUnlock (pCh->chAcc);

   return err;
}
#endif /* VIN_V21_SUPPORT */

/*******************************************************************************
Description:
   Measurement of Capacitances
Arguments:
   pCh     - pointer to the channel structure
   pIO     - pointer to the parameter / results structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   channel is protected against concurrent tasks
*******************************************************************************/
IFX_int32_t VINETIC_LT_Capacitance (VINETIC_CHANNEL *pCh,
                            VINETIC_IO_LT_CAPACITANCE *pIO)
{
   VINETIC_DEVICE *pDev          = pCh->pParent;
   IFX_boolean_t   bSettlingTime = IFX_FALSE;
   IFX_boolean_t   bReducedSlope = IFX_FALSE;
   IFX_int32_t     t1, t2;
   IFX_int32_t     err;

   /* protect channel against concurrent tasks */
   IFXOS_MutexLock (pCh->chAcc);

   err = LT_DC_Store (pCh);

   if (err == IFX_SUCCESS)
      err = LT_DC_GeneralSettings (pCh);

   /* set linemode ********************************************************** */
   if (err == IFX_SUCCESS)
   {
      if (VIN_CH_OPMOD_CUR_MODE_CUR_GET(OPMOD_CUR.value) !=
          OPMOD_MODE_CUR_RING_PAUSE)
      {
         VIN_CH_OPMOD_CUR_MODE_CUR_MODIFY(OPMOD_CUR.value,
                                          OPMOD_MODE_CUR_RING_PAUSE);
         bSettlingTime = IFX_TRUE;
      }
      switch (pIO->mode)
      {
         case VINETIC_IO_LT_CAPACITANCE_MODE_RING_TIP:
            if (VIN_CH_OPMOD_CUR_SUBMODE_CUR_GET(OPMOD_CUR.value) !=
                                                 OPMOD_SUBMOD_CUR_NORMAL)
            {
               VIN_CH_OPMOD_CUR_SUBMODE_CUR_MODIFY(OPMOD_CUR.value,
                                                   OPMOD_SUBMOD_CUR_NORMAL);
               bSettlingTime = IFX_TRUE;
            }
            break;
         case VINETIC_IO_LT_CAPACITANCE_MODE_RING_GROUND:
            if (VIN_CH_OPMOD_CUR_SUBMODE_CUR_GET(OPMOD_CUR.value) !=
                                                 OPMOD_SUBMOD_CUR_HIT)
            {
               VIN_CH_OPMOD_CUR_SUBMODE_CUR_MODIFY(OPMOD_CUR.value,
                                                   OPMOD_SUBMOD_CUR_HIT);
               bSettlingTime = IFX_TRUE;
            }
            break;
         case VINETIC_IO_LT_CAPACITANCE_MODE_TIP_GROUND:
            if (VIN_CH_OPMOD_CUR_SUBMODE_CUR_GET(OPMOD_CUR.value) !=
                                                 OPMOD_SUBMOD_CUR_HIR)
            {
               VIN_CH_OPMOD_CUR_SUBMODE_CUR_MODIFY(OPMOD_CUR.value,
                                                   OPMOD_SUBMOD_CUR_HIR);
               bSettlingTime = IFX_TRUE;
            }
            break;
      }
      err = wrReg (pCh, OPMOD_CUR_REG);
   }
   if (bSettlingTime)
      IFXOS_Wait (LT_T_LINEMODE_CHANGE);

   /* configure Ring Offsets */
   if (err == IFX_SUCCESS)
      err = setRegVal (pCh, RO1_REG, VINETIC_CAPACITANCE_START_VOLTAGE);
   if (err == IFX_SUCCESS)
   {
      switch (VIN_CH_BCR1_SEL_SLIC_GET(BCR1.value))
      {
          /* slic LCP/LCP with current limitation; adapted to gain */
          case VINETIC_SLIC_TYPE_LCP:
          case VINETIC_SLIC_TYPE_LCP_CURRENT_LIMITED:
             RO1.value *= 2;
             break;
      }
      err = wrReg (pCh, RO1_REG);
   }
   if (err == IFX_SUCCESS)
      err = setRegVal (pCh, RO2_REG, VINETIC_CAPACITANCE_STOP_VOLTAGE);
   if (err == IFX_SUCCESS)
   {
      switch (VIN_CH_BCR1_SEL_SLIC_GET(BCR1.value))
      {
          /* slic LCP/LCP with current limitation; adapted to gain */
          case VINETIC_SLIC_TYPE_LCP:
          case VINETIC_SLIC_TYPE_LCP_CURRENT_LIMITED:
             RO2.value *= 2;
             break;
      }
      err = wrReg (pCh, RO2_REG);
   }

   if (err == IFX_SUCCESS)
   {
      /* Select IT current for level meter */
      VIN_CH_LMCR_LM_SEL_MODIFY (LMCR.value, LMCR_LM_SEL_I_DC_IT);
      err = wrReg (pCh, LMCR_REG);
      IFXOS_DELAYUS (DCDSP_DELAY);
   }
   if (err == IFX_SUCCESS)
   {
      /* select RO1 */
      VIN_CH_RTR_RING_OFFSET_MODIFY (RTR.value, LT_RO1);
      /* ensure to external ringing is disabled */
      VIN_CH_RTR_REXT_EN_SET (RTR.value, 0);
      err = wrReg (pCh, RTR_REG);
   }
   if (err == IFX_SUCCESS)
   {
      /* unmask ramp ready interrupt */
      VIN_CH_MR_SRS1_M_RAMP_READY_SET (MR_SRS1.value, 0);
      err = wrReg (pCh, MR_SRS1_REG);
   }

   switch (pDev->nChipMajorRev)
   {
#ifdef VIN_V14_SUPPORT
      case VINETIC_V1x:
         if (err == IFX_SUCCESS)
         {  /* enable the ramp generator */
            VIN_CH_LMCR_RAMP_EN_SET(LMCR.value, 1);
            err = wrReg (pCh, LMCR_REG);
         }
         /* wait for Ramp to complete */
         if ( IFXOS_WaitEvent_timeout (pCh->wqLT_RAMP_RDY, LT_RAMP_TIMEOUT) == IFX_ERROR )
         {
            TRACE(VINETIC, DBG_LEVEL_HIGH, ("ERROR timeout waiting for RAMP_RDY\n"));
            SET_ERROR (ERR_LT_TIMEOUT_LM_RAMP_RDY);
            err = IFX_ERROR;
         }
         break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
      case VINETIC_V2x:
         if (err == IFX_SUCCESS)
         {  /* shorten transients with AC input impedance
               of the AC-Prefi filter */
            VIN21_CH_BCR2_AC_SHORT_EN_SET(BCR2.value, 1);
            err = wrReg (pCh, BCR2_REG);
         }
         break;
#endif /* VIN_V21_SUPPORT */
      default:
         SET_ERROR(ERR_UNKNOWN_VERSION);
         err = IFX_ERROR;
         break;
   }

   if (err == IFX_SUCCESS)
   {
      err = LT_DC_Cap_InterleavedCurrent (pCh, pIO,
                                          VINETIC_LT_CAPACITANCE_SLOPE_STEEP);
   }

   switch (pDev->nChipMajorRev)
   {
#ifdef VIN_V14_SUPPORT
      case VINETIC_V1x:
         t1 = (pIO->res[0].nLMRES * CONST_2_14) /
            ( pIO->res[0].nGain * pIO->res[0].nShift * pIO->res[0].nSamples);
         t2 = (pIO->res[1].nLMRES * CONST_2_14) /
            ( pIO->res[1].nGain * pIO->res[1].nShift * pIO->res[1].nSamples);

#ifdef DEBUG
         TRACE(VINETIC, DBG_LEVEL_LOW, ("t1 = %d, t2 = %d    t1+t2 = %d\n",
               abs(t1), abs(t2), abs(t1)+abs(t2)));
#endif /* DEBUG */
         if ((abs(t1)+abs(t2)) > 1500)
            bReducedSlope = IFX_TRUE;
         break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
      case VINETIC_V2x:
         t1 = (pIO->res[0].nLMRES) /
            (pIO->res[0].nShift * pIO->res[0].nSamples);
         t2 = (pIO->res[1].nLMRES) /
            (pIO->res[1].nShift * pIO->res[1].nSamples);
#ifdef DEBUG
         TRACE(VINETIC, DBG_LEVEL_LOW, ("t1 = %d, t2 = %d    t1+t2 = %d\n",
            abs(t1), abs(t2), abs(t1)+abs(t2)));
#endif /* DEBUG */
         if ((abs(t1)+abs(t2)) > 330000)
            bReducedSlope = IFX_TRUE;
         break;
#endif /* VIN_V21_SUPPORT */
      default:
         SET_ERROR(ERR_UNKNOWN_VERSION);
         err = IFX_ERROR;
         break;
   }

   if ((err == IFX_SUCCESS) && (bReducedSlope))
   {  /* measure again with another slope */
      err = LT_DC_Cap_InterleavedCurrent (pCh, pIO,
                                          VINETIC_LT_CAPACITANCE_SLOPE_FLAT);
   }

   if (err == IFX_SUCCESS)
   {
      pIO->cfg.nLineMode     = OPMOD_CUR.value;
      pIO->cfg.nHwRevision   = pCh->pParent->nChipRev;
   }

   if (err == IFX_SUCCESS)
      err = LT_DC_Restore (pCh);

   /* release lock */
   IFXOS_MutexUnlock (pCh->chAcc);

   return err;
}

/*******************************************************************************
Description:
   Measurement of Impedances
Arguments:
   pCh     - pointer to the channel structure
   pIO     - pointer to the parameter / results structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   channel is protected against concurrent tasks
*******************************************************************************/
IFX_int32_t VINETIC_LT_Impedance (VINETIC_CHANNEL *pCh, VINETIC_IO_LT_IMPEDANCE *pIO)
{
#ifdef VIN_V21_SUPPORT
   VINETIC_DEVICE *pDev = pCh->pParent;
#endif /* VIN_V21_SUPPORT */
   IFX_int32_t     err;

   /* protect channel against concurrent tasks */
   IFXOS_MutexLock (pCh->chAcc);

   err = LT_DC_Store (pCh);

   if (err == IFX_SUCCESS)
      err = LT_DC_GeneralSettings (pCh);
   if (err == IFX_SUCCESS)
      err = setRegVal (pCh, RGFR_REG, pIO->freq * 1000);
   if (err == IFX_SUCCESS)
      err = wrReg (pCh, RGFR_REG);
   if (err == IFX_SUCCESS)
      err = setRegVal (pCh, V1_REG, pIO->level);
   if (err == IFX_SUCCESS)
      err = wrReg (pCh, V1_REG);

   /* set linemode ********************************************************** */
   if (err == IFX_SUCCESS)
   {
      VIN_CH_OPMOD_CUR_MODE_CUR_MODIFY(OPMOD_CUR.value, OPMOD_MODE_CUR_RINGING);
      switch (pIO->mode)
      {
         case VINETIC_IO_LT_IMPEDANCE_MODE_RING_TIP:
            VIN_CH_OPMOD_CUR_SUBMODE_CUR_MODIFY(OPMOD_CUR.value,
                                                OPMOD_SUBMOD_CUR_NORMAL);
            break;
         case VINETIC_IO_LT_IMPEDANCE_MODE_RING_GROUND:
            VIN_CH_OPMOD_CUR_SUBMODE_CUR_MODIFY(OPMOD_CUR.value,
                                                OPMOD_SUBMOD_CUR_HIT);
            break;
         case VINETIC_IO_LT_IMPEDANCE_MODE_TIP_GROUND:
            VIN_CH_OPMOD_CUR_SUBMODE_CUR_MODIFY(OPMOD_CUR.value,
                                                OPMOD_SUBMOD_CUR_HIR);
            break;
      }
      err = wrReg (pCh, OPMOD_CUR_REG);
   }

#ifdef VIN_V21_SUPPORT
   if (err == IFX_SUCCESS)
   {  /* set integration time / source */
      switch (pCh->pParent->nChipMajorRev)
      {
         case VINETIC_V1x:
            /* return quantized ring amplitude */
            pIO->level = getRegVal (pCh, V1_REG);
            /* check for error */
            if (pIO->level == IFX_ERROR)
               err = IFX_ERROR;
            break;
         case VINETIC_V2x:
            err = LT21_LM_IntegrationCfg (pCh, LMCR2_LM_DCTX8K_2K,
                                          LMCR2_LM_ITS_RING_PERIOD,
                                          0 /* ms */, 0 /* ms */);
            /* store integration time to channel structure for later use */
            pCh->ltConf.nIntegrationTime = ltCfg.tIntCurrent;
            break;
         default:
            SET_ERROR(ERR_UNKNOWN_VERSION);
            err = IFX_ERROR;
            break;
      }
   }
#endif /* VIN_V21_SUPPORT */

   if (err == IFX_SUCCESS)
   {
      err = LT_DC_Current (pCh, VINETIC_IO_LT_CURRENT_MODE_IT,
                           VINETIC_IO_LT_CURRENT_SUBMODE_AC, &pIO->res);
   }

   if (err == IFX_SUCCESS)
   {
      pIO->cfg.nLineMode     = OPMOD_CUR.value;
      pIO->cfg.nHwRevision   = pCh->pParent->nChipRev;
   }

   if (err == IFX_SUCCESS)
      err = LT_DC_Restore (pCh);

   /* release lock */
   IFXOS_MutexUnlock (pCh->chAcc);

   return err;
}

/*******************************************************************************
Description:
   AC Measurements
   Level Measurement, Transhybrid Measurement, SNR Measurement, Gain Tracking
   and Idle Noise Measurement
Arguments:
   pCh     - pointer to the channel structure
   pIO     - pointer to the parameter / results structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   channel is protected against concurrent tasks
*******************************************************************************/
IFX_int32_t VINETIC_LT_AC_Measurements (VINETIC_CHANNEL *pCh,
                                        VINETIC_IO_LT_AC_MEASUREMENTS *pIO)
{
   VINETIC_DEVICE     *pDev = pCh->pParent;
   VINETIC_LT_AC       par;
   IFX_int32_t         err;

   /* protect channel against concurrent tasks */
   IFXOS_MutexLock (pCh->chAcc);

   err = LT_AC_Store (pCh);

   if (err == IFX_SUCCESS)
      err = LT_AC_GeneralSettings (pCh);

   par.mode  = (VINETIC_LT_AC_MODE) pIO->mode;
   par.p.pAC = pIO;

   switch (pDev->nChipMajorRev)
   {
#ifdef VIN_V14_SUPPORT
      case VINETIC_V1x:
         if (err == IFX_SUCCESS)
            err = LT14_AC_Level (pCh, &par, &pIO->res);
         if ((err == IFX_SUCCESS) && (pIO->mode == VINETIC_IO_LT_AC_MODE_SNR))
         {
            par.mode = VINETIC_LT_AC_MODE_SNR_NOISE;
            err = LT14_AC_Level (pCh, &par, &pIO->res_noise);
         }
         break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
      case VINETIC_V2x:
         if (err == IFX_SUCCESS)
            err = LT21_AC_Level (pCh, &par, &pIO->res);
         if ((err == IFX_SUCCESS) && (pIO->mode == VINETIC_IO_LT_AC_MODE_SNR))
         {
            par.mode = VINETIC_LT_AC_MODE_SNR_NOISE;
            err = LT21_AC_Level (pCh, &par, &pIO->res_noise);
         }
         break;
#endif /* VIN_V21_SUPPORT */
      default:
         SET_ERROR(ERR_UNKNOWN_VERSION);
         err = IFX_ERROR;
         break;
   }

   if (err == IFX_SUCCESS)
   {
      pIO->cfg.nLineMode    = OPMOD_CUR.value;
      pIO->cfg.nHwRevision  = pCh->pParent->nChipRev;
   }

   if (err == IFX_SUCCESS)
      err = LT_AC_Restore (pCh);

   /* release lock */
   IFXOS_MutexUnlock (pCh->chAcc);

   return err;
}

/*******************************************************************************
Description:
   Fully configurable AC Measurement
   (for evaluation and all purposes not covered with VINETIC_LT_AC)
Arguments:
   pCh     - pointer to the channel structure
   pIO     - pointer to the parameter / results structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   channel is protected against concurrent tasks
*******************************************************************************/
IFX_int32_t VINETIC_LT_AC_Direct (VINETIC_CHANNEL *pCh, VINETIC_IO_LT_AC_DIRECT *pIO)
{
   VINETIC_DEVICE     *pDev = pCh->pParent;
   VINETIC_LT_AC       par;
   IFX_int32_t         err;

   /* protect channel against concurrent tasks */
   IFXOS_MutexLock (pCh->chAcc);

   err = LT_AC_Store (pCh);

   if (err == IFX_SUCCESS)
      err = LT_AC_GeneralSettings (pCh);

   par.mode = VINETIC_LT_AC_MODE_DIRECT;
   par.p.pAC_direct = pIO;

   switch (pDev->nChipMajorRev)
   {
#ifdef VIN_V14_SUPPORT
      case VINETIC_V1x:
         if (err == IFX_SUCCESS)
            err = LT14_AC_Level (pCh, &par, &pIO->res);
         break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
      case VINETIC_V2x:
         if (err == IFX_SUCCESS)
            err = LT21_AC_Level (pCh, &par, &pIO->res);
         break;
#endif /* VIN_V21_SUPPORT */
      default:
         SET_ERROR(ERR_UNKNOWN_VERSION);
         err = IFX_ERROR;
         break;
   }

   if (err == IFX_SUCCESS)
   {
      pIO->cfg.nLineMode    = OPMOD_CUR.value;
      pIO->cfg.nHwRevision  = pCh->pParent->nChipRev;
   }

   if (err == IFX_SUCCESS)
      err = LT_AC_Restore (pCh);

   /* release lock */
   IFXOS_MutexUnlock (pCh->chAcc);

   return err;
}

/*******************************************************************************
Description:
   Init Linetsting
Arguments:
   None
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   None
*******************************************************************************/
IFX_int32_t VINETIC_LT_Init (IFX_void_t)
{
   memset (&ltCfg, 0, sizeof(VINETIC_LT_CONFIG));
   ltCfg.tIntCurrent    = 100;
   ltCfg.tIntVoltage    = 100;
   ltCfg.tIntResistance = 100;
   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   Configure Linetsting
Arguments:
   pCh     - pointer to the channel structure
   pIO     - pointer to the configuration structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   None
*******************************************************************************/
IFX_int32_t VINETIC_LT_Config (VINETIC_IO_LT_CONFIG *pIO)
{
   IFX_int32_t ret = IFX_SUCCESS;

#ifdef VIN_V21_SUPPORT
   /* check if desired integration time is in range with the defined timeouts */
   if (ltCfg.tIntCurrent > LT21_MAX_INTEGRATION_TIME)
   {
      ret = IFX_ERROR;
   }
   if (ltCfg.tIntVoltage > LT21_MAX_INTEGRATION_TIME)
   {
      ret = IFX_ERROR;
   }
   if (ltCfg.tIntResistance > LT21_MAX_INTEGRATION_TIME)
   {
      ret = IFX_ERROR;
   }
#endif /* VIN_V21_SUPPORT */

   /* configure the integration times */
   if (ret == IFX_SUCCESS)
   {
      switch (pIO->access)
      {
         case VINETIC_IO_LT_CONFIG_READ:
            pIO->nCurrentIntegrationTime    = ltCfg.tIntCurrent;
            pIO->nVoltageIntegrationTime    = ltCfg.tIntVoltage;
            pIO->nResistanceIntegrationTime = ltCfg.tIntResistance;
            break;
         case VINETIC_IO_LT_CONFIG_WRITE:
            ltCfg.tIntCurrent               = pIO->nCurrentIntegrationTime;
            ltCfg.tIntVoltage               = pIO->nVoltageIntegrationTime;
            ltCfg.tIntResistance            = pIO->nResistanceIntegrationTime;
            break;
      }
   }
   return ret;
}
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_LT) */
#endif /* TAPI */


