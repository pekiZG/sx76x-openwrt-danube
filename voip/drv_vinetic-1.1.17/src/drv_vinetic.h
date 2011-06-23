#ifndef _DRV_VINETIC_H
#define _DRV_VINETIC_H
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
   Module      : drv_vinetic.h
   Description :
      This file contains VINETIC specific defines and firmware messages.
      Device specific defines for the host interface is included.
*******************************************************************************/

/** \file
   This file contains the defines specific to the VINETIC
 */

#include "ifx_types.h"        /* for IFX data types */
#include "drv_vinetic_host.h" /* depending on chip type */

/* ============================= */
/* Global Defines                */
/* ============================= */

/* header words amount */
#define CMD_HEADER_CNT 2
/* Maximal Size of PHI Patch */
#define MAX_PHI_WORDS               4096 /* 8 KB = 4 KWORDS */
/* define signal array inputs */
#define MAX_MODULE_SIGNAL_INPUTS 5


/* ============================= */
/* Global Mailbox Defines        */
/* ============================= */

/* defines for command word 1 */
/* read write mask */
#define CMD1_RW  0x8000
#define CMD1_WR  0x0000
#define CMD1_RD  0x8000

#define CMD1_SC  0x4000
#define CMD1_BC  0x2000
#define CMD1_OM  0x1000
#define CMD1_CMD 0x1F00
#define CMD1_SUB 0x00F0
#define CMD1_CH  0x000F
/* command word 1 command fields */
#define CMD1_SOP 0x0100
#define CMD1_COP 0x0200
#define CMD1_IOP 0x0300
#define CMD1_VOP 0x0400
#define CMD1_EVT 0x0500
#define CMD1_CIDRX 0x1400
#define CMD1_EOP 0x0600
#define CMD1_DIOP 0x0800
#define CMD1_FRD  0x1000
#define CMD1_FRS  0x1100
#define CMD1_AOP  0x0900
#define CMD1_AOPW 0x0A00
/* defines for command word 2 */
#define CMD2_LEN 0x00ff
#define CMD2_OFFSET 0xff00
/* odd field for EVT , VOP and CIDRX */
#define CMD2_ODD 0x2000

/* defines for module ids for EOP commands */
#define CMD2_ECMD 0x1f00
#define CMD2_MOD  0xE000
#define CMD2_MOD_PCM  0x0000
#define CMD2_MOD_ALM  0x2000
#define CMD2_MOD_SIG  0x4000
#define CMD2_MOD_COD  0x6000
#define CMD2_MOD_CTL  0xA000
#define CMD2_MOD_RES  0xC000
#define CMD2_MOD_TST  0xE000

/* ============================= */
/* EDSP (firmware) Commands      */
/* ============================= */
/* pcm interface control */
#define ECMD_PCM_CTL          (CMD2_MOD_PCM | 0x0000)
#define PCM_CTL_EN            0x8000
#define ECMD_PCM_CH           (CMD2_MOD_PCM | 0x0100)
#define PCM_CH_EN             0x8000
#define PCM_CH_COD            0x7000
#define PCM_CH_XTS            0x7F00
#define PCM_CH_RTS            0x007F
#define PCM_CH_RHW            0x0080
#define PCM_CH_XHW            0x8000
#define PCM_GAIN_0DB          0x60
#define ECMD_PCM_NEARLEC      (CMD2_MOD_PCM | 0x0200)
#define PCM_NLEC_EN           0x8000
#define PCM_NLP_EN            0x0040
#define PCM_OLDP              0x1000    /* Note, available for WLEC only! */
#define PCM_MW_EN             0x0800    /* Note, available for WLEC only! */
#define PCM_DCF_EN            0x0400    /* Note, available for WLEC only! */
#define PCM_OLDC_EN           0x0100
#define PCM_LECNR             0x000F
#define ECMD_PCM_FARLEC       (CMD2_MOD_PCM | 0x0300)

/* analog line interface */
#define ECMD_ALM_CTL          (CMD2_MOD_ALM | 0x0000)
#define ALM_CTL_EN            0x8000
#define ECMD_ALM_CH           (CMD2_MOD_ALM | 0x0100)
#define ALM_CH_EN             0x8000
#define ALM_GAINX_0DB         0x6000
#define ALM_GAINR_0DB         0x0060
#define ALM_GAIN_0DB          0x60
#define ECMD_ALM_NEARLEC      (CMD2_MOD_ALM | 0x0200)
#define ALM_NLEC_EN           0x8000
#define ALM_NLP_EN            0x0040
#define ALM_OLDP              0x1000    /* Note, available for WLEC only! */
#define ALM_MW_EN             0x0800    /* Note, available for WLEC only! */
#define ALM_DCF_EN            0x0400    /* Note, available for WLEC only! */
#define ALM_OLDC_EN           0x0100
#define ALM_LECNR             0x000F
#define ECMD_ALM_FARLEC       (CMD2_MOD_ALM | 0x0300)

/* signaling module */
#define ECMD_SIG_CTL          (CMD2_MOD_SIG | 0x0000)
#define SIG_CTL_EN            0x8000
#define ECMD_SIG_CH           (CMD2_MOD_SIG | 0x0100)
#define SIG_CH_EN             0x8000
#define ECMD_SIG_CH_RTP       (CMD2_MOD_SIG | 0x1000)
#define SIG_RTP_EPOU_DTMF     0x0100
#define SIG_RTP_EPOU_ANS      0x0200
#define SIG_RTP_EPOU_CNG      0x0400
#define SIG_RTP_VBLOCK        0x8000
#define SIG_RTP_EVTRIG_DTMF   0x0001
#define SIG_RTP_EVTRIG_ANS    0x0002
#define SIG_RTP_EVTRIG_NANS   0x0004
#define SIG_RTP_EVTRIG_ANSAM  0x0008
#define SIG_RTP_EVTRIG_NANSAM 0x0010
#define SIG_RTP_EVTRIG_CNG    0x0020
#define SIG_RTP_EVTRIG_DIS    0x0040

