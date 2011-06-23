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
   Module      : drv_vinetic_tapi.c
   Date        : 2003-10-28
   Description : This file contains the implementations of vinetic related
                 tapi low level functions.
*******************************************************************************/

/* ============================= */
/* includes                      */
/* ============================= */

#include "drv_vinetic_api.h"
#include "drv_vinetic_cid.h"
#include "drv_vinetic_cram.h"
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

/* maximum number of capabilities, used for static array */
#define MAX_CAPS              24

/* Frame lengths in ms */
#define FL_5MS                5
#define FL_5_5MS              6
#define FL_10MS               10
#define FL_20MS               20
#define FL_30MS               30
#define FL_11MS               11
#ifdef TAPI_CID
/* DTMF max digt/inter digit time */
#define MAX_DIGIT_TIME        127       /* ms */
#define MAX_INTERDIGIT_TIME   127       /* ms */
#endif /* TAPI_CID */
/* define for encoder packtet time not supported */
#define NO_S                  1000

#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
#define MAX_SGLEN             10000     /* ms */
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38) */

/* ============================= */
/*             Enums             */
/* ============================= */

/* ============================= */
/* Local function declaration    */
/* ============================= */
IFX_LOCAL IFX_int32_t TAPI_LL_Phone_Tone_Off (TAPI_CONNECTION * pChannel);
IFX_LOCAL IFX_int32_t TAPI_LL_Phone_Tone_On (TAPI_CONNECTION * pChannel,
                                             IFX_int32_t nToneIndex);

IFX_LOCAL IFX_int32_t AddCaps (VINETIC_DEVICE * pDev);
IFX_LOCAL IFX_void_t AddCapability (TAPI_PHONE_CAPABILITY * CapList,
                                    IFX_uint32_t * pnCap,
                                    IFX_char_t const *description,
                                    IFX_int32_t type, IFX_int32_t value);

/* Configuration functions for Tapi Init */
/* Voice Coder Configuration */
IFX_LOCAL IFX_int32_t initVineticTapi (VINETIC_DEVICE * pDev,
                                       IFX_uint8_t nCountry,
                                       IFX_void_t const *pInit);
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
IFX_LOCAL IFX_int32_t CheckENCnPTE (IFX_uint8_t nFrameLength,
                                    IFX_uint8_t nCodec);
IFX_LOCAL IFX_char_t getEncoder (IFX_int32_t nCodec);
IFX_LOCAL IFX_int32_t getFrameLength (IFX_int32_t nPTE);
IFX_LOCAL IFX_int32_t getConvPTE (IFX_int32_t nPrePTE, IFX_int32_t nCodec);
IFX_LOCAL IFX_int32_t getPTE (IFX_int32_t nFrameLength);
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */

#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
IFX_LOCAL IFX_int32_t FaxSetupConf (VINETIC_CHANNEL * pCh);
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38) */
#ifdef TAPI_CID
IFX_LOCAL IFX_int32_t SetCidRx (VINETIC_CHANNEL const *pCh, IFX_boolean_t bEn);
#endif /* TAPI_CID */

/* ============================= */
/* Local variable definition     */
/* ============================= */
/* frequencies for TG 1 & TG 2 */
IFX_LOCAL const IFX_uint32_t VINETIC_ToneFreq[31][2] = {
   /* Freq1/Freq2(mHz) * Freq1/Freq2(Hz) - Index - DTMF Digit */
   {697000, 1209000},           /* 697/1209 Hz, 1, 1 */
   {697000, 1336000},           /* 697/1336 Hz, 2, 2 */
   {697000, 1477000},           /* 697/1477 Hz, 3, 3 */
   {770000, 1209000},           /* 770/1209 Hz, 4, 4 */
   {770000, 1336000},           /* 770/1336 Hz, 5, 5 */
   {770000, 1477000},           /* 770/1477 Hz, 6, 6 */
   {852000, 1209000},           /* 852/1209 Hz, 7, 7 */
   {852000, 1336000},           /* 852/1336 Hz, 8, 8 */
   {852000, 1477000},           /* 852/1477 Hz, 9, 9 */
   {941000, 1209000},           /* 941/1209 Hz, 10, * */
   {941000, 1336000},           /* 941/1336 Hz, 11, 0 */
   {941000, 1477000},           /* 941/1477 Hz, 12, # */
   {800000, 0},                 /* 800/0 Hz, 13, - */
   {1000000, 0},                /* 1000/0 Hz, 14, - */
   {1250000, 0},                /* 1250/0 Hz, 15, - */
   {950000, 0},                 /* 950/0 Hz, 16, - */
   {1100000, 0},                /* 1100/0 Hz, 17, - *//* --> CNG Tone */
   {1400000, 0},                /* 1400/0 Hz, 18, - */
   {1500000, 0},                /* 1500/0 Hz, 19, - */
   {1600000, 0},                /* 1600/0 Hz, 20, - */
   {1800000, 0},                /* 1800/0 Hz, 21, - */
   {2100000, 0},                /* 2100/0 Hz, 22, - *//* --> CED Tone */
   {2300000, 0},                /* 2300/0 Hz, 23, - */
   {2450000, 0},                /* 2450/0 Hz, 24, - */
   {350000, 440000},            /* 350/440 Hz, 25, - *//* --> Dial Tone */
   {440000, 480000},            /* 440/480 Hz, 26, - *//* --> Ring Back */
   {480000, 620000},            /* 480/620 Hz, 27, - *//* --> Busy Tone */
   {697000, 1633000},           /* 697/1633 Hz, 28, A */
   {770000, 1633000},           /* 770/1633 Hz, 29, B */
   {852000, 1633000},           /* 852/1633 Hz, 30, C */
   {941000, 1633000},           /* 941/1633 Hz, 31, D */
};

/* encoder packet time table : NO_S means not supported */
IFX_LOCAL const IFX_int32_t VINETIC_PteVal[MAX_CODECS][6] = {
   {0, 1, 2, 3, 4, 5},          /* PTE values */
   {NO_S, NO_S, NO_S, 30000, NO_S, NO_S},       /* G723_63, us - index 1 */
   {NO_S, NO_S, NO_S, 30000, NO_S, NO_S},       /* G723_53, us - index 2 */
   {NO_S, NO_S, NO_S, NO_S, NO_S, NO_S},
   {NO_S, NO_S, NO_S, NO_S, NO_S, NO_S},
   {NO_S, NO_S, NO_S, NO_S, NO_S, NO_S},
   {5000, 10000, 20000, NO_S, NO_S, NO_S},      /* G728 , us - index 6 */
   {NO_S, 10000, 20000, NO_S, NO_S, NO_S},      /* G729 , us - index 7 */
   {5000, 10000, 20000, NO_S, 5500, 11000},     /* MLAW , us - index 8 */
   {5000, 10000, 20000, NO_S, 5500, 11000},     /* ALAW , us - index 9 */
   {NO_S, NO_S, NO_S, NO_S, NO_S, NO_S},
   {NO_S, NO_S, NO_S, NO_S, NO_S, NO_S},
   {5000, 10000, 20000, NO_S, 5500, 11000},     /* G726_16, us - index 12 */
   {5000, 10000, 20000, NO_S, NO_S, NO_S},      /* G726_24, us - index 13 */
   {5000, 10000, 20000, NO_S, 5500, 11000},     /* G726_32, us - index 14 */
   {5000, 10000, 20000, NO_S, NO_S, NO_S},      /* G726_40, us - index 15 */
   {NO_S, 10000, 20000, NO_S, NO_S, NO_S},      /* G729_E , us - index 16 */
};

/* calculated table to set the Rx/Tx Gain in ALM/PCM module.
   Converts the gain in 'dB' into the regiter values */
IFX_LOCAL const IFX_uint8_t VINETIC_AlmPcmGain[49] = {
   /* Gain Byte Calculation Table in 'dB' See also VINETIC User's Manual Table
      15 Rev 1.0 */
   /* -24 -23 -22 -21 -20 -19 -18 -17 -16 -15 */
   224, 228, 233, 238, 243, 249, 192, 196, 201, 206,
   /* -14 -13 -12 -11 -10 -9 -8 -7 -6 -5 */
   211, 217, 160, 164, 168, 173, 179, 185, 128, 132,
   /* -4 -3 -2 -1 0 1 2 3 4 5 */
   136, 141, 147, 153, 96, 100, 104, 109, 115, 121,
   /* 6 7 8 9 10 11 12 13 14 15 */
   64, 68, 72, 77, 83, 89, 32, 36, 40, 45,
   /* 16 17 18 19 20 21 22 23 24 */
   50, 57, 0, 4, 8, 13, 18, 25, 31
};

/* ============================= */
/* Local functions definitions   */
/* ============================= */

/*******************************************************************************
Description:
  Add capability to given list
Arguments:
   pnCap       -
   description -
   type        -
   value       -
Return:
   none
*******************************************************************************/
/* add a capability to the existing ones and increase the counter nCap */
IFX_LOCAL IFX_void_t AddCapability (TAPI_PHONE_CAPABILITY * CapList,
                                    IFX_uint32_t * pnCap,
                                    IFX_char_t const *description,
                                    IFX_int32_t type, IFX_int32_t value)
{
   IFX_uint32_t capnr;

   if (pnCap == NULL)
      return;

   if (*pnCap >= MAX_CAPS)
      return;

   capnr = (*pnCap);
   strncpy (CapList[capnr].desc, description, sizeof (CapList[0].desc));
   CapList[capnr].captype = (TAPI_PHONE_CAP) type;
   CapList[capnr].cap = value;
   CapList[capnr].handle = (int) capnr;
   (*pnCap) = capnr + 1;
   TRACE (VINETIC, DBG_LEVEL_NORMAL,
          ("Cap: %s , type %ld = %ld\n\r", description, type, value));
}

/*******************************************************************************
Description:
  set all vinetic capabilities
Arguments:
   pDev  - ptr to actual Vinetic Device
Return:
   IFX_SUCCESS or IFX_ERROR if no memory is available
*******************************************************************************/
IFX_LOCAL IFX_int32_t AddCaps (VINETIC_DEVICE * pDev)
{
   /* capability list */
   TAPI_PHONE_CAPABILITY *CapList;

   /* start adding the capability with coders */
   IFX_uint32_t nCap = 0;

   /* holds the maximum amount of capabilities */
   if (pDev->CapList != NULL)
   {
      IFXOS_FREE (pDev->CapList);
   }
   pDev->CapList =
      (TAPI_PHONE_CAPABILITY *) IFXOS_MALLOC (MAX_CAPS *
                                              sizeof (TAPI_PHONE_CAPABILITY));
   if (pDev->CapList == NULL)
   {
      SET_ERROR (ERR_NO_MEM);
      return IFX_ERROR;
   }
   CapList = pDev->CapList;

   AddCapability (CapList, &nCap, "INFINEON TECHNOLOGIES", vendor, 0);
   AddCapability (CapList, &nCap, "VINETIC", device, 0);

   AddCapability (CapList, &nCap, "POTS", port, pots);
   AddCapability (CapList, &nCap, "PSTN", port, pstn);

#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
   if ((!((pDev->nEdspVers & ECMD_VERS_FEATURES) == ECMD_VERS_FEATURES_C) ||
        ((pDev->nEdspVers & ECMD_VERS_FEATURES) == ECMD_VERS_FEATURES_S)) &&
       (pDev->nChipType != VINETIC_TYPE_S))
   {
      /* coder features of M and VIP versions */
      AddCapability (CapList, &nCap, "DSP", dsp, 0);

      /* add the defaults */
      AddCapability (CapList, &nCap, "G.726 16 kbps", codec, G726_16);
      AddCapability (CapList, &nCap, "G.726 24 kbps", codec, G726_24);
      AddCapability (CapList, &nCap, "G.726 32 kbps", codec, G726_32);
      AddCapability (CapList, &nCap, "G.726 40 kbps", codec, G726_40);
      AddCapability (CapList, &nCap, "u-LAW", codec, MLAW);
      AddCapability (CapList, &nCap, "A-LAW", codec, ALAW);

      if ((pDev->nEdspVers & ECMD_VERS_MV) == ECMD_VERS_MV_4)
      {
         switch (pDev->nEdspVers & ECMD_VERS_FEATURES)
         {
         case ECMD_VERS_FEATURES_729ABE_G723:
            AddCapability (CapList, &nCap, "G.723 6.3kbps", codec, G723_63);
            AddCapability (CapList, &nCap, "G.723 5.3kbps", codec, G723_53);
            AddCapability (CapList, &nCap, "G.729", codec, G729);
            AddCapability (CapList, &nCap, "G.729E", codec, G729_E);
            break;
         case ECMD_VERS_FEATURES_G729AB_G723:
            AddCapability (CapList, &nCap, "G.723 6.3kbps", codec, G723_63);
            AddCapability (CapList, &nCap, "G.723 5.3kbps", codec, G723_53);
            break;
         case ECMD_VERS_FEATURES_G728_G723:
            AddCapability (CapList, &nCap, "G.728", codec, G728);
            AddCapability (CapList, &nCap, "G.723 6.3kbps", codec, G723_63);
            AddCapability (CapList, &nCap, "G.723 5.3kbps", codec, G723_53);
            break;
         case ECMD_VERS_FEATURES_G728_G729ABE:
            AddCapability (CapList, &nCap, "G.728", codec, G728);
            AddCapability (CapList, &nCap, "G.729", codec, G729);
            AddCapability (CapList, &nCap, "G.729E", codec, G729_E);
            break;
         case ECMD_VERS_FEATURES_729ABE_T38:
            AddCapability (CapList, &nCap, "G.729", codec, G729);
            AddCapability (CapList, &nCap, "G.729E", codec, G729_E);
            AddCapability (CapList, &nCap, "T.38", t38, 1);
            break;
         case ECMD_VERS_FEATURES_G729AB_G728_T38:
            AddCapability (CapList, &nCap, "G.729", codec, G729);
            AddCapability (CapList, &nCap, "G.728", codec, G728);
            AddCapability (CapList, &nCap, "T.38", t38, 1);
            break;
         case ECMD_VERS_FEATURES_G723_G728_T38:
            AddCapability (CapList, &nCap, "G.723 6.3kbps", codec, G723_63);
            AddCapability (CapList, &nCap, "G.723 5.3kbps", codec, G723_53);
            AddCapability (CapList, &nCap, "G.728", codec, G728);
            AddCapability (CapList, &nCap, "T.38", t38, 1);
            break;
         default:
            TRACE (VINETIC, DBG_LEVEL_NORMAL,
                   ("Warning: Unknown feature set\n\r"));
            break;
         }
      }
      /* set coder channels */
      switch (pDev->nEdspVers & ECMD_VERS_MV)
      {
      case ECMD_VERS_MV_8:
         AddCapability (CapList, &nCap, "Coder", codecs, 8);
         break;
      case 0x6000:
         AddCapability (CapList, &nCap, "Coder", codecs, 6);
         break;
      default:
         AddCapability (CapList, &nCap, "Coder", codecs, 4);
      }
   }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
   switch (pDev->nChipType)
   {
   case VINETIC_TYPE_S:
      AddCapability (CapList, &nCap, "PCM", pcm, 4);
      break;
   default:
      AddCapability (CapList, &nCap, "PCM", pcm, 8);
      AddCapability (CapList, &nCap, "CED", sigdetect, ced);
      AddCapability (CapList, &nCap, "CNG", sigdetect, cng);
      AddCapability (CapList, &nCap, "DIS", sigdetect, dis);
      AddCapability (CapList, &nCap, "POWER", sigdetect, power);
      AddCapability (CapList, &nCap, "CPT", sigdetect, cpt);
      break;
   }
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */

   AddCapability (CapList, &nCap, "Phones", phones, pDev->nAnaChan);
   /* check if no array out of bounds */
   IFXOS_ASSERT (nCap <= MAX_CAPS);
   pDev->nMaxCaps = (IFX_uint8_t) nCap;

   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   Configure VINETIC Chip for Voice Coder purposes
Arguments:
   pDev        - pointer to VINETIC device structure
   nCountry    - county of which configuration should be done
   pInit       - ptr to device initialization structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks :
   The parameter nCountry is obsolete for the moment and should later on help to
   select country specific settings in case pInit is NULL.

   if pInit isn't NULL, not other settings should be done. It is assumed that
   the user knows what he programms.
*******************************************************************************/
IFX_LOCAL IFX_int32_t initVineticTapi (VINETIC_DEVICE * pDev,
                                       IFX_uint8_t nCountry,
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
   /* do initialization as specified in init. Country isn't taken in count
      then. */
   else
   {
      IFXOS_CPY_USR2KERN (&IoInit, pInit, sizeof (VINETIC_IO_INIT));
   }
   /* Initialize Vinetic */
   ret = VINETIC_Init (pDev, &IoInit);

   return ret;
}

#ifdef TAPI_CID
/*******************************************************************************
Description:
  Disables or Enables Cid Receiver according to bEn
Arguments:
  pCh  - pointer to VINETIC channel structure
  bEn  - IFX_TRUE : enable / IFX_FALSE : disable cid receiver
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   The signalling channel must be enabled before
*******************************************************************************/
IFX_LOCAL IFX_int32_t SetCidRx (VINETIC_CHANNEL const *pCh, IFX_boolean_t bEn)
{
   IFX_int32_t ret = IFX_SUCCESS;
   IFX_uint16_t pCmd[2] = { 0 }, pData[3] =
   {
   0};
   VINETIC_DEVICE *pDev = pCh->pParent;

   /* EOP Cmd */
   pCmd[0] = (CMD1_EOP | (pCh->nChannel - 1));
   /* CID RX */
   pCmd[1] = ECMD_CIDRX;
   ret = CmdRead (pDev, pCmd, pData, 1);
   if (ret == IFX_SUCCESS)
   {
      if ((bEn == IFX_TRUE) && !(pData[2] & SIG_CIDRX_EN))
      {
         pData[2] |= SIG_CIDRX_EN;
         ret = CmdWrite (pDev, pData, 1);
      }
      else if ((bEn == IFX_FALSE) && (pData[2] & SIG_CIDRX_EN))
      {
         pData[2] &= ~SIG_CIDRX_EN;
         ret = CmdWrite (pDev, pData, 1);
      }
   }

   return ret;
}
#endif /* TAPI_CID */

#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
/*******************************************************************************
Description:
   configures vinetic for Fax T38.
Arguments:
  pCh        - pointer to VINETIC channel structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remark :
   once a valid tone is detected, NELEC coefficients will be programmed and
   NLP will be disabled in ALM NELEC
*******************************************************************************/
IFX_LOCAL IFX_int32_t FaxSetupConf (VINETIC_CHANNEL * pCh)
{
   IFX_int32_t ret = IFX_SUCCESS;

   /* reset status /error flags */
   pCh->pTapiCh->TapiFaxStatus.nStatus = 0;
   pCh->pTapiCh->TapiFaxStatus.nError = 0;
   /* The calling side will be in state ready to issue a call. The called side
      can be in idle state when responding to this call */
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   /* No Voice connection needed when fax connection started */
   if (pCh->bVoiceConnect == IFX_TRUE)
   {
      ret = TAPI_LL_Phone_Stop_Recording (pCh->pTapiCh);
      if (ret == IFX_SUCCESS)
      {
         ret = TAPI_LL_Phone_Stop_Playing (pCh->pTapiCh);
      }
#ifdef QOS_SUPPORT
      /* make sure packet redirection is stopped */
      Qos_Ctrl ((IFX_uint32_t) pCh, FIO_QOS_STOP, QOS_PORT_CLEAN);
#endif /* QOS_SUPPORT */
      /* set back voice connection flag, in case it was changed in one of the
         called functions */
      pCh->bVoiceConnect = IFX_TRUE;
   }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
   /* set near end lec coefficients and disable NLP */
   if (ret == IFX_SUCCESS)
   {
      ret = Dsp_SetNeLec_Alm (pCh, IFX_TRUE, IFX_FALSE);
   }
   /* disable all signalling modules without disabling the signalling channel.
      Note: Avoid disabling the signalling channel because of conferencing */
   if (ret == IFX_SUCCESS)
   {
      ret = Dsp_SwitchSignModules (pCh, IFX_FALSE);
   }

   return ret;
}
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38) */

