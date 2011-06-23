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
   Module      : drv_vinetic_tapi.c
   Date        : 2003-10-28
   This file contains the implementations of vinetic related
                 tapi low level functions.
******************************************************************************/

/* ============================= */
/* includes                      */
/* ============================= */

#include "drv_vinetic_api.h"
#include "drv_vinetic_cid.h"
#include "drv_vinetic_tapi.h"
#include "drv_vinetic_basic.h"
#include "drv_vinetic_main.h"
#include "drv_vinetic_dspconf.h"
#include "drv_vinetic_con.h"
#include "drv_vinetic_stream.h"
#include "drv_vinetic_tone.h"
#ifdef VIN_POLL
   #include "drv_vinetic_polled.h"
#endif /* VIN_POLL */
#ifdef TAPI


/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

/* maximum number of capabilities, used for allocating array */
#define MAX_CAPS              30

/* Frame lengths in ms */
#define FL_5MS                5
#define FL_5_5MS              6
#define FL_10MS               10
#define FL_20MS               20
#define FL_30MS               30
#define FL_11MS               11

/* define for encoder packtet time not supported, invalid payload type */
#define PT_INVAL                  1000

#ifdef TAPI_CID
/* DTMF max digt/inter digit time */
#define MAX_DIGIT_TIME        127 /* ms */
#define MAX_INTERDIGIT_TIME   127 /* ms */
#endif /* TAPI_CID */

#define AGC_CONFIG_COM_MAX       0
#define AGC_CONFIG_COM_MIN       (-50)
#define AGC_CONFIG_COM_OFFSET    50

#define AGC_CONFIG_GAIN_MAX       48
#define AGC_CONFIG_GAIN_MIN       0
#define AGC_CONFIG_GAIN_OFFSET    0

#define AGC_CONFIG_ATT_MAX       0
#define AGC_CONFIG_ATT_MIN       (-42)
#define AGC_CONFIG_ATT_OFFSET    42

#define AGC_CONFIG_LIM_MAX       (-25)
#define AGC_CONFIG_LIM_MIN       (-60)
#define AGC_CONFIG_LIM_OFFSET    60


/* ============================= */
/*             Enums             */
/* ============================= */

/* ============================= */
/* Local function declaration    */
/* ============================= */
IFX_LOCAL IFX_int32_t AddCaps      (VINETIC_DEVICE *pDev);
IFX_LOCAL IFX_void_t AddCapability (IFX_TAPI_CAP_t* CapList,
                                    IFX_uint32_t *pnCap, IFX_char_t const * description,
                                    IFX_int32_t type, IFX_int32_t value);

/* Configuration functions for Tapi Init */
/* Voice Coder Configuration */
IFX_LOCAL IFX_int32_t initVineticTapi             (VINETIC_DEVICE *pDev, IFX_uint8_t nCountry, IFX_void_t const *pInit);
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
IFX_LOCAL IFX_int32_t CheckENCnPTE                (IFX_uint8_t nFrameLength, IFX_uint8_t nCodec);
IFX_LOCAL IFX_char_t getEncoder                   (IFX_int32_t nCodec);
IFX_LOCAL IFX_int32_t getFrameLength              (IFX_int32_t nPTE);
IFX_LOCAL IFX_int32_t getConvPTE                  (IFX_int32_t nPrePTE, IFX_int32_t nCodec);
IFX_LOCAL IFX_int32_t getPTE                      (IFX_int32_t nFrameLength);
IFX_LOCAL IFX_return_t TAPI_LL_Local_Module_Connect (IFX_TAPI_CONN_ACTION_t nAction, VINETIC_DEVICE *pDev,
                                                     IFX_uint32_t nCh1, IFX_TAPI_MAP_TYPE_t nChType1,
                                                     IFX_uint32_t nCh2, IFX_TAPI_MAP_TYPE_t nChType2);
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* Predefined tones frequencies */
IFX_LOCAL
const IFX_uint32_t VINETIC_PredefinedToneFreq [IFX_TAPI_TONE_INDEX_MIN - 1][2] =
{
    /* Freq1/Freq2(Hz) *  Freq1/Freq2(Hz) - Index - DTMF Digit */
    {697, 1209},   /*   697/1209 Hz,   1,       1 */
    {697, 1336},   /*   697/1336 Hz,   2,       2 */
    {697, 1477},   /*   697/1477 Hz,   3,       3 */
    {770, 1209},   /*   770/1209 Hz,   4,       4 */
    {770, 1336},   /*   770/1336 Hz,   5,       5 */
    {770, 1477},   /*   770/1477 Hz,   6,       6 */
    {852, 1209},   /*   852/1209 Hz,   7,       7 */
    {852, 1336},   /*   852/1336 Hz,   8,       8 */
    {852, 1477},   /*   852/1477 Hz,   9,       9 */
    {941, 1209},   /*   941/1209 Hz,   10,      * */
    {941, 1336},   /*   941/1336 Hz,   11,      0 */
    {941, 1477},   /*   941/1477 Hz,   12,      # */
    {800,    0},   /*   800/0    Hz,   13,      - */
    {1000,   0},   /*   1000/0   Hz,   14,      - */
    {1250,   0},   /*   1250/0   Hz,   15,      - */
    {950,    0},   /*   950/0    Hz,   16,      - */
    {1100,   0},   /*   1100/0   Hz,   17,      - */ /* --> CNG Tone */
    {1400,   0},   /*   1400/0   Hz,   18,      - */
    {1500,   0},   /*   1500/0   Hz,   19,      - */
    {1600,   0},   /*   1600/0   Hz,   20,      - */
    {1800,   0},   /*   1800/0   Hz,   21,      - */
    {2100,   0},   /*   2100/0   Hz,   22,      - */ /* --> CED Tone */
    {2300,   0},   /*   2300/0   Hz,   23,      - */
    {2450,   0},   /*   2450/0   Hz,   24,      - */
    {350,  440},   /*   350/440  Hz,   25,      - */  /* --> Dial Tone */
    {440,  480},   /*   440/480  Hz,   26,      - */  /* --> Ring Back */
    {480,  620},   /*   480/620  Hz,   27,      - */  /* --> Busy Tone */
    {697, 1633},   /*   697/1633 Hz,   28,      A */
    {770, 1633},   /*   770/1633 Hz,   29,      B */
    {852, 1633},   /*   852/1633 Hz,   30,      C */
    {941, 1633},   /*   941/1633 Hz,   31,      D */
};

/* encoder packet time table : PT_INVAL means not supported */
IFX_LOCAL const IFX_int32_t VINETIC_PteVal [IFX_TAPI_ENC_TYPE_MAX][6] =
{
   /*                          PTE values                                */
   {0,        1,          2,           3,          4,           5        },
   /*           IFX_TAPI_ENC_TYPE_G723_63, us - index 1                  */
   {PT_INVAL, PT_INVAL,   PT_INVAL,    30000,      PT_INVAL,    PT_INVAL },
   /*           IFX_TAPI_ENC_TYPE_G723_53, us - index 2                  */
   {PT_INVAL, PT_INVAL,   PT_INVAL,    30000,      PT_INVAL,    PT_INVAL },
   {PT_INVAL, PT_INVAL,   PT_INVAL,    PT_INVAL,   PT_INVAL,    PT_INVAL },
   {PT_INVAL, PT_INVAL,   PT_INVAL,    PT_INVAL,   PT_INVAL,    PT_INVAL },
   {PT_INVAL, PT_INVAL,   PT_INVAL,    PT_INVAL,   PT_INVAL,    PT_INVAL },
   /*           IFX_TAPI_ENC_TYPE_G728   , us - index 6                  */
   {5000,     10000,      20000,       PT_INVAL,   PT_INVAL,    PT_INVAL },
   /*           IFX_TAPI_ENC_TYPE_G729   , us - index 7                  */
   {PT_INVAL, 10000,      20000,       PT_INVAL,   PT_INVAL,    PT_INVAL },
   /*           IFX_TAPI_ENC_TYPE_MLAW   , us - index 8                  */
   {5000,     10000,      20000,       PT_INVAL,   5500,        11000    },
   /*           IFX_TAPI_ENC_TYPE_ALAW   , us - index 9                  */
   {5000,     10000,      20000,       PT_INVAL,   5500,        11000    },
   {PT_INVAL, PT_INVAL,   PT_INVAL,    PT_INVAL,   PT_INVAL,    PT_INVAL },
   {PT_INVAL, PT_INVAL,   PT_INVAL,    PT_INVAL,   PT_INVAL,    PT_INVAL },
   /*           IFX_TAPI_ENC_TYPE_G726_16, us - index 12                 */
   {5000,     10000,      20000,      PT_INVAL,    5500,        11000},
   /*           IFX_TAPI_ENC_TYPE_G726_24, us - index 13                 */
   {5000,     10000,      20000,       PT_INVAL,   PT_INVAL,    PT_INVAL },
   /*           IFX_TAPI_ENC_TYPE_G726_32, us - index 14                 */
   {5000,     10000,      20000,       PT_INVAL,   5500,        11000    },
   /*           IFX_TAPI_ENC_TYPE_G726_40, us - index 15                 */
   {5000,     10000,      20000,       PT_INVAL,   PT_INVAL,    PT_INVAL },
   /*           IFX_TAPI_ENC_TYPE_G729_E , us - index 16                 */
   {PT_INVAL, 10000,      20000,       PT_INVAL,   PT_INVAL,    PT_INVAL },
   /*           IFX_TAPI_ENC_TYPE_ILBC_133 , us - index 17               */
   {PT_INVAL, PT_INVAL,   PT_INVAL,    30000,      PT_INVAL,    PT_INVAL },
   /*           IFX_TAPI_ENC_TYPE_ILBC_152 , us - index 18               */
   {PT_INVAL, PT_INVAL,   20000,       PT_INVAL,   PT_INVAL,    PT_INVAL }
};

/* calculated table to set the Rx/Tx Gain in ALM/PCM module.
   Converts the gain in 'dB' into the regiter values */
IFX_LOCAL const  IFX_uint8_t VINETIC_AlmPcmGain [49] =
{
   /* Gain Byte Calculation Table in 'dB'
      See also VINETIC User's Manual Table 15 Rev 1.0 */
   /* -24   -23   -22   -21   -20   -19   -18   -17   -16   -15   */
      224,  228,  233,  238,  243,  249,  192,  196,  201,  206,
   /* -14   -13   -12   -11   -10   -9    -8    -7    -6    -5    */
      211,  217,  160,  164,  168,  173,  179,  185,  128,  132,
   /* -4    -3    -2    -1    0     1     2     3     4     5     */
      136,  141,  147,  153,  96,   100,  104,  109,  115,  121,
   /* 6     7     8     9     10    11    12    13    14    15    */
      64,   68,   72,   77,   83,   89,   32,   36,   40,   45,
   /* 16    17    18    19    20    21    22    23    24          */
      50,   57,   0,    4,    8,    13,   18,   25,   31
};

static IFX_uint8_t AgcConfig_Reg_COM[] =
{
   /* conversion table from 'dB# to HEX register values */
   /* -50   -49   -48   -47   -46   -45   -44   -43   -42   -41   -40   */
      132,  133,  133,  134,  134,  135,  136,  137,  138,  140,  141,
   /* -39   -38   -37   -36   -35   -34   -33   -32   -31   -30   -29   */
      142,  144,  146,  148,  151,  154,  157,  160,  164,  169,  174,
   /* -28   -27   -26   -25   -24   -23   -22   -21   -20   -19   -18   */
      179,  186,  193,  7,    8,    9,    10,   11,   13,   14,   16,
   /* -17   -16   -15   -14   -13   -12   -11   -10   -9    -8    -7    */
      18,   20,   23,   26,   29,   32,   36,   40,   45,   51,   57,
   /* -6    -5    -4    -3    -2    -1    0  */
      64,   72,   81,   91,   102,  114,  128
};

static IFX_uint8_t AgcConfig_Reg_GAIN[] =
{
   /* 0     1     2     3     4     5     6     7     8     9     10    */
      136,  137,  138,  139,  141,  142,  144,  146,  148,  151,  153,
   /* 11    12    13    14    15    16    17    18    19    20    21    */
      156,  160,  164,  168,  173,  178,  185,  4,    199,  208,  218,
   /* 22    23    24    25    26    27    28    29    30    31    32    */
      229,  241,  32,   36,   40,   45,   50,   56,   63,   71,   80,
   /* 33    34    35    36    37    38    39    40    41    42    43    */
      89,   100,  112,  126,  142,  159,  178,  200,  224,  252,  255,
   /* 44    45    46    47    48    */
      255,  255,  255,  255,  255
};

static IFX_uint8_t AgcConfig_Reg_ATT[] =
{
   /* -42   -41   -40   -39   -38   -37   -36   -35   -34   -33   -32   */
      1,    1,    1,    1,    2,    2,    2,    2,    3,    3,    3,
   /* -31   -30   -29   -28   -27   -26   -25   -24   -23   -22   -21   */
      4,    4,    5,    5,    6,    6,    7,    8,    9,    10,   11,
   /* -20   -19   -18   -17   -16   -15   -14   -13   -12   -11   -10   */
      13,   14,   16,   18,   20,   23,   26,   29,   32,   36,   40,
   /* -9    -8    -7    -6    -5    -4    -3    -2    -1    0  */
      45,   51,   57,   64,   72,   81,   91,   102,  114,  127
};

static IFX_uint8_t AgcConfig_Reg_LIM[] =
{
   /* -60   -59   -58   -57   -56   -55   -54   -53   -52   -51   -50   */
      161,  165,  169,  174,  180,  186,  193,  201,  210,  220,  232,
   /* -49   -48   -47   -46   -45   -44   -43   -42   -41   -40   -39   */
      7,    8,    9,    10,   12,   13,   14,   16,   18,   20,   23,
   /* -38   -37   -36   -35   -34   -33   -32   -31   -30   -29   -28   */
      26,   29,   32,   36,   41,   46,   51,   58,   65,   73,   81,
   /* -27   -26   -25   */
      91,   103,  115
};

/* ============================= */
/* Local functions definitions   */
/* ============================= */

/**
  Add capability to given list
\param pnCap        -
\param description  -
\param type         -
\param value        -
\return
   none
\remarks
   add a capability to the existing ones and increase the counter nCap
*/
IFX_LOCAL IFX_void_t AddCapability (IFX_TAPI_CAP_t* CapList,
                                    IFX_uint32_t *pnCap, IFX_char_t const * description,
                                    IFX_int32_t type, IFX_int32_t value)
{
   IFX_uint32_t capnr;

   if (pnCap  == NULL)
      return;

   if (*pnCap >= MAX_CAPS)
      return;

   capnr = (*pnCap);
   strncpy (CapList[capnr].desc, description, sizeof(CapList[0].desc));
   CapList[capnr].captype = (IFX_TAPI_CAP_TYPE_t)type;
   CapList[capnr].cap = value;
   CapList[capnr].handle = (int)capnr;
   (*pnCap) = capnr + 1;
   TRACE (VINETIC,DBG_LEVEL_NORMAL, ("Cap: %s , type %ld = %ld\n\r",
          description, type, value));
}

/**
  set all vinetic capabilities
\param pDev ptr to actual Vinetic Device
\return
   IFX_SUCCESS or IFX_ERROR if no memory is available
Remark:
   Macro MAX_CAPS must match with the capabilities number. So adapt this macro
   accordingly if new capabilities are added.
*/
IFX_LOCAL IFX_int32_t AddCaps (VINETIC_DEVICE *pDev)
{
   /* capability list */
   IFX_TAPI_CAP_t *CapList;

   /* start adding the capability with coders */
   IFX_uint32_t nCap = 0;

   /* holds the maximum amount of capabilities */
   if (pDev->CapList != NULL)
   {
      IFXOS_FREE (pDev->CapList);
   }
   pDev->CapList = (IFX_TAPI_CAP_t*)
                   IFXOS_MALLOC(MAX_CAPS * sizeof(IFX_TAPI_CAP_t));
   if (pDev->CapList == NULL)
   {
      SET_ERROR (ERR_NO_MEM);
      return IFX_ERROR;
   }
   CapList = pDev->CapList;

   AddCapability (CapList, &nCap,"INFINEON TECHNOLOGIES",IFX_TAPI_CAP_TYPE_VENDOR,0);
   AddCapability (CapList, &nCap, "VINETIC", IFX_TAPI_CAP_TYPE_DEVICE, 0);

   AddCapability (CapList, &nCap, "POTS", IFX_TAPI_CAP_TYPE_PORT, IFX_TAPI_CAP_PORT_POTS);
   AddCapability (CapList, &nCap, "PSTN", IFX_TAPI_CAP_TYPE_PORT, IFX_TAPI_CAP_PORT_PSTN);

#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
   if ((!((pDev->nEdspVers[0] & ECMD_VERS_FEATURES)
           == ECMD_VERS_FEATURES_C)                          ||
         ((pDev->nEdspVers[0] & ECMD_VERS_FEATURES)
           == ECMD_VERS_FEATURES_S))                         &&
         (pDev->nChipType != VINETIC_TYPE_S))
   {
      /* coder features of M and VIP versions */
      AddCapability (CapList, &nCap, "DSP", IFX_TAPI_CAP_TYPE_DSP, 0);

      /* add the defaults */
      AddCapability (CapList, &nCap, "G.726 16 kbps",
                     IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G726_16);
      AddCapability (CapList, &nCap, "G.726 24 kbps",
                     IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G726_24);
      AddCapability (CapList, &nCap, "G.726 32 kbps",
                     IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G726_32);
      AddCapability (CapList, &nCap, "G.726 40 kbps",
                     IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G726_40);
      AddCapability (CapList, &nCap, "u-LAW",
                     IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_MLAW);
      AddCapability (CapList, &nCap, "A-LAW",
                     IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_ALAW);
      if ((pDev->nEdspVers[0] & ECMD_VERS_MV) == ECMD_VERS_MV_4)
      {
         switch (pDev->nEdspVers[0] & ECMD_VERS_FEATURES)
         {
         case ECMD_VERS_FEATURES_729ABE_G723:
            AddCapability (CapList, &nCap, "G.723 6.3kbps",
                           IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G723_63);
            AddCapability (CapList, &nCap, "G.723 5.3kbps",
                           IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G723_53);
            AddCapability (CapList, &nCap, "G.729",
                           IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G729);
            AddCapability (CapList, &nCap, "G.729E",
                           IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G729_E);
#ifdef VIN_2CPE
            AddCapability (CapList, &nCap, "iLBC 13.3 kbps",
                           IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_ILBC_133);
            AddCapability (CapList, &nCap, "iLBC 15.2 kbps",
                           IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_ILBC_152);
            AddCapability (CapList, &nCap, "T.38", IFX_TAPI_CAP_TYPE_T38, 1);
#endif /* VIN_2CPE */
            break;
         case ECMD_VERS_FEATURES_G729AB_G723:
            AddCapability (CapList, &nCap, "G.729",
                           IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G729);
            AddCapability (CapList, &nCap, "G.729E",
                           IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G729_E);
#ifdef VIN_2CPE
            AddCapability (CapList, &nCap, "T.38", IFX_TAPI_CAP_TYPE_T38, 1);
#else
            AddCapability (CapList, &nCap, "G.723 6.3kbps",
                           IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G723_63);
            AddCapability (CapList, &nCap, "G.723 5.3kbps",
                           IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G723_53);
#endif /* VIN_2CPE */
            break;
         case ECMD_VERS_FEATURES_G728_G723:
            AddCapability (CapList, &nCap, "G.728",
                           IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G728);
            AddCapability (CapList, &nCap, "G.723 6.3kbps",
                           IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G723_63);
            AddCapability (CapList, &nCap, "G.723 5.3kbps",
                           IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G723_53);
            break;
         case ECMD_VERS_FEATURES_G728_G729ABE:
            AddCapability (CapList, &nCap, "G.728",
                           IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G728);
            AddCapability (CapList, &nCap, "G.729",
                           IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G729);
            AddCapability (CapList, &nCap, "G.729E",
                           IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G729_E);
            break;
         case ECMD_VERS_FEATURES_729ABE_T38:
            AddCapability (CapList, &nCap, "G.729",
                           IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G729);
            AddCapability (CapList, &nCap, "G.729E",
                           IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G729_E);
            AddCapability (CapList, &nCap, "T.38",
                           IFX_TAPI_CAP_TYPE_T38, 1);
            break;
         case ECMD_VERS_FEATURES_G729AB_G728_T38:
            AddCapability (CapList, &nCap, "G.729",
                           IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G729);
            AddCapability (CapList, &nCap, "G.728",
                           IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G728);
            AddCapability (CapList, &nCap, "T.38",
                           IFX_TAPI_CAP_TYPE_T38, 1);
            break;
         case ECMD_VERS_FEATURES_G723_G728_T38:
            AddCapability (CapList, &nCap, "G.723 6.3kbps",
                           IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G723_63);
            AddCapability (CapList, &nCap, "G.723 5.3kbps",
                           IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G723_53);
            AddCapability (CapList, &nCap, "G.728",
                           IFX_TAPI_CAP_TYPE_CODEC, IFX_TAPI_ENC_TYPE_G728);
            AddCapability (CapList, &nCap, "T.38",
                           IFX_TAPI_CAP_TYPE_T38, 1);
            break;
         default:
            TRACE (VINETIC,DBG_LEVEL_NORMAL, ("Warning: Unknown feature set\n\r"));
            break;
         }
      }
      /* set coder channels */
      switch (pDev->nEdspVers[0] & ECMD_VERS_MV)
      {
      case ECMD_VERS_MV_8:
         AddCapability (CapList, &nCap, "Coder",
                        IFX_TAPI_CAP_TYPE_CODECS, 8);
         break;
      case 0x6000:
         AddCapability (CapList, &nCap, "Coder",
                        IFX_TAPI_CAP_TYPE_CODECS, 6);
         break;
      default:
         AddCapability (CapList, &nCap, "Coder",
                        IFX_TAPI_CAP_TYPE_CODECS, 4);
      }
   }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
   switch (pDev->nChipType)
   {
   case VINETIC_TYPE_S:
      AddCapability (CapList, &nCap, "PCM", IFX_TAPI_CAP_TYPE_PCM, 4);
      break;
   default:
      AddCapability (CapList, &nCap, "PCM", IFX_TAPI_CAP_TYPE_PCM, 8);
      AddCapability (CapList, &nCap, "CED",
                     IFX_TAPI_CAP_TYPE_SIGDETECT, IFX_TAPI_CAP_SIG_DETECT_CED);
      AddCapability (CapList, &nCap, "CNG",
                     IFX_TAPI_CAP_TYPE_SIGDETECT, IFX_TAPI_CAP_SIG_DETECT_CNG);
      AddCapability (CapList, &nCap, "DIS",
                     IFX_TAPI_CAP_TYPE_SIGDETECT, IFX_TAPI_CAP_SIG_DETECT_DIS);
      AddCapability (CapList, &nCap, "POWER",
                     IFX_TAPI_CAP_TYPE_SIGDETECT, IFX_TAPI_CAP_SIG_DETECT_POWER);
      AddCapability (CapList, &nCap, "CPT",
                     IFX_TAPI_CAP_TYPE_SIGDETECT, IFX_TAPI_CAP_SIG_DETECT_CPT);
      break;
   }
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */

   AddCapability (CapList, &nCap, "Phones",
                  IFX_TAPI_CAP_TYPE_PHONES, pDev->nAnaChan);
   /* check if no array out of bounds */
   IFXOS_ASSERT (nCap <= MAX_CAPS);
   pDev->nMaxCaps = (IFX_uint8_t)nCap;

   return IFX_SUCCESS;
}


/**
   Configure VINETIC Chip for Voice Coder purposes
\param pDev        pointer to VINETIC device structure
\param nCountry    county of which configuration should be done
\param pInit       ptr to device initialization structure
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   The parameter nCountry is obsolete for the moment and should later on help to
   select country specific settings in case pInit is NULL.

   if pInit isn't NULL, not other settings should be done. It is assumed that
   the user knows what he programms.
*/
IFX_LOCAL IFX_int32_t initVineticTapi (VINETIC_DEVICE *pDev, IFX_uint8_t nCountry,
                                       IFX_void_t const *pInit)
{
   VINETIC_IO_INIT IoInit;
   IFX_int32_t ret;

   if (pInit == NULL)
   {
      /* reset all init pointers and init flag */
      memset (&IoInit, 0, sizeof (VINETIC_IO_INIT));
      /* use country info here if needed */
      /* set additional default flags */
      IoInit.nFlags = FW_AUTODWLD;
   }
   /* do initialization as specified in init.
      Country isn't taken in count then. */
   else
   {
      IFXOS_CPY_USR2KERN(&IoInit, pInit, sizeof (VINETIC_IO_INIT));
   }
   /* Initialize Vinetic */
   ret = VINETIC_Host_InitChip (pDev, &IoInit);

   return ret;
}