#define ECMD_CID_SEND         (CMD2_MOD_SIG | 0x0200)
#define SIG_CID_EN            0x8000
#define SIG_CID_AD            0x2000
#define SIG_CID_HLEV          0x1000
#define SIG_CID_V23           0x0800
#define SIG_CID_A1            0x00C0
#define SIG_CID_A2            0x0030
#define SIG_CID_A2_VOICE      0x0010
#define SIG_CID_A2_NOVOICE    0x0020
#define SIG_CID_A1_VOICE      0x0040
#define SIG_CID_A1_NOVOICE    0x0080
#define SIG_CID_CIDNR         0x000F
#define ECMD_DTMF_GEN         (CMD2_MOD_SIG | 0x0300)
#define SIG_DTMFGEN_EN        0x8000
#define ECMD_DTMF_REC         (CMD2_MOD_SIG | 0x0400)
#define ECMD_DTMF_REC_COEFF   (CMD2_MOD_RES | 0x0C00)
#define SIG_DTMFREC_EN        0x8000
#define SIG_DTMFREC_ET        0x4000
#define SIG_DTMFREC_AS        0x0010
#define ECMD_SIG_ATD1         (CMD2_MOD_SIG | 0x0500)
#define SIG_ATD1_EN           0x8000
#define ECMD_SIG_MFTD         (CMD2_MOD_SIG | 0x0500)
#define SIG_MFTD_SINGLE_V21L  0x0001
#define SIG_MFTD_SINGLE_V18A  0x0002
#define SIG_MFTD_SINGLE_V27   0x0004
#define SIG_MFTD_SINGLE_CNGMOD 0x0008
#define SIG_MFTD_SINGLE_CNGFAX 0x0010
#define SIG_MFTD_SINGLE_BELL  0x0020
#define SIG_MFTD_SINGLE_V22   0x0040
#define SIG_MFTD_DUAL_V32AC   0x0001
#define SIG_MFTD_DUAL_V8bis   0x0002
#define SIG_MFTD_ATD_EN       0x0002
#define SIG_MFTD_ATD_AM_EN    0x0003
#define ECMD_SIG_ATD2         (CMD2_MOD_SIG | 0x0600)
#define SIG_ATD2_EN           0x8000
#define ECMD_SIG_UTD1         (CMD2_MOD_SIG | 0x0700)
#define SIG_UTD1_EN           0x8000
#define ECMD_SIG_UTD2         (CMD2_MOD_SIG | 0x0800)
#define SIG_UTD2_EN           0x8000
#define ECMD_SIG_CHCONF       (CMD2_MOD_SIG | 0x1000)
#define ECMD_UTG1             (CMD2_MOD_SIG | 0x0A00)
#define ECMD_UTG2             (CMD2_MOD_SIG | 0x0C00)
#define SIG_UTG_EN            0x8000
#define SIG_UTG_SM            0x4000
#define SIG_UTG_A1            0x00C0
#define SIG_UTG_A2            0x0030
#define SIG_UTG_A2_VOICE      0x0010
#define SIG_UTG_A2_NOVOICE    0x0020
#define SIG_UTG_A1_VOICE      0x0040
#define SIG_UTG_A1_NOVOICE    0x0080
#define ECMD_CIDRX            (CMD2_MOD_SIG | 0x0B00)
#define SIG_CIDRX_EN          0x8000
#define SIG_DTMGGEN_EN        0x8000
#define SIG_DTMGGEN_ET        0x4000
#define SIG_DTMGGEN_AD        0x0800
#define SIG_DTMGGEN_MOD       0x0400
#define SIG_DTMGGEN_FG        0x0200
#define SIG_DTMGGEN_A1        0x00C0
#define SIG_DTMGGEN_A2        0x0030
#define SIG_DTMGGEN_GENNR     0x000F
#define SIG_DTMGGEN_EN_OF     15
#define SIG_DTMGGEN_ET_OF     14
#define SIG_DTMGGEN_AD_OF     11
#define SIG_DTMGGEN_MOD_OF     10
#define SIG_DTMGGEN_FG_OF     9
#define SIG_DTMGGEN_A1_OF     6
#define SIG_DTMGGEN_A2_OF     4
#define SIG_DTMGGEN_CIDNR_OF  0
#define ECMD_CPT              (CMD2_MOD_SIG | 0x0900)
/* V8bis and CPT is exclusive */
#define ECMD_V8BIS            (CMD2_MOD_SIG | 0x0900)
#define ECMD_EPOU_TRIG        (CMD2_MOD_SIG | 0x0F00)
#define SIG_EPOUTRIG_DTMF     0x0001
#define SIG_EPOUTRIG_ANS      0x0002
#define SIG_EPOUTRIG_NANS     0x0004
#define SIG_EPOUTRIG_ANSAM    0x0008
#define SIG_EPOUTRIG_NANSAM   0x0010
#define SIG_EPOUTRIG_CNG      0x0020
#define SIG_EPOUTRIG_DIS      0x0040
#define SIG_EPOUTRIG_DTMFCODE 0x0F00
#define SIG_EPOUTRIG_DTMFBIT  8

#define SIG_EPOUTRIG_PR_CNT   0x1000
#define ECMD_SIG_EVTSTAT      (CMD2_MOD_SIG | 0x1C00)

/* Coder module */
#define ECMD_COD_CTL          (CMD2_MOD_COD | 0x0000)
#define COD_CTL_EN            0x8000
#define ECMD_COD_CH           (CMD2_MOD_COD | 0x0100)
#define COD_CH_EN             0x8000
#define COD_CH_PTE            0x00E0
#define COD_CH_PTE_5MS        0x0000
#define COD_CH_PTE_10MS       0x0001
#define COD_CH_PTE_20MS       0x0002
#define COD_CH_PTE_30MS       0x0003
#define COD_CH_PTE_5_5MS      0x0004
#define COD_CH_PTE_11MS       0x0005
#define COD_CH_ENC            0x001F
#define COD_CH_G711_MLAW_ENC  0x0003
#define COD_CH_G711_ULAW_ENC  0x0003
#define COD_CH_G711_ALAW_ENC  0x0002
#define COD_CH_G726_16_ENC    0x0004
#define COD_CH_G726_24_ENC    0x0005
#define COD_CH_G726_32_ENC    0x0006
#define COD_CH_G726_40_ENC    0x0007
#define COD_CH_G728_ENC       0x0010
#define COD_CH_G729_ENC       0x0012
#define COD_CH_G729_E_ENC     0x0013
#define COD_CH_ILBC_152       0x001A
#define COD_CH_ILBC_133       0x001B
#define COD_CH_G723_53_ENC    0x001C
#define COD_CH_G723_63_ENC    0x001D
#define COD_GAIN_0DB          0x60
#define COD_CH_SC             0x0100
#define COD_CH_CNG            0x2000

#define ECMD_COD_AGC          (CMD2_MOD_COD | 0x0300)

#define ECMD_COD_RTP          (CMD2_MOD_COD | 0x1000)
#define ECMD_COD_CHRTP_UP     (CMD2_MOD_COD | 0x1100)
#define ECMD_COD_CHRTP_DOWN   (CMD2_MOD_COD | 0x1900)
#define ECMD_COD_AALCONF      (CMD2_MOD_COD | 0x1000)
#define COD_CHAAL_CID         0xFF00
#define ECMD_COD_CHAAL        (CMD2_MOD_COD | 0x1100)
#define COD_CHAAL_SQNR_INTV   0x0007
#define COD_CHAAL_SQNR_INTV_5MS   0x0000
#define COD_CHAAL_SQNR_INTV_10MS   0x0001
#define COD_CHAAL_SQNR_INTV_20MS   0x0002
#define COD_CHAAL_SQNR_INTV_30MS   0x0003
#define COD_CHAAL_SQNR_INTV_5_5MS  0x0004
#define COD_CHAAL_UUIS        0x0700
#define COD_CHAAL_UUI_1RANGE  0x0000
#define COD_CHAAL_UUI_2RANGES 0x0100
#define COD_CHAAL_UUI_4RANGES 0x0200
#define COD_CHAAL_UUI_8RANGES 0x0400
#define COD_CHAAL_UUI_16RANGES 0x0800
#define ECMD_COD_CHJB         (CMD2_MOD_COD | 0x1200)
#define COD_CHJB_ADAPT        0x0001
#define COD_CHJB_PCKL         0x0002
#define COD_CHJB_RAD          0x0002
#define COD_CHJB_LOC          0x0004
#define COD_CHJB_SI           0x0008
#define COD_CHJB_NAM          0x0010
#define COD_CHJB_DVF          0x0020
#define COD_CHJB_PJE          0x0040
#define COD_CHJB_PRP          0x0080
#define ECMD_COD_CHRTCP       (CMD2_MOD_COD | 0x1300)
#define ECMD_COD_CHJBSTAT     (CMD2_MOD_COD | 0x1400)
#define ECMD_COD_CHFAXDP      (CMD2_MOD_COD | 0x0800)
#define COD_CHFDP_EN          0x8000
#define COD_CHFDP_MD          0x4000
#define COD_CHFD_DPNR         0x0F00
#define COD_CHFDP_MODGAIN     0x00FF
#define COD_CHFDP_MODMAXSGLEN 1000
#define COD_CHFDP_DEMODGAIN   0xFF00
#define ECMD_COD_CHFAXMOD     (CMD2_MOD_COD | 0x1A00)
#define COD_MOD_DBM           0x3F00
#define COD_MOD_SGLEN         0x7FFF
#define ECMD_COD_CHFAXDEMOD   (CMD2_MOD_COD | 0x1B00)
#define COD_DEMOD_ST1         0x001F
#define COD_DEMOD_ST2         0x03E0
#define COD_DEMOD_EQ          0x4000
#define COD_DEMOD_TRN         0x8000
#define ECMD_COD_CHDECSTAT    (CMD2_MOD_COD | 0x1500)
#define COD_CHDECSTAT_PTD     0xFF00
#define COD_CHDECSTAT_PTC     0x0080
#define COD_CHDECSTAT_DC      0x0040
#define COD_CHDECSTAT_DEC     0x001F
#define COD_CHDECSTAT_5MS     0x0A00 /*10 * 0.5 ms = 5   ms*/
#define COD_CHDECSTAT_5_5MS   0x0B00 /*11 * 0.5 ms = 5.5 ms*/
#define COD_CHDECSTAT_10MS    0x1400 /*20 * 0.5 ms = 10  ms*/
#define COD_CHDECSTAT_11MS    0x1600 /*22 * 0.5 ms = 11  ms*/
#define COD_CHDECSTAT_20MS    0x2800 /*40 * 0.5 ms = 20  ms*/
#define COD_CHDECSTAT_30MS    0x3C00 /*60 * 0.5 ms = 305 ms*/