#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
/*******************************************************************************
Description:
   function called to get Encoder value according to Codec selected
Arguments:
   nCodec    - Codec setected
Return:
   ENC value or IFX_ERROR
Remarks
*******************************************************************************/
IFX_LOCAL IFX_char_t getEncoder (IFX_int32_t nCodec)
{
   IFX_LOCAL IFX_char_t EncoderTable[] = {
      (IFX_char_t) IFX_ERROR,   /* 0 not supported */
      COD_CH_G723_63_ENC,
      COD_CH_G723_53_ENC,
      (IFX_char_t) IFX_ERROR, (IFX_char_t) IFX_ERROR, (IFX_char_t) IFX_ERROR,   /* 3
                                                                                   -
                                                                                   5
                                                                                   not
                                                                                   supported */
      COD_CH_G728_ENC,
      COD_CH_G729_ENC,
      COD_CH_G711_MLAW_ENC,
      COD_CH_G711_ALAW_ENC,
      /* 10: LINEAR16 and 11: LINEAR8 not supported */
      (IFX_char_t) IFX_ERROR, (IFX_char_t) IFX_ERROR,
      COD_CH_G726_16_ENC,
      COD_CH_G726_24_ENC,
      COD_CH_G726_32_ENC,
      COD_CH_G726_40_ENC,
      COD_CH_G729_E_ENC
   };
   return EncoderTable[nCodec];
}

/*******************************************************************************

Description:
   function called to get frame length in ms according to PTE value
Arguments:
   nPTE - PTE value
Return:
    encoder packet time in ms or IFX_ERROR
Remarks
*******************************************************************************/
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

/*******************************************************************************
Description:
   function called to determine the convenient PTE value according to the pre
   existing value in case this one doesn't match with the selected encoder
Arguments:
   nPrePTE         - pre existing PTE
   nCodec          - encoder algorithm value
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks
   The tables of PTE values supported per encoder look as follows :
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
*******************************************************************************/
IFX_LOCAL IFX_int32_t getConvPTE (IFX_int32_t nPrePTE, IFX_int32_t nCodec)
{
   IFX_int32_t *pTemp = NULL;
   IFX_int32_t size =
      sizeof (VINETIC_PteVal[nCodec]) / sizeof (IFX_int32_t), pre_time =
      0, abs_time = 0, i = 0, pos = 0, nConvPte;

   /* look in vertical to determine the actual packet time */
   for (i = 1; i < MAX_CODECS; i++)
   {
      pTemp = (IFX_int32_t *) VINETIC_PteVal[i];
      if (pTemp[nPrePTE] != NO_S)
      {
         pre_time = pTemp[nPrePTE];
         break;
      }
   }
   /* find first value for comparisons */
   for (i = 0; i < size; i++)
   {
      pTemp = (IFX_int32_t *) & VINETIC_PteVal[nCodec][i];
      if (*pTemp != NO_S)
      {
         abs_time = ABS (*pTemp - pre_time);
         pos = i;
         break;
      }
   }
   /* loop horizontally to determine best match */
   for (i = 0; i < size; i++)
   {
      pTemp = (IFX_int32_t *) & VINETIC_PteVal[nCodec][i];
      if (*pTemp != NO_S && (abs_time > ABS (*pTemp - pre_time)))
      {
         abs_time = ABS (*pTemp - pre_time);
         pos = i;
      }
   }
   /* set new pte value */
   nConvPte = VINETIC_PteVal[0][pos];
   /* trace out some inportant info */
   TRACE (TAPI_DRV, DBG_LEVEL_HIGH,
          ("Not matching encoder packet time of " "%ld us is set now.\n\r",
           pre_time));
   TRACE (TAPI_DRV, DBG_LEVEL_HIGH,
          ("This will be replaced by the best "
           "matching encoder packet time of %ld us.\n\r",
           VINETIC_PteVal[nCodec][pos]));

   return nConvPte;
}

/*******************************************************************************
Description:
   function called to get PTE value according to frame length in ms
Arguments:
   nFrameLength    - encoder packet time in ms
Return:
   PTE value or IFX_ERROR
Remarks
*******************************************************************************/
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

/*******************************************************************************
Description:
   function called to check the validity of the co-existence of the requested
   PTE value and encoder algorithm value
Arguments:
   nFrameLength    - encoder packet time (PTE) value
   nCodec          - encoder algorithm value
Return:
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
*******************************************************************************/
IFX_LOCAL IFX_int32_t CheckENCnPTE (IFX_uint8_t nFrameLength,
                                    IFX_uint8_t nCodec)
{
   IFX_int32_t ret = IFX_SUCCESS;

   switch (nFrameLength)
   {
      /* 5ms, not possible for G.729 and G.723.1 */
   case COD_CH_PTE_5MS:
      if ((nCodec == COD_CH_G729_ENC) || (nCodec == COD_CH_G729_E_ENC) ||
          (nCodec == COD_CH_G723_53_ENC) || (nCodec == COD_CH_G723_63_ENC))
      {
         ret = IFX_ERROR;
      }
      break;
      /* 10ms and 20ms, not possible for G.723.1 */
   case COD_CH_PTE_10MS:
   case COD_CH_PTE_20MS:
      if ((nCodec == COD_CH_G723_53_ENC) || (nCodec == COD_CH_G723_63_ENC))
      {
         ret = IFX_ERROR;
      }
      break;
      /* 30ms, allowed for all codecs */
   case COD_CH_PTE_30MS:
      break;
      /* 5.5ms and 11ms, only allowed for G.711, G.726-16kbps, G.726-32kbps */
   case COD_CH_PTE_5_5MS:
   case COD_CH_PTE_11MS:
      if ((nCodec != COD_CH_G711_ALAW_ENC) && (nCodec != COD_CH_G711_MLAW_ENC)
          && (nCodec != COD_CH_G726_16_ENC) && (nCodec != COD_CH_G726_32_ENC))
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

/****************************************************************************
Description :
   WakeUp the TAPI channel task
Arguments   :
   pChannel  - handle to the channel specific TAPI structure
Return      :
   NONE
****************************************************************************/
IFX_void_t TAPI_WakeUp (TAPI_CONNECTION * pChannel)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   /* In polled mode, there are no sleeping tasks (blocked on select), so no
      wakeup needed */
   if ((pDev->bNeedWakeup == IFX_TRUE) && (pDev->IrqPollMode & VIN_TAPI_WAKEUP))
   {
      /* don't wake up any more */
      pDev->bNeedWakeup = IFX_FALSE;
      IFXOS_WakeUp (pDev->EventList, IFXOS_READQ);
   }
}

/*******************************************************************************
Description:
   Configure Tonegenerators and start timer
Arguments:
   pChannel       - handle to TAPI_CONNECTION structure
   nToneIndex     - user selected tone index
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   programs directly coefficient register using a frequency / coefficient table
*******************************************************************************/
IFX_LOCAL IFX_int32_t TAPI_LL_Phone_Tone_On (TAPI_CONNECTION * pChannel,
                                             IFX_int32_t nToneIndex)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   IFX_uint32_t nOnTime /* , c1 = 0, c2 = 0 */ ;
   IFX_uint16_t nDscr;
   IFX_int32_t ret;
   IFX_int32_t freqA, freqB;

   /* get frequency from table, adapt index offset */
   nToneIndex -= 1;
   freqA = (IFX_int32_t) VINETIC_ToneFreq[nToneIndex][VINETIC_TG1_FREQ];
   freqB = (IFX_int32_t) VINETIC_ToneFreq[nToneIndex][VINETIC_TG2_FREQ];
   /* program the coefficients */
   ret = Tone_TG_SetCoeff (pCh, pDev->nChipMajorRev, freqA, freqB);

   /* calculate on-time for timer */
   nOnTime = pChannel->TapiTgToneData.nOnTime / 4;
   /* check for smallest on-time */
   if (nOnTime < 20)
   {
      nOnTime = 20;
   }
   /* Enable tone generators with CRAM coefficients */
   nDscr = (DSCR_COR8) | (DSCR_PTG) | (DSCR_TG1_EN);
   if (freqB)
   {
      /* use the second tone generator */
      pChannel->TapiTgToneData.bUseSecondTG = IFX_TRUE;
      /* also start tone generator 2 */
      nDscr |= DSCR_TG2_EN;
   }
   /* call target function to set tone generator(s) */
   ret =
      RegModify (pDev, CMD1_SOP | (pChannel->nChannel), SOP_OFFSET_DSCR,
                 (DSCR_COR8) | (DSCR_PTG) | (DSCR_TG1_EN | DSCR_TG2_EN), nDscr);
   if (ret != IFX_SUCCESS)
      return IFX_ERROR;
   /* tone is played */
   pChannel->TapiTgToneData.nToneState = TAPI_CT_ACTIVE;
   if (pCh->Tone_PredefTimer == 0)
   {
      /* timer not created */
      LOG (TAPI_DRV, DBG_LEVEL_HIGH,
           ("\n\rDRV_ERROR: could not create tone timer\n\r"));
      return IFX_ERROR;
   }
   else
   {
      /* no error during creation of timer, so set and start tone timer */
      TAPI_SetTime_Timer (pCh->Tone_PredefTimer, nOnTime, IFX_FALSE, IFX_TRUE);
      return IFX_SUCCESS;
   }
}

/*******************************************************************************
Description:
   Stops the timer for tone generation
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_LOCAL IFX_int32_t TAPI_LL_Phone_Tone_Off (TAPI_CONNECTION * pChannel)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   IFX_int32_t ret = IFX_SUCCESS;

   /* delete tone timer */
   if ((pDev->pChannel[pChannel->nChannel].Tone_PredefTimer == 0) ||
       !TAPI_Stop_Timer (pDev->pChannel[pChannel->nChannel].Tone_PredefTimer))
   {
      LOG (TAPI_DRV, DBG_LEVEL_HIGH,
           ("\n\rDRV_ERROR: could not delete tone timer\n\r"));
      return IFX_ERROR;
   }
   /* call target function to stop tone generator(s) */
   ret =
      RegModify (pDev, CMD1_SOP | (pChannel->nChannel), SOP_OFFSET_DSCR,
                 (DSCR_COR8) | (DSCR_PTG) | (DSCR_TG1_EN | DSCR_TG2_EN), 0);
   if (ret == IFX_SUCCESS)
   {
      /* no tone is played */
      pChannel->TapiTgToneData.nToneState = TAPI_CT_IDLE;
      /* reset variable */
      pChannel->TapiTgToneData.bUseSecondTG = IFX_FALSE;
   }
   return ret;
}

/*******************************************************************************
Description:
   Low Level Init function. This function is normaly called from the
   generic init function.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
Return:
   Error code      - IFX_TRUE : Init successful
                   - IFX_FALSE: Init not successful
Remark:
   make sure DTMF and Hook interrupt are unmask. Important for the services
   Hook, Dial, DTMF
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Init (TAPI_CONNECTION * pChannel,
                                TAPI_INIT const *pInit)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t ret = IFX_SUCCESS;
   IFX_uint8_t mode = TAPI_NONE;
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   TAPI_RTP_CONF rtpConf;
   TAPI_JB_CONF jbConf;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */

   /* check if device initialization was done already */
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
      ret = AddCaps (pDev);
   }
   if (ret == IFX_SUCCESS)
   {
      /* set tapi init flag to prevent non tapi driver functions */
      pDev->nDevState |= DS_TAPI_INIT;
   }
   if (pInit->nMode != TAPI_NONE)
   {
      switch (pDev->nChipType)
      {
      case VINETIC_TYPE_C:
         /* error checking for C. No coder available and default must be PCM */
         switch (pInit->nMode)
         {
         case TAPI_VOICE_CODER:
            SET_ERROR (ERR_FUNC_PARM);
            break;
         case TAPI_DEFAULT:
            mode = TAPI_PCM_DSP;
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
         case TAPI_PCM_DSP:
         case TAPI_VOICE_CODER:
            SET_ERROR (ERR_FUNC_PARM);
            break;
         default:
            mode = TAPI_PCM_PHONE;
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
      case TAPI_DEFAULT:
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
      case TAPI_VOICE_CODER:
         ret = Basic_VoIPConf (pCh);
         if (ret == IFX_SUCCESS &&
             ((pDev->nEdspVers & ECMD_VERS_EDSP_PRT) == ECMD_VERS_EDSP_PRT_RTP))
         {
            memset (&rtpConf, 0, sizeof (TAPI_RTP_CONF));
            rtpConf.nEventPT = pChannel->nEvtPT;
            rtpConf.nEvents = TAPI_RTPEV_ALL;
            ret = TAPI_LL_Phone_RtpConf (pChannel, &rtpConf);
         }
         if (ret == IFX_SUCCESS)
         {
            memset (&jbConf, 0, sizeof (TAPI_JB_CONF));
            jbConf.nInitialSize = 0x02D0;
            jbConf.nMinSize = 0x0050;
            jbConf.nMaxSize = 0x05A0;
            jbConf.nPckAdpt = JB_CONF_PCK_VOICE;
            jbConf.nJbType = ADAPTIVE_JB_TYPE;
            jbConf.nScaling = 0x16;
            ret = TAPI_LL_Phone_JB_Set (pChannel, &jbConf);
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
#endif /* VIN_POLL */
         break;
      case TAPI_PCM_DSP:
         ret = Basic_PcmConf (pCh, mode);
         if (ret == IFX_SUCCESS)
            pCh->nFlags |= CF_TAPI_PCMMODE;
         break;
      case TAPI_PCM_PHONE:
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
      ret = InitChTimers (pCh);

   return ret;
}

/*******************************************************************************
Description:
   Enable / disable the automatic battery switch.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structured
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_AutoBatterySwitch (TAPI_CONNECTION * pChannel)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];

   /* Modify the automatic battery switch in the AUTOMOD regiser. Modify the
      cached value inside the driver and write the modified cached value to the
      device mailbox */

   /* set nBat */
   if (pChannel->TapiOpControlData.nBatterySw)
      AUTOMOD.value |= AUTO_BAT_ON;
   else
      AUTOMOD.value &= ~AUTO_BAT_MASK;

   /* switch to appropriate automatic mode battery */
   return wrReg (pCh, AUTOMOD_REG);
}

/*******************************************************************************
Description:
   Switch polarity.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structured
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Polarity (TAPI_CONNECTION * pChannel)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];

   /* Modify the reverse polarity in the BRC2 register. Modify the cached value
      inside the driver and write the modified cached value to the device
      mailbox */

   /* set nPol */
   if (pChannel->TapiOpControlData.nPolarity)
      BCR2.value |= BCR2_REV_POL;
   else
      BCR2.value &= ~BCR2_REV_POL;

   /* set appropriate reverse polarity */
   return wrReg (pCh, BCR2_REG);
}

/*******************************************************************************
Description:
   Prepare parameters and call the Target Configuration Function to switch the
   line mode.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   nMode           - linefeed mode
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_SwitchLine (TAPI_CONNECTION * pChannel,
                                      IFX_int32_t nMode)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t ret;

   switch (nMode)
   {
   case TAPI_LINEFEED_NORMAL:
      /* set active high operating mode */
      OPMOD_CUR.value = pDev->nActiveMode;
      ret = wrReg (pCh, OPMOD_CUR_REG);
      if (ret != IFX_SUCCESS)
         break;

      /* Disable the reverse polarity in the BRC2 register. Modify the cached
         value inside the driver and write the modified cached value to the
         device mailbox */
      BCR2.value &= ~BCR2_REV_POL;
      ret = wrReg (pCh, BCR2_REG);
      if (ret != IFX_SUCCESS)
         break;

      /* disable the automatic battery switch in the AUTOMOD regiser. Modify
         the cached value inside the driver and write the modified cached value
         to the device mailbox */
      AUTOMOD.value &= ~AUTO_BAT_MASK;
      ret = wrReg (pCh, AUTOMOD_REG);

      break;
   case TAPI_LINEFEED_REVERSED:
      /* set active high operating mode */
      OPMOD_CUR.value = pDev->nActiveMode;
      ret = wrReg (pCh, OPMOD_CUR_REG);
      if (ret != IFX_SUCCESS)
         break;

      /* Enable the reverse polarity in the BRC2 register. Modify the cached
         value inside the driver and write the modified cached value to the
         device mailbox */
      BCR2.value |= BCR2_REV_POL;
      ret = wrReg (pCh, BCR2_REG);
      if (ret != IFX_SUCCESS)
         break;

      /* disable the automatic battery switch in the AUTOMOD regiser. Modify
         the cached value inside the driver and write the modified cached value
         to the device mailbox */
      AUTOMOD.value &= ~AUTO_BAT_MASK;
      ret = wrReg (pCh, AUTOMOD_REG);

      break;
   case TAPI_LINEFEED_PARKED:
      /* set power down VBATH operating mode */
      OPMOD_CUR.value = BATH_BGND;
      ret = wrReg (pCh, OPMOD_CUR_REG);
      if (ret != IFX_SUCCESS)
         break;

      /* Disable the reverse polarity in the BRC2 register. Modify the cached
         value inside the driver and write the modified cached value to the
         device mailbox */
      BCR2.value &= ~BCR2_REV_POL;
      ret = wrReg (pCh, BCR2_REG);
      if (ret != IFX_SUCCESS)
         break;

      /* disable the automatic battery switch in the AUTOMOD regiser. Modify
         the cached value inside the driver and write the modified cached value
         to the device mailbox */
      AUTOMOD.value &= ~AUTO_BAT_MASK;
      ret = wrReg (pCh, AUTOMOD_REG);

      break;
   case TAPI_LINEFEED_PARKED_REVERSED:
      /* set sleep on VBATH operating mode */
      OPMOD_CUR.value = SLEEP_BATH_BGND;
      ret = wrReg (pCh, OPMOD_CUR_REG);
      if (ret != IFX_SUCCESS)
         break;

      /* Enable the reverse polarity in the BRC2 register. Modify the cached
         value inside the driver and write the modified cached value to the
         device mailbox */
      BCR2.value |= BCR2_REV_POL;
      ret = wrReg (pCh, BCR2_REG);
      if (ret != IFX_SUCCESS)
         break;

      /* disable the automatic battery switch in the AUTOMOD regiser. Modify
         the cached value inside the driver and write the modified cached value
         to the device mailbox */
      AUTOMOD.value &= ~AUTO_BAT_MASK;
      ret = wrReg (pCh, AUTOMOD_REG);

      break;
   case TAPI_LINEFEED_HIGH_IMPEDANCE:
      /* set Active with HIR operating mode */
      OPMOD_CUR.value = ACT_HIR;
      ret = wrReg (pCh, OPMOD_CUR_REG);
      break;
   case TAPI_LINEFEED_POWER_DOWN:
      /* set Power down High Impedance operating mode */
      OPMOD_CUR.value = POW_DOWN;
      ret = wrReg (pCh, OPMOD_CUR_REG);
      break;
   case TAPI_LINEFEED_GROUND_START:
      /* set Ground Start operating mode */
      OPMOD_CUR.value = GND_START;
      ret = wrReg (pCh, OPMOD_CUR_REG);
      break;
   case TAPI_LINEFEED_NORMAL_AUTO:
      /* set active high operating mode */
      OPMOD_CUR.value = pDev->nActiveMode;
      ret = wrReg (pCh, OPMOD_CUR_REG);
      if (ret != IFX_SUCCESS)
         break;

      /* Disable the reverse polarity in the BRC2 register. Modify the cached
         value inside the driver and write the modified cached value to the
         device mailbox */
      BCR2.value &= ~BCR2_REV_POL;
      ret = wrReg (pCh, BCR2_REG);
      if (ret != IFX_SUCCESS)
         break;

      /* enable the automatic battery switch in the AUTOMOD regiser. Modify the
         cached value inside the driver and write the modified cached value to
         the device mailbox */
      AUTOMOD.value |= AUTO_BAT_ON;
      ret = wrReg (pCh, AUTOMOD_REG);

      break;
   case TAPI_LINEFEED_REVERSED_AUTO:
      /* set active high operating mode */
      OPMOD_CUR.value = pDev->nActiveMode;
      ret = wrReg (pCh, OPMOD_CUR_REG);
      if (ret != IFX_SUCCESS)
         break;

      /* Enable the reverse polarity in the BRC2 register. Modify the cached
         value inside the driver and write the modified cached value to the
         device mailbox */
      BCR2.value |= BCR2_REV_POL;
      ret = wrReg (pCh, BCR2_REG);
      if (ret != IFX_SUCCESS)
         break;

      /* enable the automatic battery switch in the AUTOMOD regiser. Modify the
         cached value inside the driver and write the modified cached value to
         the device mailbox */
      AUTOMOD.value |= AUTO_BAT_ON;
      ret = wrReg (pCh, AUTOMOD_REG);

      break;
   case TAPI_LINEFEED_RING_BURST:
      switch (pChannel->TapiRingData.RingConfig.nMode)
      {
      case INTERNAL_UNBALANCED_ROT:
         OPMOD_CUR.value = RNG_RNG_GND;
         ret = wrReg (pCh, OPMOD_CUR_REG);
         break;
      case INTERNAL_UNBALANCED_ROR:
         OPMOD_CUR.value = RNG_TIP_GND;
         ret = wrReg (pCh, OPMOD_CUR_REG);
         break;
      case INTERNAL_BALANCED:
      default:
         OPMOD_CUR.value = RNG;
         ret = wrReg (pCh, OPMOD_CUR_REG);
         break;
      }

      /* Disable the reverse polarity in the BRC2 register. Modify the cached
         value inside the driver and write the modified cached value to the
         device mailbox */
      BCR2.value &= ~BCR2_REV_POL;
      ret = wrReg (pCh, BCR2_REG);
      if (ret != IFX_SUCCESS)
         break;

      /* disable the automatic battery switch in the AUTOMOD regiser. Modify
         the cached value inside the driver and write the modified cached value
         to the device mailbox */
      AUTOMOD.value &= ~AUTO_BAT_MASK;
      ret = wrReg (pCh, AUTOMOD_REG);

      break;
   case TAPI_LINEFEED_RING_PAUSE:
      switch (pChannel->TapiRingData.RingConfig.nMode)
      {
      case INTERNAL_UNBALANCED_ROT:
         OPMOD_CUR.value = RNG_PAUSE_TIP_GND;
         ret = wrReg (pCh, OPMOD_CUR_REG);
         break;
      case INTERNAL_UNBALANCED_ROR:
         OPMOD_CUR.value = RNG_PAUSE_RNG_GND;
         ret = wrReg (pCh, OPMOD_CUR_REG);
         break;
      case INTERNAL_BALANCED:
      default:
         OPMOD_CUR.value = RNG_PAUSE;
         ret = wrReg (pCh, OPMOD_CUR_REG);
         break;
      }
      break;
   case TAPI_LINEFEED_METER:
      /* set Active with Metering operating mode */
      OPMOD_CUR.value = ACT_HIGH_MET;
      ret = wrReg (pCh, OPMOD_CUR_REG);
      break;
   case TAPI_LINEFEED_ACTIVE_LOW:
   case TAPI_LINEFEED_NORMAL_LOW:
      OPMOD_CUR.value = ACT_LOW;
      ret = wrReg (pCh, OPMOD_CUR_REG);
      break;
   case TAPI_LINEFEED_REVERSED_LOW:
      /* set active low operating mode */
      OPMOD_CUR.value = ACT_LOW;
      ret = wrReg (pCh, OPMOD_CUR_REG);
      if (ret != IFX_SUCCESS)
         break;
      /* Enable the reverse polarity in the BRC2 register. Modify the cached
         value inside the driver and write the modified cached value to the
         device mailbox */
      BCR2.value |= BCR2_REV_POL;
      ret = wrReg (pCh, BCR2_REG);
      if (ret != IFX_SUCCESS)
         break;

      /* disable the automatic battery switch in the AUTOMOD regiser. Modify
         the cached value inside the driver and write the modified cached value
         to the device mailbox */
      AUTOMOD.value &= ~AUTO_BAT_MASK;
      ret = wrReg (pCh, AUTOMOD_REG);
      break;
   case TAPI_LINEFEED_ACTIVE_BOOSTED:
      OPMOD_CUR.value = ACT_BOOST;
      ret = wrReg (pCh, OPMOD_CUR_REG);
      break;
   case TAPI_LINEFEED_POWER_DOWN_RESISTIVE_SWITCH:
      OPMOD_CUR.value = PDNR_SWITCH;
      ret = wrReg (pCh, OPMOD_CUR_REG);
      break;
   case TAPI_LINEFEED_ACT_T:
      OPMOD_CUR.value = ACT_TEST_IN;
      ret = wrReg (pCh, OPMOD_CUR_REG);
      break;
   default:
      ret = IFX_ERROR;
      break;
   }

   return ret;
}