#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
/**
   function called to get Encoder value according to Codec selected
\param nCodec    Codec setected
\return
   ENC value or IFX_ERROR
*/
IFX_LOCAL IFX_char_t getEncoder (IFX_int32_t nCodec)
{
   IFX_LOCAL IFX_char_t EncoderTable[] =
   {
      (IFX_char_t)IFX_ERROR,                       /* 0 not supported */
      COD_CH_G723_63_ENC,
      COD_CH_G723_53_ENC,
       /* 3 - 5 not supported */
      (IFX_char_t)IFX_ERROR, (IFX_char_t)IFX_ERROR, (IFX_char_t)IFX_ERROR,
      COD_CH_G728_ENC,
      COD_CH_G729_ENC,
      COD_CH_G711_MLAW_ENC,
      COD_CH_G711_ALAW_ENC,
      /* 10: LINEAR16 and 11: LINEAR8 not supported */
      (IFX_char_t)IFX_ERROR, (IFX_char_t)IFX_ERROR,
      COD_CH_G726_16_ENC,
      COD_CH_G726_24_ENC,
      COD_CH_G726_32_ENC,
      COD_CH_G726_40_ENC,
      COD_CH_G729_E_ENC,
      COD_CH_ILBC_133,
      COD_CH_ILBC_152
   };

   return EncoderTable[nCodec];
}

/**

   function called to get frame length in ms according to PTE value
\param nPTE PTE value
\return
    encoder packet time in ms or IFX_ERROR
Remarks
*/
IFX_LOCAL IFX_int32_t getFrameLength (IFX_int32_t nPTE)
{
   switch (nPTE)
   {
   case COD_CH_PTE_5MS:
      return (FL_5MS);
   case COD_CH_PTE_5_5MS:
      return (FL_5_5MS);
   case COD_CH_PTE_10MS:
      return (FL_10MS);
   case COD_CH_PTE_20MS:
      return (FL_20MS);
   case COD_CH_PTE_30MS:
      return (FL_30MS);
   case COD_CH_PTE_11MS:
      return (FL_11MS);
   default:
      return IFX_ERROR;
   }
}

/**
   function called to determine the convenient PTE value according to the pre
   existing value in case this one doesn't match with the selected encoder
\param nPrePTE         pre existing PTE
\param nCodec          encoder algorithm value
\return
   IFX_SUCCESS or IFX_ERROR
Remarks
   The tables of PTE values supported per encoder look as follows :
   - G.711 Alaw         : 5ms, 10 ms, 20 ms, 5.5 ms, 11 ms
   - G.711 Mulaw        : 5ms, 10 ms, 20 ms, 5.5 ms, 11 ms
   - G.723 5.3 KBps     : 30 ms
   - G.723 6.3 KBps     : 30 ms
   - G.728              : 5ms, 10 ms, 20 ms
   - G.729_AB           : 10 ms, 20 ms
   - G.729_E            : 10 ms, 20 ms
   - G.726 16 KBps      : 5ms, 10 ms, 20 ms, 5.5 ms, 11 ms
   - G.726 24 kBps      : 5ms, 10 ms, 20 ms
   - G.726 32 KBps      : 5ms, 10 ms, 20 ms, 5.5 ms, 11 ms
   - G.726 40 KBps      : 5ms, 10 ms, 20 ms
   - ILBC  13.3 KBps    : 30 ms
   - ILBC  15.2 KBps    : 20 ms
*/
IFX_LOCAL IFX_int32_t getConvPTE (IFX_int32_t nPrePTE, IFX_int32_t nCodec)
{
   IFX_int32_t *pTemp    = NULL;
   IFX_int32_t  size, pre_time = 0, abs_time = 0, i = 0, pos = 0, nConvPte;

   /* set size */
   size = sizeof (VINETIC_PteVal [nCodec]) / sizeof (IFX_int32_t);
   /* look in vertical to determine the actual packet time */
   for (i = 1; i < IFX_TAPI_ENC_TYPE_MAX; i++)
   {
      pTemp = (IFX_int32_t *)VINETIC_PteVal [i];
      if (pTemp [nPrePTE] != PT_INVAL)
      {
         pre_time = pTemp [nPrePTE];
         break;
      }
   }
   /* find first value for comparisons */
   for (i = 0; i < size; i ++)
   {
      pTemp = (IFX_int32_t *)&VINETIC_PteVal [nCodec][i] ;
      if (*pTemp != PT_INVAL)
      {
         abs_time = ABS (*pTemp - pre_time);
         pos = i;
         break;
      }
   }
   /* loop horizontally to determine best match */
   for (i = 0; i < size; i++)
   {
      pTemp   = (IFX_int32_t *)&VINETIC_PteVal [nCodec][i];
      if (*pTemp != PT_INVAL && (abs_time > ABS (*pTemp - pre_time)))
      {
         abs_time = ABS (*pTemp - pre_time);
         pos = i;
      }
   }
   /* set new pte value */
   nConvPte = VINETIC_PteVal [0][pos];
   /* trace out some inportant info */
   TRACE (TAPI_DRV, DBG_LEVEL_HIGH, ("Not matching encoder packet time of "
         "%ld us is set now.\n\r", pre_time));
   TRACE (TAPI_DRV, DBG_LEVEL_HIGH, ("This will be replaced by the best "
         "matching encoder packet time of %ld us.\n\r",
         VINETIC_PteVal [nCodec][pos]));

   return nConvPte;
}


/**
   function called to get PTE value according to frame length in ms
\param nFrameLength    encoder packet time in ms
\return
   PTE value or IFX_ERROR
Remarks
*/
IFX_LOCAL IFX_int32_t getPTE (IFX_int32_t nFrameLength)
{
   switch (nFrameLength)
   {
   case FL_5MS:
      return (COD_CH_PTE_5MS);
   case FL_5_5MS:
      return COD_CH_PTE_5_5MS;
   case FL_10MS:
      return (COD_CH_PTE_10MS);
   case FL_20MS:
      return (COD_CH_PTE_20MS);
   case FL_30MS:
      return (COD_CH_PTE_30MS);
   case FL_11MS:
      return (COD_CH_PTE_11MS);
   default:
      return IFX_ERROR;
   }
}

/**
   function called to check the validity of the co-existence of the requested
   PTE value and encoder algorithm value
\param nFrameLength    encoder packet time (PTE) value
\param nCodec          encoder algorithm value
\return
   IFX_SUCCESS or IFX_ERROR
Remarks
   Not every encoder packet time are allowed for each encoder algorithm.
   So the setting of encoder packet time has to meet following requirements:

   - G.711 Alaw         : 5ms, 10 ms, 20 ms, 5.5 ms, 11 ms
   - G.711 Mulaw        : 5ms, 10 ms, 20 ms, 5.5 ms, 11 ms
   - G.723 5.3 Kbps     : 30 ms
   - G.723 6.3 Kbps     : 30 ms
   - G.728              : 5ms, 10 ms, 20 ms
   - G.729_AB           : 10 ms, 20 ms
   - G.729_E            : 10 ms, 20 ms
   - G.726 16 Kbps      : 5ms, 10 ms, 20 ms, 5.5 ms, 11 ms
   - G.726 24 kbps      : 5ms, 10 ms, 20 ms
   - G.726 32 Kbps      : 5ms, 10 ms, 20 ms, 5.5 ms, 11 ms
   - G.726 40 Kbps      : 5ms, 10 ms, 20 ms
*/
IFX_LOCAL IFX_int32_t CheckENCnPTE (IFX_uint8_t nFrameLength, IFX_uint8_t nCodec)
{
   IFX_int32_t ret = IFX_SUCCESS;

   switch (nFrameLength)
   {
      /* 5ms, not possible for G.729, G.723.1 and ILBCx */
      case COD_CH_PTE_5MS :
         if ((nCodec == COD_CH_G729_ENC) || (nCodec == COD_CH_G729_E_ENC)
           || (nCodec == COD_CH_G723_53_ENC) || (nCodec == COD_CH_G723_63_ENC)
           || (nCodec == COD_CH_ILBC_152) || (nCodec == COD_CH_ILBC_133))
         {
            ret = IFX_ERROR;
         }
         break;
      /* 10ms, not possible for G.723x and ILBCx */
      case COD_CH_PTE_10MS :
         if ((nCodec == COD_CH_G723_53_ENC) || (nCodec == COD_CH_G723_63_ENC)
             || (nCodec == COD_CH_ILBC_152) || (nCodec == COD_CH_ILBC_133))
         {
            ret = IFX_ERROR;
         }
         break;
      /* 20ms, not possible for G.723x and ILBC 13.3  */
      case COD_CH_PTE_20MS :
         if ((nCodec == COD_CH_G723_53_ENC) || (nCodec == COD_CH_G723_63_ENC)
             || (nCodec == COD_CH_ILBC_133))
         {
            ret = IFX_ERROR;
         }
         break;
      /* 30ms, allowed for all codecs, except ILBC 15.2 */
      case COD_CH_PTE_30MS :
         if (nCodec == COD_CH_ILBC_152)
         {
            ret = IFX_ERROR;
         }
         break;
      /* 5.5ms and 11ms, only allowed for G.711, G.726-16kbps, G.726-32kbps */
      case COD_CH_PTE_5_5MS :
      case COD_CH_PTE_11MS  :
         if ((nCodec != COD_CH_G711_ALAW_ENC) &&
             (nCodec != COD_CH_G711_MLAW_ENC) &&
             (nCodec != COD_CH_G726_16_ENC)   &&
             (nCodec != COD_CH_G726_32_ENC))
         {
            ret = IFX_ERROR;
         }
         break;
      default:
         break;
   }

   return ret;
}
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */

/* ============================= */
/* Global function definition    */
/* ============================= */

/**
   WakeUp the TAPI channel task
\param pChannel Handle to the channel specific TAPI structure
\return
   NONE
*/
IFX_void_t TAPI_WakeUp(TAPI_CONNECTION *pChannel)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   /* In polled mode, there are no sleeping tasks (blocked on select), so no
      wakeup needed */
   if ((pDev->bNeedWakeup == IFX_TRUE) &&
       (pDev->IrqPollMode & VIN_TAPI_WAKEUP)
      )
   {
      /* don't wake up any more */
      pDev->bNeedWakeup = IFX_FALSE;
      IFXOS_WakeUp (pDev->EventList, IFXOS_READQ);
   }
}

/**
   Low Level initialization function. This function is called from the
   TAPI init function.
\param pChannel Handle to TAPI_CONNECTION structure
\param pInit    Initializations flags
\return Return value according to IFX_return_t
      - IFX_ERROR if an error occured
      - IFX_SUCCESS if successful
\remarks
   Performs all neccesary actions to provide a fully operating system including
   firmware download and initialization. Althoug this function is called for
   each channel the device initialization must also be made sure within this
   function.
   Depending on the underlying device the default mode is mapped to the best
   possible initialization covering all features of the device.
   For VINETIC-S the default mode is IFX_TAPI_INIT_MODE_PCM_PHONE,
   for VINETIC-C the default mode is IFX_TAPI_INIT_MODE_PCM_DSP,
   for VINETIC-VIP and M the default mode is IFX_TAPI_INIT_MODE_VOICE_CODER. */
IFX_return_t TAPI_LL_Phone_Init(TAPI_CONNECTION *pChannel, IFX_TAPI_CH_INIT_t const *pInit)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t ret = IFX_SUCCESS;
   IFX_uint8_t mode = IFX_TAPI_INIT_MODE_NONE;
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   IFX_TAPI_PKT_RTP_CFG_t rtpConf;
   IFX_TAPI_JB_CFG_t jbConf;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */

   /* check if device initialization was done already   */
   if (!(pDev->nDevState & DS_DEV_INIT))
   {
      /* initialize chip one time */
      ret = initVineticTapi (pDev, pInit->nCountry, pInit->pProc);
   }
   if (!(pDev->nDevState & DS_DEV_INIT))
   {
      return IFX_ERROR;
   }
   /* check if tapi was previously initialized */
   else if (!(pDev->nDevState & DS_TAPI_INIT))
   {
      /* set Tapi max Channel */
      ret = AddCaps(pDev);
   }
   if (ret == IFX_SUCCESS)
   {
      /* set tapi init flag to prevent non tapi driver functions */
      pDev->nDevState |= DS_TAPI_INIT;
   }
   if (pInit->nMode != IFX_TAPI_INIT_MODE_NONE)
   {
      switch (pDev->nChipType)
      {
         case VINETIC_TYPE_C:
            /* error checking for C. No coder available and default must be PCM */
            switch (pInit->nMode)
            {
               case IFX_TAPI_INIT_MODE_VOICE_CODER:
                  SET_ERROR (ERR_FUNC_PARM);
                  break;
               case IFX_TAPI_INIT_MODE_DEFAULT:
                  mode = IFX_TAPI_INIT_MODE_PCM_DSP;
                  break;
               default:
                  mode = pInit->nMode;
                  break;
            }
            break;
         case VINETIC_TYPE_S:
            switch (pInit->nMode)
            {
               /* error checking for S. No DSP and coder available, only one
                  default possible */
               case IFX_TAPI_INIT_MODE_PCM_DSP:
               case IFX_TAPI_INIT_MODE_VOICE_CODER:
                  SET_ERROR (ERR_FUNC_PARM);
                  break;
               default:
                  mode = IFX_TAPI_INIT_MODE_PCM_PHONE;
                  break;
            }
            break;
         default:
            mode = pInit->nMode;
            break;
      }
   }

   /* Do appropriate initialization for channel */
   if (ret == IFX_SUCCESS)
   {
      switch (mode)
      {
      case IFX_TAPI_INIT_MODE_DEFAULT:
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
      case IFX_TAPI_INIT_MODE_VOICE_CODER:

         ret = Basic_VoIPConf (pCh);

         if (ret == IFX_SUCCESS &&
            (pDev->nEdspVers[0] & ECMD_VERS_EDSP_PRT) == ECMD_VERS_EDSP_PRT_RTP &&
            pChannel->nChannel < pDev->nCoderCnt)
         {
            memset (&rtpConf, 0, sizeof (IFX_TAPI_PKT_RTP_CFG_t));
            rtpConf.nEventPT = pChannel->nEvtPT;
            rtpConf.nEvents = IFX_TAPI_PKT_EV_OOB_ALL;

            ret = TAPI_LL_Phone_RtpConf (pChannel, &rtpConf);
         }
         if (ret == IFX_SUCCESS && pChannel->nChannel < pDev->nCoderCnt)
         {
            memset (&jbConf, 0, sizeof (IFX_TAPI_JB_CFG_t));
            jbConf.nInitialSize = 0x02D0;
            jbConf.nMinSize = 0x0050;
            jbConf.nMaxSize = 0x05A0;
            jbConf.nPckAdpt = IFX_TAPI_JB_PKT_ADAPT_VOICE;
            jbConf.nJbType = IFX_TAPI_JB_TYPE_ADAPTIVE;
            jbConf.nScaling = 0x16;

            ret = TAPI_LL_Phone_JB_Set (pChannel, &jbConf);
         }
         if (ret == IFX_SUCCESS && pChannel->nChannel < pDev->nSigCnt)
         {
            /* enable default DTMF receiver */
            IFX_TAPI_SIG_DETECTION_t sig;
            sig.sig = IFX_TAPI_SIG_DTMFTX;

            ret = TAPI_LL_Enable_Signal (pChannel, &sig);
         }
#ifdef QOS_SUPPORT
         if ((ret == IFX_SUCCESS) && !(pDev->nDevState & DS_QOS_INIT))
         {
            ret = Qos_Init ((IFX_uint32_t) pDev);
         }
#endif /* QOS_SUPPORT */
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
         if (ret == IFX_SUCCESS)
            pCh->nFlags |= CF_TAPI_VOICEMODE;
#ifdef VIN_POLL
         /** If the polling implementation is used, add this device to the
             polling list */
         if (ret == IFX_SUCCESS)
            ret = VINETIC_IrqPollDevAddPoll (pDev);
            /** If the polling implementation is used, add this device to the
                event list */
         if (ret == IFX_SUCCESS)
            ret = VINETIC_IrqPollDevAddEvent (pDev);
#endif /* VIN_POLL*/
         break;
      case IFX_TAPI_INIT_MODE_PCM_DSP:
         ret = Basic_PcmConf (pCh, mode);
         if (ret == IFX_SUCCESS)
            pCh->nFlags |= CF_TAPI_PCMMODE;
         break;
      case IFX_TAPI_INIT_MODE_PCM_PHONE :
         ret = Basic_PcmConf (pCh, mode);
         if (ret == IFX_SUCCESS)
            pCh->nFlags |= CF_TAPI_S_PCMMODE;
#ifdef VIN_POLL
         /** If the polling implementation is used, add this device to the
             event list */
         if (ret == IFX_SUCCESS)
            ret = VINETIC_IrqPollDevAddEvent (pDev);
#endif /* VIN_POLL */
         break;
      default:
         /* no specific initialization */
         break;
      }
   }
   /* initialize needed timers */
   if (ret == IFX_SUCCESS)
      ret = VINETIC_InitChTimers(pCh);

   return ret;
}

/**
   Prepare parameters and call the Target Configuration Function to activate/
   deactivate the pcm interface.
\param pChannel   Handle to TAPI_CONNECTION structure
\param nMode  Activation mode
   -1: timeslot activated
   -0: timeslot deactivated
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   The configuration must be done previously with the low level function
   TAPI_LL_Phone_PCM_Activation.
   Resource availability check is done. The function returns with error when
   the resource or the timeslot is not available.
*/
IFX_return_t TAPI_LL_Phone_PCM_Activation(TAPI_CONNECTION *pChannel,
                                         IFX_uint32_t nMode)
{
   IFX_uint32_t      IdxRx, BitValueRx;
   IFX_uint32_t      IdxTx, BitValueTx;
   VINETIC_DEVICE    *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   FWM_PCM_CH        *pPcmCh;
   IFX_TAPI_PCM_CFG_t   *pPcmCfg;
   IFX_int32_t       ret = IFX_SUCCESS;

   if (pChannel->nChannel >= pDev->nPcmCnt)
   {
      ret = IFX_ERROR;
      SET_ERROR (ERR_NORESOURCE);
      goto end_fkt;
   }

   pPcmCh = &pDev->pPcmCh[pChannel->nChannel].pcm_ch;
   pPcmCfg = &pChannel->TapiPCMData.PCMConfig;

   /* Activate Timeslots and set highway */
   switch (nMode)
   {
   case  0:
      /*
      the device variables PcmRxTs, PcmTxTs are global for all
      channels and must be protected against concurent tasks access
      */
      IFXOS_MutexLock (pDev->memberAcc);
      switch (pPcmCh->bit.cod)
      {
      case 0: /* IFX_TAPI_PCM_RES_LINEAR_16BIT */
         /* get the index in the time slot array. Each bit reflects one time
         slot. Modulo 32 separate the index of the array and the bit inside
         the field. */
         /* for 16 Bit mode, the next higher time slot has to be reserved too */
         IdxRx = (pPcmCh->bit.rts + 1) >> 5;
         BitValueRx = 1 << ((pPcmCh->bit.rts + 1) & 0x1F);
         IdxTx = (pPcmCh->bit.xts + 1) >> 5;
         BitValueTx = 1 << ((pPcmCh->bit.xts + 1) & 0x1F);

         /* Receive Slot checking */
         if ((pDev->PcmRxTs[pPcmCh->bit.r_hw][IdxRx] & BitValueRx) == IFX_FALSE)
         {
            IFXOS_MutexUnlock (pDev->memberAcc);
            LOG(TAPI_DRV,DBG_LEVEL_HIGH,
               ("\n\rDRV_ERROR: Rx Slot is not available\n\r"));
            ret = IFX_ERROR;
            SET_ERROR (ERR_NORESOURCE);
            goto end_fkt;
         }

         if ((pDev->PcmTxTs[pPcmCh->bit.x_hw][IdxTx] & BitValueTx) == IFX_FALSE)
         {
            IFXOS_MutexUnlock (pDev->memberAcc);
            LOG(TAPI_DRV,DBG_LEVEL_HIGH,
               ("\n\rDRV_ERROR: Tx Slot is not available\n\r"));
            ret = IFX_ERROR;
            SET_ERROR (ERR_NORESOURCE);
            goto end_fkt;
         }

         /* reserve the time slots now, because the parameter are OK */
         pDev->PcmRxTs[pPcmCh->bit.r_hw][IdxRx]  &= ~BitValueRx;
         pDev->PcmTxTs[pPcmCh->bit.x_hw][IdxTx] &= ~BitValueTx;
         /* Fall through and do not 'break'!
         For 16-Bit PCM Mode, the next higher PCM time slot was reserved
         and now the used time slot will be reserved.
         That part is similar to the 8-Bit PCM Mode */
         /* break; */
         /*lint -fallthrough */
      case 2: /* IFX_TAPI_PCM_RES_ALAW_8BIT */
      case 3: /* IFX_TAPI_PCM_RES_ULAW_8BIT */
      default:
         /* get the index in the time slot array. Each bit reflects one time
         slot. Modulo 32 separate the index of the array and the bit inside
         the field. */
         IdxRx = pPcmCh->bit.rts >> 5;
         BitValueRx = 1 << (pPcmCh->bit.rts & 0x1F);
         IdxTx = pPcmCh->bit.xts >> 5;
         BitValueTx = 1 << (pPcmCh->bit.xts & 0x1F);

         /* Receive Slot checking */
         if ((pDev->PcmRxTs[pPcmCh->bit.r_hw][IdxRx] & BitValueRx) == IFX_FALSE)
         {
            IFXOS_MutexUnlock (pDev->memberAcc);
            LOG(TAPI_DRV,DBG_LEVEL_HIGH,
               ("\n\rDRV_ERROR: Rx Slot is not available\n\r"));
            ret = IFX_ERROR;
            SET_ERROR (ERR_NORESOURCE);
            goto end_fkt;
         }

         if ((pDev->PcmTxTs[pPcmCh->bit.x_hw][IdxTx] & BitValueTx) == IFX_FALSE)
         {
            IFXOS_MutexUnlock (pDev->memberAcc);
            LOG(TAPI_DRV,DBG_LEVEL_HIGH,
               ("\n\rDRV_ERROR: Tx Slot is not available\n\r"));
            ret = IFX_ERROR;
            SET_ERROR (ERR_NORESOURCE);
            goto end_fkt;
         }

         /* reserve the time slots now, because the parameter are OK */
         pDev->PcmRxTs[pPcmCh->bit.r_hw][IdxRx]  &= ~BitValueRx;
         pDev->PcmTxTs[pPcmCh->bit.x_hw][IdxTx] &= ~BitValueTx;
         /* Fall through and do not 'break'!
         For 16-Bit PCM Mode, the next higher PCM time slot was reserved
         and now the used time slot will be reserved.
         That part is similar to the 8-Bit PCM Mode */
         break;
      }
      /* disable channel: This will deactivate the timeslots */
      pPcmCh->bit.en = 0;
      /* write coefficients back */
      ret = CmdWrite (pDev, pPcmCh->value, CMD_PCM_CH_LEN);
      /* release share variables lock */
      IFXOS_MutexUnlock (pDev->memberAcc);
      break;
   case 1:
      /*
      the device variables PcmRxTs, PcmTxTs are global for all
      channels and must be protected against concurent tasks access
      */
      IFXOS_MutexLock (pDev->memberAcc);
#ifdef VIN_V14_SUPPORT
      /* The VINETIC-1.x has a limitation here. When 16-Bit mode, every second
         PCM channel is blocked. During driver initialization, every second
         PCM channel is assigned to the first half of the TAPI
         channel (0,2,4,6,...). The other PCM channel are assigned to the second
         half of the TAPI channel (1,3,5,7...). The first half are allowed to
         support 16-Bit mode if no PCM channel has been configured in 8-Bit
         before. In 16-Bit mode the upper half of TAPI channel can not be used.
      */
      if (pDev->nChipMajorRev == VINETIC_V1x)
      {
         VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];

         if (pDev->nPcmResolution == 0)
         {
            /* first time PCM channel gets enabled for this device . Set the
               running mode (8-Bit or 16-Bit). In case of 16-Bit mode block
               the upper half of TAPI channels to use for PCM. */
            if (pPcmCfg->nResolution == IFX_TAPI_PCM_RES_LINEAR_16BIT)
               pDev->nPcmResolution = 16;
            else
               pDev->nPcmResolution = 8;
         }

         if (((pDev->nPcmResolution == 16) &&
            (pPcmCfg->nResolution != IFX_TAPI_PCM_RES_LINEAR_16BIT)) ||
             ((pDev->nPcmResolution == 8) &&
             pPcmCfg->nResolution == IFX_TAPI_PCM_RES_LINEAR_16BIT))
         {
            /* this mode is not supported. A resolution that is configured once
               can not be changed anymore, because it could conflict wiht all
               previous configured PCM channels.
            */
            IFXOS_MutexUnlock (pDev->memberAcc);
            LOG (TAPI_DRV, DBG_LEVEL_HIGH,
                 ("\n\rDRV_ERROR: Previous PCM resolution conflict\n\r"));
            ret = IFX_ERROR;
            SET_ERROR (ERR_FUNC_PARM);
            goto end_fkt;
         }

         if ((pPcmCfg->nResolution == IFX_TAPI_PCM_RES_LINEAR_16BIT) &&
            (pCh->nPcmMaxResolution < 16))
         {
            /* This PCM channel does not support to run with 16-Bit resolution,
               because it might be one of the upper TAPI channels. These
               channels are supposed to only support 8-Bit mode. The lower half
               of the TAPI channel should support 16-Bit mode.
            */
            IFXOS_MutexUnlock (pDev->memberAcc);
            LOG (TAPI_DRV, DBG_LEVEL_HIGH,
                 ("\n\rDRV_ERROR: PCM 16-Bit resolution not supported\n\r"));
            ret = IFX_ERROR;
            SET_ERROR (ERR_NOTSUPPORTED);
            goto end_fkt;
         }
      }
#endif /* VIN_V14_SUPPORT */
      /* check timeslot allocation */
      switch (pPcmCfg->nResolution)
      {
      case IFX_TAPI_PCM_RES_LINEAR_16BIT:
         /* get the index in the time slot array. Each bit reflects one time
         slot. Modulo 32 separate the index of the array and the bit inside
         the field. */
         /* for 16 Bit mode, the next higher time slot has to be reserved too */
         IdxRx = (pPcmCfg->nTimeslotRX + 1) >> 5;
         BitValueRx = 1 << ((pPcmCfg->nTimeslotRX + 1) & 0x1F);
         IdxTx = (pPcmCfg->nTimeslotTX + 1) >> 5;
         BitValueTx = 1 << ((pPcmCfg->nTimeslotTX + 1) & 0x1F);

         /* Receive Slot checking */
         if (pDev->PcmRxTs[pPcmCfg->nHighway][IdxRx] & BitValueRx)
         {
            IFXOS_MutexUnlock (pDev->memberAcc);
            LOG(TAPI_DRV,DBG_LEVEL_HIGH,
               ("\n\rDRV_ERROR: Rx Slot is not available\n\r"));
            ret = IFX_ERROR;
            SET_ERROR (ERR_NORESOURCE);
            goto end_fkt;
         }

         if (pDev->PcmTxTs[pPcmCfg->nHighway][IdxTx] & BitValueTx)
         {
            IFXOS_MutexUnlock (pDev->memberAcc);
            LOG(TAPI_DRV,DBG_LEVEL_HIGH,
               ("\n\rDRV_ERROR: Tx Slot is not available\n\r"));
            ret = IFX_ERROR;
            SET_ERROR (ERR_NORESOURCE);
            goto end_fkt;
         }

         /* reserve the time slots now, because the parameter are OK */
         pDev->PcmRxTs[pPcmCfg->nHighway][IdxRx] |= BitValueRx;
         pDev->PcmTxTs[pPcmCfg->nHighway][IdxTx] |= BitValueTx;

         /* Fall through and do not 'break'!
         For 16-Bit PCM Mode, the next higher PCM time slot was reserved
         and now the used time slot will be reserved.
         That part is similar to the 8-Bit PCM Mode */
         /* break; */
         /*lint -fallthrough */
      case IFX_TAPI_PCM_RES_ALAW_8BIT:
      case IFX_TAPI_PCM_RES_ULAW_8BIT:
         /* get the index in the time slot array. Each bit reflects one time
         slot. Modulo 32 separate the index of the array and the bit inside
         the field. */
         IdxRx = pPcmCfg->nTimeslotRX >> 5;
         BitValueRx = 1 << (pPcmCfg->nTimeslotRX & 0x1F);
         IdxTx = pPcmCfg->nTimeslotTX >> 5;
         BitValueTx = 1 << (pPcmCfg->nTimeslotTX & 0x1F);

         /* Receive Slot checking */
         if (pDev->PcmRxTs[pPcmCfg->nHighway][IdxRx] & BitValueRx)
         {
            IFXOS_MutexUnlock (pDev->memberAcc);
            LOG(TAPI_DRV,DBG_LEVEL_HIGH,
               ("\n\rDRV_ERROR: Rx Slot is not available\n\r"));
            ret = IFX_ERROR;
            SET_ERROR (ERR_NORESOURCE);
            goto end_fkt;
         }

         if (pDev->PcmTxTs[pPcmCfg->nHighway][IdxTx] & BitValueTx)
         {
            IFXOS_MutexUnlock (pDev->memberAcc);
            LOG(TAPI_DRV,DBG_LEVEL_HIGH,
               ("\n\rDRV_ERROR: Tx Slot is not available\n\r"));
            ret = IFX_ERROR;
            SET_ERROR (ERR_NORESOURCE);
            goto end_fkt;
         }

         /* reserve the time slots now, because the parameter are OK */
         pDev->PcmRxTs[pPcmCfg->nHighway][IdxRx] |= BitValueRx;
         pDev->PcmTxTs[pPcmCfg->nHighway][IdxTx] |= BitValueTx;

         break;
      default:
         LOG(TAPI_DRV,DBG_LEVEL_HIGH,
            ("\n\rDRV_ERROR: This resolution is unknown\n\r"));
         IFXOS_MutexUnlock (pDev->memberAcc);
         ret = IFX_ERROR;
         SET_ERROR (ERR_NOTSUPPORTED);
         goto end_fkt;
      }
      /* activate Timeslots */
      switch (pPcmCfg->nResolution)
      {
      case  IFX_TAPI_PCM_RES_ALAW_8BIT:
         pPcmCh->bit.cod = 2;
         break;
      case IFX_TAPI_PCM_RES_ULAW_8BIT:
         pPcmCh->bit.cod = 3;
         break;
      case IFX_TAPI_PCM_RES_LINEAR_16BIT:
      default:
         pPcmCh->bit.cod = 0;
         break;
      }
      pPcmCh->bit.xts  = pPcmCfg->nTimeslotTX;
      pPcmCh->bit.rts  = pPcmCfg->nTimeslotRX;
      pPcmCh->bit.r_hw = pPcmCfg->nHighway;
      pPcmCh->bit.x_hw = pPcmCfg->nHighway;
      pPcmCh->bit.en = 1;

      ret = CmdWrite (pDev, pPcmCh->value, CMD_PCM_CH_LEN);
      IFXOS_MutexUnlock (pDev->memberAcc);
      break;
   default:
      ret = IFX_ERROR;
      goto end_fkt;
      /* break; */
   }

