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
IFX_LOCAL IFX_int32_t Dsp_SetToneDetectors (VINETIC_CHANNEL *pCh,
                                            IFX_boolean_t bEn,
                                            SIG_MOD_STATE* pState);
#endif /* TAPI */
IFX_LOCAL IFX_int32_t Atd_SetCoeff         (VINETIC_DEVICE *pDev,
                                            IFX_uint16_t nRgap,
                                            IFX_uint8_t nPhaseRep,
                                            IFX_uint16_t nLevel,
                                            IFX_uint8_t nAtd);
IFX_LOCAL IFX_int32_t Dsp_MFTD_SigSet      (VINETIC_CHANNEL *pCh,
                                            IFX_uint32_t nSignal,
                                            IFX_uint32_t nSignalExt);

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* val,0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150,155,160,165,170,175,180,185,190,195,200,205,210,215,220,225,230,235,240,245,250,255,260,265,270,275,280,285,290,295,300,305,310,315,320,325,330,335,340,345,350,355,360,365,370,375,380,385,390,395,400,405,410,415,420,425,430,435,440,445,450,455,460,465,470,475,480,485,490,495,500,505,510,515,520,525,530,535,540,545,550,555,560,565,570,575,580,585,590,595,600,605,610,615,620,625,630,635,640,645,650,655,660,665,670,675,680,685,690,695,700,705,710,715,720,725,730,735,740,745,750,755,760,765,770,775,780,785,790,795,800,805,810,815,820,825,830,835,840,845,850,855,860,865,870,875,880,885,890,895,900,905,910,915,920,925,930,935,940,945,950,955,960 */
/* dB,0,-0,5,-1,-1,5,-2,-2,5,-3,-3,5,-4,-4,5,-5,-5,5,-6,-6,5,-7,-7,5,-8,-8,5,-9,-9,5,-10,-10,5,-11,-11,5,-12,-12,5,-13,-13,5,-14,-14,5,-15,-15,5,-16,-16,5,-17,-17,5,-18,-18,5,-19,-19,5,-20,-20,5,-21,-21,5,-22,-22,5,-23,-23,5,-24,-24,5,-25,-25,5,-26,-26,5,-27,-27,5,-28,-28,5,-29,-29,5,-30,-30,5,-31,-31,5,-32,-32,5,-33,-33,5,-34,-34,5,-35,-35,5,-36,-36,5,-37,-37,5,-38,-38,5,-39,-39,5,-40,-40,5,-41,-41,5,-42,-42,5,-43,-43,5,-44,-44,5,-45,-45,5,-46,-46,5,-47,-47,5,-48,-48,5,-49,-49,5,-50,-50,5,-51,-51,5,-52,-52,5,-53,-53,5,-54,-54,5,-55,-55,5,-56,-56,5,-57,-57,5,-58,-58,5,-59,-59,5,-60,-60,5,-61,-61,5,-62,-62,5,-63,-63,5,-64,-64,5,-65,-65,5,-66,-66,5,-67,-67,5,-68,-68,5,-69,-69,5,-70,-70,5,-71,-71,5,-72,-72,5,-73,-73,5,-74,-74,5,-75,-75,5,-76,-76,5,-77,-77,5,-78,-78,5,-79,-79,5,-80,-80,5,-81,-81,5,-82,-82,5,-83,-83,5,-84,-84,5,-85,-85,5,-86,-86,5,-87,-87,5,-88,-88,5,-89,-89,5,-90,-90,5,-91,-91,5,-92,-92,5,-93,-93,5,-94,-94,5,-95,-95,5,-96 */
/* rx Val, 32768,30935,29205,27571,26029,24573,23198,21900,20675,19519,18427,17396,16423,15504,14637,13818,13045,12315,11627,10976,10362,9783,9235,8719,8231,7771,7336,6925,6538,6172,5827,5501,5193,4903,4629,4370,4125,3894,3677,3471,3277,3093,2920,2757,2603,2457,2320,2190,2068,1952,1843,1740,1642,1550,1464,1382,1305,1232,1163,1098,1036,978,924,872,823,777,734,693,654,617,583,550,519,490,463,437,413,389,368,347,328,309,292,276,260,246,232,219,207,195,184,174,164,155,146,138,130,123,116,110,104,98,92,87,82,78,73,69,65,62,58,55,52,49,46,44,41,39,37,35,33,31,29,28,26,25,23,22,21,20,18,17,16,16,15,14,13,12,12,11,10,10,9,9,8,8,7,7,7,6,6,6,5,5,5,4,4,4,4,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 */
static const IFX_uint16_t cid_rx_levels[] = {0x8000, 0x78D6, 0x7214, 0x6BB2, 0x65AC, 0x5FFC, 0x5A9D, 0x558C, 0x50C3, 0x4C3E, 0x47FA, 0x43F4, 0x4026, 0x3C90, 0x392C, 0x35FA, 0x32F5, 0x301B, 0x2D6A, 0x2AE0, 0x287A, 0x2636, 0x2413, 0x220E, 0x2026, 0x1E5A, 0x1CA7, 0x1B0D, 0x198A, 0x181C, 0x16C3, 0x157D, 0x1449, 0x1326, 0x1214, 0x1111, 0x101D, 0x0F36, 0x0E5C, 0x0D8E, 0x0CCC, 0x0C15, 0x0B68, 0x0AC5, 0x0A2A, 0x0999, 0x090F, 0x088E, 0x0813, 0x079F, 0x0732, 0x06CB, 0x066A, 0x060E, 0x05B7, 0x0565, 0x0518, 0x04CF, 0x048A, 0x0449, 0x040C, 0x03D2, 0x039B, 0x0367, 0x0337, 0x0309, 0x02DD, 0x02B4, 0x028D, 0x0269, 0x0246, 0x0226, 0x0207, 0x01EA, 0x01CE, 0x01B4, 0x019C, 0x0185, 0x016F, 0x015B, 0x0147, 0x0135, 0x0124, 0x0113, 0x0104, 0x00F5, 0x00E7, 0x00DB, 0x00CE, 0x00C3, 0x00B8, 0x00AD, 0x00A4, 0x009B, 0x0092, 0x008A, 0x0082, 0x007B, 0x0074, 0x006D, 0x0067, 0x0061, 0x005C, 0x0057, 0x0052, 0x004D, 0x0049, 0x0045, 0x0041, 0x003D, 0x003A, 0x0037, 0x0033, 0x0031, 0x002E, 0x002B, 0x0029, 0x0026, 0x0024, 0x0022, 0x0020, 0x001E, 0x001D, 0x001B, 0x001A, 0x0018, 0x0017, 0x0015, 0x0014, 0x0013, 0x0012, 0x0011, 0x0010, 0x000F, 0x000E, 0x000D, 0x000D, 0x000C, 0x000B, 0x000A, 0x000A, 0x0009, 0x0009, 0x0008, 0x0008, 0x0007, 0x0007, 0x0006, 0x0006, 0x0006, 0x0005, 0x0005, 0x0005, 0x0004, 0x0004, 0x0004, 0x0004, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};
/*tx Val,48300,45598,43048,40640,38366,36220,34194,32281,30475,28771,27161,25642,24207,22853,21575,20368,19229,18153,17138,16179,15274,14419,13613,12851,12132,11454,10813,10208,9637,9098,8589,8109,7655,7227,6823,6441,6081,5741,5419,5116,4830,4560,4305,4064,3837,3622,3419,3228,3048,2877,2716,2564,2421,2285,2157,2037,1923,1815,1714,1618,1527,1442,1361,1285,1213,1145,1081,1021,964,910,859,811,766,723,682,644,608,574,542,512,483,456,430,406,384,362,342,323,305,288,272,256,242,229,216,204,192,182,171,162,153,144,136,129,121,115,108,102,96,91,86,81,77,72,68,64,61,57,54,51,48,46,43,41,38,36,34,32,30,29,27,26,24,23,22,20,19,18,17,16,15,14,14,13,12,11,11,10,10,9,9,8,8,7,7,6,6,6,5,5,5,5,4,4,4,4,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1*/
static const IFX_uint16_t cid_tx_levels[] = {0xBCAC, 0xB21E, 0xA827, 0x9EBF, 0x95DE, 0x8D7C, 0x8592, 0x7E19, 0x770B, 0x7062, 0x6A19, 0x6429, 0x5E8F, 0x5945, 0x5446, 0x4F90, 0x4B1C, 0x46E9, 0x42F1, 0x3F32, 0x3BA9, 0x3853, 0x352C, 0x3233, 0x2F64, 0x2CBD, 0x2A3D, 0x27E0, 0x25A5, 0x238A, 0x218D, 0x1FAC, 0x1DE7, 0x1C3A, 0x1AA6, 0x1928, 0x17C0, 0x166C, 0x152B, 0x13FC, 0x12DE, 0x11CF, 0x10D0, 0x0FDF, 0x0EFC, 0x0E26, 0x0D5B, 0x0C9C, 0x0BE7, 0x0B3D, 0x0A9C, 0x0A04, 0x0974, 0x08ED, 0x086D, 0x07F4, 0x0782, 0x0717, 0x06B1, 0x0651, 0x05F7, 0x05A1, 0x0551, 0x0505, 0x04BD, 0x0479, 0x0439, 0x03FC, 0x03C3, 0x038D, 0x035A, 0x032A, 0x02FD, 0x02D2, 0x02AA, 0x0284, 0x0260, 0x023E, 0x021D, 0x01FF, 0x01E3, 0x01C7, 0x01AE, 0x0196, 0x017F, 0x016A, 0x0155, 0x0142, 0x0130, 0x011F, 0x010F, 0x0100, 0x00F2, 0x00E4, 0x00D7, 0x00CB, 0x00C0, 0x00B5, 0x00AB, 0x00A1, 0x0098, 0x0090, 0x0088, 0x0080, 0x0079, 0x0072, 0x006C, 0x0066, 0x0060, 0x005A, 0x0055, 0x0051, 0x004C, 0x0048, 0x0044, 0x0040, 0x003C, 0x0039, 0x0036, 0x0033, 0x0030, 0x002D, 0x002B, 0x0028, 0x0026, 0x0024, 0x0022, 0x0020, 0x001E, 0x001C, 0x001B, 0x0019, 0x0018, 0x0016, 0x0015, 0x0014, 0x0013, 0x0012, 0x0011, 0x0010, 0x000F, 0x000E, 0x000D, 0x000C, 0x000C, 0x000B, 0x000A, 0x000A, 0x0009, 0x0009, 0x0008, 0x0008, 0x0007, 0x0007, 0x0006, 0x0006, 0x0006, 0x0005, 0x0005, 0x0005, 0x0004, 0x0004, 0x0004, 0x0004, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};



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
                   /* number of UTDs for a channel can only be 1 or 2 */
   IFX_uint8_t     nMaxUtd = VIN_NUM_UTD;

   /* When UTD2 is supported by FW set the number of UTDs to 2 */
   if (pDev->nCapabilities & VIN_EDSP_UTD2_SUPPORTED)
      nMaxUtd = 2;

   *nUtd = 0;
   /* check signal until no further signal to be detect, maybe zero.
      For each run the current configured signal is removed from nSignal */
   while ((nSignal != 0) && err == IFX_SUCCESS && (*nUtd) < nMaxUtd)
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

   \param dir  - VIN_SIG_TX: local tx
               - VIN_SIG_RX: remote rx

   \return
      IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t Dsp_SetDtmfRec (VINETIC_CHANNEL const *pCh, IFX_boolean_t bEn,
                            IFX_uint8_t dir)
{
   IFX_int32_t ret = IFX_SUCCESS, ch = pCh->nChannel - 1;
   IFX_uint16_t pCmd [3] = {0};
   VINETIC_DEVICE *pDev = pCh->pParent;

   /* EOP Cmd */
   pCmd [0] = (CMD1_EOP | ch);
   /* DTMF REC */
   pCmd [1] = ECMD_DTMF_REC;
   pCmd [2] = pDev->pSigCh[ch].dtmf_rec.value;

   switch (dir)
   {
   case VIN_SIG_TX:
      pDev->pSigCh[ch].dtmf_rec.bit.is = 0;
      break;
   case VIN_SIG_RX:
      pDev->pSigCh[ch].dtmf_rec.bit.is = 1;
      break;
   default:
      break;
   }

   if ((bEn == IFX_TRUE) && !(pDev->pSigCh[ch].dtmf_rec.bit.en))
   {
      pDev->pSigCh[ch].dtmf_rec.bit.en = 1;
   }
   else if ((bEn == IFX_FALSE) && (pDev->pSigCh[ch].dtmf_rec.bit.en))
   {
      pDev->pSigCh[ch].dtmf_rec.bit.en = 0;
   }
   if (pCmd[2] != pDev->pSigCh[ch].dtmf_rec.value)
   {
      pCmd [2] = pDev->pSigCh[ch].dtmf_rec.value;
      ret = CmdWrite (pDev, pCmd, 1);
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
IFX_LOCAL IFX_int32_t Dsp_SetToneDetectors (VINETIC_CHANNEL *pCh,
                                            IFX_boolean_t bEn,
                                            SIG_MOD_STATE* pState)
{
   IFX_int32_t   ret = IFX_SUCCESS;
   IFX_uint8_t   ch = pCh->nChannel - 1;
   VINETIC_DEVICE *pDev    = pCh->pParent;

   if (bEn == IFX_FALSE)
   {
      /* remember module state */
      pState->flag.atd1 = pDev->pSigCh[ch].sig_atd1.bit.en;
      pState->flag.atd2 = pDev->pSigCh[ch].sig_atd2.bit.en;
      pState->flag.utd1 = pDev->pSigCh[ch].sig_utd1.bit.en;
      pState->flag.utd2 = pDev->pSigCh[ch].sig_utd2.bit.en;
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
      if (ret == IFX_SUCCESS && pDev->pSigCh[ch].sig_utd2.bit.en == 1 &&
          (pDev->nCapabilities & VIN_EDSP_UTD2_SUPPORTED)               )
      {
         pDev->pSigCh[ch].sig_utd2.bit.en = 0;
         ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_utd2.value, CMD_SIG_UTD_LEN);
      }
   }
   else
   {
      if (pState->flag.atd1 == 1 && pDev->pSigCh[ch].sig_atd1.bit.en == 0)
      {
         pDev->pSigCh[ch].sig_atd1.bit.en = 1;
         ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_atd1.value, CMD_SIG_ATD_LEN);
      }
      if (ret == IFX_SUCCESS && pState->flag.atd1 == 1 &&
          pDev->pSigCh[ch].sig_atd2.bit.en == 0)
      {
         pDev->pSigCh[ch].sig_atd2.bit.en = 1;
         ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_atd2.value, CMD_SIG_ATD_LEN);
      }
      if (ret == IFX_SUCCESS && pState->flag.atd1 == 1 &&
          pDev->pSigCh[ch].sig_utd1.bit.en == 0)
      {
         pDev->pSigCh[ch].sig_utd1.bit.en = 1;
         ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_utd1.value, CMD_SIG_UTD_LEN);
      }
      if (ret == IFX_SUCCESS && pState->flag.atd1 == 1 &&
          pDev->pSigCh[ch].sig_utd2.bit.en == 0 &&
          (pDev->nCapabilities & VIN_EDSP_UTD2_SUPPORTED))  
      {
         pDev->pSigCh[ch].sig_utd2.bit.en = 1;
         ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_utd2.value, CMD_SIG_UTD_LEN);
      }
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
         pDev->pSigCh[ch].sig_utd2.bit.en = 0;
         break;
      case  1:
         /* unmask UTD1 interrupt 0 -> 1  */
         nRising |= VIN_UTD1_OK_MASK;
         /* first UTD used */
         pDev->pSigCh[ch].sig_utd1.bit.en = 1;
         pDev->pSigCh[ch].sig_utd2.bit.en = 0;
         break;
      default:
         /* all UTDs are used : unmask interrupt 0 -> 1 for all utds */
         nRising |= VIN_UTD1_OK_MASK;
         /*  */
         pDev->pSigCh[ch].sig_utd1.bit.en = 1;
         if (pDev->nCapabilities & VIN_EDSP_UTD2_SUPPORTED)
         {
            pDev->pSigCh[ch].sig_utd2.bit.en = 1;
            nRising |= VIN_UTD2_OK_MASK;
         }
         break;
   }
   if (err == IFX_SUCCESS)
      err = VINETIC_Host_Set_EdspIntMask (pCh, VIN_EDSP_UTD, IFX_FALSE,
                                          nRising, 0);
   if (err == IFX_SUCCESS)
      err = CmdWrite (pDev, pDev->pSigCh[ch].sig_utd1.value, CMD_SIG_UTD_LEN);
   if (err == IFX_SUCCESS && (pDev->nCapabilities & VIN_EDSP_UTD2_SUPPORTED))
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
   Enables signal detection on the Modem Fax Tone Detector (MFTD)

   \param pCh - pointer to VINETIC channel structure
   \param nSignal - signal definition
   \param nSignalExt - extended signal definition
   \return
      IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t Dsp_MFTD_SigEnable (VINETIC_CHANNEL *pCh,
                                IFX_uint32_t nSignal, IFX_uint32_t nSignalExt)
{
   IFX_int32_t err = IFX_SUCCESS;
   VIN_IntMask_t   intMask;

   /* disable interrupts to avoid unwanted interrupts */
   memset (&intMask, 0, sizeof (intMask));
   err = VINETIC_Host_Set_EdspIntMask (pCh, VIN_EDSP_MFTD, IFX_FALSE,
                                       intMask.rising, intMask.falling);

   /* enable the tone detectors given and leave the others running */
   if (err == IFX_SUCCESS)
      err = Dsp_MFTD_SigSet (pCh,
                             pCh->pTapiCh->TapiMiscData.sigMask | nSignal,
                             pCh->pTapiCh->TapiMiscData.sigMaskExt | nSignalExt);


   if (err == IFX_SUCCESS)
   {
      /* enable all interrupts of the MFTD */
      intMask.rising = intMask.falling = (VIN_MFTD1_MASK | VIN_MFTD2_MASK);
      err = VINETIC_Host_Set_EdspIntMask (pCh, VIN_EDSP_MFTD, IFX_FALSE,
                                          intMask.rising, intMask.falling);
   }

   return err;
}