/*******************************************************************************
Description:
   Prepare parameters and call the Target Configuration Function to activate/
   deactivate the pcm interface.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   nMode           - 1: timeslot activated
                     0: timeslot deactivated
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_PCM_Activation (TAPI_CONNECTION * pChannel,
                                          IFX_uint32_t nMode)
{
   IFX_uint32_t IdxRx, BitValueRx;
   IFX_uint32_t IdxTx, BitValueTx;
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   FWM_PCM_CH *pPcmCh = &pDev->pPcmCh[pChannel->nChannel].pcm_ch;
   TAPI_PCM_CONFIG *pPcmCfg = &pChannel->TapiPCMData.PCMConfig;
   IFX_int32_t ret = IFX_SUCCESS;

   /* Activate Timeslots and set high way */
   switch (nMode)
   {
   case 0:
      /*
         the device variables PcmRxTs, PcmTxTs are global for all channels and
         must be protected against concurent tasks access */
      IFXOS_MutexLock (pDev->memberAcc);
      switch (pPcmCh->bit.cod)
      {
      case 0:                  /* LINEAR_16_BIT */
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
            LOG (TAPI_DRV, DBG_LEVEL_HIGH,
                 ("\n\rDRV_ERROR: Rx Slot is not available\n\r"));
            ret = IFX_ERROR;
            goto end_fkt;
         }

         if ((pDev->PcmTxTs[pPcmCh->bit.x_hw][IdxTx] & BitValueTx) == IFX_FALSE)
         {
            IFXOS_MutexUnlock (pDev->memberAcc);
            LOG (TAPI_DRV, DBG_LEVEL_HIGH,
                 ("\n\rDRV_ERROR: Tx Slot is not available\n\r"));
            ret = IFX_ERROR;
            goto end_fkt;
         }

         /* reserve the time slots now, because the parameter are OK */
         pDev->PcmRxTs[pPcmCh->bit.r_hw][IdxRx] &= ~BitValueRx;
         pDev->PcmTxTs[pPcmCh->bit.x_hw][IdxTx] &= ~BitValueTx;
         /* Fall through and do not 'break'! For 16-Bit PCM Mode, the next
            higher PCM time slot was reserved and now the used time slot will
            be reserved. That part is similar to the 8-Bit PCM Mode */
         /*lint -fallthrough */
      case 2:                  /* A_LAW_8_BIT */
      case 3:                  /* U_LAW_8_BIT */
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
            LOG (TAPI_DRV, DBG_LEVEL_HIGH,
                 ("\n\rDRV_ERROR: Rx Slot is not available\n\r"));
            ret = IFX_ERROR;
            goto end_fkt;
         }

         if ((pDev->PcmTxTs[pPcmCh->bit.x_hw][IdxTx] & BitValueTx) == IFX_FALSE)
         {
            IFXOS_MutexUnlock (pDev->memberAcc);
            LOG (TAPI_DRV, DBG_LEVEL_HIGH,
                 ("\n\rDRV_ERROR: Tx Slot is not available\n\r"));
            ret = IFX_ERROR;
            goto end_fkt;
         }

         /* reserve the time slots now, because the parameter are OK */
         pDev->PcmRxTs[pPcmCh->bit.r_hw][IdxRx] &= ~BitValueRx;
         pDev->PcmTxTs[pPcmCh->bit.x_hw][IdxTx] &= ~BitValueTx;
         /* Fall through and do not 'break'! For 16-Bit PCM Mode, the next
            higher PCM time slot was reserved and now the used time slot will
            be reserved. That part is similar to the 8-Bit PCM Mode */
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
      /* The device variables PcmRxTs, PcmTxTs are global for all channels and
         must be protected against concurent tasks access */
      IFXOS_MutexLock (pDev->memberAcc);
      /* check timeslot allocation */
      switch (pPcmCfg->nResolution)
      {
      case LINEAR_16_BIT:
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
            LOG (TAPI_DRV, DBG_LEVEL_HIGH,
                 ("\n\rDRV_ERROR: Rx Slot is not available\n\r"));
            ret = IFX_ERROR;
            goto end_fkt;
         }

         if (pDev->PcmTxTs[pPcmCfg->nHighway][IdxTx] & BitValueTx)
         {
            IFXOS_MutexUnlock (pDev->memberAcc);
            LOG (TAPI_DRV, DBG_LEVEL_HIGH,
                 ("\n\rDRV_ERROR: Tx Slot is not available\n\r"));
            ret = IFX_ERROR;
            goto end_fkt;
         }

         /* reserve the time slots now, because the parameter are OK */
         pDev->PcmRxTs[pPcmCfg->nHighway][IdxRx] |= BitValueRx;
         pDev->PcmTxTs[pPcmCfg->nHighway][IdxTx] |= BitValueTx;

         /* Fall through and do not 'break'! For 16-Bit PCM Mode, the next
            higher PCM time slot was reserved and now the used time slot will
            be reserved. That part is similar to the 8-Bit PCM Mode */
         /*lint -fallthrough */
      case A_LAW_8_BIT:
      case U_LAW_8_BIT:
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
            LOG (TAPI_DRV, DBG_LEVEL_HIGH,
                 ("\n\rDRV_ERROR: Rx Slot is not available\n\r"));
            ret = IFX_ERROR;
            goto end_fkt;
         }

         if (pDev->PcmTxTs[pPcmCfg->nHighway][IdxTx] & BitValueTx)
         {
            IFXOS_MutexUnlock (pDev->memberAcc);
            LOG (TAPI_DRV, DBG_LEVEL_HIGH,
                 ("\n\rDRV_ERROR: Tx Slot is not available\n\r"));
            ret = IFX_ERROR;
            goto end_fkt;
         }

         /* reserve the time slots now, because the parameter are OK */
         pDev->PcmRxTs[pPcmCfg->nHighway][IdxRx] |= BitValueRx;
         pDev->PcmTxTs[pPcmCfg->nHighway][IdxTx] |= BitValueTx;

         break;
      default:
         LOG (TAPI_DRV, DBG_LEVEL_HIGH,
              ("\n\rDRV_ERROR: This resolution is unknown\n\r"));
         IFXOS_MutexUnlock (pDev->memberAcc);
         ret = IFX_ERROR;
         goto end_fkt;
      }
      /* activate Timeslots */
      switch (pPcmCfg->nResolution)
      {
      case A_LAW_8_BIT:
         pPcmCh->bit.cod = 2;
         break;
      case U_LAW_8_BIT:
         pPcmCh->bit.cod = 3;
         break;
      case LINEAR_16_BIT:
      default:
         pPcmCh->bit.cod = 0;
         break;
      }
      pPcmCh->bit.xts = pPcmCfg->nTimeslotTX;
      pPcmCh->bit.rts = pPcmCfg->nTimeslotRX;
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
      LOG (TAPI_DRV, DBG_LEVEL_HIGH,
           ("DRV_ERROR: Error in (de)activating PCM Module!\n\r"));
   }

   return ret;
}

/*******************************************************************************
Description:
   Prepare parameters and call the Target Configuration Function to configure
   pcm interface.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pPCMConfig      - contains the new configuration for pcm interface
Return:
   IFX_SUCCESS/IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_PCM_Config (TAPI_CONNECTION * pChannel,
                                      TAPI_PCM_CONFIG const *pPCMConfig)
{
   IFX_int32_t ret = IFX_SUCCESS;
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;

   if ((pDev->nDevState & DS_PCM_EN) == IFX_FALSE)
   {
      LOG (TAPI_DRV, DBG_LEVEL_HIGH,
           ("DRV_ERROR: PCM Module not enabled during Initialization!\n\r"));
      ret = IFX_ERROR;
   }

   if (ret == IFX_SUCCESS)
   {
      /* check PCM resolution for supported values */
      switch (pPCMConfig->nResolution)
      {
      case A_LAW_8_BIT:
      case U_LAW_8_BIT:
      case LINEAR_16_BIT:
         break;
      default:
         ret = IFX_ERROR;
         break;
      }
   }

   if ((pPCMConfig->nTimeslotRX >= PCM_MAX_TS) ||
       (pPCMConfig->nTimeslotTX >= PCM_MAX_TS))
   {
      TRACE (TAPI_DRV, DBG_LEVEL_HIGH,
             ("\n\rDRV_ERROR: Number of PCM timeslot out of range\n\r"));
      ret = IFX_ERROR;
   }

   if (pPCMConfig->nHighway >= PCM_HIGHWAY)
   {
      TRACE (TAPI_DRV, DBG_LEVEL_HIGH,
             ("\n\rDRV_ERROR: Number of PCM Highway %d out of range\n\r",
              (int) pPCMConfig->nHighway));
      ret = IFX_ERROR;
   }

   return ret;
}

/*******************************************************************************
Description:
   Prepare parameters and call the Target Configuration Function to set
   the ring configuration.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pRingConfig     - pointer to ring config structure
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Ring_Config (TAPI_CONNECTION * pChannel,
                                       TAPI_RING_CONFIG const *pRingConfig)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   IFX_uint16_t nRtrMask = 0, nRtrVal = 0;
   IFX_int32_t ret = IFX_ERROR;

   switch (pRingConfig->nMode)
   {
   case INTERNAL_BALANCED:
   case INTERNAL_UNBALANCED_ROT:
   case INTERNAL_UNBALANCED_ROR:
      nRtrMask |= (RTR_REXT_EN | RTR_RTR_SENSE);
      break;
   case EXTERNAL_IT_CS:
      /* External Ringing enable : REXT-EN = 1 */
      /* IT Current Sence : REXT-SENSE = 0 */
      nRtrMask |= (RTR_REXT_EN | RTR_RTR_SENSE);
      nRtrVal |= RTR_REXT_EN;
      break;
   case EXTERNAL_IO_CS:
      /* External Ringing enable : REXT-EN = 1 */
      /* IT Current Sence : REXT-SENSE = 1 */
      nRtrMask |= (RTR_REXT_EN | RTR_RTR_SENSE);
      nRtrVal |= (RTR_REXT_EN | RTR_RTR_SENSE);
      break;
   default:
      LOG (TAPI_DRV, DBG_LEVEL_HIGH,
           ("\n\rDRV_ERROR: This case is not yet supported!\n\r"));
      break;
   }
   switch (pRingConfig->nSubmode)
   {
   case DC_RNG_TRIP_STANDARD:
      /* Ring Trip with DC selected : RTR-SEL = 0 */
      /* Standard Ring Trip : RTR-FAST = 0 */
      nRtrMask |= (RTR_RTR_FAST | RTR_RTR_SEL);
      break;
   case DC_RNG_TRIP_FAST:
      /* DC Ring Trip : RTR-SEL = 0 */
      /* Fast Ring Trip : RTR-FAST = 1 */
      nRtrMask |= (RTR_RTR_FAST | RTR_RTR_SEL);
      nRtrVal |= RTR_RTR_FAST;
      break;
   case AC_RNG_TRIP_STANDARD:
      /* AC Ring Trip : RTR-SEL = 1 */
      /* Standard Ring Trip : RTR-FAST = 0 */
      nRtrMask |= (RTR_RTR_FAST | RTR_RTR_SEL);
      nRtrVal |= RTR_RTR_SEL;
      break;
   case AC_RNG_TRIP_FAST:
      /* AC_Ring Trip : RTR-SEL = 1 */
      /* Fast Ring Trip : RTR-FAST = 1 */
      nRtrMask |= (RTR_RTR_FAST | RTR_RTR_SEL);
      nRtrVal |= (RTR_RTR_FAST | RTR_RTR_SEL);
      break;
   default:
      LOG (TAPI_DRV, DBG_LEVEL_HIGH,
           ("\n\rDRV_ERROR: This case is not yet supported!\n\r"));
      break;
   }
   if (nRtrMask != 0)
   {
      ret =
         RegModify (pDev, CMD1_SOP | (pChannel->nChannel), SOP_OFFSET_RTR,
                    nRtrMask, nRtrVal);
   }

   return ret;
}

/*******************************************************************************
Description:
   Play a tome of given index
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   res - resource number which is used for playing the tone. The available
         resources is device dependend.
   pToneCoeff  - handle to the tone definition to play
   dst         - destination where to play the tone
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Tone_Play (TAPI_CONNECTION * pChannel,
                                     IFX_uint8_t res,
                                     COMPLEX_TONE const *pToneCoeff,
                                     TAPI_TONE_DST dst)
{
   IFX_int32_t ret = IFX_SUCCESS, src = TAPI_TONE_SRC_DSP;
   IFX_uint8_t ch = pChannel->nChannel;
   TAPI_DEV *pTapiDev = pChannel->pTapiDevice;
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   TAPI_SIMPLE_TONE const *pToneSimple = NULL;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[ch];

   /* check for valid resource number */
   if (pDev->nToneGenCnt < res)
   {
      SET_ERROR (ERR_NORESOURCE);
      return IFX_ERROR;
   }
   if (pToneCoeff->type != TAPI_TONE_TYPE_PREDEF)
   {
      pToneSimple = &pToneCoeff->tone.simple;
      /* set source where the tone is played out. Check default and set it
         explicitly here */
      if (pDev->nChipType == VINETIC_TYPE_S)
      {
         if (dst == TAPI_TONE_DST_NET)
         {
            SET_ERROR (ERR_FUNC_PARM);
            return IFX_ERROR;
         }
         src = TAPI_TONE_SRC_TG;
      }
      else
      {
         if (res != 0)
            src = TAPI_TONE_SRC_TG;
      }
   }
   pChannel->TapiComplexToneData[res].src = src;
   switch (pToneCoeff->type)
   {
   case TAPI_TONE_TYPE_PREDEF:
      /* check if line is in active state */
      if (!
          (pChannel->TapiOpControlData.nLineMode == TAPI_LINEFEED_NORMAL ||
           pChannel->TapiOpControlData.nLineMode == TAPI_LINEFEED_REVERSED ||
           pChannel->TapiOpControlData.nLineMode == TAPI_LINEFEED_NORMAL_AUTO ||
           pChannel->TapiOpControlData.nLineMode ==
           TAPI_LINEFEED_REVERSED_AUTO))
      {
         ret = TAPI_Phone_Set_Linefeed (pChannel, TAPI_LINEFEED_NORMAL);
      }

      if (ret == IFX_SUCCESS)
      {
         /* turn tone on and start timer */
         ret = TAPI_LL_Phone_Tone_On (pChannel, pToneCoeff->tone.nPredefined);
      }
      break;
   case TAPI_TONE_TYPE_SIMPLE:
      switch (src)
      {
      default:
      case TAPI_TONE_SRC_DEFAULT:
      case TAPI_TONE_SRC_DSP:
         /* disable DTMF / AT generator because it is overlaid with the UTG */
         pDev->pSigCh[ch].sig_dtmfgen.bit.en = 0;
         ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_dtmfgen.value,
                         CMD_SIG_DTMFGEN_LEN);
         /* Activate the universal tone generator disable the DTMF generator
            and program the simple tone sequence */
         if (ret == IFX_SUCCESS)
            ret = Tone_UTG_Start (pChannel, pToneSimple, dst);
         break;
      case TAPI_TONE_SRC_TG:
         /* play tone on ALM. Switch it on and start the timer */
         Tone_TG_Play (pCh);
         /* we play the first step */
         pCh->nToneStep = 1;
         /* set tone timer */
         TAPI_SetTime_Timer (pCh->pToneRes[res].Tone_Timer,
                             pToneSimple->cadence[0], IFX_FALSE, IFX_TRUE);
         break;
      }
      break;
   case TAPI_TONE_TYPE_COMP:
      /* disable DTMF / AT generator because it is overlaid with the UTG */
      pDev->pSigCh[ch].sig_dtmfgen.bit.en = 0;
      ret =
         CmdWrite (pDev, pDev->pSigCh[ch].sig_dtmfgen.value,
                   CMD_SIG_DTMFGEN_LEN);
      pToneSimple = &(pTapiDev->pToneTbl
                      [pToneCoeff->tone.composed.tones[0]].tone.simple);
      if (ret == IFX_SUCCESS)
         ret = Tone_UTG_Start (pChannel, pToneSimple, dst);
      break;
   default:
      ret = IFX_ERROR;
      break;
   }
   return ret;
}