end_fkt:
   /* Check Error */
   if (ret == IFX_ERROR)
   {
      LOG(TAPI_DRV,DBG_LEVEL_HIGH,
          ("DRV_ERROR: Error in (de)activating PCM Module!\n\r"));
   }

   return ret;
}

/**
   Prepare parameters and call the target configuration function to configure
   the PCM interface.
\param pChannel Handle to TAPI_CONNECTION structure
\param pPCMConfig Contains the new configuration for PCM interface
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   Performs error checking according to the underlying device capability.
*/
IFX_return_t TAPI_LL_Phone_PCM_Config (TAPI_CONNECTION *pChannel,
                                      IFX_TAPI_PCM_CFG_t const *pPCMConfig)
{
   IFX_int32_t       ret = IFX_SUCCESS;
   VINETIC_DEVICE    *pDev   = (VINETIC_DEVICE *)pChannel->pDevice;

   if ((pDev->nDevState & DS_PCM_EN) == IFX_FALSE)
   {
      LOG(TAPI_DRV,DBG_LEVEL_HIGH,
           ("DRV_ERROR: PCM Module not enabled during Initialization!\n\r"));
      ret = IFX_ERROR;
   }

   if (ret == IFX_SUCCESS)
   {
      /* check PCM resolution for supported values */
      switch (pPCMConfig->nResolution)
      {
      case IFX_TAPI_PCM_RES_ALAW_8BIT:
      case IFX_TAPI_PCM_RES_ULAW_8BIT:
      case IFX_TAPI_PCM_RES_LINEAR_16BIT:
         break;
      default:
         ret = IFX_ERROR;
         break;
      }
   }

   if ((pPCMConfig->nTimeslotRX >= PCM_MAX_TS) ||
      (pPCMConfig->nTimeslotTX >= PCM_MAX_TS))
   {
      TRACE (TAPI_DRV,DBG_LEVEL_HIGH,
         ("\n\rDRV_ERROR: Number of PCM timeslot out of range\n\r"));
      ret = IFX_ERROR;
   }

   if (pPCMConfig->nHighway >= PCM_HIGHWAY)
   {
      TRACE (TAPI_DRV,DBG_LEVEL_HIGH,
         ("\n\rDRV_ERROR: Number of PCM Highway %d out of range\n\r",
         (int)pPCMConfig->nHighway));
      ret = IFX_ERROR;
   }

   return ret;
}

/**
   Configures predefined tones of Index 1 to 31
\param pToneTable  Handle to COMPLEX_TONE structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   Not a low level function
   Constant Table VINETIC_PredefinedToneFreq defines the frequencies used for
   the predefined tones in index range 1 to 31. In case this table is changed
   in the future, this function must be adapted accordingly.
   The actual table status is :
      - Either Freq2 > Freq 1 => DTMF,
      - or Freq2 = 0 (e.g CNG tone)
\todo This is not a low level function. The frequencies are not
      related to the device.
*/
IFX_return_t TAPI_LL_Phone_Tone_Predef_Config (COMPLEX_TONE* pToneTable)
{
   IFX_TAPI_TONE_SIMPLE_t predef_tone;
   IFX_uint32_t i;

   /* setup tone structure for predefined tone */
   memset (&predef_tone, 0, sizeof (predef_tone));
   /* add all predefined tones to tone table */
   for (i = 0; i < (IFX_TAPI_TONE_INDEX_MIN - 1); i++)
   {
      /* read frequencies from contant table */
      predef_tone.freqA       = VINETIC_PredefinedToneFreq[i][0];
      predef_tone.freqB       = VINETIC_PredefinedToneFreq[i][1];
      /* tone indexes start from 1 on */
      predef_tone.index       = i + 1 ;
      /* set frequencies to play and levels */
      switch (predef_tone.freqB)
      {
         case  0:
            /* play only freqA */
            predef_tone.frequencies [0] = IFX_TAPI_TONE_FREQA;
            /* -9 dB = -90 x 0.1 dB for the higher frequency */
            predef_tone.levelA          = -90;
            predef_tone.levelB          = 0;
            break;
         default:
            /* In this case, freqB > freqA according to table
               VINETIC_PredefinedToneFreq : Play both frequencies as dtmf  */
            predef_tone.frequencies [0] = (IFX_TAPI_TONE_FREQA |
                                           IFX_TAPI_TONE_FREQB);
            /* -11 dB = -110 x 0.1 dB for the lower frequency ,
               -9  dB = -90  * 0.1 dB for the higher frequency
                        (to attenuate the higher tx loss) */
            predef_tone.levelA          = -110;
            predef_tone.levelB          = -90;
            break;
      }
      /* set cadences and loop */
      switch (predef_tone.index)
      {
         /* play CNG and CED tone continuously */
         case 15:
         case 17:
         case 22:
            predef_tone.loop   = 0;
            break;
         /* Special case for tone 25 (dialtone), tone 26 (ringback tone) and
         tone 27 (busy tone) */
         case 25:
         case 26:
         case 27:
            /* tone played continuously */
            predef_tone.loop = 0;
            if (predef_tone.index == 26)
            {
               predef_tone.cadence [0] = 1000;
               predef_tone.cadence [1] = 4000;
            }
            else if (predef_tone.index == 27)
            {
               predef_tone.cadence [0] = 500;
               predef_tone.cadence [1] = 500;
            }
            break;
         default:
            predef_tone.cadence [0] = 100;
            predef_tone.cadence [1] = 0;
            /* tone played only once */
            predef_tone.loop        = 1;
            break;
      }
      if (TAPI_Phone_Add_SimpleTone(pToneTable, &predef_tone) == IFX_ERROR)
      {
         TRACE (TAPI_DRV, DBG_LEVEL_HIGH, ("Predefined tone configuration"
                " failed for tone index %d\n\r", predef_tone.index));
         return IFX_ERROR;
      }
   }

   return IFX_SUCCESS;
}

/**
   Play a tone with the given tone definition
\param pChannel  Handle to TAPI_CONNECTION structure
\param res       Resource number which is used for playing the tone. The
                 available resources is device dependend. Per device there
                 may be multiple resources to play a tone. If the resource
                 number increases the amout of available resources this
                 function returns an error. Usually res is set to zero.
\param pToneCoeff  Handle to the tone definition to play
\param dst         Destination where to play the tone: local or network
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   This function handles all necessary steps to play out the complete
   tone sequence. It returns immediately.
*/
IFX_return_t TAPI_LL_Phone_Tone_Play (TAPI_CONNECTION *pChannel,
                                     IFX_uint8_t res,
                                     COMPLEX_TONE const *pToneCoeff,
                                     TAPI_TONE_DST dst)
{
   IFX_int32_t ret = IFX_SUCCESS, src = IFX_TAPI_TONE_SRC_DSP;
   IFX_uint8_t ch = pChannel->nChannel;
   TAPI_DEV *pTapiDev = pChannel->pTapiDevice;
   VINETIC_DEVICE   *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   TAPI_OPCONTROL_DATA const *pOpControl = &pChannel->TapiOpControlData;
   IFX_TAPI_TONE_SIMPLE_t const *pToneSimple = NULL;
#ifndef VIN_2CPE
   VINETIC_CHANNEL *pCh = &pDev->pChannel[ch];
#endif /* VIN_2CPE */

#ifdef VIN_2CPE
   /* check for valid resource number */
   if ( res >= pDev->nToneGenCnt)
   {
      SET_ERROR (ERR_NORESOURCE);
      return IFX_ERROR;
   }
#endif /* VIN_2CPE */
   if (pToneCoeff->type != TAPI_TONE_TYPE_PREDEF)
   {
      pToneSimple = (IFX_TAPI_TONE_SIMPLE_t const *)&pToneCoeff->tone.simple;
      /* set source where the tone is played out. Check default and set it
         explicitly here */
      if (pDev->nChipType == VINETIC_TYPE_S)
      {
         if (dst == TAPI_TONE_DST_NET)
         {
            SET_ERROR (ERR_FUNC_PARM);
            return IFX_ERROR;
         }
         src = IFX_TAPI_TONE_SRC_TG;
      }
      else
      {
         if ( (res != 0) && (pDev->VinCapabl.nUtgPerCh == 1))
            src = IFX_TAPI_TONE_SRC_TG;
         else
         {
            src = IFX_TAPI_TONE_SRC_DSP;
         }
      }
   }
   pChannel->TapiComplexToneData[res].src = src;
   switch (pToneCoeff->type)
   {
      case  TAPI_TONE_TYPE_PREDEF:
         /* check if line is in active state */
         if (!(pOpControl->nLineMode == IFX_TAPI_LINE_FEED_ACTIVE ||
               pOpControl->nLineMode   == IFX_TAPI_LINE_FEED_ACTIVE_REV ||
               pOpControl->nLineMode  == IFX_TAPI_LINE_FEED_NORMAL_AUTO ||
               pOpControl->nLineMode   == IFX_TAPI_LINE_FEED_REVERSED_AUTO))
         {
            ret = TAPI_Phone_Set_Linefeed(pChannel, IFX_TAPI_LINE_FEED_ACTIVE);
         }

         if (ret == IFX_SUCCESS)
         {
            /* turn tone on and start timer */
            ret = TAPI_LL_Host_Phone_Tone_On(pChannel, pToneCoeff->tone.nPredefined);
         }
      break;

      case TAPI_TONE_TYPE_COMP:
         /* Set the first simple for composed tones and continue with simple
            tone handling */
         pToneSimple = &(pTapiDev->pToneTbl
                         [pToneCoeff->tone.composed.tones[0]].tone.simple);

         /*lint -fallthrough */
      case TAPI_TONE_TYPE_SIMPLE:
         if (pToneSimple == IFX_NULL)
         {
            SET_ERROR (ERR_UNKNOWN);
            return IFX_ERROR;
         }
         switch (src)
         {
            default:
            case  IFX_TAPI_TONE_SRC_DEFAULT:
            case  IFX_TAPI_TONE_SRC_DSP:
               IFXOS_MutexLock (pDev->memberAcc);
               ret = Dsp_SigActStatus (pDev, pChannel->nChannel,
                                       IFX_TRUE, CmdWrite);
#ifndef FW_ETU
               /* disable DTMF / AT generator because it is overlaid
                  with the UTG */
               if (ret == IFX_SUCCESS &&
                   pDev->pSigCh[ch].sig_dtmfgen.bit.en != 0)
               {
                  pDev->pSigCh[ch].sig_dtmfgen.bit.en = 0;
                  ret = CmdWrite (pDev,
                                  pDev->pSigCh[ch].sig_dtmfgen.value,
                                  CMD_SIG_DTMFGEN_LEN);
               }
#endif /* FW_ETU */
               /* Activate the universal tone generator disable the DTMF generator
                  and program the simple tone sequence */
               if (ret == IFX_SUCCESS)
               {
                  ret = Tone_UTG_Start(pChannel, pToneSimple, dst, res);
               }
               IFXOS_MutexUnlock (pDev->memberAcc);
               break;

            case  IFX_TAPI_TONE_SRC_TG:
#ifdef VIN_2CPE /* No ALM tone generator for 2CPE */
               TRACE (TAPI_DRV, DBG_LEVEL_HIGH,
                      ("IFX_TAPI_TONE_SRC_TG not supported for 2CPE\n\r"));
               ret = IFX_ERROR;
#else /* VIN_2CPE */
               /* play tone on ALM. Switch it on and start the timer */
               ret = Tone_TG_Play (pCh);
               if (ret == IFX_SUCCESS)
               {
                  /* we play the first step */
                  pCh->nToneStep = 1;
                  /* set tone timer */
                  TAPI_SetTime_Timer(pCh->pToneRes[res].Tone_Timer,
                                     pToneSimple->cadence[0], IFX_FALSE, IFX_TRUE);
               }
#endif /* VIN_2CPE */
               break;
         }
         break;
      default:
         ret = IFX_ERROR;
         SET_ERROR (ERR_FUNC_PARM);
         break;
   }

   return ret;
}

/**
   Stop playing a tone of given definition immediately
\param pChannel  Handle to TAPI_CONNECTION structure
\param res       Resource number which is used for playing the tone. The
                 available resources is device dependend. Per device there
                 may be multiple resources to play a tone. If the resource
                 number increases the amout of available resources this
                 function returns an error. Usually res is set to zero.
\param pToneCoeff  Handle to the tone definition to play
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
*/
IFX_return_t TAPI_LL_Phone_Tone_Stop (TAPI_CONNECTION *pChannel,
                                     IFX_uint8_t res,
                                     COMPLEX_TONE const *pToneCoeff)
{
   VINETIC_DEVICE  *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t ret, src;

   /* Stop any voice path teardowm timer that may be running */
   TAPI_Stop_Timer (pCh->pToneRes[res].Tone_Timer);
   if (pToneCoeff->type == TAPI_TONE_TYPE_PREDEF)
   {
      if (pChannel->TapiTgToneData.nToneState != TAPI_CT_IDLE)
         /* turn tone off and stop timer */
         return TAPI_LL_Host_Phone_Tone_Off(pChannel);
      else
         return IFX_ERROR;
   }
   else
   {
      /* check for valid resource number */
      if (pDev->nToneGenCnt < res)
      {
         SET_ERROR (ERR_NORESOURCE);
         return IFX_ERROR;
      }
      /* set source where the tone is played out. Check default and set it
         explicitly here */
      if (pDev->nChipType == VINETIC_TYPE_S)
      {
         src = IFX_TAPI_TONE_SRC_TG;
      }
      else
      {
         if (res == 0)
            src = IFX_TAPI_TONE_SRC_DSP;
#ifdef  VIN_2CPE
         else if ( (res != 0) && (pDev->nChipType == VINETIC_TYPE_2CPE) )
         {  /* CPE: the second resource (if available) is also an utg */
            src = IFX_TAPI_TONE_SRC_DSP;
         }
#endif   /* VIN_2CPE */
         else
            src = IFX_TAPI_TONE_SRC_TG;
      }
      if (src == IFX_TAPI_TONE_SRC_DSP)
      {
         IFXOS_MutexLock (pDev->memberAcc);
         ret = Dsp_Utg_Stop (pCh, res);
         if (ret == IFX_SUCCESS)
            ret = Dsp_SigActStatus (pDev, pChannel->nChannel,
                                    IFX_FALSE, CmdWrite);
         IFXOS_MutexUnlock (pDev->memberAcc);
      }
      else
      {
#ifdef VIN_2CPE
         TRACE (TAPI_DRV, DBG_LEVEL_HIGH,
                ("IFX_TAPI_TONE_SRC_TG not supported for 2CPE\n\r"));
         ret = IFX_ERROR;
#else /* VIN_2CPE */
         TAPI_Stop_Timer (pCh->pToneRes[res].Tone_Timer);
         /* call target function to stop tone generator(s) */
         DSCR.value &= ~(DSCR_COR8 | DSCR_PTG | DSCR_TG1_EN | DSCR_TG2_EN);
         ret = wrReg (pCh, DSCR_REG);
         if (ret == IFX_SUCCESS)
         {
            /* no tone is played */
            pChannel->TapiTgToneData.nToneState = TAPI_CT_IDLE;
            /* reset variable */
            pChannel->TapiTgToneData.bUseSecondTG = IFX_FALSE;
         }
#endif /* VIN_2CPE */
      }
   }

   return ret;
}

#ifdef TAPI_DTMF
/**
   Translate Chip Specific Digit to Tapi Digit
\param pDev            Handle to TAPI_CONNECTION structure
\param nDtmfDigit      detected digit
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   This function is called in the VINETIC Interrupt Routine when a Digit is
   detected!
*/
IFX_void_t VINETIC_Tapi_Event_Dtmf (TAPI_CONNECTION *pChannel,
                                    IFX_uint8_t nDtmfDigit)
{
   IFX_uint8_t nTapiDigit;

   switch (nDtmfDigit)
   {
   case 0x00:
      nTapiDigit = 0x0B;
      break;                    /* '0' */
   case 0x0B:
      nTapiDigit = 0x0C;
      break;                    /* '#' */
   case 0x0C:
      nTapiDigit = 0x1C;
      break;                    /* 'A' */
   case 0x0D:
      nTapiDigit = 0x1D;
      break;                    /* 'B' */
   case 0x0E:
      nTapiDigit = 0x1E;
      break;                    /* 'C' */
   case 0x0F:
      nTapiDigit = 0x1F;
      break;                    /* 'D' */
   default:
      nTapiDigit = nDtmfDigit;
      break;                    /* 1 - 9 */
   }

   TAPI_Phone_Event_DTMF (pChannel, nTapiDigit);
}
#endif /* TAPI_DTMF */

#ifdef TAPI_CID

/**
   This function converts the given TAPI DTMF code to the device DTMF code
\param nChar       Digit to encode
\param pDtmfCode   Pointer to return DTMF code
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   This function is used by the DTMF CID implementation to store the device
   DTMF codes in the playout buffer.
   For the vinetic firmware, following caracters are coded as follows:
       '*' = 0x2A (ASCII) = 0x0A (VINETIC)
       '#' = 0x23 (ASCII) = 0x0B (VINETIC)
       'A' = 0x41 (ASCII) = 0x0C (VINETIC)
       'B' = 0x42 (ASCII) = 0x0D (VINETIC)
       'C' = 0x43 (ASCII) = 0x0E (VINETIC)
       'D' = 0x44 (ASCII) = 0x0F (VINETIC)
       '0' ...  '9' as is.
*/
IFX_return_t TAPI_LL_Phone_Dtmf_GetCode (IFX_char_t nChar, IFX_uint8_t *pDtmfCode)
{
   IFX_int32_t ret;

   if (pDtmfCode == IFX_NULL)
      return IFX_ERROR;

   ret = IFX_SUCCESS;

   switch (nChar)
   {
   case '*':
      *pDtmfCode = 0x0A;
      break;
   case '#':
      *pDtmfCode = 0x0B;
      break;
   case 'A':
      *pDtmfCode = 0x0C;
      break;
   case 'B':
      *pDtmfCode = 0x0D;
      break;
   case 'C':
      *pDtmfCode = 0x0E;
      break;
   case 'D':
      *pDtmfCode = 0x0F;
      break;
   default:
      if ((nChar >= '0') && (nChar <= '9'))
         *pDtmfCode = (IFX_uint8_t)nChar - '0';
      else
         ret = IFX_ERROR;
      break;
   }

   return ret;
}