/* Resource module */
#define ECMD_CID_DATA         (CMD2_MOD_RES | 0x0900)
#define ECMD_CID_COEF         (CMD2_MOD_RES | 0x0800)
#define ECMD_DTMF_COEF        (CMD2_MOD_RES | 0x0A00)
#define ECMD_DTMF_DATA        (CMD2_MOD_RES | 0x0B00)
#define ECMD_ATD_COEF         (CMD2_MOD_RES | 0x0D00)
#define ECMD_MFTD_ATD_COEF    (CMD2_MOD_RES | 0x0D00)
#define ECMD_UTD_COEF         (CMD2_MOD_RES | 0x0E00)
#define ECMD_MFTD_DIS_COEF    (CMD2_MOD_RES | 0x0E00)
#define ECMD_AGC_COEF         (CMD2_MOD_RES | 0x0F00)
#define ECMD_NELEC_COEFS      (CMD2_MOD_RES | 0x0000)
#define ECMD_NELEC_COEFS_LEN  0xFF00
#define ECMD_NELEC_COEF_ADDR  (CMD2_MOD_RES | 0x0100)
#define ECMD_NELEC_COEF_DATA  (CMD2_MOD_RES | 0x0200)
#define ECMD_NELEC_NLP        (CMD2_MOD_RES | 0x0300)
#define ECMD_CPT_COEF         (CMD2_MOD_RES | 0x1000)
/* V8bis and CPT is exclusive */
#define ECMD_V8BIS_COEF       (CMD2_MOD_RES | 0x1000)
#define ECMD_UTG_COEF         (CMD2_MOD_RES | 0x1100)
#define ECMD_MFTD_UDG_SINGLE_COEF  (CMD2_MOD_RES | 0x1200)
#define ECMD_MFTD_UDG_DUAL_COEF    (CMD2_MOD_RES | 0x1300)


/* control commands */
/* command error acknowledge, clears status bit CERR */
#define ECMD_CMDERR_ACK    (CMD2_MOD_CTL | 0x0000)
#define ECMD_ERR_ACK       (CMD2_MOD_CTL | 0x0101)
/* power contol, only write */
#define ECMD_POW_CTL       (CMD2_MOD_CTL | 0x0201)
/* Test and download module*/
/* defines for firmware version */
#define ECMD_VERS          (CMD2_MOD_TST  | 0x0600)
#define ECMD_VERS_VERSION                    0x00FF
#define ECMD_VERS_FEATURES                   0x0F00
#define ECMD_VERS_FEATURES_729ABE_G723       0x0000
#define ECMD_VERS_FEATURES_G729AB_G723       0x0100
#define ECMD_VERS_FEATURES_G728_G723         0x0200
#define ECMD_VERS_FEATURES_G728_G729ABE      0x0300
#define ECMD_VERS_FEATURES_729ABE_T38        0x0400
#define ECMD_VERS_FEATURES_G729AB_G728_T38   0x0500
#define ECMD_VERS_FEATURES_G723_G728_T38     0x0600

#define ECMD_VERS_FEATURES_C              0x0800
#define ECMD_VERS_FEATURES_S              0x0F00
#define ECMD_VERS_MV                      0x6000
#define ECMD_VERS_MV_4                    0x0000
#define ECMD_VERS_EDSP_PRT                0x1000
#define ECMD_VERS_EDSP_PRT_RTP            0x0000
#define ECMD_VERS_EDSP_PRT_AAL            0x1000
#define ECMD_VERS_MV_8                    0x2000
#define ECMD_VERS_MV_4M                   0x4000
#define ECMD_VERS_MV_4VIP_6COD            0x6000
#define ECMD_VERS_MFTD                    0x0002
#define ECMD_VERS_CAP                     0x0010

/* ECMD_COD_CHJB Channel Jitter Buffer Command Bits */
#define ECMD_COD_CHJB_ADAP                0x0001
#define ECMD_COD_CHJB_PCKL                0x0002
#define ECMD_COD_CHJB_LOC                 0x0004
#define ECMD_COD_CHJB_SI                  0x0008
#define ECMD_COD_CHJB_SF_8                0x0800
#define ECMD_COD_CHJB_SF_16               0x1600
#define ECMD_COD_CHJB_SIZE_0_MS           0x0000
#define ECMD_COD_CHJB_SIZE_16_MS          0x0080
#define ECMD_COD_CHJB_SIZE_25_MS          0x00C8
#define ECMD_COD_CHJB_SIZE_50_MS          0x0190
#define ECMD_COD_CHJB_SIZE_160_MS         0x0500

#define ECMD_SET_FPI       (CMD2_MOD_TST  | 0x1400)
#define ECMD_ACCESS_FPI    (CMD2_MOD_TST  | 0x1500)
#define ECMD_CRC_FPI       (CMD2_MOD_TST  | 0x1600)
#define ECMD_CRC_DRAM      (CMD2_MOD_TST  | 0x1D00)
#define ECMD_CRC_PRAM      (CMD2_MOD_TST  | 0x1A00)
#define ECMD_DWLD_END      (CMD2_MOD_TST  | 0x1F00)
#define ECMD_SET_DRAM_ADR  (CMD2_MOD_TST  | 0x1B00)
#define ECMD_SET_PRAM_ADR  (CMD2_MOD_TST  | 0x1800)
#define ECMD_DRAM          (CMD2_MOD_TST  | 0x1C00)
#define ECMD_PRAM          (CMD2_MOD_TST  | 0x1900)
#define ECMD_AUTODWLD      (CMD2_MOD_TST  | 0x1E00)

/* DTMF/AT Generator DTC Dual Alert Tone*/
#define ECMD_DTMFGEN_DTC_DAT  0x0012

#define ECMD_ENDIAN_CTL    (CMD2_MOD_CTL  | 0x0400)
#define ECMD_ENDIAN_LE     0x0101
#define ECMD_ENDIAN_BE     0

/* UTG mask coefficients for tone generation steps */
#define ECMD_UTG_COEF_MSK_NXT             0xF000
#define ECMD_UTG_COEF_MSK_REP             0x0E00
#define ECMD_UTG_COEF_MSK_FI              0x0100
#define ECMD_UTG_COEF_MSK_FREQ            0x00F0
#define ECMD_UTG_COEF_MSK_F1              0x0080
#define ECMD_UTG_COEF_MSK_F2              0x0040
#define ECMD_UTG_COEF_MSK_F3              0x0020
#define ECMD_UTG_COEF_MSK_F4              0x0010
#define ECMD_UTG_COEF_MSK_M12             0x0008
#define ECMD_UTG_COEF_MSK_FO              0x0004
#define ECMD_UTG_COEF_MSK_SA              0x0003
#define ECMD_UTG_COEF_SA_00               0x0000
#define ECMD_UTG_COEF_SA_01               0x0001
#define ECMD_UTG_COEF_SA_10               0x0002
#define ECMD_UTG_COEF_SA_11               0x0003

/* ============================= */
/* Signal Array                  */
/* ============================= */