/*******************************************************************************
Description:
   Play a tome of given index
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   res - resource number which is used for playing the tone. The available
         resources is device dependend.
   pToneCoeff  - handle to the tone definition to play
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Tone_Stop (TAPI_CONNECTION * pChannel,
                                     IFX_uint8_t res,
                                     COMPLEX_TONE const *pToneCoeff)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   FWM_SIG_UTG *pUtgCmd = &pDev->pSigCh[pChannel->nChannel].utg;
   IFX_int32_t ret, src;

   /* Stop any voice path teardowm timer that may be running */
   TAPI_Stop_Timer (pCh->pToneRes[res].Tone_Timer);
   if (pToneCoeff->type == TAPI_TONE_TYPE_PREDEF)
   {
      if (pChannel->TapiTgToneData.nToneState != TAPI_CT_IDLE)
         /* turn tone off and stop timer */
         return TAPI_LL_Phone_Tone_Off (pChannel);
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
         src = TAPI_TONE_SRC_TG;
      }
      else
      {
         if (res == 0)
            src = TAPI_TONE_SRC_DSP;
         else
            src = TAPI_TONE_SRC_TG;
      }
      if (src == TAPI_TONE_SRC_DSP)
      {
         /* Stop tone playing for tones which is using the UTG Force the UTG to
            stop immediately if already disabled */
         if (pUtgCmd->bit.sm == 1 && pUtgCmd->bit.en == 0)
         {
            /* enable the UTG first because it is already disabled and auto
               stop is activated. So first enabled it and set auto stop to
               false */
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
         /* reenable the DTMF module */
         pDev->pSigCh[pCh->nChannel - 1].sig_dtmfgen.bit.en = 1;
         ret =
            CmdWrite (pDev, pDev->pSigCh[pCh->nChannel - 1].sig_dtmfgen.value,
                      CMD_SIG_DTMFGEN_LEN);
      }
      else
      {
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
      }
   }
   return ret;
}

/*******************************************************************************
Description:
   Configure metering mode of chip
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   nMode       - use TTX (0) or reverse polarity (1)
   nFreq       - Default/12 KHz/16 KHz
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Meter_Config (TAPI_CONNECTION * pChannel,
                                        IFX_uint8_t nMode, IFX_uint8_t nFreq)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t ret = IFX_SUCCESS;

   /* Note: Switching TTX 12 KHz or TTX 16 KHz requires a new programming of
      CRAM coefficients. Therefore, the switch will not be done here anymore
      and is the job of CRAM download. nFreq is set as default. */
   /* nFreq = TAPI_METER_FREQ_DEFAULT; */

   /* set register value according to config TTX => BCR2_TTX_EN = 1 / Reverse
      Polarity => BCR2_TTX_EN = 0 Modify the cached value inside the driver and
      write the modified cached value to the device mailbox */
   if (nMode == TAPI_METER_MODE_TTX)
      BCR2.value |= BCR2_TTX_EN;
   else
      BCR2.value &= ~BCR2_TTX_EN;

   ret = wrReg (pCh, BCR2_REG);

   return ret;
}

/*******************************************************************************
Description:
   Sets the tone level of the tone currently played.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pToneLevel      - pointer to TAPI_TONE_LEVEL structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remark:
   To avoid float point calculation, the solution adapted was to provide a
   complete CRAM coef table with level, quantised values and CRAM settings
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Tone_Set_Level (TAPI_CONNECTION * pChannel,
                                          TAPI_TONE_LEVEL const *pToneLevel)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
#ifdef VIN_V21_SUPPORT
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
#endif /* VIN_V21_SUPPORT */
#ifdef VIN_V14_SUPPORT
   COEFF_TAB_VAL_ENTRY tab_entry;
#endif /* VIN_V14_SUPPORT */
   IFX_int32_t ret = IFX_SUCCESS;

   switch (pDev->nChipMajorRev)
   {
#ifdef VIN_V14_SUPPORT
   case VINETIC_V1x:
      memset (&tab_entry, 0, sizeof (COEFF_TAB_VAL_ENTRY));
      /* set coefficients according to choosing generator */
      if (pToneLevel->nGenerator == 0x01 || pToneLevel->nGenerator == 0xFF)
      {
         tab_entry.levelVal = (IFX_int32_t) pToneLevel->nLevel;
         /* write level to CRAM */
         ret =
            Cram_rwbyVal (pDev, pChannel->nChannel, IOSET, VINETIC_TG1_AMPL,
                          &tab_entry);
      }
      if (pToneLevel->nGenerator == 0x02 || pToneLevel->nGenerator == 0xFF)
      {
         tab_entry.levelVal = (IFX_int32_t) pToneLevel->nLevel;
         /* write level to CRAM */
         ret =
            Cram_rwbyVal (pDev, pChannel->nChannel, IOSET, VINETIC_TG2_AMPL,
                          &tab_entry);
      }

      TRACE (TAPI_DRV, DBG_LEVEL_HIGH,
             ("Vinetic TG Level mdB (quant) = %ld\n\r",
              (IFX_int32_t) tab_entry.quantVal));
      break;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
   case VINETIC_V2x:
      /* configure the amplitude of the first tone generator */
      if (pToneLevel->nGenerator == 0x01 || pToneLevel->nGenerator == 0xFF)
      {
         TG1A.value = (IFX_uint16_t) pToneLevel->nLevel;
         ret = wrReg (pCh, TG1A_REG);
      }
      /* configure the amplitude of the second tone generator */
      if ((ret == IFX_SUCCESS) &&
          (pToneLevel->nGenerator == 0x02 || pToneLevel->nGenerator == 0xFF))
      {
         TG2A.value = (IFX_uint16_t) pToneLevel->nLevel;
         ret = wrReg (pCh, TG2A_REG);
      }
      break;
#endif /* VIN_V21_SUPPORT */
   default:
      SET_ERROR (ERR_UNKNOWN_VERSION);
      ret = IFX_ERROR;
      break;
   }

   return ret;
}

#ifdef TAPI_DTMF
/*******************************************************************************
Description:
   Translate Chip Specific Digit to Tapi Digit
Arguments:
   pDev            - handle to TAPI_CONNECTION structure
   pToneLevel      - pointer to TAPI_TONE_LEVEL structure
Return:
   none
Remark:
   This function is called in the VINETIC Interrupt Routine when a Digit is
   detected!
*******************************************************************************/
IFX_void_t VINETIC_Tapi_Event_Dtmf (TAPI_CONNECTION * pChannel,
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
/*******************************************************************************
Description:
   Do low level DTMF cid configuration
Arguments:
   pChannel     - handle to TAPI_CONNECTION structure
   pDtmfCid     - Dtmf Cid configuration structure
Return:
   IFX_SUCCESS/IFX_ERROR
Remark:
   For Vinetic, the DTMF digit duration time and the time between 2 DTMF digits
   can be programmed via DTMF/AT Generator Coefficients. The generator should be
   disable when programming the coefficients.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_CID_ConfDtmf (TAPI_CONNECTION * pChannel,
                                        TAPI_DTMFCID const *pDtmfCid)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t ret;

   /* check times to set */
   if ((pDtmfCid->digitTime > MAX_DIGIT_TIME) ||
       (pDtmfCid->interDigitTime > MAX_INTERDIGIT_TIME))
   {
      LOG (TAPI_DRV, DBG_LEVEL_HIGH,
           ("\n\rDRV_ERROR: Max digit time or "
            "inter digit time is 127 ms\n\r"));
      return IFX_ERROR;
   }
   /* Write DTMF/AT generator coefficients */
   /** \todo How to pass gain and attenuation? */
   ret = Dsp_DtmfConf (pCh, 0xFF, 0xFF, (pDtmfCid->digitTime * 2),
                       (pDtmfCid->interDigitTime * 2), 0xFF, 0xFF);

   return ret;
}

/*********************************************************************************
Description:
   This function returns appropriate dtmf code
Arguments:
   nChar -  digit to encode
Return:
   encoded digit or -1 if error
Remarks:
   For the vinetic firmware, following caracters are coded as follows:
       '*' = 0x2A (ASCII) = 0x0A (VINETIC)
       '#' = 0x23 (ASCII) = 0x0B (VINETIC)
       'A' = 0x41 (ASCII) = 0x0C (VINETIC)
       'B' = 0x42 (ASCII) = 0x0D (VINETIC)
       'C' = 0x43 (ASCII) = 0x0E (VINETIC)
       'D' = 0x44 (ASCII) = 0x0F (VINETIC)
       '0' ...  '9' as is.
*******************************************************************************/
IFX_int8_t TAPI_LL_Phone_Dtmf_GetCode (IFX_char_t nChar)
{
   IFX_int8_t nEncodedDigit;

   switch (nChar)
   {
   case '*':
      nEncodedDigit = 0x0A;
      break;
   case '#':
      nEncodedDigit = 0x0B;
      break;
   case 'A':
      nEncodedDigit = 0x0C;
      break;
   case 'B':
      nEncodedDigit = 0x0D;
      break;
   case 'C':
      nEncodedDigit = 0x0E;
      break;
   case 'D':
      nEncodedDigit = 0x0F;
      break;
   default:
      if ((nChar >= '0') && (nChar <= '9'))
         nEncodedDigit = (nChar - '0');
      else
         nEncodedDigit = IFX_ERROR;
      break;
   }

   return (nEncodedDigit);
}

/*********************************************************************************
Description:
   configure CID sending over CID sender (with FSK)
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pCidConfig      - Cid configuration structure
Return:
   IFX_SUCCESS/IFX_ERROR
Remark:
   If HLEV or V23 are to change, Caller Id sender must be disabled so that
   this change takes effect. So the function will check if one of these 2
   parameters changes and will set EN = 0 if. If this is the case, Caller Id
   sender will be enabled later on if it was active before configuration.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_CID_ConfFsk (TAPI_CONNECTION * pChannel,
                                       TAPI_CID_CONFIG const *pCidConfig)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   IFX_uint16_t pCmd[2] = { 0 },
                pData[3] = {0},
                nChange = 0, nVal = 0;
   IFX_int32_t ret = IFX_SUCCESS;
   IFX_uint32_t bEn = 0;
   IFX_uint32_t nV23 = 0;

   /* EOP Cmd */
   pCmd[0] = (CMD1_EOP | (pCh->nChannel - 1));
   /* CID SEND */
   pCmd[1] = ECMD_CID_SEND;
   ret = CmdRead (pDev, pCmd, pData, 1);
   if (ret == IFX_SUCCESS)
   {
      /* set V23 configuration */
      nV23 = pCidConfig->nSpec;
      /* in case of NTT cid transmission, the sending is done according to V23
         standard. Only the FSK buffer is different. */
      if (pCidConfig->nSpec == TAPI_CID_NTT)
      {
         nV23 = TAPI_CID_V23;
      }
      /* save EN state */
      bEn = ((pData[2] & SIG_CID_EN) != 0);
      /* look at bits to change */
      if (((pData[2] & SIG_CID_AD) >> 13) != pCidConfig->nAutoDeac)
      {
         nChange |= SIG_CID_AD;
         nVal |= ((pCidConfig->nAutoDeac << 13) & SIG_CID_AD);
      }
      if (((pData[2] & SIG_CID_HLEV) >> 12) != pCidConfig->nHlev)
      {
         nChange |= SIG_CID_HLEV;
         nVal |= ((pCidConfig->nHlev << 12) & SIG_CID_HLEV);
      }
      if (((pData[2] & SIG_CID_V23) >> 11) != nV23)
      {
         nChange |= SIG_CID_V23;
         nVal |= ((nV23 << 11) & SIG_CID_V23);
      }
      /* check if EN should be set to 0 */
      if ((nChange & (SIG_CID_HLEV | SIG_CID_V23)) && bEn)
         nChange |= SIG_CID_EN;

      /* set new values */
      pData[2] = (pData[2] & ~nChange) | nVal;
      ret = CmdWrite (pDev, pData, 1);
   }
   /* check if Caller Id should be enabled */
   if ((ret == IFX_SUCCESS) && (nChange & SIG_CID_EN) && bEn)
   {
      pData[2] |= SIG_CID_EN;
      ret = CmdWrite (pDev, pData, 1);
   }

   return ret;
}

/*******************************************************************************
Description:
   send cid data to vinetic
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pCid            - contains CID data
Return:
   IFX_SUCCESS/IFX_ERROR
Remarks:
   In case of FSK Cid, data are mapped wordwise into the internal channel
   structure and the CID machine is trigerred.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_CID_Sending (TAPI_CONNECTION * pChannel,
                                       TAPI_CID_DATA const *pCid)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   VINETIC_CID_TYPE nCidType;
   IFX_int32_t ret = IFX_SUCCESS;

   switch (pChannel->TapiCidConfig.nMode)
   {
   case TAPI_CID_FSK_MODE:
      if (pCh->cidSend.nState == CID_SETUP)
      {
         pCh->cidSend.nCidCnt =
            (pCid->nCidParamLen / 2) + (pCid->nCidParamLen % 2);
         pCh->cidSend.nOdd = (pCid->nCidParamLen % 2);
         cpb2w (pCh->cidSend.pCid, pCid->cidParam, (pCh->cidSend.nCidCnt * 2));
         if (pChannel->TapiCid2.bCid2IsOn == IFX_TRUE)
         {
            nCidType = CID_TYPE_2;
         }
         else
         {
            if (pChannel->TapiCidConfig.nSpec == TAPI_CID_NTT)
               nCidType = CID_TYPE_1_NTT;
            else
               nCidType = CID_TYPE_1;
         }
         ret = SIGNL_SetCIDCoeff (pCh, nCidType);
         /* now start non blocking low level machine */
         if (ret == IFX_SUCCESS)
         {
            pCh->cidSend.nCidType = nCidType;
            ret = VINETIC_CidFskMachine (pCh);
         }
      }
      else
      {
         ret = IFX_ERROR;
         LOG (TAPI_DRV, DBG_LEVEL_HIGH,
              ("\n\rDRV_ERROR: Cid sending is ongoing"
               " in this channel %d\n\r", pChannel->nChannel));
      }
      break;
   case TAPI_CID_DTMF_MODE:
      if (pCh->dtmfSend.state == DTMF_READY)
      {
         memcpy (pCh->cidSend.pCid, pCid->cidParam, pCid->nCidParamLen);
         ret =
            VINETIC_DtmfStart (pCh, pCh->cidSend.pCid, pCid->nCidParamLen, 1,
                               VINETIC_CidDtmfMachine, IFX_TRUE);
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

/*********************************************************************************
Description:
   Start caller id receiver
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS/IFX_ERROR
Remarks:
   It is checked if the signalling channel is active. If no, error code
   is given back, otherwise the caller id receiver is activated.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_CidRx_Start (TAPI_CONNECTION * pChannel)
{
   IFX_int32_t ret = IFX_ERROR;
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];

   /* check if signalling channel is active */
   if (pDev->pSigCh[pChannel->nChannel].sig_ch.bit.en == IFX_TRUE)
   {
      ret = SetCidRx (pCh, IFX_TRUE);
   }

   return ret;
}

/*********************************************************************************
Description:
   Stop caller id receiver
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS/IFX_ERROR
Remarks:
   It is checked if the signalling channel is active. If no, error code
   is given back, otherwise the caller id receiver is activated.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_CidRx_Stop (TAPI_CONNECTION * pChannel)
{
   IFX_int32_t ret = IFX_ERROR;
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];

   /* check if signalling channel is active */
   if (pDev->pSigCh[pChannel->nChannel].sig_ch.bit.en == IFX_TRUE)
   {
      ret = SetCidRx (pCh, IFX_FALSE);
   }

   return ret;
}

/*******************************************************************************
Description:
   Caller id receiver status
Arguments:
   pChannel       - handle to TAPI_CONNECTION structure
   pCidRxStatus   - handle to TAPI_CIDRX_STATUS structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   The status variable must be protected because of possible mutual access.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_CidRx_Status (TAPI_CONNECTION * pChannel,
                                        TAPI_CIDRX_STATUS * pCidRxStatus)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;

   /* protect access to status structure , lock interrupts */
   Vinetic_IrqLockDevice (pDev);
   *pCidRxStatus = pChannel->TapiCidRx.stat;
   Vinetic_IrqUnlockDevice (pDev);

   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   CID Receiver data collection.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
Return:
   none
Remark :
   This function is called by the function handling vinetic packets in
   interrupt level.
*******************************************************************************/
IFX_void_t TAPI_LL_Phone_CidRxDataCollect (TAPI_CONNECTION * pChannel)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   TAPI_CIDRX_STATUS *new = &pChannel->TapiCidRx.stat;
   PACKET *pPacket = &pCh->cidRxPacket;
   TAPI_CIDRX_DATA *pBuf = NULL;
   IFX_int32_t nOdd, nCarrierDet;
   IFX_uint32_t nLen;

   /* set cd and odd */
   nOdd = pPacket->cmd2 & CMD2_ODD;
   nCarrierDet = ntohs (pPacket->pData[0]) & 0x8000;
   switch (pPacket->cmd2 & CMD2_LEN)
   {
   case 1:
      /* if CD = 1, caller id receiving just started */
      if (nCarrierDet)
         new->nStatus = TAPI_CIDRX_STAT_ONGOING;
      /* if CD = 0, carrier lost. No Cid data more */
      else
         new->nStatus = TAPI_CIDRX_STAT_DATA_RDY;
      break;
   case 2:
      /* check odd bit to get data length */
      if (nOdd)
         nLen = 1;
      else
         nLen = 2;
      /* get buffer for cid buffering */
      pBuf = (TAPI_CIDRX_DATA *) TAPI_Phone_GetCidRxBuf (pChannel, nLen);
      if (pBuf == NULL)
      {
         new->nError = TAPI_CIDRX_ERR_READ;
         break;
      }
      /* at least the high byte is valid */
      pBuf->data[pBuf->nSize++] = HIGHBYTE (ntohs (pPacket->pData[1]));
      /* if odd bit isn't set, then the low byte is the second data */
      if (nOdd == 0)
         pBuf->data[pBuf->nSize++] = LOWBYTE (ntohs (pPacket->pData[1]));
      /* if CD = 0, data are ready */
      if (nCarrierDet == 0)
         new->nStatus = TAPI_CIDRX_STAT_DATA_RDY;
      break;
   default:
      /* shouldn't be */
      break;
   }
   /* event for the user : error or data available */
   if ((new->nStatus == TAPI_CIDRX_STAT_DATA_RDY) ||
       (new->nError == TAPI_CIDRX_ERR_READ))
   {
      TAPI_Phone_Event_CidRx (pChannel);
   }
}

/*******************************************************************************
Description:
   Get the cid data already collected in an ongoing collection.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   pCidRxData  - handle to TAPI_CIDRX_DATA structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   in cid ongoing state, this function protects the reading of actual data
   buffer from interrupts, because the data reading is done on interrupt level
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Get_CidRxDataCollected (TAPI_CONNECTION * pChannel,
                                                  TAPI_CIDRX_DATA * pCidRxData)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   TAPI_CIDRX_DATA *pData = pChannel->TapiCidRx.pData;
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

/*******************************************************************************
Description:
   Send CPE alerting signal (CAS).
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   bAStxOk must be set to IFX_TRUE if tone was succesfully sent.
*******************************************************************************/
/** \todo As soon as the tapi tone interface basing on UTG will be fully
   available, the out commented parts of this function will be used and the
   actually executed part removed. The tone api is still in implementation and
   test phase. */