/**
   Send CID data
\param pChannel        Handle to TAPI_CONNECTION structure
\param pCid            contains CID data
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   This function is non blocking. It handles all necessary steps to transmit a
   CID. It returns an error if a CID transmission is already running.
*/
IFX_return_t TAPI_LL_Phone_CID_Sending (TAPI_CONNECTION *pChannel,
                                       TAPI_CID_DATA const *pCid)
{
   VINETIC_DEVICE       *pDev      = (VINETIC_DEVICE *)pChannel->pDevice;
   VINETIC_CHANNEL      *pCh       = &pDev->pChannel[pChannel->nChannel];
   IFX_uint8_t          ch         = (pCh->nChannel - 1);
   IFX_TAPI_CID_DTMF_CFG_t *pDtmfConf = &pChannel->TapiCidConf.TapiCidDtmfConf;
   IFX_int32_t           ret       = IFX_SUCCESS;

   switch (pChannel->TapiCidConf.nStandard)
   {
   case IFX_TAPI_CID_STD_TELCORDIA:
   case IFX_TAPI_CID_STD_ETSI_FSK:
   case IFX_TAPI_CID_STD_SIN:
   case IFX_TAPI_CID_STD_NTT:
      /* check whether UTG2 is active,
      because the status bits for CIS and UTG2 are overlaid
      => CIS could not be used while UTG2 is active! */
      if (pDev->pSigCh[pChannel->nChannel].utg[1].bit.en == 1)
      {
         TRACE (TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Cid Sender "
                 "could not be used while UTG2 is active! \n\r"));
         return (IFX_ERROR);
      }
      if (pCh->cidSend.nState == CID_SETUP)
      {
         /* odd num (lowest bit it set) */
         pCh->cidSend.nOdd     = (pCid->nCidParamLen & 1);
         /* get length in words (round up) */
         pCh->cidSend.nCidCnt  = (pCid->nCidParamLen>>1) + pCh->cidSend.nOdd;
         cpb2w(pCh->cidSend.pCid, (IFX_uint8_t *)pCid->cidParam,
               ((IFX_uint32_t)pCh->cidSend.nCidCnt << 1));
         ret = Dsp_SetCIDCoeff(pCh, pCid->txHookMode,
                               &pChannel->TapiCidConf.TapiCidFskConf,
                               pCh->cidSend.nCidCnt);
         /* now start non blocking low level machine */
         if (ret == IFX_SUCCESS)
         {
            /* Bellcore specification shall be used */
            if (pChannel->TapiCidConf.nStandard == IFX_TAPI_CID_STD_TELCORDIA)
            {
               pDev->pSigCh[ch].cid_sender[CMD_HEADER_CNT] &= ~SIG_CID_V23;
            }
            /* ITU-T V.23 specification shall be used */
            else
            {
               pDev->pSigCh[ch].cid_sender[CMD_HEADER_CNT] |= SIG_CID_V23;
            }
            pCh->cidSend.nCidType = pCid->txHookMode;
            ret = VINETIC_CidFskMachine (pCh);
         }
      }
      else
      {
         ret = IFX_ERROR;
         SET_ERROR (ERR_CID_RUNNING);
      }
      break;
   case IFX_TAPI_CID_STD_ETSI_DTMF:
      if (pCh->dtmfSend.state == DTMF_READY)
      {
         /* check digit and interdigit times */
         if ((pDtmfConf->digitTime > MAX_DIGIT_TIME) ||
             (pDtmfConf->interDigitTime > MAX_INTERDIGIT_TIME))
         {
            TRACE (TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Max digit time or "
                 "inter digit time is 127 ms\n\r"));
            return IFX_ERROR;
         }
         /* Write DTMF/AT generator coefficients */
         ret = Dsp_SetDtmfCoeff (pCh, (pDtmfConf->digitTime * 2),
                                (pDtmfConf->interDigitTime * 2));
         if (ret == IFX_SUCCESS)
         {
            /* not using cpb2w, byteorder is corrected in VINETIC_CidDtmfMachine */
            memcpy (pCh->cidSend.pCid, pCid->cidParam, pCid->nCidParamLen);
            ret = VINETIC_DtmfStart (pCh, pCh->cidSend.pCid,
                                     pCid->nCidParamLen, 1,
                                     VINETIC_CidDtmfMachine, IFX_TRUE);
         }
      }
      else
      {
         ret = IFX_ERROR;
         LOG (TAPI_DRV, DBG_LEVEL_HIGH,
              ("\n\rDRV_ERROR: DTMF transmission is ongoing"
               " in this channel\n\r"));
      }
      break;
   default:
      ret = IFX_ERROR;
      break;
   }

   return ret;
}

/**
   Start the CID receiver
\param pChannel    Handle to TAPI_CONNECTION structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   It is checked if the needed resource is available. Otherwise it returns an
   error.
*/
IFX_return_t TAPI_LL_Phone_CidRx_Start (TAPI_CONNECTION *pChannel)
{
   IFX_int32_t ret = IFX_ERROR;
   VINETIC_DEVICE  *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel [pChannel->nChannel];

   /* check if signalling channel is active */
   if (pDev->pSigCh[pChannel->nChannel].sig_ch.bit.en == IFX_TRUE)
   {
      ret = Dsp_SetCidRec (pCh, &pChannel->TapiCidConf.TapiCidFskConf, IFX_TRUE);
   }

   return ret;
}

/**
   Stop CID receiver
\param pChannel        Handle to TAPI_CONNECTION structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   It is checked if the resource is activated. Otherwise it returns an
   error.
*/
IFX_return_t TAPI_LL_Phone_CidRx_Stop (TAPI_CONNECTION *pChannel)
{
   IFX_int32_t ret = IFX_ERROR;
   VINETIC_DEVICE  *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel [pChannel->nChannel];

   /* check if signalling channel is active */
   if (pDev->pSigCh[pChannel->nChannel].sig_ch.bit.en == IFX_TRUE)
   {
      ret = Dsp_SetCidRec (pCh, &pChannel->TapiCidConf.TapiCidFskConf, IFX_FALSE);
   }

   return ret;
}

/**
   Returns the CID receiver status
\param pChannel       Handle to TAPI_CONNECTION structure
\param pCidRxStatus   Handle to IFX_TAPI_CID_RX_STATUS_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   The status variable must be protected because of possible mutual access.
\todo This is no low level function because the status is stored in TAPI
*/
IFX_return_t TAPI_LL_Phone_CidRx_Status (TAPI_CONNECTION *pChannel,
                                        IFX_TAPI_CID_RX_STATUS_t *pCidRxStatus)
{
   VINETIC_DEVICE  *pDev = (VINETIC_DEVICE *)pChannel->pDevice;

   /* protect access to status structure , lock interrupts */
   Vinetic_IrqLockDevice (pDev);
   *pCidRxStatus = pChannel->TapiCidRx.stat;
   Vinetic_IrqUnlockDevice (pDev);

   return IFX_SUCCESS;
}

/**
   CID Receiver data collection.
\param pChannel        Handle to TAPI_CONNECTION structure
\remarks
   This is no TAPI low level function, because is not used by TAPI.
   This function is called by the function handling vinetic packets in
   interrupt level.
\todo This is no TAPI low level function, because is not used by TAPI.
*/
IFX_void_t TAPI_LL_Phone_CidRxDataCollect (TAPI_CONNECTION *pChannel)
{
   VINETIC_DEVICE  *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel [pChannel->nChannel];
   IFX_TAPI_CID_RX_STATUS_t *new = &pChannel->TapiCidRx.stat;
   PACKET *pPacket = &pCh->cidRxPacket;
   IFX_TAPI_CID_RX_DATA_t *pBuf = NULL;
   IFX_int32_t nOdd, nCarrierDet;
   IFX_uint32_t nLen;

   /* set cd and odd */
   nOdd        = pPacket->cmd2 & CMD2_ODD;
   nCarrierDet = ntohs(pPacket->pData[0]) & 0x8000;
   switch (pPacket->cmd2 & CMD2_LEN)
   {
   case  1:
      /* if CD = 1, caller id receiving just started */
      if (nCarrierDet)
         new->nStatus = IFX_TAPI_CID_RX_STATE_ONGOING;
      /* if CD = 0, carrier lost. No Cid data more */
      else
         new->nStatus = IFX_TAPI_CID_RX_STATE_DATA_READY;
      break;
   case  2:
      /* check odd bit to get data length */
      if (nOdd)
         nLen = 1;
      else
         nLen = 2;
      /* get buffer for cid buffering */
      pBuf = (IFX_TAPI_CID_RX_DATA_t *)TAPI_Phone_GetCidRxBuf (pChannel, nLen);
      if (pBuf == NULL)
      {
         new->nError = IFX_TAPI_CID_RX_ERROR_READ;
         break;
      }
      /* at least the high byte is valid */
      pBuf->data [pBuf->nSize++] = HIGHBYTE (ntohs(pPacket->pData [1]));
      /* if odd bit isn't set, then the low byte is the second data */
      if (nOdd == 0)
         pBuf->data [pBuf->nSize++] = LOWBYTE (ntohs(pPacket->pData [1]));
      /* if CD = 0, data are ready */
      if (nCarrierDet == 0)
         new->nStatus = IFX_TAPI_CID_RX_STATE_DATA_READY;
      break;
   default:
      /* shouldn't be */
      break;
   }
   /* event for the user : error or data available */
   if ((new->nStatus == IFX_TAPI_CID_RX_STATE_DATA_READY) ||
       (new->nError  == IFX_TAPI_CID_RX_ERROR_READ)
      )
   {
      TAPI_Phone_Event_CidRx (pChannel);
   }
}

/**
   Get the cid data already collected in an ongoing collection.
\param pChannel    Handle to TAPI_CONNECTION structure
\param pCidRxData  Handle to IFX_TAPI_CID_RX_DATA_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   In CID ongoing state, this function protects the reading of actual data
   buffer from interrupts, because the data reading is done on interrupt level
*/
IFX_return_t TAPI_LL_Phone_Get_CidRxDataCollected (TAPI_CONNECTION *pChannel,
                                                  IFX_TAPI_CID_RX_DATA_t *pCidRxData)
{
   VINETIC_DEVICE  *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   IFX_TAPI_CID_RX_DATA_t *pData = pChannel->TapiCidRx.pData;
   IFX_int32_t ret = IFX_ERROR;

   /* protect buffer access from mutual access, lock interrupts */
   Vinetic_IrqLockDevice (pDev);
   if (pData != NULL)
   {
      *pCidRxData = *pData;
      ret = IFX_SUCCESS;
   }
   Vinetic_IrqUnlockDevice (pDev);

   return ret;
}
#endif /* TAPI_CID */

#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
/**
   Sets the Recording Codec
\param pChannel        Handle to TAPI_CONNECTION structure
\param nCodec          the codec for the record audio channel
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   This function sets the recording codec and a suitable PTE value.
   Coder channel module remains disabled after the completion of
   this function. If the user needs another PTE, he can program this trough
   the appropriate interface. The PTE value set as default follows
   - G.711 Alaw         : 10 ms
   - G.711 Mulaw        : 10 ms
   - G.723 5.3 Kbps     : 30 ms - no other choice possible
   - G.723 6.3 Kbps     : 30 ms - no other choice possible
   - G.728              : 10 ms
   - G.729_AB           : 10 ms
   - G.729_E            : 10 ms
   - G.726 16 Kbps      : 10 ms
   - G.726 24 kbps      : 10 ms
   - G.726 32 Kbps      : 10 ms
   - G.726 40 Kbps      : 10 ms

   This function also caches the actual codec set.
*/
IFX_return_t TAPI_LL_Phone_Set_Rec_Codec(TAPI_CONNECTION *pChannel,
                                        IFX_int32_t nCodec)
{
   VINETIC_DEVICE  *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   FWM_COD_CH      *pCodCh = &pDev->pCodCh[pChannel->nChannel].cod_ch;
   IFX_int32_t      ret = IFX_SUCCESS, nCod = 0, nPte = -1;

   /* check coder module */
   if (pDev->nDevState & DS_COD_EN)
   {
      /* set encoder val according to codec */
      nCod = getEncoder (nCodec);
   }
   else
   {
      ret = IFX_ERROR;
      SET_ERROR(ERR_CODCONF_NOTVALID);
   }
   /* PTE value suitable ? if not, search a suitable pte  */
   if ((ret == IFX_SUCCESS) && (nCod != IFX_ERROR))
   {
      /* get matching PTE to set */
      if (CheckENCnPTE ((IFX_uint8_t)pCodCh->bit.pte,
                        (IFX_uint8_t)nCod) == IFX_ERROR)
      {
         nPte = getConvPTE ((IFX_uint8_t)pCodCh->bit.pte, nCodec);
      }
   }
   else
      ret = IFX_ERROR;
   /* set Encoder: The coder channel will be enabled at recording start.
      Suitable Pte from pte table is set. The settings must be protected
      against concurrent access
   */
   if (ret == IFX_SUCCESS)
   {
      IFX_boolean_t bUpdate = IFX_FALSE;

      IFXOS_MutexLock (pDev->memberAcc);

      /* store the given encoder parameter in the driver parameter */
      if (pDev->pCodCh[pChannel->nChannel].enc_conf != (IFX_uint16_t) nCod)
      {
         pDev->pCodCh[pChannel->nChannel].enc_conf = (IFX_uint16_t) nCod;
         if (pCodCh->bit.enc)
         {
            /* encoder is enable, write the changed codec to the device */
            pCodCh->bit.enc = (IFX_uint8_t) nCod;
            /* it is required to send a firmware message */
            bUpdate = IFX_TRUE;
         }
      }

      if (nPte != -1)
      {
         pCodCh->bit.pte = (IFX_uint8_t) nPte;
         /* it is required to send a firmware message */
         bUpdate = IFX_TRUE;
      }

      if (bUpdate != IFX_FALSE)
      {
         /* it is required to send a firmware message */
         ret = CmdWrite (pDev, pCodCh->value, 2);
      }

      IFXOS_MutexUnlock (pDev->memberAcc);
   }
   /* cache actual codec */
   /** \todo Do this - if really needed - in the calling function in TAPI */
   if (ret == IFX_SUCCESS)
      pChannel->nCodec = nCodec;
   /* release fw mgs lock */

   return ret;
}
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */

/**
   Starts the coder in upstream / record data
\param pChannel        Handle to TAPI_CONNECTION structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
  This function enables the coder channel with the existing
 configuration.
  It resets, also, the members of the VINETIC_CHANNEL structure concerning the
  streaming and clears the channel Fifo, dedicated for incoming voice packets.
*/
IFX_return_t TAPI_LL_Phone_Start_Recording(TAPI_CONNECTION *pChannel)
{
   IFX_int32_t ret = IFX_SUCCESS;

#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CODCH *pCodCh = &pDev->pCodCh[pChannel->nChannel];

   if (pCodCh->cod_ch.bit.enc != pCodCh->enc_conf)
   {
      /* encoder has to be updated */
      /* protect fwmsg against concurrent tasks */
      IFXOS_MutexLock (pDev->memberAcc);
      pCodCh->cod_ch.bit.enc = pCodCh->enc_conf;
      /* unlock */
      IFXOS_MutexUnlock (pDev->memberAcc);

      if (pCodCh->cod_ch.bit.en == 0)
      {
         /* The 'en' bit has to be set too. function 'TAPI_LL_Voice_Enable'
            writes the firmware message to the device */
         ret = TAPI_LL_Voice_Enable (pChannel, 1);

         if (ret != IFX_SUCCESS)
         {
            /* enabling the coder-module failed, mark this in the 'enc'
               parameter */
            /* protect fwmsg against concurrent tasks */
            IFXOS_MutexLock (pDev->memberAcc);
            pCodCh->cod_ch.bit.enc = 0;
            /* unlock */
            IFXOS_MutexUnlock (pDev->memberAcc);
         }
      }
      else
      {
         /* The 'en' bit was already set in the firmware message but the
            parameter 'enc' has changed. The firmware message has to be sent to
            the device */

         /* protect fwmsg against concurrent tasks */
         IFXOS_MutexLock (pDev->memberAcc);
         ret = CmdWrite (pDev, (IFX_uint16_t *) & pCodCh->cod_ch, 2);
         /* unlock */
         IFXOS_MutexUnlock (pDev->memberAcc);
      }
   }
   else
   {
      /* nothing to do. Encoder is already running */
      return (IFX_SUCCESS);
   }

   if (ret == IFX_SUCCESS)
   {
      VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];

      /* set voice connection flag */
      pCh->bVoiceConnect = IFX_TRUE;
      /* Clear fifo for this channel */
      Vinetic_IrqLockDevice (pDev);
      Fifo_Clear (&pCh->rdFifo);
      Vinetic_IrqUnlockDevice (pDev);
   }

#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */

   return ret;
}


/**
   Stops the Recording.
\param pChannel        Handle to TAPI_CONNECTION structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   This function disables the Coder Channel module.
   flag of the VINETIC_CHANNEL structure and wakes up processes to read
   the last arrived data.
  The function calls TAPI_LL_Voice_Enable which also switches the event
  transmission after the coder is stopped. It is switched on from the
  cached structure in TAPI_LL_Start_Recording. Modifing on while the
  coder is stopped is done in the cached strucutre. */
IFX_return_t TAPI_LL_Phone_Stop_Recording(TAPI_CONNECTION *pChannel)
{
   IFX_int32_t ret = IFX_ERROR;

#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   VINETIC_CODCH *pCodCh = &pDev->pCodCh[pChannel->nChannel];

   if (pCodCh->cod_ch.bit.enc)
   {
      /* encoder has to be updated */
      /* protect fwmsg against concurrent tasks */
      IFXOS_MutexLock (pDev->memberAcc);
      pCodCh->cod_ch.bit.enc = 0;
      /* unlock */
      IFXOS_MutexUnlock (pDev->memberAcc);

      if (pCodCh->cod_ch.bit.dec == 0)
      {
         /* decoder is not running -> disable the whole coder-module */
         /* protect fwmsg against concurrent tasks */

         ret = TAPI_LL_Voice_Enable (pChannel, 0);
         if (ret != IFX_SUCCESS)
            return ret;
         pCh->bVoiceConnect = IFX_FALSE;
      }
      else
      {
         /* decoder is still running -> do not disable the whole coder-module
            send the updated firmware message to the device */
         IFXOS_MutexLock (pDev->memberAcc);
         ret = CmdWrite (pDev, (IFX_uint16_t *) & pCodCh->cod_ch, 2);
         /* unlock */
         IFXOS_MutexUnlock (pDev->memberAcc);
      }
   }
   else
      ret = IFX_SUCCESS;

   /* in case of blocking voice service, wakeup the sleeping on the
      voice data queue to avoid running in time out */
   if ((pCh->nFlags & CF_NONBLOCK) == 0)
   {
       IFXOS_WakeUpEvent (pCh->wqRead);
   }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */

   return ret;
}

/**
   Starts the Playing.
\param pChannel        Handle to TAPI_CONNECTION structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
  This function does the same as TAPI_LL_Phone_Start_Recording :
  start Recording / playing are done by enabling the coder channel.
*/
IFX_return_t TAPI_LL_Phone_Start_Playing(TAPI_CONNECTION *pChannel)
{
   IFX_int32_t ret = IFX_ERROR;

#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CODCH *pCodCh = &pDev->pCodCh[pChannel->nChannel];

   if (pCodCh->cod_ch.bit.dec == 0)
   {
      /* protect fwmsg against concurrent tasks */
      IFXOS_MutexLock (pDev->memberAcc);
      pCodCh->cod_ch.bit.dec = 1;
      /* unlock */
      IFXOS_MutexUnlock (pDev->memberAcc);

      if (pCodCh->cod_ch.bit.en == 0)
      {
         /* The 'en' bit has to be set too. function 'TAPI_LL_Voice_Enable'
            writes the firmware message to the device */

         ret = TAPI_LL_Voice_Enable (pChannel, 1);

         if (ret != IFX_SUCCESS)
         {
            /* enabling the coder-module failed, mark this in the 'dec'
               parameter */
            /* protect fwmsg against concurrent tasks */
            IFXOS_MutexLock (pDev->memberAcc);
            pCodCh->cod_ch.bit.dec = 0;
            /* unlock */
            IFXOS_MutexUnlock (pDev->memberAcc);
         }
      }
      else
      {
         /* The 'en' bit was already set in the firmware message but the
            parameter 'dec' has changed. Only if the 'dec' bit is zero, the
            firmware message has to be sent to the device */

         /* protect fwmsg against concurrent tasks */
         IFXOS_MutexLock (pDev->memberAcc);
         ret = CmdWrite (pDev, (IFX_uint16_t *) & pCodCh->cod_ch, 2);
         /* unlock */
         IFXOS_MutexUnlock (pDev->memberAcc);
      }
   }
   else
      ret = IFX_SUCCESS;

#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */

   return ret;
}

/**
   Stops the Playing.
\param pChannel        Handle to TAPI_CONNECTION structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
  This function does the same as TAPI_LL_Phone_Stop_Recording :
  stop Recording / playing are done by disabling the coder channel.
*/
IFX_return_t TAPI_LL_Phone_Stop_Playing(TAPI_CONNECTION *pChannel)
{
   IFX_int32_t ret = IFX_ERROR;

#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   VINETIC_CODCH *pCodCh = &pDev->pCodCh[pChannel->nChannel];

   if (pCodCh->cod_ch.bit.dec)
   {
      /* decoder has to be updated */
      /* protect fwmsg against concurrent tasks */
      IFXOS_MutexLock (pDev->memberAcc);
      pCodCh->cod_ch.bit.dec = 0;
      /* unlock */
      IFXOS_MutexUnlock (pDev->memberAcc);

      if (pCodCh->cod_ch.bit.enc == 0)
      {
         /* encoder is not running -> disable the whole coder-module */
         ret = TAPI_LL_Voice_Enable (pChannel, 0);
         if (ret != IFX_SUCCESS)
            return ret;
         pCh->bVoiceConnect = IFX_FALSE;
      }
      else
      {
         /* enocder is still running -> do not disable the whole coder-module
            send the updated firmware message to the device */
         IFXOS_MutexLock (pDev->memberAcc);
         ret = CmdWrite (pDev, (IFX_uint16_t *) & pCodCh->cod_ch, 2);
         /* unlock */
         IFXOS_MutexUnlock (pDev->memberAcc);
      }
   }
   else
      ret = IFX_SUCCESS;

#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */

   return ret;
}

#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
/**
   configure rt(c)p for a new connection
\param pChannel        Handle to TAPI_CONNECTION structure
\param pRtpConf        Handle to IFX_TAPI_PKT_RTP_CFG_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
*/
IFX_return_t TAPI_LL_Phone_RtpConf (TAPI_CONNECTION *pChannel,
                                   IFX_TAPI_PKT_RTP_CFG_t const *pRtpConf)
{
   IFX_int32_t ret = IFX_SUCCESS;
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   IFX_uint8_t ch = pChannel->nChannel, bApplyRec = IFX_FALSE;
   IFX_uint16_t pCmd [5] = {0};
   FWM_SIG_CH_CONF sigConf;

   memset (&sigConf, 0, sizeof(FWM_SIG_CH_CONF));
   if (ch < pDev->nSigCnt)
   {
      if (pDev->pCodCh[ch].cod_ch.bit.en &&
          pChannel->nEvtPT != pRtpConf->nEventPT)
      {
         SET_ERROR (ERR_COD_RUNNING);
         return IFX_ERROR;
      }
      sigConf.value[0]  = (CMD1_EOP | ch);
      sigConf.value [1] = ECMD_SIG_CH_RTP;
      sigConf.bit.ssrch = HIGHWORD (pRtpConf->nSsrc);
      sigConf.bit.ssrcl = LOWWORD  (pRtpConf->nSsrc);

      /* Ignore the setting for nEventPT and transmit tones as voice in the
         configured codec */
      if (pRtpConf->nEvents == IFX_TAPI_PKT_EV_OOB_NO)
         sigConf.bit.eventPt = pChannel->nEvtPT;
      else
      {
         if (pRtpConf->nEventPT == 0)
         {
            SET_ERROR(ERR_WRONG_EVPT);
            return IFX_ERROR;
         }
         sigConf.bit.eventPt = pRtpConf->nEventPT & 0x7F;
      }
      /* do event transmission settings for DTMF receiver */
      if (ret == IFX_SUCCESS)
      {
         pCmd[2] = pDev->pSigCh[ch].dtmf_rec.value;
         IFXOS_MutexLock (pDev->memberAcc);
         pDev->pSigCh[ch].dtmf_rec.bit.as = IFX_FALSE;
         switch (pRtpConf->nEvents)
         {
         case IFX_TAPI_PKT_EV_OOB_NO:
            /* No Event transmission support */
            /* store the setting for later use */
            pDev->pSigCh[ch].et_stat.value = 0;
            break;
         case IFX_TAPI_PKT_EV_OOB_DEFAULT:
         case IFX_TAPI_PKT_EV_OOB_ONLY:
            pDev->pSigCh[ch].dtmf_rec.bit.as = IFX_TRUE;
            /*lint-fallthroug */
         case IFX_TAPI_PKT_EV_OOB_ALL:
            /* event transmission support */
            /* store the setting for later use */
            pDev->pSigCh[ch].et_stat.value = 0xFFFF;
            break;
         default:
            ret = IFX_ERROR;
            break;
         }
         if (pCmd[2] != pDev->pSigCh[ch].dtmf_rec.value)
         {
            pCmd[2] = pDev->pSigCh[ch].dtmf_rec.value;
            bApplyRec = IFX_TRUE;
         }
         IFXOS_MutexUnlock (pDev->memberAcc);
      }
      /* set event pt, ssrc, sequence nr for signalling channel : This has to be
         done with inactive signalling channel */
#ifdef FW_ETU
      {
         sigConf.bit.coderCh = ch;
         switch (pRtpConf->nEvents)
         {
         case IFX_TAPI_PKT_EV_OOB_NO:
            /* No Event transmission support */
            break;
         case IFX_TAPI_PKT_EV_OOB_DEFAULT:
         case IFX_TAPI_PKT_EV_OOB_ONLY:
            /* muting the voice in sig channel rtp */
            sigConf.bit.a1 = 2;
            sigConf.bit.a2 = 2;
            break;
         case IFX_TAPI_PKT_EV_OOB_ALL:
            /* event transmission support no muting */
            sigConf.bit.a1 = 1;
            sigConf.bit.a2 = 1;
         break;
         default:
            ret = IFX_ERROR;
            break;
      }
      switch (pRtpConf->nPlayEvents)
      {
      case IFX_TAPI_PKT_EV_OOBPLAY_DEFAULT:
      case IFX_TAPI_PKT_EV_OOBPLAY_PLAY:
            /* all playout always enabled */
            sigConf.bit.evMaskTOG = 0x7;
            break;
      case IFX_TAPI_PKT_EV_OOBPLAY_MUTE:
         /* all playout always enabled */
         sigConf.bit.evMaskTOG = 0x0;
         break;
         default:
            ret = IFX_ERROR;
            break;
         }
         /* enable the trigger if mask is set */
         if (pChannel->TapiMiscData.nExceptionMask.Bits.eventDetect == 0)
         {
            sigConf.bit.evMaskTrig = 0x7F;
         }
         else
         {
            sigConf.bit.evMaskTrig = 0;
         }
         ret = CmdWrite (pDev, sigConf.value, 6);
         if (ret == IFX_SUCCESS)
         {
            SIG_MOD_STATE state;

            state.value = 0;
            IFXOS_MutexLock (pDev->memberAcc);
            if (pDev->pCodCh[ch].cod_ch.bit.en == IFX_TRUE)
            {
               /* if coder running we apply it directely otherwise
                  it is just stored */
               ret = Dsp_UpdateEventTrans (pDev, ch, pDev->pSigCh[ch].et_stat,
                                           &state);
            }
            IFXOS_MutexUnlock (pDev->memberAcc);
            /* if DTMF receiver has been written we do not need it afterwards */
            if (state.flag.dtmf_rec == IFX_TRUE)
               bApplyRec = IFX_FALSE;
         }
      }
#else
      ret = CmdWrite (pDev, sigConf.value, 4);
#endif /* FW_ETU */
      if (bApplyRec)
      {
         /* value changed, so write it */
         pCmd[0] = CMD1_EOP | ch;
         pCmd[1] = ECMD_DTMF_REC;
         pCmd[2] = pDev->pSigCh[ch].dtmf_rec.value;
         ret = CmdWrite (pDev, pCmd, 1);
      }
   }
   /* store the information for downstream service. */
   if (ret == IFX_SUCCESS)
   {
      /* error when coder is active */
      if (pRtpConf->nEventPT)
      {
         /* Configure transmission through events and use payload type value
            from nEventPT */
         pChannel->nEvtPT = pRtpConf->nEventPT;
      }
   }
#if 0
   /* set the RTP time stamp */
   if (ret == IFX_SUCCESS)
   {
      /* global command for all channels */
      pCmd[0] = CMD1_EOP;
      pCmd[1] = ECMD_COD_RTP;
      pCmd[2] = HIGHWORD (pRtpConf->nTimestamp);
      pCmd[3] = LOWWORD (pRtpConf->nTimestamp);
      ret = CmdWrite (pDev, pCmd, 2);
   }
#endif /* 0 */
   /* set ssrc, sequence nr for coder channel */
   if (ret == IFX_SUCCESS)
   {
      /* do rtp settings for coder channel */
      pCmd[0] = CMD1_EOP | ch;
      pCmd[1] = ECMD_COD_CHRTP_UP;
      if (pCmd[2] != HIGHWORD (pRtpConf->nSsrc) ||
          pCmd[3] != LOWWORD (pRtpConf->nSsrc) ||
          pCmd[4] != pRtpConf->nSeqNr)
      {
         /* write only if needed */
         pCmd[2] = HIGHWORD (pRtpConf->nSsrc);
         pCmd[3] = LOWWORD (pRtpConf->nSsrc);
         pCmd[4] = pRtpConf->nSeqNr;
         ret = CmdWrite (pDev, pCmd, 3);
      }
   }
   /* error case */
   if (ret != IFX_SUCCESS)
   {
      LOG (TAPI_DRV,DBG_LEVEL_HIGH,
          ("\n\rDRV_ERROR: RTP Configuration failed\n\r"));
   }
   else
   {
      pDev->pCodCh[ch].nSeqNr = pRtpConf->nSeqNr;
      pDev->pCodCh[ch].nSsrc  = pRtpConf->nSsrc;
   }

   return ret;
}