/**
   Disables signal detection on the Modem Fax Tone Detector (MFTD)

   \param pCh - pointer to VINETIC channel structure
   \param nSignal - signal definition
   \param nSignalExt - extended signal definition
   \return
      IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t Dsp_MFTD_SigDisable (VINETIC_CHANNEL *pCh,
                                 IFX_uint32_t nSignal, IFX_uint32_t nSignalExt)
{
   IFX_int32_t err = IFX_SUCCESS;
   VIN_IntMask_t   intMask;

   /* disable interrupts to avoid unwanted interrupts */
   memset (&intMask, 0, sizeof (intMask));
   err = VINETIC_Host_Set_EdspIntMask (pCh, VIN_EDSP_MFTD, IFX_FALSE,
                                       intMask.rising, intMask.falling);

   /* disable the tone detectors given and leave the others running */
   if (err == IFX_SUCCESS)
      err = Dsp_MFTD_SigSet (pCh,
                             pCh->pTapiCh->TapiMiscData.sigMask & ~nSignal,
                             pCh->pTapiCh->TapiMiscData.sigMaskExt & ~nSignalExt);

   if (err == IFX_SUCCESS)
   {
      /* enable all interrupts of the MFTD */
      intMask.rising = intMask.falling = (VIN_MFTD1_MASK | VIN_MFTD2_MASK);
      err = VINETIC_Host_Set_EdspIntMask (pCh, VIN_EDSP_MFTD, IFX_FALSE,
                                          intMask.rising, intMask.falling);
   }

   return err;
}