IFX_int32_t TAPI_LL_Phone_CID_Send_AS (TAPI_CONNECTION * pChannel)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   TAPI_CID2 *pCid2 = &pChannel->TapiCid2;
   IFX_int32_t ret = IFX_SUCCESS;

   pCid2->state = TAPI_CID2_SEND_AS;
   /* event flag is IFX_FALSE */
   pCh->pTapiCh->TapiCid2.bAStxOk = IFX_FALSE;
   switch (pCid2->conf.stdSelect)
   {
   case TAPI_CID2_TIA:
   case TAPI_CID2_ETSI:
   case TAPI_CID2_BT:
      /* DTMF Generation ongoing ? */
      if (pCh->dtmfSend.state != DTMF_READY)
      {
         ret = IFX_ERROR;
         LOG (TAPI_DRV, DBG_LEVEL_HIGH,
              ("\n\rDRV_ERROR: DTMF transmission is "
               " ongoing on this channel\n\r"));
      }
      /* This function sends a dual alert tone (2130 Hz/ 2750 Hz)to the vinetic
         via the dtmf generator (FW). The tone duration will be set via DTMF
         generator coefficients. Note: the event transmission must be
         deactivated during this operation. */
      /* set duration time and level for DTMF tone : Level : TIA is -22dB
         (dynamic range is -14 dB to -34 dB) ETSI is -18dB Use default : -18dB
         => Level = 128*10^(dBval / 20) = 16,11 = 0x10

       */
      if (ret == IFX_SUCCESS)
      {
         ret =
            Dsp_DtmfConf (pCh, 0x10, 0x10,
                          (pCid2->conf.std.defaultS.casDuration * 2), 0xFF,
                          0xFF, 0xFF);
      }

      /* send dual alert tone : DTC = 01 0010b = 2130 Hz / 2750 Hz */
      if (ret == IFX_SUCCESS)
      {
         /* set DTMF DTC Dual Alert Tone command and start dtmf sending routine
          */
         pCh->cidSend.pCid[0] = ECMD_DTMFGEN_DTC_DAT;
         ret =
            VINETIC_DtmfStart (pCh, (IFX_uint16_t *) pCh->cidSend.pCid, 1, 1,
                               VINETIC_CidDtmfMachine, IFX_FALSE);
      }
      break;
   case TAPI_CID2_NTT:
      /* Programm UTG directly here for a quick test */
      {
         IFX_uint16_t pUtgCoefSet[27] =
            { 0x0600, 0xD119, 0x7FFF, 0x0000, 0x0000,
            0x0000, 0x0000, 0x1B43, 0x3441, 0x1E1D,
            0x0000, 0x2D2D, 0x2D00, 0x00C8, 0x10C0,
            0x0064, 0x2000, 0x00C8, 0x3062, 0x0200,
            0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
            0x7FFF, 0x7FFF
         };
         IFX_uint16_t pUtgCmd[2] = {
            0x0600, 0x4A01
         };
         IFX_uint16_t pUtgSet[3];

         pUtgCoefSet[0] &= ~CMD1_CH;
         pUtgCoefSet[0] |= pChannel->nChannel;
         ret = CmdWrite (pDev, pUtgCoefSet, 25);
         /* read UTG settings */
         if (ret == IFX_SUCCESS)
         {
            pUtgCmd[0] |= pChannel->nChannel;
            ret = CmdRead (pDev, pUtgCmd, pUtgSet, 1);
         }
         /* Set UTG */
         if (ret == IFX_SUCCESS)
         {
            /* set UTGNR = ch, EN = 1, SM = 0, A2 = 01 */
            pUtgSet[2] = ((pUtgSet[2] &
                          ~(CMD1_CH | SIG_UTG_SM | SIG_UTG_A2)) | SIG_UTG_EN |
                            SIG_UTG_A2_VOICE | pChannel->nChannel);
            ret = CmdWrite (pDev, pUtgSet, 1);
         }
         /* Set : EN = 0, SM = 1 */
         if (ret == IFX_SUCCESS)
         {
            pUtgSet[2] = ((pUtgSet[2] & ~SIG_UTG_EN) | SIG_UTG_SM);
            ret = CmdWrite (pDev, pUtgSet, 1);
         }
      }
      break;
   default:
      return IFX_ERROR;
   }
   /* Tone sending was just trigerred. It is now unknown if the tone was sent.
      if this happens, a wakeup will be done and a flag will be set. To avoid a
      deadlock, wait with a time out */
   if (ret == IFX_SUCCESS)
   {
      /* after 500 ms, the tone must have been played already */
      IFXOS_WaitEvent_timeout (pCid2->asEvt, 500);
      /* check event flag */
      if (pCh->pTapiCh->TapiCid2.bAStxOk == IFX_FALSE)
      {
         /* no tone played without timeout */
         ret = IFX_ERROR;
      }
   }

   return ret;
}
#endif /* TAPI_CID */

#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
/*******************************************************************************
Description:
   Sets the Recording Codec
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   nCodec          - the codec for the record audio channel
Return:
   IFX_FALSE - codec not accepted
   IFX_TRUE  - codec is enabled
Remarks:
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
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Set_Rec_Codec (TAPI_CONNECTION * pChannel,
                                         IFX_int32_t nCodec)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   FWM_COD_CH *pCodCh = &pDev->pCodCh[pChannel->nChannel].cod_ch;
   IFX_int32_t ret = IFX_SUCCESS, nCod = 0, nPte = -1;

   /* check coder module */
   if (pDev->nDevState & DS_COD_EN)
      /* set encoder val according to codec */
      nCod = getEncoder (nCodec);
   else
   {
      ret = IFX_ERROR;
      SET_ERROR (ERR_CODCONF_NOTVALID);
   }
   /* PTE value suitable ? if not, search a suitable pte */
   if ((ret == IFX_SUCCESS) && (nCod != IFX_ERROR))
   {
      /* get matching PTE to set */
      if (CheckENCnPTE ((IFX_uint8_t) pCodCh->bit.pte, (IFX_uint8_t) nCod) ==
          IFX_ERROR)
      {
         nPte = getConvPTE ((IFX_uint8_t) pCodCh->bit.pte, nCodec);
      }
   }
   else
      ret = IFX_ERROR;
   /* set Encoder: The coder channel will be enabled at recording start set Pte
      if suitable pte from pte table. The settings must be protected against
      concurrent access */
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
   if (ret == IFX_SUCCESS)
      pChannel->nCodec = nCodec;
   /* release fw mgs lock */

   return ret;
}
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */

/*******************************************************************************
Description:
   Starts the Recording.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
  This function enables the Coder Channel module with the existing
 configuration.
  It resets, also, the members of the VINETIC_CHANNEL structure concerning the
  streaming and clears the channel Fifo, dedicated for incoming voice packets.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Start_Recording (TAPI_CONNECTION * pChannel)
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

/*******************************************************************************
Description:
   Stops the Recording.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   This function disables the Coder Channel module, sets to IFX_TRUE the
   bRxAbort
   flag of the VINETIC_CHANNEL structure and wakes up processes to read
   the last arrived data.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Stop_Recording (TAPI_CONNECTION * pChannel)
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

   IFXOS_WakeUpEvent (pCh->wqRead);
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */

   return ret;
}

/*******************************************************************************
Description:
   Starts the Playing.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS
Remarks:
  This function does the same as TAPI_LL_Phone_Start_Recording :
  start Recording / playing are done by enabling the coder channel.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Start_Playing (TAPI_CONNECTION * pChannel)
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

/*******************************************************************************
Description:
   Stops the Playing.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS
Remarks:
  This function does the same as TAPI_LL_Phone_Stop_Recording :
  stop Recording / playing are done by disabling the coder channel.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Stop_Playing (TAPI_CONNECTION * pChannel)
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
         /* enocder is not running -> disable the whole coder-module */
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
/*******************************************************************************
Description:
   configure rt(c)p for a new connection
Arguments:
   pChannel        -handle to TAPI_CONNECTION structure
   pRtpConf        -handle to TAPI_RTP_CONF structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_RtpConf (TAPI_CONNECTION * pChannel,
                                   TAPI_RTP_CONF const *pRtpConf)
{
   IFX_int32_t ret = IFX_SUCCESS, bEnableCoder;
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   IFX_uint8_t ch = pChannel->nChannel;
   IFX_uint16_t pCmd[2] = { 0 }, pData[5];
   FWM_SIG_CH_CONF sigConf;

   memset (&sigConf, 0, sizeof (FWM_SIG_CH_CONF));
   sigConf.value[0] = (CMD1_EOP | pChannel->nChannel);
   sigConf.value[1] = ECMD_SIG_CH_RTP;
   sigConf.bit.ssrch = HIGHWORD (pRtpConf->nSsrc);
   sigConf.bit.ssrcl = LOWWORD (pRtpConf->nSsrc);
   sigConf.value[4] = pRtpConf->nSeqNr;

   if (pRtpConf->nEvents == TAPI_RTPEV_NO)
      /* Ignore the setting for nEventPT and transmit tones as voice in the
         configured codec */
      sigConf.bit.eventPt = pChannel->nEvtPT;
   else
   {
      if (pRtpConf->nEventPT == 0)
      {
         SET_ERROR (ERR_WRONG_EVPT);
         return IFX_ERROR;
      }
      sigConf.bit.eventPt = pRtpConf->nEventPT & 0x7F;
   }
   /* set event pt, ssrc, sequence nr for signalling channel : This has to be
      done with inactive signalling channel */
#if 0
   pDev->pSigCh[ch].sig_ch.bit.en = 0;
   /* enable */
   ret = CmdWrite (pDev, pDev->pSigCh[ch].sig_ch.value, CMD_SIG_CH_LEN);
   if (ret == IFX_SUCCESS)
#endif /* 0 */
   {
#ifdef FW_ETU
      sigConf.bit.coderCh = ch;
      switch (pRtpConf->nEvents)
      {
      case TAPI_RTPEV_NO:
         /* No Event transmission support */
         sigConf.bit.evMaskTOG = 0x7;
         break;
      case TAPI_RTPEV_DEFAULT:
      case TAPI_RTPEV_ONLY:
         /* muting the voice in sig channel rtp */
         sigConf.bit.a1 = 1;
         sigConf.bit.a2 = 1;
         /* all playout always enabled */
         sigConf.bit.evMaskTOG = 0x7;
         break;
      case TAPI_RTPEV_ALL:
         /* event transmission support no muting */
         sigConf.bit.a1 = 2;
         sigConf.bit.a2 = 2;
         /* all playout always enabled */
         sigConf.bit.evMaskTOG = 0x7;
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
         /* enable event transmission for all detectors, switched on or not */
         if (pRtpConf->nEvents == TAPI_RTPEV_NO)
         {
            pDev->pSigCh[ch].sig_atd1.bit.et = 0;
            pDev->pSigCh[ch].sig_atd2.bit.et = 0;
            pDev->pSigCh[ch].sig_utd1.bit.et = 0;
            pDev->pSigCh[ch].sig_utd2.bit.et = 0;
         }
         else
         {
            pDev->pSigCh[ch].sig_atd1.bit.et = 1;
            pDev->pSigCh[ch].sig_atd2.bit.et = 1;
            pDev->pSigCh[ch].sig_utd1.bit.et = 1;
            pDev->pSigCh[ch].sig_utd2.bit.et = 1;
         }
         ret =
            CmdWrite (pDev, pDev->pSigCh[ch].sig_atd1.value, CMD_SIG_ATD_LEN);
         if (ret == IFX_SUCCESS)
            ret =
               CmdWrite (pDev, pDev->pSigCh[ch].sig_atd2.value,
                         CMD_SIG_ATD_LEN);
         if (ret == IFX_SUCCESS)
            ret =
               CmdWrite (pDev, pDev->pSigCh[ch].sig_utd1.value,
                         CMD_SIG_UTD_LEN);
         if (ret == IFX_SUCCESS)
            ret =
               CmdWrite (pDev, pDev->pSigCh[ch].sig_utd2.value,
                         CMD_SIG_UTD_LEN);
      }
#else
      ret = CmdWrite (pDev, sigConf.value, 4);
#endif /* FW_ETU */
   }
   /* store the information for downstream service. */
   if (ret == IFX_SUCCESS)
   {
      if (pRtpConf->nEventPT)
      {
         /* Configure transmission through events and use payload type value
            from nEventPT */
         pChannel->nEvtPT = pRtpConf->nEventPT;
      }
      pChannel->nSsrc = pRtpConf->nSsrc;
   }
#if 0
   /* set event transmission support accordingly : The signalling channel has
      to be active */
   if (ret == IFX_SUCCESS)
   {
      /* signalling channel has to be active for this purpose. this also
         activates the tone detectors */
      Dsp_SwitchSignModules (pCh, IFX_TRUE);
   }
#endif /* 0 */
   /* do event transmission settings */
   if (ret == IFX_SUCCESS)
   {
      pCmd[0] = CMD1_EOP | ch;
      pCmd[1] = ECMD_DTMF_REC;
      ret = CmdRead (pDev, pCmd, pData, 1);
      if (ret == IFX_SUCCESS)
      {
         switch (pRtpConf->nEvents)
         {
         case TAPI_RTPEV_NO:
            /* No Event transmission support */
            pData[2] &= ~(SIG_DTMFREC_ET | SIG_DTMFREC_AS);
            break;
         case TAPI_RTPEV_DEFAULT:
         case TAPI_RTPEV_ONLY:
         case TAPI_RTPEV_ALL:
            /* event transmission support */
            pData[2] |= SIG_DTMFREC_ET | SIG_DTMFREC_AS;
            break;
         default:
            ret = IFX_ERROR;
            break;
         }
         ret = CmdWrite (pDev, pData, 1);
      }
   }
   /* set the RTP time stamp */
   if (ret == IFX_SUCCESS)
   {
      pData[0] = CMD1_EOP | ch;
      pData[1] = ECMD_COD_RTP;
      pData[2] = HIGHWORD (pRtpConf->nTimestamp);
      pData[3] = LOWWORD (pRtpConf->nTimestamp);
      ret = CmdWrite (pDev, pData, 2);
   }
   /* set ssrc, sequence nr for coder channel : This has to be done with
      inactive coder channel */
   if (ret == IFX_SUCCESS)
   {
      /* save current coder status */
      bEnableCoder = pDev->pCodCh[ch].cod_ch.bit.en;
      ret = TAPI_LL_Voice_Enable (pChannel, 0);
      /* do rtp settings for coder channel */
      if (ret == IFX_SUCCESS)
      {
         pCmd[0] = CMD1_EOP | ch;
         pCmd[1] = ECMD_COD_CHRTP;
         ret = CmdRead (pDev, pCmd, pData, 3);
         if (ret == IFX_SUCCESS)
         {
            pData[2] = HIGHWORD (pRtpConf->nSsrc);
            pData[3] = LOWWORD (pRtpConf->nSsrc);
            pData[4] = pRtpConf->nSeqNr;
            ret = CmdWrite (pDev, pData, 3);
         }
      }
      /* enable back coder channel if enable before settings */
      if (ret == IFX_SUCCESS && bEnableCoder)
         ret = TAPI_LL_Voice_Enable (pChannel, 1);
   }
   /* error case */
   if (ret != IFX_SUCCESS)
   {
      LOG (TAPI_DRV, DBG_LEVEL_HIGH,
           ("\n\rDRV_ERROR: RTP Configuration failed\n\r"));
   }

   return ret;
}

/*******************************************************************************
Description:
   configure a new payload type
Arguments:
   pChannel          -handle to TAPI_CONNECTION structure
   pRtpPTConf        -handle to TAPI_RTP_PT_CONF structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Rtp_SetPayloadType (TAPI_CONNECTION * pChannel,
                                              TAPI_RTP_PT_CONF const
                                              *pRtpPTConf)
{
   IFX_int32_t ret = IFX_SUCCESS;
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   IFX_uint16_t pCmd[2] = { 0 }, pData[20] =
   {
   0};

   /* SetTraceLevel (VINETIC, DBG_LEVEL_LOW); */
   /* setup 1st cmd header */
   pCmd[0] = (CMD1_EOP | pChannel->nChannel);
   /* read actual coder channel RTP settings */
   pCmd[1] = ECMD_COD_CHRTP;
   ret = CmdRead (pDev, pCmd, pData, 18);
   /* set different payload types */
   if (ret == IFX_SUCCESS)
   {
      /* G711, ALAW, 64 KBps */
      pData[5] = pRtpPTConf->nPT[ALAW] << 8;
      /* G711, uLAW, 64 KBps */
      pData[5] |= pRtpPTConf->nPT[MLAW] | 0x8080;
      /* G726, 16 KBps */
      pData[6] = pRtpPTConf->nPT[G726_16] << 8;
      /* G726, 24 KBps */
      pData[6] |= pRtpPTConf->nPT[G726_24] | 0x8080;
      /* G726, 32 KBps */
      pData[7] = pRtpPTConf->nPT[G726_32] << 8;
      /* G726, 40 KBps */
      pData[7] |= pRtpPTConf->nPT[G726_40] | 0x8080;
      /* G728, 16 KBps */
      pData[12] = (pData[12] & ~0xFF00) | (pRtpPTConf->nPT[G728] << 8);
      /* G729, 8KBps */
      pData[13] = (pRtpPTConf->nPT[G729] << 8) | pRtpPTConf->nPT[G729_E];
      /* G723, 5,3 KBps */
      pData[18] = (pRtpPTConf->nPT[G723_53] << 8);
      /* G723, 6,3 KBps */
      pData[18] |= pRtpPTConf->nPT[G723_63];

      ret = CmdWrite (pDev, pData, 18);
   }
   else
   {
      LOG (TAPI_DRV, DBG_LEVEL_HIGH,
           ("\n\rDRV_ERROR: can't read"
            " Coder Channel RTP configuration.\n\r"));
   }
   /* SetTraceLevel (VINETIC, DBG_LEVEL_HIGH); */

   return ret;
}

/*******************************************************************************
Description:
   gets the RTCP statistic information for the addressed channel
Arguments:
   pChannel        -handle to TAPI_CONNECTION structure
   pRTCP           -handle to TAPI_RTCP_DATA structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   RTCP statistics for the specified channel are reset after the completion of
   the read request. A Coder Channel Statistics write command is issued on this
   purpose.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_RTCP_GetStatistics (TAPI_CONNECTION * pChannel,
                                              TAPI_RTCP_DATA * pRTCP)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   IFX_uint16_t pCmd[2] = { 0 }
   , pData[16] =
   {
   0};
   IFX_int32_t ret = IFX_SUCCESS;

   /* EOP command */
   pCmd[0] = CMD1_EOP | (pChannel->nChannel);
   /* Read Coder Channel Statistics */
   pCmd[1] = ECMD_COD_CHRTCP;
   ret = CmdRead (pDev, pCmd, pData, 14);
   if (ret == IFX_SUCCESS)
   {
      /* assign the RTCP values to the corresponding members of the
         TAPI_RTCP_DATA structure */
      pRTCP->rtp_ts = (pData[2] << 16) | pData[3];
      pRTCP->psent = (pData[4] << 16) | pData[5];
      pRTCP->osent = (pData[6] << 16) | pData[7];
      pRTCP->rssrc = (pData[8] << 16) | pData[9];
      pRTCP->fraction = ((pData[10] & 0xFF00) >> 8);
      pRTCP->lost = ((pData[10] & 0xFF) << 16) | pData[11];
      pRTCP->last_seq = (pData[12] << 16) | pData[13];
      pRTCP->jitter = (pData[14] << 16) | pData[15];
      /* return stored SSRC */
      pRTCP->ssrc = pChannel->nSsrc;
      /* The calling control task will set the parameters lsr and dlsr */
   }
   else
   {
      LOG (TAPI_DRV, DBG_LEVEL_HIGH,
           ("\n\rDRV_ERROR: can't read Coder Channel RTCP statistics.\n\r"));
   }

   return ret;
}

/*******************************************************************************
Description:
   Prepare the RTCP statistic information for the addressed channel
   (in ISR context)
Arguments:
   pChannel        -handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
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
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_RTCP_PrepareStatistics (TAPI_CONNECTION * pChannel)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   IFX_uint16_t pCmd[2];
   IFX_int32_t ret = IFX_SUCCESS;

   /* EOP command with read bit set (the Short Command bit remains 0) */
   pCmd[0] = CMD1_EOP | CMD1_RD | (pChannel->nChannel);
   /* Read Coder Channel Statistics (the length field is 0, see VINETIC User's
      Manual) */
   pCmd[1] = ECMD_COD_CHRTCP | CMD_COD_RTCP_LEN;
   /* write read-request using ISR-specific write function */
   ret = CmdWriteIsr (pDev, pCmd, 0);
   if (ret != IFX_SUCCESS)
   {
      LOG (TAPI_DRV, DBG_LEVEL_HIGH,
           ("\n\rDRV_ERROR: can't request Coder Channel RTCP statistics.\n\r"));
   }

   return ret;
}

/*********************************************************************************
Description:
   Gathers previously requested RTCP statistic information for the addressed
   channel
Arguments:
   pChannel        -handle to TAPI_CONNECTION structure
   pRTCP           -handle to the returned data.
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   See description under TAPI_LL_Phone_RTCP_PrepareStatistics.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_RTCP_GatherStatistics (TAPI_CONNECTION * pChannel,
                                                 TAPI_RTCP_DATA * pRTCP)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   IFX_uint16_t *pData, ch = pChannel->nChannel;

   if (pDev->pCodCh[ch].rtcp_update == IFX_TRUE)
   {
      pData = pDev->pCodCh[ch].rtcp;
      /* assign the RTCP values to the corresponding members of the
         TAPI_RTCP_DATA structure */
      pRTCP->rtp_ts = (pData[0] << 16) | pData[1];
      pRTCP->psent = (pData[2] << 16) | pData[3];
      pRTCP->osent = (pData[4] << 16) | pData[5];
      pRTCP->ssrc = (pData[6] << 16) | pData[7];
      pRTCP->fraction = ((pData[8] & 0xFF00) >> 8);
      pRTCP->lost = ((pData[8] & 0xFF) << 16) | pData[9];
      pRTCP->last_seq = (pData[10] << 16) | pData[11];
      pRTCP->jitter = (pData[12] << 16) | pData[13];
      pDev->pCodCh[ch].rtcp_update = IFX_FALSE;

      return IFX_SUCCESS;
   }
   else
      return IFX_ERROR;
}

