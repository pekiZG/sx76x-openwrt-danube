#ifndef _DRV_VINETIC_CRAM_H
#define _DRV_VINETIC_CRAM_H
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
   Module      : drv_vinetic_cram.h
   Date        : 2003-10-27
   Description : This file contains the declaration of the functions
                 for CRAM operations
*******************************************************************************/

/* ============================= */
/* Global Defines                */
/* ============================= */

/* enum for CRAM Table types */
enum
{
   VINETIC_TG1_FREQ = 0,
   VINETIC_TG2_FREQ,
   VINETIC_TG1_AMPL,
   VINETIC_TG2_AMPL,
   /* add list here ... */
   MAX_CRAM_TAB_SETTINGS = 4
};

/* ============================= */
/* Global Structures             */
/* ============================= */

/* CRAM Coefficients settings info */
typedef struct
{
   /* CRAM coefficients address */
   IFX_uint16_t regAddr;
   /* offset of used nibble in CRAM coefficient */
   IFX_uint8_t  nibOffset;
   /* length of used nibbles in CRAM coeff */
   IFX_uint8_t  nibLength;
   /* Mask for CRAM Coefs */
   IFX_uint32_t CRAMCoefMask;
} VINETIC_CRAM_SETTINGS_TAB;

/* structure for CRAM settings
  (description of CRAM and
  quantized value table)
*/
typedef struct
{
   /* Quantum unit */
   IFX_int32_t    quantVal;
   /* CRAM coeff */
   IFX_int32_t    CRAMcoeff;
} COEFF_TAB_ENTRY;

/* Structure used for CRAM settings
   to avoid floating points operations in driver.
   Compute the floating point operation in application and
   give appropriate value to levelVal
*/
typedef struct
{
   /* level after fp computation
      in task */
   IFX_int32_t    levelVal;
   /* Quantum unit */
   IFX_int32_t    quantVal;
   /* CRAM coeff */
   IFX_int32_t    CRAMcoeff;
} COEFF_TAB_VAL_ENTRY;

/* Stucture used for Vinetic V2.1 filter coefficients */
typedef struct
{
   IFX_int16_t      freq;
   IFX_uint16_t     filt1;
   IFX_uint16_t     filt2;
   IFX_uint16_t     filt3;
   IFX_uint16_t     filt4;
   IFX_uint16_t     filt5;
} VIN21_FILT_ENTRY;


/* ============================= */
/* Global variable declaration   */
/* ============================= */

/* CRAM Table for Tone generator Frequencies */
extern const COEFF_TAB_ENTRY    VINETIC_CRAM_TGFreqQuantVal_Table[];
/* CRAM Table for Tone generator Amplitudes */
extern const COEFF_TAB_ENTRY    VINETIC_CRAM_TGAmpQuantVal_Table[];
/* CRAM table for Ring Frequency */
extern const  COEFF_TAB_ENTRY   VINETIC_CRAM_RingFreqQuantVal_Table [];
/* CRAM table for Ring Low Pass Frequency */
extern const  COEFF_TAB_ENTRY   VINETIC_CRAM_RingLpFreqQuantVal_Table [];
/* CRAM table for Ring DC Offset */
extern const  COEFF_TAB_ENTRY   VINETIC_CRAM_RingDcQuantVal_Table_13 [];
extern const  COEFF_TAB_ENTRY   VINETIC_CRAM_RingDcQuantVal_Table_14 [];
/* CRAM table for Ring Delay */
extern const  COEFF_TAB_ENTRY   VINETIC_CRAM_RingDelayQuantVal_Table [];
/* CRAM table for ramp Slope */
extern const  COEFF_TAB_ENTRY   VINETIC_CRAM_RampSlopeQuantVal_Table_13 [];
extern const  COEFF_TAB_ENTRY   VINETIC_CRAM_RampSlopeQuantVal_Table_14 [];
/* CRAM table for IK1 */
extern const  COEFF_TAB_ENTRY   VINETIC_CRAM_IK1QuantVal_Table_13 [];
extern const  COEFF_TAB_ENTRY   VINETIC_CRAM_IK1QuantVal_Table_14 [];
/* Ring Amplitude (Vrms) for 20 HZ frequency */
extern const  COEFF_TAB_ENTRY   VINETIC_CRAM_RingAmpQuantVal_Table_13 [];
extern const  COEFF_TAB_ENTRY   VINETIC_CRAM_RingAmpQuantVal_Table_14 [];
#ifdef VIN_V21_SUPPORT
#if (VIN_CFG_FEATURES & VIN_FEAT_LT)
extern const  VIN21_FILT_ENTRY  VINETIC_V21_Filter_Table [];
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_LT) */
#endif /* VIN_V21_SUPPORT */

/* array of partial values used to approach the function 10^X */
extern const IFX_uint32_t  tens  [28][2];

/* ============================= */
/* Global function declaration   */
/* ============================= */

/* global functions for reading  CRAM hex/quantum
   values from CRAM tables */