typedef enum _ECMD_IX_SIG
{
   ECMD_IX_EMPTY      = 0x00,
   ECMD_IX_PCM_OUT0   = 0x01,
   ECMD_IX_PCM_OUT1   = 0x02,
   ECMD_IX_PCM_OUT2   = 0x03,
   ECMD_IX_PCM_OUT3   = 0x04,
   ECMD_IX_PCM_OUT4   = 0x05,
   ECMD_IX_PCM_OUT5   = 0x06,
   ECMD_IX_PCM_OUT6   = 0x07,
   ECMD_IX_PCM_OUT7   = 0x08,
   ECMD_IX_PCM_OUT8   = 0x09,
   ECMD_IX_PCM_OUT9   = 0x0A,
   ECMD_IX_PCM_OUT10  = 0x0B,
   ECMD_IX_PCM_OUT11  = 0x0C,
   ECMD_IX_PCM_OUT12  = 0x0D,
   ECMD_IX_PCM_OUT13  = 0x0E,
   ECMD_IX_PCM_OUT14  = 0x0F,
   ECMD_IX_PCM_OUT15  = 0x10,
   ECMD_IX_ALM_OUT0   = 0x11,
   ECMD_IX_ALM_OUT1   = 0x12,
   ECMD_IX_ALM_OUT2   = 0x13,
   ECMD_IX_ALM_OUT3   = 0x14,
   ECMD_IX_COD_OUT0   = 0x18,
   ECMD_IX_COD_OUT1   = 0x19,
   ECMD_IX_COD_OUT2   = 0x1A,
   ECMD_IX_COD_OUT3   = 0x1B,
   ECMD_IX_COD_OUT4   = 0x1C,
   ECMD_IX_COD_OUT5   = 0x1D,
   ECMD_IX_COD_OUT6   = 0x1E,
   ECMD_IX_COD_OUT7   = 0x1F,
   ECMD_IX_SIG_OUTA0  = 0x28,
   ECMD_IX_SIG_OUTB0  = 0x29,
   ECMD_IX_SIG_OUTA1  = 0x2A,
   ECMD_IX_SIG_OUTB1  = 0x2B,
   ECMD_IX_SIG_OUTA2  = 0x2C,
   ECMD_IX_SIG_OUTB2  = 0x2D,
   ECMD_IX_SIG_OUTA3  = 0x2E,
   ECMD_IX_SIG_OUTB3  = 0x2F
} ECMD_IX_SIG;

/* ============================ */
/* defines for the ram access   */
/* ============================ */
typedef enum _VINETIC_FW_RAM
{
   D_RAM = 0,
   P_RAM
} VINETIC_FW_RAM;

/* ============================= */
/* Global Structures             */
/* ============================= */

/* CRC structure */
typedef struct
{
   IFX_uint8_t nMemId;
   IFX_uint32_t nStartAdr;
   IFX_uint32_t nStopAdr;
   IFX_uint16_t nCrc;
} VINETIC_IO_CRC;

/* ============================= */
/* CID Module                    */
/* ============================= */

/* CID Sending structure */
typedef struct
{
   IFX_uint8_t  nCh;
   IFX_uint8_t  nResNr;
   IFX_uint16_t nBurstTime;
   IFX_uint16_t nBurst2Cid;
   IFX_uint16_t nCid2Burst;
   IFX_uint8_t  nOdd;
   IFX_uint8_t  nLen;
   IFX_uint16_t pData[128];
} VINETIC_IO_CID_SEND;


#define CMD_ALM_CH_LEN 4
#define CMD_PCM_CH_LEN 5
#define CMD_SIG_CH_LEN 1
#define CMD_COD_CH_LEN 5
#define CMD_COD_RTCP_LEN 0xE
#define CMD_SIG_ATD_LEN 1
#define CMD_SIG_UTD_LEN 1
#define CMD_SIG_UTDCOEFF_LEN 7
#define CMD_SIG_ATDCOEFF_LEN 6
#define CMD_SIG_DTMFGEN_LEN  1
#define CMD_SIG_CPT_LEN      1
#define CMD_SIG_V8BIS_LEN    1
#define CMD_SIG_CPTCOEFF_LEN  0x16
#define CMD_SIG_V8BISCOEFF_LEN  0x4
#define CMD_SIG_MFTDCTRL_LEN 3
#define CMD_SIG_MFTD_ATDCOEFF_LEN 7
#define CMD_SIG_MFTD_DISCOEFF_LEN 2
#define CMD_SIG_MFTD_UGDSINGLECOEFF_LEN 6
#define CMD_SIG_MFTD_UGDDUALCOEFF 9
#define CMD_VERS_FEATURES_BASIC_LEN 2
#define CMD_VERS_FEATURES_EXT_LEN 3

/******************************************************************************
** Firmware Analog Line Interface Channel Cmd
**
*******************************************************************************/
typedef union
{
   IFX_uint16_t value[CMD_HEADER_CNT + CMD_ALM_CH_LEN];
   struct {
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
      /* cmd 1 */
      unsigned ch                            : 4;
      unsigned res                           : 4;
      unsigned cmd                           : 5;
      unsigned bc                            : 1;
      unsigned res0                          : 1;
      unsigned rw                            : 1;
      /* cmd 2 */
      unsigned length                        : 8;
      unsigned ecmd                          : 5;
      unsigned mod                           : 3;
      /* data */
      unsigned i1                            : 6;
      unsigned res1                          : 9;
      unsigned en                            : 1;

      unsigned gain_r                        : 8;
      unsigned gain_x                        : 8;

      unsigned i3                            : 6;
      unsigned res3                          : 2;
      unsigned i2                            : 6;
      unsigned res2                          : 2;

      unsigned i5                            : 6;
      unsigned res5                          : 2;
      unsigned i4                            : 6;
      unsigned res4                          : 2;
#endif /* LITTLE_ENDIAN */
#if (__BYTE_ORDER == __BIG_ENDIAN)
      /* cmd 1 */
      unsigned rw                            : 1;
      unsigned res0                          : 1;
      unsigned bc                            : 1;
      unsigned cmd                           : 5;
      unsigned res                           : 4;
      unsigned ch                            : 4;
      /* cmd 2 */
      unsigned mod                           : 3;
      unsigned ecmd                          : 5;
      unsigned length                        : 8;
      /* data */
      unsigned en                            : 1;
      unsigned res1                          : 9;
      unsigned i1                            : 6;

      unsigned gain_x                        : 8;
      unsigned gain_r                        : 8;

      unsigned res2                          : 2;
      unsigned i2                            : 6;
      unsigned res3                          : 2;
      unsigned i3                            : 6;

      unsigned res4                          : 2;
      unsigned i4                            : 6;
      unsigned res5                          : 2;
      unsigned i5                            : 6;
   #endif /* BIG_ENDIAN */
   } bit;
} FWM_ALM_CH;