/*******************************************************************************
Description:
   resets  RTCP statistics
Arguments:
   pChannel          -handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_RTCP_Reset (TAPI_CONNECTION * pChannel)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   IFX_uint16_t pCmd[2] = { 0 };

   /* reset RTCP Statistics by sending a Coder Channel Statistics (RTCP
      command) write command of length 0 */
   pCmd[0] = CMD1_EOP | (pChannel->nChannel);
   pCmd[1] = ECMD_COD_CHRTCP;

   return CmdWrite (pDev, pCmd, 0);
}

/*******************************************************************************
Description:
   configures the jitter buffer
Arguments:
   pChannel   - handle to TAPI_CONNECTION structure
   pJbConf    - handle to TAPI_JB_CONF structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   This function caches the actual Jitter buffer configuration set.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_JB_Set (TAPI_CONNECTION * pChannel,
                                  TAPI_JB_CONF const *pJbConf)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   IFX_uint16_t pData[6] = { 0 };
   IFX_int32_t ret;

   /* setup command for jb configuration */
   pData[0] = CMD1_EOP | (pChannel->nChannel);
   pData[1] = ECMD_COD_CHJB;
   pData[2] = COD_CHJB_PJE | COD_CHJB_DVF;
   /* do JB settings */
   switch (pJbConf->nJbType)
   {
   case FIXED_JB_TYPE:
      /* NAM (Non Adaptive Mode not yet considered by Specification. Default is
         : NAM = 0 means the jitter buffer tries to keep the play out delay
         close to the programmed initial jitter buffer size
         (pJbConf->nInitialSize) */
      break;
   case ADAPTIVE_JB_TYPE:
      /* set adaptive type */
      pData[2] |= COD_CHJB_ADAPT;
      /* do settings for local adaptation and sample interpolation */
      switch (pJbConf->nLocalAdpt)
      {
      case LOCAL_ADAPT_OFF:
         break;
      case LOCAL_ADAPT_ON:
         pData[2] |= COD_CHJB_LOC;
         break;
      case LOCAL_ADAPT_SI_ON:
         pData[2] |= (COD_CHJB_LOC | COD_CHJB_SI);
         break;
      default:
         LOG (TAPI_DRV, DBG_LEVEL_HIGH,
              ("\n\rDRV_ERROR: Wrong Parameter. Can't set Jitter Buffer\n\r"));
         return IFX_ERROR;
      }
      break;
   default:
      LOG (TAPI_DRV, DBG_LEVEL_HIGH,
           ("\n\rDRV_ERROR: Wrong Parameter. Can't set Jitter Buffer\n\r"));
      return IFX_ERROR;
   }
   /* scaling factor */
   pData[2] |= ((IFX_uint8_t) pJbConf->nScaling << 8);
   switch (pJbConf->nPckAdpt)
   {
   case 0:
   case 1:
      /* not supported */
      break;
   case JB_CONF_PCK_VOICE:
      /* Packet adaption is optimized for voice. Reduced adjustment speed and
         packet repetition is off */
      /* ADAP = 1, ->see above PJE = 1 SF = default ->see above */
      pData[2] |= COD_CHJB_PJE;
      break;
   case JB_CONF_PCK_DATA:
      /* Packet adaption is optimized for data */
      pData[2] |= COD_CHJB_RAD | COD_CHJB_PRP | COD_CHJB_DVF;
      break;
   default:
      SET_ERROR (ERR_FUNC_PARM);
      return IFX_ERROR;
   }
   /* Initial Size of JB */
   pData[3] = pJbConf->nInitialSize;
   /* minimum Size of JB */
   pData[4] = pJbConf->nMinSize;
   /* maximum Size of JB */
   pData[5] = pJbConf->nMaxSize;

   ret = CmdWrite (pDev, pData, 4);
   /* cache actual jitter buffer config */
   if (ret == IFX_SUCCESS)
      memcpy (&pChannel->TapiJbData, pJbConf, sizeof (TAPI_JB_CONF));

   return ret;
}

/*******************************************************************************
Description:
   query jitter buffer statistics
Arguments:
   pChannel          -handle to TAPI_CONNECTION structure
   pJbData           -handle to TAPI_JB_DATA structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_JB_GetStatistics (TAPI_CONNECTION * pChannel,
                                            TAPI_JB_DATA * pJbData)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   IFX_uint16_t pCmd[2] = { 0 }
   , pData[28] =
   {
   0};
   IFX_int32_t ret = IFX_SUCCESS;

   /* setup command to read jb configuration */
   pCmd[0] = CMD1_EOP | (pChannel->nChannel);
   pCmd[1] = ECMD_COD_CHJB;
   ret = CmdRead (pDev, pCmd, pData, 1);
   /* get type of configured jitter buffer */
   if (ret == IFX_SUCCESS)
   {
      switch (pData[2] & COD_CHJB_ADAPT)
      {
      case 0:
         /* jitter buffer type */
         pJbData->nType = FIXED_JB_TYPE;
         break;
      case 1:
      default:
         /* jitter buffer type */
         pJbData->nType = ADAPTIVE_JB_TYPE;
         break;
      }
      /* read Coder Channel JB Statistics */
      pCmd[0] = CMD1_EOP | (pChannel->nChannel);
      pCmd[1] = ECMD_COD_CHJBSTAT;
      ret = CmdRead (pDev, pCmd, pData, 26);
   }
   /* assign the JB statistics values to the corresponding members of the
      TAPI_JB_DATA structure */
   if (ret == IFX_SUCCESS)
   {
      /* incoming time, not supported anymore */
      pJbData->nInTime = 0;
      /* Comfort Noise Generation, not supported anymore */
      pJbData->nCNG = 0;
      /* Bad Frame Interpolation, not supported anymore */
      pJbData->nBFI = 0;
      /* max packet delay, not supported anymore */
      pJbData->nMaxDelay = 0;
      /* minimum packet delay, not supported anymore */
      pJbData->nMinDelay = 0;
      /* network jitter value, not supported anymore */
      pJbData->nNwJitter = 0;
      /* play out delay */
      pJbData->nPODelay = pData[2];
      /* max play out delay */
      pJbData->nMaxPODelay = pData[3];
      /* min play out delay */
      pJbData->nMinPODelay = pData[4];
      /* current jitter buffer size */
      pJbData->nBufSize = pData[5];
      pJbData->nMaxBufSize = pData[6];
      pJbData->nMinBufSize = pData[7];
      /* received packet number */
      pJbData->nPackets = ((pData[8] << 16) | pData[9]);
      /* lost packets number, not supported anymore */
      pJbData->nLost = 0;
      /* invalid packet number (discarded) */
      pJbData->nInvalid = pData[10];
      /* duplication packet number, not supported anymore */
      pJbData->nDuplicate = 0;
      /* late packets number */
      pJbData->nLate = pData[11];
      /* early packets number */
      pJbData->nEarly = pData[12];
      /* resynchronizations number */
      pJbData->nResync = pData[13];
      /* new support */
      pJbData->nIsUnderflow = ((pData[14] << 16) | pData[15]);
      pJbData->nIsNoUnderflow = ((pData[16] << 16) | pData[17]);
      pJbData->nIsIncrement = ((pData[18] << 16) | pData[19]);
      pJbData->nSkDecrement = ((pData[20] << 16) | pData[21]);
      pJbData->nDsDecrement = ((pData[22] << 16) | pData[23]);
      pJbData->nDsOverflow = ((pData[24] << 16) | pData[25]);
      pJbData->nSid = ((pData[26] << 16) | pData[27]);
   }
   else
   {
      LOG (TAPI_DRV, DBG_LEVEL_HIGH,
           ("\n\rDRV_ERROR: JB Data reading failed\n\r"));
   }

   return ret;
}

/*******************************************************************************
Description:
   resets jitter buffer statistics
Arguments:
   pChannel          -handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_JB_Reset (TAPI_CONNECTION * pChannel)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   IFX_uint16_t pCmd[2] = { 0 };

   /* reset JB Statistics by sending a Coder Channel JB Statistics write
      command of length 0 */
   pCmd[0] = CMD1_EOP | (pChannel->nChannel);
   pCmd[1] = ECMD_COD_CHJBSTAT;

   return CmdWrite (pDev, pCmd, 0);
}

/*******************************************************************************
Description:
   configures AAL fields for a new connection
Arguments:
   pChannel          -handle to TAPI_CONNECTION structure
   pAalConf          -handle to TAPI_AAL_CONF structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   The coder channel has to be disabled before the AAL channel configuration
   can be done. The user task should make sure it activate the coder channel
   after having set this configuration.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Aal_Set (TAPI_CONNECTION * pChannel,
                                   TAPI_AAL_CONF const *pAalConf)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   IFX_uint16_t pCmd[3] = { 0 }, pData[3] =
   {
   0};
   IFX_int32_t ret = IFX_SUCCESS;

   /* set cmd 1 */
   pCmd[0] = (CMD1_EOP | pChannel->nChannel);

   /* write aal start timestamp */
   pCmd[1] = ECMD_COD_AALCONF;
   pCmd[2] = pAalConf->nTimestamp;
   ret = CmdWrite (pDev, pCmd, 1);
   /* read actual aal coder channel configuration */
   if (ret == IFX_SUCCESS)
   {
      pCmd[1] = ECMD_COD_CHAAL;
      ret = CmdRead (pDev, pCmd, pData, 1);
   }
   /* disable coder channel */
   if (ret == IFX_SUCCESS)
      ret = TAPI_LL_Voice_Enable (pChannel, 0);
   /* set connection Id : This has to be done with disabled coder channel */
   if (ret == IFX_SUCCESS)
   {
      pData[2] &= ~COD_CHAAL_CID;
      pData[2] |= (pAalConf->nCid << 8) & COD_CHAAL_CID;
      ret = CmdWrite (pDev, pData, 1);
   }
   /* \todo what about signalling channel */

   return ret;
}

#if 0
/*******************************************************************************
Description:
   switches Coder after a Decoder status change.
Arguments:
   pChannel          -handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
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
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Aal_SwitchCoder (TAPI_CONNECTION * pChannel)
{
   IFX_uint8_t nPteNew = 0, ch = pChannel->nChannel;
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   IFX_uint16_t *pDecChg = pDev->pCodCh[ch].dec_status;
   VINETIC_CODCH *pCodCh = &pDev->pCodCh[ch];
   IFX_int32_t ret = IFX_SUCCESS;

   SetTraceLevel (VINETIC, DBG_LEVEL_LOW);

   /* new packet time value */
   switch (pDecChg[2] & COD_CHDECSTAT_PTD)
   {
   case COD_CHDECSTAT_5MS:
      nPteNew = COD_CH_PTE_5MS;
      break;
   case COD_CHDECSTAT_5_5MS:
      nPteNew = COD_CH_PTE_5_5MS;
      break;
   case COD_CHDECSTAT_10MS:
      nPteNew = COD_CH_PTE_10MS;
      break;
   case COD_CHDECSTAT_11MS:
      nPteNew = COD_CH_PTE_11MS;
      break;
   case COD_CHDECSTAT_20MS:
      nPteNew = COD_CH_PTE_20MS;
      break;
   case COD_CHDECSTAT_30MS:
      nPteNew = COD_CH_PTE_30MS;
      break;
   default:
      LOG (TAPI_DRV, DBG_LEVEL_HIGH,
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
         TRACE (TAPI_DRV, DBG_LEVEL_HIGH,
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

/*******************************************************************************
Description:
   Programs an AAL profile
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pProfile        - handle to TAPI_AAL_PROFILE structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   none
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_AalProfile (TAPI_CONNECTION * pChannel,
                                      TAPI_AAL_PROFILE const *pProfile)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   IFX_int32_t ret = IFX_SUCCESS, i, nEnc, nMax = 0;
   IFX_uint16_t pCmd[2], pData[14], nUUI;

   if (pProfile->rows < 1 || pProfile->rows > 10)
      return IFX_ERROR;

   memset (pData, 0, sizeof (IFX_uint16_t) * 14);
   pCmd[0] = (CMD1_EOP | pChannel->nChannel);
   pCmd[1] = ECMD_COD_CHAAL | 2;
   /* read first both entries */
   ret = CmdRead (pDev, pCmd, pData, 2);
   for (i = 0; i < pProfile->rows; i++)
   {
      nEnc = getEncoder ((IFX_int32_t) pProfile->codec[i]);
      if (nEnc == IFX_ERROR)
         return IFX_ERROR;
      switch (pProfile->nUUI[i])
      {
      case RANGE_12_15:
      case RANGE_8_11:
      case RANGE_4_7:
      case RANGE_0_3:
         nMax = 4;
         break;
      case RANGE_0_7:
      case RANGE_8_15:
         nMax = 2;
         break;
      default:
         nMax = 0;
         nUUI = 0;
         break;
      }

      switch (pProfile->nUUI[i])
      {
      case RANGE_12_15:
         nUUI = 3 * 4;
         break;
      case RANGE_8_11:
         nUUI = 2 * 4;
         break;
      case RANGE_4_7:
         nUUI = 4;
         break;
      case RANGE_8_15:
         nUUI = 8;
         break;
      default:
         nUUI = 0;
         break;
      }
      pData[i + 4] =
         (IFX_uint16_t) ((IFX_uint8_t) pProfile->
                         len[i] << 10) | (IFX_uint16_t) (nUUI << 6) |
         (IFX_uint16_t) ((IFX_uint8_t) nEnc);
   }
   /* clear the UUIS field and retain SQNR-INTV */
   pData[3] &= ~COD_CHAAL_UUIS;
   /* set the UUI subrange parameter according to the maximum ranges */
   switch (nMax)
   {
   case 0:
      pData[3] |= COD_CHAAL_UUI_1RANGE;
      break;
   case 1:
      pData[3] |= COD_CHAAL_UUI_2RANGES;
      break;
   case 4:
      pData[3] |= COD_CHAAL_UUI_4RANGES;
      break;
   case 8:
      pData[3] |= COD_CHAAL_UUI_8RANGES;
      break;
   case 16:
      pData[3] |= COD_CHAAL_UUI_16RANGES;
      break;
   default:
      SET_ERROR (ERR_INVALID);
      return IFX_ERROR;
   }
   ret = CmdWrite (pDev, pData, 12);
   return ret;
}

/*******************************************************************************
Description:
  Enables or Disables voice coder channel according to nMode
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   nMode           - 0: off, 1: on
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   To succesfully modify the coder channel settings, at least the two first
   command Words should be programed.
   Function should protect access tpo fw messages
*******************************************************************************/
IFX_int32_t TAPI_LL_Voice_Enable (TAPI_CONNECTION * pChannel, IFX_uint8_t nMode)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   FWM_COD_CH *pCodCh = &pDev->pCodCh[pChannel->nChannel].cod_ch;
   IFX_int32_t ret = IFX_SUCCESS, bSet = IFX_FALSE, i;

   /* protect fwmsg against concurrent tasks */
   IFXOS_MutexLock (pDev->memberAcc);
   if (pChannel->nChannel == 3 && nMode == 1 &&
       pDev->nChipMajorRev == VINETIC_V1x)
   {
      /* check if any CPTD is activated */
      for (i = 0; i < pDev->nMaxRes; i++)
      {
         if (VIN_ECMD_SIGCPT_EN_GET (pDev->pSigCh[i].nCpt) == 1)
         {
            /* unlock */
            IFXOS_MutexUnlock (pDev->memberAcc);
            SET_ERROR (ERR_NORESOURCE);
            return IFX_ERROR;
         }
      }
   }
   /* switch on */
   if ((nMode == 1) && !(pCodCh->bit.en))
   {
      pCodCh->bit.en = 1;
      bSet = IFX_TRUE;
   }
   /* switch off */
   else if ((nMode == 0) && (pCodCh->bit.en))
   {
      pCodCh->bit.en = 0;
      bSet = IFX_TRUE;
   }
   if (bSet)
      ret = CmdWrite (pDev, (IFX_uint16_t *) pCodCh, 2);
   /* unlock */
   IFXOS_MutexUnlock (pDev->memberAcc);

   return ret;
}

/*******************************************************************************
Description:
   Connect analog phone channels directly
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   pMap        - handle to TAPI_PHONE_MAPPING structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Channel_Add (TAPI_CONNECTION * pChannel,
                                       TAPI_PHONE_MAPPING const *pMap)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   IFX_uint32_t nThisCh = pChannel->nChannel;
   int err;

   if ((pMap->nPhoneCh >= pDev->nAnaChan) || (nThisCh >= pDev->nAnaChan))
   {
      SET_ERROR (ERR_FUNC_PARM);
      return IFX_ERROR;
   }
   err =
      Con_ConnectPrepare (&pDev->pAlmCh[nThisCh].signal,
                          &pDev->pAlmCh[pMap->nPhoneCh].signal, 0);
   err =
      Con_ConnectPrepare (&pDev->pAlmCh[pMap->nPhoneCh].signal,
                          &pDev->pAlmCh[nThisCh].signal, 0);
   if (err == IFX_SUCCESS)
      err = Con_ConnectConfigure (pDev);
   else
   {
      SET_ERROR (ERR_NO_FREE_INPUT_SLOT);
   }
   return err;
}

/*******************************************************************************
Description:
   Disconnect analog phone channels
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   pMap        - handle to TAPI_PHONE_MAPPING structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Channel_Remove (TAPI_CONNECTION * pChannel,
                                          TAPI_PHONE_MAPPING const *pMap)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   IFX_uint32_t nThisCh = pChannel->nChannel;
   int err;

   if ((pMap->nPhoneCh >= pDev->nAnaChan) || (nThisCh >= pDev->nAnaChan))
   {
      SET_ERROR (ERR_FUNC_PARM);
      return IFX_ERROR;
   }

   /* connect ALM to SIG */
   err =
      Con_DisconnectPrepare (&pDev->pAlmCh[nThisCh].signal,
                             &pDev->pAlmCh[pMap->nPhoneCh].signal, 0);
   /* connect SIG to ALM */
   err =
      Con_DisconnectPrepare (&pDev->pAlmCh[pMap->nPhoneCh].signal,
                             &pDev->pAlmCh[nThisCh].signal, 0);
   if (err == IFX_SUCCESS)
      err = Con_ConnectConfigure (pDev);
   else
   {
      SET_ERROR (ERR_CON_INVALID);
   }
   return err;
}

/*******************************************************************************
Description:
   Adds a data channel to an analog phone device
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   pMap        - handle to TAPI_DATA_MAPPING structure
Return:
   IFX_SUCCESS / IFX_ERROR
Remarks:
   Does not yet support conferencing!
*******************************************************************************/
IFX_int32_t TAPI_LL_Data_Channel_Add (TAPI_CONNECTION * pChannel,
                                      TAPI_DATA_MAPPING const *pMap)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   IFX_uint32_t nThisCh = pChannel->nChannel;
   int err;
   VINDSP_MODULE *pDstMod = IFX_NULL;

   switch (pMap->nChType)
   {
   case TAPI_DATA_MAPTYPE_CODER:
      SET_ERROR (ERR_NOTSUPPORTED);
      return IFX_ERROR;
   case TAPI_DATA_MAPTYPE_PCM:
      if (pMap->nDstCh < pDev->nPcmCnt)
         pDstMod = &pDev->pPcmCh[pMap->nDstCh].signal;
      break;
   case TAPI_DATA_MAPTYPE_PHONE:
   case TAPI_DATA_MAPTYPE_DEFAULT:
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
   /* connect Dst (ALM or PCM) to SIG, if not already done */
   err =
      Con_ConnectPrepare (&pDev->pSigCh[nThisCh].signal, pDstMod,
                          ALM_TO_SIG_IN);
   /* check if signaling module is already connected. If yes this is a
      conference, so the coder and ALM or PCM are connected */
   if (err == IFX_SUCCESS)
   {
      if (pDev->pSigCh[nThisCh].signal.in[LOCAL_SIG_IN].i != ECMD_IX_EMPTY)
      {
         /* coder signal connection exists already, so connect COD to Dst (ALM
            or PCM) */
         err = Con_ConnectPrepare (pDstMod, &pDev->pCodCh[nThisCh].signal, 0);
      }
      else
      {
         /* connect SIG to Dst (ALM or PCM) */
         err = Con_ConnectPrepare (pDstMod, &pDev->pSigCh[nThisCh].signal, 0);
      }
   }

   if (err == IFX_SUCCESS)
      err = Con_ConnectConfigure (pDev);
   else
   {
      SET_ERROR (ERR_NO_FREE_INPUT_SLOT);
   }

   return err;
}