/**
   configure a new payload type
\param pChannel          Handle to TAPI_CONNECTION structure
\param pRtpPTConf        Handle to IFX_TAPI_PKT_RTP_PT_CFG_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
*/
IFX_return_t TAPI_LL_Phone_Rtp_SetPayloadType (TAPI_CONNECTION *pChannel,
                                              IFX_TAPI_PKT_RTP_PT_CFG_t const *pRtpPTConf)
{

   IFX_int32_t ret = IFX_SUCCESS;
   VINETIC_DEVICE  *pDev =  (VINETIC_DEVICE *)pChannel->pDevice;
   VINETIC_CHANNEL *pCh  =  &pDev->pChannel[pChannel->nChannel];

   /* Set payload types in upstream direction */
   ret = Dsp_SetRTPPayloadType (pCh, IFX_TRUE, (IFX_uint8_t*)pRtpPTConf->nPTup);
#ifdef VIN_2CPE
   /* Set payload types in downstream direction */
   if (ret == IFX_SUCCESS)
      ret = Dsp_SetRTPPayloadType (pCh, IFX_FALSE,
                                   (IFX_uint8_t*)pRtpPTConf->nPTdown);
#endif /* VIN_2CPE*/

   if (ret == IFX_ERROR)
      LOG (TAPI_DRV,DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Coder Channel RTP"
           "configuration.\n\r"));

   return ret;
}

/**
   gets the RTCP statistic information for the addressed channel
\param pChannel        Handle to TAPI_CONNECTION structure
\param pRTCP           Handle to IFX_TAPI_PKT_RTCP_STATISTICS_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   RTCP statistics for the specified channel are reset after the completion of
   the read request. A Coder Channel Statistics write command is issued on this
   purpose.
*/
IFX_return_t TAPI_LL_Phone_RTCP_GetStatistics (TAPI_CONNECTION *pChannel,
                                              IFX_TAPI_PKT_RTCP_STATISTICS_t *pRTCP)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   IFX_uint16_t pCmd [2] = {0}, pData[16] = {0};
   IFX_int32_t ret = IFX_SUCCESS;

   /*EOP command */
   pCmd [0] = CMD1_EOP | (pChannel->nChannel);
   /* Read Coder Channel Statistics */
   pCmd [1] = ECMD_COD_CHRTCP;
   ret = CmdRead (pDev, pCmd, pData, 14);
   if (ret == IFX_SUCCESS)
   {
      /* assign the RTCP values to the corresponding members of the
         IFX_TAPI_PKT_RTCP_STATISTICS_t structure */
      pRTCP->rtp_ts        = (pData[2] << 16) | pData[3];
      pRTCP->psent         = (pData[4] << 16) | pData[5];
      pRTCP->osent         = (pData[6] << 16) | pData[7];
      pRTCP->rssrc         = (pData[8] << 16) | pData[9];
      pRTCP->fraction      = ((pData[10] & 0xFF00)>> 8);
      pRTCP->lost          = ((pData[10] & 0xFF) << 16) | pData[11];
      pRTCP->last_seq      = (pData[12] << 16) | pData [13];
      pRTCP->jitter        = (pData[14] << 16) | pData[15];
      /* return stored SSRC */
      pRTCP->ssrc = pDev->pCodCh[pChannel->nChannel].nSsrc;
      /* The calling control task will set the parameters lsr and dlsr */
   }
   else
   {
      LOG (TAPI_DRV,DBG_LEVEL_HIGH,
          ("\n\rDRV_ERROR: can't read Coder Channel RTCP statistics.\n\r"));
   }

   return ret;
}

/**
   Prepare the RTCP statistic information for the addressed channel
   (in ISR context)
\param pChannel        Handle to TAPI_CONNECTION structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   Together with TAPI_LL_Phone_RTCP_GatherStatistics, this function provides
   non-blocking and non-OS-dependent access to RTCP statistics.  The client
   calls TAPI_LL_Phone_RTCP_PrepareStatistics, performs other time-consuming
   tasks, and then calls TAPI_LL_Phone_RTCP_GatherStatistics to read the
   command mailbox. This function must execute in the context of a
   non-interruptible ISR, hence we are not concerned with protection from
   interrupts.
   Note that it is the responsibility of the client to gather statistics in the
   same order that they are requested (since there's a single command outbox
   per VINETIC device).
*/
IFX_return_t TAPI_LL_Phone_RTCP_PrepareStatistics (TAPI_CONNECTION *pChannel)
{
   VINETIC_DEVICE       *pDev    = (VINETIC_DEVICE *)pChannel->pDevice;
   IFX_uint16_t          pCmd[2];
   IFX_int32_t           ret     = IFX_SUCCESS;

   /* EOP command with read bit set (the Short Command bit remains 0) */
   pCmd[0] = CMD1_EOP | CMD1_RD | (pChannel->nChannel);
   /* Read Coder Channel Statistics (the length field is 0, see VINETIC User's
      Manual) */
   pCmd[1] = ECMD_COD_CHRTCP | CMD_COD_RTCP_LEN;
   /* write read-request using ISR-specific write function */
   ret = CmdWriteIsr (pDev, pCmd, 0);
   if (ret != IFX_SUCCESS)
   {
      LOG (TAPI_DRV,DBG_LEVEL_HIGH,
           ("\n\rDRV_ERROR: can't request Coder Channel RTCP statistics.\n\r"));
   }

   return ret;
}

/**
   Gathers previously requested RTCP statistic information for the addressed
   channel
\param pChannel        Handle to TAPI_CONNECTION structure
\param pRTCP           Handle to the returned data.
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   See description under TAPI_LL_Phone_RTCP_PrepareStatistics.
*/
IFX_return_t TAPI_LL_Phone_RTCP_GatherStatistics (TAPI_CONNECTION *pChannel,
                                                 IFX_TAPI_PKT_RTCP_STATISTICS_t *pRTCP)
{
   VINETIC_DEVICE *pDev      = (VINETIC_DEVICE *)pChannel->pDevice;
   IFX_uint16_t*    pData, ch = pChannel->nChannel;

   if (pDev->pCodCh[ch].rtcp_update == IFX_TRUE)
   {
      pData = pDev->pCodCh[ch].rtcp;
      /* assign the RTCP values to the corresponding members
         of the IFX_TAPI_PKT_RTCP_STATISTICS_t structure */
      pRTCP->rtp_ts        = (pData[0] << 16) | pData[1];
      pRTCP->psent         = (pData[2] << 16) | pData[3];
      pRTCP->osent         = (pData[4] << 16) | pData[5];
      pRTCP->ssrc          = (pData[6] << 16) | pData[7];
      pRTCP->fraction      = ((pData[8] & 0xFF00)>> 8);
      pRTCP->lost          = ((pData[8] & 0xFF) << 16) | pData[9];
      pRTCP->last_seq      = (pData[10] << 16) | pData [11];
      pRTCP->jitter        = (pData[12] << 16) | pData[13];
      pDev->pCodCh[ch].rtcp_update = IFX_FALSE;

      return IFX_SUCCESS;
   }
   else
      return IFX_ERROR;
}

/**
   resets  RTCP statistics
\param pChannel          Handle to TAPI_CONNECTION structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
*/
IFX_return_t TAPI_LL_Phone_RTCP_Reset (TAPI_CONNECTION *pChannel)
{
   VINETIC_DEVICE *pDev     = (VINETIC_DEVICE *)pChannel->pDevice;
   IFX_uint16_t    pCmd [2] = {0};

   /* reset RTCP Statistics by sending a Coder Channel
      Statistics (RTCP command) write command of length 0 */
   pCmd [0] = CMD1_EOP | (pChannel->nChannel);
   pCmd [1] = ECMD_COD_CHRTCP;

   return CmdWrite (pDev, pCmd, 0);
}

/**
   configures the jitter buffer
\param pChannel   Handle to TAPI_CONNECTION structure
\param pJbConf    Handle to IFX_TAPI_JB_CFG_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   This function caches the actual Jitter buffer configuration set.
*/
IFX_return_t TAPI_LL_Phone_JB_Set (TAPI_CONNECTION *pChannel,
                                  IFX_TAPI_JB_CFG_t const *pJbConf)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   IFX_uint16_t    pData [6] = {0};
   IFX_int32_t     ret;

   /* setup command for jb configuration */
   pData [0] = CMD1_EOP | (pChannel->nChannel);
   pData [1] = ECMD_COD_CHJB;
   pData[2] = COD_CHJB_PJE;
   /* do JB settings */
   switch (pJbConf->nJbType)
   {
   case IFX_TAPI_JB_TYPE_FIXED:
      break;
   case IFX_TAPI_JB_TYPE_ADAPTIVE:
      /* set adaptive type */
      pData[2] |= COD_CHJB_ADAPT;
      /* do settings for local adaptation and sample interpolation */
      switch (pJbConf->nLocalAdpt)
      {
      case IFX_TAPI_JB_LOCAL_ADAPT_OFF:
         break;
      case IFX_TAPI_JB_LOCAL_ADAPT_ON:
         pData [2] |= COD_CHJB_LOC;
         break;
      case IFX_TAPI_JB_LOCAL_ADAPT_SI_ON:
         pData [2] |= (COD_CHJB_LOC | COD_CHJB_SI);
         break;
      default:
         LOG (TAPI_DRV,DBG_LEVEL_HIGH,
             ("\n\rDRV_ERROR: Wrong Parameter. Can't set Jitter Buffer\n\r"));
         return IFX_ERROR;
      }
      break;
   default:
      LOG (TAPI_DRV,DBG_LEVEL_HIGH,
             ("\n\rDRV_ERROR: Wrong Parameter. Can't set Jitter Buffer\n\r"));
      return IFX_ERROR;
   }
   /* scaling factor */
   pData [2] |= ((IFX_uint8_t)pJbConf->nScaling << 8);
   switch (pJbConf->nPckAdpt)
   {
      case  0:
      case  1:
         /* not supported */
      break;
      case  IFX_TAPI_JB_PKT_ADAPT_VOICE:
      /* Packet adaption is optimized for voice. Reduced adjustment speed and
         packet repetition is off */
      /* ADAP = 1, ->see above PJE = 1 SF = default ->see above */
         pData [2] |= COD_CHJB_PJE;
      break;
      case  IFX_TAPI_JB_PKT_ADAPT_DATA:
         /* Packet adaption is optimized for data */
         pData [2] |= COD_CHJB_RAD | COD_CHJB_PRP | COD_CHJB_DVF;
      break;
      default:
         SET_ERROR(ERR_FUNC_PARM);
         return IFX_ERROR;
   }
   /* Initial Size of JB */
   pData [3] = pJbConf->nInitialSize;
   /* minimum Size of JB */
   pData [4] = pJbConf->nMinSize;
   /* maximum Size of JB  */
   pData [5] = pJbConf->nMaxSize;
   ret = CmdWrite (pDev, pData, 4);
   /* cache actual jitter buffer config */
   if (ret == IFX_SUCCESS)
      memcpy (&pChannel->TapiJbData, pJbConf, sizeof (IFX_TAPI_JB_CFG_t));

   return ret;
}

/**
   query jitter buffer statistics
\param pChannel          Handle to TAPI_CONNECTION structure
\param pJbData           Handle to IFX_TAPI_JB_STATISTICS_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
*/
IFX_return_t TAPI_LL_Phone_JB_GetStatistics (TAPI_CONNECTION *pChannel,
                                            IFX_TAPI_JB_STATISTICS_t *pJbData)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   IFX_uint16_t    pCmd [2] = {0}, pData[30] = {0};
   IFX_int32_t     ret = IFX_SUCCESS, val;

   /* setup command to read jb configuration */
   pCmd [0] = CMD1_EOP | (pChannel->nChannel);
   pCmd [1] = ECMD_COD_CHJB;
   ret = CmdRead (pDev, pCmd, pData, 1);
   /* get type of configured jitter buffer */
   if (ret == IFX_SUCCESS)
   {
      switch (pData [2] & COD_CHJB_ADAPT)
      {
      case 0:
         /* jitter buffer type */
         pJbData->nType = IFX_TAPI_JB_TYPE_FIXED;
         break;
      case 1:
      default:
         /* jitter buffer type */
         pJbData->nType = IFX_TAPI_JB_TYPE_ADAPTIVE;
         break;
      }
      /* read Coder Channel JB Statistics */
      /* When extended JB statistic is available a length of 28 can be read
         otherwise only a length of 26 may be read. */
      pCmd [0] = CMD1_EOP | (pChannel->nChannel);
      pCmd [1] = ECMD_COD_CHJBSTAT;
      if (pDev->nCapabilities & VIN_EDSP_EXTENDED_JB)
         ret = CmdRead (pDev, pCmd, pData, 28);
      else
         ret = CmdRead (pDev, pCmd, pData, 26);
   }
   /* assign the JB statistics values to the corresponding members of the
      IFX_TAPI_JB_STATISTICS_t structure */
   if (ret == IFX_SUCCESS)
   {
      /* incoming time, not supported anymore */
      pJbData->nInTime           = 0;
      /* Comfort Noise Generation, not supported anymore */
      pJbData->nCNG              = 0;
      /* Bad Frame Interpolation, not supported anymore */
      pJbData->nBFI              = 0;
      /* max packet delay, not supported anymore */
      pJbData->nMaxDelay         = 0;
      /* minimum packet delay, not supported anymore */
      pJbData->nMinDelay         = 0;
      /* network jitter value, not supported anymore */
      pJbData->nNwJitter         = 0;
      /* play out delay */
      pJbData->nPODelay          = pData [2];
      /* max play out delay */
      pJbData->nMaxPODelay       = pData [3];
      /* min play out delay */
      pJbData->nMinPODelay       = pData [4];
      /* current jitter buffer size */
      pJbData->nBufSize          = pData [5];
      pJbData->nMaxBufSize       = pData [6];
      pJbData->nMinBufSize       = pData [7];
      /* received packet number  */
      pJbData->nPackets          = ((pData [8] << 16) | pData [9]);
      /* lost packets number, not supported anymore */
      pJbData->nLost             = 0;
      /* invalid packet number (discarded) */
      pJbData->nInvalid          = pData [10];
      /* duplication packet number, not supported anymore */
      pJbData->nDuplicate        =  0;
      /* late packets number */
      pJbData->nLate             = pData [11];
      /* early packets number */
      pJbData->nEarly            = pData [12];
      /* resynchronizations number */
      pJbData->nResync           = pData [13];
      /* new support */
      pJbData->nIsUnderflow      = ((pData [14] << 16) | pData [15]);
      pJbData->nIsNoUnderflow    = ((pData [16] << 16) | pData [17]);
      pJbData->nIsIncrement      = ((pData [18] << 16) | pData [19]);
      pJbData->nSkDecrement      = ((pData [20] << 16) | pData [21]);
      pJbData->nDsDecrement      = ((pData [22] << 16) | pData [23]);
      pJbData->nDsOverflow       = ((pData [24] << 16) | pData [25]);
      pJbData->nSid              = ((pData [26] << 16) | pData [27]);

      /* This data is only provided when extended JB statistics are supported */
      if (pDev->nCapabilities & VIN_EDSP_EXTENDED_JB)
      {
         pJbData->nRecBytesL        = ((pData [28] << 16) | pData [29]);
         /* Because the firmware does not provide the high DWORD part
             the driver has to handle the overflow. This is done by comparing the
             new and the old value. If the new value is smaller, there is
             an overflow and high is increased */
         if (pJbData->nRecBytesL < pDev->pCodCh->nRecBytesL)
            pDev->pCodCh->nRecBytesH++;
         pDev->pCodCh->nRecBytesL = pJbData->nRecBytesL;
         /* apply the high DWORD part */
         pJbData->nRecBytesH = pDev->pCodCh->nRecBytesH;
      }
   }
   if (pDev->nCapabilities & VIN_EDSP_EXTENDED_JB)
   {
      if (ret == IFX_SUCCESS)
      {
         pCmd [0] = CMD1_EOP | (pChannel->nChannel);
         pCmd [1] = ECMD_SIG_EVTSTAT;
         ret = CmdRead (pDev, pCmd, pData, 9);
      }
      if (ret == IFX_SUCCESS)
      {
         val = ((pData [10] << 16) | pData [11]);
         if (val < pDev->pSigCh->nRecBytesL)
            pDev->pSigCh->nRecBytesH++;
         pDev->pSigCh->nRecBytesL = val;
         pJbData->nRecBytesL += val;
         /* apply the high DWORD part */
         pJbData->nRecBytesH += pDev->pSigCh->nRecBytesH;
      }
   }

   return ret;
}

/**
   resets jitter buffer statistics
\param pChannel          Handle to TAPI_CONNECTION structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
*/
IFX_return_t TAPI_LL_Phone_JB_Reset (TAPI_CONNECTION *pChannel)
{
   VINETIC_DEVICE *pDev     = (VINETIC_DEVICE *)pChannel->pDevice;
   IFX_uint16_t    pCmd [2] = {0};

   /* reset JB Statistics by sending a Coder Channel JB Statistics write
      command of length 0 */
   pCmd [0] = CMD1_EOP | (pChannel->nChannel);
   pCmd [1] = ECMD_COD_CHJBSTAT;

   return CmdWrite (pDev, pCmd, 0);
}

/**
   configures AAL fields for a new connection
\param pChannel          Handle to TAPI_CONNECTION structure
\param pAalConf          Handle to IFX_TAPI_PCK_AAL_CFG_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   The coder channel has to be disabled before the AAL channel configuration
   can be done. The user task should make sure it activate the coder channel
   after having set this configuration.
*/
IFX_return_t TAPI_LL_Phone_Aal_Set (TAPI_CONNECTION *pChannel,
                                   IFX_TAPI_PCK_AAL_CFG_t const *pAalConf)
{
   VINETIC_DEVICE *pDev     = (VINETIC_DEVICE *)pChannel->pDevice;
   IFX_uint16_t    pCmd [3] = {0}, pData [3] = {0};
   IFX_int32_t     ret      = IFX_SUCCESS;

   /* set cmd 1 */
   pCmd [0] = (CMD1_EOP | pChannel->nChannel);

   /* write aal start timestamp */
   pCmd [1] = ECMD_COD_AALCONF;
   pCmd [2] = pAalConf->nTimestamp;
   ret = CmdWrite (pDev, pCmd, 1);
   /* read actual aal coder channel configuration */
   if (ret == IFX_SUCCESS)
   {
      pCmd [1] = ECMD_COD_CHAAL;
      ret = CmdRead (pDev, pCmd, pData, 1);
   }
   /* disable coder channel */
   if (ret == IFX_SUCCESS)
      ret = TAPI_LL_Voice_Enable (pChannel, 0);
   /* set connection Id : This has to be done with disabled coder channel */
   if (ret == IFX_SUCCESS)
   {
      pData [2] &= ~COD_CHAAL_CID;
      pData [2] |= (pAalConf->nCid << 8) & COD_CHAAL_CID;
      ret = CmdWrite (pDev, pData, 1);
   }
   /* \todo what about signalling channel */

   return ret;
}

#if 0
/**
   switches Coder after a Decoder status change.
\param pChannel          Handle to TAPI_CONNECTION structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   If the Decoder status changes during an AAL voice connection, the changed
   parameters (Packet time or Decoder) will be set back from this function. So
   this function should be called in AAL Mode every time a decoder status
   change occured.

   This function assumes that PTE and Encoder matches together and does the
   settings without check.

   The function checks

   - if packet time or decoder or the two has changed.
   - in case of packet time change, the function converts the packet time for
     the coder channel speech compression accordingly.
   - programs the coder new without a match check between coder and packet time.

   The function has been tested in Interrupt mode. In this mode, it was called
   from the interrupt routine.
*/
IFX_return_t TAPI_LL_Phone_Aal_SwitchCoder (TAPI_CONNECTION *pChannel)
{
   IFX_uint8_t nPteNew = 0, ch = pChannel->nChannel;
   VINETIC_DEVICE  *pDev    = (VINETIC_DEVICE *)pChannel->pDevice;
   IFX_uint16_t    *pDecChg = pDev->pCodCh[ch].dec_status;
   VINETIC_CODCH *pCodCh = &pDev->pCodCh[ch];
   IFX_int32_t      ret     = IFX_SUCCESS;

   SetTraceLevel (VINETIC, DBG_LEVEL_LOW);

   /* new packet time value */
   switch (pDecChg [2] & COD_CHDECSTAT_PTD)
   {
   case  COD_CHDECSTAT_5MS:
      nPteNew = COD_CH_PTE_5MS;
      break;
   case  COD_CHDECSTAT_5_5MS:
      nPteNew = COD_CH_PTE_5_5MS;
      break;
   case  COD_CHDECSTAT_10MS:
      nPteNew = COD_CH_PTE_10MS;
      break;
   case  COD_CHDECSTAT_11MS:
      nPteNew = COD_CH_PTE_11MS;
      break;
   case  COD_CHDECSTAT_20MS:
      nPteNew = COD_CH_PTE_20MS;
      break;
   case  COD_CHDECSTAT_30MS:
      nPteNew = COD_CH_PTE_30MS;
      break;
   default:
      LOG (TAPI_DRV,DBG_LEVEL_HIGH,
          ("\n\rDRV_ERROR: Packet time not supported\n\r"));
      ret = IFX_ERROR;
   }
   if (ret == IFX_SUCCESS)
   {
      IFX_boolean_t bUpdate = IFX_FALSE;

      /* check if pte should be changed */
      if (pCodCh->cod_ch.bit.pte != nPteNew)
      {
         /* fw message to update has to be sent */
         TRACE (TAPI_DRV,DBG_LEVEL_HIGH,
          ("Ch %d: PTE change 0x%02X -> 0x%02X\n\r", pChannel->nChannel,
                 pCodCh->cod_ch.bit.pte, nPteNew));

         bUpdate = IFX_TRUE;

         pCodCh->cod_ch.bit.pte = nPteNew;
      }
      /* check if coder should be changed */
      if (pCodCh->enc_conf != (pDecChg[2] & COD_CHDECSTAT_DEC))
      {
         /* update local encoder storage, because the decoder is different */
         pCodCh->enc_conf = (pDecChg[2] & COD_CHDECSTAT_DEC);

         if (pCodCh->cod_ch.bit.enc)
         {
            /* enocder is enable. A fw message has to be sent to device */
            TRACE (TAPI_DRV, DBG_LEVEL_HIGH,
                   ("Ch %d: Coder change 0x%02X -> 0x%02X\n\r",
                    pChannel->nChannel, pCodCh->cod_ch.bit.enc,
                    pCodCh->enc_conf));

            bUpdate = IFX_TRUE;
            pCodCh->cod_ch.bit.enc = pCodCh->enc_conf;
         }
      }

      /* configure what needs to be configured */
      if (bUpdate != IFX_FALSE)
      {
         ret = CmdWrite (pDev, pCodCh->cod_ch.value, 2);
      }
   }

   SetTraceLevel (VINETIC, DBG_LEVEL_HIGH);
   pDev->pCodCh[ch].dec_update = IFX_FALSE;

   return ret;
}
#endif /* 0 */