/**
   Sets signal detection on the Modem Fax Tone Detector (MFTD)

   \param pCh - pointer to VINETIC channel structure
   \param nSignal - signal definition
   \param nSignalExt - extended signal definition
   \return
      IFX_SUCCESS or IFX_ERROR
   \remarks
      Sets the MFTD to detect exact the tones given in the signal and signalExt
      parameters.
*/
IFX_LOCAL IFX_int32_t Dsp_MFTD_SigSet (VINETIC_CHANNEL *pCh,
                                       IFX_uint32_t nSignal,
                                       IFX_uint32_t nSignalExt)
{
   IFX_uint8_t     ch   = pCh->nChannel - 1;
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t err = IFX_SUCCESS;

   /* reprogram the MFTD only if something has changed */
   if ((nSignal    != pCh->pTapiCh->TapiMiscData.sigMask) ||
       (nSignalExt != pCh->pTapiCh->TapiMiscData.sigMaskExt))
   {
      /* changing anything but the MH bit requires to stop the MFTD first
         but stop only if the MFTD has previously been enabled */
      if ((pDev->pSigCh[ch].sig_mftd.bit.en == 1) &&
          (((nSignal    ^ pCh->pTapiCh->TapiMiscData.sigMask) &
            ~IFX_TAPI_SIG_TONEHOLDING_ENDMASK )                ||
            (nSignalExt ^ pCh->pTapiCh->TapiMiscData.sigMaskExt)  ))
      {
         pDev->pSigCh[ch].sig_mftd.bit.en = 0;
         err = CmdWrite (pDev, (IFX_uint16_t*)&pDev->pSigCh[ch].sig_mftd,
                         CMD_SIG_MFTDCTRL_LEN);
         if (err != IFX_SUCCESS)
         {
            return err;
         }
      }

      /* clear the data fields of the cached command first */
      pDev->pSigCh[ch].sig_mftd.value[CMD_HEADER_CNT+0] &= 0x000F;
      pDev->pSigCh[ch].sig_mftd.value[CMD_HEADER_CNT+1]  = 0x0000;
      pDev->pSigCh[ch].sig_mftd.value[CMD_HEADER_CNT+2]  = 0x0000;

      /* Modem holding characteristic is independent of direction */
      if (nSignal & IFX_TAPI_SIG_TONEHOLDING_ENDMASK)
         pDev->pSigCh[ch].sig_mftd.bit.mh = 1;

      /* Next the single tone detectors are activated as required */

      /* V.21 mark sequence */
      if (nSignalExt & (IFX_TAPI_SIG_EXT_V21L | IFX_TAPI_SIG_EXT_V21LRX))
         pDev->pSigCh[ch].sig_mftd.bit.single2 |=  SIG_MFTD_SINGLE_V21L;

      if (nSignalExt & (IFX_TAPI_SIG_EXT_V21L | IFX_TAPI_SIG_EXT_V21LTX))
         pDev->pSigCh[ch].sig_mftd.bit.single1 |=  SIG_MFTD_SINGLE_V21L;

      /* V.18A mark sequence */
      if (nSignalExt & (IFX_TAPI_SIG_EXT_V18A | IFX_TAPI_SIG_EXT_V18ARX))
         pDev->pSigCh[ch].sig_mftd.bit.single2 |=  SIG_MFTD_SINGLE_V18A;

      if (nSignalExt & (IFX_TAPI_SIG_EXT_V18A | IFX_TAPI_SIG_EXT_V18ATX))
         pDev->pSigCh[ch].sig_mftd.bit.single1 |=  SIG_MFTD_SINGLE_V18A;

      /* V.27, V.32 carrier */
      if (nSignalExt & (IFX_TAPI_SIG_EXT_V27 | IFX_TAPI_SIG_EXT_V27RX))
         pDev->pSigCh[ch].sig_mftd.bit.single2 |= SIG_MFTD_SINGLE_V27;

      if (nSignalExt & (IFX_TAPI_SIG_EXT_V27 | IFX_TAPI_SIG_EXT_V27TX))
         pDev->pSigCh[ch].sig_mftd.bit.single1 |= SIG_MFTD_SINGLE_V27;

      /* CNG Modem Calling Tone */
      if (nSignal & (IFX_TAPI_SIG_CNGMOD | IFX_TAPI_SIG_CNGMODRX))
         pDev->pSigCh[ch].sig_mftd.bit.single2 |= SIG_MFTD_SINGLE_CNGMOD;

      if (nSignal & (IFX_TAPI_SIG_CNGMOD | IFX_TAPI_SIG_CNGMODTX))
         pDev->pSigCh[ch].sig_mftd.bit.single1 |= SIG_MFTD_SINGLE_CNGMOD;

      /* CNG Fax Calling Tone */
      if (nSignal & (IFX_TAPI_SIG_CNGFAX | IFX_TAPI_SIG_CNGFAXRX))
         pDev->pSigCh[ch].sig_mftd.bit.single2 |= SIG_MFTD_SINGLE_CNGFAX;

      if (nSignal & (IFX_TAPI_SIG_CNGFAX | IFX_TAPI_SIG_CNGFAXTX))
         pDev->pSigCh[ch].sig_mftd.bit.single1 |= SIG_MFTD_SINGLE_CNGFAX;

      /* Bell answering tone */
      if (nSignalExt & (IFX_TAPI_SIG_EXT_BELL | IFX_TAPI_SIG_EXT_BELLRX))
         pDev->pSigCh[ch].sig_mftd.bit.single2 |= SIG_MFTD_SINGLE_BELL;

      if (nSignalExt & (IFX_TAPI_SIG_EXT_BELL | IFX_TAPI_SIG_EXT_BELLTX))
         pDev->pSigCh[ch].sig_mftd.bit.single1 |= SIG_MFTD_SINGLE_BELL;

      /* V.22 unsrambled binary ones */
      if (nSignalExt & (IFX_TAPI_SIG_EXT_V22 | IFX_TAPI_SIG_EXT_V22RX))
         pDev->pSigCh[ch].sig_mftd.bit.single2 |= SIG_MFTD_SINGLE_V22;

      if (nSignalExt & (IFX_TAPI_SIG_EXT_V22 | IFX_TAPI_SIG_EXT_V22TX))
         pDev->pSigCh[ch].sig_mftd.bit.single1 |= SIG_MFTD_SINGLE_V22;

      /* Next the dual tone detectors are activated as required */

      /* V.32 AC signal */
      if (nSignalExt & (IFX_TAPI_SIG_EXT_V32AC | IFX_TAPI_SIG_EXT_V32ACRX))
         pDev->pSigCh[ch].sig_mftd.bit.dual2 |= SIG_MFTD_DUAL_V32AC;

      if (nSignalExt & (IFX_TAPI_SIG_EXT_V32AC | IFX_TAPI_SIG_EXT_V32ACTX))
         pDev->pSigCh[ch].sig_mftd.bit.dual1 |= SIG_MFTD_DUAL_V32AC;

      /* V8bis signal */
      if (nSignal & (IFX_TAPI_SIG_V8BISRX))
         pDev->pSigCh[ch].sig_mftd.bit.dual2 |= SIG_MFTD_DUAL_V8bis;

      if (nSignal & (IFX_TAPI_SIG_V8BISTX))
         pDev->pSigCh[ch].sig_mftd.bit.dual1 |= SIG_MFTD_DUAL_V8bis;

      /* Next the answering tone detectors are activated as required */
      /* ATD can either be disabled, enabled to detect signal and phase
         reversals or enabled to detect signal with phase reversals and AM.
         So when enabled the phase reversal detection is always active.
         Enabeling detection of AM implicitly activates the detector. */

      if (nSignal & (IFX_TAPI_SIG_CED | IFX_TAPI_SIG_CEDRX |
                     IFX_TAPI_SIG_PHASEREV | IFX_TAPI_SIG_PHASEREVRX))
         pDev->pSigCh[ch].sig_mftd.bit.atd2 = SIG_MFTD_ATD_EN;

      if (nSignal & (IFX_TAPI_SIG_CED | IFX_TAPI_SIG_CEDTX |
                     IFX_TAPI_SIG_PHASEREV | IFX_TAPI_SIG_PHASEREVTX))
         pDev->pSigCh[ch].sig_mftd.bit.atd1 = SIG_MFTD_ATD_EN;

      if (nSignal & (IFX_TAPI_SIG_AM  | IFX_TAPI_SIG_AMRX))
         pDev->pSigCh[ch].sig_mftd.bit.atd2 = SIG_MFTD_ATD_AM_EN;

      if (nSignal & (IFX_TAPI_SIG_AM  | IFX_TAPI_SIG_AMTX))
         pDev->pSigCh[ch].sig_mftd.bit.atd1 = SIG_MFTD_ATD_AM_EN;

      /* Next the DIS tone detector is enabled if required  */
      if (nSignal & (IFX_TAPI_SIG_DIS | IFX_TAPI_SIG_DISRX))
         pDev->pSigCh[ch].sig_mftd.bit.dis2 = 1;

      if (nSignal & (IFX_TAPI_SIG_DIS | IFX_TAPI_SIG_DISTX))
         pDev->pSigCh[ch].sig_mftd.bit.dis1 = 1;

      /* Finally select which parts of the MFTD should be enabled  */

      /* Enable ATD event transmission if ATD is enabled */
      if ((pDev->pSigCh[ch].sig_mftd.bit.atd1 != 0) ||
          (pDev->pSigCh[ch].sig_mftd.bit.atd2 != 0)   )
      {
         pDev->pSigCh[ch].sig_mftd.bit.eta     = 1;
      }

      /* Enable DIS event transmission if DIS tone detector is enabled */
      if ((pDev->pSigCh[ch].sig_mftd.bit.dis1 != 0) ||
          (pDev->pSigCh[ch].sig_mftd.bit.dis2 != 0)   )
      {
         pDev->pSigCh[ch].sig_mftd.bit.etd  = 1;
      }

      /* Enable the CNG event transmission if CNG tone detector is enabled */
      if ((pDev->pSigCh[ch].sig_mftd.bit.single1 & SIG_MFTD_SINGLE_CNGFAX) ||
          (pDev->pSigCh[ch].sig_mftd.bit.single2 & SIG_MFTD_SINGLE_CNGFAX)   )
      {
         pDev->pSigCh[ch].sig_mftd.bit.etc     = 1;
      }

      /* Finally enable the entire MFTD if any detectors are enabled */
      if ((pDev->pSigCh[ch].sig_mftd.bit.mh != 0) ||
          (pDev->pSigCh[ch].sig_mftd.value[CMD_HEADER_CNT+1] != 0) ||
          (pDev->pSigCh[ch].sig_mftd.value[CMD_HEADER_CNT+2] != 0)   )
      {
         pDev->pSigCh[ch].sig_mftd.bit.en   = 1;
      }

      err = CmdWrite (pDev, (IFX_uint16_t*)&pDev->pSigCh[ch].sig_mftd,
                      CMD_SIG_MFTDCTRL_LEN);
   }

   if (err == IFX_SUCCESS)
   {
      pCh->pTapiCh->TapiMiscData.sigMask = nSignal;
      pCh->pTapiCh->TapiMiscData.sigMaskExt = nSignalExt;
   }
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

/**
   Enables or disables the signalling interface modules :
   DTMF gen, DTMF rec, CID, Tone generators

\param
   pCh    pointer to VINETIC channel structure
\param
   bOn    Switches on or off.
         - IFX_TRUE : restore the module state
         - IFX_FALSE : switch off and remember the state in pState
\param
   pState stores the current module state for restoring
\return
   IFX_SUCCESS or IFX_ERROR
\remark
   This function is called to disable the signalling modules without
   disabling the signalling channel itself and thus to avoid disturbance of an
   ongoing conference, as the input signals of signalling channel are connected
   in this process.
   If the modules are switched off the state is stored in pState. pState can
   be used to reactivate all modules, which were switched on before.
   If no matter what all modules should be activated pState->value must
   be 0xFFFF.
   The UTG and CID may be not, otherwise an error is generated */
IFX_int32_t Dsp_SwitchSignModules (VINETIC_CHANNEL *pCh, IFX_boolean_t bOn,
                                   SIG_MOD_STATE* pState)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t ret, ch = pCh->nChannel - 1;
   IFX_uint16_t pCmd[3];

#if 1
   if (bOn == IFX_FALSE &&
       (pDev->pSigCh[ch].utg[0].bit.en == IFX_TRUE))
   {
      /* the UTG or CID sender may not be active while the module
         is switched off */
      return IFX_SUCCESS;
   }
#endif
   if (bOn == IFX_FALSE &&
       (pDev->pSigCh[ch].cid_sender[2] & SIG_CID_EN))
   {
      /* the UTG or CID sender may not be active while the module
         is switched off */
      SET_ERROR (ERR_INVALID_SIGSTATE);
      return IFX_ERROR;
   }
   ret = Dsp_SetToneDetectors (pCh, bOn, pState);
   if (ret == IFX_SUCCESS)
   {
      pCmd [0] = (CMD1_EOP | ch);
      pCmd [1] = ECMD_DTMF_REC;
      if (bOn == IFX_TRUE)
      {
         /* switch it on if state flags on */
         if (pDev->pSigCh[ch].dtmf_rec.bit.en == 0 && pState->flag.dtmf_rec == 1)
         {
            pDev->pSigCh[ch].dtmf_rec.bit.en = 1;
            pCmd [2] = pDev->pSigCh[ch].dtmf_rec.value;
            ret = CmdWrite (pDev, pCmd, 1);
         }
      }
      else
      {
         /* store state and switch of */
         pState->flag.dtmf_rec = pDev->pSigCh[ch].dtmf_rec.bit.en;
         if (pDev->pSigCh[ch].dtmf_rec.bit.en == 1)
         {
            pDev->pSigCh[ch].dtmf_rec.bit.en = 0;
            pCmd [2] = pDev->pSigCh[ch].dtmf_rec.value;
            ret = CmdWrite (pDev, pCmd, 1);
         }
      }
   }
   if (ret == IFX_SUCCESS)
   {
      if (bOn == IFX_TRUE)
      {
         /* switch it on if state flags on */
         if (pDev->pSigCh[ch].sig_dtmfgen.bit.en == 0 && pState->flag.dtmfgen == 1)
         {
            pDev->pSigCh[ch].sig_dtmfgen.bit.en = 1;
            ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_dtmfgen.value,
                            CMD_SIG_DTMFGEN_LEN);
         }
      }
      else
      {
         /* store state and switch of */
         pState->flag.dtmfgen = pDev->pSigCh[ch].sig_dtmfgen.bit.en;
         if (pDev->pSigCh[ch].sig_dtmfgen.bit.en == 1)
         {
            pDev->pSigCh[ch].sig_dtmfgen.bit.en = 0;
            ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_dtmfgen.value,
                            CMD_SIG_DTMFGEN_LEN);
         }
      }
   }

   return ret;
}

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
      -5dB = 0x47FA, -10 dB = 0x287A, - 15 dB = 0x16C3, -20dB = 0xCCC,
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
      /* calculate level value from pFskConf->levelTX */
      pCidCoefs[2] = cid_tx_levels[ (-1)*pFskConf->levelTX / 5 ];
      /*printk("cid tx level %d dB/10, coef 0x%04X\n\r", pFskConf->levelTX, pCidCoefs[2]);*/
      /* set seizure and mark according to CID type */
      switch (cidHookMode)
      {
      /* offhook CID, called CID type 2, NTT inclusive */
      case  IFX_TAPI_CID_HM_OFFHOOK:
         /* set Seizure - off hook CID has always 0 seizure bits */
         pCidCoefs[3] = 0;
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
IFX_int32_t Dsp_SetCidRec (VINETIC_CHANNEL const *pCh, 
                           IFX_TAPI_CID_FSK_CFG_t *pFskConf,
                           IFX_boolean_t bEn)
{
   IFX_int32_t     err      = IFX_SUCCESS, ch = pCh->nChannel - 1;
   IFX_uint16_t    pCmd [5] = {0};
   VINETIC_DEVICE *pDev     = pCh->pParent;

   if (pDev->nChipMajorRev == VINETIC_V1x && bEn == IFX_TRUE)
   {
      err = Dsp_SetDtmfRec (pCh, IFX_FALSE, VIN_SIG_TX);
   }
   /* EOP Cmd */
   pCmd [0] = (CMD1_EOP | ch);
   /* CID RX */
   if (err == IFX_SUCCESS)
   {
      if (bEn == IFX_TRUE)
      {
         pCmd[1] = (CMD2_MOD_RES | 0x0700);
         /* coefficients for CID rx */
         pCmd[2] = cid_rx_levels[ (-1)*pFskConf->levelRX / 5 ];
         /*printk("cid rx level %d dB/10, coef 0x%04X\n\r", pFskConf->levelRX, pCmd[2]);*/
         /* set Seizure */
         pCmd[3] = pFskConf->seizureRX;
         /* set Mark */
         pCmd[4] = pFskConf->markRXOnhook;
         err = CmdWrite (pDev, pCmd, 3);
         if (err == IFX_SUCCESS)
         {
            pCmd [1] = ECMD_CIDRX;
            pCmd [2] = SIG_CIDRX_EN | ch;
            err = CmdWrite (pDev, pCmd, 1);
         }
      }
      else
      {
         pCmd [1] = ECMD_CIDRX;
         pCmd [2] = ch;
         err = CmdWrite (pDev, pCmd, 1);
      }
   }
   if (err == IFX_SUCCESS &&
       pDev->nChipMajorRev == VINETIC_V1x && bEn == IFX_FALSE)
   {
      /* for the 4VIP V1.4 the DTMF must be reconfigured cause of the overlay */
      pCmd[1] = ECMD_DTMF_REC_COEFF;
      /* Level= -56dB, Twist= 9.1dB */
      pCmd[2] = 0xC81C;
      pCmd[3] = 0x0060;
      err = CmdWrite (pDev, pCmd, 2);
      if (err == IFX_SUCCESS)
         err = Dsp_SetDtmfRec ((VINETIC_CHANNEL const *) pCh,
                                IFX_TRUE, VIN_SIG_TX);
   }

   return err;
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

/**
   Enables or disables the signalling interface modules :
   DTMF gen, DTMF rec, CID, Tone generators
\param  pDev    pointer to VINETIC device structure
\param  ch     channel number
\param  state  activation state of the modules
\return
   IFX_SUCCESS or IFX_ERROR
\remark
   This function is called to disable or enable the event transmission flags
   of the different firmware modules.
   It also programs it to the DSP.
   The DTMF receiver needs special handling cause it one more bit is
   modified in TAPI_LL_Send_DTMF_OOB and in TAPI_LL_RTP_Conf.
   The calling function need at least to know, whether the command has been
   sent */
IFX_int32_t Dsp_UpdateEventTrans (VINETIC_DEVICE *pDev, IFX_uint8_t ch,
                                  SIG_MOD_STATE state, SIG_MOD_STATE* apply)
{
   IFX_int32_t err = IFX_SUCCESS;

   if (state.flag.atd1 != pDev->pSigCh[ch].sig_atd1.bit.et)
   {
      pDev->pSigCh[ch].sig_atd1.bit.et = state.flag.atd1;
      err = CmdWrite (pDev, pDev->pSigCh[ch].sig_atd1.value, CMD_SIG_ATD_LEN);
   }
   if (err == IFX_SUCCESS &&
       state.flag.atd2 != pDev->pSigCh[ch].sig_atd2.bit.et)
   {
      pDev->pSigCh[ch].sig_atd2.bit.et = state.flag.atd2;
      err = CmdWrite (pDev, pDev->pSigCh[ch].sig_atd2.value, CMD_SIG_ATD_LEN);
   }
   if (err == IFX_SUCCESS &&
       state.flag.utd1 != pDev->pSigCh[ch].sig_utd1.bit.et)
   {
      pDev->pSigCh[ch].sig_utd1.bit.et = state.flag.utd1;
      err = CmdWrite (pDev, pDev->pSigCh[ch].sig_utd1.value, CMD_SIG_UTD_LEN);
   }
#ifdef INCLUDE_UTD2
   if (err == IFX_SUCCESS &&
       state.flag.utd2 != pDev->pSigCh[ch].sig_utd2.bit.et)
   {
      pDev->pSigCh[ch].sig_utd2.bit.et = state.flag.utd2;
      err = CmdWrite (pDev, pDev->pSigCh[ch].sig_utd2.value, CMD_SIG_UTD_LEN);
   }
#endif /* INCLUDE_UTD2 */
   if (err == IFX_SUCCESS &&
      state.flag.dtmf_rec != pDev->pSigCh[ch].dtmf_rec.bit.et)
   {
      IFX_uint16_t pCmd[3];
      if (apply != IFX_NULL)
         apply->flag.dtmf_rec = IFX_TRUE;
      pDev->pSigCh[ch].dtmf_rec.bit.et = state.flag.dtmf_rec;
      /* set DTMF Receiver read command */
      pCmd[0] = CMD1_EOP | ch;
      pCmd[1] = ECMD_DTMF_REC;
      pCmd[2] = pDev->pSigCh[ch].dtmf_rec.value;
      err = CmdWrite (pDev, pCmd, 1);
   }

   return err;
}

/**
   Resets the signalling channel
\param  pDev    pointer to VINETIC device structure
\param  ch     channel number
\param  bOn Target operation status on or maybe off
         - IFX_TRUE it must be switched on. No check if needed is done
         - IFX_FALSE a resource has been deactivated, thus check if signaling
           channel is still needed
\return
   IFX_SUCCESS or IFX_ERROR
\remark
   This function is called to minimize the DSP usage after deactivating resources.
   If no more resource is used in the signaling channel the channel is shut down. */
IFX_int32_t Dsp_SigActStatus (VINETIC_DEVICE *pDev, IFX_uint8_t ch, IFX_boolean_t bOn,
            IFX_int32_t (*pCmdWrite) (VINETIC_DEVICE *pDev, IFX_uint16_t* pCmd, IFX_uint8_t count))
{
   IFX_int32_t ret = IFX_SUCCESS;

   if (bOn == IFX_FALSE)
   {
      if (pDev->pSigCh[ch].dtmf_rec.bit.en == 0 &&
         pDev->pSigCh[ch].sig_dtmfgen.bit.en == 0 &&
         pDev->pSigCh[ch].sig_atd1.bit.en == 0 &&
         pDev->pSigCh[ch].sig_atd2.bit.en == 0 &&
         pDev->pSigCh[ch].sig_utd1.bit.en == 0 &&
         pDev->pSigCh[ch].sig_utd2.bit.en == 0 &&
         pDev->pSigCh[ch].utg[0].bit.en == 0 && pDev->pSigCh[ch].utg[0].bit.sm == 0 &&
         pDev->pSigCh[ch].utg[1].bit.en == 0 && pDev->pSigCh[ch].utg[1].bit.sm == 0 &&
         !(pDev->pSigCh[ch].cid_sender[2] & SIG_CID_EN) &&
         !(pDev->pSigCh[ch].nCpt & VIN_ECMD_SIGCPT_EN)
         )
      {
         /* check to be switched off */
         if (pDev->pSigCh[ch].sig_ch.bit.en == 1)
         {
            /* switch the signaling channel off */
            pDev->pSigCh[ch].sig_ch.bit.en = 0;
            ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_ch.value, CMD_SIG_CH_LEN);
         }
      }
      else
      {
         /* leave the state activated, but additional check is done */
         if (pDev->pSigCh[ch].sig_ch.bit.en == 0)
         {
            SET_ERROR (ERR_INVALID_SIGSTATE);
            return IFX_ERROR;
         }
         return IFX_SUCCESS;
      }
   }
   else
   {
      /* switch the signaling channel on */
      if (pDev->pSigCh[ch].sig_ch.bit.en == 0)
      {
         pDev->pSigCh[ch].sig_ch.bit.en = 1;
         ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_ch.value, CMD_SIG_CH_LEN);
      }
   }
   return ret;
}