/* ring frequency */
extern IFX_uint16_t CRAM_getRingFreqHex      (VINETIC_DEVICE *pDev, IFX_int32_t nFreqHz);
extern IFX_uint16_t CRAM_getRingLpFreqHex    (VINETIC_DEVICE *pDev, IFX_int32_t nFreqHz);
extern IFX_int32_t  CRAM_getRingFreqVal      (VINETIC_DEVICE *pDev, IFX_uint16_t nFreqHex);
/* ring offset */
extern IFX_uint16_t CRAM_getRingDcHex        (VINETIC_DEVICE *pDev, IFX_int32_t nRingDc);
extern IFX_int32_t  CRAM_getRingDcVal        (VINETIC_DEVICE *pDev, IFX_uint16_t nRingDcHex);
/* ring delay */
extern IFX_uint16_t CRAM_getRingDelayHex     (VINETIC_DEVICE *pDev, IFX_int32_t nDelay);
extern IFX_int32_t  CRAM_getRingDelayVal     (VINETIC_DEVICE *pDev, IFX_uint16_t nRingDelayHex);
/* ramp slope */
extern IFX_uint16_t CRAM_getRampSlopeHex     (VINETIC_DEVICE *pDev, IFX_int32_t nRamSlop);
extern IFX_int32_t  CRAM_getRampSlopeVal     (VINETIC_DEVICE *pDev, IFX_uint16_t nRampSlopeHex);
/* voltage Vlim (for resistance measurement) */
extern IFX_uint16_t CRAM_getVlimHex          (VINETIC_DEVICE *pDev, IFX_int32_t nVlim);
extern IFX_int32_t  CRAM_getVlimVal          (VINETIC_DEVICE *pDev, IFX_uint16_t nVlimHex);
/* voltage Vk1 (for resistance measurement) */
extern IFX_uint16_t CRAM_getVk1Hex           (VINETIC_DEVICE *pDev, IFX_int32_t nVk1);
extern IFX_int32_t  CRAM_getVk1Val           (VINETIC_DEVICE *pDev, IFX_uint16_t nVk1Hex);
/* active/ active boost mode current */
extern IFX_uint16_t CRAM_getIk1Hex           (VINETIC_DEVICE *pDev, IFX_int32_t nIk1);
extern IFX_int32_t  CRAM_getIk1Val           (VINETIC_DEVICE *pDev, IFX_uint16_t nIk1Hex);
/* tone generator frequencies */
extern IFX_uint16_t CRAM_getTGFreqHex        (VINETIC_DEVICE *pDev, IFX_int32_t nTGFreq);
extern IFX_int32_t  CRAM_getTGFreqVal        (VINETIC_DEVICE *pDev, IFX_uint16_t nTGFreqHex);
/* band pass frequenies */
extern IFX_uint16_t CRAM_getBPFreqHex        (VINETIC_DEVICE *pDev, IFX_int32_t nBPFreq);
/* tone generators level */
extern IFX_uint16_t CRAM_getTGAmpHex         (VINETIC_DEVICE *pDev, IFX_int32_t nTGLevel);
extern IFX_int32_t  CRAM_getTGAmpVal         (VINETIC_DEVICE *pDev, IFX_uint16_t nTGLevelHex);
/* ring amplitude */
extern IFX_uint16_t CRAM_getRingAmpHex       (VINETIC_DEVICE *pDev, IFX_int32_t nRingAC);
extern IFX_int32_t  CRAM_getRingAmpVal       (VINETIC_DEVICE *pDev, IFX_uint16_t nRingACHex);
/* CRAM table management functions */
extern IFX_int32_t  Cram_rwbyUnit            (VINETIC_DEVICE *pDev, IFX_uint8_t nCh, IFX_uint8_t nMode,
                                              IFX_uint8_t nCoeffType, COEFF_TAB_ENTRY *pEntry);
extern IFX_int32_t  Cram_rwbyVal             (VINETIC_DEVICE *pDev, IFX_uint8_t nCh, IFX_uint8_t nMode,
                                              IFX_uint8_t nCoeffType, COEFF_TAB_VAL_ENTRY *pEntry);
extern IFX_int32_t  Cram_getUnitCoef         (IFX_uint8_t nMode, COEFF_TAB_ENTRY *pEntry,
                                              COEFF_TAB_ENTRY *pUnit);

#ifdef VIN_V21_SUPPORT
IFX_int16_t Vin21_Calc_Ring_Offset_Set (IFX_int32_t ro);
IFX_int16_t Vin21_Calc_Ring_Freq_Set   (IFX_int32_t rf);
IFX_int32_t Vin21_Calc_Ring_Freq_Get   (IFX_int16_t rf);
IFX_int16_t Vin21_Calc_TG_Freq_Set     (IFX_int32_t tf);
IFX_int16_t Vin21_Calc_TG_Gain_Set     (IFX_int32_t rg);
IFX_int16_t Vin21_Calc_RampSlope_Set   (IFX_int32_t rs);
IFX_int16_t Vin21_Calc_Ik1_Set         (IFX_int32_t ik);
IFX_int16_t Vin21_Calc_RingAmp_Set     (IFX_int32_t ringamp);
#endif /* VIN_V21_SUPPORT */

#endif /* _DRV_VINETIC_CRAM_H */