/******************************************************************************
** Firmware PCM Interface Channel Cmd
**
*******************************************************************************/
typedef union
{
   IFX_uint16_t value[CMD_HEADER_CNT + CMD_PCM_CH_LEN];
   struct {
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
      /* cmd 1 */
      unsigned ch                            : 4;
      unsigned res                           : 4;
      unsigned cmd                           : 5;
      unsigned bc                            : 1;
      unsigned res0                          : 1;
      unsigned rw                            : 1;
      /* cmd 2 */
      unsigned length                        : 8;
      unsigned ecmd                          : 5;
      unsigned mod                           : 3;
      /* data */
      unsigned i1                            : 6;
      unsigned bp                            : 1;
      unsigned hp                            : 1;
      unsigned codnr                         : 4;
      unsigned cod                           : 3;
      unsigned en                            : 1;

      unsigned rts                           : 7;
      unsigned r_hw                          : 1;
      unsigned xts                           : 7;
      unsigned x_hw                          : 1;

      unsigned gain_2                        : 8;
      unsigned gain_1                        : 8;

      unsigned i3                            : 6;
      unsigned res2                          : 2;
      unsigned i2                            : 6;
      unsigned res1                          : 2;

      unsigned i5                            : 6;
      unsigned res4                          : 2;
      unsigned i4                            : 6;
      unsigned res3                          : 2;
#endif /* LITTLE_ENDIAN */
#if (__BYTE_ORDER == __BIG_ENDIAN)
      /* cmd 1 */
      unsigned rw                            : 1;
      unsigned res0                          : 1;
      unsigned bc                            : 1;
      unsigned cmd                           : 5;
      unsigned res                           : 4;
      unsigned ch                            : 4;
      /* cmd 2 */
      unsigned mod                           : 3;
      unsigned ecmd                          : 5;
      unsigned length                        : 8;
      /* data */
      unsigned en                            : 1;
      unsigned cod                           : 3;
      unsigned codnr                         : 4;
      unsigned hp                            : 1;
      unsigned bp                            : 1;
      unsigned i1                            : 6;

      unsigned x_hw                          : 1;
      unsigned xts                           : 7;
      unsigned r_hw                          : 1;
      unsigned rts                           : 7;

      unsigned gain_1                        : 8;
      unsigned gain_2                        : 8;

      unsigned res1                          : 2;
      unsigned i2                            : 6;
      unsigned res2                          : 2;
      unsigned i3                            : 6;

      unsigned res3                          : 2;
      unsigned i4                            : 6;
      unsigned res4                          : 2;
      unsigned i5                            : 6;
#endif /* BIG_ENDIAN */
   } bit;
} FWM_PCM_CH;

typedef union
{
   IFX_uint16_t value[3];
   struct
   {
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
      /* cmd 1 */
      unsigned ch                            : 4;
      unsigned res                           : 4;
      unsigned cmd                           : 5;
      unsigned bc                            : 1;
      unsigned res0                          : 1;
      unsigned rw                            : 1;
      /* cmd 2 */
      unsigned length                        : 8;
      unsigned ecmd                          : 5;
      unsigned mod                           : 3;
      /* data */
      unsigned i2                            : 6;
      unsigned res2                          : 2;
      unsigned i1                            : 6;
      unsigned res1                          : 1;
      unsigned en                            : 1;
#endif /* LITTLE_ENDIAN */
#if (__BYTE_ORDER == __BIG_ENDIAN)
      /* cmd 1 */
      unsigned rw                            : 1;
      unsigned res0                          : 1;
      unsigned bc                            : 1;
      unsigned cmd                           : 5;
      unsigned res                           : 4;
      unsigned ch                            : 4;
      /* cmd 2 */
      unsigned mod                           : 3;
      unsigned ecmd                          : 5;
      unsigned length                        : 8;
      /* data */
      unsigned en                            : 1;
      unsigned res1                          : 1;
      unsigned i1                            : 6;
      unsigned res2                          : 2;
      unsigned i2                            : 6;
#endif /* BIG_ENDIAN */
   } bit;
} FWM_SIG_CH;

typedef union
{
   IFX_uint16_t value[CMD_HEADER_CNT + CMD_COD_CH_LEN];
   struct {
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
      /* cmd 1 */
      unsigned ch                            : 4;
      unsigned res                           : 4;
      unsigned cmd                           : 5;
      unsigned bc                            : 1;
      unsigned res0                          : 1;
      unsigned rw                            : 1;
      /* cmd 2 */
      unsigned length                        : 8;
      unsigned ecmd                          : 5;
      unsigned mod                           : 3;
      /* data */
      unsigned i1                            : 6;
      unsigned res2                          : 1;
      unsigned ns                            : 1;
      unsigned codnr                         : 4;
      unsigned res1                          : 3;
      unsigned en                            : 1;

      unsigned enc                           : 5;
      unsigned pte                           : 3;
      unsigned sc                            : 1;
      unsigned pst                           : 1;
      unsigned im                            : 1;
      unsigned dec                           : 1;
      unsigned bfi                           : 1;
      unsigned cng                           : 1;
      unsigned pf                            : 1;
      unsigned hp                            : 1;

      unsigned gain2                         : 8;
      unsigned gain1                         : 8;

      unsigned i3                            : 6;
      unsigned res4                          : 2;
      unsigned i2                            : 6;
      unsigned res3                          : 2;

      unsigned i5                            : 6;
      unsigned res6                          : 2;
      unsigned i4                            : 6;
      unsigned res5                          : 2;
#endif /* LITTLE_ENDIAN */
#if (__BYTE_ORDER == __BIG_ENDIAN)
      /* cmd 1 */
      unsigned rw                            : 1;
      unsigned res0                          : 1;
      unsigned bc                            : 1;
      unsigned cmd                           : 5;
      unsigned res                           : 4;
      unsigned ch                            : 4;
      /* cmd 2 */
      unsigned mod                           : 3;
      unsigned ecmd                          : 5;
      unsigned length                        : 8;
      /* data */
      unsigned en                            : 1;
      unsigned res1                          : 3;
      unsigned codnr                         : 4;
      unsigned ns                            : 1;
      unsigned res2                          : 1;
      unsigned i1                            : 6;

      unsigned hp                            : 1;
      unsigned pf                            : 1;
      unsigned cng                           : 1;
      unsigned bfi                           : 1;
      unsigned dec                           : 1;
      unsigned im                            : 1;
      unsigned pst                           : 1;
      unsigned sc                            : 1;
      unsigned pte                           : 3;
      unsigned enc                           : 5;

      unsigned gain1                         : 8;
      unsigned gain2                         : 8;

      unsigned res3                          : 2;
      unsigned i2                            : 6;
      unsigned res4                          : 2;
      unsigned i3                            : 6;

      unsigned res5                          : 2;
      unsigned i4                            : 6;
      unsigned res6                          : 2;
      unsigned i5                            : 6;
#endif /* BIG_ENDIAN */
   } bit;
} FWM_COD_CH;

typedef union
{
   IFX_uint16_t value [3];
   struct {
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
      /* cmd 1 */
      unsigned ch                            : 4;
      unsigned res                           : 4;
      unsigned cmd                           : 5;
      unsigned bc                            : 1;
      unsigned sc                            : 1;
      unsigned rw                            : 1;
      /* cmd 2 */
      unsigned length                        : 8;
      unsigned ecmd                          : 5;
      unsigned mod                           : 3;
      /* data 1 */
      unsigned resnr:4;
      unsigned is:2;
      unsigned md:2;
      unsigned res2:6;
      unsigned et:1;
      unsigned en:1;
#endif /* LITTLE_ENDIAN */
#if (__BYTE_ORDER == __BIG_ENDIAN)
      /* cmd 1 */
      unsigned rw                            : 1;
      unsigned sc                            : 1;
      unsigned bc                            : 1;
      unsigned cmd                           : 5;
      unsigned res                           : 4;
      unsigned ch                            : 4;
      /* cmd 2 */
      unsigned mod                           : 3;
      unsigned ecmd                          : 5;
      unsigned length                        : 8;
      /* data 1 */
      unsigned en:1;
      unsigned et:1;
      unsigned res2:6;
      unsigned md:2;
      unsigned is:2;
      unsigned resnr:4;
#endif /* BIG_ENDIAN */
   } bit;
}FWM_SIG_XTD;