/**
   Programs an AAL profile
\param pChannel        Handle to TAPI_CONNECTION structure
\param pProfile        Handle to IFX_TAPI_PCK_AAL_PROFILE_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   none
*/
IFX_return_t TAPI_LL_Phone_AalProfile (TAPI_CONNECTION *pChannel,
                                      IFX_TAPI_PCK_AAL_PROFILE_t const *pProfile)
{
   VINETIC_DEVICE  *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   IFX_int32_t      ret  = IFX_SUCCESS, i, nEnc, nMax = 0;
   IFX_uint16_t     pCmd [2], pData[14], nUUI;

   if (pProfile->rows < 1 || pProfile->rows > 10)
      return IFX_ERROR;

   memset (pData, 0, sizeof(IFX_uint16_t) * 14);
   pCmd [0] = (CMD1_EOP | pChannel->nChannel);
   pCmd [1] = ECMD_COD_CHAAL | 2;
   /* read first both entries */
   ret = CmdRead (pDev, pCmd, pData, 2);
   for (i = 0; i < pProfile->rows; i++)
   {
      nEnc = getEncoder ((IFX_int32_t)pProfile->codec[i]);
      if (nEnc == IFX_ERROR)
         return IFX_ERROR;
      switch (pProfile->nUUI[i])
      {
         case IFX_TAPI_PKT_AAL_PROFILE_RANGE_12_15:
         case IFX_TAPI_PKT_AAL_PROFILE_RANGE_8_11:
         case IFX_TAPI_PKT_AAL_PROFILE_RANGE_4_7:
         case IFX_TAPI_PKT_AAL_PROFILE_RANGE_0_3:
            nMax = 4;
         break;
         case IFX_TAPI_PKT_AAL_PROFILE_RANGE_0_7:
         case IFX_TAPI_PKT_AAL_PROFILE_RANGE_8_15:
            nMax = 2;
         break;
         default:
            nMax = 0;
            nUUI = 0;
         break;
      }

      switch (pProfile->nUUI[i])
      {
         case IFX_TAPI_PKT_AAL_PROFILE_RANGE_12_15:
            nUUI = 3 * 4;
         break;
         case IFX_TAPI_PKT_AAL_PROFILE_RANGE_8_11:
            nUUI = 2 * 4;
         break;
         case IFX_TAPI_PKT_AAL_PROFILE_RANGE_4_7:
            nUUI = 4;
         break;
         case IFX_TAPI_PKT_AAL_PROFILE_RANGE_8_15:
            nUUI = 8;
         break;
         default:
            nUUI = 0;
         break;
      }
      pData[i + 4] = (IFX_uint16_t)((IFX_uint8_t)pProfile->len[i] << 10) |
                     (IFX_uint16_t)(nUUI << 6) |
                     (IFX_uint16_t)((IFX_uint8_t)nEnc);
   }
   /* clear the UUIS field and retain SQNR-INTV */
   pData[3] &= ~COD_CHAAL_UUIS;
   /* set the UUI subrange parameter according to the maximum ranges */
   switch (nMax)
   {
      case  0:
         pData[3] |= COD_CHAAL_UUI_1RANGE;
      break;
      case  1:
         pData[3] |= COD_CHAAL_UUI_2RANGES;
      break;
      case  4:
         pData[3] |= COD_CHAAL_UUI_4RANGES;
      break;
      case  8:
         pData[3] |= COD_CHAAL_UUI_8RANGES;
      break;
      case  16:
         pData[3] |= COD_CHAAL_UUI_16RANGES;
      break;
      default:
         SET_ERROR (ERR_INVALID);
         return IFX_ERROR;
   }
   ret = CmdWrite (pDev, pData, 12);
   return ret;
}

/**
  Enables or Disables voice coder channel according to nMode
\param pChannel        Handle to TAPI_CONNECTION structure
\param nMode           0: off, 1: on
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   This function is not a TAPI low level function because it is not needed
   by TAPI.
   To succesfully modify the coder channel settings, at least the two first
   command Words should be programed.
   Function should protect access tpo fw messages
   After activating the coder activate stored ET bits: DTMF receiver and
   generator, ATD, UTD. For FW_ETU Sig-Ch and no generator is touched.
   Remember all ET bits and switch ET bits to zero: DTMF receiver and
   generator, ATD, UTD. For ETU Sig-Ch and no generator.
   Reset the signaling channel with Dsp_SigReset.
   This is neccessary to initialze the event playout unit. Otherwise the
   sequence number may not match for the new connection and events could
   be played out.
*******************************************************************************/
IFX_return_t TAPI_LL_Voice_Enable (TAPI_CONNECTION * pChannel, IFX_uint8_t nMode)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   IFX_uint8_t ch = pChannel->nChannel;
   FWM_COD_CH *pCodCh = &pDev->pCodCh[ch].cod_ch;
   IFX_int32_t ret = IFX_SUCCESS, bSet = IFX_FALSE, i;

   /* protect fwmsg against concurrent tasks */
   IFXOS_MutexLock (pDev->memberAcc);

   if ((pChannel->nChannel == 3) && (nMode == 1) &&
       (pDev->nChipMajorRev == VINETIC_V1x))
   {
      /* check if any CPTD is activated */
      for (i = 0; i < pDev->nMaxRes; i++)
      {
         if (VIN_ECMD_SIGCPT_EN_GET (pDev->pSigCh[i].nCpt) == 1)
         {
            /* unlock */
            IFXOS_MutexUnlock (pDev->memberAcc);
            SET_ERROR(ERR_NORESOURCE);
            return IFX_ERROR;
         }
      }
   }
   /* switch on */
   if ((nMode == 1) && !(pCodCh->bit.en))
   {
      pCodCh->bit.en = 1;
      bSet = IFX_TRUE;
      /* switch signaling channel on */
      ret = Dsp_SigActStatus (pDev, ch, IFX_TRUE, CmdWrite);
#ifndef FW_ETU
      /* always activate the DTMF generator */
      if (ret == IFX_SUCCESS &&
          pDev->pSigCh[ch].sig_dtmfgen.bit.en == 0)
      {
         pDev->pSigCh[ch].sig_dtmfgen.bit.en = 1;
         ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_dtmfgen.value,
                         CMD_SIG_DTMFGEN_LEN);
      }
#endif /* FW_ETU */
   }
   /* switch off */
   else if ((nMode == 0) && (pCodCh->bit.en))
   {
      SIG_MOD_STATE state;
      state.value = 0x0;
      pCodCh->bit.en = 0;
      bSet = IFX_TRUE;

      /***********************************************************************
       switch off ET of every algorithm
       ***********************************************************************/
      ret = Dsp_UpdateEventTrans (pDev, ch, state, IFX_NULL);
#ifndef FW_ETU
      if (ret == IFX_SUCCESS &&
          pDev->pSigCh[ch].sig_dtmfgen.bit.en == 1)
      {
         pDev->pSigCh[ch].sig_dtmfgen.bit.en = 0;
         ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_dtmfgen.value,
                         CMD_SIG_DTMFGEN_LEN);
      }
#endif /* FW_ETU */
      if (ret == IFX_SUCCESS)
         ret = Dsp_SigActStatus (pDev, ch, IFX_FALSE, CmdWrite);
   }
   if (ret == IFX_SUCCESS && bSet)
      ret = CmdWrite (pDev, (IFX_uint16_t *) pCodCh, 2);

   /* activate the event transmission if configured */
   if (ret == IFX_SUCCESS && nMode == 1 &&
       pDev->pSigCh[ch].et_stat.value != 0)
   {
      ret = Dsp_UpdateEventTrans (pDev, ch, pDev->pSigCh[ch].et_stat, IFX_NULL);
   }

   /* unlock */
   IFXOS_MutexUnlock (pDev->memberAcc);

   return ret;
}

/**
   Connect a data channel to an analog phone device
\param pChannel    Handle to TAPI_CONNECTION structure
\param pMap        Handle to IFX_TAPI_MAP_DATA_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   By definition a data channel consists of the COD-module and the SIG-module
   of the same TAPI connection.
*/
IFX_return_t TAPI_LL_Data_Channel_Add (TAPI_CONNECTION *pChannel,
                                      IFX_TAPI_MAP_DATA_t const *pMap)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   IFX_uint32_t nThisCh = pChannel->nChannel;
   int err, cnt = 0, i, j;
   VINDSP_MODULE *pDstMod = IFX_NULL,
                 *tmpSigs[MAX_MODULE_SIGNAL_INPUTS] = {IFX_NULL};
   VINDSP_MODULE_SIGNAL *pInput = IFX_NULL;

   switch (pMap->nChType)
   {
   case IFX_TAPI_MAP_DATA_TYPE_CODER:
      SET_ERROR (ERR_NOTSUPPORTED);
      return IFX_ERROR;
   case IFX_TAPI_MAP_DATA_TYPE_PCM:
      if (pMap->nDstCh < pDev->nPcmCnt)
         pDstMod = &pDev->pPcmCh[pMap->nDstCh].signal;
      break;
   case IFX_TAPI_MAP_DATA_TYPE_PHONE:
   case IFX_TAPI_MAP_DATA_TYPE_DEFAULT:
   default:
      if (pMap->nDstCh < pDev->nAnaChan)
         pDstMod = &pDev->pAlmCh[pMap->nDstCh].signal;
      break;
   }
   if ((pDstMod == IFX_NULL) || (nThisCh >= pDev->nSigCnt))
   {
      SET_ERROR (ERR_FUNC_PARM);
      return IFX_ERROR;
   }
   /* Here pDstMod points to a valid module which should be connected to the
      data channel. It can either be an ALM or an PCM module. */

   /* Verify that this is a data channel by making sure SIG is getting it's
      input from the COD-module of the same channel. */
   if (pDev->pSigCh[nThisCh].signal.in[REMOTE_SIG_IN].i !=
       pDev->pCodCh[nThisCh].signal.nSignal)
   {
      SET_ERROR (ERR_WRONG_CHANNEL_MODE);
      return IFX_ERROR;
   }

   /* connect SIG output to Dst (ALM or PCM) input, if not already done */
   err = Con_ConnectPrepare (&pDev->pSigCh[nThisCh].signal, pDstMod,
                             LOCAL_SIG_OUT);
   if (err == IFX_SUCCESS)
   {
      /* Connect the Dst (ALM or PCM) to the data channel, if not already done.
         If the upstream input (I1) of the signalling module is already in
         use by another ALM or PCM output connect the new output to the next
         free upstream input on the coder of the same data channel instead. */
      if (pDev->pSigCh[nThisCh].signal.in[LOCAL_SIG_IN].i == ECMD_IX_EMPTY)
      {
         /* connect Dst (ALM or PCM) output to SIG input (I1) */
         err = Con_ConnectPrepare (pDstMod, &pDev->pSigCh[nThisCh].signal, 0);
      }
      else
      {
         /* Connect the Dst (ALM or PCM) output to COD input because the
            SIG input (I1) is already in use. This happens when adding more
            than one phone/PCM to a data channel. But prevent connecting
            Dst to the coder when it is already connected to signalling. */
         if (pDev->pSigCh[nThisCh].signal.in[LOCAL_SIG_IN].i
             != pDstMod->nSignal)
           err = Con_ConnectPrepare (pDstMod, &pDev->pCodCh[nThisCh].signal,0);
      }
   }

   if (err == IFX_SUCCESS)
   {
      /* Check if the Dst (ALM or PCM) is now connected to more than one
         data channel. If this is the case then connect these data channels
         so that all the data channels can talk to each other */

      /* Count to how many data channels the analog module is connected
         and store a pointer to each signalling module in a temporary list. */
      for (pInput = pDstMod->pInputs, cnt = 0;
           pInput != IFX_NULL; pInput = pInput->pNext )
      {
         if (pInput->pParent->nModType == VINDSP_MT_SIG)
            tmpSigs[cnt++] = pInput->pParent;
         if (pInput->pParent->nModType == VINDSP_MT_COD)
            tmpSigs[cnt++] = pInput->pParent->pInputs->pParent;
      }

      if (cnt > 1)
      {
         /* Conference with more than one data channel. So now connect each
            data channel to all the others that are stored in the list above.
            For this each signaling output is connected to one input on every
            coder taking part in the conference. ConnectPrepare may be called
            even if the input is already connected. */
         for (i = 0; i <  cnt; i++)
            for (j = 0; j < cnt; j++)
               if (i != j)
                  err = Con_ConnectPrepare (tmpSigs[i],
                                            tmpSigs[j]->pInputs->pParent,
                                            LOCAL_SIG_OUT);
         /* PS: The pointer is implicitly valid by a check done above */
      }
   }

   if (err == IFX_SUCCESS)
       err = Con_ConnectConfigure (pDev);
   else
   {
       SET_ERROR(ERR_NO_FREE_INPUT_SLOT);
   }

   return err;
}

/**
   Removes a data channel from an analog phone device
\param pChannel    Handle to TAPI_CONNECTION structure
\param pMap        Handle to IFX_TAPI_MAP_DATA_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   Does now support conferencing
*/
IFX_return_t TAPI_LL_Data_Channel_Remove (TAPI_CONNECTION *pChannel,
                                         IFX_TAPI_MAP_DATA_t const *pMap)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   IFX_uint32_t nThisCh = pChannel->nChannel;
   IFX_int32_t err = IFX_SUCCESS, i, cnt = 0, still_required;
   VINDSP_MODULE *pTmpMod;
   VINDSP_MODULE_SIGNAL *pInput = IFX_NULL, *pDstInput;
   VINDSP_MODULE *pDstMod = IFX_NULL,
                 *tmpSigs[MAX_MODULE_SIGNAL_INPUTS] = {IFX_NULL};

   switch (pMap->nChType)
   {
   case IFX_TAPI_MAP_DATA_TYPE_CODER:
      SET_ERROR (ERR_NOTSUPPORTED);
      return IFX_ERROR;
   case IFX_TAPI_MAP_DATA_TYPE_PCM:
      if (pMap->nDstCh < pDev->nPcmCnt)
         pDstMod = &pDev->pPcmCh[pMap->nDstCh].signal;
      break;
   case IFX_TAPI_MAP_DATA_TYPE_PHONE:
   case IFX_TAPI_MAP_DATA_TYPE_DEFAULT:
   default:
      if (pMap->nDstCh < pDev->nAnaChan)
         pDstMod = &pDev->pAlmCh[pMap->nDstCh].signal;
      break;
   }
   if ((pDstMod == IFX_NULL) || (nThisCh >= pDev->nSigCnt))
   {
      SET_ERROR (ERR_FUNC_PARM);
      return IFX_ERROR;
   }

   /* disconnect SIG output from Dst (ALM or PCM) input */
   err = Con_DisconnectPrepare (&pDev->pSigCh[nThisCh].signal,
                                pDstMod, LOCAL_SIG_OUT);
   if (err == IFX_SUCCESS)
   {
      /* Disconnect Dst from either the COD or SIG module depending on where
         the output of the Dst is connected to. */
      if (pDev->pSigCh[nThisCh].signal.in[LOCAL_SIG_IN].i == pDstMod->nSignal)
         /* disconnect Dst (ALM or PCM) output from SIG input */
         err = Con_DisconnectPrepare (pDstMod,
                                      &pDev->pSigCh[nThisCh].signal, 0);
      else
         /* disconnect Dst (ALM or PCM) output from COD input */
         err = Con_DisconnectPrepare (pDstMod,
                                      &pDev->pCodCh[nThisCh].signal, 0);
   }

   /* Now the local input on the SIG on the data channel may be empty
      so reroute a signal from an input of the COD on the same channel when
      possible. Do not reroute signals coming from signalling modules */
   for (i = 0;
        err == IFX_SUCCESS && i <  MAX_MODULE_SIGNAL_INPUTS &&
        pDev->pSigCh[nThisCh].signal.in[LOCAL_SIG_IN].i == ECMD_IX_EMPTY; i++)
   {
      pTmpMod = pDev->pCodCh[nThisCh].signal.in[i].pOut;
      if (pTmpMod != IFX_NULL && pTmpMod->nModType != VINDSP_MT_SIG)
      {
         /* disconnect Dst (ALM or PCM) output from COD input */
         err = Con_DisconnectPrepare (pTmpMod,
                                      &pDev->pCodCh[nThisCh].signal, 0);
         if (err == IFX_SUCCESS)
            /* connect Dst (ALM or PCM) output to SIG input (I1) */
            err = Con_ConnectPrepare (pTmpMod,
                                      &pDev->pSigCh[nThisCh].signal, 0);
      }
   }

   /* Finally the output of our signalling module may still be connected to
      the coder on other data channels because the channels have been
      connected to the Dst (ALM or PCM) module that we just disconnected.
      So now check all data channels that we are connected to because of the
      Dst and if the connection is still needed because we are still in a
      conference where another ALM or PCM module connects our data channel
      with another one. */

   /* Store a pointer to each signalling module in a data channel that
      the Dst (ALM or PCM) still connects to in a temporary list. */
   for (pInput = pDstMod->pInputs, cnt = 0;
        pInput != IFX_NULL; pInput = pInput->pNext )
   {
      if (pInput->pParent->nModType == VINDSP_MT_SIG)
         tmpSigs[cnt++] = pInput->pParent;
      if (pInput->pParent->nModType == VINDSP_MT_COD)
         tmpSigs[cnt++] = pInput->pParent->pInputs->pParent;
   }
   /* Loop over the list just built */
   for (i = 0, still_required = 0;
        err == IFX_SUCCESS && i <  cnt && !still_required; i++)
   {
      /* Loop over all modules connecting to the data channel */
      for (pInput = tmpSigs[i]->pInputs2;
           pInput != IFX_NULL; pInput = pInput->pNext)
      {
         /* Skip COD modules connecting to the data channel */
         if (pInput->pParent->nModType == VINDSP_MT_COD)
            continue;
         /* For remaining modules check if any of the inputs connecting to
            the output is the signalling module we started with. If so then
            mark the connection as still required. */
         for (pDstInput = pInput->pParent->pInputs;
              pDstInput != IFX_NULL; pDstInput = pDstInput->pNext)
            if (pDstInput->pParent == &pDev->pSigCh[nThisCh].signal ||
                pDstInput->pParent == &pDev->pCodCh[nThisCh].signal   )
            {
               still_required = 1;
               break;
            }
      }
      /* If no longer required disconnect the data channels */
      if (! still_required)
      {
         err = Con_DisconnectPrepare (&pDev->pSigCh[nThisCh].signal,
                                      tmpSigs[i]->pInputs->pParent,
                                      LOCAL_SIG_OUT);
         if (err == IFX_SUCCESS)
            err = Con_DisconnectPrepare (tmpSigs[i],
                                         &pDev->pCodCh[nThisCh].signal,
                                         LOCAL_SIG_OUT);
      }
   }

   /* write the configuration to the chip */
   if (err == IFX_SUCCESS)
       err = Con_ConnectConfigure (pDev);
   else
   {
      SET_ERROR (ERR_CON_INVALID);
   }
   return err;
}

/**
   Connect an analog phone channel to another  (ALM or PCM)
\param pChannel    Handle to TAPI_CONNECTION structure
\param pMap        Handle to IFX_TAPI_MAP_PHONE_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
*/
IFX_return_t TAPI_LL_Phone_Channel_Add (TAPI_CONNECTION *pChannel,
                                        IFX_TAPI_MAP_PHONE_t const *pMap)
{
   return TAPI_LL_Local_Module_Connect (IFX_TAPI_CONN_ACTION_CREATE,
                                        (VINETIC_DEVICE *)pChannel->pDevice,
                                        pChannel->nChannel, IFX_TAPI_MAP_TYPE_PHONE,
                                        pMap->nPhoneCh, pMap->nChType);
}

/**
   Disconnect analog phone channels
\param pChannel    Handle to TAPI_CONNECTION structure
\param pMap        Handle to IFX_TAPI_MAP_PHONE_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
*/
IFX_return_t TAPI_LL_Phone_Channel_Remove (TAPI_CONNECTION *pChannel,
                                          IFX_TAPI_MAP_PHONE_t const *pMap)
{
   return TAPI_LL_Local_Module_Connect (IFX_TAPI_CONN_ACTION_REMOVE,
                                        (VINETIC_DEVICE *)pChannel->pDevice,
                                        pChannel->nChannel, IFX_TAPI_MAP_TYPE_PHONE,
                                        pMap->nPhoneCh, pMap->nChType);
}

/**
   Adds a PCM module to either an analog phone module or another PCM module
\param pChannel    Handle to TAPI_CONNECTION structure
\param pMap        Handle to IFX_TAPI_MAP_PCM_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
*/
IFX_return_t TAPI_LL_PCM_Channel_Add (TAPI_CONNECTION *pChannel,
                                     IFX_TAPI_MAP_PCM_t const *pMap)
{
   return TAPI_LL_Local_Module_Connect (IFX_TAPI_CONN_ACTION_CREATE,
                                        (VINETIC_DEVICE *)pChannel->pDevice,
                                        pChannel->nChannel, IFX_TAPI_MAP_TYPE_PCM,
                                        pMap->nDstCh, pMap->nChType);
}

/**
   Remove a PCM module from either an analog phone module or another PCM module
\param pChannel    Handle to TAPI_CONNECTION structure
\param pMap        Handle to IFX_TAPI_MAP_PCM_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
*/
IFX_return_t TAPI_LL_PCM_Channel_Remove (TAPI_CONNECTION *pChannel,
                                        IFX_TAPI_MAP_PCM_t const *pMap)
{
   return TAPI_LL_Local_Module_Connect (IFX_TAPI_CONN_ACTION_REMOVE,
                                        (VINETIC_DEVICE *)pChannel->pDevice,
                                        pChannel->nChannel, IFX_TAPI_MAP_TYPE_PCM,
                                        pMap->nDstCh, pMap->nChType);
}

/**
   Create or remove a symetric connection between two local modules
   (PCM or ALM) on the same device
\param nAction     Action to be performed IFX_TAPI_CONN_ACTION_t
\param pDev        Pointer to the Vinetic device structure
\param nCh1        Resource number of the first module
\param nChType1    Which module type to use from channel 1
\param nCh2        Resource number of the second module
\param nChType2    Which module type to use from channel 2
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
*/
IFX_LOCAL
IFX_return_t TAPI_LL_Local_Module_Connect (IFX_TAPI_CONN_ACTION_t nAction,
                                           VINETIC_DEVICE *pDev,
                                           IFX_uint32_t nCh1,
                                           IFX_TAPI_MAP_TYPE_t nChType1,
                                           IFX_uint32_t nCh2,
                                           IFX_TAPI_MAP_TYPE_t nChType2)
{
   VINDSP_MODULE  *pMod1 = IFX_NULL,
                  *pMod2 = IFX_NULL;
   IFX_int32_t    err = IFX_SUCCESS;

   /* get pointers to the signal struct of
      the selected modules in the channels */
   switch (nChType1)
   {
   case IFX_TAPI_MAP_TYPE_CODER:
      SET_ERROR (ERR_NOTSUPPORTED);
      return IFX_ERROR;
   case IFX_TAPI_MAP_TYPE_PCM:
      if (nCh1 < pDev->nPcmCnt)
         pMod1 = &pDev->pPcmCh[nCh1].signal;
      break;
   case IFX_TAPI_MAP_TYPE_PHONE:
   case IFX_TAPI_MAP_TYPE_DEFAULT:
   default:
      if (nCh1 < pDev->nAnaChan)
         pMod1 = &pDev->pAlmCh[nCh1].signal;
      break;
   }

   switch (nChType2)
   {
   case IFX_TAPI_MAP_TYPE_CODER:
      SET_ERROR (ERR_NOTSUPPORTED);
      return IFX_ERROR;
   case IFX_TAPI_MAP_TYPE_PCM:
      if (nCh2 < pDev->nPcmCnt)
         pMod2 = &pDev->pPcmCh[nCh2].signal;
      break;
   case IFX_TAPI_MAP_TYPE_PHONE:
   case IFX_TAPI_MAP_TYPE_DEFAULT:
   default:
      if (nCh2 < pDev->nAnaChan)
         pMod2 = &pDev->pAlmCh[nCh2].signal;
      break;
   }

   if ((pMod1 == IFX_NULL) || (pMod2 == IFX_NULL))
   {
      SET_ERROR (ERR_FUNC_PARM);
      return IFX_ERROR;
   }

   /* now pMod1 and pMod2 point to the modules to be connected/disconnected */

   if (nAction == IFX_TAPI_CONN_ACTION_CREATE)
   {
      /* connect the forward direction */
      err = Con_ConnectPrepare (pMod1, pMod2, 0);
      if (err == IFX_SUCCESS)
      {
         /* connect the reverse direction */
         err = Con_ConnectPrepare (pMod2, pMod1, 0);
      }
      if (err != IFX_SUCCESS)
      {
         SET_ERROR(ERR_NO_FREE_INPUT_SLOT);
      }
   }
   if (nAction == IFX_TAPI_CONN_ACTION_REMOVE)
   {
      /* disconnect the forward direction */
      err = Con_DisconnectPrepare (pMod1, pMod2, 0);
      if (err == IFX_SUCCESS)
      {
         /* disconnect the reverse direction */
         err = Con_DisconnectPrepare (pMod2, pMod1, 0);
      }
      if (err != IFX_SUCCESS)
      {
         SET_ERROR(ERR_CON_INVALID);
      }
   }

   if (err == IFX_SUCCESS)
      err = Con_ConnectConfigure (pDev);

   return err;
}
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */


#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
/**
   Configures the Datapump for Modulation
\param pChannel          Handle to TAPI_CONNECTION structure
\param pFaxMod           Handle to IFX_TAPI_T38_MOD_DATA_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   -  This function has also to map the Fax_Downstream ptr to the write
 interface
   -  The configuration of the Modulator requires that the datapump is disable.
      So the followings are done here :
      1- Disable datapump if enable
      2- Set Modulator
      3- Map read interface to Fax_Downstream
      4- Activate datapump for Modulation and
         set other datapump parameters related to Modulation
   - This operation uses a data channel. Any reference to phone channel should
     be done with an instance of a phone channel pointer
*/
IFX_return_t TAPI_LL_FaxT38_SetModulator (TAPI_CONNECTION *pChannel,
                                         IFX_TAPI_T38_MOD_DATA_t const *pFaxMod)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t ret;

   if (pDev->pCodCh[pChannel->nChannel].cod_ch.bit.en)
   {
      SET_ERROR (ERR_COD_RUNNING);
      return IFX_ERROR;
   }
   /* disable datapump */
   ret = Dsp_SetDatapump (pCh, IFX_FALSE, IFX_FALSE, 0, 0, 0, 0);

   /* set command to configure the modulator */
   if (ret == IFX_SUCCESS)
      ret = Dsp_SetDPMod (pCh, pFaxMod->nStandard, pFaxMod->nSigLen,
                          pFaxMod->nDbm, pFaxMod->nTEP, pFaxMod->nTraining);
   else
   {
      pChannel->TapiFaxStatus.nStatus &= ~ (IFX_TAPI_FAX_T38_TX_ON |
                                            IFX_TAPI_FAX_T38_DP_ON);
      /* set datapump failed, no further action required */
      return ret;
   }
   if (ret == IFX_SUCCESS)
   {
      /* map write interface for fax support */
      pCh->if_write = Fax_DownStream;
      /* set fax state as running */
      pChannel->TapiFaxStatus.nStatus |= IFX_TAPI_FAX_T38_TX_ON;
      /* configure and start the datapump for modulation */
      ret = Dsp_SetDatapump (pCh, IFX_TRUE, IFX_FALSE, pFaxMod->nGainTx,
                             pFaxMod->nMobsm, pFaxMod->nMobrd, 0);
   }
   /* handle error case: Continue with voice */
   if (ret == IFX_ERROR)
   {
      ret = Dsp_SetDatapump (pCh, IFX_FALSE, IFX_FALSE, 0, 0, 0, 0);
      pCh->if_write   = VoIP_DownStream;
      pChannel->TapiFaxStatus.nStatus &= ~IFX_TAPI_FAX_T38_TX_ON;
      /* set error and issue tapi exception */
      TAPI_FaxT38_Event_Update (pChannel, 0, IFX_TAPI_FAX_T38_ERROR_SETUP);
   }
   else
      pChannel->TapiFaxStatus.nStatus |= IFX_TAPI_FAX_T38_DP_ON;

   return ret;
}

/**
   Configures the Datapump for Demodulation
\param pChannel          Handle to TAPI_CONNECTION structure
\param pFaxDemod         Handle to IFX_TAPI_T38_DEMOD_DATA_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   -  This function has also to map the Fax_UpStream ptr to the read interface
      and reset the data fifo
   -  The configuration of the Demodulator requires that the datapump is
 disable.
      So the followings are done here :
         1- Disable datapump if enable
         2- Set Demodulator
         3- Map read interface to Fax_UpStream
         4- Activate datapump for Demodulation and
            set other datapump parameters related to Demodulation
   - This operation uses a data channel. Any reference to phone channel should
     be done with an instance of a phone channel pointer
*/
IFX_return_t TAPI_LL_FaxT38_SetDemodulator (TAPI_CONNECTION *pChannel,
                                           IFX_TAPI_T38_DEMOD_DATA_t const *pFaxDemod)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t ret;

   if (pDev->pCodCh[pChannel->nChannel].cod_ch.bit.en)
   {
      SET_ERROR (ERR_COD_RUNNING);
   	return IFX_ERROR;
   }
   /* Disable Channel Fax datapump */
   ret = Dsp_SetDatapump (pCh, IFX_FALSE, IFX_FALSE, 0, 0, 0, 0);

   /* set command to configure the demodulator */
   if (ret == IFX_SUCCESS)
      ret = Dsp_SetDPDemod (pCh, pFaxDemod->nStandard1, pFaxDemod->nStandard2,
                            pFaxDemod->nEqualizer, pFaxDemod->nTraining);
   else
      /* set datapump failed, no further action required */
      return ret;

   if (ret == IFX_SUCCESS)
   {
      /* Clear fifo for this channel to record valid fax data */
      Vinetic_IrqLockDevice (pDev);
      Fifo_Clear (&pCh->rdFifo);
      /* map write interface for fax support */
      pCh->if_read = Fax_UpStream;
      /* set fax state as running */
      pChannel->TapiFaxStatus.nStatus |= IFX_TAPI_FAX_T38_TX_ON;
      Vinetic_IrqUnlockDevice (pDev);
      /* configure the datapump for demodulation */
      ret = Dsp_SetDatapump (pCh, IFX_TRUE, IFX_TRUE, pFaxDemod->nGainRx,
                             0, 0, pFaxDemod->nDmbsd);
   }
   else
   {
      pChannel->TapiFaxStatus.nStatus &= ~ (IFX_TAPI_FAX_T38_TX_ON |
                                            IFX_TAPI_FAX_T38_DP_ON);
      /* SetDPDemod failed, no further action required */
      return ret;
   }
   /* configure the datapump for demodulation */
   if (ret == IFX_ERROR)
   {
      /* handle error case: continue with voice */
      ret = Dsp_SetDatapump (pCh, IFX_FALSE, IFX_FALSE, 0, 0, 0, 0);
      Vinetic_IrqLockDevice (pDev);
      pCh->if_read   = VoIP_UpStream;
      /* from now on, fax is running */
      pChannel->TapiFaxStatus.nStatus = 0;
      Vinetic_IrqUnlockDevice (pDev);
      /* set error and issue tapi exception */
      TAPI_FaxT38_Event_Update (pChannel, 0, IFX_TAPI_FAX_T38_ERROR_SETUP);
   }
   else
      pChannel->TapiFaxStatus.nStatus |= IFX_TAPI_FAX_T38_DP_ON;

   return ret;
}

/**
   disables the Fax datapump
\param pChannel          Handle to TAPI_CONNECTION structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   -  This function has to remap the VoIp_UpStream and VoIP_Downstream to
      the driver read and write interfaces
   -  This operation uses a data channel. Any reference to phone channel should
      be done with an instance of a phone channel pointer
*/
IFX_return_t TAPI_LL_FaxT38_DisableDataPump (TAPI_CONNECTION *pChannel)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t ret = IFX_SUCCESS;

   /* Disable Channel Fax datapump */
   ret = Dsp_SetDatapump (pCh, IFX_FALSE, IFX_FALSE, 0, 0, 0, 0);
   /* activate voice path and make driver ready for a next fax connection */
   if (ret == IFX_SUCCESS)
   {
      /* reset status /error flags */
      pChannel->TapiFaxStatus.nStatus = 0;
      pChannel->TapiFaxStatus.nError = 0;
      /* remap read/write interfaces */
      pCh->if_write = VoIP_DownStream;
      pCh->if_read  = VoIP_UpStream;
   }
   else
   {
      /* set error and issue tapi exception */
      TAPI_FaxT38_Event_Update (pChannel, 0, IFX_TAPI_FAX_T38_ERROR_DATAPUMP);
      TRACE (TAPI_DRV, DBG_LEVEL_HIGH,
             ("DRV_ERR: Disable Datapump" " failed\n\r"));
   }

   return ret;
}

/**
   query Fax Status
\param pChannel          Handle to TAPI_CONNECTION structure
\param pFaxStatus        Handle to IFX_TAPI_T38_STATUS_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   -  This operation uses a data channel. Any reference to phone channel should
      be done with an instance of a phone channel pointer
*/
IFX_return_t TAPI_LL_FaxT38_GetStatus (TAPI_CONNECTION *pChannel,
                                      IFX_TAPI_T38_STATUS_t *pFaxStatus)
{
   /* read status from cache structure */
   *pFaxStatus = pChannel->TapiFaxStatus;

   return IFX_SUCCESS;
}
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38) */

/**
   Returns Vinetic's number of capability lists
\return
   The amount of capabilitie entries
\remarks
   VINETIC_CAP_Table can be updated without any changes in TAPI functions
   concerning capabilities' services.
*/
IFX_uint32_t TAPI_LL_Phone_Get_Capabilities (TAPI_CONNECTION *pChannel)
{
   return ((VINETIC_DEVICE *)pChannel->pDevice)->nMaxCaps;
}

/**
   returns Vinetic's capability lists
\param pChannel          Handle to TAPI_CONNECTION structure
\param pCapList        Handle to IFX_TAPI_CAP_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   The whole set of VINETIC capabilities is copied to the passed pointer
   argument.
*/
IFX_return_t TAPI_LL_Phone_Get_Capability_List (TAPI_CONNECTION *pChannel,
                                               IFX_TAPI_CAP_t *pCapList)
{
   VINETIC_DEVICE  *pDev   = (VINETIC_DEVICE *)pChannel->pDevice;

   memcpy(pCapList, pDev->CapList,
          ((IFX_uint32_t)pDev->nMaxCaps * sizeof(IFX_TAPI_CAP_t)));
   return IFX_SUCCESS;
}

/**
   Checks if a specific capability is supported from VINETIC
\param pChannel    Handle to TAPI_CONNECTION structure
\param pCapList    Handle to IFX_TAPI_CAP_t structure
\return Support status of the capability
   - 0 if not supported
   - 1 if supported
\remarks
   This function compares only the captype and the cap members of the given
   IFX_TAPI_CAP_t structure with the ones of the VINETIC_CAP_Table.
*/
IFX_int32_t TAPI_LL_Phone_Check_Capability (TAPI_CONNECTION *pChannel,
                                            IFX_TAPI_CAP_t *pCapList)
{
   IFX_int32_t cnt;
   IFX_int32_t ret = 0;
   VINETIC_DEVICE  *pDev   = (VINETIC_DEVICE *)pChannel->pDevice;

   /* do checks */
   for (cnt = 0; cnt < pDev->nMaxCaps; cnt++)
   {
      if (pCapList->captype == pDev->CapList[cnt].captype)
      {
         switch (pCapList->captype)
         {
         /* Handle number counters, cap is returned */
         case IFX_TAPI_CAP_TYPE_PCM:
         case IFX_TAPI_CAP_TYPE_CODECS:
         case IFX_TAPI_CAP_TYPE_PHONES:
         case IFX_TAPI_CAP_TYPE_T38:
            pCapList->cap = pDev->CapList[cnt].cap;
            ret = 1;
            break;
         default:
            /* default IFX_TRUE or IFX_FALSE capabilities */
            if (pCapList->cap == pDev->CapList[cnt].cap)
               ret = 1;
            break;
         }
      }
   }
   return ret;
}

/**
   Sets the frame length for the audio packets
\param pChannel        Handle to TAPI_CONNECTION structure
\param nFrameLength    length of frames in milliseconds
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   This function sets the frame length only when the requested value
   is valid in relation to the pre-existing Codec value of the Coder Channel
   configuration. Coder channel module remains either disabled or enabled
   after the completion of this function, depending on its previous state.
*/
IFX_return_t TAPI_LL_Phone_Set_Frame_Length (TAPI_CONNECTION *pChannel,
                                            IFX_int32_t nFrameLength)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE  *pDev   = (VINETIC_DEVICE *)pChannel->pDevice;
   FWM_COD_CH      *pCodCh = &pDev->pCodCh[pChannel->nChannel].cod_ch;
   IFX_int32_t      ret    = IFX_SUCCESS, nPte = 0;

   /* check coder module */
   if (pDev->nDevState & DS_COD_EN)
   {
      /* set packet time according to frame length */
      nPte = getPTE (nFrameLength);
   }
   else
   {
      ret = IFX_ERROR;
      SET_ERROR(ERR_CODCONF_NOTVALID);
   }

   /* check if PTE and pre existing ENC match together */
   if ((ret == IFX_SUCCESS) && (nPte != IFX_ERROR))
      ret = CheckENCnPTE ((IFX_uint8_t)nPte, (IFX_uint8_t)pCodCh->bit.enc);
   else
      ret = IFX_ERROR;
   /*  set the requested PTE value*/
   if (ret == IFX_SUCCESS)
   {
      IFXOS_MutexLock (pDev->memberAcc);
      pCodCh->bit.pte = (IFX_uint32_t) nPte;
      ret = CmdWrite (pDev, pCodCh->value, 2);
      IFXOS_MutexUnlock (pDev->memberAcc);
   }

   return ret;
#else
   return IFX_ERROR;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
}

/**
   Retrieves the saved frame length for the audio packets, as saved when applied
   to the channel
\param pChannel        Handle to TAPI_CONNECTION structure
\param pFrameLength    Handle to length of frames in milliseconds
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
*/
IFX_return_t TAPI_LL_Phone_Get_Frame_Length (TAPI_CONNECTION *pChannel,
                                            IFX_int32_t *pFrameLength)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE  *pDev   = (VINETIC_DEVICE *)pChannel->pDevice;
   FWM_COD_CH      *pCodCh = &pDev->pCodCh[pChannel->nChannel].cod_ch;
   IFX_int32_t      nFrameLength;

   /* set frame length according to stored PTE */
   nFrameLength = getFrameLength (pCodCh->bit.pte);
   if (nFrameLength == IFX_ERROR)
   {
      LOG (TAPI_DRV,DBG_LEVEL_HIGH,
          ("\n\rDRV_ERROR: PTE value has no corresponding frame length.\n\r"));
      return IFX_ERROR;
   }
   *pFrameLength = nFrameLength;
   return IFX_SUCCESS;
#else
   return IFX_ERROR;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
}

/**
   Controls the DTMF sending mode. The DTMF may be sent out-of-band (OOB)
   or in-band.
\param pChannel  Handle to TAPI_CONNECTION structure
\param nOobMode  Mode of the inband and out of band transmission of
                 RFC2883 event packets
          - 0:  IFX_TAPI_PKT_EV_OOB_ALL, DTMF is send in-band and out-ofband
          - 1: IFX_TAPI_PKT_EV_OOB_ONLY , DTMF is send only outof band
          - 2: IFX_TAPI_PKT_EV_OOB_NO, DTMF is send only inband
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   This function sets the AS bit (Auto Suppression) in DTMF Receiver's
   configuration when the DTMF sending only out-of-band is requested. In the
   other case, AS bit is set to zero. In both cases ET bit (Event Transmission
   Support) is set. In-band DTMF sending mode is only valid for G.711 and G.726
   recording codecs. This function works only when the Coder, Signalling and
   ALI modules have been configured.
*/
IFX_return_t TAPI_LL_Phone_Send_DTMF_OOB (TAPI_CONNECTION * pChannel,
                                         IFX_TAPI_PKT_EV_OOB_t nOobMode)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE *pDev     = (VINETIC_DEVICE *)pChannel->pDevice;
   IFX_uint8_t     ch       = pChannel->nChannel;
   IFX_uint16_t pCmd[3] = { 0 };
   IFX_int32_t     ret      = IFX_SUCCESS;

   /* Read Signaling Module status */
   if (!(pDev->nDevState & DS_SIG_EN))
   {
      SET_ERROR(ERR_SIGMOD_NOTEN);
      return IFX_ERROR;
   }

   if (nOobMode != IFX_TAPI_PKT_EV_OOB_NO && pChannel->nEvtPT == 0)
   {
      SET_ERROR(ERR_WRONG_EVPT);
      return IFX_ERROR;
   }

   IFXOS_MutexLock (pDev->memberAcc);
   /* store the setting for later use */
   pDev->pSigCh[ch].et_stat.flag.dtmf_rec = IFX_TRUE;
   pCmd [2] = pDev->pSigCh[ch].dtmf_rec.value;
   pDev->pSigCh[ch].dtmf_rec.bit.as = IFX_FALSE;
   switch (nOobMode)
   {
      case  IFX_TAPI_PKT_EV_OOB_ALL:
         /* enable transmission */
         break;
      case  IFX_TAPI_PKT_EV_OOB_ONLY:
         /* enable transmission and suppress in band signals */
         pDev->pSigCh[ch].dtmf_rec.bit.as = IFX_TRUE;
         break;
      case  IFX_TAPI_PKT_EV_OOB_NO:
         /* disable transmission and enable in band signals */
         /* store the setting for later use */
         pDev->pSigCh[ch].et_stat.flag.dtmf_rec = IFX_FALSE;
         break;
      default:
      SET_ERROR (ERR_FUNC_PARM);
      return IFX_ERROR;
   }
   if (pDev->pCodCh[ch].cod_ch.bit.en)
      /* if coder running we apply it directely otherwise
         it is just stored */
   {
      pDev->pSigCh[ch].dtmf_rec.bit.et =
               pDev->pSigCh[ch].et_stat.flag.dtmf_rec;
   }
   if (pCmd[2] != pDev->pSigCh[ch].dtmf_rec.value)
   {
      /* write only when AS or ET has changed */
      pCmd [0] = CMD1_EOP | ch;
      pCmd [1] = ECMD_DTMF_REC;
      pCmd [2] = pDev->pSigCh[ch].dtmf_rec.value;
      ret = CmdWrite (pDev, pCmd, 1);
   }

   IFXOS_MutexUnlock (pDev->memberAcc);

   return ret;
#else
   return IFX_ERROR;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
}

/**
   sets the Voice Activation Detection mode.
\param pChannel        Handle to TAPI_CONNECTION structure
\param nVAD            IFX_TRUE: VAD switched on
                     IFX_FALSE: VAD is off
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   This function sets the SC bit (Silence Compression) of Coder Channel's
   cofiguration if VAD is on. Otherwise, it sets it to zero. Recording Codec
   G.729E (11.8kbps) does not support VAD.
   In Case of VAD with comfort noise generation, CNG bit will be set only if
   Codec is G711.
   SIC maybe modified in G.711, G.723 and ILBC on fly.
   The SIC bit is don't care for G728, G729E.
   For G.729 A/B coder must be stopped and started (ENC =0), which resets
   the statistics -> recommend to user to do it before coder start.
*/
IFX_return_t TAPI_LL_Phone_VAD  (TAPI_CONNECTION *pChannel, IFX_int32_t nVAD)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE  *pDev   = (VINETIC_DEVICE *)pChannel->pDevice;
   FWM_COD_CH      *pCodCh = &pDev->pCodCh[pChannel->nChannel].cod_ch;
   IFX_int32_t      ret    = IFX_SUCCESS;
   IFX_uint32_t sc_bit, cng_bit, ns_bit;

   /* protect fw messages */
   IFXOS_MutexLock (pDev->memberAcc);
   /* do appropriate VAD setting */

   /* store the current register bits into local variables.
      Only these bit are getting modified in this function */
   ns_bit = pCodCh->bit.ns;
   cng_bit = pCodCh->bit.cng;
   sc_bit = pCodCh->bit.sc;
   /* set Coder Channel write command */

   ns_bit = 0;
   switch (nVAD)
   {
   case  IFX_TAPI_ENC_VAD_NOVAD:
      /* deactive silence compression */
      sc_bit = 0;
      cng_bit = 0;
      break;
   case IFX_TAPI_ENC_VAD_CNG_ONLY:
      /* deactive silence compression */
      sc_bit = 0;
      cng_bit = 1;
      break;
   case IFX_TAPI_ENC_VAD_SC_ONLY:
      /* deactive silence compression */
      sc_bit = 1;
      cng_bit = 0;
      break;
   default:
      /* activate silence compression */
      sc_bit = 1;
      cng_bit = 1;
      if (nVAD != IFX_TAPI_ENC_VAD_G711)
         ns_bit = 1;
      break;
   }

   /* check if there is a need to send an update
   firmware message. Only send a message if
   any bit has changed */
   if ((ns_bit != pCodCh->bit.ns) ||
       (cng_bit != pCodCh->bit.cng) ||
       (sc_bit != pCodCh->bit.sc))
   {
      /* it is only allowed to change the SC bit
         if the encoder is running G.729A or G.729B */
      if ((pCodCh->bit.enc == COD_CH_G729_ENC) &&
         (sc_bit != pCodCh->bit.sc))
      {
         /* firmware bits have changed and we want to
            write a firmware message. This is not allowed
            when the encoder is currently running. */
         SET_ERROR (ERR_COD_RUNNING);
         ret = IFX_ERROR;
      }

      if (ret == IFX_SUCCESS)
      {
         /* write the modified bits to the firmware message
         and send it to the command mailbox */
         pCodCh->bit.ns = ns_bit;
         pCodCh->bit.cng = cng_bit;
         pCodCh->bit.sc = sc_bit;

         /*  set the requested VAD mode*/
         ret = CmdWrite (pDev, pCodCh->value, 2);
      }
   }

   /* release locks */
   IFXOS_MutexUnlock (pDev->memberAcc);

   return ret;
#else
   return IFX_ERROR;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
}

/**
   sets the LEC configuration on the PCM mode.
\param pChannel        Handle to TAPI_CONNECTION structure
\param pLecConf        Handle to IFX_TAPI_LEC_CFG_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   This function shouldn't be called when a disabling of ALM Nelec can have a
   fatal impact on the ongoing voice/fax connection. It will be more suitable to
   call the function at initialization / configuration time.
*/
IFX_return_t TAPI_LL_Phone_LecConf (TAPI_CONNECTION *pChannel,
                                   IFX_TAPI_LEC_CFG_t const *pLecConf)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE    *pDev    = (VINETIC_DEVICE *)pChannel->pDevice;
   VINETIC_CHANNEL   *pCh     = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t       ret      = IFX_SUCCESS;
   IFX_boolean_t     bEnLec;
   IFX_boolean_t     bEnNlp;

#if 0
   IFX_boolean_t     bEnWLec;
   if (pLecConf->nType == IFX_TAPI_LEC_TYPE_OFF )
   {
      bEnLec  = IFX_FALSE;
      bEnWLec = IFX_FALSE;
   }
   else if (pLecConf->nType == IFX_TAPI_LEC_TYPE_WLEC )
   {
      bEnLec  = IFX_TRUE;
      bEnWLec = IFX_TRUE;
   }
   else if (pLecConf->nType == IFX_TAPI_LEC_TYPE_NLEC )
   {
      bEnLec  = IFX_TRUE;
      bEnWLec = IFX_FALSE;
   }
   else
   {  /* misconfiguration! */
      SET_ERROR (ERR_FUNC_PARM);
      return IFX_ERROR;
   }
   if (pLecConf->nType != IFX_TAPI_LEC_TYPE_OFF )
   {
      /* plausi-check gain values, disable LEC if gains are IFX_TAPI_LEC_GAIN_OFF */
   bEnLec = (pLecConf->nGainIn != IFX_TAPI_LEC_GAIN_OFF   &&
             pLecConf->nGainOut != IFX_TAPI_LEC_GAIN_OFF) ?
             IFX_TRUE : IFX_FALSE;
      if (bEnLec == IFX_FALSE)
         bEnWLec = IFX_FALSE;
   }
   bEnNlp = (pLecConf->bNlp == IFX_TAPI_LEC_NLP_OFF) ? IFX_FALSE : IFX_TRUE;

#endif /* 0 */
   /* enable LEC if gains are not IFX_TAPI_LEC_GAIN_OFF */
   bEnLec = (pLecConf->nGainIn != IFX_TAPI_LEC_GAIN_OFF   &&
             pLecConf->nGainOut != IFX_TAPI_LEC_GAIN_OFF) ?
             IFX_TRUE : IFX_FALSE;
   bEnNlp = (pLecConf->bNlp == IFX_TAPI_LEC_NLP_OFF) ? IFX_FALSE : IFX_TRUE;
   ret = Dsp_SetNeLec_Alm(pCh, bEnLec, bEnNlp, 0);

   return ret;
#else
   return IFX_ERROR;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
}


/**
   sets the LEC configuration on the PCM.
\param pChannel        Handle to TAPI_CONNECTION structure
\param pLecConf        Handle to IFX_TAPI_LEC_CFG_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   This function shouldn't be called when a disabling of PCM Nelec can have a
   fatal impact on the ongoing voice/fax connection. It will be more suitable to
   call the function at initialization / configuration time.
*/
IFX_return_t TAPI_LL_Phone_LecConf_Pcm (TAPI_CONNECTION *pChannel,
                                       IFX_TAPI_LEC_CFG_t const *pLecConf)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE    *pDev    = (VINETIC_DEVICE *)pChannel->pDevice;
   VINETIC_CHANNEL   *pCh     = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t       ret      = IFX_SUCCESS;
   IFX_boolean_t     bEnLec;
   IFX_boolean_t     bEnNlp;