/*******************************************************************************
Description:
   Removes a data channel from an analog phone device
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   pMap        - handle to TAPI_DATA_MAPPING structure
Return:
   IFX_SUCCESS / IFX_ERROR
Remarks:
   Does not yet support conferencing!
*******************************************************************************/
IFX_int32_t TAPI_LL_Data_Channel_Remove (TAPI_CONNECTION * pChannel,
                                         TAPI_DATA_MAPPING const *pMap)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   IFX_uint32_t nThisCh = pChannel->nChannel;
   int err;
   VINDSP_MODULE *pDstMod = IFX_NULL;

   switch (pMap->nChType)
   {
   case TAPI_DATA_MAPTYPE_CODER:
      SET_ERROR (ERR_NOTSUPPORTED);
      return IFX_ERROR;
   case TAPI_DATA_MAPTYPE_PCM:
      if (pMap->nDstCh < pDev->nPcmCnt)
         pDstMod = &pDev->pPcmCh[pMap->nDstCh].signal;
      break;
   case TAPI_DATA_MAPTYPE_PHONE:
   case TAPI_DATA_MAPTYPE_DEFAULT:
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

   /* disconnect Dst from SIG */
   err =
      Con_DisconnectPrepare (&pDev->pSigCh[nThisCh].signal, pDstMod,
                             ALM_TO_SIG_IN);
   if (pDstMod->nSignal != pDev->pSigCh[nThisCh].signal.in[LOCAL_SIG_IN].i)
   {
      err = Con_DisconnectPrepare (pDstMod, &pDev->pCodCh[nThisCh].signal, 0);
   }
   else
   {
      /* disconnect SIG from Dst */
      err = Con_DisconnectPrepare (pDstMod, &pDev->pSigCh[nThisCh].signal, 0);
      /* now a second module could be still connected to the signaling module
         but its output is directely connected to this coder module. So we now
         map it completely to the signaling module supporting also dial
         detection. This is a workaround because the signaling module only
         supports 2 inputs. */
      if (pDev->pSigCh[nThisCh].signal.in[LOCAL_SIG_IN].i == ECMD_IX_EMPTY &&
          pDev->pSigCh[nThisCh].signal.pInputs2 != IFX_NULL)
      {
         /* input of signaling module is not empty -> module connected and the
            output of the signaling module is still used. Get the module
            connected to it. */
         pDstMod = pDev->pSigCh[nThisCh].signal.pInputs2->pParent;
         /* if the module is NULL it is not ALM or PCM -> error */
         if (pDstMod != IFX_NULL)
         {
            err =
               Con_DisconnectPrepare (pDstMod, &pDev->pCodCh[nThisCh].signal,
                                      0);
            /* connect SIG to Dst (ALM or PCM) */
            err =
               Con_ConnectPrepare (pDstMod, &pDev->pSigCh[nThisCh].signal, 0);
         }
         else
         {
            SET_ERROR (ERR_CON_INVALID);
         }
      }
   }
   if (err == IFX_SUCCESS)
      err = Con_ConnectConfigure (pDev);
   else
   {
      SET_ERROR (ERR_CON_INVALID);
   }
   return err;
}

/*******************************************************************************
Description:
   Adds a data channel to an analog phone device
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   pMap        - handle to TAPI_PCM_MAPPING structure
Return:
   IFX_SUCCESS / IFX_ERROR
Remarks:
   Does not yet support conferencing!
*******************************************************************************/
IFX_int32_t TAPI_LL_PCM_Channel_Add (TAPI_CONNECTION * pChannel,
                                     TAPI_PCM_MAPPING const *pMap)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   IFX_uint32_t nThisCh = pChannel->nChannel;
   IFX_uint32_t nDstCh = pMap->nDstCh;
   int err;

   if ((nDstCh >= pDev->nAnaChan) || (nThisCh >= pDev->nPcmCnt))
   {
      SET_ERROR (ERR_FUNC_PARM);
      return IFX_ERROR;
   }

   /* PCM to ALM connection */
   /* 1. connect PCM out to ALM directely */
   err =
      Con_ConnectPrepare (&pDev->pPcmCh[nThisCh].signal,
                          &pDev->pAlmCh[nDstCh].signal, 0);
   /* 2. connect ALM out to PCM directely */
   err =
      Con_ConnectPrepare (&pDev->pAlmCh[nDstCh].signal,
                          &pDev->pPcmCh[nThisCh].signal, 0);

   if (err == IFX_SUCCESS)
      err = Con_ConnectConfigure (pDev);
   else
   {
      SET_ERROR (ERR_NO_FREE_INPUT_SLOT);
   }
   return err;
}

/*******************************************************************************
Description:
   Removes a data channel from an analog phone device
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   pMap        - handle to TAPI_PCM_MAPPING structure
Return:
   IFX_SUCCESS / IFX_ERROR
Remarks:
   Does not yet support conferencing!
*******************************************************************************/
IFX_int32_t TAPI_LL_PCM_Channel_Remove (TAPI_CONNECTION * pChannel,
                                        TAPI_PCM_MAPPING const *pMap)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   IFX_uint32_t nThisCh = pChannel->nChannel;
   IFX_uint32_t nDstCh = pMap->nDstCh;
   int err;

   if ((nDstCh >= pDev->nAnaChan) || (nThisCh >= pDev->nPcmCnt))
   {
      SET_ERROR (ERR_FUNC_PARM);
      return IFX_ERROR;
   }

   /* 1. disconnect PCM out to ALM directely */
   err =
      Con_DisconnectPrepare (&pDev->pPcmCh[nThisCh].signal,
                             &pDev->pAlmCh[nDstCh].signal, 0);
   /* 2. disconnect ALM out to PCM directely */
   err =
      Con_DisconnectPrepare (&pDev->pAlmCh[nDstCh].signal,
                             &pDev->pPcmCh[nThisCh].signal, 0);
   if (err == IFX_SUCCESS)
      err = Con_ConnectConfigure (pDev);
   else
   {
      SET_ERROR (ERR_CON_INVALID);
   }
   return err;
}
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */

#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
/*******************************************************************************
Description:
   Configures the Datapump for Modulation
Arguments:
   pChannel          -handle to TAPI_CONNECTION structure
   pFaxMod           -handle to TAPI_FAX_MODDATA structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   -  This function has also to map the Fax_Downstream ptr to the write
 interface
   -  The configuration of the Modulator requires that the datapump is disable.
      So the followings are done here :
      1- Disable datapump if enable
      2- Set Modulator
      3- Activate datapump for Modulation and
         set other datapump parameters related to Modulation
      4- Map read interface to Fax_Downstream
*******************************************************************************/
IFX_int32_t TAPI_LL_FaxT38_SetModulator (TAPI_CONNECTION * pChannel,
                                         TAPI_FAX_MODDATA const *pFaxMod)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t ret = IFX_SUCCESS;

   /* configure vinetic for fax */
   ret = FaxSetupConf (pCh);
   /* disable datapump */
   if (ret == IFX_SUCCESS)
      ret = Dsp_SetDatapump (pCh, IFX_FALSE, IFX_FALSE, 0, 0, 0, 0);
   /* set command to configure the modulator */
   if (ret == IFX_SUCCESS)
      ret =
         Dsp_SetDPMod (pCh, pFaxMod->nStandard, pFaxMod->nSigLen, pFaxMod->nDbm,
                       pFaxMod->nTEP, pFaxMod->nTraining);
   /* configure the datapump for modulation */
   if (ret == IFX_SUCCESS)
      ret =
         Dsp_SetDatapump (pCh, IFX_TRUE, IFX_FALSE, pFaxMod->nGainTx,
                          pFaxMod->nMobsm, pFaxMod->nMobrd, 0);
   /* check error */
   if (ret == IFX_SUCCESS)
   {
      /* map write interface for fax support */
      pCh->if_write = Fax_DownStream;
      /* from now on, fax is running */
      pChannel->TapiFaxStatus.nStatus |= TAPI_FAX_T38_TX_ON;
   }
   else
   {
      /* set error and issue tapi exception */
      TAPI_FaxT38_Event_Update (pChannel, 0, TAPI_FAX_T38_DATA_ERR);
   }

   return ret;
}

/*******************************************************************************
Description:
   Configures the Datapump for Demodulation
Arguments:
   pChannel          -handle to TAPI_CONNECTION structure
   pFaxDemod         -handle to TAPI_FAX_DEMODDATA structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   -  This function has also to map the Fax_UpStream ptr to the read interface
      and reset the data fifo
   -  The configuration of the Demodulator requires that the datapump is
 disable.
      So the followings are done here :
         1- Disable datapump if enable
         2- Set Demodulator
         3- Activate datapump for Demodulation and
            set other datapump parameters related to Demodulation
         4- Map read interface to Fax_UpStream
*******************************************************************************/
IFX_int32_t TAPI_LL_FaxT38_SetDemodulator (TAPI_CONNECTION * pChannel,
                                           TAPI_FAX_DEMODDATA const *pFaxDemod)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t ret = IFX_SUCCESS;

   /* check fax state */
   ret = FaxSetupConf (pCh);
   /* Disable Channel Fax datapump */
   if (ret == IFX_SUCCESS)
      ret = Dsp_SetDatapump (pCh, IFX_FALSE, IFX_FALSE, 0, 0, 0, 0);
   /* set command to configure the demodulator */
   if (ret == IFX_SUCCESS)
      ret =
         Dsp_SetDPDemod (pCh, pFaxDemod->nStandard1, pFaxDemod->nStandard2,
                         pFaxDemod->nEqualizer, pFaxDemod->nTraining);
   /* check error */
   if (ret == IFX_SUCCESS)
   {
      /* Clear fifo for this channel to record valid fax data */
      Vinetic_IrqLockDevice (pDev);
      Fifo_Clear (&pCh->rdFifo);
      /* map write interface for fax support */
      pCh->if_read = Fax_UpStream;
      /* from now on, fax is running */
      pChannel->TapiFaxStatus.nStatus |= TAPI_FAX_T38_TX_ON;
      Vinetic_IrqUnlockDevice (pDev);
   }
   /* configure the datapump for demodulation */
   if (ret == IFX_SUCCESS)
      ret = Dsp_SetDatapump (pCh, IFX_TRUE, IFX_TRUE, pFaxDemod->nGainRx,
                             0, 0, pFaxDemod->nDmbsd);
   else
   {
      ret = Dsp_SetDatapump (pCh, IFX_FALSE, IFX_FALSE, 0, 0, 0, 0);
      pCh->if_read   = VoIP_UpStream;
      pChannel->TapiFaxStatus.nStatus &= ~TAPI_FAX_T38_TX_ON;
      /* set error and issue tapi exception */
      TAPI_FaxT38_Event_Update (pChannel, 0, TAPI_FAX_T38_DATA_ERR);
   }

   return ret;
}

/*******************************************************************************
Description:
   disables the Fax datapump
Arguments:
   pChannel          -handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   This function has to remap the VoIp_UpStream and VoIP_Downstream to
   the driver read and write interfaces
*******************************************************************************/
IFX_int32_t TAPI_LL_FaxT38_DisableDataPump (TAPI_CONNECTION * pChannel)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t ret = IFX_SUCCESS;

   /* Disable Channel Fax datapump */
   ret = Dsp_SetDatapump (pCh, IFX_FALSE, IFX_FALSE, 0, 0, 0, 0);
   /* switch on signalling interface modules */
   if (ret == IFX_SUCCESS)
      ret = Dsp_SwitchSignModules (pCh, IFX_TRUE);
   /* Enable NLP */
   if (ret == IFX_SUCCESS)
      /** \todo restore state as before fax transmission? */
      ret = Dsp_SetNeLec_Alm (pCh, IFX_TRUE, IFX_TRUE);
   /* activate voice path and make driver ready for a next fax connection */
   if (ret == IFX_SUCCESS)
   {
      /* remap read/write interfaces */
      pCh->if_write = VoIP_DownStream;
      pCh->if_read = VoIP_UpStream;
   }
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   /* Enable back Coder Channel if */
   if ((ret == IFX_SUCCESS) && (pCh->bVoiceConnect == IFX_TRUE))
   {
      ret = TAPI_LL_Phone_Start_Recording (pChannel);
      if (ret == IFX_SUCCESS)
         ret = TAPI_LL_Phone_Start_Playing (pChannel);
   }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
   if (ret == IFX_ERROR)
   {
      /* set error and issue tapi exception */
      TAPI_FaxT38_Event_Update (pChannel, 0, TAPI_FAX_T38_ERR);
      TRACE (TAPI_DRV, DBG_LEVEL_HIGH,
             ("DRV_ERR: Disable Datapump" " failed\n\r"));
   }

   return ret;
}

/*******************************************************************************
Description:
   query Fax Status
Arguments:
   pChannel          -handle to TAPI_CONNECTION structure
   pFaxStatus        -handle to TAPI_FAX_STATUS structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
*******************************************************************************/
IFX_int32_t TAPI_LL_FaxT38_GetStatus (TAPI_CONNECTION * pChannel,
                                      TAPI_FAX_STATUS * pFaxStatus)
{
   /* read status from cache structure */
   *pFaxStatus = pChannel->TapiFaxStatus;

   return IFX_SUCCESS;
}
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38) */

/*******************************************************************************
Description:
   returns Vinetic's number of capability lists
Arguments:
Return:
   Vinetic's entry size of VINETIC_CAP_Table
Remarks:
   VINETIC_CAP_Table can be updated without any changes in TAPI functions
   concerning capabilities' services.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Get_Capabilities (TAPI_CONNECTION * pChannel)
{
   return ((VINETIC_DEVICE *) pChannel->pDevice)->nMaxCaps;
}

/*******************************************************************************
Description:
   returns Vinetic's capability lists
Arguments:
   pCapList        -handle to TAPI_PHONE_CAPABILITY structure
Return:
   IFX_SUCCESS
Remarks:
   The whole set of VINETIC capabilities is copied to the passed pointer
   argument.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Get_Capability_List (TAPI_CONNECTION * pChannel,
                                               TAPI_PHONE_CAPABILITY * pCapList)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;

   memcpy (pCapList, pDev->CapList,
           ((IFX_uint32_t) pDev->nMaxCaps * sizeof (TAPI_PHONE_CAPABILITY)));
   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   checks if a specific capability is supported from VINETIC
Arguments:
   pCapList        -handle to TAPI_PHONE_CAPABILITY structure
Return:
   0 if not supported, 1 if supported
Remarks:
   This function compares only the captype and the cap members of the given
   TAPI_PHONE_CAPABILITY structure with the ones of the VINETIC_CAP_Table.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Check_Capability (TAPI_CONNECTION * pChannel,
                                            TAPI_PHONE_CAPABILITY * pCapList)
{
   IFX_int32_t cnt;
   IFX_int32_t ret = 0;
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;

   /* do checks */
   for (cnt = 0; cnt < pDev->nMaxCaps; cnt++)
   {
      if (pCapList->captype == pDev->CapList[cnt].captype)
      {
         switch (pCapList->captype)
         {
            /* handle number counters, cap is returned */
         case pcm:
         case codecs:
         case phones:
         case t38:
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

/*******************************************************************************
Description:
   sets the frame length for the audio packets
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   nFrameLength    - length of frames in milliseconds
Return:
   IFX_SUCCESS/IFX_ERROR
Remarks:
   This function sets the frame length only when the requested value
   is valid in relation to the pre-existing Codec value of the Coder Channel
   configuration. Coder channel module remains either disabled or enabled
   after the completion of this function, depending on its previous state.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Set_Frame_Length (TAPI_CONNECTION * pChannel,
                                            IFX_int32_t nFrameLength)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   FWM_COD_CH *pCodCh = &pDev->pCodCh[pChannel->nChannel].cod_ch;
   IFX_int32_t ret = IFX_SUCCESS, nPte = 0;

   /* check coder module */
   if (pDev->nDevState & DS_COD_EN)
      /* set packet time according to frame length */
      nPte = getPTE (nFrameLength);
   else
   {
      ret = IFX_ERROR;
      SET_ERROR (ERR_CODCONF_NOTVALID);
   }

   /* check if PTE and pre existing ENC match together */
   if ((ret == IFX_SUCCESS) && (nPte != IFX_ERROR))
      ret = CheckENCnPTE ((IFX_uint8_t) nPte, (IFX_uint8_t) pCodCh->bit.enc);
   else
      ret = IFX_ERROR;
   /* set the requested PTE value */
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

/*********************************************************************************
Description:
   Retrieves the saved frame length for the audio packets, as saved when applied
   to the channel
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pFrameLength    - handle to length of frames in milliseconds
Return:
   IFX_SUCCESS/IFX_ERROR
Remarks:
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Get_Frame_Length (TAPI_CONNECTION * pChannel,
                                            IFX_int32_t * pFrameLength)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   FWM_COD_CH *pCodCh = &pDev->pCodCh[pChannel->nChannel].cod_ch;
   IFX_int32_t nFrameLength;

   /* set frame length according to stored PTE */
   nFrameLength = getFrameLength (pCodCh->bit.pte);
   if (nFrameLength == IFX_ERROR)
   {
      LOG (TAPI_DRV, DBG_LEVEL_HIGH,
           ("\n\rDRV_ERROR: PTE value has no corresponding frame length.\n\r"));
      return IFX_ERROR;
   }
   *pFrameLength = nFrameLength;
   return IFX_SUCCESS;
#else
   return IFX_ERROR;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
}

/*******************************************************************************
Description:
   Controls the DTMF sending mode. The DTMF may be sent out-of-band (OOB)
   or in-band.
Arguments:
   pChannel         - handle to TAPI_CONNECTION structure
   nOobMode         - mode of the inband and out of band transmission of
                     RFC2883 event packets
                     - 0: TAPI_OOB_INOUT, DTMF is send in-band and out-of-band
                     - 1: TAPI_OOB_OUT, DTMF is send only out-of band
                     - 2: TAPI_OOB_IN, DTMF is send only in-band
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   This function sets the AS bit (Auto Suppression) in DTMF Receiver's
   configuration when the DTMF sending only out-of-band is requested. In the
   other case, AS bit is set to zero. In both cases ET bit (Event Transmission
   Support) is set. In-band DTMF sending mode is only valid for G.711 and G.726
   recording codecs. This function works only when the Coder, Signalling and
   ALI modules have been configured.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Send_DTMF_OOB (TAPI_CONNECTION * pChannel,
                                         IFX_int32_t nOobMode)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   IFX_uint8_t ch = pChannel->nChannel;
   IFX_uint16_t pCmd[2] = { 0 };
   IFX_uint16_t pData[4] = { 0 };
   IFX_int32_t ret = IFX_SUCCESS;

   /* Read Signaling Module status */
   if (!(pDev->nDevState & DS_SIG_EN))
   {
      SET_ERROR (ERR_SIGMOD_NOTEN);
      return IFX_ERROR;
   }
   /* set DTMF Receiver read command */
   pCmd[0] = CMD1_EOP | ch;
   pCmd[1] = ECMD_DTMF_REC;
   /* read DTMF Receiver current configuration */
   ret = CmdRead (pDev, pCmd, pData, 1);
   if (ret != IFX_SUCCESS)
   {
      return IFX_ERROR;
   }
   /* store the previous configuration of the DTMF receiver */
   pData[0] = CMD1_EOP | ch;
   pData[1] = ECMD_DTMF_REC;
   pData[2] &= ~(SIG_DTMFREC_ET | SIG_DTMFREC_AS);

   if (nOobMode != TAPI_OOB_IN && pChannel->nEvtPT == 0)
   {
      SET_ERROR (ERR_WRONG_EVPT);
      return IFX_ERROR;
   }

   switch (nOobMode)
   {
   case TAPI_OOB_INOUT:
      /* enable transmission */
      pData[2] |= SIG_DTMFREC_ET;
      break;
   case TAPI_OOB_OUT:
      /* enable transmission and suppress in band signals */
      pData[2] |= SIG_DTMFREC_ET | SIG_DTMFREC_AS;
      break;
   case TAPI_OOB_IN:
      /* disable transmission and enable in band signals */
      break;
   default:
      SET_ERROR (ERR_FUNC_PARM);
      break;
   }
   /* enable the DTMF Receiver and set the requested DTMF mode */
   ret = CmdWrite (pDev, pData, 1);

   return IFX_SUCCESS;
#else
   return IFX_ERROR;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
}

/*********************************************************************************
Description:
   sets the Voice Activation Detection mode.
   Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   nVAD            - IFX_TRUE: VAD switched on
                     IFX_FALSE: VAD is off
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   This function sets the SC bit (Silence Compression) of Coder Channel's
   cofiguration if VAD is on. Otherwise, it sets it to zero. Recording Codec
   G.729E (11.8kbps) does not support VAD.

   in Case of VAD with comfort noise generation, CNG bit will be set only if
   Codec is G711.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_VAD (TAPI_CONNECTION * pChannel, IFX_int32_t nVAD)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   FWM_COD_CH *pCodCh = &pDev->pCodCh[pChannel->nChannel].cod_ch;
   IFX_int32_t ret = IFX_SUCCESS;

   /* first check if Coder Control Module is enabled */
   if (!(pDev->nDevState & DS_COD_EN))
   {
      ret = IFX_ERROR;
      SET_ERROR (ERR_CODCONF_NOTVALID);
   }

   /* protect fw messages */
   IFXOS_MutexLock (pDev->memberAcc);
   /* do appropriate VAD setting */
   if (ret == IFX_SUCCESS)
   {
      /* set Coder Channel write command */

      switch (nVAD)
      {
      case TAPI_VAD_NOVAD:
         /* deactive silence compression */
         pCodCh->bit.sc = 0;
         pCodCh->bit.cng = 0;
         break;
      default:
         /* activate silence compression */
         pCodCh->bit.sc = 1;
         /* handle according to actual codec */
         switch (pCodCh->bit.enc)
         {
         case COD_CH_G729_E_ENC:
            /* nCodec G.729E (11.8kbps) does not support voice activity
               detection */
            pCodCh->bit.sc = 0;
            pCodCh->bit.cng = 0;
            break;
         case COD_CH_G711_ALAW_ENC:
         case COD_CH_G711_MLAW_ENC:
            /* activate comfort noise generation also if G711 */
            pCodCh->bit.cng = 1;
            if (nVAD == TAPI_VAD_G711)
               pCodCh->bit.ns = 0;
            else
               pCodCh->bit.ns = 1;
            break;
         default:
            pCodCh->bit.cng = 0;
            break;
         }
         break;
      }
   }
   /* set the requested VAD mode */
   if (ret == IFX_SUCCESS)
      ret = CmdWrite (pDev, pCodCh->value, 2);
   /* release locks */
   IFXOS_MutexUnlock (pDev->memberAcc);

   return ret;
#else
   return IFX_ERROR;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
}