typedef union
{
   IFX_uint16_t value [5];
   struct {
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
      /* cmd 1 */
      unsigned ch                            : 4;
      unsigned res                           : 4;
      unsigned cmd                           : 5;
      unsigned bc                            : 1;
      unsigned sc                            : 1;
      unsigned rw                            : 1;
      /* cmd 2 */
      unsigned length                        : 8;
      unsigned ecmd                          : 5;
      unsigned mod                           : 3;
      /* data 1 */
      unsigned resnr                         : 4;
      unsigned res3                          : 3;
      unsigned mh                            : 1;
      unsigned res2                          : 4;
      unsigned etc                           : 1;
      unsigned etd                           : 1;
      unsigned eta                           : 1;
      unsigned en                            : 1;
      /* data 2  (upstream) */
      unsigned single1                       : 7;
      unsigned res5                          : 2;
      unsigned dis1                          : 1;
      unsigned atd1                          : 2;
      unsigned dual1                         : 2;
      unsigned res4                          : 2;
      /* data 3 (downstream) */
      unsigned single2                       : 7;
      unsigned res7                          : 2;
      unsigned dis2                          : 1;
      unsigned atd2                          : 2;
      unsigned dual2                         : 2;
      unsigned res6                          : 2;
#endif /* LITTLE_ENDIAN */
#if (__BYTE_ORDER == __BIG_ENDIAN)
      /* cmd 1 */
      unsigned rw                            : 1;
      unsigned sc                            : 1;
      unsigned bc                            : 1;
      unsigned cmd                           : 5;
      unsigned res                           : 4;
      unsigned ch                            : 4;
      /* cmd 2 */
      unsigned mod                           : 3;
      unsigned ecmd                          : 5;
      unsigned length                        : 8;
      /* data 1 */
      unsigned en                            : 1;
      unsigned eta                           : 1;
      unsigned etd                           : 1;
      unsigned etc                           : 1;
      unsigned res2                          : 4;
      unsigned mh                            : 1;
      unsigned res3                          : 3;
      unsigned resnr                         : 4;
      /* data 2  (upstream) */
      unsigned res4                          : 2;
      unsigned dual1                         : 2;
      unsigned atd1                          : 2;
      unsigned dis1                          : 1;
      unsigned res5                          : 2;
      unsigned single1                       : 7;
      /* data 3 (downstream) */
      unsigned res6                          : 2;
      unsigned dual2                         : 2;
      unsigned atd2                          : 2;
      unsigned dis2                          : 1;
      unsigned res7                          : 2;
      unsigned single2                       : 7;
#endif /* BIG_ENDIAN */
   } bit;
}FWM_SIG_MFTD;

typedef union
{
   IFX_uint16_t value [CMD_HEADER_CNT + CMD_SIG_DTMFGEN_LEN];
   struct {
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
      /* cmd 1 */
      unsigned ch                            : 4;
      unsigned res                           : 4;
      unsigned cmd                           : 5;
      unsigned bc                            : 1;
      unsigned sc                            : 1;
      unsigned rw                            : 1;
      /* cmd 2 */
      unsigned length                        : 8;
      unsigned ecmd                          : 5;
      unsigned cmdmod                        : 3;
      /* data 1 */
      unsigned resnr:4;
      unsigned addb:2;
      unsigned adda:2;
      unsigned res3:1;
      unsigned fg:1;
      unsigned mod:1;
      unsigned ad:1;
      unsigned res2:2;
      unsigned et:1;
      unsigned en:1;
#endif /* LITTLE_ENDIAN */
#if (__BYTE_ORDER == __BIG_ENDIAN)
      /* cmd 1 */
      unsigned rw                            : 1;
      unsigned sc                            : 1;
      unsigned bc                            : 1;
      unsigned cmd                           : 5;
      unsigned res                           : 4;
      unsigned ch                            : 4;
      /* cmd 2 */
      unsigned cmdmod                        : 3;
      unsigned ecmd                          : 5;
      unsigned length                        : 8;
      /* data 1 */
      unsigned en:1;
      unsigned et:1;
      unsigned res2:2;
      unsigned ad:1;
      unsigned mod:1;
      unsigned fg:1;
      unsigned res3:1;
      unsigned adda:2;
      unsigned addb:2;
      unsigned resnr:4;
#endif /* BIG_ENDIAN */
   } bit;
} FWM_SIG_DTMFGEN;

typedef union
{
   IFX_uint16_t value;
   struct {
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
      /* data 1 */
      unsigned dtrnr:4;
      unsigned as:1;
      unsigned is:1;
      unsigned res1:8;
      unsigned et:1;
      unsigned en:1;
#endif /* LITTLE_ENDIAN */
#if (__BYTE_ORDER == __BIG_ENDIAN)
      /* data 1 */
      unsigned en:1;
      unsigned et:1;
      unsigned res1:8;
      unsigned is:1;
      unsigned as:1;
      unsigned dtrnr:4;
#endif /* BIG_ENDIAN */
   } bit;
} FWM_SIG_DTMFREC;

typedef union
{
   IFX_uint16_t value [3];
   struct {
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
      /* cmd 1 */
      unsigned ch                            : 4;
      unsigned res                           : 4;
      unsigned cmd                           : 5;
      unsigned bc                            : 1;
      unsigned sc                            : 1;
      unsigned rw                            : 1;
      /* cmd 2 */
      unsigned length                        : 8;
      unsigned ecmd                          : 5;
      unsigned mod                           : 3;
      /* data 1 */
      unsigned utgnr:4;
      unsigned a2:2;
      unsigned a1:2;
      unsigned res2:4;
      unsigned log:1;
      unsigned sq:1;
      unsigned sm:1;
      unsigned en:1;
#endif /* LITTLE_ENDIAN */
#if (__BYTE_ORDER == __BIG_ENDIAN)
      /* cmd 1 */
      unsigned rw                            : 1;
      unsigned sc                            : 1;
      unsigned bc                            : 1;
      unsigned cmd                           : 5;
      unsigned res                           : 4;
      unsigned ch                            : 4;
      /* cmd 2 */
      unsigned mod                           : 3;
      unsigned ecmd                          : 5;
      unsigned length                        : 8;
      /* data 1 */
      unsigned en:1;
      unsigned sm:1;
      unsigned sq:1;
      unsigned log:1;
      unsigned res2:4;
      unsigned a1:2;
      unsigned a2:2;
      unsigned utgnr:4;
#endif /* BIG_ENDIAN */
   } bit;
}FWM_SIG_UTG;

typedef union
{
   IFX_uint16_t value[8];
   struct
   {
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
      /* cmd 1 */
      unsigned ch                            : 4;
      unsigned res                           : 4;
      unsigned cmd                           : 5;
      unsigned bc                            : 1;
      unsigned res0                          : 1;
      unsigned rw                            : 1;
      /* cmd 2 */
      unsigned length                        : 8;
      unsigned ecmd                          : 5;
      unsigned mod                           : 3;
      /* data 1 */
      unsigned ssrch                         : 16;
      /* data 2 */
      unsigned ssrcl                         : 16;
      /* data 3 */
      unsigned res2                          : 16;
      /* data 4 */
      unsigned eventPt                       : 7;
      unsigned res3                          : 9;
      /* data 5 */
      unsigned a2                            : 2;
      unsigned a1                            : 2;
      unsigned coderCh                       : 4;
      unsigned evMaskTOG                     : 3;
      unsigned res4                          : 4;
      unsigned vblock                        : 1;
      /* data 6 */
      unsigned evMaskTrig                    : 7;
      unsigned res5                          : 9;
#endif /* LITTLE_ENDIAN */
#if (__BYTE_ORDER == __BIG_ENDIAN)
      /* cmd 1 */
      unsigned rw                            : 1;
      unsigned res0                          : 1;
      unsigned bc                            : 1;
      unsigned cmd                           : 5;
      unsigned res                           : 4;
      unsigned ch                            : 4;
      /* cmd 2 */
      unsigned mod                           : 3;
      unsigned ecmd                          : 5;
      unsigned length                        : 8;
      /* data 1 */
      unsigned ssrch                         : 16;
      /* data 2 */
      unsigned ssrcl                         : 16;
      /* data 3 */
      unsigned res2                          : 16;
      /* data 4 */
      unsigned res3                          : 9;
      unsigned eventPt                       : 7;
      /* data 5 */
      unsigned vblock                        : 1;
      unsigned res4                          : 4;
      unsigned evMaskTOG                     : 3;
      unsigned coderCh                       : 4;
      unsigned a1                            : 2;
      unsigned a2                            : 2;
      /* data 6 */
      unsigned res5                          : 9;
      unsigned evMaskTrig                    : 7;
#endif /* BIG_ENDIAN */
   } bit;
} FWM_SIG_CH_CONF;