#if 0
   IFX_boolean_t     bEnWLec;
   if (pLecConf->nType == IFX_TAPI_LEC_TYPE_OFF )
   {
      bEnLec  = IFX_FALSE;
      bEnWLec = IFX_FALSE;
   }
   else if (pLecConf->nType == IFX_TAPI_LEC_TYPE_WLEC )
   {
      bEnLec  = IFX_TRUE;
      bEnWLec = IFX_TRUE;
   }
   else if (pLecConf->nType == IFX_TAPI_LEC_TYPE_NLEC )
   {
      bEnLec  = IFX_TRUE;
      bEnWLec = IFX_FALSE;
   }
   else
   {  /* misconfiguration! */
      SET_ERROR (ERR_FUNC_PARM);
      return IFX_ERROR;
   }

   if (pLecConf->nType != IFX_TAPI_LEC_TYPE_OFF )
   {
      /* plausi-check gain values, disable LEC if gains are
         IFX_TAPI_LEC_GAIN_OFF */
      bEnLec = (pLecConf->nGainIn != IFX_TAPI_LEC_GAIN_OFF   &&
                pLecConf->nGainOut != IFX_TAPI_LEC_GAIN_OFF) ?
                IFX_TRUE : IFX_FALSE;
      bEnNlp = (pLecConf->bNlp == IFX_TAPI_LEC_NLP_OFF) ? IFX_FALSE : IFX_TRUE;
      if (bEnLec == IFX_FALSE)
         bEnWLec = IFX_FALSE;
   }
   bEnNlp = (pLecConf->bNlp == IFX_TAPI_LEC_NLP_OFF) ? IFX_FALSE : IFX_TRUE;
#endif /* 0 */
   /* enable LEC if gains are not IFX_TAPI_LEC_GAIN_OFF */
   bEnLec = (pLecConf->nGainIn != IFX_TAPI_LEC_GAIN_OFF   &&
             pLecConf->nGainOut != IFX_TAPI_LEC_GAIN_OFF) ?
             IFX_TRUE : IFX_FALSE;
   bEnNlp = (pLecConf->bNlp == IFX_TAPI_LEC_NLP_OFF) ? IFX_FALSE : IFX_TRUE;

   ret = Dsp_SetNeLec_Pcm(pCh, bEnLec, bEnNlp, 0);

   return ret;
#else
   return IFX_ERROR;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
}

/******************************************************************************/
/**
   TAPI_LL_Phone_AGC_Cfg
      Configure the AGC resource inside the VINETIC. The parameter are given
      as HEX values.
   \param
     - pChannel:  handle to TAPI_CONNECTION structure
     - pParam:    handle to IFX_TAPI_ENC_AGC_CFG_t structure
   \return
      - IFX_ERROR if error
      - IFX_SUCCESS if successful
   \remarks
*******************************************************************************/
IFX_return_t TAPI_LL_Phone_AGC_Cfg (TAPI_CONNECTION *pChannel,
                                    IFX_TAPI_ENC_AGC_CFG_t *pParam)
{
   VINETIC_DEVICE  *pDev        = (VINETIC_DEVICE *)pChannel->pDevice;
   IFX_uint16_t pCmd [2], pData [7];
   IFX_uint8_t    regCOM, regGAIN, regATT, regLIM;

   pCmd [0] = CMD1_EOP | pChannel->nChannel;
   pCmd [1] = ECMD_AGC_COEF;

   if (CmdRead (pDev, pCmd, pData, 5) != IFX_SUCCESS)
      return IFX_ERROR;

#if 0
{
   int i;
   for (i=0; i<7; i++)
   {
      printk("AGC rd  %d -> 0x%04X\n\r", i, pData[i]);
   }
}
#endif

   /* check if given parameter are in range */
   if ((pParam->com > AGC_CONFIG_COM_MAX)    ||
       (pParam->com < AGC_CONFIG_COM_MIN)    ||
       (pParam->gain > AGC_CONFIG_GAIN_MAX)  ||
       (pParam->gain < AGC_CONFIG_GAIN_MIN)  ||
       (pParam->att > AGC_CONFIG_ATT_MAX)    ||
       (pParam->att < AGC_CONFIG_ATT_MIN)    ||
       (pParam->lim > AGC_CONFIG_LIM_MAX)    ||
       (pParam->lim < AGC_CONFIG_LIM_MIN))
   {
      return IFX_ERROR;
   }

   /* get the register values out of the 'dB' values */
   regCOM   = AgcConfig_Reg_COM[pParam->com + AGC_CONFIG_COM_OFFSET];
   regGAIN  = AgcConfig_Reg_GAIN[pParam->gain + AGC_CONFIG_GAIN_OFFSET];
   regATT   = AgcConfig_Reg_ATT[pParam->att + AGC_CONFIG_ATT_OFFSET];
   regLIM   = AgcConfig_Reg_LIM[pParam->lim + AGC_CONFIG_LIM_OFFSET];

   /* update with the given parameter and write the message back to the
      device */

   /* set the 'COM' value in the message and keep the 'GAIN-INT' value */
   pData [2] = (regCOM << 8) | (pData [0] & 0xFF);

   /* set the 'GAIN' value and the 'ATT' value in the message */
   pData [4] = (regGAIN << 8) | regATT;

   /* set the 'LIM' value in the message and keep the 'DEC' value */
   pData [5] = (pData [3] & 0xFF00) | regLIM;

#if 0
{
   int i;
   for (i=0; i<7; i++)
   {
      printk("AGC CFG %d -> 0x%04X\n\r", i, pData[i]);
   }
}
#endif

   if (CmdWrite (pDev, pData, 5) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
}


/**
  Enable/Disable the AGC resoucre inside the VINETIC device.
\param pChannel Handle to TAPI_CONNECTION structure
\param Param IFX_TAPI_ENC_AGC_MODE_t enumeration
\return Return value according to IFX_return_t
      - IFX_ERROR if an error occured
      - IFX_SUCCESS if successful
\remarks
   This implementation assumes that the index of the AGC resource is fixed
   assigned to the related index of the Coder-Module.
   If the AGC resource is enable or disable, it depends on 'Param'.
*/
IFX_return_t TAPI_LL_Phone_AGC_Enable(TAPI_CONNECTION *pChannel,
                                      IFX_TAPI_ENC_AGC_MODE_t Param)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE  *pDev        = (VINETIC_DEVICE *)pChannel->pDevice;
   /*VINETIC_CHANNEL *pCh         = &pDev->pChannel[pChannel->nChannel];*/
   IFX_uint16_t pData [3];

   pData [0] = CMD1_EOP | pChannel->nChannel;
   pData [1] = ECMD_COD_AGC;

   pData [2] = (Param ? 0x8000: 0) | pChannel->nChannel;

   if (CmdWrite (pDev, pData, 1) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
#else
   return IFX_ERROR;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
}


/**
   Set the phone volume
\param pChannel Handle to TAPI_CONNECTION structure
\param pVol     Handle to IFX_TAPI_LINE_VOLUME_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   Gain Parameter are given in 'dB'. The range is -24dB ... 24dB.
*/
IFX_return_t TAPI_LL_Phone_Volume (TAPI_CONNECTION *pChannel,
                                  IFX_TAPI_LINE_VOLUME_t const *pVol)
{
   IFX_int32_t       ret;
   IFX_uint8_t       ch = pChannel->nChannel;
   IFX_boolean_t     bALM_mute = IFX_FALSE;
   VINETIC_DEVICE    *pDev = (VINETIC_DEVICE *)pChannel->pDevice;

   /* range check, cos gain var is integer */
   if ((pVol->nGainTx < (IFX_TAPI_LINE_VOLUME_LOW)) ||
       (pVol->nGainTx > IFX_TAPI_LINE_VOLUME_HIGH)  ||
       (pVol->nGainRx < (IFX_TAPI_LINE_VOLUME_LOW)) ||
       (pVol->nGainRx > IFX_TAPI_LINE_VOLUME_HIGH))
   {
      /* parameter are out of supported range */

      if ((pVol->nGainRx == IFX_TAPI_LINE_VOLUME_OFF) &&
          (pVol->nGainTx == IFX_TAPI_LINE_VOLUME_OFF))
      {
         /* special case: mute the interface with both parameter set to
               IFX_TAPI_LINE_VOLUME_OFF */
         bALM_mute = IFX_TRUE;
      }
      else
      {
         LOG (TAPI_DRV,DBG_LEVEL_HIGH,
              ("\n\rDRV_ERROR: Volume Gain out of range!\n\r"));
         return IFX_ERROR;
      }
   }

   /* protect fw msg */
   IFXOS_MutexLock (pDev->memberAcc);

   if (bALM_mute == IFX_FALSE)
   {
      /* get actual settings into local var */
      pDev->pAlmCh[ch].ali_ch.bit.gain_x =
         (IFX_uint32_t)VINETIC_AlmPcmGain [pVol->nGainTx + 24];
      pDev->pAlmCh[ch].ali_ch.bit.gain_r =
         (IFX_uint32_t)VINETIC_AlmPcmGain [pVol->nGainRx + 24];
      pDev->pAlmCh[ch].ali_ch.bit.en = 1;
   }
   else
   {
      /* mute the interface */
      pDev->pAlmCh[ch].ali_ch.bit.en = 0;
   }

   /* write local configuration if */
   ret = CmdWrite (pDev, pDev->pAlmCh[ch].ali_ch.value, CMD_ALM_CH_LEN);
   /* release lock */
   IFXOS_MutexUnlock (pDev->memberAcc);

   /* SetTraceLevel (VINETIC, DBG_LEVEL_HIGH); */

   return ret;
}

/**
   Sets the PCM interface volume.
\param pChannel Handle to TAPI_CONNECTION structure
\param pVol     Handle to IFX_TAPI_LINE_VOLUME_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   Gain Parameter are given in 'dB'. The range is -24dB ... 24dB.
*/
IFX_return_t TAPI_LL_Phone_Pcm_Volume (TAPI_CONNECTION *pChannel,
                                      IFX_TAPI_LINE_VOLUME_t const *pVol)
{
   IFX_int32_t       ret;
   IFX_boolean_t     bPCM_mute = IFX_FALSE;
   IFX_uint8_t       ch = pChannel->nChannel;
   VINETIC_DEVICE    *pDev = (VINETIC_DEVICE *)pChannel->pDevice;

   /* range check, cos gain var is integer */
   if ((pVol->nGainTx < (IFX_TAPI_LINE_VOLUME_LOW)) ||
       (pVol->nGainTx > IFX_TAPI_LINE_VOLUME_HIGH)  ||
       (pVol->nGainRx < (IFX_TAPI_LINE_VOLUME_LOW)) ||
       (pVol->nGainRx > IFX_TAPI_LINE_VOLUME_HIGH))
   {
      /* parameter are out of supported range */

      if ((pVol->nGainRx == IFX_TAPI_LINE_VOLUME_OFF) &&
          (pVol->nGainTx == IFX_TAPI_LINE_VOLUME_OFF))
      {
         /* special case: mute the interface with both parameter set to
               IFX_TAPI_LINE_VOLUME_OFF */
         bPCM_mute = IFX_TRUE;
      }
      else
      {
         LOG (TAPI_DRV,DBG_LEVEL_HIGH,
              ("\n\rDRV_ERROR: Volume Gain out of range for PCM!\n\r"));
         return IFX_ERROR;
      }
   }

   /* protect fw msg */
   IFXOS_MutexLock (pDev->memberAcc);

   if (bPCM_mute == IFX_FALSE)
   {
      /* get actual settings into local var */
      pDev->pPcmCh[ch].pcm_ch.bit.gain_1 =
         (IFX_uint32_t)VINETIC_AlmPcmGain [pVol->nGainTx + 24];
      pDev->pPcmCh[ch].pcm_ch.bit.gain_2 =
         (IFX_uint32_t)VINETIC_AlmPcmGain [pVol->nGainRx + 24];
      pDev->pPcmCh[ch].pcm_ch.bit.en = 1;
   }
   else
   {
      /* mute the interface */
      pDev->pPcmCh[ch].pcm_ch.bit.en = 0;
   }

   /* write local configuration if */
   ret = CmdWrite (pDev, pDev->pPcmCh[ch].pcm_ch.value, CMD_PCM_CH_LEN);
   /* release lock */
   IFXOS_MutexUnlock (pDev->memberAcc);

   /* SetTraceLevel (VINETIC, DBG_LEVEL_HIGH); */

   return ret;
}

/**
   Enables signal detection
\param pChannel Handle to TAPI_CONNECTION structure
\param pSig     Handle to IFX_TAPI_SIG_DETECTION_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   none
*/
IFX_return_t TAPI_LL_Enable_Signal (TAPI_CONNECTION *pChannel,
                                   IFX_TAPI_SIG_DETECTION_t const *pSig)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE  *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   VINETIC_CHANNEL *pCh  = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t      err  = IFX_SUCCESS;

   if (pDev->nChipType == VINETIC_TYPE_S)
   {
      SET_ERROR (ERR_NOTSUPPORTED);
      return IFX_ERROR;
   }

   IFXOS_MutexLock   (pDev->memberAcc);
   err = Dsp_SigActStatus (pDev, pChannel->nChannel, IFX_TRUE, CmdWrite);

   if ( pDev->nMftdCnt == 0 )
   {
      /* This section handles firmware which provides ATD/UTD */

      /* CED detection */
      if (pSig->sig &
          (IFX_TAPI_SIG_CEDMASK | IFX_TAPI_SIG_CEDENDMASK |
           IFX_TAPI_SIG_PHASEREVMASK | IFX_TAPI_SIG_AMMASK |
           IFX_TAPI_SIG_TONEHOLDING_ENDMASK | IFX_TAPI_SIG_DISMASK)
         )
      {
         /*  ATD1 configured for detection of CED */
         err = Dsp_AtdSigEnable (pCh, pSig->sig);
      }
      if ((err == IFX_SUCCESS) &&
          (pSig->sig & (IFX_TAPI_SIG_CNGFAXMASK | IFX_TAPI_SIG_CNGMODMASK)))
      {
         /*  UTD1 configured for CNG (1100 Hz) detection */
         /*  UTD2 configured for CNG (1300 Hz) detection  */
         err = Dsp_UtdSigEnable (pCh, pSig->sig);
      }
   }
   else
   {
      /* This section handles firmware which provides MFTD */

      err = Dsp_MFTD_SigEnable (pCh, pSig->sig, pSig->sig_ext);
   }

   if ((err == IFX_SUCCESS) && (pSig->sig & IFX_TAPI_SIG_CIDENDTX))
   {
      /* enable the Caller-ID transmission event */
      pChannel->TapiMiscData.sigMask |= IFX_TAPI_SIG_CIDENDTX;
   }
   if ((err == IFX_SUCCESS) &&
       (pSig->sig & (IFX_TAPI_SIG_DTMFTX | IFX_TAPI_SIG_DTMFRX)))
   {
      if (pSig->sig & IFX_TAPI_SIG_DTMFTX)
      {
         err = Dsp_SetDtmfRec (pCh, IFX_TRUE, 1);
         if (err == IFX_SUCCESS)
            /* enable the DTMF receiver */
            pChannel->TapiMiscData.sigMask |= IFX_TAPI_SIG_DTMFTX;
      }
      else
      {
         err = Dsp_SetDtmfRec (pCh, IFX_TRUE, 2);
         if (err == IFX_SUCCESS)
            /* enable the DTMF receiver */
            pChannel->TapiMiscData.sigMask |= IFX_TAPI_SIG_DTMFRX;
      }
   }

   IFXOS_MutexUnlock   (pDev->memberAcc);
   return err;
#else
   return IFX_ERROR;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
}

/**
   Enables signal detection
\param pChannel Handle to TAPI_CONNECTION structure
\param pSig     Handle to IFX_TAPI_SIG_DETECTION_t structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   none
*/
IFX_return_t TAPI_LL_Disable_Signal (TAPI_CONNECTION *pChannel,
                                    IFX_TAPI_SIG_DETECTION_t const *pSig)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE  *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   VINETIC_CHANNEL *pCh  = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t      err  = IFX_SUCCESS;

   IFXOS_MutexLock   (pDev->memberAcc);

   if ( pDev->nMftdCnt == 0 )
   {
      /* This section handles firmware which provides ATD/UTD */

      pChannel->TapiMiscData.sigMask &= ~(pSig->sig);

      if (pSig->sig &
          (IFX_TAPI_SIG_CEDMASK | IFX_TAPI_SIG_CEDENDMASK |
           IFX_TAPI_SIG_PHASEREVMASK | IFX_TAPI_SIG_AMMASK |
           IFX_TAPI_SIG_TONEHOLDING_ENDMASK | IFX_TAPI_SIG_DISMASK)
         )
      {
         err = Dsp_AtdConf (pCh, pChannel->TapiMiscData.sigMask);
      }

      if (pSig->sig & (IFX_TAPI_SIG_CNGFAXMASK | IFX_TAPI_SIG_CNGMODMASK))
      {
         /*  UTD1 configured for CNG (1100 Hz) detection */
         /*  UTD2 configured for CNG (1300 Hz) detection  */
         err = Dsp_UtdConf (pCh, pChannel->TapiMiscData.sigMask &
                           (IFX_TAPI_SIG_CNGFAXMASK | IFX_TAPI_SIG_CNGMODMASK));
      }
   }
   else
   {
      /* This section handles firmware which provides MFTD */

      err = Dsp_MFTD_SigDisable (pCh, pSig->sig, pSig->sig_ext);
   }
   if ((err == IFX_SUCCESS) && (pSig->sig & (IFX_TAPI_SIG_DTMFTX | IFX_TAPI_SIG_DTMFRX)))
   {
      err = Dsp_SetDtmfRec (pCh, IFX_FALSE, 0);
      if (err == IFX_SUCCESS)
         /* enable the DTMF receiver */
         pChannel->TapiMiscData.sigMask &= ~(IFX_TAPI_SIG_DTMFTX | IFX_TAPI_SIG_DTMFRX);
   }
   if (err == IFX_SUCCESS)
      err = Dsp_SigActStatus (pDev, pChannel->nChannel, IFX_FALSE, CmdWrite);

   IFXOS_MutexUnlock   (pDev->memberAcc);

   return err;
#else
   return IFX_ERROR;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
}

/**
   Enables signal detection
\param pChannel Handle to TAPI_CONNECTION structure
\param pTone    Handle to the simple tone structure
\param signal   the type of signal and direction to detect
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   none
*/
IFX_return_t TAPI_LL_CPTD_Start (TAPI_CONNECTION *pChannel,
                                IFX_TAPI_TONE_SIMPLE_t const *pTone, IFX_int32_t signal)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE  *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   VINETIC_CHANNEL *pCh  = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t      err = IFX_SUCCESS;
   IFX_uint8_t      ch = pCh->nChannel - 1;
   IFX_uint16_t     pCmd[CMD_SIG_CPTCOEFF_LEN + CMD_HEADER_CNT];
   DSP_CPTD_FL_t    frameLength;

   if (pDev->nChipType == VINETIC_TYPE_S)
   {
      SET_ERROR (ERR_NOTSUPPORTED);
      return IFX_ERROR;
   }
   err = Dsp_SigActStatus (pDev, pChannel->nChannel, IFX_TRUE, CmdWrite);

   /* set CPTD coefficients ************************************************* */
   if (err == IFX_SUCCESS)
   {
      memset (pCmd, 0, (CMD_SIG_CPTCOEFF_LEN  + CMD_HEADER_CNT) * 2);
      pCmd[0] = CMD1_EOP | ch;
      pCmd[1] = ECMD_CPT_COEF;
      /* nFrameSize is depending on the tone cadence and will be
         decided inside Dsp_Cptd_SetCoeff */
      Dsp_Cptd_SetCoeff (pCh, pTone, pCmd, &frameLength);
      /* write CPTD coefficients */
      err = CmdWrite (pDev, pCmd, CMD_SIG_CPTCOEFF_LEN);
   }

   /* activate CPTD ********************************************************* */
   pCmd[1] = ECMD_CPT;
   if (err == IFX_SUCCESS)
   {
      /* on activation of the CPTD also nFrameSize is configured as
         determined above... */
      err = Dsp_Cptd_Conf (pCh, pTone, signal, pCmd, frameLength);
   }
   if (err == IFX_SUCCESS)
   {
      err = CmdWrite (pDev, pCmd, CMD_SIG_CPT_LEN);
      /* write interrupt masks */
      if (err == IFX_SUCCESS)
      {
         /* enable cpt 0 -> 1 interrupt */
         err = VINETIC_Host_Set_EdspIntMask (pCh, VIN_EDSP_CPT, IFX_FALSE,
                                             VIN_CPT_MASK, 0);
      }
   }

   return err;
#else
   return IFX_ERROR;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
}

/**
   Disables signal detection
\param pChannel Handle to TAPI_CONNECTION structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   none
*/
IFX_return_t TAPI_LL_CPTD_Stop (TAPI_CONNECTION *pChannel)
{
   VINETIC_DEVICE  *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   VINETIC_CHANNEL *pCh  = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t      err  = IFX_SUCCESS;
   IFX_uint8_t      ch = pCh->nChannel - 1;
   IFX_uint16_t     pCmd[3];

   /* disable cpt 0 -> 1 interrupt which was enabled during _CTTD_Start.
      falling edge interrupts are dont care. */
   err = VINETIC_Host_Set_EdspIntMask (pCh, VIN_EDSP_CPT, IFX_FALSE, 0, 0);
   /* write CPTD configuration */
   pCmd[0] = CMD1_EOP | ch;
   pCmd[1] = ECMD_CPT;
   pCmd[2] = 0;
   /* set resource */
   VIN_ECMD_SIGCPT_CPTNR_SET (pCmd[2], ch);
   if (err == IFX_SUCCESS)
      err = CmdWrite (pDev, pCmd, CMD_SIG_CPT_LEN);
   if (err == IFX_SUCCESS)
      err = Dsp_SigActStatus (pDev, pChannel->nChannel, IFX_FALSE, CmdWrite);

   return err;
}

#ifdef TAPI_LL_DRV
/* device driver handling functions */
IFX_return_t VIN_TAPI_LL_ChannelOpen(TAPI_CHANNEL_t *pChannel, void* pCtx, IFX_uint16_t nCh);
IFX_return_t VIN_TAPI_LL_ChannelClose(TAPI_CHANNEL_t *pChannel, void* pCtx);
/* TAPI low level functions */
IFX_return_t VIN_TAPI_LL_ChannelInit(TAPI_CHANNEL_t *pChannel, IFX_TAPI_CH_INIT_t const *pInit);

/**
   Low level driver registration
\param pCtx TAPI driver context for low level function pointers.
       Preinitialized to zero
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   Registers low level functions for the TAPI driver.
   Unused functions can be left blank.
   This function is called after the driver has been loaded.
*/
IFX_return_t TAPI_LL_DrvInit (TAPI_LL_CTX_t* pCtx)
{
   TRACE (VINETIC,DBG_LEVEL_HIGH, ("%s\n\r", __FUNCTION__));

   pCtx->TAPI_LL_chInit = VIN_TAPI_LL_ChannelInit;
   pCtx->TAPI_LL_chOpen = VIN_TAPI_LL_ChannelOpen;
   pCtx->TAPI_LL_chClose = VIN_TAPI_LL_ChannelClose;

   TAPI_DrvInit (pCtx);
   return IFX_SUCCESS;
}

/* just for test */
IFX_return_t VIN_TAPI_LL_ChannelInit(TAPI_CHANNEL_t *pChannel, IFX_TAPI_CH_INIT_t const *pInit)
{
   TRACE (VINETIC,DBG_LEVEL_HIGH, ("%s\n\r", __FUNCTION__));
   return IFX_SUCCESS;
}

/**
   Called when a TAPI channel is opened
\param pChannel Pointer to a TAPI channel context structure
\param pCtx TAPI driver context for low level function pointers
\param nCh absolut TAPI channel number
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
*/
IFX_return_t VIN_TAPI_LL_ChannelOpen(TAPI_CHANNEL_t *pChannel, void* pCtx, IFX_uint16_t nCh)
{
   VINETIC_DEVICE* pDev = (VINETIC_DEVICE*)pCtx;

   TRACE (VINETIC,DBG_LEVEL_HIGH, ("%s\n\r", __FUNCTION__));
   /** \todo nCh must be changed to on a per device basis */
   pDev->pChannel[nCh].pTESTTapiCh = pChannel;
   return IFX_SUCCESS;
}

/**
   Called when a TAPI channel is closed
\param pChannel Pointer to a TAPI channel context structure
\param pCtx TAPI driver context for low level function pointers
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
*/
IFX_return_t VIN_TAPI_LL_ChannelClose(TAPI_CHANNEL_t *pChannel, void* pCtx)
{
   VINETIC_DEVICE* pDev = (VINETIC_DEVICE*)pCtx;

   TRACE (VINETIC,DBG_LEVEL_HIGH, ("%s\n\r", __FUNCTION__));
   pDev->pChannel[pChannel->nCh].pTESTTapiCh = IFX_NULL;
   return IFX_SUCCESS;
}
#endif /* TAPI_LL_DRV */

#endif /* TAPI */