/*********************************************************************************
Description:
   sets the LEC configuration on the PCM mode.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pLecConf        - handle to TAPI_LECCONF structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   This function shouldn't be called when a disabling of ALM Nelec can have a
   fatal impact on the ongoing voice/fax connection. It will be more suitable to
   call the function at initialization / configuration time.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_LecConf (TAPI_CONNECTION * pChannel,
                                   TAPI_LECCONF const *pLecConf)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t ret = IFX_SUCCESS;
   IFX_boolean_t bEnLec;
   IFX_boolean_t bEnNlp;

   /* enable LEC if gains are not TAPI_LEC_GAIN_OFF */
   bEnLec = (pLecConf->nGainIn != TAPI_LEC_GAIN_OFF &&
             pLecConf->nGainOut != TAPI_LEC_GAIN_OFF) ? IFX_TRUE : IFX_FALSE;
   bEnNlp = (pLecConf->bNlp == TAPI_LEC_NLP_OFF) ? IFX_FALSE : IFX_TRUE;

   ret = Dsp_SetNeLec_Alm (pCh, bEnLec, bEnNlp);

   return ret;
#else
   return IFX_ERROR;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
}

/*********************************************************************************
Description:
   sets the LEC configuration on the PCM.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pLecConf        - handle to TAPI_LECCONF structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   This function shouldn't be called when a disabling of PCM Nelec can have a
   fatal impact on the ongoing voice/fax connection. It will be more suitable to
   call the function at initialization / configuration time.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_LecConf_Pcm (TAPI_CONNECTION * pChannel,
                                       TAPI_LECCONF const *pLecConf)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t ret = IFX_SUCCESS;
   IFX_boolean_t bEnLec;
   IFX_boolean_t bEnNlp;

   /* enable LEC if gains are not TAPI_LEC_GAIN_OFF */
   bEnLec = (pLecConf->nGainIn != TAPI_LEC_GAIN_OFF &&
             pLecConf->nGainOut != TAPI_LEC_GAIN_OFF) ? IFX_TRUE : IFX_FALSE;
   bEnNlp = (pLecConf->bNlp == TAPI_LEC_NLP_OFF) ? IFX_FALSE : IFX_TRUE;

   ret = Dsp_SetNeLec_Pcm (pCh, bEnLec, bEnNlp);

   return ret;
#else
   return IFX_ERROR;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
}

/******************************************************************************/
/**
   TAPI_LL_Phone_AgcEnable
      Enable/Disable the AGC resoucre inside the VINETIC device. This
      implementation assumes that the index of the AGC resource is fixed
      assigned to the related index of the Coder-Module.
      If the AGC resource is enable or disable, it depends on 'Param'.
   \param
     - pChannel:  handle to TAPI_CONNECTION structure
     - Param:     TAPI_AGC_MODE enumeration
   \return
      - IFX_ERROR if error
      - IFX_SUCCESS if successful
   \remarks
*******************************************************************************/
IFX_return_t TAPI_LL_Phone_AgcEnable (TAPI_CONNECTION * pChannel,
                                      TAPI_AGC_MODE Param)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   /* VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel]; */
   IFX_uint16_t pData[3];

   pData[0] = CMD1_EOP | pChannel->nChannel;
   pData[1] = ECMD_COD_AGC;

   pData[2] = (Param ? 0x8000 : 0) | pChannel->nChannel;

   if (CmdWrite (pDev, pData, 1) != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;
#else
   return IFX_ERROR;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
}

/*********************************************************************************
Description:
   Set the phone volume
Arguments:
   pChannel - handle to TAPI_CONNECTION structure
   pVol     - handle to TAPI_PHONE_VOLUME structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   Gain Parameter are given in 'dB'. The range is -24dB ... 24dB.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Volume (TAPI_CONNECTION * pChannel,
                                  TAPI_PHONE_VOLUME const *pVol)
{
   IFX_int32_t ret;
   IFX_uint8_t ch = pChannel->nChannel;
   IFX_boolean_t bALM_mute = IFX_FALSE;
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;

   /* range check, cos gain var is integer */
   if ((pVol->nGainTx < (TAPI_PHONE_VOLCONF_LOW)) ||
       (pVol->nGainTx > TAPI_PHONE_VOLCONF_HIGH) ||
       (pVol->nGainRx < (TAPI_PHONE_VOLCONF_LOW)) ||
       (pVol->nGainRx > TAPI_PHONE_VOLCONF_HIGH))
   {
      /* parameter are out of supported range */

      if ((pVol->nGainRx == TAPI_PHONE_VOLCONF_OFF) &&
          (pVol->nGainTx == TAPI_PHONE_VOLCONF_OFF))
      {
         /* special case: mute the interface with both parameter set to
            TAPI_PHONE_VOLCONF_OFF */
         bALM_mute = IFX_TRUE;
      }
      else
      {
         LOG (TAPI_DRV, DBG_LEVEL_HIGH,
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
         (IFX_uint32_t) VINETIC_AlmPcmGain[pVol->nGainTx + 24];
      pDev->pAlmCh[ch].ali_ch.bit.gain_r =
         (IFX_uint32_t) VINETIC_AlmPcmGain[pVol->nGainRx + 24];
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

/*********************************************************************************
Description:
   Sets the PCM interface volume.
Arguments:
   pChannel - handle to TAPI_CONNECTION structure
   pVol     - handle to TAPI_PHONE_VOLUME structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   Gain Parameter are given in 'dB'. The range is -24dB ... 24dB.
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Pcm_Volume (TAPI_CONNECTION * pChannel,
                                      TAPI_PHONE_VOLUME const *pVol)
{
   IFX_int32_t ret;
   IFX_boolean_t bPCM_mute = IFX_FALSE;
   IFX_uint8_t ch = pChannel->nChannel;
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;

   /* range check, cos gain var is integer */
   if ((pVol->nGainTx < (TAPI_PHONE_VOLCONF_LOW)) ||
       (pVol->nGainTx > TAPI_PHONE_VOLCONF_HIGH) ||
       (pVol->nGainRx < (TAPI_PHONE_VOLCONF_LOW)) ||
       (pVol->nGainRx > TAPI_PHONE_VOLCONF_HIGH))
   {
      /* parameter are out of supported range */

      if ((pVol->nGainRx == TAPI_PHONE_VOLCONF_OFF) &&
          (pVol->nGainTx == TAPI_PHONE_VOLCONF_OFF))
      {
         /* special case: mute the interface with both parameter set to
            TAPI_PHONE_VOLCONF_OFF */
         bPCM_mute = IFX_TRUE;
      }
      else
      {
         LOG (TAPI_DRV, DBG_LEVEL_HIGH,
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
         (IFX_uint32_t) VINETIC_AlmPcmGain[pVol->nGainTx + 24];
      pDev->pPcmCh[ch].pcm_ch.bit.gain_2 =
         (IFX_uint32_t) VINETIC_AlmPcmGain[pVol->nGainRx + 24];
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

/*********************************************************************************
Description:
   This service enables or disables a high level path of a phone channel.
Arguments:
   pChannel - handle to TAPI_CONNECTION structure
   bEnable  - The parameter represent a boolean value of \ref TAPI_HIGH_LEVEL.
               - 0: TAPI_HIGH_LEVEL_DISABLE, disable the high level path.
               - 1: TAPI_HIGH_LEVEL_ENABLE, enable the high level path.
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   This service is intended for phone channels only and must be used in
   combination with IFXPHONE_VOLUME or IFXPHONE_PCM_VOLUME to set the max. level
   (TAPI_PHONE_VOLCONF_HIGH) or to restore level
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_High_Level (TAPI_CONNECTION * pChannel,
                                      IFX_int32_t bEnable)
{
   IFX_int32_t err = IFX_SUCCESS;
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t ch = (pCh->nChannel - 1);

   if (ch >= pDev->nAnaChan)
   {
      /* not an analog channel */
      SET_ERROR (ERR_FUNC_PARM);
      return IFX_ERROR;
   }

   if (bEnable == TAPI_HIGH_LEVEL_ENABLE)
   {
      /* store current register values */
      err = storeRegVal (pCh, BCR1_REG);
      if (err == IFX_ERROR)
         return IFX_ERROR;
      err = storeRegVal (pCh, BCR2_REG);
      if (err == IFX_ERROR)
         return IFX_ERROR;
      err = storeRegVal (pCh, TSTR2_REG);
      if (err == IFX_ERROR)
         return IFX_ERROR;
      /* change bits */
      VIN_CH_BCR1_TEST_EN_SET (BCR1.value, 1);
      VIN_CH_BCR2_TTX_EN_SET (BCR2.value, 0);
      VIN_CH_BCR2_IM_DIS_SET (BCR2.value, 1);
      VIN_CH_BCR2_AR_DIS_SET (BCR2.value, 1);
      VIN_CH_BCR2_FRR_DIS_SET (BCR2.value, 1);
      VIN_CH_TSTR2_OPIM_AN_SET (TSTR2.value, 1);
      VIN_CH_TSTR2_AC_RGAIN_SET (TSTR2.value, 1);
      if (pDev->nChipMajorRev == VINETIC_V1x)
      {
         VIN_CH_TSTR2_OPIM_HW_SET (TSTR2.value, 1);
      }
      /* write changed registers */
      err = wrReg (pCh, BCR1_REG);
      if (err == IFX_ERROR)
         return IFX_ERROR;
      err = wrReg (pCh, BCR2_REG);
      if (err == IFX_ERROR)
         return IFX_ERROR;
      err = wrReg (pCh, TSTR2_REG);
      if (err == IFX_ERROR)
         return IFX_ERROR;
   }
   else
   {
      /* restore register values */
      err = restoreRegVal (pCh, BCR1_REG);
      if (err == IFX_ERROR)
         return IFX_ERROR;
      err = restoreRegVal (pCh, BCR2_REG);
      if (err == IFX_ERROR)
         return IFX_ERROR;
      err = restoreRegVal (pCh, TSTR2_REG);
      if (err == IFX_ERROR)
         return IFX_ERROR;
   }
   return err;
}

/*******************************************************************************
Description:
   Enables signal detection
Arguments:
   pChannel - handle to TAPI_CONNECTION structure
   pSig     - handle to TAPI_PHONE_SIGDETECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   none
*******************************************************************************/
IFX_int32_t TAPI_LL_Enable_Signal (TAPI_CONNECTION * pChannel,
                                   TAPI_PHONE_SIGDETECTION const *pSig)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t err = IFX_SUCCESS;

   if (pDev->nChipType == VINETIC_TYPE_S)
   {
      SET_ERROR (ERR_NOTSUPPORTED);
      return IFX_ERROR;
   }
   IFXOS_MutexLock (pDev->memberAcc);
   /* CED detection */
   if (pSig->sig &
             (TAPI_SIGNAL_CEDMASK | TAPI_SIGNAL_CEDENDMASK |
              TAPI_SIGNAL_PHASEREVMASK | TAPI_SIGNAL_AMMASK |
              TAPI_SIGNAL_TONEHOLDING_ENDMASK | TAPI_SIGNAL_DISMASK))
   {
      /* ATD1 configured for detection of CED */
      err = Dsp_AtdSigEnable (pCh, pSig->sig);
   }
   if ((err == IFX_SUCCESS) &&
       (pSig->sig & (TAPI_SIGNAL_CNGFAXMASK | TAPI_SIGNAL_CNGMODMASK)))
   {
      /* UTD1 configured for CNG (1100 Hz) detection */
      /* UTD2 configured for CNG (1300 Hz) detection */
      err = Dsp_UtdSigEnable (pCh, pSig->sig);
   }

   if ((err == IFX_SUCCESS) && (pSig->sig & TAPI_SIGNAL_CIDENDTX))
   {
      /* enable the Caller-ID transmission event */
      pChannel->TapiMiscData.sigMask |= TAPI_SIGNAL_CIDENDTX;
   }

   IFXOS_MutexUnlock (pDev->memberAcc);
   return err;
#else
   return IFX_ERROR;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
}

/*******************************************************************************
Description:
   Enables signal detection
Arguments:
   pChannel - handle to TAPI_CONNECTION structure
   pSig     - handle to TAPI_PHONE_SIGDETECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   none
*******************************************************************************/
IFX_int32_t TAPI_LL_Disable_Signal (TAPI_CONNECTION * pChannel,
                                    TAPI_PHONE_SIGDETECTION const *pSig)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t err = IFX_SUCCESS;

   IFXOS_MutexLock (pDev->memberAcc);

   pChannel->TapiMiscData.sigMask &= ~(pSig->sig);

   if (pSig->sig &
             (TAPI_SIGNAL_CEDMASK | TAPI_SIGNAL_CEDENDMASK |
              TAPI_SIGNAL_PHASEREVMASK | TAPI_SIGNAL_AMMASK |
              TAPI_SIGNAL_TONEHOLDING_ENDMASK | TAPI_SIGNAL_DISMASK))
   {
      err = Dsp_AtdConf (pCh, pChannel->TapiMiscData.sigMask);
   }

   if (pSig->sig & (TAPI_SIGNAL_CNGFAXMASK | TAPI_SIGNAL_CNGMODMASK))
   {
      /* UTD1 configured for CNG (1100 Hz) detection */
      /* UTD2 configured for CNG (1300 Hz) detection */
      Dsp_UtdConf (pCh,
                   pChannel->TapiMiscData.
                   sigMask & (TAPI_SIGNAL_CNGFAXMASK | TAPI_SIGNAL_CNGMODMASK));
   }

   IFXOS_MutexUnlock (pDev->memberAcc);

   return err;
#else
   return IFX_ERROR;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
}

/*********************************************************************************
Description:
   Enables signal detection
Arguments:
   pChannel - handle to TAPI_CONNECTION structure
   pTone    - handle to the simple tone structure
   signal   - the type of signal and direction to detect
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   none
*******************************************************************************/
IFX_int32_t TAPI_LL_CPTD_Start (TAPI_CONNECTION * pChannel,
                                TAPI_SIMPLE_TONE const *pTone,
                                IFX_int32_t signal)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t err = IFX_SUCCESS;
   IFX_uint8_t ch = pCh->nChannel - 1;
   IFX_uint16_t pCmd[CMD_SIG_CPTCOEFF_LEN + CMD_HEADER_CNT];

   if (pDev->nChipType == VINETIC_TYPE_S)
   {
      SET_ERROR (ERR_NOTSUPPORTED);
      return IFX_ERROR;
   }

   memset (pCmd, 0, (CMD_SIG_CPTCOEFF_LEN + CMD_HEADER_CNT) * 2);
   pCmd[0] = CMD1_EOP | ch;
   pCmd[1] = ECMD_CPT_COEF;
   Dsp_Cptd_SetCoeff (pCh, pTone, pCmd);
   /* write CPTD coefficients */
   err = CmdWrite (pDev, pCmd, CMD_SIG_CPTCOEFF_LEN);
   /* set up cpt command, pCmd[2] will be cleared in Dsp_Cptd_Conf */
   pCmd[1] = ECMD_CPT;
   if (err == IFX_SUCCESS)
      err = Dsp_Cptd_Conf (pCh, pTone, signal, pCmd);
   if (err == IFX_SUCCESS)
   {
      /* unmask setting for interrupt 0 -> 1 */
      pDev->regMRSRE1[ch] &= ~SRE1_CPT;
      /* write CPTD configuration */
      err = CmdWrite (pDev, pCmd, CMD_SIG_CPT_LEN);
      /* write interrupt masks */
      if (err == IFX_SUCCESS)
         err = RegWrite (pDev, CMD1_IOP | ch, &pDev->regMRSRE1[ch], 1, MRSRE1);
   }
   return err;
#else
   return IFX_ERROR;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
}

/*********************************************************************************
Description:
   Enables signal detection
Arguments:
   pChannel - handle to TAPI_CONNECTION structure
   pSig     - handle to TAPI_PHONE_SIGDETECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   none
*******************************************************************************/
IFX_int32_t TAPI_LL_CPTD_Stop (TAPI_CONNECTION * pChannel)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t err = IFX_SUCCESS;
   IFX_uint8_t ch = pCh->nChannel - 1;
   IFX_uint16_t pCmd[3];

   /* mask setting for interrupt 0 -> 1 */
   pDev->regMRSRE1[ch] |= SRE1_CPT;
   /* write interrupt masks */
   err = RegWrite (pDev, CMD1_IOP | ch, &pDev->regMRSRE1[ch], 1, MRSRE1);
   /* write CPTD configuration */
   pCmd[0] = CMD1_EOP | ch;
   pCmd[1] = ECMD_CPT;
   pCmd[2] = 0;
   /* set resource */
   VIN_ECMD_SIGCPT_CPTNR_SET (pCmd[2], ch);
   if (err == IFX_SUCCESS)
      err = CmdWrite (pDev, pCmd, CMD_SIG_CPT_LEN);
   return err;
}

/*********************************************************************************
Description:
   Restores the line state back after fault
Arguments:
   pChannel - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   On fault the line state is stored and with this function reapplied again
*******************************************************************************/
IFX_int32_t TAPI_LL_Restore_FaultLine (TAPI_CONNECTION * pChannel)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) pChannel->pDevice;
   VINETIC_CHANNEL *pCh = &pDev->pChannel[pChannel->nChannel];
   return ScWrite (pDev, OPMOD_CUR.value | CMD1_SC | (pCh->nChannel - 1));
}
#endif /* TAPI */