/* values for a2 and a1 */
#define SIG_UTG_MUTE       2
#define SIG_UTG_INJECT     1
#define SIG_UTG_NONE       0

/* signaling module ATD command access macros */
#define VIN_ECMD_SIGATD_EN                           (1 << 15)
#define VIN_ECMD_SIGATD_EN_GET(reg)                  ((reg & VIN_ECMD_SIGATD_EN) >> 15)
#define VIN_ECMD_SIGATD_EN_SET(reg, val)              (reg = ((reg & ~VIN_ECMD_SIGATD_EN) | (((val) & 0x0001) << 15)))

#define VIN_ECMD_SIGATD_ET                           (1 << 14)
#define VIN_ECMD_SIGATD_ET_GET(reg)                  ((reg & VIN_ECMD_SIGATD_ET) >> 14)
#define VIN_ECMD_SIGATD_ET_SET(reg, val)             (reg = ((reg & ~VIN_ECMD_SIGATD_ET) | (((val) & 0x0001) << 14)))

#define VIN_ECMD_SIGATD_MD_MASK                      (((1 << 2) - 1) << 6)
#define VIN_ECMD_SIGATD_MD_GET(reg)                  (((reg) >> 6) & ((1 << 2) - 1))
#define VIN_ECMD_SIGATD_MD_SET(reg, val)             (reg = ( (reg & ~VIN_ECMD_SIGATD_MD_MASK) | ((((1 << 2) - 1) & (val)) << 6)))

#define VIN_ECMD_SIGATD_MD_ANS_PHREV                  0x0
#define VIN_ECMD_SIGATD_MD_ANS_PHREV_AM               0x1
#define VIN_ECMD_SIGATD_MD_DIS                        0x2
#define VIN_ECMD_SIGATD_MD_TONEHOLDING                0x3

#define VIN_ECMD_SIGATD_IS_MASK                      (((1 << 2) - 1) << 4)
#define VIN_ECMD_SIGATD_IS_GET(reg)                  (((reg) >> 4) & ((1 << 2) - 1))
#define VIN_ECMD_SIGATD_IS_SET(reg, val)             (reg = ( (reg & ~VIN_ECMD_SIGATD_IS_MASK) | ((((1 << 2) - 1) & (val)) << 4)))

#define VIN_ECMD_SIGATD_IS_SIGINA                    0x00
#define VIN_ECMD_SIGATD_IS_SIGINB                    0x01
#define VIN_ECMD_SIGATD_IS_SIGINBOTH                 0x02

#define VIN_ECMD_SIGATD_ATDNR_MASK                   (((1 << 4) - 1) << 0)
#define VIN_ECMD_SIGATD_ATDNR_GET(reg)               (((reg) >> 0) & ((1 << 4) - 1))
#define VIN_ECMD_SIGATD_ATDNR_SET(reg, val)          (reg = ( (reg & ~VIN_ECMD_SIGATD_ATDNR_MASK) | ((((1 << 4) - 1) & (val)) << 0)))

/* signaling module UTD command access macros */
#define VIN_ECMD_SIGUTD_EN                           (1 << 15)
#define VIN_ECMD_SIGUTD_EN_GET(reg)                  ((reg & VIN_ECMD_SIGUTD_EN) >> 15)
#define VIN_ECMD_SIGUTD_EN_SET(reg, val)             (reg = ((reg & ~VIN_ECMD_SIGUTD_EN) | (((val) & 0x0001) << 15)))

#define VIN_ECMD_SIGUTD_ET                           (1 << 14)
#define VIN_ECMD_SIGUTD_ET_GET(reg)                  ((reg & VIN_ECMD_SIGUTD_ET) >> 14)
#define VIN_ECMD_SIGUTD_ET_SET(reg, val)             (reg = ((reg & ~VIN_ECMD_SIGUTD_ET) | (((val) & 0x0001) << 14)))

#define VIN_ECMD_SIGUTD_MD_MASK                      (((1 << 2) - 1) << 6)
#define VIN_ECMD_SIGUTD_MD_GET(reg)                  (((reg) >> 6) & ((1 << 2) - 1))
#define VIN_ECMD_SIGUTD_MD_SET(reg, val)             (reg = ( (reg & ~VIN_ECMD_SIGUTD_MD_MASK) | ((((1 << 2) - 1) & (val)) << 6)))

#define VIN_ECMD_SIGUTD_MD_UNV                       0x0
#define VIN_ECMD_SIGUTD_MD_V18                       0x1
#define VIN_ECMD_SIGUTD_MD_MODHOLDING                0x2

#define VIN_ECMD_SIGUTD_IS_MASK                      (((1 << 2) - 1) << 4)
#define VIN_ECMD_SIGUTD_IS_GET(reg)                  (((reg) >> 4) & ((1 << 2) - 1))
#define VIN_ECMD_SIGUTD_IS_SET(reg, val)             (reg = ( (reg & ~VIN_ECMD_SIGUTD_IS_MASK) | ((((1 << 2) - 1) & (val)) << 4)))

#define VIN_ECMD_SIGUTD_IS_SIGINA                    0x00
#define VIN_ECMD_SIGUTD_IS_SIGINB                    0x01
#define VIN_ECMD_SIGUTD_IS_SIGINBOTH                 0x02

#define VIN_ECMD_SIGUTD_UTDNR_MASK                   (((1 << 4) - 1) << 0)
#define VIN_ECMD_SIGUTD_UTDNR_GET(reg)               (((reg) >> 0) & ((1 << 4) - 1))
#define VIN_ECMD_SIGUTD_UTDNR_SET(reg, val)          (reg = ( (reg & ~VIN_ECMD_SIGUTD_UTDNR_MASK) | ((((1 << 4) - 1) & (val)) << 0)))

/* access definitions for CPT */
#define VIN_ECMD_SIGCPT_EN                           (1 << 15)
#define VIN_ECMD_SIGCPT_EN_GET(reg)                  ((reg & VIN_ECMD_SIGCPT_EN) >> 15)
#define VIN_ECMD_SIGCPT_EN_SET(reg, val)             (reg = ((reg & ~VIN_ECMD_SIGCPT_EN) | (((val) & 0x0001) << 15)))

#define VIN_ECMD_SIGCPT_AT                           (1 << 14)
#define VIN_ECMD_SIGCPT_AT_GET(reg)                  ((reg & VIN_ECMD_SIGCPT_EN) >> 14)
#define VIN_ECMD_SIGCPT_AT_SET(reg, val)             (reg = ((reg & ~VIN_ECMD_SIGCPT_AT) | (((val) & 0x0001) << 14)))

#define VIN_ECMD_SIGCPT_ATS_MASK                      (((1 << 2) - 1) << 12)
#define VIN_ECMD_SIGCPT_ATS_GET(reg)                  (((reg) >> 12) & ((1 << 2) - 1))
#define VIN_ECMD_SIGCPT_ATS_SET(reg, val)             (reg = ( (reg & ~VIN_ECMD_SIGCPT_ATS_MASK) | ((((1 << 2) - 1) & (val)) << 12)))

#define VIN_ECMD_SIGCPT_TP                           (1 << 11)
#define VIN_ECMD_SIGCPT_TP_GET(reg)                  ((reg & VIN_ECMD_SIGCPT_TP) >> 11)
#define VIN_ECMD_SIGCPT_TP_SET(reg, val)             (reg = ((reg & ~VIN_ECMD_SIGCPT_TP) | (((val) & 0x0001) << 11)))

#define VIN_ECMD_SIGCPT_CNT                          (1 << 10)
#define VIN_ECMD_SIGCPT_CNT_GET(reg)                 ((reg & VIN_ECMD_SIGCPT_CNT) >> 10)
#define VIN_ECMD_SIGCPT_CNT_SET(reg, val)            (reg = ((reg & ~VIN_ECMD_SIGCPT_CNT) | (((val) & 0x0001) << 10)))