/**
   Resets the signalling channel
\param  pDev    pointer to VINETIC device structure
\param  ch     channel number
\return
   IFX_SUCCESS or IFX_ERROR
\remark
   This function is called to reset and initialize the signaling algorithms again
   after a coder stop.
   Stores all activation states of ATD, UTD, DTMF receiver and generator,
   switches the signaling channel off and on again. Then restore en bits.
   The UTG and CID may be not, otherwise an error is generated */
IFX_int32_t Dsp_SigReset  (VINETIC_DEVICE *pDev, IFX_uint8_t ch)
{
   SIG_MOD_STATE modState;
   IFX_int32_t ret;

   modState.value = 0;
   IFXOS_MutexLock (pDev->memberAcc);
   /* reset signaling module */
   ret = Dsp_SwitchSignModules (&pDev->pChannel[ch], IFX_FALSE, &modState);
   if (ret == IFX_SUCCESS)
   {
      /* switch the signaling channel off */
      pDev->pSigCh[ch].sig_ch.bit.en = 0;
      ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_ch.value, CMD_SIG_CH_LEN);
   }
   if (ret == IFX_SUCCESS)
   {
      /* switch the signaling channel on */
      pDev->pSigCh[ch].sig_ch.bit.en = 1;
      ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_ch.value, CMD_SIG_CH_LEN);
   }
   /* restore the module states */
   if (ret == IFX_SUCCESS)
   {
      ret = Dsp_SwitchSignModules (&pDev->pChannel[ch], IFX_TRUE, &modState);
   }
   IFXOS_MutexUnlock (pDev->memberAcc);
   return ret;
}
#endif /* (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M | VIN_FEAT_VIN_2CPE) */