#define VIN_ECMD_SIGCPT_FL_MASK                      (((1 << 2) - 1) << 8)
#define VIN_ECMD_SIGCPT_FL_GET(reg)                  (((reg) >> 8) & ((1 << 2) - 1))
#define VIN_ECMD_SIGCPT_FL_SET(reg, val)             (reg = ( (reg & ~VIN_ECMD_SIGCPT_FL_MASK) | ((((1 << 2) - 1) & (val)) << 8)))
#define VIN_ECMD_SIGCPT_FL_16MS                      0
#define VIN_ECMD_SIGCPT_FL_32MS                      1
#define VIN_ECMD_SIGCPT_FL_64MS                      2

#define VIN_ECMD_SIGCPT_WS                           (1 << 7)
#define VIN_ECMD_SIGCPT_WS_GET(reg)                  ((reg & VIN_ECMD_SIGCPT_WS) >> 7)
#define VIN_ECMD_SIGCPT_WS_SET(reg, val)             (reg = ((reg & ~VIN_ECMD_SIGCPT_WS) | (((val) & 0x0001) << 7)))
#define VIN_ECMD_SIGCPT_WS_HAMMING                   0
#define VIN_ECMD_SIGCPT_WS_BLACKMAN                  1

#define VIN_ECMD_SIGCPT_IS_MASK                      (((1 << 2) - 1) << 4)
#define VIN_ECMD_SIGCPT_IS_GET(reg)                  (((reg) >> 4) & ((1 << 2) - 1))
#define VIN_ECMD_SIGCPT_IS_SET(reg, val)             (reg = ( (reg & ~VIN_ECMD_SIGCPT_IS_MASK) | ((((1 << 2) - 1) & (val)) << 4)))

#define VIN_ECMD_SIGCPT_IS_SIGINA                    0x00
#define VIN_ECMD_SIGCPT_IS_SIGINB                    0x01
#define VIN_ECMD_SIGCPT_IS_SIGINBOTH                 0x02

#define VIN_ECMD_SIGCPT_CPTNR_MASK                   (((1 << 4) - 1) << 0)
#define VIN_ECMD_SIGCPT_CPTNR_GET(reg)               (((reg) >> 0) & ((1 << 4) - 1))
#define VIN_ECMD_SIGCPT_CPTNR_SET(reg, val)          (reg = ( (reg & ~VIN_ECMD_SIGCPT_CPTNR_MASK) | ((((1 << 4) - 1) & (val)) << 0)))

/* CPTD coefficient */
#define VIN_ECMD_SIGCPTCOEFF_F1                      0x0001
#define VIN_ECMD_SIGCPTCOEFF_F2                      0x0004
#define VIN_ECMD_SIGCPTCOEFF_F3                      0x0010
#define VIN_ECMD_SIGCPTCOEFF_F4                      0x0040

#define VIN_ECMD_SIGCPTCOEFF_F1_MASK                 (((1 << 2) - 1) << 0)
#define VIN_ECMD_SIGCPTCOEFF_F1_GET(reg)             (((reg) >> 0) & ((1 << 2) - 1))
#define VIN_ECMD_SIGCPTCOEFF_F1_SET(reg, val)        (reg = ( (reg & ~VIN_ECMD_SIGCPTCOEFF_F1_MASK) | ((((1 << 2) - 1) & (val)) << 0)))

#define VIN_ECMD_SIGCPTCOEFF_F2_MASK                 (((1 << 2) - 1) << 2)
#define VIN_ECMD_SIGCPTCOEFF_F2_GET(reg)             (((reg) >> 2) & ((1 << 2) - 1))
#define VIN_ECMD_SIGCPTCOEFF_F2_SET(reg, val)        (reg = ( (reg & ~VIN_ECMD_SIGCPTCOEFF_F2_MASK) | ((((1 << 2) - 1) & (val)) << 2)))

#define VIN_ECMD_SIGCPTCOEFF_F3_MASK                 (((1 << 2) - 1) << 4)
#define VIN_ECMD_SIGCPTCOEFF_F3_GET(reg)             (((reg) >> 4) & ((1 << 2) - 1))
#define VIN_ECMD_SIGCPTCOEFF_F3_SET(reg, val)        (reg = ( (reg & ~VIN_ECMD_SIGCPTCOEFF_F3_MASK) | ((((1 << 2) - 1) & (val)) << 4)))

#define VIN_ECMD_SIGCPTCOEFF_F4_MASK                 (((1 << 2) - 1) << 6)
#define VIN_ECMD_SIGCPTCOEFF_F4_GET(reg)             (((reg) >> 6) & ((1 << 2) - 1))
#define VIN_ECMD_SIGCPTCOEFF_F4_SET(reg, val)        (reg = ( (reg & ~VIN_ECMD_SIGCPTCOEFF_F4_MASK) | ((((1 << 2) - 1) & (val)) << 6)))

#define VIN_ECMD_SIGCPTCOEFF_F12OR34                 (1 << 12)
#define VIN_ECMD_SIGCPTCOEFF_F12OR34_GET(reg)        ((reg & VIN_ECMD_SIGCPTCOEFF_F12OR34) >> 12)
#define VIN_ECMD_SIGCPTCOEFF_F12OR34_SET(reg, val)   (reg = ((reg & ~VIN_ECMD_SIGCPTCOEFF_F12OR34) | (((val) & 0x0001) << 12)))

#define VIN_ECMD_SIGCPTCOEFF_E                       (1 << 13)
#define VIN_ECMD_SIGCPTCOEFF_E_GET(reg)              ((reg & VIN_ECMD_SIGCPTCOEFF_E) >> 13)
#define VIN_ECMD_SIGCPTCOEFF_E_SET(reg, val)         (reg = ((reg & ~VIN_ECMD_SIGCPTCOEFF_E) | (((val) & 0x0001) << 13)))

#define VIN_ECMD_SIGCPTCOEFF_P                       (1 << 14)
#define VIN_ECMD_SIGCPTCOEFF_P_GET(reg)              ((reg & VIN_ECMD_SIGCPTCOEFF_P) >> 14)
#define VIN_ECMD_SIGCPTCOEFF_P_SET(reg, val)         (reg = ((reg & ~VIN_ECMD_SIGCPTCOEFF_P) | (((val) & 0x0001) << 14)))

#define VIN_ECMD_SIGCPTCOEFF_FX_0HZ                  0x8000
#define VIN_ECMD_SIGCPTCOEFF_LEVEL_0DB               0x2C7B

/* access definitions for V8bis */
#define VIN_ECMD_SIGV8BIS_EN                           (1 << 15)
#define VIN_ECMD_SIGV8BIS_EN_GET(reg)                  ((reg & VIN_ECMD_SIGV8BIS_EN) >> 15)
#define VIN_ECMD_SIGV8BIS_EN_SET(reg, val)             (reg = ((reg & ~VIN_ECMD_SIGV8BIS_EN) | (((val) & 0x0001) << 15)))

#define VIN_ECMD_SIGV8BIS_TP                           (1 << 11)
#define VIN_ECMD_SIGV8BIS_TP_GET(reg)                  ((reg & VIN_ECMD_SIGV8BIS_TP) >> 11)
#define VIN_ECMD_SIGV8BIS_TP_SET(reg, val)             (reg = ((reg & ~VIN_ECMD_SIGV8BIS_TP) | (((val) & 0x0001) << 11)))

#define VIN_ECMD_SIGV8BIS_V8BISNR_MASK                   (((1 << 4) - 1) << 0)
#define VIN_ECMD_SIGV8BIS_V8BISNR_GET(reg)               (((reg) >> 0) & ((1 << 4) - 1))
#define VIN_ECMD_SIGV8BIS_V8BISNR_SET(reg, val)          (reg = ( (reg & ~VIN_ECMD_SIGV8BIS_V8BISNR_MASK) | ((((1 << 4) - 1) & (val)) << 0)))

#endif /* _DRV_VINETIC_H */
