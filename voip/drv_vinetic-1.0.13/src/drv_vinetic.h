#ifndef _DRV_VINETIC_H
#define _DRV_VINETIC_H
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
   Module      : drv_vinetic.h
   Date        : 2002-11-18
   Description :
      This file contains VINETIC specific defines as command defines and
      register definitions.
*******************************************************************************/

/** \file
   This file contains the defines specific to the VINETIC
 */

#include "ifx_types.h" /* for IFX_* typedefs */

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
/* Low Level addresses           */
/* ============================= */

/* defines for vinetic low level access */
#define VINETIC_NWD        0x02
#define VINETIC_EOM        0x03
#define VINETIC_EOM_MUX    0x04
#define VINETIC_ISR        0x0C

/* shift factor for Vinetic 1x parallel mux access,
   system dependent.
   Remarks: Please adapt macro in user configuration
            file if necessary.
*/
#ifndef VIN_1X_PARACC_MUX_SHIFT
#define VIN_1X_PARACC_MUX_SHIFT   0
#endif /* VIN_1X_PARACC_MUX_SHIFT */

/* shift factor for Vinetic 2x parallel mux access,
   shift by one to avoid access on odd addresses,
   system dependent.

   Remarks: Please adapt macro in user configuration
            file if necessary.

*/
#ifndef VIN_2X_PARACC_MUX_SHIFT
#define VIN_2X_PARACC_MUX_SHIFT   1
#endif /* VIN_2X_PARACC_MUX_SHIFT */

/* shift factor for Vinetic parallel access,
   exclusive mux access,
   system dependent.

   Remarks: Please adapt macro in user configuration
            file if necessary
*/
#ifndef VIN_PARACC_SHIFT
#define VIN_PARACC_SHIFT          0
#endif /* VIN_PARACC_SHIFT */

/* io region size (should be enough in all modes) */
#define VINETIC_REGION_SIZE   32

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
#define SIG_DTMFREC_EN        0x8000
#define SIG_DTMFREC_ET        0x4000
#define SIG_DTMFREC_AS        0x0010
#define ECMD_SIG_ATD1         (CMD2_MOD_SIG | 0x0500)
#define SIG_ATD1_EN           0x8000
#define ECMD_SIG_ATD2         (CMD2_MOD_SIG | 0x0600)
#define SIG_ATD2_EN           0x8000
#define ECMD_SIG_UTD1         (CMD2_MOD_SIG | 0x0700)
#define SIG_UTD1_EN           0x8000
#define ECMD_SIG_UTD2         (CMD2_MOD_SIG | 0x0800)
#define SIG_UTD2_EN           0x8000
#define ECMD_SIG_CHCONF       (CMD2_MOD_SIG | 0x1000)
#define ECMD_UTG              (CMD2_MOD_SIG | 0x0A00)
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
#define SIG_DTMGGEN_MOD_OF    10
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
#define COD_CH_G723_63_ENC    0x001D
#define COD_CH_G723_53_ENC    0x001C
#define COD_CH_G728_ENC       0x0010
#define COD_CH_G729_ENC       0x0012
#define COD_CH_G711_MLAW_ENC  0x0003
#define COD_CH_G711_ULAW_ENC  0x0003
#define COD_CH_G711_ALAW_ENC  0x0002
#define COD_CH_G726_16_ENC    0x0004
#define COD_CH_G726_24_ENC    0x0005
#define COD_CH_G726_32_ENC    0x0006
#define COD_CH_G726_40_ENC    0x0007
#define COD_CH_G729_E_ENC     0x0013
#define COD_GAIN_0DB          0x60
#define COD_CH_SC             0x0100
#define COD_CH_CNG            0x2000

#define ECMD_COD_AGC          (CMD2_MOD_COD | 0x0300)

#define ECMD_COD_RTP          (CMD2_MOD_COD | 0x1000)
#define ECMD_COD_CHRTP        (CMD2_MOD_COD | 0x1100)
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
#define ECMD_UTD_COEF         (CMD2_MOD_RES | 0x0E00)
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


/* ============================= */
/* Short Commands                */
/* ============================= */

/* short commands for  vinetic access */
/* read interrupt status register */
#define RIR       0x0000
#define RSREG     0x0010
#define RHWSR     0x0020
#define RSRGPIO   0x0030
#define RBXSR     0x0040
/* read all channel specific interrupt status registers */
#define RISR      0x0090
#define RIHWSR    0x00A0
#define RISRGPIO  0x00B0
#define RIBXSR    0x00C0
#define RFIBXMS   0x0100
#define ROBXML    0x0110
#define RPOBX     0x0120
#define RCOBX     0x0130
#define WMAXCBX   0x0140
#define WMINCBX   0x0150
#define WLEMP     0x0230
#define WSTEDSP   0x0240
#define WLPMP     0x0300
#define WSWRST    0x0310
#define WRESYNC   0x0320
#define RPHIERR   0x0330
#define WPHIERR   0x0330

/* Note: The following three states are not valid for every linemode! */
#define HIT                0x0010
#define HIR                0x0020
#define HIRT               0x0030

/* ============================= */
/* Short Commands Operating Mode */
/* ============================= */

#define POW_DOWN           0x1000
#define RNG_PAUSE          0x1100
#define RNG_PAUSE_HIT      0x1110
#define RNG_PAUSE_HIR      0x1120
#define RNG_PAUSE_HIRT     0x1130
#define RNG_PAUSE_BOOST    0x1140
#define RNG_PAUSE_TIP_GND  0x1150
#define RNG_PAUSE_RNG_GND  0x1160
#define ACT_HIGH           0x1200
#define ACT_HIT            0x1210
#define ACT_HIR            0x1220
#define ACT_HIRT           0x1230
#define ACT_BOOST          0x1240
#define ACT_TIP_GND        0x1250
#define ACT_RNG_GND        0x1260
#define ACT_LOW            0x1280
#define ACT_TEST_IN        0x12A0 /* SLIC S-MAX mode */
#define SLEEP_BATH_BGND    0x1300
#define SLEEP_BATR_BGND    0x1340
#define GND_START          0x1400
#define ACT_RNG            0x1500
#define RNG_HIT            0x1510
#define RNG_HIR            0x1520
#define RNG                0x1540
#define RNG_TIP_GND        0x1550
#define RNG_RNG_GND        0x1560
#define ACT_HIGH_MET       0x1600
#define ACT_MET_HIT        0x1610
#define ACT_MET_HIR        0x1620
#define ACT_RNG_MET        0x1640
#define ACT_MET_TIP_GND    0x1650
#define ACT_MET_RNG_GND    0x1660
#define ACT_LOW_MET        0x1680
#define BATH_BGND          0x1700
#define BATR_BGND          0x1740
#define PDNR_SWITCH        0x1760 /* SLIC S-MAX mode */

/* ============================= */
/* EDSP Definitions              */
/* ============================= */

/* short commands for  vinetic access */
/* you can find the related #defines in VINETIC_io.h */
#define SC_RIR                (CMD1_RD | CMD1_SC | RIR)
#define SC_RSR                (CMD1_RD | CMD1_SC | RSREG)
#define SC_RHWSR              (CMD1_RD | CMD1_SC | RHWSR)
#define SC_RSRGPIO            (CMD1_RD | CMD1_SC | RSRGPIO)
#define SC_RBXSR              (CMD1_RD | CMD1_SC | RBXSR)
#define SC_RISR               (CMD1_RD | CMD1_SC | RISR)
#define SC_RIHWSR             (CMD1_RD | CMD1_SC | RIHWSR)
#define SC_RISRGPIO           (CMD1_RD | CMD1_SC | RISRGPIO)
#define SC_RIBXSR             (CMD1_RD | CMD1_SC | RIBXSR)
#define SC_RFIBXMS            (CMD1_RD | CMD1_SC | RFIBXMS)
#define SC_ROBXML             (CMD1_RD | CMD1_SC | ROBXML)
#define SC_RPOBX              (CMD1_RD | CMD1_SC | RPOBX)
#define SC_RCOBX              (CMD1_RD | CMD1_SC | RCOBX)
#define SC_WMAXCBX            (CMD1_WR | CMD1_SC | WMAXCBX)
#define SC_WMINCBX            (CMD1_WR | CMD1_SC | WMINCBX)
#define SC_WLEMP              (CMD1_WR | CMD1_SC | WLEMP)
#define SC_WSTEDSP            (CMD1_WR | CMD1_SC | WSTEDSP)
#define SC_WLPMP              (CMD1_WR | CMD1_SC | WLPMP)
#define SC_WSWRST             (CMD1_WR | CMD1_SC | WSWRST)
#define SC_WRESYNC            (CMD1_WR | CMD1_SC | WRESYNC)
#define SC_RPHIERR            (CMD1_RD | CMD1_SC | RPHIERR)
#define SC_WPHIERR            (CMD1_RD | CMD1_SC | WPHIERR)

/* ============================= */
/* Short Commands Operating Mode */
/* ============================= */
#define SC_POW_DOWN           (CMD1_WR | CMD1_SC | POW_DOWN)
#define SC_RNG_PAUSE_HIT      (CMD1_WR | CMD1_SC | RNG_PAUSE_HIT)
#define SC_RNG_PAUSE_HIR      (CMD1_WR | CMD1_SC | RNG_PAUSE_HIR)
#define SC_RNG_PAUSE_HIRT     (CMD1_WR | CMD1_SC | RNG_PAUSE_HIRT)
#define SC_RNG_PAUSE          (CMD1_WR | CMD1_SC | RNG_PAUSE)
#define SC_RNG_PAUSE_TIP_GND  (CMD1_WR | CMD1_SC | RNG_PAUSE_TIP_GND)
#define SC_RNG_PAUSE_RNG_GND  (CMD1_WR | CMD1_SC | RNG_PAUSE_RNG_GND)
#define SC_RNG_PAUSE_BOOST    (CMD1_WR | CMD1_SC | RNG_PAUSE_BOOST)
#define SC_ACT_HIGH           (CMD1_WR | CMD1_SC | ACT_HIGH)
#define SC_ACT_HIT            (CMD1_WR | CMD1_SC | ACT_HIT)
#define SC_ACT_HIR            (CMD1_WR | CMD1_SC | ACT_HIR)
#define SC_ACT_HIRT           (CMD1_WR | CMD1_SC | ACT_HIRT)
#define SC_ACT_BOOST          (CMD1_WR | CMD1_SC | ACT_BOOST)
#define SC_ACT_RNG            (CMD1_WR | CMD1_SC | ACT_RNG)
#define SC_ACT_TIP_GND        (CMD1_WR | CMD1_SC | ACT_TIP_GND)
#define SC_ACT_RNG_GND        (CMD1_WR | CMD1_SC | ACT_RNG_GND)
#define SC_ACT_LOW            (CMD1_WR | CMD1_SC | ACT_LOW)
#define SC_ACT_TEST_IN        (CMD1_WR | CMD1_SC | ACT_TEST_IN)
#define SC_SLEEP_BATH_BGND    (CMD1_WR | CMD1_SC | SLEEP_BATH_BGND)
#define SC_SLEEP_BATR_BGND    (CMD1_WR | CMD1_SC | SLEEP_BATR_BGND)
#define SC_GND_START          (CMD1_WR | CMD1_SC | GND_START)
#define SC_RNG_HIT            (CMD1_WR | CMD1_SC | RNG_HIT)
#define SC_RNG_HIR            (CMD1_WR | CMD1_SC | RNG_HIR)
#define SC_RNG                (CMD1_WR | CMD1_SC | RNG)
#define SC_RNG_TIP_GND        (CMD1_WR | CMD1_SC | RNG_TIP_GND)
#define SC_RNG_RNG_GND        (CMD1_WR | CMD1_SC | RNG_RNG_GND)
#define SC_ACT_HIGH_MET       (CMD1_WR | CMD1_SC | ACT_HIGH_MET)
#define SC_ACT_MET_HIT        (CMD1_WR | CMD1_SC | ACT_MET_HIT)
#define SC_ACT_MET_HIR        (CMD1_WR | CMD1_SC | ACT_MET_HIR)
#define SC_ACT_RNG_MET        (CMD1_WR | CMD1_SC | ACT_RNG_MET)
#define SC_ACT_MET_TIP_GND    (CMD1_WR | CMD1_SC | ACT_MET_TIP_GND)
#define SC_ACT_MET_RNG_GND    (CMD1_WR | CMD1_SC | ACT_MET_RNG_GND)
#define SC_ACT_LOW_MET        (CMD1_WR | CMD1_SC | ACT_LOW_MET)
#define SC_BATH_BGND          (CMD1_WR | CMD1_SC | BATH_BGND)
#define SC_BATR_BGND          (CMD1_WR | CMD1_SC | BATR_BGND)
#define SC_PDNR_SWITCH        (CMD1_WR | CMD1_SC | PDNR_SWITCH)

/* ============================= */
/* Register Access               */
/* ============================= */

/* Bit definitions of Interrupt Register (IR) */
#define IR_ISRCH                       0xFF00
#define IR_ISRE7                       0x8000
#define IR_ISRE6                       0x4000
#define IR_ISRE5                       0x2000
#define IR_ISRE4                       0x1000
#define IR_ISR3                        0x0800
#define IR_ISR2                        0x0400
#define IR_ISR1                        0x0200
#define IR_ISR0                        0x0100
#define IR_RDYQ                        0x0080
#define IR_RESET                       0x0010
#define IR_GPIO                        0x0004
#define IR_MBX_EVT                     0x0002
#define IR_HW_STAT                     0x0001

/* Bit definition of HWSR1 */
#define  HWSR1_HW_ERR                  0x1
#define  HWSR1_WOKE_UP                 0x4
#define  HWSR1_MCLK_FAIL               0x8
#define  HWSR1_EDSP_WD_FAIL            0x80
#define  HWSR1_TXA_CRASH               0x100
#define  HWSR1_TXB_CRASH               0x200
#define  HWSR1_CLK_FAIL                0x400
#define  HWSR1_SYNC_FAIL               0x800

/* Bit definition of HWSR2 */
#define  HWSR2_EDSP_MIPS_OL            0x1
#define  HWSR2_EDSP_CS_FAIL            0x2
#define  HWSR2_DL_RDY                  0x100

/* Bit definition of BXSR1 */
/* Command Error */
#define  BXSR1_CERR                    0x100

/* Bit definition of BXSR2 */
/* Mailbox Empty */
#define  BXSR2_MBX_EMPTY               0x1
/* Command In-Box Overflow */
#define  BXSR2_CIBX_OF                 0x2
/* Packet In-Box Overflow */
#define  BXSR2_PIBX_OF                 0x4
/* Command Out-Box Data */
#define  BXSR2_COBX_DATA               0x8
/* Packet Out-Box Data*/
#define  BXSR2_POBX_DATA               0x10
#define  BXSR2_HOST_ERR                0x20

/* Bit definition of SRGPIO */
#define  SRGPIO_GPIO0                  0x1
#define  SRGPIO_GPIO1                  0x2
#define  SRGPIO_GPIO2                  0x4
#define  SRGPIO_GPIO3                  0x8
#define  SRGPIO_GPIO4                  0x10
#define  SRGPIO_GPIO5                  0x20
#define  SRGPIO_GPIO6                  0x40
#define  SRGPIO_GPIO7                  0x80

/*Bit Definition of SRE1 */
#define SRE1_DTMFR_DT                  0x8000
#define SRE1_DTMFR_PDT                 0x4000
#define SRE1_DTMFR_DTC                 0x3C00
#define SRE1_UTD1_OK                   0x0200
#define SRE1_UTD2_OK                   0x0100
#define SRE1_CPT                       0x0080
#define SRE1_V8BIS                     0x0080
#define SRE1_CIDR_OF                   0x0040
#define SRE1_DTMFG_BUF                 0x0020
#define SRE1_DTMFG_REQ                 0x0010
#define SRE1_DTMFG_ACT                 0x0008
#define SRE1_DTMF_MASK                 0x0038
#define SRE1_CIS_BUF                   0x0004
#define SRE1_CIS_REQ                   0x0002
#define SRE1_CIS_ACT                   0x0001
#define SRE1_CID_MASK                  0x0007
#define SRE1_UTG_ACT                   0x0008 /* overlaid with SRE1_DTMFG_ACT */
/* DTMF Keys Masks */
#define SRE1_DTMFR_DTC_0               0x0000
#define SRE1_DTMFR_DTC_1               0x0400
#define SRE1_DTMFR_DTC_2               0x0800
#define SRE1_DTMFR_DTC_3               0x0C00
#define SRE1_DTMFR_DTC_4               0x1000
#define SRE1_DTMFR_DTC_5               0x1400
#define SRE1_DTMFR_DTC_6               0x1800
#define SRE1_DTMFR_DTC_7               0x1C00
#define SRE1_DTMFR_DTC_8               0x2000
#define SRE1_DTMFR_DTC_9               0x2400
#define SRE1_DTMFR_DTC_STAR            0x2800
#define SRE1_DTMFR_DTC_HASH            0x2C00
#define SRE1_DTMFR_DTC_A               0x3000
#define SRE1_DTMFR_DTC_B               0x3400
#define SRE1_DTMFR_DTC_C               0x3800
#define SRE1_DTMFR_DTC_D               0x3C00
/* Bit Definition of SRE2 */
#define SRE2_ATD1_DT                   0x8000
#define SRE2_ATD1_NPR                  0x6000
#define SRE2_ATD1_AM                   0x1000
#define SRE2_ATD2_DT                   0x0800
#define SRE2_ATD2_NPR                  0x0600
#define SRE2_ATD2_AM                   0x0100
#define SRE2_EPOU_STAT                 0x0080
#define SRE2_ETU_OF                    0x0040
#define SRE2_PVPUOF                    0x0020
#define SRE2_VPOU_STAT                 0x0010
#define SRE2_VPOU_JBL                  0x0008
#define SRE2_VPOU_JBH                  0x0004
#define SRE2_DEC_ERR                   0x0002
#define SRE2_EPOU_TRIG                 0x0002
#define SRE2_DEC_CHG                   0x0001
#define SRE2_FDP_REQ                   SRE2_DEC_CHG
#define SRE2_FDP_ERR                   SRE2_PVPUOF
/* Reversal Phase Masks ATD1 */
#define SRE2_ATD1_NPR_NO_REV           0x0000
#define SRE2_ATD1_NPR_1_REV            0x2000
#define SRE2_ATD1_NPR_2_REV            0x4000
#define SRE2_ATD1_NPR_3_REV            0x6000
/* Reversal Phase Mask ATD2 */
#define SRE2_ATD2_NPR_NO_REV           0x0000
#define SRE2_ATD2_NPR_1_REV            0x0200
#define SRE2_ATD2_NPR_2_REV            0x0400
#define SRE2_ATD2_NPR_3_REV            0x0600

/*Bit Definition of SRS1*/
#define SRS1_RAMP_READY                0x4000
#define SRS1_HOOK                      0x2000
#define SRS1_GNDK                      0x1000
#define SRS1_GNDKP                     0x0800
#define SRS1_GNDKH                     0x0400
#define SRS1_ICON                      0x0200
#define SRS1_VTRLIM                    0x0100
#define SRS1_LM_THRES                  0x0040
#define SRS1_LM_OK                     0x0020
#define SRS1_DU_IO                     0x001F

/*Bit Definition of SRS2*/
#define SRS2_OTEMP                     0x0008
#define SRS2_CS_FAIL_CRAM              0x0004
#define SRS2_CS_FAIL_DSP               0x0002
#define SRS2_CS_FAIL_DCCTL             0x0001

/* Bit definition of FCONF */
#define FCONF_PRG_FUSE       0x1
/* Bit definition of GCR1 */
#define GCR1_DD_GPIO0                  0x0001
#define GCR1_DD_GPIO1                  0x0002
#define GCR1_DD_GPIO2                  0x0004
#define GCR1_DD_GPIO3                  0x0008
#define GCR1_DD_GPIO4                  0x0010
#define GCR1_DD_GPIO5                  0x0020
#define GCR1_DD_GPIO6                  0x0040
#define GCR1_DD_GPIO7                  0x0080
#define GCR1_HOST_GPIO0                0x0100
#define GCR1_HOST_GPIO1                0x0200
#define GCR1_HOST_GPIO2                0x0400
#define GCR1_HOST_GPIO3                0x0800
#define GCR1_HOST_GPIO4                0x1000
#define GCR1_HOST_GPIO5                0x2000
#define GCR1_HOST_GPIO6                0x4000
#define GCR1_HOST_GPIO7                0x8000

/* Bit definition of GCR2 */
#define GCR2_OEN_GPIO0                 0x0001
#define GCR2_OEN_GPIO1                 0x0002
#define GCR2_OEN_GPIO2                 0x0004
#define GCR2_OEN_GPIO3                 0x0008
#define GCR2_OEN_GPIO4                 0x0010
#define GCR2_OEN_GPIO5                 0x0020
#define GCR2_OEN_GPIO6                 0x0040
#define GCR2_OEN_GPIO7                 0x0080
#define GCR2_IEN_GPIO0                 0x0100
#define GCR2_IEN_GPIO1                 0x0200
#define GCR2_IEN_GPIO2                 0x0400
#define GCR2_IEN_GPIO3                 0x0800
#define GCR2_IEN_GPIO4                 0x1000
#define GCR2_IEN_GPIO5                 0x2000
#define GCR2_IEN_GPIO6                 0x4000
#define GCR2_IEN_GPIO7                 0x8000

/* defines in DSCR Register */
#define DSCR_TG1_EN                    0x0001
#define DSCR_TG2_EN                    0x0002
#define DSCR_TG_MASK                   0x0003
#define DSCR_PTG                       0x0004
#define DSCR_COR8                      0x0008
#define DSCR_DG_KEY_MASK               0x00F0

/* defines in AUTOMOD Register */
#define AUTO_BAT_OFF                   0x0000
#define AUTO_BAT_ON                    0x0002
#define AUTO_BAT_MASK                  0x0002

/* defines in BCR1 register */
#define BCR1_DUP_GNDK_MASK             0xF000
#define BCR1_LMAC64                    0x0800
#define BCR1_PRAM_DCC                  0x0400
#define BCR1_HIM_RES                   0x0200
#define BCR1_HIM_AN                    0x0100
#define BCR1_LMABS_EN                  0x0080
#define BCR1_DC_HOLD                   0x0040
#define BCR1_EN_IK                     0x0020
#define BCR1_TEST_EN                   0x0010
#define BCR1_SEL_SLIC_MASK             0x000F

/* defines in BCR2 Register */
#define BCR2_LPRX_CR                   0x8000
#define BCR2_CRAM_EN                   0x4000
#define BCR2_SOFT_DIS                  0x2000
#define BCR2_TTX_EN                    0x1000
#define BCR2_TTX_12K                   0x0800
#define BCR2_REV_POL                   0x0400
#define BCR2_AC_SHORT_EN               0x0200
#define BCR2_TH_DIS                    0x0080
#define BCR2_IM_DIS                    0x0040
#define BCR2_AX_DIS                    0x0020
#define BCR2_AR_DIS                    0x0010
#define BCR2_FRX_DIS                   0x0008
#define BCR2_FRR_DIS                   0x0004
#define BCR2_HPX_DIS                   0x0002
#define BCR2_HPR_DIS                   0x0001

/* TSTR2 bits */
#define TSTR2_COX_16                   0x0001
#define TSTR2_COR_64                   0x0002
#define TSTR2_OPIM_HW                  0x0004
#define TSTR2_OPIM_AN                  0x0008
#define TSTR2_AC_XGAIN                 0x0040
#define TSTR2_AC_RGAIN                 0x0080
#define TSTR2_AC_SHORT                 0x0100
#define TSTR2_AC_DLB_8K                0x0200
#define TSTR2_AC_DLB_32K               0x0400
#define TSTR2_AC_DLB_128K              0x0800
#define TSTR2_AC_DLB_16M               0x1000
#define TSTR2_AC_ALB_16M               0x2000
#define TSTR2_AC_ALB_PP                0x4000

/* TSTR 3 bits */
#define TSTR3_DC_LP1_DIS               0x0100
#define TSTR3_DC_LP2_DIS               0x0200
#define TSTR3_DC_DIS                   0x0400

/* defines in RTR Register */
#define RTR_RTR_SENSE                  0x0080   /* External Ringburst Sense */
#define RTR_RTR_SEL                    0x0040   /* Ring Trip Select */
#define RTR_RTR_FAST                   0x0020   /* Ring Trip Fast */
#define RTR_PCM2DC                     0x0010   /* PCM to DC */
#define RTR_RING_OFFSET                0x000C   /* Ring Offset Selection Mask */
#define RTR_RO0                        0x0000   /* R00 - see users manual */
#define RTR_RO1                        0x0004   /* R01 - see users manual */
#define RTR_RO2                        0x0008   /* R02 - see users manual */
#define RTR_RO3                        0x000C   /* R03 - see users manual */
#define RTR_ASYNC_H_R                  0x0002   /* Asynchronous External Ringing */
#define RTR_REXT_EN                    0x0001   /* External Ring Enable */

/*defines in LMCR Register             */
#define LMCR_LM_ITIME_MASK             0xF000 /* Level Metering Integration Time */
#define LMCR_LM_IT_16                  0x0000 /* Integration Time = 16 ms */
#define LMCR_LM_IT_32                  0x1000 /* Integration Time = 32 ms */
#define LMCR_LM_IT_48                  0x2000 /* Integration Time = 48 ms */
#define LMCR_LM_IT_64                  0x3000 /* Integration Time = 64 ms */
#define LMCR_LM_IT_80                  0x4000 /* Integration Time = 80 ms */
#define LMCR_LM_IT_96                  0x5000 /* Integration Time = 96 ms */
#define LMCR_LM_IT_112                 0x6000 /* Integration Time = 112 ms */
#define LMCR_LM_IT_128                 0x7000 /* Integration Time = 128 ms */
#define LMCR_LM_IT_144                 0x8000 /* Integration Time = 144 ms */
#define LMCR_LM_IT_160                 0x9000 /* Integration Time = 160 ms */
#define LMCR_LM_IT_176                 0xA000 /* Integration Time = 176 ms */
#define LMCR_LM_IT_192                 0xB000 /* Integration Time = 192 ms */
#define LMCR_LM_IT_208                 0xC000 /* Integration Time = 208 ms */
#define LMCR_LM_IT_224                 0xD000 /* Integration Time = 224 ms */
#define LMCR_LM_IT_240                 0xE000 /* Integration Time = 240 ms */
#define LMCR_LM_IT_256                 0xF000 /* Integration Time = 256 ms */
#define LMCR_LM_EN                     0x0800 /* Level Metering Enable */
#define LMCR_LM2PCM                    0x0400 /* Level Meter to PCM */
#define LMCR_LM_ONCE                   0x0200 /* Level Meter Once */
#define LMCR_DC_AD16                   0x0100 /* Additional gain factor 16 */
#define LMCR_LM_FILT_SEL               0x0080 /* Select Level Metering Notch or Bandpass Filter */
#define LMCR_LM_FILT                   0x0040 /* Enable Level Metering Filter */
#define LMCR_LM_RECT                   0x0020 /* Enable Level Metering Rectifier in DC level meter */
#define LMCR_LM_RAMP_EN                0x0010 /* Ramp enable */
#define LMCR_LM_SEL_MASK               0x000F /* Level Metering Source Select */
#define LMCR_LM_SEL_AC_TX              0x0000 /* AC level metering in transmit */
#define LMCR_LM_SEL_TTX_REAL           0x0001 /* Real part of TTX */
#define LMCR_LM_SEL_TTX_IMG            0x0002 /* Imaginay part of TTX */
#define LMCR_LM_SEL_U_IO3_IO2          0x0003 /* Voltage on IO3 - IO2 */
#define LMCR_LM_SEL_U_DC_DCN_DCP       0x0004 /* DC out voltage on DCN-DCP */
#define LMCR_LM_SEL_I_DC_IT            0x0005 /* DC current on IT */
#define LMCR_LM_SEL_AC_RX              0x0006 /* AC level metering in receive */
#define LMCR_LM_SEL_AC_RX_TX           0x0007 /* AC level metering in receive and transmit */
#define LMCR_LM_SEL_U_IO4_IO2          0x0008 /* Voltage on IO4 - IO2 */
#define LMCR_LM_SEL_I_DC_IL            0x0009 /* DC current on IL */
#define LMCR_LM_SEL_U_IO2              0x000A /* Voltage on IO2 */
#define LMCR_LM_SEL_U_IO4              0x000B /* Voltage on IO4 */
#define LMCR_LM_SEL_U_IO3              0x000C /* Voltage on IO3 */
#define LMCR_LM_SEL_VDD_3              0x000D /* Vdd / 3 */
#define LMCR_LM_SEL_VDD                0x000D /* same as above */
#define LMCR_LM_SEL_DC_PREFI_OFFSET    0x000E /* Offset of DC-Prefi (short circuit on DC-Prefi input */
#define LMCR_LM_SEL_U_IO4_IO3          0x000F /* Voltage on IO4 - IO3 */

#ifdef VIN_V21_SUPPORT
#define LMCR_LM_INT_OFF                0x0000 /* Level Metering Integration Setup */
#define LMCR_LM_INT_ONCE               0x0002
#define LMCR_LM_INT_CONTINOUSLY        0x0003
#define LMCR_LM_MAG_OFF                0x0000 /* Level Metering Magnitude Processing */
#define LMCR_LM_MAG_SQUARE             0x0002
#define LMCR_LM_MAG_RECITFY            0x0003
#define LMCR2_LM2PCM_OFF               0x0000 /* Level Metering Result via PCM */
#define LMCR2_LM2PCM_ON                0x0001
typedef enum {
  LMCR2_LM_ITS_LM_ITIME             =  0x0000,/* Level Metering Integration Time Source */
  LMCR2_LM_ITS_RING_PERIOD          =  0x0001
} LMCR2_LM_ITS;
typedef enum {
  LMCR2_LM_DCTX8K_2K                =  0x0000,/* Level Metering Path Selection */
  LMCR2_LM_DCTX8K_8K                =  0x0001
} LMCR2_LM_DCTX8K;
#define LMCR2_LM_FILT_SEL_BYPASS       0x0000 /* Level Metering Filter Selection */
#define LMCR2_LM_FILT_SEL_BANDPASS     0x0004
#define LMCR2_LM_FILT_SEL_NOTCH        0x0005
#define LMCR2_LM_FILT_SEL_LOWPASS      0x0006
#define LMCR2_LM_FILT_SEL_HIGHPASS     0x0007

#endif /* VIN_V21_SUPPORT */

/* ============================= */
/* Handshake Register Defines    */
/* ============================= */

#define FIBXMS_CBOX                    0xff00
#define FIBXMS_PBOX                    0x00ff
#define OBXML_CDATA                    0x1F00
#define OBXML_PDATA                    0x00FF

/* ============================= */
/* PHI Register Defines          */
/* ============================= */

#define IOP_GCR1                       0x0047
#define IOP_GCR2                       0x0048
#define PHICHKR                        0x0049
#define MISCCMD                        0x004A

/* ============================= */
/* SOP Register Defines          */
/* ============================= */

typedef enum
{
   SOP_OFFSET_DCCHKR                =  0x0000,
   SOP_OFFSET_DSCHKR                =  0x0001,
   SOP_OFFSET_CCR                   =  0x0002,
   SOP_OFFSET_LMRES                 =  0x0003,
   SOP_OFFSET_CCHKR                 =  0x0004,
   SOP_OFFSET_IOCTL1                =  0x0005,
   SOP_OFFSET_IOCTL2                =  0x0006,
   SOP_OFFSET_BCR1                  =  0x0007,
   SOP_OFFSET_BCR2                  =  0x0008,
   SOP_OFFSET_DSCR                  =  0x0009,
   SOP_OFFSET_LMCR                  =  0x000A,
   SOP_OFFSET_RTR                   =  0x000B,
   SOP_OFFSET_OFR                   =  0x000C,
   SOP_OFFSET_AUTOMOD               =  0x000D,
   SOP_OFFSET_TSTR1                 =  0x000E,
   SOP_OFFSET_TSTR2                 =  0x000F,
   SOP_OFFSET_TSTR3                 =  0x0010
} VINETIC_SOP_REGISTER_OFFSETS;

#define CCR_JUMP_AC3                   0x0010

/* Io Ctl1 defines */
#define IOCTL1_INEN_IO2                0x0400
#define IOCTL1_INEN_IO3                0x0800
#define IOCTL1_INEN_IO4                0x1000

/* Basis Conf Reg settings */
#define BCR_NR                         2
#define BCR_CMD_LEN                    3

/*Offset COP-Registers (8-bits)*/
/* CRAM register number */
#define CRAM_REG_NR                    28
#define CRAM_CMD_LEN                   6
#define CRAM_LPR                       0x0000
#define CRAM_LPX                       0x0004
#define CRAM_AR                        0x0008
#define CRAM_AX                        0x000C
#define CRAM_FRR                       0x0010
#define CRAM_FRX                       0x0014
#define CRAM_PTG1                      0x0018
#define CRAM_PTG2                      0x001C
#define CRAM_TH1                       0x0020
#define CRAM_TH2                       0x0024
#define CRAM_TH3                       0x0028
#define CRAM_IM1_F                     0x002C
#define CRAM_IM2_F                     0x0030
#define CRAM_FBIQ1                     0x0034
#define CRAM_FBIQ2                     0x0038
#define CRAM_TTX                       0x003C
#define CRAM_LM_AC                     0x0040
#define CRAM_TTX2                      0x0044
#define CRAM_RINGF                     0x0048
#define CRAM_RINGO                     0x004C
#define CRAM_RGD                       0x004F
#define CRAM_RINGI                     0x0050
#define CRAM_DCF1                      0x0054
#define CRAM_VK1                       0x0055
#define CRAM_IK1                       0x0056
#define CRAM_DCF2                      0x0058
#define CRAM_DCF3                      0x005C
#define CRAM_VRTL                      0x005F
#define CRAM_HF1                       0x0060
#define CRAM_HF2                       0x0064
#define CRAM_RAMPF                     0x0068
#define CRAM_CRAMP                     0x006A
#define CRAM_DC_RES                    0x006C
#define CRAM_ROM1                      0x0070
#define CRAM_ROM2                      0x0074
#define CRAM_ROM3                      0x0078
#define CRAM_ROM4                      0x007C

/* COP (CRAM)Register Masks */
#define CRAM_BPQ1_MASK                 0x00F0
#define CRAM_BPQ2_MASK                 0x000F
#define CRAM_BPQ3_MASK                 0x000F

/*Offset IOP-Registers (16-bits)*/

/* Mask Register Offsets */
#define MRSRE1                         0x0000
#define MRSRE2                         0x0001
#define MRSRS1                         0x0002
#define MRSRS2                         0x0003
#define MRHWSR1                        0x0012
#define MRHWSR2                        0x0013
#define MRBXSR1                        0x0016
#define MRBXSR2                        0x0017
#define IOP_OFFSET_MRSRGPIO                        0x001A
#define MFSRE1                         0x0060
#define MFSRE2                         0x0061
#define MFSRS1                         0x0062
#define MFSRS2                         0x0063
#define MFHWSR1                        0x0072
#define MFHWSR2                        0x0073
#define MFBXSR1                        0x0076
#define MFBXSR2                        0x0077
#define IOP_OFFSET_MFSRGPIO                        0x007A

/* Common Registers */
#define OPMOD_SRC_IOP                  0x0020
#define OPMOD_CUR_IOP                  0x0021
#define OPMOD_CUR_MODE_MASK            0x0F00
#define OPMOD_CUR_SUBMODE_MASK         0x00F0
#define OPMOD_CUR_DCCTL_MASK           0x000F
#define FUSE1_IOP                      0x0022
#define FUSE2_IOP                      0x0023
#define CFUSE1_IOP                     0x0030
#define CFUSE2_IOP                     0x0031

#define OPMOD_MODE_CUR_PDNH            0x0
#define OPMOD_MODE_CUR_RING_PAUSE      0x1
#define OPMOD_MODE_CUR_ACTIVE          0x2
#define OPMOD_MODE_CUR_SLEEP_PWDN      0x3
#define OPMOD_MODE_CUR_RINGING         0x5
#define OPMOD_MODE_CUR_ACTIVE_BOOSTED  0x6
#define OPMOD_MODE_CUR_PDNR            0x7
#define OPMOD_SUBMOD_CUR_NORMAL        0x0
#define OPMOD_SUBMOD_CUR_HIT           0x1
#define OPMOD_SUBMOD_CUR_HIR           0x2
#define OPMOD_SUBMOD_CUR_HIRT          0x3

#define REVISION                       0x0040
#define REVISION_REV                   0x00ff
#define REVISION_TYPE                  0xf000
#define REVISION_TYPE_S                0x0000
#define REVISION_TYPE_M                0x1000
#define REVISION_TYPE_VIP              0x2000
#define REVISION_TYPE_C                0x4000
#define REVISION_ANACHAN               0x0f00

#define CHIPID1                        0x0041
#define CHIPID2                        0x0042
#define CHIPID3                        0x0043
#define GCONF                          0x0044
#define EDSPFUSE                       0x0045
#define PLLCTRLE                       0x0046

/* VINETIC Interrupts events */
enum DEV_EVENTS
{
   /* HWSR1 events flag */
   EV_HW1STAT = 0,
   /* HWSR2 events flag */
   EV_HW2STAT,
   /* Mailbox event flag */
   EV_MBX,
   /* Mailbox empty event flag */
   EV_MBXEMPTY,
   /* Packet outbox event flag */
   EV_MBXPOBX,
   /* GPIO events flag */
   EV_GPIO,
   /* last event */
   MAX_IRQ_EVENTS
};

/* Channel Event IDs */
enum CH_EVENTS
{
   /* SRE1 events flag */
   EV_SRE1,
   /* SRE2 events flag */
   EV_SRE2,
   /* SRS1 events flag */
   EV_SRS1,
   /* SRS1 events flag */
   EV_SRS2,
   /* last event */
   MAX_CH_EVENTS
};

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
/* CRAM Coefficients Module      */
/* ============================= */

/* ============================= */
/* Global function declaration   */
/* ============================= */

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

struct _VINETIC_DEVICE;
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} GCR1_REG_TYPE;
#define GCR1 pDev->regGCR1
#define GCR1_REG &pDev->regGCR1

typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} GCR2_REG_TYPE;
#define GCR2 pDev->regGCR2
#define GCR2_REG &pDev->regGCR2

typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} MRSRGPIO_REG_TYPE;
#define MRSRGPIO pDev->regMRSRGPIO
#define MRSRGPIO_REG &pDev->regMRSRGPIO

typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} MFSRGPIO_REG_TYPE;
#define MFSRGPIO pDev->regMFSRGPIO
#define MFSRGPIO_REG &pDev->regMFSRGPIO


/* ************************************************************************** *
   Channel Specific register definitions
 * ************************************************************************** */

typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} REG_TYPE;

/******************************************************************************
 ** LMRES, Level Metering Result
 **    LM-RES[15:0] - LM result (selected by the LM-SEL bits in the LMCR register)
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_int16_t store;
   IFX_int16_t value;
} LMRES_REG_TYPE;

#define LMRES pCh->regLMRES
#define LMRES_REG &pCh->regLMRES

/******************************************************************************
 ** CCHKR, CRAM Checksum Register
 **    C-CHECK[15:0] - CRAM checksum
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} CCHKR_REG_TYPE;

#define CCHKR pCh->regCCHKR
#define CCHKR_REG &pCh->regCCHKR

/******************************************************************************
 ** IOCTL1, I/O Control Register 1
 **    INEN-IO4 - Enables input for programmable I/O pin IO4 with analog functionality.
 **    INEN-IO3 - Enables input for programmable I/O pin IO3 with analog functionality.
 **    INEN-IO2 - Enables input for programmable I/O pin IO2 with analog functionality.
 **    INEN-IO1 - Enables input for programmable I/O pin IO1.
 **    INEN-IO0 - Enables input for programmable I/O pin IO0.
 **    OEN-IO4 - Enables output driver of the IO4 pin with analog functionality.
 **    OEN-IO3 - Enables output driver of the IO3 pin with analog functionality.
 **    OEN-IO2 - Enables output driver of the IO2 pin with analog functionality.
 **    OEN-IO1 - Enables output driver of the IO1 pin.
 **    OEN-IO0 - Enables output driver of the IO0 pin.
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} IOCTL1_REG_TYPE;

#define IOCTL1 pCh->regIOCTL1
#define IOCTL1_REG &pCh->regIOCTL1

/******************************************************************************
 ** IOCTL2, I/O Control Register 2
 **    DUP-OVT[2:0] - Data Upstream Persistence Counter end value. Restricts the rate of interrupts generated by the OTEMP bit in the Status Register for Analog-Line-Module Interrupts 2 (SRS2). The interval is programmable from 1 ms to 57 ms in steps of 8 ms (reset value is 1 ms).
 **    DD-IO4 - Value for the programmable I/O pin IO4 if programmed as an output pin.
 **    DD-IO3 - Value for the programmable I/O pin IO3 if programmed as an output pin.
 **    DD-IO2 - Value for the programmable I/O pin IO2 if programmed as an output pin.
 **    DD-IO1 - Value for the programmable I/O pin IO1 if programmed as an output pin.
 **    DD-IO0 - Value for the programmable I/O pin IO0 if programmed as an output pin.
 **    DUP[3:0] - Data Upstream Persistence Counter end value. Restricts the rate of interrupts generated by the HOOK bit in the interrupt register ISR. The interval is programmable from 1 ms to 16 ms in steps of 1 ms (reset value is 10 ms).
 **    DUP-IO[3:0] - Data Upstream Persistence Counter end value for the I/O pins whenused as digital input pins.and the bits ICON and VTRLIM in register ISR
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} IOCTL2_REG_TYPE;

#define IOCTL2 pCh->regIOCTL2
#define IOCTL2_REG &pCh->regIOCTL2

/******************************************************************************
 ** BCR1, Basic Configuration Register 1
 **    DUP-GNDK[15:12] - Data Upstream Persistence Counter Groundkey
 **    LMAC64 - Auxiliary gain in the AC levelmeter
 **    PRAM-DCC - Configuration bit for DCCTL
 **    HIM-RES - Higher impedance in analog impedance matching loop.
 **    HIM-AN - Higher impedance in analog impedance matching loop.
 **    LMABS-EN - Rectifier or squarer selection for the AC levelmeter
 **    DC-HOLD - DC HOLD
 **    EN-IK - Enable Enhanced Dc Regulation
 **    TEST-EN - Activates the VINETIC-4VIP test features.
 **    SEL-SLIC[3:0] - Selection of the current SLIC type used.
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} BCR1_REG_TYPE;

#define BCR1 pCh->regBCR1
#define BCR1_REG &pCh->regBCR1

/******************************************************************************
 ** BCR2, Basic Configuration Register 2
 **    LPRX-CR - Select CRAM coefficients for the filter characteristic of the LPR/LPX filters.
 **    CRAM-EN - Enables use of CRAM coefficients for programmable filters and DC loop.
 **    SOFT-DIS - Polarity soft reversal (to minimize noise on DC feeding).
 **    TTX-EN - Disables the generation of TTX bursts for metering signals.
 **    TTX-12K - Selection of TTX frequencies.
 **    REVPOL - Reverses the polarity of DC feeding.
 **    AC-SHORT-EN - Enables temporarily lower input impedance at input pin ITAC.
 **    TH-DIS - Disables the TH filter.
 **    IM-DIS - Disables the IM filter.
 **    AX-DIS - Disables the AX filter.
 **    AR-DIS - Disables the AR filter.
 **    FRX-DIS - Disables the FRX filter.
 **    FRR-DIS - Disables the FRR filter.
 **    HPX-DIS - Disables the high-pass filter in transmit direction.
 **    HPR-DIS - Disables the high-pass filter in receive direction.
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} BCR2_REG_TYPE;

#define BCR2 pCh->regBCR2
#define BCR2_REG &pCh->regBCR2

/******************************************************************************
 ** DSCR, DTMF Sender Configuration Register
 **    DG-KEY[3:0] - Selects one of sixteen DTMF keys.
 **    COR8 - Cuts off receive path at 8 kHz before the tone generator summation point. Allows sending of tone generator signals with no overlaid voice.
 **    PTG - Programmable coefficients for tone generators will be used.
 **    TG2-EN - Enables tone generator two.
 **    TG1-EN - Enables tone generator one.
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} DSCR_REG_TYPE;

#define DSCR pCh->regDSCR
#define DSCR_REG &pCh->regDSCR

/******************************************************************************
 ** LMCR, Level Metering Configuration Register
 **    LM-ITIME[3:0] - Integration time for AC Level Metering.
 **    LM-EN - Enables level metering.
 **    LM2PCM - Level metering source/result feeding to PCM or IOM-2 interface.
 **    LM-ONCE - Level metering execution mode.
 **    DC-AD16 - Additional digital amplification in the DC AD path for level metering.
 **    LM-FILT-SEL - Selects a notch filter instead of the band-pass filter for level metering.
 **    LM-FILT - Enables a programmable band-pass or notch filter for level metering.
 **    LM-RECT - Enables rectifier in DC level meter.
 **    RAMP-EN - Enables ramp generator.
 **    LM-SEL[3:0] - Selection of the source for the level metering.
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} LMCR_REG_TYPE;

#define LMCR pCh->regLMCR
#define LMCR_REG &pCh->regLMCR

/******************************************************************************
 ** RTR, Ring Configuration Register
 **    RTR-EXTS - External Ringburst Sense
 **    RTR-SEL - Ring Trip method selection.
 **    RTR-FAST - Fast ring trip detection for reducing power dissipation:
 **    PCM2DC - PCM voice channel data added to the DC-output.
 **    RING-OFFSET[1:0] - Selection of the Ring Offset source (see specification)
 **    ASYNCH-R - Enables asynchronous ringing in case of external ringing.
 **    REXT-EN - Enables the use of an external ring signal generator.
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} RTR_REG_TYPE;

#define RTR pCh->regRTR
#define RTR_REG &pCh->regRTR

/******************************************************************************
 ** OFR, DC Offset Register
 **    DC-OFFSET[15:0] - DC-offset
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_int16_t store;
   IFX_int16_t value;
} OFR_REG_TYPE;

#define OFR pCh->regOFR
#define OFR_REG &pCh->regOFR

/******************************************************************************
 ** AUTOMOD, Automatic Mode Register
 **    AUTO-MET-START - Start Auto Metering
 **    AUTO-MET-EN - Auto Metering
 **    AUTO-OFFH - Automatic mode offhook detection.
 **    AUTO-BAT - Automatic mode battery switching.
 **    PDOT-DIS - Power Down Overtemperature Disable
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} AUTOMOD_REG_TYPE;

#define AUTOMOD pCh->regAUTOMOD
#define AUTOMOD_REG &pCh->regAUTOMOD

/******************************************************************************
 ** TSTR1, Test Register 1
 **    PD-AC-PR - AC-PREFI Power Down
 **    PD-AC-PO - AC-POFI Power Down
 **    PD-AC-AD - AC-ADC Power Down
 **    PD-AC-DA - AC-DAC Power Down
 **    PD-DC-PR - DC-PREFI Power Down
 **    PD-DC-AD - DC-ADC Power Down
 **    PD-DC-DA - DC-DAC Power Down
 **    PD-DCBUF - DC-BUFFER Power Down
 **    PD-ACDITH - AC ADC dither Power Down
 **    PD-DCREF - DC loop reference Power Down
 **    PD-GNKC - Ground key comparator (GNKC) is set to Power Down
 **    PD-OFHC - Off-hook comparator (OFHC) Power Down
 **    PD-OVTC - Overtemperature comparator (OVTC) Power Down
 **    PD-ACREF - AC loop reference Power Down
 **    PD-IREF - Channel bias current block Power Down
 **    PD-HVI - HV interface (to SLIC-E/-E2/-P) Power Down
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} TSTR1_REG_TYPE;

#define TSTR1 pCh->regTSTR1
#define TSTR1_REG &pCh->regTSTR1

/******************************************************************************
 ** TSTR2, Test Register 2
 **    AC-ALB-PP - AC analog loop-back via prefi-pofi filters.
 **    AC-ALB-16M - AC analog loop via a 16-MHz bitstream.
 **    AC-DLB-16M - AC digital loop via a 16-MHz bitstream.
 **    AC-DLB-128K - The signal from the PCM interface is looped back.
 **    AC-DLB-32K - The signal from the PCM interface is looped back.
 **    AC-DLB-8K - AC digital loop via 8 kHz
 **    AC-SHORT - The input pin ITAC will be set to a lower input impedance.
 **    AC-RGAIN - Analog gain in receive direction (should be set to zero).
 **    AC-XGAIN - Analog gain in transmit direction (should be set to zero).
 **    LOW-PERF[1:0] - Reduction of the analog clock of the HW-filters.
 **    OPIM-AN - Open Impedance Matching Loop in the analog part.
 **    OPIM-HW - Open digital Impedance Matching Loop through the hardware filters.
 **    COR-64 - Cut off the AC receive path at 64 kHz.
 **    COX-16 - Cut off the AC transmit path at 16 kHz.
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} TSTR2_REG_TYPE;

#define TSTR2 pCh->regTSTR2
#define TSTR2_REG &pCh->regTSTR2

/******************************************************************************
 ** TSTR3, Test Register 3
 **    DC-ALB-PP - DC analog loop-back via prefi-pofi filters.
 **    DC-ALB-1M - AC analog loop via a 1-MHz bitstream.
 **    DC-DLB-1M - DC digital loop via a 1-MHz bitstream.
 **    DC-POFI-HI - Higher value for the DC post filter cut-off frequency.
 **    DC-HOLD - Actual DC output value hold (the value of the last DSP filter stage will
 **    DCC-DIS - Disables DC characteristic
 **    DC-LP2-DIS - Disables low pass 2 (LP2) of DC characteristic
 **    DC-LP1-DIS - Disables low pass 1 (LP1) of DC characteristic
 **    ATST-RES5 - reserved for future use
 **    ATST-RES4 - reserved for future use
 **    ATST-RES3 - reserved for future use
 **    ATST-RES2 - reserved for future use
 **    ATST-RES1 - reserved for future use
 **    ATST-RES0 - reserved for future use
 **    NO-DITH-AC - Disables dithering of the AC ADC
 **    NO-AUTO-DITH-AC - Disables automatic dithering of the AC ADC
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} TSTR3_REG_TYPE;

#define TSTR3 pCh->regTSTR3
#define TSTR3_REG &pCh->regTSTR3

/******************************************************************************
 ** MR-SRS1, Mask Register for Status Register for Analog-Line-Module Interrupts 1
 **    M-AUTO-MET-DONE - Mask for AUTO-MET-DONE Bit
 **    M-RAMP-READY - Mask for RAMP-READY bit
 **    M-HOOK - Mask for HOOK bit
 **    M-GNDK - Mask for GNDK bit
 **    M-GNKP - Mask for GNKP bit
 **    M-GNDKH - Mask for GNDKH bit
 **    M-ICON - Mask for ICON bit
 **    M-VTRLIM - Mask for VTRLIM bit
 **    M-LM-THRES - Mask for LM-THRES bit
 **    M-LM-OK - Mask for LM-OK bit
 **    M-DU-IO[4:0] - Data on I/O pins 1 to 4 filtered by DUP-IO counter and interrupt generation masked by the DU-M-IO[4:0] bits. A change of any of this bits generates an interrupt.
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} MR_SRS1_REG_TYPE;

#define MR_SRS1 pCh->regMR_SRS1
#define MR_SRS1_REG &pCh->regMR_SRS1

/******************************************************************************
 ** MR-SRS2, Mask Register for Status Register for Analog-Line-Module Interrupts 2
 **    M-OTEMP - Mask for OTEMP bit
 **    M-CS-FAIL-CRAM - Mask for CS-FAIL-CRAM bit
 **    M-CS-FAIL-DSP - Mask for CS-FAIL-DSP bit
 **    M-CS-FAIL-DCCTL - Mask for CS-FAIL-DCCTL bit
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} MR_SRS2_REG_TYPE;

#define MR_SRS2 pCh->regMR_SRS2
#define MR_SRS2_REG &pCh->regMR_SRS2

/******************************************************************************
 ** MF-SRS1, Mask Register for Status Register for Analog-Line-Module Interrupts 1
 **    M-AUTO-MET-DONE - Mask for AUTO-MET-DONE Bit
 **    M-RAMP-READY - Mask for RAMP-READY bit
 **    M-HOOK - Mask for HOOK bit
 **    M-GNDK - Mask for GNDK bit
 **    M-GNKP - Mask for GNKP bit
 **    M-GNDKH - Mask for GNDKH bit
 **    M-ICON - Mask for ICON bit
 **    M-VTRLIM - Mask for VTRLIM bit
 **    M-LM-THRES - Mask for LM-THRES bit
 **    M-LM-OK - Mask for LM-OK bit
 **    M-DU-IO[4:0] - Data on I/O pins 1 to 4 filtered by DUP-IO counter and interrupt generation masked by the DU-M-IO[4:0] bits. A change of any of this bits generates an interrupt.
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} MF_SRS1_REG_TYPE;

#define MF_SRS1 pCh->regMF_SRS1
#define MF_SRS1_REG &pCh->regMF_SRS1

/******************************************************************************
 ** MF-SRS2, Mask Register for Status Register for Analog-Line-Module Interrupts 2
 **    M-OTEMP - Mask for OTEMP bit
 **    M-CS-FAIL-CRAM - Mask for CS-FAIL-CRAM bit
 **    M-CS-FAIL-DSP - Mask for CS-FAIL-DSP bit
 **    M-CS-FAIL-DCCTL - Mask for CS-FAIL-DCCTL bit
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} MF_SRS2_REG_TYPE;

#define MF_SRS2 pCh->regMF_SRS2
#define MF_SRS2_REG &pCh->regMF_SRS2

/******************************************************************************
 ** OPMOD-CUR, Operating Mode Status Register Current
 **    MODE-CUR[2:0] - Current operating mode.
 **    SUBMODE-CUR[4:0] - Current operating submode.
 **    DCCTL-REV - DCCTL FW Revision
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} OPMOD_CUR_REG_TYPE;

#define OPMOD_CUR pCh->regOPMOD_CUR
#define OPMOD_CUR_REG &pCh->regOPMOD_CUR

/******************************************************************************
 ** TG1F, TG1 Freq
 **    TG1F - TG1 Freq
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} TG1F_REG_TYPE;

#define TG1F pCh->regTG1F
#define TG1F_REG &pCh->regTG1F

/******************************************************************************
 ** BP1F, BP1 Freq
 **    BP1F - BP1 Frequency
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} BP1F_REG_TYPE;

#define BP1F pCh->regBP1F
#define BP1F_REG &pCh->regBP1F

/******************************************************************************
 ** TG2F, TG2 Freq
 **    TG2F - TG2 Freq
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} TG2F_REG_TYPE;

#define TG2F pCh->regTG2F
#define TG2F_REG &pCh->regTG2F

/******************************************************************************
 ** BP2F, BP2 Freq
 **    BP2F - BP2 Frequency
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} BP2F_REG_TYPE;

#define BP2F pCh->regBP2F
#define BP2F_REG &pCh->regBP2F

/******************************************************************************
 ** TG1A, TG1 Amplitude
 **    TG1A - TG1A
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} TG1A_REG_TYPE;

#define TG1A pCh->regTG1A
#define TG1A_REG &pCh->regTG1A

/******************************************************************************
 ** TG2A, TG2 Amplitude
 **    TG2A - TG2A
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} TG2A_REG_TYPE;

#define TG2A pCh->regTG2A
#define TG2A_REG &pCh->regTG2A

/******************************************************************************
 ** BP12Q, BP1, BP2 Quality
 **    BP1Q - BP2 Quality
 **    BP2Q - BP1 Quality
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} BP12Q_REG_TYPE;

#define BP12Q pCh->regBP12Q
#define BP12Q_REG &pCh->regBP12Q

/******************************************************************************
 ** BP3F, BP3 Freq
 **    BP3F - BP3 Frequency
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} BP3F_REG_TYPE;

#define BP3F pCh->regBP3F
#define BP3F_REG &pCh->regBP3F

/******************************************************************************
 ** RGFR, Ring Frequency
 **    RGFR - Ring Frequency
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} RGFR_REG_TYPE;

#define RGFR pCh->regRGFR
#define RGFR_REG &pCh->regRGFR

/******************************************************************************
 ** LMAC, LMAC
 **    LMAC - LMAC
 **    BP3Q - BP3 Quality
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} LMAC_REG_TYPE;

#define LM_AC pCh->regLMAC
#define LMAC_REG &pCh->regLMAC

/******************************************************************************
 ** LMDC, DC LM Shift Factor
 **    LMDC - LMDC
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_int16_t store;
   IFX_int16_t value;
} LMDC_REG_TYPE;

#define LMDC pCh->regLMDC
#define LMDC_REG &pCh->regLMDC

/******************************************************************************
 ** V1, V1
 **    V1 - V1
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} V1_REG_TYPE;

#define V1 pCh->regV1
#define V1_REG &pCh->regV1

/******************************************************************************
 ** RGTP,
 **    RGTP - RGTP
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} RGTP_REG_TYPE;

#define RGTP pCh->regRGTP
#define RGTP_REG &pCh->regRGTP

/******************************************************************************
 ** CREST, CREST Factor
 **    CREST - CREST
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} CREST_REG_TYPE;

#define CREST pCh->regCREST
#define CREST_REG &pCh->regCREST

/******************************************************************************
 ** RO1, Ring Offset 1
 **    RO1 - RO1
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} RO1_REG_TYPE;

#define RO1 pCh->regRO1
#define RO1_REG &pCh->regRO1

/******************************************************************************
 ** RO2, Ring Offset 2
 **    RO2 - RO2
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} RO2_REG_TYPE;

#define RO2 pCh->regRO2
#define RO2_REG &pCh->regRO2

/******************************************************************************
 ** RO3, Ring Offset 3
 **    RO3 - RO3
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} RO3_REG_TYPE;

#define RO3 pCh->regRO3
#define RO3_REG &pCh->regRO3

/******************************************************************************
 ** RGD, Ring Delay
 **    RGD - RGD
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} RGD_REG_TYPE;

#define RGD pCh->regRGD
#define RGD_REG &pCh->regRGD

/******************************************************************************
 ** I2,
 **    I2 - I2
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} I2_REG_TYPE;

#define I2 pCh->regI2
#define I2_REG &pCh->regI2

/******************************************************************************
 ** VLIM,
 **    VLIM - VLIM
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} VLIM_REG_TYPE;

#define VLIM pCh->regVLIM
#define VLIM_REG &pCh->regVLIM

/******************************************************************************
 ** VK1,
 **    VK1 - VK1
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} VK1_REG_TYPE;

#define VK1 pCh->regVK1
#define VK1_REG &pCh->regVK1

/******************************************************************************
 ** IK1, IK1
 **    IK1 - IK1
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} IK1_REG_TYPE;

#define IK1 pCh->regIK1
#define IK1_REG &pCh->regIK1

/******************************************************************************
 ** VRTL,
 **    VRTL - VRTL
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} VRTL_REG_TYPE;

#define VRTL pCh->regVRTL
#define VRTL_REG &pCh->regVRTL

/******************************************************************************
 ** HPD,
 **    HPD - HPD
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} HPD_REG_TYPE;

#define HPD pCh->regHPD
#define HPD_REG &pCh->regHPD

/******************************************************************************
 ** HACT,
 **    HACT - HACT
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} HACT_REG_TYPE;

#define HACT pCh->regHACT
#define HACT_REG &pCh->regHACT

/******************************************************************************
 ** HRING,
 **    HRING - HRING
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} HRING_REG_TYPE;

#define HRING pCh->regHRING
#define HRING_REG &pCh->regHRING

/******************************************************************************
 ** HACRT,
 **    HACRT - HACRT
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} HACRT_REG_TYPE;

#define HACRT pCh->regHACRT
#define HACRT_REG &pCh->regHACRT

/******************************************************************************
 ** HLS,
 **    HLS - HLS
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} HLS_REG_TYPE;

#define HLS pCh->regHLS
#define HLS_REG &pCh->regHLS

/******************************************************************************
 ** HMW,
 **    HMW - HMW
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} HMW_REG_TYPE;

#define HMW pCh->regHMW
#define HMW_REG &pCh->regHMW

/******************************************************************************
 ** HAHY,
 **    HAHY - HAHY
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} HAHY_REG_TYPE;

#define HAHY pCh->regHAHY
#define HAHY_REG &pCh->regHAHY

/******************************************************************************
 ** FRTR,
 **    FRTR - FRTR
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} FRTR_REG_TYPE;

#define FRTR pCh->regFRTR
#define FRTR_REG &pCh->regFRTR

/******************************************************************************
 ** CRAMP,
 **    CRAMP - CRAMP
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} CRAMP_REG_TYPE;

#define CRAMP pCh->regCRAMP
#define CRAMP_REG &pCh->regCRAMP

#ifdef VIN_V21_SUPPORT
/******************************************************************************
 ** LMRES2, Level Metering Result2
 **    LM-RES2[15:0] - LM result (selected by the LM-SEL bits in the LMCR register)
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} LMRES2_REG_TYPE;

#define LMRES2 pCh->regLMRES2
#define LMRES2_REG &pCh->regLMRES2

/******************************************************************************
 ** LMCR2, Level Metering Configuration Register 2
 **    LM2PCM - Level Metering Result via PCM
 **    LM_ITS - Integration Time Source
 **    LM_DCTX8K - Level Metering Path Selection
 **    LM_FILT_SEL - Level Metering Filter Selection
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} LMCR2_REG_TYPE;

#define LMCR2 pCh->regLMCR2
#define LMCR2_REG &pCh->regLMCR2

/******************************************************************************
 ** LM_ITIME,
 **    LM_ITIME - LM_ITIME
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} LM_ITIME_REG_TYPE;

#define LM_ITIME pCh->regLM_ITIME
#define LM_ITIME_REG &pCh->regLM_ITIME

/******************************************************************************
 ** LM_DELAY,
 **    LM_DELAY - LM_DELAY
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} LM_DELAY_REG_TYPE;

#define LM_DELAY pCh->regLM_DELAY
#define LM_DELAY_REG &pCh->regLM_DELAY

/******************************************************************************
 ** LM_SHIFT,
 **    LM_SHIFT - LM_SHIFT
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} LM_SHIFT_REG_TYPE;

#define LM_SHIFT pCh->regLM_SHIFT
#define LM_SHIFT_REG &pCh->regLM_SHIFT

/******************************************************************************
 ** LM_THRES,
 **    LM_THRES - LM_THRES
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} LM_THRES_REG_TYPE;

#define LM_THRES pCh->regLM_THRES
#define LM_THRES_REG &pCh->regLM_THRES

/******************************************************************************
 ** LM_RCLOWTHRES,
 **    LM_RCLOWTHRES - LM_RCLOWTHRES
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} LM_RCLOWTHRES_REG_TYPE;

#define LM_RCLOWTHRES pCh->regLM_RCLOWTHRES
#define LM_RCLOWTHRES_REG &pCh->regLM_RCLOWTHRES

/******************************************************************************
 ** LM_FILT1,
 **    LM_FILT1 - LM_FILT1
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} LM_FILT1_REG_TYPE;

#define LM_FILT1 pCh->regLM_FILT1
#define LM_FILT1_REG &pCh->regLM_FILT1

/******************************************************************************
 ** LM_FILT2,
 **    LM_FILT2 - LM_FILT2
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} LM_FILT2_REG_TYPE;

#define LM_FILT2 pCh->regLM_FILT2
#define LM_FILT2_REG &pCh->regLM_FILT2

/******************************************************************************
 ** LM_FILT3,
 **    LM_FILT3 - LM_FILT3
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} LM_FILT3_REG_TYPE;

#define LM_FILT3 pCh->regLM_FILT3
#define LM_FILT3_REG &pCh->regLM_FILT3

/******************************************************************************
 ** LM_FILT4,
 **    LM_FILT4 - LM_FILT4
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} LM_FILT4_REG_TYPE;

#define LM_FILT4 pCh->regLM_FILT4
#define LM_FILT4_REG &pCh->regLM_FILT4

/******************************************************************************
 ** LM_FILT5,
 **    LM_FILT5 - LM_FILT5
 *******************************************************************************/
typedef struct {
   IFX_uint8_t  address;
   IFX_uint8_t  length;
   IFX_uint16_t regType;
   IFX_char_t   name[15];
   /* setVal function pointer */
   IFX_uint16_t (* setVal) (struct _VINETIC_DEVICE *pDev, IFX_int32_t nVal);
   /* getVal function pointer */
   IFX_int32_t (* getVal) (struct _VINETIC_DEVICE *pDev, IFX_uint16_t nCoef);
   IFX_uint16_t store;
   IFX_uint16_t value;
} LM_FILT5_REG_TYPE;

#define LM_FILT5 pCh->regLM_FILT5
#define LM_FILT5_REG &pCh->regLM_FILT5

#endif /* VIN_V21_SUPPORT */



/* ************************************************************************** */
/* automatically generated register description - do not change               */
/* ************************************************************************** */



/***********************************************************************/
/*  Module      :  CH register address and bits                        */
/***********************************************************************/

#define VIN_CH                                        (0)

/***********************************************************************/


/***Level Metering Result***/

#define VIN_CH_LMRES                                  ( (VIN_CH + 0x03) )

#define VIN_CH_LMRES_LM_RES

/***CRAM Checksum Register***/

#define VIN_CH_CCHKR                                  ( (VIN_CH + 0x04) )

#define VIN_CH_CCHKR_C_CHECK

/***I/O Control Register 1***/

#define VIN_CH_IOCTL1                                 ( (VIN_CH + 0x05) )

#define VIN_CH_IOCTL1_RES15                           (1 << 15)
#define VIN_CH_IOCTL1_RES15_GET(reg)                  ((reg & VIN_CH_IOCTL1_RES15) >> 15)
#define VIN_CH_IOCTL1_RES15_SET(reg, val)             (reg = ((reg & ~VIN_CH_IOCTL1_RES15) | (((val) & 0x0001) << 15)))
#define VIN_CH_IOCTL1_RES14                           (1 << 14)
#define VIN_CH_IOCTL1_RES14_GET(reg)                  ((reg & VIN_CH_IOCTL1_RES14) >> 14)
#define VIN_CH_IOCTL1_RES14_SET(reg, val)             (reg = ((reg & ~VIN_CH_IOCTL1_RES14) | (((val) & 0x0001) << 14)))
#define VIN_CH_IOCTL1_RES13                           (1 << 13)
#define VIN_CH_IOCTL1_RES13_GET(reg)                  ((reg & VIN_CH_IOCTL1_RES13) >> 13)
#define VIN_CH_IOCTL1_RES13_SET(reg, val)             (reg = ((reg & ~VIN_CH_IOCTL1_RES13) | (((val) & 0x0001) << 13)))
#define VIN_CH_IOCTL1_INEN_IO4                        (1 << 12)
#define VIN_CH_IOCTL1_INEN_IO4_GET(reg)               ((reg & VIN_CH_IOCTL1_INEN_IO4) >> 12)
#define VIN_CH_IOCTL1_INEN_IO4_SET(reg, val)          (reg = ((reg & ~VIN_CH_IOCTL1_INEN_IO4) | (((val) & 0x0001) << 12)))
#define VIN_CH_IOCTL1_INEN_IO3                        (1 << 11)
#define VIN_CH_IOCTL1_INEN_IO3_GET(reg)               ((reg & VIN_CH_IOCTL1_INEN_IO3) >> 11)
#define VIN_CH_IOCTL1_INEN_IO3_SET(reg, val)          (reg = ((reg & ~VIN_CH_IOCTL1_INEN_IO3) | (((val) & 0x0001) << 11)))
#define VIN_CH_IOCTL1_INEN_IO2                        (1 << 10)
#define VIN_CH_IOCTL1_INEN_IO2_GET(reg)               ((reg & VIN_CH_IOCTL1_INEN_IO2) >> 10)
#define VIN_CH_IOCTL1_INEN_IO2_SET(reg, val)          (reg = ((reg & ~VIN_CH_IOCTL1_INEN_IO2) | (((val) & 0x0001) << 10)))
#define VIN_CH_IOCTL1_INEN_IO1                        (1 << 9)
#define VIN_CH_IOCTL1_INEN_IO1_GET(reg)               ((reg & VIN_CH_IOCTL1_INEN_IO1) >> 9)
#define VIN_CH_IOCTL1_INEN_IO1_SET(reg, val)          (reg = ((reg & ~VIN_CH_IOCTL1_INEN_IO1) | (((val) & 0x0001) << 9)))
#define VIN_CH_IOCTL1_INEN_IO0                        (1 << 8)
#define VIN_CH_IOCTL1_INEN_IO0_GET(reg)               ((reg & VIN_CH_IOCTL1_INEN_IO0) >> 8)
#define VIN_CH_IOCTL1_INEN_IO0_SET(reg, val)          (reg = ((reg & ~VIN_CH_IOCTL1_INEN_IO0) | (((val) & 0x0001) << 8)))
#define VIN_CH_IOCTL1_RES7                            (1 << 7)
#define VIN_CH_IOCTL1_RES7_GET(reg)                   ((reg & VIN_CH_IOCTL1_RES7) >> 7)
#define VIN_CH_IOCTL1_RES7_SET(reg, val)              (reg = ((reg & ~VIN_CH_IOCTL1_RES7) | (((val) & 0x0001) << 7)))
#define VIN_CH_IOCTL1_RES6                            (1 << 6)
#define VIN_CH_IOCTL1_RES6_GET(reg)                   ((reg & VIN_CH_IOCTL1_RES6) >> 6)
#define VIN_CH_IOCTL1_RES6_SET(reg, val)              (reg = ((reg & ~VIN_CH_IOCTL1_RES6) | (((val) & 0x0001) << 6)))
#define VIN_CH_IOCTL1_RES5                            (1 << 5)
#define VIN_CH_IOCTL1_RES5_GET(reg)                   ((reg & VIN_CH_IOCTL1_RES5) >> 5)
#define VIN_CH_IOCTL1_RES5_SET(reg, val)              (reg = ((reg & ~VIN_CH_IOCTL1_RES5) | (((val) & 0x0001) << 5)))
#define VIN_CH_IOCTL1_OEN_IO4                         (1 << 4)
#define VIN_CH_IOCTL1_OEN_IO4_GET(reg)                ((reg & VIN_CH_IOCTL1_OEN_IO4) >> 4)
#define VIN_CH_IOCTL1_OEN_IO4_SET(reg, val)           (reg = ((reg & ~VIN_CH_IOCTL1_OEN_IO4) | (((val) & 0x0001) << 4)))
#define VIN_CH_IOCTL1_OEN_IO3                         (1 << 3)
#define VIN_CH_IOCTL1_OEN_IO3_GET(reg)                ((reg & VIN_CH_IOCTL1_OEN_IO3) >> 3)
#define VIN_CH_IOCTL1_OEN_IO3_SET(reg, val)           (reg = ((reg & ~VIN_CH_IOCTL1_OEN_IO3) | (((val) & 0x0001) << 3)))
#define VIN_CH_IOCTL1_OEN_IO2                         (1 << 2)
#define VIN_CH_IOCTL1_OEN_IO2_GET(reg)                ((reg & VIN_CH_IOCTL1_OEN_IO2) >> 2)
#define VIN_CH_IOCTL1_OEN_IO2_SET(reg, val)           (reg = ((reg & ~VIN_CH_IOCTL1_OEN_IO2) | (((val) & 0x0001) << 2)))
#define VIN_CH_IOCTL1_OEN_IO1                         (1 << 1)
#define VIN_CH_IOCTL1_OEN_IO1_GET(reg)                ((reg & VIN_CH_IOCTL1_OEN_IO1) >> 1)
#define VIN_CH_IOCTL1_OEN_IO1_SET(reg, val)           (reg = ((reg & ~VIN_CH_IOCTL1_OEN_IO1) | (((val) & 0x0001) << 1)))
#define VIN_CH_IOCTL1_OEN_IO0                         (1 << 0)
#define VIN_CH_IOCTL1_OEN_IO0_GET(reg)                ((reg & VIN_CH_IOCTL1_OEN_IO0) >> 0)
#define VIN_CH_IOCTL1_OEN_IO0_SET(reg, val)           (reg = ((reg & ~VIN_CH_IOCTL1_OEN_IO0) | (((val) & 0x0001) << 0)))

/***I/O Control Register 2***/

#define VIN_CH_IOCTL2                                 ( (VIN_CH + 0x06) )

#define VIN_CH_IOCTL2_DUP_OVT_MASK                    (((1 << 3) - 1) << 13)
#define VIN_CH_IOCTL2_DUP_OVT_GET(reg)                (((reg) >> 13) & ((1 << 3) - 1))
#define VIN_CH_IOCTL2_DUP_OVT_SET(val)                ((((1 << 3) - 1) & (val)) << 13)
#define VIN_CH_IOCTL2_DUP_OVT_MODIFY(reg, val)        (reg = ( (reg & ~VIN_CH_IOCTL2_DUP_OVT_MASK) | ((((1 << 3) - 1) & (val)) << 13) ))
#define VIN_CH_IOCTL2_DD_IO4                          (1 << 12)
#define VIN_CH_IOCTL2_DD_IO4_GET(reg)                 ((reg & VIN_CH_IOCTL2_DD_IO4) >> 12)
#define VIN_CH_IOCTL2_DD_IO4_SET(reg, val)            (reg = ((reg & ~VIN_CH_IOCTL2_DD_IO4) | (((val) & 0x0001) << 12)))
#define VIN_CH_IOCTL2_DD_IO3                          (1 << 11)
#define VIN_CH_IOCTL2_DD_IO3_GET(reg)                 ((reg & VIN_CH_IOCTL2_DD_IO3) >> 11)
#define VIN_CH_IOCTL2_DD_IO3_SET(reg, val)            (reg = ((reg & ~VIN_CH_IOCTL2_DD_IO3) | (((val) & 0x0001) << 11)))
#define VIN_CH_IOCTL2_DD_IO2                          (1 << 10)
#define VIN_CH_IOCTL2_DD_IO2_GET(reg)                 ((reg & VIN_CH_IOCTL2_DD_IO2) >> 10)
#define VIN_CH_IOCTL2_DD_IO2_SET(reg, val)            (reg = ((reg & ~VIN_CH_IOCTL2_DD_IO2) | (((val) & 0x0001) << 10)))
#define VIN_CH_IOCTL2_DD_IO1                          (1 << 9)
#define VIN_CH_IOCTL2_DD_IO1_GET(reg)                 ((reg & VIN_CH_IOCTL2_DD_IO1) >> 9)
#define VIN_CH_IOCTL2_DD_IO1_SET(reg, val)            (reg = ((reg & ~VIN_CH_IOCTL2_DD_IO1) | (((val) & 0x0001) << 9)))
#define VIN_CH_IOCTL2_DD_IO0                          (1 << 8)
#define VIN_CH_IOCTL2_DD_IO0_GET(reg)                 ((reg & VIN_CH_IOCTL2_DD_IO0) >> 8)
#define VIN_CH_IOCTL2_DD_IO0_SET(reg, val)            (reg = ((reg & ~VIN_CH_IOCTL2_DD_IO0) | (((val) & 0x0001) << 8)))
#define VIN_CH_IOCTL2_DUP_MASK                        (((1 << 4) - 1) << 4)
#define VIN_CH_IOCTL2_DUP_GET(reg)                    (((reg) >> 4) & ((1 << 4) - 1))
#define VIN_CH_IOCTL2_DUP_SET(val)                    ((((1 << 4) - 1) & (val)) << 4)
#define VIN_CH_IOCTL2_DUP_MODIFY(reg, val)            (reg = ( (reg & ~VIN_CH_IOCTL2_DUP_MASK) | ((((1 << 4) - 1) & (val)) << 4) ))
#define VIN_CH_IOCTL2_DUP_IO_MASK                     (((1 << 4) - 1) << 0)
#define VIN_CH_IOCTL2_DUP_IO_GET(reg)                 (((reg) >> 0) & ((1 << 4) - 1))
#define VIN_CH_IOCTL2_DUP_IO_SET(val)                 ((((1 << 4) - 1) & (val)) << 0)
#define VIN_CH_IOCTL2_DUP_IO_MODIFY(reg, val)         (reg = ( (reg & ~VIN_CH_IOCTL2_DUP_IO_MASK) | ((((1 << 4) - 1) & (val)) << 0) ))

/***Basic Configuration Register 1***/

#define VIN_CH_BCR1                                   ( (VIN_CH + 0x07) )

#define VIN_CH_BCR1_DUP_GNDK_MASK                     (((1 << 4) - 1) << 12)
#define VIN_CH_BCR1_DUP_GNDK_GET(reg)                 (((reg) >> 12) & ((1 << 4) - 1))
#define VIN_CH_BCR1_DUP_GNDK_SET(val)                 ((((1 << 4) - 1) & (val)) << 12)
#define VIN_CH_BCR1_DUP_GNDK_MODIFY(reg, val)         (reg = ( (reg & ~VIN_CH_BCR1_DUP_GNDK_MASK) | ((((1 << 4) - 1) & (val)) << 12) ))
#define VIN_CH_BCR1_LMAC64                            (1 << 11)
#define VIN_CH_BCR1_LMAC64_GET(reg)                   ((reg & VIN_CH_BCR1_LMAC64) >> 11)
#define VIN_CH_BCR1_LMAC64_SET(reg, val)              (reg = ((reg & ~VIN_CH_BCR1_LMAC64) | (((val) & 0x0001) << 11)))
#define VIN_CH_BCR1_PRAM_DCC                          (1 << 10)
#define VIN_CH_BCR1_PRAM_DCC_GET(reg)                 ((reg & VIN_CH_BCR1_PRAM_DCC) >> 10)
#define VIN_CH_BCR1_PRAM_DCC_SET(reg, val)            (reg = ((reg & ~VIN_CH_BCR1_PRAM_DCC) | (((val) & 0x0001) << 10)))
#define VIN_CH_BCR1_HIM_RES                           (1 << 9)
#define VIN_CH_BCR1_HIM_RES_GET(reg)                  ((reg & VIN_CH_BCR1_HIM_RES) >> 9)
#define VIN_CH_BCR1_HIM_RES_SET(reg, val)             (reg = ((reg & ~VIN_CH_BCR1_HIM_RES) | (((val) & 0x0001) << 9)))
#define VIN_CH_BCR1_HIM_AN                            (1 << 8)
#define VIN_CH_BCR1_HIM_AN_GET(reg)                   ((reg & VIN_CH_BCR1_HIM_AN) >> 8)
#define VIN_CH_BCR1_HIM_AN_SET(reg, val)              (reg = ((reg & ~VIN_CH_BCR1_HIM_AN) | (((val) & 0x0001) << 8)))
#define VIN_CH_BCR1_LMABS_EN                          (1 << 7)
#define VIN_CH_BCR1_LMABS_EN_GET(reg)                 ((reg & VIN_CH_BCR1_LMABS_EN) >> 7)
#define VIN_CH_BCR1_LMABS_EN_SET(reg, val)            (reg = ((reg & ~VIN_CH_BCR1_LMABS_EN) | (((val) & 0x0001) << 7)))
#define VIN_CH_BCR1_DC_HOLD                           (1 << 6)
#define VIN_CH_BCR1_DC_HOLD_GET(reg)                  ((reg & VIN_CH_BCR1_DC_HOLD) >> 6)
#define VIN_CH_BCR1_DC_HOLD_SET(reg, val)             (reg = ((reg & ~VIN_CH_BCR1_DC_HOLD) | (((val) & 0x0001) << 6)))
#define VIN_CH_BCR1_EN_IK                             (1 << 5)
#define VIN_CH_BCR1_EN_IK_GET(reg)                    ((reg & VIN_CH_BCR1_EN_IK) >> 5)
#define VIN_CH_BCR1_EN_IK_SET(reg, val)               (reg = ((reg & ~VIN_CH_BCR1_EN_IK) | (((val) & 0x0001) << 5)))
#define VIN_CH_BCR1_TEST_EN                           (1 << 4)
#define VIN_CH_BCR1_TEST_EN_GET(reg)                  ((reg & VIN_CH_BCR1_TEST_EN) >> 4)
#define VIN_CH_BCR1_TEST_EN_SET(reg, val)             (reg = ((reg & ~VIN_CH_BCR1_TEST_EN) | (((val) & 0x0001) << 4)))
#define VIN_CH_BCR1_SEL_SLIC_MASK                     (((1 << 4) - 1) << 0)
#define VIN_CH_BCR1_SEL_SLIC_GET(reg)                 (((reg) >> 0) & ((1 << 4) - 1))
#define VIN_CH_BCR1_SEL_SLIC_SET(val)                 ((((1 << 4) - 1) & (val)) << 0)
#define VIN_CH_BCR1_SEL_SLIC_MODIFY(reg, val)         (reg = ( (reg & ~VIN_CH_BCR1_SEL_SLIC_MASK) | ((((1 << 4) - 1) & (val)) << 0) ))

/***Basic Configuration Register 2***/

#define VIN_CH_BCR2                                   ( (VIN_CH + 0x08) )

#define VIN_CH_BCR2_LPRX_CR                           (1 << 15)
#define VIN_CH_BCR2_LPRX_CR_GET(reg)                  ((reg & VIN_CH_BCR2_LPRX_CR) >> 15)
#define VIN_CH_BCR2_LPRX_CR_SET(reg, val)             (reg = ((reg & ~VIN_CH_BCR2_LPRX_CR) | (((val) & 0x0001) << 15)))
#define VIN_CH_BCR2_CRAM_EN                           (1 << 14)
#define VIN_CH_BCR2_CRAM_EN_GET(reg)                  ((reg & VIN_CH_BCR2_CRAM_EN) >> 14)
#define VIN_CH_BCR2_CRAM_EN_SET(reg, val)             (reg = ((reg & ~VIN_CH_BCR2_CRAM_EN) | (((val) & 0x0001) << 14)))
#define VIN_CH_BCR2_SOFT_DIS                          (1 << 13)
#define VIN_CH_BCR2_SOFT_DIS_GET(reg)                 ((reg & VIN_CH_BCR2_SOFT_DIS) >> 13)
#define VIN_CH_BCR2_SOFT_DIS_SET(reg, val)            (reg = ((reg & ~VIN_CH_BCR2_SOFT_DIS) | (((val) & 0x0001) << 13)))
#define VIN_CH_BCR2_TTX_EN                            (1 << 12)
#define VIN_CH_BCR2_TTX_EN_GET(reg)                   ((reg & VIN_CH_BCR2_TTX_EN) >> 12)
#define VIN_CH_BCR2_TTX_EN_SET(reg, val)              (reg = ((reg & ~VIN_CH_BCR2_TTX_EN) | (((val) & 0x0001) << 12)))
#define VIN_CH_BCR2_TTX_12K                           (1 << 11)
#define VIN_CH_BCR2_TTX_12K_GET(reg)                  ((reg & VIN_CH_BCR2_TTX_12K) >> 11)
#define VIN_CH_BCR2_TTX_12K_SET(reg, val)             (reg = ((reg & ~VIN_CH_BCR2_TTX_12K) | (((val) & 0x0001) << 11)))
#define VIN_CH_BCR2_REVPOL                            (1 << 10)
#define VIN_CH_BCR2_REVPOL_GET(reg)                   ((reg & VIN_CH_BCR2_REVPOL) >> 10)
#define VIN_CH_BCR2_REVPOL_SET(reg, val)              (reg = ((reg & ~VIN_CH_BCR2_REVPOL) | (((val) & 0x0001) << 10)))
#define VIN_CH_BCR2_AC_SHORT_EN                       (1 << 9)
#define VIN_CH_BCR2_AC_SHORT_EN_GET(reg)              ((reg & VIN_CH_BCR2_AC_SHORT_EN) >> 9)
#define VIN_CH_BCR2_AC_SHORT_EN_SET(reg, val)         (reg = ((reg & ~VIN_CH_BCR2_AC_SHORT_EN) | (((val) & 0x0001) << 9)))
#define VIN_CH_BCR2_RES8                              (1 << 8)
#define VIN_CH_BCR2_RES8_GET(reg)                     ((reg & VIN_CH_BCR2_RES8) >> 8)
#define VIN_CH_BCR2_RES8_SET(reg, val)                (reg = ((reg & ~VIN_CH_BCR2_RES8) | (((val) & 0x0001) << 8)))
#define VIN_CH_BCR2_TH_DIS                            (1 << 7)
#define VIN_CH_BCR2_TH_DIS_GET(reg)                   ((reg & VIN_CH_BCR2_TH_DIS) >> 7)
#define VIN_CH_BCR2_TH_DIS_SET(reg, val)              (reg = ((reg & ~VIN_CH_BCR2_TH_DIS) | (((val) & 0x0001) << 7)))
#define VIN_CH_BCR2_IM_DIS                            (1 << 6)
#define VIN_CH_BCR2_IM_DIS_GET(reg)                   ((reg & VIN_CH_BCR2_IM_DIS) >> 6)
#define VIN_CH_BCR2_IM_DIS_SET(reg, val)              (reg = ((reg & ~VIN_CH_BCR2_IM_DIS) | (((val) & 0x0001) << 6)))
#define VIN_CH_BCR2_AX_DIS                            (1 << 5)
#define VIN_CH_BCR2_AX_DIS_GET(reg)                   ((reg & VIN_CH_BCR2_AX_DIS) >> 5)
#define VIN_CH_BCR2_AX_DIS_SET(reg, val)              (reg = ((reg & ~VIN_CH_BCR2_AX_DIS) | (((val) & 0x0001) << 5)))
#define VIN_CH_BCR2_AR_DIS                            (1 << 4)
#define VIN_CH_BCR2_AR_DIS_GET(reg)                   ((reg & VIN_CH_BCR2_AR_DIS) >> 4)
#define VIN_CH_BCR2_AR_DIS_SET(reg, val)              (reg = ((reg & ~VIN_CH_BCR2_AR_DIS) | (((val) & 0x0001) << 4)))
#define VIN_CH_BCR2_FRX_DIS                           (1 << 3)
#define VIN_CH_BCR2_FRX_DIS_GET(reg)                  ((reg & VIN_CH_BCR2_FRX_DIS) >> 3)
#define VIN_CH_BCR2_FRX_DIS_SET(reg, val)             (reg = ((reg & ~VIN_CH_BCR2_FRX_DIS) | (((val) & 0x0001) << 3)))
#define VIN_CH_BCR2_FRR_DIS                           (1 << 2)
#define VIN_CH_BCR2_FRR_DIS_GET(reg)                  ((reg & VIN_CH_BCR2_FRR_DIS) >> 2)
#define VIN_CH_BCR2_FRR_DIS_SET(reg, val)             (reg = ((reg & ~VIN_CH_BCR2_FRR_DIS) | (((val) & 0x0001) << 2)))
#define VIN_CH_BCR2_HPX_DIS                           (1 << 1)
#define VIN_CH_BCR2_HPX_DIS_GET(reg)                  ((reg & VIN_CH_BCR2_HPX_DIS) >> 1)
#define VIN_CH_BCR2_HPX_DIS_SET(reg, val)             (reg = ((reg & ~VIN_CH_BCR2_HPX_DIS) | (((val) & 0x0001) << 1)))
#define VIN_CH_BCR2_HPR_DIS                           (1 << 0)
#define VIN_CH_BCR2_HPR_DIS_GET(reg)                  ((reg & VIN_CH_BCR2_HPR_DIS) >> 0)
#define VIN_CH_BCR2_HPR_DIS_SET(reg, val)             (reg = ((reg & ~VIN_CH_BCR2_HPR_DIS) | (((val) & 0x0001) << 0)))

/***DTMF Sender Configuration Register***/

#define VIN_CH_DSCR                                   ( (VIN_CH + 0x09) )

#define VIN_CH_DSCR_RES15                             (1 << 15)
#define VIN_CH_DSCR_RES15_GET(reg)                    ((reg & VIN_CH_DSCR_RES15) >> 15)
#define VIN_CH_DSCR_RES15_SET(reg, val)               (reg = ((reg & ~VIN_CH_DSCR_RES15) | (((val) & 0x0001) << 15)))
#define VIN_CH_DSCR_RES14                             (1 << 14)
#define VIN_CH_DSCR_RES14_GET(reg)                    ((reg & VIN_CH_DSCR_RES14) >> 14)
#define VIN_CH_DSCR_RES14_SET(reg, val)               (reg = ((reg & ~VIN_CH_DSCR_RES14) | (((val) & 0x0001) << 14)))
#define VIN_CH_DSCR_RES13                             (1 << 13)
#define VIN_CH_DSCR_RES13_GET(reg)                    ((reg & VIN_CH_DSCR_RES13) >> 13)
#define VIN_CH_DSCR_RES13_SET(reg, val)               (reg = ((reg & ~VIN_CH_DSCR_RES13) | (((val) & 0x0001) << 13)))
#define VIN_CH_DSCR_RES12                             (1 << 12)
#define VIN_CH_DSCR_RES12_GET(reg)                    ((reg & VIN_CH_DSCR_RES12) >> 12)
#define VIN_CH_DSCR_RES12_SET(reg, val)               (reg = ((reg & ~VIN_CH_DSCR_RES12) | (((val) & 0x0001) << 12)))
#define VIN_CH_DSCR_RES11                             (1 << 11)
#define VIN_CH_DSCR_RES11_GET(reg)                    ((reg & VIN_CH_DSCR_RES11) >> 11)
#define VIN_CH_DSCR_RES11_SET(reg, val)               (reg = ((reg & ~VIN_CH_DSCR_RES11) | (((val) & 0x0001) << 11)))
#define VIN_CH_DSCR_RES10                             (1 << 10)
#define VIN_CH_DSCR_RES10_GET(reg)                    ((reg & VIN_CH_DSCR_RES10) >> 10)
#define VIN_CH_DSCR_RES10_SET(reg, val)               (reg = ((reg & ~VIN_CH_DSCR_RES10) | (((val) & 0x0001) << 10)))
#define VIN_CH_DSCR_RES9                              (1 << 9)
#define VIN_CH_DSCR_RES9_GET(reg)                     ((reg & VIN_CH_DSCR_RES9) >> 9)
#define VIN_CH_DSCR_RES9_SET(reg, val)                (reg = ((reg & ~VIN_CH_DSCR_RES9) | (((val) & 0x0001) << 9)))
#define VIN_CH_DSCR_RES8                              (1 << 8)
#define VIN_CH_DSCR_RES8_GET(reg)                     ((reg & VIN_CH_DSCR_RES8) >> 8)
#define VIN_CH_DSCR_RES8_SET(reg, val)                (reg = ((reg & ~VIN_CH_DSCR_RES8) | (((val) & 0x0001) << 8)))
#define VIN_CH_DSCR_DG_KEY_MASK                       (((1 << 4) - 1) << 4)
#define VIN_CH_DSCR_DG_KEY_GET(reg)                   (((reg) >> 4) & ((1 << 4) - 1))
#define VIN_CH_DSCR_DG_KEY_SET(val)                   ((((1 << 4) - 1) & (val)) << 4)
#define VIN_CH_DSCR_DG_KEY_MODIFY(reg, val)           (reg = ( (reg & ~VIN_CH_DSCR_DG_KEY_MASK) | ((((1 << 4) - 1) & (val)) << 4) ))
#define VIN_CH_DSCR_COR8                              (1 << 3)
#define VIN_CH_DSCR_COR8_GET(reg)                     ((reg & VIN_CH_DSCR_COR8) >> 3)
#define VIN_CH_DSCR_COR8_SET(reg, val)                (reg = ((reg & ~VIN_CH_DSCR_COR8) | (((val) & 0x0001) << 3)))
#define VIN_CH_DSCR_PTG                               (1 << 2)
#define VIN_CH_DSCR_PTG_GET(reg)                      ((reg & VIN_CH_DSCR_PTG) >> 2)
#define VIN_CH_DSCR_PTG_SET(reg, val)                 (reg = ((reg & ~VIN_CH_DSCR_PTG) | (((val) & 0x0001) << 2)))
#define VIN_CH_DSCR_TG2_EN                            (1 << 1)
#define VIN_CH_DSCR_TG2_EN_GET(reg)                   ((reg & VIN_CH_DSCR_TG2_EN) >> 1)
#define VIN_CH_DSCR_TG2_EN_SET(reg, val)              (reg = ((reg & ~VIN_CH_DSCR_TG2_EN) | (((val) & 0x0001) << 1)))
#define VIN_CH_DSCR_TG1_EN                            (1 << 0)
#define VIN_CH_DSCR_TG1_EN_GET(reg)                   ((reg & VIN_CH_DSCR_TG1_EN) >> 0)
#define VIN_CH_DSCR_TG1_EN_SET(reg, val)              (reg = ((reg & ~VIN_CH_DSCR_TG1_EN) | (((val) & 0x0001) << 0)))

/***Level Metering Configuration Register***/

#define VIN_CH_LMCR                                   ( (VIN_CH + 0x0a) )

#define VIN_CH_LMCR_LM_ITIME_MASK                     (((1 << 4) - 1) << 12)
#define VIN_CH_LMCR_LM_ITIME_GET(reg)                 (((reg) >> 12) & ((1 << 4) - 1))
#define VIN_CH_LMCR_LM_ITIME_SET(val)                 ((((1 << 4) - 1) & (val)) << 12)
#define VIN_CH_LMCR_LM_ITIME_MODIFY(reg, val)         (reg = ( (reg & ~VIN_CH_LMCR_LM_ITIME_MASK) | ((((1 << 4) - 1) & (val)) << 12) ))
#define VIN_CH_LMCR_LM_EN                             (1 << 11)
#define VIN_CH_LMCR_LM_EN_GET(reg)                    ((reg & VIN_CH_LMCR_LM_EN) >> 11)
#define VIN_CH_LMCR_LM_EN_SET(reg, val)               (reg = ((reg & ~VIN_CH_LMCR_LM_EN) | (((val) & 0x0001) << 11)))
#define VIN_CH_LMCR_LM2PCM                            (1 << 10)
#define VIN_CH_LMCR_LM2PCM_GET(reg)                   ((reg & VIN_CH_LMCR_LM2PCM) >> 10)
#define VIN_CH_LMCR_LM2PCM_SET(reg, val)              (reg = ((reg & ~VIN_CH_LMCR_LM2PCM) | (((val) & 0x0001) << 10)))
#define VIN_CH_LMCR_LM_ONCE                           (1 << 9)
#define VIN_CH_LMCR_LM_ONCE_GET(reg)                  ((reg & VIN_CH_LMCR_LM_ONCE) >> 9)
#define VIN_CH_LMCR_LM_ONCE_SET(reg, val)             (reg = ((reg & ~VIN_CH_LMCR_LM_ONCE) | (((val) & 0x0001) << 9)))
#define VIN_CH_LMCR_DC_AD16                           (1 << 8)
#define VIN_CH_LMCR_DC_AD16_GET(reg)                  ((reg & VIN_CH_LMCR_DC_AD16) >> 8)
#define VIN_CH_LMCR_DC_AD16_SET(reg, val)             (reg = ((reg & ~VIN_CH_LMCR_DC_AD16) | (((val) & 0x0001) << 8)))
#define VIN_CH_LMCR_LM_FILT_SEL                       (1 << 7)
#define VIN_CH_LMCR_LM_FILT_SEL_GET(reg)              ((reg & VIN_CH_LMCR_LM_FILT_SEL) >> 7)
#define VIN_CH_LMCR_LM_FILT_SEL_SET(reg, val)         (reg = ((reg & ~VIN_CH_LMCR_LM_FILT_SEL) | (((val) & 0x0001) << 7)))
#define VIN_CH_LMCR_LM_FILT                           (1 << 6)
#define VIN_CH_LMCR_LM_FILT_GET(reg)                  ((reg & VIN_CH_LMCR_LM_FILT) >> 6)
#define VIN_CH_LMCR_LM_FILT_SET(reg, val)             (reg = ((reg & ~VIN_CH_LMCR_LM_FILT) | (((val) & 0x0001) << 6)))
#define VIN_CH_LMCR_LM_RECT                           (1 << 5)
#define VIN_CH_LMCR_LM_RECT_GET(reg)                  ((reg & VIN_CH_LMCR_LM_RECT) >> 5)
#define VIN_CH_LMCR_LM_RECT_SET(reg, val)             (reg = ((reg & ~VIN_CH_LMCR_LM_RECT) | (((val) & 0x0001) << 5)))
#define VIN_CH_LMCR_RAMP_EN                           (1 << 4)
#define VIN_CH_LMCR_RAMP_EN_GET(reg)                  ((reg & VIN_CH_LMCR_RAMP_EN) >> 4)
#define VIN_CH_LMCR_RAMP_EN_SET(reg, val)             (reg = ((reg & ~VIN_CH_LMCR_RAMP_EN) | (((val) & 0x0001) << 4)))
#define VIN_CH_LMCR_LM_SEL_MASK                       (((1 << 4) - 1) << 0)
#define VIN_CH_LMCR_LM_SEL_GET(reg)                   (((reg) >> 0) & ((1 << 4) - 1))
#define VIN_CH_LMCR_LM_SEL_SET(val)                   ((((1 << 4) - 1) & (val)) << 0)
#define VIN_CH_LMCR_LM_SEL_MODIFY(reg, val)           (reg = ( (reg & ~VIN_CH_LMCR_LM_SEL_MASK) | ((((1 << 4) - 1) & (val)) << 0) ))

/***Ring Configuration Register***/

#define VIN_CH_RTR                                    ( (VIN_CH + 0x0b) )

#define VIN_CH_RTR_RES15                              (1 << 15)
#define VIN_CH_RTR_RES15_GET(reg)                     ((reg & VIN_CH_RTR_RES15) >> 15)
#define VIN_CH_RTR_RES15_SET(reg, val)                (reg = ((reg & ~VIN_CH_RTR_RES15) | (((val) & 0x0001) << 15)))
#define VIN_CH_RTR_RES14                              (1 << 14)
#define VIN_CH_RTR_RES14_GET(reg)                     ((reg & VIN_CH_RTR_RES14) >> 14)
#define VIN_CH_RTR_RES14_SET(reg, val)                (reg = ((reg & ~VIN_CH_RTR_RES14) | (((val) & 0x0001) << 14)))
#define VIN_CH_RTR_RES13                              (1 << 13)
#define VIN_CH_RTR_RES13_GET(reg)                     ((reg & VIN_CH_RTR_RES13) >> 13)
#define VIN_CH_RTR_RES13_SET(reg, val)                (reg = ((reg & ~VIN_CH_RTR_RES13) | (((val) & 0x0001) << 13)))
#define VIN_CH_RTR_RES12                              (1 << 12)
#define VIN_CH_RTR_RES12_GET(reg)                     ((reg & VIN_CH_RTR_RES12) >> 12)
#define VIN_CH_RTR_RES12_SET(reg, val)                (reg = ((reg & ~VIN_CH_RTR_RES12) | (((val) & 0x0001) << 12)))
#define VIN_CH_RTR_RES11                              (1 << 11)
#define VIN_CH_RTR_RES11_GET(reg)                     ((reg & VIN_CH_RTR_RES11) >> 11)
#define VIN_CH_RTR_RES11_SET(reg, val)                (reg = ((reg & ~VIN_CH_RTR_RES11) | (((val) & 0x0001) << 11)))
#define VIN_CH_RTR_RES10                              (1 << 10)
#define VIN_CH_RTR_RES10_GET(reg)                     ((reg & VIN_CH_RTR_RES10) >> 10)
#define VIN_CH_RTR_RES10_SET(reg, val)                (reg = ((reg & ~VIN_CH_RTR_RES10) | (((val) & 0x0001) << 10)))
#define VIN_CH_RTR_RES9                               (1 << 9)
#define VIN_CH_RTR_RES9_GET(reg)                      ((reg & VIN_CH_RTR_RES9) >> 9)
#define VIN_CH_RTR_RES9_SET(reg, val)                 (reg = ((reg & ~VIN_CH_RTR_RES9) | (((val) & 0x0001) << 9)))
#define VIN_CH_RTR_RES8                               (1 << 8)
#define VIN_CH_RTR_RES8_GET(reg)                      ((reg & VIN_CH_RTR_RES8) >> 8)
#define VIN_CH_RTR_RES8_SET(reg, val)                 (reg = ((reg & ~VIN_CH_RTR_RES8) | (((val) & 0x0001) << 8)))
#define VIN_CH_RTR_RTR_EXTS                           (1 << 7)
#define VIN_CH_RTR_RTR_EXTS_GET(reg)                  ((reg & VIN_CH_RTR_RTR_EXTS) >> 7)
#define VIN_CH_RTR_RTR_EXTS_SET(reg, val)             (reg = ((reg & ~VIN_CH_RTR_RTR_EXTS) | (((val) & 0x0001) << 7)))
#define VIN_CH_RTR_RTR_SEL                            (1 << 6)
#define VIN_CH_RTR_RTR_SEL_GET(reg)                   ((reg & VIN_CH_RTR_RTR_SEL) >> 6)
#define VIN_CH_RTR_RTR_SEL_SET(reg, val)              (reg = ((reg & ~VIN_CH_RTR_RTR_SEL) | (((val) & 0x0001) << 6)))
#define VIN_CH_RTR_RTR_FAST                           (1 << 5)
#define VIN_CH_RTR_RTR_FAST_GET(reg)                  ((reg & VIN_CH_RTR_RTR_FAST) >> 5)
#define VIN_CH_RTR_RTR_FAST_SET(reg, val)             (reg = ((reg & ~VIN_CH_RTR_RTR_FAST) | (((val) & 0x0001) << 5)))
#define VIN_CH_RTR_PCM2DC                             (1 << 4)
#define VIN_CH_RTR_PCM2DC_GET(reg)                    ((reg & VIN_CH_RTR_PCM2DC) >> 4)
#define VIN_CH_RTR_PCM2DC_SET(reg, val)               (reg = ((reg & ~VIN_CH_RTR_PCM2DC) | (((val) & 0x0001) << 4)))
#define VIN_CH_RTR_RING_OFFSET_MASK                   (((1 << 2) - 1) << 2)
#define VIN_CH_RTR_RING_OFFSET_GET(reg)               (((reg) >> 2) & ((1 << 2) - 1))
#define VIN_CH_RTR_RING_OFFSET_SET(val)               ((((1 << 2) - 1) & (val)) << 2)
#define VIN_CH_RTR_RING_OFFSET_MODIFY(reg, val)       (reg = ( (reg & ~VIN_CH_RTR_RING_OFFSET_MASK) | ((((1 << 2) - 1) & (val)) << 2) ))
#define VIN_CH_RTR_ASYNCH_R                           (1 << 1)
#define VIN_CH_RTR_ASYNCH_R_GET(reg)                  ((reg & VIN_CH_RTR_ASYNCH_R) >> 1)
#define VIN_CH_RTR_ASYNCH_R_SET(reg, val)             (reg = ((reg & ~VIN_CH_RTR_ASYNCH_R) | (((val) & 0x0001) << 1)))
#define VIN_CH_RTR_REXT_EN                            (1 << 0)
#define VIN_CH_RTR_REXT_EN_GET(reg)                   ((reg & VIN_CH_RTR_REXT_EN) >> 0)
#define VIN_CH_RTR_REXT_EN_SET(reg, val)              (reg = ((reg & ~VIN_CH_RTR_REXT_EN) | (((val) & 0x0001) << 0)))

/***DC Offset Register***/

#define VIN_CH_OFR                                    ( (VIN_CH + 0x0c) )

#define VIN_CH_OFR_DC_OFFSET

/***Automatic Mode Register***/

#define VIN_CH_AUTOMOD                                ( (VIN_CH + 0x0d) )

#define VIN_CH_AUTOMOD_RES15                          (1 << 15)
#define VIN_CH_AUTOMOD_RES15_GET(reg)                 ((reg & VIN_CH_AUTOMOD_RES15) >> 15)
#define VIN_CH_AUTOMOD_RES15_SET(reg, val)            (reg = ((reg & ~VIN_CH_AUTOMOD_RES15) | (((val) & 0x0001) << 15)))
#define VIN_CH_AUTOMOD_RES14                          (1 << 14)
#define VIN_CH_AUTOMOD_RES14_GET(reg)                 ((reg & VIN_CH_AUTOMOD_RES14) >> 14)
#define VIN_CH_AUTOMOD_RES14_SET(reg, val)            (reg = ((reg & ~VIN_CH_AUTOMOD_RES14) | (((val) & 0x0001) << 14)))
#define VIN_CH_AUTOMOD_RES13                          (1 << 13)
#define VIN_CH_AUTOMOD_RES13_GET(reg)                 ((reg & VIN_CH_AUTOMOD_RES13) >> 13)
#define VIN_CH_AUTOMOD_RES13_SET(reg, val)            (reg = ((reg & ~VIN_CH_AUTOMOD_RES13) | (((val) & 0x0001) << 13)))
#define VIN_CH_AUTOMOD_RES12                          (1 << 12)
#define VIN_CH_AUTOMOD_RES12_GET(reg)                 ((reg & VIN_CH_AUTOMOD_RES12) >> 12)
#define VIN_CH_AUTOMOD_RES12_SET(reg, val)            (reg = ((reg & ~VIN_CH_AUTOMOD_RES12) | (((val) & 0x0001) << 12)))
#define VIN_CH_AUTOMOD_RES11                          (1 << 11)
#define VIN_CH_AUTOMOD_RES11_GET(reg)                 ((reg & VIN_CH_AUTOMOD_RES11) >> 11)
#define VIN_CH_AUTOMOD_RES11_SET(reg, val)            (reg = ((reg & ~VIN_CH_AUTOMOD_RES11) | (((val) & 0x0001) << 11)))
#define VIN_CH_AUTOMOD_RES10                          (1 << 10)
#define VIN_CH_AUTOMOD_RES10_GET(reg)                 ((reg & VIN_CH_AUTOMOD_RES10) >> 10)
#define VIN_CH_AUTOMOD_RES10_SET(reg, val)            (reg = ((reg & ~VIN_CH_AUTOMOD_RES10) | (((val) & 0x0001) << 10)))
#define VIN_CH_AUTOMOD_RES9                           (1 << 9)
#define VIN_CH_AUTOMOD_RES9_GET(reg)                  ((reg & VIN_CH_AUTOMOD_RES9) >> 9)
#define VIN_CH_AUTOMOD_RES9_SET(reg, val)             (reg = ((reg & ~VIN_CH_AUTOMOD_RES9) | (((val) & 0x0001) << 9)))
#define VIN_CH_AUTOMOD_RES8                           (1 << 8)
#define VIN_CH_AUTOMOD_RES8_GET(reg)                  ((reg & VIN_CH_AUTOMOD_RES8) >> 8)
#define VIN_CH_AUTOMOD_RES8_SET(reg, val)             (reg = ((reg & ~VIN_CH_AUTOMOD_RES8) | (((val) & 0x0001) << 8)))
#define VIN_CH_AUTOMOD_RES7                           (1 << 7)
#define VIN_CH_AUTOMOD_RES7_GET(reg)                  ((reg & VIN_CH_AUTOMOD_RES7) >> 7)
#define VIN_CH_AUTOMOD_RES7_SET(reg, val)             (reg = ((reg & ~VIN_CH_AUTOMOD_RES7) | (((val) & 0x0001) << 7)))
#define VIN_CH_AUTOMOD_RES6                           (1 << 6)
#define VIN_CH_AUTOMOD_RES6_GET(reg)                  ((reg & VIN_CH_AUTOMOD_RES6) >> 6)
#define VIN_CH_AUTOMOD_RES6_SET(reg, val)             (reg = ((reg & ~VIN_CH_AUTOMOD_RES6) | (((val) & 0x0001) << 6)))
#define VIN_CH_AUTOMOD_RES5                           (1 << 5)
#define VIN_CH_AUTOMOD_RES5_GET(reg)                  ((reg & VIN_CH_AUTOMOD_RES5) >> 5)
#define VIN_CH_AUTOMOD_RES5_SET(reg, val)             (reg = ((reg & ~VIN_CH_AUTOMOD_RES5) | (((val) & 0x0001) << 5)))
#define VIN_CH_AUTOMOD_AUTO_MET_START                 (1 << 4)
#define VIN_CH_AUTOMOD_AUTO_MET_START_GET(reg)        ((reg & VIN_CH_AUTOMOD_AUTO_MET_START) >> 4)
#define VIN_CH_AUTOMOD_AUTO_MET_START_SET(reg, val)   (reg = ((reg & ~VIN_CH_AUTOMOD_AUTO_MET_START) | (((val) & 0x0001) << 4)))
#define VIN_CH_AUTOMOD_AUTO_MET_EN                    (1 << 3)
#define VIN_CH_AUTOMOD_AUTO_MET_EN_GET(reg)           ((reg & VIN_CH_AUTOMOD_AUTO_MET_EN) >> 3)
#define VIN_CH_AUTOMOD_AUTO_MET_EN_SET(reg, val)      (reg = ((reg & ~VIN_CH_AUTOMOD_AUTO_MET_EN) | (((val) & 0x0001) << 3)))
#define VIN_CH_AUTOMOD_AUTO_OFFH                      (1 << 2)
#define VIN_CH_AUTOMOD_AUTO_OFFH_GET(reg)             ((reg & VIN_CH_AUTOMOD_AUTO_OFFH) >> 2)
#define VIN_CH_AUTOMOD_AUTO_OFFH_SET(reg, val)        (reg = ((reg & ~VIN_CH_AUTOMOD_AUTO_OFFH) | (((val) & 0x0001) << 2)))
#define VIN_CH_AUTOMOD_AUTO_BAT                       (1 << 1)
#define VIN_CH_AUTOMOD_AUTO_BAT_GET(reg)              ((reg & VIN_CH_AUTOMOD_AUTO_BAT) >> 1)
#define VIN_CH_AUTOMOD_AUTO_BAT_SET(reg, val)         (reg = ((reg & ~VIN_CH_AUTOMOD_AUTO_BAT) | (((val) & 0x0001) << 1)))
#define VIN_CH_AUTOMOD_PDOT_DIS                       (1 << 0)
#define VIN_CH_AUTOMOD_PDOT_DIS_GET(reg)              ((reg & VIN_CH_AUTOMOD_PDOT_DIS) >> 0)
#define VIN_CH_AUTOMOD_PDOT_DIS_SET(reg, val)         (reg = ((reg & ~VIN_CH_AUTOMOD_PDOT_DIS) | (((val) & 0x0001) << 0)))

/***Test Register 1***/

#define VIN_CH_TSTR1                                  ( (VIN_CH + 0x0e) )

#define VIN_CH_TSTR1_PD_AC_PR                         (1 << 15)
#define VIN_CH_TSTR1_PD_AC_PR_GET(reg)                ((reg & VIN_CH_TSTR1_PD_AC_PR) >> 15)
#define VIN_CH_TSTR1_PD_AC_PR_SET(reg, val)           (reg = ((reg & ~VIN_CH_TSTR1_PD_AC_PR) | (((val) & 0x0001) << 15)))
#define VIN_CH_TSTR1_PD_AC_PO                         (1 << 14)
#define VIN_CH_TSTR1_PD_AC_PO_GET(reg)                ((reg & VIN_CH_TSTR1_PD_AC_PO) >> 14)
#define VIN_CH_TSTR1_PD_AC_PO_SET(reg, val)           (reg = ((reg & ~VIN_CH_TSTR1_PD_AC_PO) | (((val) & 0x0001) << 14)))
#define VIN_CH_TSTR1_PD_AC_AD                         (1 << 13)
#define VIN_CH_TSTR1_PD_AC_AD_GET(reg)                ((reg & VIN_CH_TSTR1_PD_AC_AD) >> 13)
#define VIN_CH_TSTR1_PD_AC_AD_SET(reg, val)           (reg = ((reg & ~VIN_CH_TSTR1_PD_AC_AD) | (((val) & 0x0001) << 13)))
#define VIN_CH_TSTR1_PD_AC_DA                         (1 << 12)
#define VIN_CH_TSTR1_PD_AC_DA_GET(reg)                ((reg & VIN_CH_TSTR1_PD_AC_DA) >> 12)
#define VIN_CH_TSTR1_PD_AC_DA_SET(reg, val)           (reg = ((reg & ~VIN_CH_TSTR1_PD_AC_DA) | (((val) & 0x0001) << 12)))
#define VIN_CH_TSTR1_PD_DC_PR                         (1 << 11)
#define VIN_CH_TSTR1_PD_DC_PR_GET(reg)                ((reg & VIN_CH_TSTR1_PD_DC_PR) >> 11)
#define VIN_CH_TSTR1_PD_DC_PR_SET(reg, val)           (reg = ((reg & ~VIN_CH_TSTR1_PD_DC_PR) | (((val) & 0x0001) << 11)))
#define VIN_CH_TSTR1_PD_DC_AD                         (1 << 10)
#define VIN_CH_TSTR1_PD_DC_AD_GET(reg)                ((reg & VIN_CH_TSTR1_PD_DC_AD) >> 10)
#define VIN_CH_TSTR1_PD_DC_AD_SET(reg, val)           (reg = ((reg & ~VIN_CH_TSTR1_PD_DC_AD) | (((val) & 0x0001) << 10)))
#define VIN_CH_TSTR1_PD_DC_DA                         (1 << 9)
#define VIN_CH_TSTR1_PD_DC_DA_GET(reg)                ((reg & VIN_CH_TSTR1_PD_DC_DA) >> 9)
#define VIN_CH_TSTR1_PD_DC_DA_SET(reg, val)           (reg = ((reg & ~VIN_CH_TSTR1_PD_DC_DA) | (((val) & 0x0001) << 9)))
#define VIN_CH_TSTR1_PD_DCBUF                         (1 << 8)
#define VIN_CH_TSTR1_PD_DCBUF_GET(reg)                ((reg & VIN_CH_TSTR1_PD_DCBUF) >> 8)
#define VIN_CH_TSTR1_PD_DCBUF_SET(reg, val)           (reg = ((reg & ~VIN_CH_TSTR1_PD_DCBUF) | (((val) & 0x0001) << 8)))
#define VIN_CH_TSTR1_PD_ACDITH                        (1 << 7)
#define VIN_CH_TSTR1_PD_ACDITH_GET(reg)               ((reg & VIN_CH_TSTR1_PD_ACDITH) >> 7)
#define VIN_CH_TSTR1_PD_ACDITH_SET(reg, val)          (reg = ((reg & ~VIN_CH_TSTR1_PD_ACDITH) | (((val) & 0x0001) << 7)))
#define VIN_CH_TSTR1_PD_DCREF                         (1 << 6)
#define VIN_CH_TSTR1_PD_DCREF_GET(reg)                ((reg & VIN_CH_TSTR1_PD_DCREF) >> 6)
#define VIN_CH_TSTR1_PD_DCREF_SET(reg, val)           (reg = ((reg & ~VIN_CH_TSTR1_PD_DCREF) | (((val) & 0x0001) << 6)))
#define VIN_CH_TSTR1_PD_GNKC                          (1 << 5)
#define VIN_CH_TSTR1_PD_GNKC_GET(reg)                 ((reg & VIN_CH_TSTR1_PD_GNKC) >> 5)
#define VIN_CH_TSTR1_PD_GNKC_SET(reg, val)            (reg = ((reg & ~VIN_CH_TSTR1_PD_GNKC) | (((val) & 0x0001) << 5)))
#define VIN_CH_TSTR1_PD_OFHC                          (1 << 4)
#define VIN_CH_TSTR1_PD_OFHC_GET(reg)                 ((reg & VIN_CH_TSTR1_PD_OFHC) >> 4)
#define VIN_CH_TSTR1_PD_OFHC_SET(reg, val)            (reg = ((reg & ~VIN_CH_TSTR1_PD_OFHC) | (((val) & 0x0001) << 4)))
#define VIN_CH_TSTR1_PD_OVTC                          (1 << 3)
#define VIN_CH_TSTR1_PD_OVTC_GET(reg)                 ((reg & VIN_CH_TSTR1_PD_OVTC) >> 3)
#define VIN_CH_TSTR1_PD_OVTC_SET(reg, val)            (reg = ((reg & ~VIN_CH_TSTR1_PD_OVTC) | (((val) & 0x0001) << 3)))
#define VIN_CH_TSTR1_PD_ACREF                         (1 << 2)
#define VIN_CH_TSTR1_PD_ACREF_GET(reg)                ((reg & VIN_CH_TSTR1_PD_ACREF) >> 2)
#define VIN_CH_TSTR1_PD_ACREF_SET(reg, val)           (reg = ((reg & ~VIN_CH_TSTR1_PD_ACREF) | (((val) & 0x0001) << 2)))
#define VIN_CH_TSTR1_PD_IREF                          (1 << 1)
#define VIN_CH_TSTR1_PD_IREF_GET(reg)                 ((reg & VIN_CH_TSTR1_PD_IREF) >> 1)
#define VIN_CH_TSTR1_PD_IREF_SET(reg, val)            (reg = ((reg & ~VIN_CH_TSTR1_PD_IREF) | (((val) & 0x0001) << 1)))
#define VIN_CH_TSTR1_PD_HVI                           (1 << 0)
#define VIN_CH_TSTR1_PD_HVI_GET(reg)                  ((reg & VIN_CH_TSTR1_PD_HVI) >> 0)
#define VIN_CH_TSTR1_PD_HVI_SET(reg, val)             (reg = ((reg & ~VIN_CH_TSTR1_PD_HVI) | (((val) & 0x0001) << 0)))

/***Test Register 2***/

#define VIN_CH_TSTR2                                  ( (VIN_CH + 0x0f) )

#define VIN_CH_TSTR2_AC_ALB_PP                        (1 << 14)
#define VIN_CH_TSTR2_AC_ALB_PP_GET(reg)               ((reg & VIN_CH_TSTR2_AC_ALB_PP) >> 14)
#define VIN_CH_TSTR2_AC_ALB_PP_SET(reg, val)          (reg = ((reg & ~VIN_CH_TSTR2_AC_ALB_PP) | (((val) & 0x0001) << 14)))
#define VIN_CH_TSTR2_AC_ALB_16M                       (1 << 13)
#define VIN_CH_TSTR2_AC_ALB_16M_GET(reg)              ((reg & VIN_CH_TSTR2_AC_ALB_16M) >> 13)
#define VIN_CH_TSTR2_AC_ALB_16M_SET(reg, val)         (reg = ((reg & ~VIN_CH_TSTR2_AC_ALB_16M) | (((val) & 0x0001) << 13)))
#define VIN_CH_TSTR2_AC_DLB_16M                       (1 << 12)
#define VIN_CH_TSTR2_AC_DLB_16M_GET(reg)              ((reg & VIN_CH_TSTR2_AC_DLB_16M) >> 12)
#define VIN_CH_TSTR2_AC_DLB_16M_SET(reg, val)         (reg = ((reg & ~VIN_CH_TSTR2_AC_DLB_16M) | (((val) & 0x0001) << 12)))
#define VIN_CH_TSTR2_AC_DLB_128K                      (1 << 11)
#define VIN_CH_TSTR2_AC_DLB_128K_GET(reg)             ((reg & VIN_CH_TSTR2_AC_DLB_128K) >> 11)
#define VIN_CH_TSTR2_AC_DLB_128K_SET(reg, val)        (reg = ((reg & ~VIN_CH_TSTR2_AC_DLB_128K) | (((val) & 0x0001) << 11)))
#define VIN_CH_TSTR2_AC_DLB_32K                       (1 << 10)
#define VIN_CH_TSTR2_AC_DLB_32K_GET(reg)              ((reg & VIN_CH_TSTR2_AC_DLB_32K) >> 10)
#define VIN_CH_TSTR2_AC_DLB_32K_SET(reg, val)         (reg = ((reg & ~VIN_CH_TSTR2_AC_DLB_32K) | (((val) & 0x0001) << 10)))
#define VIN_CH_TSTR2_AC_DLB_8K                        (1 << 9)
#define VIN_CH_TSTR2_AC_DLB_8K_GET(reg)               ((reg & VIN_CH_TSTR2_AC_DLB_8K) >> 9)
#define VIN_CH_TSTR2_AC_DLB_8K_SET(reg, val)          (reg = ((reg & ~VIN_CH_TSTR2_AC_DLB_8K) | (((val) & 0x0001) << 9)))
#define VIN_CH_TSTR2_AC_SHORT                         (1 << 8)
#define VIN_CH_TSTR2_AC_SHORT_GET(reg)                ((reg & VIN_CH_TSTR2_AC_SHORT) >> 8)
#define VIN_CH_TSTR2_AC_SHORT_SET(reg, val)           (reg = ((reg & ~VIN_CH_TSTR2_AC_SHORT) | (((val) & 0x0001) << 8)))
#define VIN_CH_TSTR2_AC_RGAIN                         (1 << 7)
#define VIN_CH_TSTR2_AC_RGAIN_GET(reg)                ((reg & VIN_CH_TSTR2_AC_RGAIN) >> 7)
#define VIN_CH_TSTR2_AC_RGAIN_SET(reg, val)           (reg = ((reg & ~VIN_CH_TSTR2_AC_RGAIN) | (((val) & 0x0001) << 7)))
#define VIN_CH_TSTR2_AC_XGAIN                         (1 << 6)
#define VIN_CH_TSTR2_AC_XGAIN_GET(reg)                ((reg & VIN_CH_TSTR2_AC_XGAIN) >> 6)
#define VIN_CH_TSTR2_AC_XGAIN_SET(reg, val)           (reg = ((reg & ~VIN_CH_TSTR2_AC_XGAIN) | (((val) & 0x0001) << 6)))
#define VIN_CH_TSTR2_LOW_PERF_MASK                    (((1 << 2) - 1) << 4)
#define VIN_CH_TSTR2_LOW_PERF_GET(reg)                (((reg) >> 4) & ((1 << 2) - 1))
#define VIN_CH_TSTR2_LOW_PERF_SET(val)                ((((1 << 2) - 1) & (val)) << 4)
#define VIN_CH_TSTR2_LOW_PERF_MODIFY(reg, val)        (reg = ( (reg & ~VIN_CH_TSTR2_LOW_PERF_MASK) | ((((1 << 2) - 1) & (val)) << 4) ))
#define VIN_CH_TSTR2_OPIM_AN                          (1 << 3)
#define VIN_CH_TSTR2_OPIM_AN_GET(reg)                 ((reg & VIN_CH_TSTR2_OPIM_AN) >> 3)
#define VIN_CH_TSTR2_OPIM_AN_SET(reg, val)            (reg = ((reg & ~VIN_CH_TSTR2_OPIM_AN) | (((val) & 0x0001) << 3)))
#define VIN_CH_TSTR2_OPIM_HW                          (1 << 2)
#define VIN_CH_TSTR2_OPIM_HW_GET(reg)                 ((reg & VIN_CH_TSTR2_OPIM_HW) >> 2)
#define VIN_CH_TSTR2_OPIM_HW_SET(reg, val)            (reg = ((reg & ~VIN_CH_TSTR2_OPIM_HW) | (((val) & 0x0001) << 2)))
#define VIN_CH_TSTR2_COR_64                           (1 << 1)
#define VIN_CH_TSTR2_COR_64_GET(reg)                  ((reg & VIN_CH_TSTR2_COR_64) >> 1)
#define VIN_CH_TSTR2_COR_64_SET(reg, val)             (reg = ((reg & ~VIN_CH_TSTR2_COR_64) | (((val) & 0x0001) << 1)))
#define VIN_CH_TSTR2_COX_16                           (1 << 0)
#define VIN_CH_TSTR2_COX_16_GET(reg)                  ((reg & VIN_CH_TSTR2_COX_16) >> 0)
#define VIN_CH_TSTR2_COX_16_SET(reg, val)             (reg = ((reg & ~VIN_CH_TSTR2_COX_16) | (((val) & 0x0001) << 0)))

/***Test Register 3***/

#define VIN_CH_TSTR3                                  ( (VIN_CH + 0x10) )

#define VIN_CH_TSTR3_DC_ALB_PP                        (1 << 15)
#define VIN_CH_TSTR3_DC_ALB_PP_GET(reg)               ((reg & VIN_CH_TSTR3_DC_ALB_PP) >> 15)
#define VIN_CH_TSTR3_DC_ALB_PP_SET(reg, val)          (reg = ((reg & ~VIN_CH_TSTR3_DC_ALB_PP) | (((val) & 0x0001) << 15)))
#define VIN_CH_TSTR3_DC_ALB_1M                        (1 << 14)
#define VIN_CH_TSTR3_DC_ALB_1M_GET(reg)               ((reg & VIN_CH_TSTR3_DC_ALB_1M) >> 14)
#define VIN_CH_TSTR3_DC_ALB_1M_SET(reg, val)          (reg = ((reg & ~VIN_CH_TSTR3_DC_ALB_1M) | (((val) & 0x0001) << 14)))
#define VIN_CH_TSTR3_DC_DLB_1M                        (1 << 13)
#define VIN_CH_TSTR3_DC_DLB_1M_GET(reg)               ((reg & VIN_CH_TSTR3_DC_DLB_1M) >> 13)
#define VIN_CH_TSTR3_DC_DLB_1M_SET(reg, val)          (reg = ((reg & ~VIN_CH_TSTR3_DC_DLB_1M) | (((val) & 0x0001) << 13)))
#define VIN_CH_TSTR3_DC_POFI_HI                       (1 << 12)
#define VIN_CH_TSTR3_DC_POFI_HI_GET(reg)              ((reg & VIN_CH_TSTR3_DC_POFI_HI) >> 12)
#define VIN_CH_TSTR3_DC_POFI_HI_SET(reg, val)         (reg = ((reg & ~VIN_CH_TSTR3_DC_POFI_HI) | (((val) & 0x0001) << 12)))
#define VIN_CH_TSTR3_DC_HOLD                          (1 << 11)
#define VIN_CH_TSTR3_DC_HOLD_GET(reg)                 ((reg & VIN_CH_TSTR3_DC_HOLD) >> 11)
#define VIN_CH_TSTR3_DC_HOLD_SET(reg, val)            (reg = ((reg & ~VIN_CH_TSTR3_DC_HOLD) | (((val) & 0x0001) << 11)))
#define VIN_CH_TSTR3_DCC_DIS                          (1 << 10)
#define VIN_CH_TSTR3_DCC_DIS_GET(reg)                 ((reg & VIN_CH_TSTR3_DCC_DIS) >> 10)
#define VIN_CH_TSTR3_DCC_DIS_SET(reg, val)            (reg = ((reg & ~VIN_CH_TSTR3_DCC_DIS) | (((val) & 0x0001) << 10)))
#define VIN_CH_TSTR3_DC_LP2_DIS                       (1 << 9)
#define VIN_CH_TSTR3_DC_LP2_DIS_GET(reg)              ((reg & VIN_CH_TSTR3_DC_LP2_DIS) >> 9)
#define VIN_CH_TSTR3_DC_LP2_DIS_SET(reg, val)         (reg = ((reg & ~VIN_CH_TSTR3_DC_LP2_DIS) | (((val) & 0x0001) << 9)))
#define VIN_CH_TSTR3_DC_LP1_DIS                       (1 << 8)
#define VIN_CH_TSTR3_DC_LP1_DIS_GET(reg)              ((reg & VIN_CH_TSTR3_DC_LP1_DIS) >> 8)
#define VIN_CH_TSTR3_DC_LP1_DIS_SET(reg, val)         (reg = ((reg & ~VIN_CH_TSTR3_DC_LP1_DIS) | (((val) & 0x0001) << 8)))
#define VIN_CH_TSTR3_ATST_RES5                        (1 << 7)
#define VIN_CH_TSTR3_ATST_RES5_GET(reg)               ((reg & VIN_CH_TSTR3_ATST_RES5) >> 7)
#define VIN_CH_TSTR3_ATST_RES5_SET(reg, val)          (reg = ((reg & ~VIN_CH_TSTR3_ATST_RES5) | (((val) & 0x0001) << 7)))
#define VIN_CH_TSTR3_ATST_RES4                        (1 << 6)
#define VIN_CH_TSTR3_ATST_RES4_GET(reg)               ((reg & VIN_CH_TSTR3_ATST_RES4) >> 6)
#define VIN_CH_TSTR3_ATST_RES4_SET(reg, val)          (reg = ((reg & ~VIN_CH_TSTR3_ATST_RES4) | (((val) & 0x0001) << 6)))
#define VIN_CH_TSTR3_ATST_RES3                        (1 << 5)
#define VIN_CH_TSTR3_ATST_RES3_GET(reg)               ((reg & VIN_CH_TSTR3_ATST_RES3) >> 5)
#define VIN_CH_TSTR3_ATST_RES3_SET(reg, val)          (reg = ((reg & ~VIN_CH_TSTR3_ATST_RES3) | (((val) & 0x0001) << 5)))
#define VIN_CH_TSTR3_ATST_RES2                        (1 << 4)
#define VIN_CH_TSTR3_ATST_RES2_GET(reg)               ((reg & VIN_CH_TSTR3_ATST_RES2) >> 4)
#define VIN_CH_TSTR3_ATST_RES2_SET(reg, val)          (reg = ((reg & ~VIN_CH_TSTR3_ATST_RES2) | (((val) & 0x0001) << 4)))
#define VIN_CH_TSTR3_ATST_RES1                        (1 << 3)
#define VIN_CH_TSTR3_ATST_RES1_GET(reg)               ((reg & VIN_CH_TSTR3_ATST_RES1) >> 3)
#define VIN_CH_TSTR3_ATST_RES1_SET(reg, val)          (reg = ((reg & ~VIN_CH_TSTR3_ATST_RES1) | (((val) & 0x0001) << 3)))
#define VIN_CH_TSTR3_ATST_RES0                        (1 << 2)
#define VIN_CH_TSTR3_ATST_RES0_GET(reg)               ((reg & VIN_CH_TSTR3_ATST_RES0) >> 2)
#define VIN_CH_TSTR3_ATST_RES0_SET(reg, val)          (reg = ((reg & ~VIN_CH_TSTR3_ATST_RES0) | (((val) & 0x0001) << 2)))
#define VIN_CH_TSTR3_NO_DITH_AC                       (1 << 1)
#define VIN_CH_TSTR3_NO_DITH_AC_GET(reg)              ((reg & VIN_CH_TSTR3_NO_DITH_AC) >> 1)
#define VIN_CH_TSTR3_NO_DITH_AC_SET(reg, val)         (reg = ((reg & ~VIN_CH_TSTR3_NO_DITH_AC) | (((val) & 0x0001) << 1)))
#define VIN_CH_TSTR3_NO_AUTO_DITH_AC                  (1 << 0)
#define VIN_CH_TSTR3_NO_AUTO_DITH_AC_GET(reg)         ((reg & VIN_CH_TSTR3_NO_AUTO_DITH_AC) >> 0)
#define VIN_CH_TSTR3_NO_AUTO_DITH_AC_SET(reg, val)    (reg = ((reg & ~VIN_CH_TSTR3_NO_AUTO_DITH_AC) | (((val) & 0x0001) << 0)))

/***Level Metering Result2***/

#define VIN_CH_LMRES2                                 ( (VIN_CH + NULL) )

#define VIN_CH_LMRES2_LM_RES2

/***Mask Register for Status Register for Analog-Line-Mod
ule Interrupts 1***/

#define VIN_CH_MR_SRS1                                ( (VIN_CH + 0x02) )

#define VIN_CH_MR_SRS1_M_AUTO_MET_DONE                (1 << 15)
#define VIN_CH_MR_SRS1_M_AUTO_MET_DONE_GET(reg)       ((reg & VIN_CH_MR_SRS1_M_AUTO_MET_DONE) >> 15)
#define VIN_CH_MR_SRS1_M_AUTO_MET_DONE_SET(reg, val)  (reg = ((reg & ~VIN_CH_MR_SRS1_M_AUTO_MET_DONE) | (((val) & 0x0001) << 15)))
#define VIN_CH_MR_SRS1_M_RAMP_READY                   (1 << 14)
#define VIN_CH_MR_SRS1_M_RAMP_READY_GET(reg)          ((reg & VIN_CH_MR_SRS1_M_RAMP_READY) >> 14)
#define VIN_CH_MR_SRS1_M_RAMP_READY_SET(reg, val)     (reg = ((reg & ~VIN_CH_MR_SRS1_M_RAMP_READY) | (((val) & 0x0001) << 14)))
#define VIN_CH_MR_SRS1_M_HOOK                         (1 << 13)
#define VIN_CH_MR_SRS1_M_HOOK_GET(reg)                ((reg & VIN_CH_MR_SRS1_M_HOOK) >> 13)
#define VIN_CH_MR_SRS1_M_HOOK_SET(reg, val)           (reg = ((reg & ~VIN_CH_MR_SRS1_M_HOOK) | (((val) & 0x0001) << 13)))
#define VIN_CH_MR_SRS1_M_GNDK                         (1 << 12)
#define VIN_CH_MR_SRS1_M_GNDK_GET(reg)                ((reg & VIN_CH_MR_SRS1_M_GNDK) >> 12)
#define VIN_CH_MR_SRS1_M_GNDK_SET(reg, val)           (reg = ((reg & ~VIN_CH_MR_SRS1_M_GNDK) | (((val) & 0x0001) << 12)))
#define VIN_CH_MR_SRS1_M_GNKP                         (1 << 11)
#define VIN_CH_MR_SRS1_M_GNKP_GET(reg)                ((reg & VIN_CH_MR_SRS1_M_GNKP) >> 11)
#define VIN_CH_MR_SRS1_M_GNKP_SET(reg, val)           (reg = ((reg & ~VIN_CH_MR_SRS1_M_GNKP) | (((val) & 0x0001) << 11)))
#define VIN_CH_MR_SRS1_M_GNDKH                        (1 << 10)
#define VIN_CH_MR_SRS1_M_GNDKH_GET(reg)               ((reg & VIN_CH_MR_SRS1_M_GNDKH) >> 10)
#define VIN_CH_MR_SRS1_M_GNDKH_SET(reg, val)          (reg = ((reg & ~VIN_CH_MR_SRS1_M_GNDKH) | (((val) & 0x0001) << 10)))
#define VIN_CH_MR_SRS1_M_ICON                         (1 << 9)
#define VIN_CH_MR_SRS1_M_ICON_GET(reg)                ((reg & VIN_CH_MR_SRS1_M_ICON) >> 9)
#define VIN_CH_MR_SRS1_M_ICON_SET(reg, val)           (reg = ((reg & ~VIN_CH_MR_SRS1_M_ICON) | (((val) & 0x0001) << 9)))
#define VIN_CH_MR_SRS1_M_VTRLIM                       (1 << 8)
#define VIN_CH_MR_SRS1_M_VTRLIM_GET(reg)              ((reg & VIN_CH_MR_SRS1_M_VTRLIM) >> 8)
#define VIN_CH_MR_SRS1_M_VTRLIM_SET(reg, val)         (reg = ((reg & ~VIN_CH_MR_SRS1_M_VTRLIM) | (((val) & 0x0001) << 8)))
#define VIN_CH_MR_SRS1_RES7                           (1 << 7)
#define VIN_CH_MR_SRS1_RES7_GET(reg)                  ((reg & VIN_CH_MR_SRS1_RES7) >> 7)
#define VIN_CH_MR_SRS1_RES7_SET(reg, val)             (reg = ((reg & ~VIN_CH_MR_SRS1_RES7) | (((val) & 0x0001) << 7)))
#define VIN_CH_MR_SRS1_M_LM_THRES                     (1 << 6)
#define VIN_CH_MR_SRS1_M_LM_THRES_GET(reg)            ((reg & VIN_CH_MR_SRS1_M_LM_THRES) >> 6)
#define VIN_CH_MR_SRS1_M_LM_THRES_SET(reg, val)       (reg = ((reg & ~VIN_CH_MR_SRS1_M_LM_THRES) | (((val) & 0x0001) << 6)))
#define VIN_CH_MR_SRS1_M_LM_OK                        (1 << 5)
#define VIN_CH_MR_SRS1_M_LM_OK_GET(reg)               ((reg & VIN_CH_MR_SRS1_M_LM_OK) >> 5)
#define VIN_CH_MR_SRS1_M_LM_OK_SET(reg, val)          (reg = ((reg & ~VIN_CH_MR_SRS1_M_LM_OK) | (((val) & 0x0001) << 5)))
#define VIN_CH_MR_SRS1_M_DU_IO_MASK                   (((1 << 5) - 1) << 0)
#define VIN_CH_MR_SRS1_M_DU_IO_GET(reg)               (((reg) >> 0) & ((1 << 5) - 1))
#define VIN_CH_MR_SRS1_M_DU_IO_SET(val)               ((((1 << 5) - 1) & (val)) << 0)
#define VIN_CH_MR_SRS1_M_DU_IO_MODIFY(reg, val)       (reg = ( (reg & ~VIN_CH_MR_SRS1_M_DU_IO_MASK) | ((((1 << 5) - 1) & (val)) << 0) ))

/***Mask Register for Status Register for Analog-Line-Mod
ule Interrupts 2***/

#define VIN_CH_MR_SRS2                                ( (VIN_CH + 0x03) )

#define VIN_CH_MR_SRS2_RES15                          (1 << 15)
#define VIN_CH_MR_SRS2_RES15_GET(reg)                 ((reg & VIN_CH_MR_SRS2_RES15) >> 15)
#define VIN_CH_MR_SRS2_RES15_SET(reg, val)            (reg = ((reg & ~VIN_CH_MR_SRS2_RES15) | (((val) & 0x0001) << 15)))
#define VIN_CH_MR_SRS2_RES14                          (1 << 14)
#define VIN_CH_MR_SRS2_RES14_GET(reg)                 ((reg & VIN_CH_MR_SRS2_RES14) >> 14)
#define VIN_CH_MR_SRS2_RES14_SET(reg, val)            (reg = ((reg & ~VIN_CH_MR_SRS2_RES14) | (((val) & 0x0001) << 14)))
#define VIN_CH_MR_SRS2_RES13                          (1 << 13)
#define VIN_CH_MR_SRS2_RES13_GET(reg)                 ((reg & VIN_CH_MR_SRS2_RES13) >> 13)
#define VIN_CH_MR_SRS2_RES13_SET(reg, val)            (reg = ((reg & ~VIN_CH_MR_SRS2_RES13) | (((val) & 0x0001) << 13)))
#define VIN_CH_MR_SRS2_RES12                          (1 << 12)
#define VIN_CH_MR_SRS2_RES12_GET(reg)                 ((reg & VIN_CH_MR_SRS2_RES12) >> 12)
#define VIN_CH_MR_SRS2_RES12_SET(reg, val)            (reg = ((reg & ~VIN_CH_MR_SRS2_RES12) | (((val) & 0x0001) << 12)))
#define VIN_CH_MR_SRS2_RES11                          (1 << 11)
#define VIN_CH_MR_SRS2_RES11_GET(reg)                 ((reg & VIN_CH_MR_SRS2_RES11) >> 11)
#define VIN_CH_MR_SRS2_RES11_SET(reg, val)            (reg = ((reg & ~VIN_CH_MR_SRS2_RES11) | (((val) & 0x0001) << 11)))
#define VIN_CH_MR_SRS2_RES10                          (1 << 10)
#define VIN_CH_MR_SRS2_RES10_GET(reg)                 ((reg & VIN_CH_MR_SRS2_RES10) >> 10)
#define VIN_CH_MR_SRS2_RES10_SET(reg, val)            (reg = ((reg & ~VIN_CH_MR_SRS2_RES10) | (((val) & 0x0001) << 10)))
#define VIN_CH_MR_SRS2_RES9                           (1 << 9)
#define VIN_CH_MR_SRS2_RES9_GET(reg)                  ((reg & VIN_CH_MR_SRS2_RES9) >> 9)
#define VIN_CH_MR_SRS2_RES9_SET(reg, val)             (reg = ((reg & ~VIN_CH_MR_SRS2_RES9) | (((val) & 0x0001) << 9)))
#define VIN_CH_MR_SRS2_RES8                           (1 << 8)
#define VIN_CH_MR_SRS2_RES8_GET(reg)                  ((reg & VIN_CH_MR_SRS2_RES8) >> 8)
#define VIN_CH_MR_SRS2_RES8_SET(reg, val)             (reg = ((reg & ~VIN_CH_MR_SRS2_RES8) | (((val) & 0x0001) << 8)))
#define VIN_CH_MR_SRS2_RES7                           (1 << 7)
#define VIN_CH_MR_SRS2_RES7_GET(reg)                  ((reg & VIN_CH_MR_SRS2_RES7) >> 7)
#define VIN_CH_MR_SRS2_RES7_SET(reg, val)             (reg = ((reg & ~VIN_CH_MR_SRS2_RES7) | (((val) & 0x0001) << 7)))
#define VIN_CH_MR_SRS2_RES6                           (1 << 6)
#define VIN_CH_MR_SRS2_RES6_GET(reg)                  ((reg & VIN_CH_MR_SRS2_RES6) >> 6)
#define VIN_CH_MR_SRS2_RES6_SET(reg, val)             (reg = ((reg & ~VIN_CH_MR_SRS2_RES6) | (((val) & 0x0001) << 6)))
#define VIN_CH_MR_SRS2_RES5                           (1 << 5)
#define VIN_CH_MR_SRS2_RES5_GET(reg)                  ((reg & VIN_CH_MR_SRS2_RES5) >> 5)
#define VIN_CH_MR_SRS2_RES5_SET(reg, val)             (reg = ((reg & ~VIN_CH_MR_SRS2_RES5) | (((val) & 0x0001) << 5)))
#define VIN_CH_MR_SRS2_RES4                           (1 << 4)
#define VIN_CH_MR_SRS2_RES4_GET(reg)                  ((reg & VIN_CH_MR_SRS2_RES4) >> 4)
#define VIN_CH_MR_SRS2_RES4_SET(reg, val)             (reg = ((reg & ~VIN_CH_MR_SRS2_RES4) | (((val) & 0x0001) << 4)))
#define VIN_CH_MR_SRS2_M_OTEMP                        (1 << 3)
#define VIN_CH_MR_SRS2_M_OTEMP_GET(reg)               ((reg & VIN_CH_MR_SRS2_M_OTEMP) >> 3)
#define VIN_CH_MR_SRS2_M_OTEMP_SET(reg, val)          (reg = ((reg & ~VIN_CH_MR_SRS2_M_OTEMP) | (((val) & 0x0001) << 3)))
#define VIN_CH_MR_SRS2_M_CS_FAIL_CRAM                 (1 << 2)
#define VIN_CH_MR_SRS2_M_CS_FAIL_CRAM_GET(reg)        ((reg & VIN_CH_MR_SRS2_M_CS_FAIL_CRAM) >> 2)
#define VIN_CH_MR_SRS2_M_CS_FAIL_CRAM_SET(reg, val)   (reg = ((reg & ~VIN_CH_MR_SRS2_M_CS_FAIL_CRAM) | (((val) & 0x0001) << 2)))
#define VIN_CH_MR_SRS2_M_CS_FAIL_DSP                  (1 << 1)
#define VIN_CH_MR_SRS2_M_CS_FAIL_DSP_GET(reg)         ((reg & VIN_CH_MR_SRS2_M_CS_FAIL_DSP) >> 1)
#define VIN_CH_MR_SRS2_M_CS_FAIL_DSP_SET(reg, val)    (reg = ((reg & ~VIN_CH_MR_SRS2_M_CS_FAIL_DSP) | (((val) & 0x0001) << 1)))
#define VIN_CH_MR_SRS2_M_CS_FAIL_DCCTL                (1 << 0)
#define VIN_CH_MR_SRS2_M_CS_FAIL_DCCTL_GET(reg)       ((reg & VIN_CH_MR_SRS2_M_CS_FAIL_DCCTL) >> 0)
#define VIN_CH_MR_SRS2_M_CS_FAIL_DCCTL_SET(reg, val)  (reg = ((reg & ~VIN_CH_MR_SRS2_M_CS_FAIL_DCCTL) | (((val) & 0x0001) << 0)))

/***Mask Register for Status Register for Analog-Line-Mod
ule Interrupts 1***/

#define VIN_CH_MF_SRS1                                ( (VIN_CH + 0x62) )

#define VIN_CH_MF_SRS1_M_AUTO_MET_DONE                (1 << 15)
#define VIN_CH_MF_SRS1_M_AUTO_MET_DONE_GET(reg)       ((reg & VIN_CH_MF_SRS1_M_AUTO_MET_DONE) >> 15)
#define VIN_CH_MF_SRS1_M_AUTO_MET_DONE_SET(reg, val)  (reg = ((reg & ~VIN_CH_MF_SRS1_M_AUTO_MET_DONE) | (((val) & 0x0001) << 15)))
#define VIN_CH_MF_SRS1_M_RAMP_READY                   (1 << 14)
#define VIN_CH_MF_SRS1_M_RAMP_READY_GET(reg)          ((reg & VIN_CH_MF_SRS1_M_RAMP_READY) >> 14)
#define VIN_CH_MF_SRS1_M_RAMP_READY_SET(reg, val)     (reg = ((reg & ~VIN_CH_MF_SRS1_M_RAMP_READY) | (((val) & 0x0001) << 14)))
#define VIN_CH_MF_SRS1_M_HOOK                         (1 << 13)
#define VIN_CH_MF_SRS1_M_HOOK_GET(reg)                ((reg & VIN_CH_MF_SRS1_M_HOOK) >> 13)
#define VIN_CH_MF_SRS1_M_HOOK_SET(reg, val)           (reg = ((reg & ~VIN_CH_MF_SRS1_M_HOOK) | (((val) & 0x0001) << 13)))
#define VIN_CH_MF_SRS1_M_GNDK                         (1 << 12)
#define VIN_CH_MF_SRS1_M_GNDK_GET(reg)                ((reg & VIN_CH_MF_SRS1_M_GNDK) >> 12)
#define VIN_CH_MF_SRS1_M_GNDK_SET(reg, val)           (reg = ((reg & ~VIN_CH_MF_SRS1_M_GNDK) | (((val) & 0x0001) << 12)))
#define VIN_CH_MF_SRS1_M_GNKP                         (1 << 11)
#define VIN_CH_MF_SRS1_M_GNKP_GET(reg)                ((reg & VIN_CH_MF_SRS1_M_GNKP) >> 11)
#define VIN_CH_MF_SRS1_M_GNKP_SET(reg, val)           (reg = ((reg & ~VIN_CH_MF_SRS1_M_GNKP) | (((val) & 0x0001) << 11)))
#define VIN_CH_MF_SRS1_M_GNDKH                        (1 << 10)
#define VIN_CH_MF_SRS1_M_GNDKH_GET(reg)               ((reg & VIN_CH_MF_SRS1_M_GNDKH) >> 10)
#define VIN_CH_MF_SRS1_M_GNDKH_SET(reg, val)          (reg = ((reg & ~VIN_CH_MF_SRS1_M_GNDKH) | (((val) & 0x0001) << 10)))
#define VIN_CH_MF_SRS1_M_ICON                         (1 << 9)
#define VIN_CH_MF_SRS1_M_ICON_GET(reg)                ((reg & VIN_CH_MF_SRS1_M_ICON) >> 9)
#define VIN_CH_MF_SRS1_M_ICON_SET(reg, val)           (reg = ((reg & ~VIN_CH_MF_SRS1_M_ICON) | (((val) & 0x0001) << 9)))
#define VIN_CH_MF_SRS1_M_VTRLIM                       (1 << 8)
#define VIN_CH_MF_SRS1_M_VTRLIM_GET(reg)              ((reg & VIN_CH_MF_SRS1_M_VTRLIM) >> 8)
#define VIN_CH_MF_SRS1_M_VTRLIM_SET(reg, val)         (reg = ((reg & ~VIN_CH_MF_SRS1_M_VTRLIM) | (((val) & 0x0001) << 8)))
#define VIN_CH_MF_SRS1_RES7                           (1 << 7)
#define VIN_CH_MF_SRS1_RES7_GET(reg)                  ((reg & VIN_CH_MF_SRS1_RES7) >> 7)
#define VIN_CH_MF_SRS1_RES7_SET(reg, val)             (reg = ((reg & ~VIN_CH_MF_SRS1_RES7) | (((val) & 0x0001) << 7)))
#define VIN_CH_MF_SRS1_M_LM_THRES                     (1 << 6)
#define VIN_CH_MF_SRS1_M_LM_THRES_GET(reg)            ((reg & VIN_CH_MF_SRS1_M_LM_THRES) >> 6)
#define VIN_CH_MF_SRS1_M_LM_THRES_SET(reg, val)       (reg = ((reg & ~VIN_CH_MF_SRS1_M_LM_THRES) | (((val) & 0x0001) << 6)))
#define VIN_CH_MF_SRS1_M_LM_OK                        (1 << 5)
#define VIN_CH_MF_SRS1_M_LM_OK_GET(reg)               ((reg & VIN_CH_MF_SRS1_M_LM_OK) >> 5)
#define VIN_CH_MF_SRS1_M_LM_OK_SET(reg, val)          (reg = ((reg & ~VIN_CH_MF_SRS1_M_LM_OK) | (((val) & 0x0001) << 5)))
#define VIN_CH_MF_SRS1_M_DU_IO_MASK                   (((1 << 5) - 1) << 0)
#define VIN_CH_MF_SRS1_M_DU_IO_GET(reg)               (((reg) >> 0) & ((1 << 5) - 1))
#define VIN_CH_MF_SRS1_M_DU_IO_SET(val)               ((((1 << 5) - 1) & (val)) << 0)
#define VIN_CH_MF_SRS1_M_DU_IO_MODIFY(reg, val)       (reg = ( (reg & ~VIN_CH_MF_SRS1_M_DU_IO_MASK) | ((((1 << 5) - 1) & (val)) << 0) ))

/***Mask Register for Status Register for Analog-Line-Mod
ule Interrupts 2***/

#define VIN_CH_MF_SRS2                                ( (VIN_CH + 0x63) )

#define VIN_CH_MF_SRS2_RES15                          (1 << 15)
#define VIN_CH_MF_SRS2_RES15_GET(reg)                 ((reg & VIN_CH_MF_SRS2_RES15) >> 15)
#define VIN_CH_MF_SRS2_RES15_SET(reg, val)            (reg = ((reg & ~VIN_CH_MF_SRS2_RES15) | (((val) & 0x0001) << 15)))
#define VIN_CH_MF_SRS2_RES14                          (1 << 14)
#define VIN_CH_MF_SRS2_RES14_GET(reg)                 ((reg & VIN_CH_MF_SRS2_RES14) >> 14)
#define VIN_CH_MF_SRS2_RES14_SET(reg, val)            (reg = ((reg & ~VIN_CH_MF_SRS2_RES14) | (((val) & 0x0001) << 14)))
#define VIN_CH_MF_SRS2_RES13                          (1 << 13)
#define VIN_CH_MF_SRS2_RES13_GET(reg)                 ((reg & VIN_CH_MF_SRS2_RES13) >> 13)
#define VIN_CH_MF_SRS2_RES13_SET(reg, val)            (reg = ((reg & ~VIN_CH_MF_SRS2_RES13) | (((val) & 0x0001) << 13)))
#define VIN_CH_MF_SRS2_RES12                          (1 << 12)
#define VIN_CH_MF_SRS2_RES12_GET(reg)                 ((reg & VIN_CH_MF_SRS2_RES12) >> 12)
#define VIN_CH_MF_SRS2_RES12_SET(reg, val)            (reg = ((reg & ~VIN_CH_MF_SRS2_RES12) | (((val) & 0x0001) << 12)))
#define VIN_CH_MF_SRS2_RES11                          (1 << 11)
#define VIN_CH_MF_SRS2_RES11_GET(reg)                 ((reg & VIN_CH_MF_SRS2_RES11) >> 11)
#define VIN_CH_MF_SRS2_RES11_SET(reg, val)            (reg = ((reg & ~VIN_CH_MF_SRS2_RES11) | (((val) & 0x0001) << 11)))
#define VIN_CH_MF_SRS2_RES10                          (1 << 10)
#define VIN_CH_MF_SRS2_RES10_GET(reg)                 ((reg & VIN_CH_MF_SRS2_RES10) >> 10)
#define VIN_CH_MF_SRS2_RES10_SET(reg, val)            (reg = ((reg & ~VIN_CH_MF_SRS2_RES10) | (((val) & 0x0001) << 10)))
#define VIN_CH_MF_SRS2_RES9                           (1 << 9)
#define VIN_CH_MF_SRS2_RES9_GET(reg)                  ((reg & VIN_CH_MF_SRS2_RES9) >> 9)
#define VIN_CH_MF_SRS2_RES9_SET(reg, val)             (reg = ((reg & ~VIN_CH_MF_SRS2_RES9) | (((val) & 0x0001) << 9)))
#define VIN_CH_MF_SRS2_RES8                           (1 << 8)
#define VIN_CH_MF_SRS2_RES8_GET(reg)                  ((reg & VIN_CH_MF_SRS2_RES8) >> 8)
#define VIN_CH_MF_SRS2_RES8_SET(reg, val)             (reg = ((reg & ~VIN_CH_MF_SRS2_RES8) | (((val) & 0x0001) << 8)))
#define VIN_CH_MF_SRS2_RES7                           (1 << 7)
#define VIN_CH_MF_SRS2_RES7_GET(reg)                  ((reg & VIN_CH_MF_SRS2_RES7) >> 7)
#define VIN_CH_MF_SRS2_RES7_SET(reg, val)             (reg = ((reg & ~VIN_CH_MF_SRS2_RES7) | (((val) & 0x0001) << 7)))
#define VIN_CH_MF_SRS2_RES6                           (1 << 6)
#define VIN_CH_MF_SRS2_RES6_GET(reg)                  ((reg & VIN_CH_MF_SRS2_RES6) >> 6)
#define VIN_CH_MF_SRS2_RES6_SET(reg, val)             (reg = ((reg & ~VIN_CH_MF_SRS2_RES6) | (((val) & 0x0001) << 6)))
#define VIN_CH_MF_SRS2_RES5                           (1 << 5)
#define VIN_CH_MF_SRS2_RES5_GET(reg)                  ((reg & VIN_CH_MF_SRS2_RES5) >> 5)
#define VIN_CH_MF_SRS2_RES5_SET(reg, val)             (reg = ((reg & ~VIN_CH_MF_SRS2_RES5) | (((val) & 0x0001) << 5)))
#define VIN_CH_MF_SRS2_RES4                           (1 << 4)
#define VIN_CH_MF_SRS2_RES4_GET(reg)                  ((reg & VIN_CH_MF_SRS2_RES4) >> 4)
#define VIN_CH_MF_SRS2_RES4_SET(reg, val)             (reg = ((reg & ~VIN_CH_MF_SRS2_RES4) | (((val) & 0x0001) << 4)))
#define VIN_CH_MF_SRS2_M_OTEMP                        (1 << 3)
#define VIN_CH_MF_SRS2_M_OTEMP_GET(reg)               ((reg & VIN_CH_MF_SRS2_M_OTEMP) >> 3)
#define VIN_CH_MF_SRS2_M_OTEMP_SET(reg, val)          (reg = ((reg & ~VIN_CH_MF_SRS2_M_OTEMP) | (((val) & 0x0001) << 3)))
#define VIN_CH_MF_SRS2_M_CS_FAIL_CRAM                 (1 << 2)
#define VIN_CH_MF_SRS2_M_CS_FAIL_CRAM_GET(reg)        ((reg & VIN_CH_MF_SRS2_M_CS_FAIL_CRAM) >> 2)
#define VIN_CH_MF_SRS2_M_CS_FAIL_CRAM_SET(reg, val)   (reg = ((reg & ~VIN_CH_MF_SRS2_M_CS_FAIL_CRAM) | (((val) & 0x0001) << 2)))
#define VIN_CH_MF_SRS2_M_CS_FAIL_DSP                  (1 << 1)
#define VIN_CH_MF_SRS2_M_CS_FAIL_DSP_GET(reg)         ((reg & VIN_CH_MF_SRS2_M_CS_FAIL_DSP) >> 1)
#define VIN_CH_MF_SRS2_M_CS_FAIL_DSP_SET(reg, val)    (reg = ((reg & ~VIN_CH_MF_SRS2_M_CS_FAIL_DSP) | (((val) & 0x0001) << 1)))
#define VIN_CH_MF_SRS2_M_CS_FAIL_DCCTL                (1 << 0)
#define VIN_CH_MF_SRS2_M_CS_FAIL_DCCTL_GET(reg)       ((reg & VIN_CH_MF_SRS2_M_CS_FAIL_DCCTL) >> 0)
#define VIN_CH_MF_SRS2_M_CS_FAIL_DCCTL_SET(reg, val)  (reg = ((reg & ~VIN_CH_MF_SRS2_M_CS_FAIL_DCCTL) | (((val) & 0x0001) << 0)))

/***Operating Mode Status Register Current***/

#define VIN_CH_OPMOD_CUR                              ( (VIN_CH + 0x21) )

#define VIN_CH_OPMOD_CUR_RES15                        (1 << 15)
#define VIN_CH_OPMOD_CUR_RES15_GET(reg)               ((reg & VIN_CH_OPMOD_CUR_RES15) >> 15)
#define VIN_CH_OPMOD_CUR_RES15_SET(reg, val)          (reg = ((reg & ~VIN_CH_OPMOD_CUR_RES15) | (((val) & 0x0001) << 15)))
#define VIN_CH_OPMOD_CUR_RES14                        (1 << 14)
#define VIN_CH_OPMOD_CUR_RES14_GET(reg)               ((reg & VIN_CH_OPMOD_CUR_RES14) >> 14)
#define VIN_CH_OPMOD_CUR_RES14_SET(reg, val)          (reg = ((reg & ~VIN_CH_OPMOD_CUR_RES14) | (((val) & 0x0001) << 14)))
#define VIN_CH_OPMOD_CUR_RES13                        (1 << 13)
#define VIN_CH_OPMOD_CUR_RES13_GET(reg)               ((reg & VIN_CH_OPMOD_CUR_RES13) >> 13)
#define VIN_CH_OPMOD_CUR_RES13_SET(reg, val)          (reg = ((reg & ~VIN_CH_OPMOD_CUR_RES13) | (((val) & 0x0001) << 13)))
#define VIN_CH_OPMOD_CUR_RES12                        (1 << 12)
#define VIN_CH_OPMOD_CUR_RES12_GET(reg)               ((reg & VIN_CH_OPMOD_CUR_RES12) >> 12)
#define VIN_CH_OPMOD_CUR_RES12_SET(reg, val)          (reg = ((reg & ~VIN_CH_OPMOD_CUR_RES12) | (((val) & 0x0001) << 12)))
#define VIN_CH_OPMOD_CUR_MODE_CUR_MASK                (((1 << 4) - 1) << 8)
#define VIN_CH_OPMOD_CUR_MODE_CUR_GET(reg)            (((reg) >> 8) & ((1 << 4) - 1))
#define VIN_CH_OPMOD_CUR_MODE_CUR_SET(val)            ((((1 << 4) - 1) & (val)) << 8)
#define VIN_CH_OPMOD_CUR_MODE_CUR_MODIFY(reg, val)    (reg = ( (reg & ~VIN_CH_OPMOD_CUR_MODE_CUR_MASK) | ((((1 << 4) - 1) & (val)) << 8) ))
#define VIN_CH_OPMOD_CUR_SUBMODE_CUR_MASK             (((1 << 4) - 1) << 4)
#define VIN_CH_OPMOD_CUR_SUBMODE_CUR_GET(reg)         (((reg) >> 4) & ((1 << 4) - 1))
#define VIN_CH_OPMOD_CUR_SUBMODE_CUR_SET(val)         ((((1 << 4) - 1) & (val)) << 4)
#define VIN_CH_OPMOD_CUR_SUBMODE_CUR_MODIFY(reg, val) (reg = ( (reg & ~VIN_CH_OPMOD_CUR_SUBMODE_CUR_MASK) | ((((1 << 4) - 1) & (val)) << 4) ))
#define VIN_CH_OPMOD_CUR_DCCTL_REV_MASK               (((1 << 4) - 1) << 0)
#define VIN_CH_OPMOD_CUR_DCCTL_REV_GET(reg)           (((reg) >> 0) & ((1 << 4) - 1))
#define VIN_CH_OPMOD_CUR_DCCTL_REV_SET(val)           ((((1 << 4) - 1) & (val)) << 0)
#define VIN_CH_OPMOD_CUR_DCCTL_REV_MODIFY(reg, val)   (reg = ( (reg & ~VIN_CH_OPMOD_CUR_DCCTL_REV_MASK) | ((((1 << 4) - 1) & (val)) << 0) ))

/***TG1 Freq***/

#define VIN_CH_TG1F                                   ( (VIN_CH + 0x18) )

#define VIN_CH_TG1F_TG1F

/***BP1 Freq***/

#define VIN_CH_BP1F                                   ( (VIN_CH + 0x19) )

#define VIN_CH_BP1F_BP1F

/***TG2 Freq***/

#define VIN_CH_TG2F                                   ( (VIN_CH + 0x1A) )

#define VIN_CH_TG2F_TG2F

/***BP2 Freq***/

#define VIN_CH_BP2F                                   ( (VIN_CH + 0x1B) )

#define VIN_CH_BP2F_BP2F

/***TG1 Amplitude***/

#define VIN_CH_TG1A                                   ( (VIN_CH + 0x1C) )

#define VIN_CH_TG1A_TG1A

/***TG2 Amplitude***/

#define VIN_CH_TG2A                                   ( (VIN_CH + 0x1D) )

#define VIN_CH_TG2A_TG2A

/***BP1, BP2 Quality***/

#define VIN_CH_BP12Q                                  ( (VIN_CH + 0x1E) )

#define VIN_CH_BP12Q_RES15                            (1 << 15)
#define VIN_CH_BP12Q_RES15_GET(reg)                   ((reg & VIN_CH_BP12Q_RES15) >> 15)
#define VIN_CH_BP12Q_RES15_SET(reg, val)              (reg = ((reg & ~VIN_CH_BP12Q_RES15) | (((val) & 0x0001) << 15)))
#define VIN_CH_BP12Q_RES14                            (1 << 14)
#define VIN_CH_BP12Q_RES14_GET(reg)                   ((reg & VIN_CH_BP12Q_RES14) >> 14)
#define VIN_CH_BP12Q_RES14_SET(reg, val)              (reg = ((reg & ~VIN_CH_BP12Q_RES14) | (((val) & 0x0001) << 14)))
#define VIN_CH_BP12Q_RES13                            (1 << 13)
#define VIN_CH_BP12Q_RES13_GET(reg)                   ((reg & VIN_CH_BP12Q_RES13) >> 13)
#define VIN_CH_BP12Q_RES13_SET(reg, val)              (reg = ((reg & ~VIN_CH_BP12Q_RES13) | (((val) & 0x0001) << 13)))
#define VIN_CH_BP12Q_RES12                            (1 << 12)
#define VIN_CH_BP12Q_RES12_GET(reg)                   ((reg & VIN_CH_BP12Q_RES12) >> 12)
#define VIN_CH_BP12Q_RES12_SET(reg, val)              (reg = ((reg & ~VIN_CH_BP12Q_RES12) | (((val) & 0x0001) << 12)))
#define VIN_CH_BP12Q_RES11                            (1 << 11)
#define VIN_CH_BP12Q_RES11_GET(reg)                   ((reg & VIN_CH_BP12Q_RES11) >> 11)
#define VIN_CH_BP12Q_RES11_SET(reg, val)              (reg = ((reg & ~VIN_CH_BP12Q_RES11) | (((val) & 0x0001) << 11)))
#define VIN_CH_BP12Q_RES10                            (1 << 10)
#define VIN_CH_BP12Q_RES10_GET(reg)                   ((reg & VIN_CH_BP12Q_RES10) >> 10)
#define VIN_CH_BP12Q_RES10_SET(reg, val)              (reg = ((reg & ~VIN_CH_BP12Q_RES10) | (((val) & 0x0001) << 10)))
#define VIN_CH_BP12Q_RES9                             (1 << 9)
#define VIN_CH_BP12Q_RES9_GET(reg)                    ((reg & VIN_CH_BP12Q_RES9) >> 9)
#define VIN_CH_BP12Q_RES9_SET(reg, val)               (reg = ((reg & ~VIN_CH_BP12Q_RES9) | (((val) & 0x0001) << 9)))
#define VIN_CH_BP12Q_RES8                             (1 << 8)
#define VIN_CH_BP12Q_RES8_GET(reg)                    ((reg & VIN_CH_BP12Q_RES8) >> 8)
#define VIN_CH_BP12Q_RES8_SET(reg, val)               (reg = ((reg & ~VIN_CH_BP12Q_RES8) | (((val) & 0x0001) << 8)))
#define VIN_CH_BP12Q_BP1Q_MASK                        (((1 << 4) - 1) << 4)
#define VIN_CH_BP12Q_BP1Q_GET(reg)                    (((reg) >> 4) & ((1 << 4) - 1))
#define VIN_CH_BP12Q_BP1Q_SET(val)                    ((((1 << 4) - 1) & (val)) << 4)
#define VIN_CH_BP12Q_BP1Q_MODIFY(reg, val)            (reg = ( (reg & ~VIN_CH_BP12Q_BP1Q_MASK) | ((((1 << 4) - 1) & (val)) << 4) ))
#define VIN_CH_BP12Q_BP2Q_MASK                        (((1 << 4) - 1) << 0)
#define VIN_CH_BP12Q_BP2Q_GET(reg)                    (((reg) >> 0) & ((1 << 4) - 1))
#define VIN_CH_BP12Q_BP2Q_SET(val)                    ((((1 << 4) - 1) & (val)) << 0)
#define VIN_CH_BP12Q_BP2Q_MODIFY(reg, val)            (reg = ( (reg & ~VIN_CH_BP12Q_BP2Q_MASK) | ((((1 << 4) - 1) & (val)) << 0) ))

/***BP3 Freq***/

#define VIN_CH_BP3F                                   ( (VIN_CH + 0x40) )

#define VIN_CH_BP3F_BP3F

/***Ring Frequency***/

#define VIN_CH_RGFR                                   ( (VIN_CH + 0x48) )

#define VIN_CH_RGFR_RGFR

/***LMAC***/

#define VIN_CH_LMAC                                   ( (VIN_CH + 0x41) )

#define VIN_CH_LMAC_RES15                             (1 << 15)
#define VIN_CH_LMAC_RES15_GET(reg)                    ((reg & VIN_CH_LMAC_RES15) >> 15)
#define VIN_CH_LMAC_RES15_SET(reg, val)               (reg = ((reg & ~VIN_CH_LMAC_RES15) | (((val) & 0x0001) << 15)))
#define VIN_CH_LMAC_RES14                             (1 << 14)
#define VIN_CH_LMAC_RES14_GET(reg)                    ((reg & VIN_CH_LMAC_RES14) >> 14)
#define VIN_CH_LMAC_RES14_SET(reg, val)               (reg = ((reg & ~VIN_CH_LMAC_RES14) | (((val) & 0x0001) << 14)))
#define VIN_CH_LMAC_RES13                             (1 << 13)
#define VIN_CH_LMAC_RES13_GET(reg)                    ((reg & VIN_CH_LMAC_RES13) >> 13)
#define VIN_CH_LMAC_RES13_SET(reg, val)               (reg = ((reg & ~VIN_CH_LMAC_RES13) | (((val) & 0x0001) << 13)))
#define VIN_CH_LMAC_RES12                             (1 << 12)
#define VIN_CH_LMAC_RES12_GET(reg)                    ((reg & VIN_CH_LMAC_RES12) >> 12)
#define VIN_CH_LMAC_RES12_SET(reg, val)               (reg = ((reg & ~VIN_CH_LMAC_RES12) | (((val) & 0x0001) << 12)))
#define VIN_CH_LMAC_LMAC_MASK                         (((1 << 4) - 1) << 8)
#define VIN_CH_LMAC_LMAC_GET(reg)                     (((reg) >> 8) & ((1 << 4) - 1))
#define VIN_CH_LMAC_LMAC_SET(val)                     ((((1 << 4) - 1) & (val)) << 8)
#define VIN_CH_LMAC_LMAC_MODIFY(reg, val)             (reg = ( (reg & ~VIN_CH_LMAC_LMAC_MASK) | ((((1 << 4) - 1) & (val)) << 8) ))
#define VIN_CH_LMAC_RES7                              (1 << 7)
#define VIN_CH_LMAC_RES7_GET(reg)                     ((reg & VIN_CH_LMAC_RES7) >> 7)
#define VIN_CH_LMAC_RES7_SET(reg, val)                (reg = ((reg & ~VIN_CH_LMAC_RES7) | (((val) & 0x0001) << 7)))
#define VIN_CH_LMAC_RES6                              (1 << 6)
#define VIN_CH_LMAC_RES6_GET(reg)                     ((reg & VIN_CH_LMAC_RES6) >> 6)
#define VIN_CH_LMAC_RES6_SET(reg, val)                (reg = ((reg & ~VIN_CH_LMAC_RES6) | (((val) & 0x0001) << 6)))
#define VIN_CH_LMAC_RES5                              (1 << 5)
#define VIN_CH_LMAC_RES5_GET(reg)                     ((reg & VIN_CH_LMAC_RES5) >> 5)
#define VIN_CH_LMAC_RES5_SET(reg, val)                (reg = ((reg & ~VIN_CH_LMAC_RES5) | (((val) & 0x0001) << 5)))
#define VIN_CH_LMAC_RES4                              (1 << 4)
#define VIN_CH_LMAC_RES4_GET(reg)                     ((reg & VIN_CH_LMAC_RES4) >> 4)
#define VIN_CH_LMAC_RES4_SET(reg, val)                (reg = ((reg & ~VIN_CH_LMAC_RES4) | (((val) & 0x0001) << 4)))
#define VIN_CH_LMAC_BP3Q_MASK                         (((1 << 4) - 1) << 0)
#define VIN_CH_LMAC_BP3Q_GET(reg)                     (((reg) >> 0) & ((1 << 4) - 1))
#define VIN_CH_LMAC_BP3Q_SET(val)                     ((((1 << 4) - 1) & (val)) << 0)
#define VIN_CH_LMAC_BP3Q_MODIFY(reg, val)             (reg = ( (reg & ~VIN_CH_LMAC_BP3Q_MASK) | ((((1 << 4) - 1) & (val)) << 0) ))

/***DC LM Shift Factor***/

#define VIN_CH_LMDC                                   ( (VIN_CH + 0x68) )

#define VIN_CH_LMDC_LMDC

/***V1***/

#define VIN_CH_V1                                     ( (VIN_CH + 0x49) )

#define VIN_CH_V1_V1

/******/

#define VIN_CH_RGTP                                   ( (VIN_CH + 0x4A) )

#define VIN_CH_RGTP_RGTP

/***CREST Factor***/

#define VIN_CH_CREST                                  ( (VIN_CH + 0x4B) )

#define VIN_CH_CREST_CREST

/***Ring Offset 1***/

#define VIN_CH_RO1                                    ( (VIN_CH + 0x4C) )

#define VIN_CH_RO1_RO1

/***Ring Offset 2***/

#define VIN_CH_RO2                                    ( (VIN_CH + 0x4D) )

#define VIN_CH_RO2_RO2

/***Ring Offset 3***/

#define VIN_CH_RO3                                    ( (VIN_CH + 0x4E) )

#define VIN_CH_RO3_RO3

/***Ring Delay***/

#define VIN_CH_RGD                                    ( (VIN_CH + 0x4F) )

#define VIN_CH_RGD_RGD

/******/

#define VIN_CH_I2                                     ( (VIN_CH + 0x53) )

#define VIN_CH_I2_I2

/******/

#define VIN_CH_VLIM                                   ( (VIN_CH + 0x54) )

#define VIN_CH_VLIM_VLIM

/******/

#define VIN_CH_VK1                                    ( (VIN_CH + 0x55) )

#define VIN_CH_VK1_VK1

/***IK1***/

#define VIN_CH_IK1                                    ( (VIN_CH + 0x56) )

#define VIN_CH_IK1_IK1

/******/

#define VIN_CH_VRTL                                   ( (VIN_CH + 0x5F) )

#define VIN_CH_VRTL_VRTL

/******/

#define VIN_CH_HPD                                    ( (VIN_CH + 0x60) )

#define VIN_CH_HPD_HPD

/******/

#define VIN_CH_HACT                                   ( (VIN_CH + 0x61) )

#define VIN_CH_HACT_HACT

/******/

#define VIN_CH_HRING                                  ( (VIN_CH + 0x62) )

#define VIN_CH_HRING_HRING

/******/

#define VIN_CH_HACRT                                  ( (VIN_CH + 0x63) )

#define VIN_CH_HACRT_HACRT

/******/

#define VIN_CH_HLS                                    ( (VIN_CH + 0x64) )

#define VIN_CH_HLS_HLS

/******/

#define VIN_CH_HMW                                    ( (VIN_CH + 0x65) )

#define VIN_CH_HMW_HMW

/******/

#define VIN_CH_HAHY                                   ( (VIN_CH + 0x66) )

#define VIN_CH_HAHY_HAHY

/******/

#define VIN_CH_FRTR                                   ( (VIN_CH + 0x67) )

#define VIN_CH_FRTR_FRTR

/******/

#define VIN_CH_CRAMP                                  ( (VIN_CH + 0x6A) )

#define VIN_CH_CRAMP_CRAMP

/***********************************************************************/
/*  Module      :  CH register address and bits                        */
/***********************************************************************/

#define VIN21_CH                                      (0)

/***********************************************************************/


/***Basic Configuration Register 1***/

#define VIN21_CH_BCR1                                 ( (VIN21_CH + 0x07) )

#define VIN21_CH_BCR1_DUP_GNDK_MASK                   (((1 << 4) - 1) << 12)
#define VIN21_CH_BCR1_DUP_GNDK_GET(reg)               (((reg) >> 12) & ((1 << 4) - 1))
#define VIN21_CH_BCR1_DUP_GNDK_SET(val)               ((((1 << 4) - 1) & (val)) << 12)
#define VIN21_CH_BCR1_DUP_GNDK_MODIFY(reg, val)       (reg = ( (reg & ~VIN21_CH_BCR1_DUP_GNDK_MASK) | ((((1 << 4) - 1) & (val)) << 12) ))
#define VIN21_CH_BCR1_RES7_MASK                       (((1 << 5) - 1) << 7)
#define VIN21_CH_BCR1_RES7_GET(reg)                   (((reg) >> 7) & ((1 << 5) - 1))
#define VIN21_CH_BCR1_RES7_SET(val)                   ((((1 << 5) - 1) & (val)) << 7)
#define VIN21_CH_BCR1_RES7_MODIFY(reg, val)           (reg = ( (reg & ~VIN21_CH_BCR1_RES7_MASK) | ((((1 << 5) - 1) & (val)) << 7) ))
#define VIN21_CH_BCR1_DC_HOLD                         (1 << 6)
#define VIN21_CH_BCR1_DC_HOLD_GET(reg)                ((reg & VIN21_CH_BCR1_DC_HOLD) >> 6)
#define VIN21_CH_BCR1_DC_HOLD_SET(reg, val)           (reg = ((reg & ~VIN21_CH_BCR1_DC_HOLD) | (((val) & 0x0001) << 6)))
#define VIN21_CH_BCR1_EN_IK                           (1 << 5)
#define VIN21_CH_BCR1_EN_IK_GET(reg)                  ((reg & VIN21_CH_BCR1_EN_IK) >> 5)
#define VIN21_CH_BCR1_EN_IK_SET(reg, val)             (reg = ((reg & ~VIN21_CH_BCR1_EN_IK) | (((val) & 0x0001) << 5)))
#define VIN21_CH_BCR1_TEST_EN                         (1 << 4)
#define VIN21_CH_BCR1_TEST_EN_GET(reg)                ((reg & VIN21_CH_BCR1_TEST_EN) >> 4)
#define VIN21_CH_BCR1_TEST_EN_SET(reg, val)           (reg = ((reg & ~VIN21_CH_BCR1_TEST_EN) | (((val) & 0x0001) << 4)))
#define VIN21_CH_BCR1_SEL_SLIC_MASK                   (((1 << 4) - 1) << 0)
#define VIN21_CH_BCR1_SEL_SLIC_GET(reg)               (((reg) >> 0) & ((1 << 4) - 1))
#define VIN21_CH_BCR1_SEL_SLIC_SET(val)               ((((1 << 4) - 1) & (val)) << 0)
#define VIN21_CH_BCR1_SEL_SLIC_MODIFY(reg, val)       (reg = ( (reg & ~VIN21_CH_BCR1_SEL_SLIC_MASK) | ((((1 << 4) - 1) & (val)) << 0) ))

/***Basic Configuration Register 2***/

#define VIN21_CH_BCR2                                 ( (VIN21_CH + 0x08) )

#define VIN21_CH_BCR2_LPRX_CR                         (1 << 15)
#define VIN21_CH_BCR2_LPRX_CR_GET(reg)                ((reg & VIN21_CH_BCR2_LPRX_CR) >> 15)
#define VIN21_CH_BCR2_LPRX_CR_SET(reg, val)           (reg = ((reg & ~VIN21_CH_BCR2_LPRX_CR) | (((val) & 0x0001) << 15)))
#define VIN21_CH_BCR2_CRAM_EN                         (1 << 14)
#define VIN21_CH_BCR2_CRAM_EN_GET(reg)                ((reg & VIN21_CH_BCR2_CRAM_EN) >> 14)
#define VIN21_CH_BCR2_CRAM_EN_SET(reg, val)           (reg = ((reg & ~VIN21_CH_BCR2_CRAM_EN) | (((val) & 0x0001) << 14)))
#define VIN21_CH_BCR2_SOFT_DIS                        (1 << 13)
#define VIN21_CH_BCR2_SOFT_DIS_GET(reg)               ((reg & VIN21_CH_BCR2_SOFT_DIS) >> 13)
#define VIN21_CH_BCR2_SOFT_DIS_SET(reg, val)          (reg = ((reg & ~VIN21_CH_BCR2_SOFT_DIS) | (((val) & 0x0001) << 13)))
#define VIN21_CH_BCR2_TTX_EN                          (1 << 12)
#define VIN21_CH_BCR2_TTX_EN_GET(reg)                 ((reg & VIN21_CH_BCR2_TTX_EN) >> 12)
#define VIN21_CH_BCR2_TTX_EN_SET(reg, val)            (reg = ((reg & ~VIN21_CH_BCR2_TTX_EN) | (((val) & 0x0001) << 12)))
#define VIN21_CH_BCR2_RES11                           (1 << 11)
#define VIN21_CH_BCR2_RES11_GET(reg)                  ((reg & VIN21_CH_BCR2_RES11) >> 11)
#define VIN21_CH_BCR2_RES11_SET(reg, val)             (reg = ((reg & ~VIN21_CH_BCR2_RES11) | (((val) & 0x0001) << 11)))
#define VIN21_CH_BCR2_REVPOL                          (1 << 10)
#define VIN21_CH_BCR2_REVPOL_GET(reg)                 ((reg & VIN21_CH_BCR2_REVPOL) >> 10)
#define VIN21_CH_BCR2_REVPOL_SET(reg, val)            (reg = ((reg & ~VIN21_CH_BCR2_REVPOL) | (((val) & 0x0001) << 10)))
#define VIN21_CH_BCR2_AC_SHORT_EN                     (1 << 9)
#define VIN21_CH_BCR2_AC_SHORT_EN_GET(reg)            ((reg & VIN21_CH_BCR2_AC_SHORT_EN) >> 9)
#define VIN21_CH_BCR2_AC_SHORT_EN_SET(reg, val)       (reg = ((reg & ~VIN21_CH_BCR2_AC_SHORT_EN) | (((val) & 0x0001) << 9)))
#define VIN21_CH_BCR2_CCC                             (1 << 8)
#define VIN21_CH_BCR2_CCC_GET(reg)                    ((reg & VIN21_CH_BCR2_CCC) >> 8)
#define VIN21_CH_BCR2_CCC_SET(reg, val)               (reg = ((reg & ~VIN21_CH_BCR2_CCC) | (((val) & 0x0001) << 8)))
#define VIN21_CH_BCR2_TH_DIS                          (1 << 7)
#define VIN21_CH_BCR2_TH_DIS_GET(reg)                 ((reg & VIN21_CH_BCR2_TH_DIS) >> 7)
#define VIN21_CH_BCR2_TH_DIS_SET(reg, val)            (reg = ((reg & ~VIN21_CH_BCR2_TH_DIS) | (((val) & 0x0001) << 7)))
#define VIN21_CH_BCR2_IM_DIS                          (1 << 6)
#define VIN21_CH_BCR2_IM_DIS_GET(reg)                 ((reg & VIN21_CH_BCR2_IM_DIS) >> 6)
#define VIN21_CH_BCR2_IM_DIS_SET(reg, val)            (reg = ((reg & ~VIN21_CH_BCR2_IM_DIS) | (((val) & 0x0001) << 6)))
#define VIN21_CH_BCR2_AX_DIS                          (1 << 5)
#define VIN21_CH_BCR2_AX_DIS_GET(reg)                 ((reg & VIN21_CH_BCR2_AX_DIS) >> 5)
#define VIN21_CH_BCR2_AX_DIS_SET(reg, val)            (reg = ((reg & ~VIN21_CH_BCR2_AX_DIS) | (((val) & 0x0001) << 5)))
#define VIN21_CH_BCR2_AR_DIS                          (1 << 4)
#define VIN21_CH_BCR2_AR_DIS_GET(reg)                 ((reg & VIN21_CH_BCR2_AR_DIS) >> 4)
#define VIN21_CH_BCR2_AR_DIS_SET(reg, val)            (reg = ((reg & ~VIN21_CH_BCR2_AR_DIS) | (((val) & 0x0001) << 4)))
#define VIN21_CH_BCR2_FRX_DIS                         (1 << 3)
#define VIN21_CH_BCR2_FRX_DIS_GET(reg)                ((reg & VIN21_CH_BCR2_FRX_DIS) >> 3)
#define VIN21_CH_BCR2_FRX_DIS_SET(reg, val)           (reg = ((reg & ~VIN21_CH_BCR2_FRX_DIS) | (((val) & 0x0001) << 3)))
#define VIN21_CH_BCR2_FRR_DIS                         (1 << 2)
#define VIN21_CH_BCR2_FRR_DIS_GET(reg)                ((reg & VIN21_CH_BCR2_FRR_DIS) >> 2)
#define VIN21_CH_BCR2_FRR_DIS_SET(reg, val)           (reg = ((reg & ~VIN21_CH_BCR2_FRR_DIS) | (((val) & 0x0001) << 2)))
#define VIN21_CH_BCR2_HPX_DIS                         (1 << 1)
#define VIN21_CH_BCR2_HPX_DIS_GET(reg)                ((reg & VIN21_CH_BCR2_HPX_DIS) >> 1)
#define VIN21_CH_BCR2_HPX_DIS_SET(reg, val)           (reg = ((reg & ~VIN21_CH_BCR2_HPX_DIS) | (((val) & 0x0001) << 1)))
#define VIN21_CH_BCR2_HPR_DIS                         (1 << 0)
#define VIN21_CH_BCR2_HPR_DIS_GET(reg)                ((reg & VIN21_CH_BCR2_HPR_DIS) >> 0)
#define VIN21_CH_BCR2_HPR_DIS_SET(reg, val)           (reg = ((reg & ~VIN21_CH_BCR2_HPR_DIS) | (((val) & 0x0001) << 0)))

/***Level Metering Configuration Register***/

#define VIN21_CH_LMCR                                 ( (VIN21_CH + 0x0a) )

#define VIN21_CH_LMCR_LM_ACT                          (1 << 15)
#define VIN21_CH_LMCR_LM_ACT_GET(reg)                 ((reg & VIN21_CH_LMCR_LM_ACT) >> 15)
#define VIN21_CH_LMCR_LM_ACT_SET(reg, val)            (reg = ((reg & ~VIN21_CH_LMCR_LM_ACT) | (((val) & 0x0001) << 15)))
#define VIN21_CH_LMCR_RAMP_EN                         (1 << 14)
#define VIN21_CH_LMCR_RAMP_EN_GET(reg)                ((reg & VIN21_CH_LMCR_RAMP_EN) >> 14)
#define VIN21_CH_LMCR_RAMP_EN_SET(reg, val)           (reg = ((reg & ~VIN21_CH_LMCR_RAMP_EN) | (((val) & 0x0001) << 14)))
#define VIN21_CH_LMCR_LM_INT_MASK                     (((1 << 2) - 1) << 12)
#define VIN21_CH_LMCR_LM_INT_GET(reg)                 (((reg) >> 12) & ((1 << 2) - 1))
#define VIN21_CH_LMCR_LM_INT_SET(val)                 ((((1 << 2) - 1) & (val)) << 12)
#define VIN21_CH_LMCR_LM_INT_MODIFY(reg, val)         (reg = ( (reg & ~VIN21_CH_LMCR_LM_INT_MASK) | ((((1 << 2) - 1) & (val)) << 12) ))
#define VIN21_CH_LMCR_RES10_MASK                      (((1 << 2) - 1) << 10)
#define VIN21_CH_LMCR_RES10_GET(reg)                  (((reg) >> 10) & ((1 << 2) - 1))
#define VIN21_CH_LMCR_RES10_SET(val)                  ((((1 << 2) - 1) & (val)) << 10)
#define VIN21_CH_LMCR_RES10_MODIFY(reg, val)          (reg = ( (reg & ~VIN21_CH_LMCR_RES10_MASK) | ((((1 << 2) - 1) & (val)) << 10) ))
#define VIN21_CH_LMCR_LM_MAG_MASK                     (((1 << 2) - 1) << 8)
#define VIN21_CH_LMCR_LM_MAG_GET(reg)                 (((reg) >> 8) & ((1 << 2) - 1))
#define VIN21_CH_LMCR_LM_MAG_SET(val)                 ((((1 << 2) - 1) & (val)) << 8)
#define VIN21_CH_LMCR_LM_MAG_MODIFY(reg, val)         (reg = ( (reg & ~VIN21_CH_LMCR_LM_MAG_MASK) | ((((1 << 2) - 1) & (val)) << 8) ))
#define VIN21_CH_LMCR_RES6_MASK                       (((1 << 2) - 1) << 6)
#define VIN21_CH_LMCR_RES6_GET(reg)                   (((reg) >> 6) & ((1 << 2) - 1))
#define VIN21_CH_LMCR_RES6_SET(val)                   ((((1 << 2) - 1) & (val)) << 6)
#define VIN21_CH_LMCR_RES6_MODIFY(reg, val)           (reg = ( (reg & ~VIN21_CH_LMCR_RES6_MASK) | ((((1 << 2) - 1) & (val)) << 6) ))
#define VIN21_CH_LMCR_LM_PRE_GAIN                     (1 << 5)
#define VIN21_CH_LMCR_LM_PRE_GAIN_GET(reg)            ((reg & VIN21_CH_LMCR_LM_PRE_GAIN) >> 5)
#define VIN21_CH_LMCR_LM_PRE_GAIN_SET(reg, val)       (reg = ((reg & ~VIN21_CH_LMCR_LM_PRE_GAIN) | (((val) & 0x0001) << 5)))
#define VIN21_CH_LMCR_LM_RCMEAS                       (1 << 4)
#define VIN21_CH_LMCR_LM_RCMEAS_GET(reg)              ((reg & VIN21_CH_LMCR_LM_RCMEAS) >> 4)
#define VIN21_CH_LMCR_LM_RCMEAS_SET(reg, val)         (reg = ((reg & ~VIN21_CH_LMCR_LM_RCMEAS) | (((val) & 0x0001) << 4)))
#define VIN21_CH_LMCR_LM_SEL_MASK                     (((1 << 4) - 1) << 0)
#define VIN21_CH_LMCR_LM_SEL_GET(reg)                 (((reg) >> 0) & ((1 << 4) - 1))
#define VIN21_CH_LMCR_LM_SEL_SET(val)                 ((((1 << 4) - 1) & (val)) << 0)
#define VIN21_CH_LMCR_LM_SEL_MODIFY(reg, val)         (reg = ( (reg & ~VIN21_CH_LMCR_LM_SEL_MASK) | ((((1 << 4) - 1) & (val)) << 0) ))

/***Automatic Mode Register***/

#define VIN21_CH_AUTOMOD                              ( (VIN21_CH + 0x0d) )

#define VIN21_CH_AUTOMOD_RES15                        (1 << 15)
#define VIN21_CH_AUTOMOD_RES15_GET(reg)               ((reg & VIN21_CH_AUTOMOD_RES15) >> 15)
#define VIN21_CH_AUTOMOD_RES15_SET(reg, val)          (reg = ((reg & ~VIN21_CH_AUTOMOD_RES15) | (((val) & 0x0001) << 15)))
#define VIN21_CH_AUTOMOD_RES14                        (1 << 14)
#define VIN21_CH_AUTOMOD_RES14_GET(reg)               ((reg & VIN21_CH_AUTOMOD_RES14) >> 14)
#define VIN21_CH_AUTOMOD_RES14_SET(reg, val)          (reg = ((reg & ~VIN21_CH_AUTOMOD_RES14) | (((val) & 0x0001) << 14)))
#define VIN21_CH_AUTOMOD_RES13                        (1 << 13)
#define VIN21_CH_AUTOMOD_RES13_GET(reg)               ((reg & VIN21_CH_AUTOMOD_RES13) >> 13)
#define VIN21_CH_AUTOMOD_RES13_SET(reg, val)          (reg = ((reg & ~VIN21_CH_AUTOMOD_RES13) | (((val) & 0x0001) << 13)))
#define VIN21_CH_AUTOMOD_RES12                        (1 << 12)
#define VIN21_CH_AUTOMOD_RES12_GET(reg)               ((reg & VIN21_CH_AUTOMOD_RES12) >> 12)
#define VIN21_CH_AUTOMOD_RES12_SET(reg, val)          (reg = ((reg & ~VIN21_CH_AUTOMOD_RES12) | (((val) & 0x0001) << 12)))
#define VIN21_CH_AUTOMOD_RES11                        (1 << 11)
#define VIN21_CH_AUTOMOD_RES11_GET(reg)               ((reg & VIN21_CH_AUTOMOD_RES11) >> 11)
#define VIN21_CH_AUTOMOD_RES11_SET(reg, val)          (reg = ((reg & ~VIN21_CH_AUTOMOD_RES11) | (((val) & 0x0001) << 11)))
#define VIN21_CH_AUTOMOD_RES10                        (1 << 10)
#define VIN21_CH_AUTOMOD_RES10_GET(reg)               ((reg & VIN21_CH_AUTOMOD_RES10) >> 10)
#define VIN21_CH_AUTOMOD_RES10_SET(reg, val)          (reg = ((reg & ~VIN21_CH_AUTOMOD_RES10) | (((val) & 0x0001) << 10)))
#define VIN21_CH_AUTOMOD_RES9                         (1 << 9)
#define VIN21_CH_AUTOMOD_RES9_GET(reg)                ((reg & VIN21_CH_AUTOMOD_RES9) >> 9)
#define VIN21_CH_AUTOMOD_RES9_SET(reg, val)           (reg = ((reg & ~VIN21_CH_AUTOMOD_RES9) | (((val) & 0x0001) << 9)))
#define VIN21_CH_AUTOMOD_RES8                         (1 << 8)
#define VIN21_CH_AUTOMOD_RES8_GET(reg)                ((reg & VIN21_CH_AUTOMOD_RES8) >> 8)
#define VIN21_CH_AUTOMOD_RES8_SET(reg, val)           (reg = ((reg & ~VIN21_CH_AUTOMOD_RES8) | (((val) & 0x0001) << 8)))
#define VIN21_CH_AUTOMOD_PDGNDK_EN                    (1 << 7)
#define VIN21_CH_AUTOMOD_PDGNDK_EN_GET(reg)           ((reg & VIN21_CH_AUTOMOD_PDGNDK_EN) >> 7)
#define VIN21_CH_AUTOMOD_PDGNDK_EN_SET(reg, val)      (reg = ((reg & ~VIN21_CH_AUTOMOD_PDGNDK_EN) | (((val) & 0x0001) << 7)))
#define VIN21_CH_AUTOMOD_AUTO_TRAIN                   (1 << 6)
#define VIN21_CH_AUTOMOD_AUTO_TRAIN_GET(reg)          ((reg & VIN21_CH_AUTOMOD_AUTO_TRAIN) >> 6)
#define VIN21_CH_AUTOMOD_AUTO_TRAIN_SET(reg, val)     (reg = ((reg & ~VIN21_CH_AUTOMOD_AUTO_TRAIN) | (((val) & 0x0001) << 6)))
#define VIN21_CH_AUTOMOD_AUTO_RING_EN                 (1 << 5)
#define VIN21_CH_AUTOMOD_AUTO_RING_EN_GET(reg)        ((reg & VIN21_CH_AUTOMOD_AUTO_RING_EN) >> 5)
#define VIN21_CH_AUTOMOD_AUTO_RING_EN_SET(reg, val)   (reg = ((reg & ~VIN21_CH_AUTOMOD_AUTO_RING_EN) | (((val) & 0x0001) << 5)))
#define VIN21_CH_AUTOMOD_AUTO_MET_START               (1 << 4)
#define VIN21_CH_AUTOMOD_AUTO_MET_START_GET(reg)      ((reg & VIN21_CH_AUTOMOD_AUTO_MET_START) >> 4)
#define VIN21_CH_AUTOMOD_AUTO_MET_START_SET(reg, val) (reg = ((reg & ~VIN21_CH_AUTOMOD_AUTO_MET_START) | (((val) & 0x0001) << 4)))
#define VIN21_CH_AUTOMOD_AUTO_MET_EN                  (1 << 3)
#define VIN21_CH_AUTOMOD_AUTO_MET_EN_GET(reg)         ((reg & VIN21_CH_AUTOMOD_AUTO_MET_EN) >> 3)
#define VIN21_CH_AUTOMOD_AUTO_MET_EN_SET(reg, val)    (reg = ((reg & ~VIN21_CH_AUTOMOD_AUTO_MET_EN) | (((val) & 0x0001) << 3)))
#define VIN21_CH_AUTOMOD_AUTO_OFFH                    (1 << 2)
#define VIN21_CH_AUTOMOD_AUTO_OFFH_GET(reg)           ((reg & VIN21_CH_AUTOMOD_AUTO_OFFH) >> 2)
#define VIN21_CH_AUTOMOD_AUTO_OFFH_SET(reg, val)      (reg = ((reg & ~VIN21_CH_AUTOMOD_AUTO_OFFH) | (((val) & 0x0001) << 2)))
#define VIN21_CH_AUTOMOD_AUTO_BAT                     (1 << 1)
#define VIN21_CH_AUTOMOD_AUTO_BAT_GET(reg)            ((reg & VIN21_CH_AUTOMOD_AUTO_BAT) >> 1)
#define VIN21_CH_AUTOMOD_AUTO_BAT_SET(reg, val)       (reg = ((reg & ~VIN21_CH_AUTOMOD_AUTO_BAT) | (((val) & 0x0001) << 1)))
#define VIN21_CH_AUTOMOD_PDOT_DIS                     (1 << 0)
#define VIN21_CH_AUTOMOD_PDOT_DIS_GET(reg)            ((reg & VIN21_CH_AUTOMOD_PDOT_DIS) >> 0)
#define VIN21_CH_AUTOMOD_PDOT_DIS_SET(reg, val)       (reg = ((reg & ~VIN21_CH_AUTOMOD_PDOT_DIS) | (((val) & 0x0001) << 0)))

/***Test Register 1***/

#define VIN21_CH_TSTR1                                ( (VIN21_CH + 0x0e) )

#define VIN21_CH_TSTR1_RES13_MASK                     (((1 << 3) - 1) << 13)
#define VIN21_CH_TSTR1_RES13_GET(reg)                 (((reg) >> 13) & ((1 << 3) - 1))
#define VIN21_CH_TSTR1_RES13_SET(val)                 ((((1 << 3) - 1) & (val)) << 13)
#define VIN21_CH_TSTR1_RES13_MODIFY(reg, val)         (reg = ( (reg & ~VIN21_CH_TSTR1_RES13_MASK) | ((((1 << 3) - 1) & (val)) << 13) ))
#define VIN21_CH_TSTR1_PD_AC_DA                       (1 << 12)
#define VIN21_CH_TSTR1_PD_AC_DA_GET(reg)              ((reg & VIN21_CH_TSTR1_PD_AC_DA) >> 12)
#define VIN21_CH_TSTR1_PD_AC_DA_SET(reg, val)         (reg = ((reg & ~VIN21_CH_TSTR1_PD_AC_DA) | (((val) & 0x0001) << 12)))
#define VIN21_CH_TSTR1_RES0_MASK                      (((1 << 12) - 1) << 0)
#define VIN21_CH_TSTR1_RES0_GET(reg)                  (((reg) >> 0) & ((1 << 12) - 1))
#define VIN21_CH_TSTR1_RES0_SET(val)                  ((((1 << 12) - 1) & (val)) << 0)
#define VIN21_CH_TSTR1_RES0_MODIFY(reg, val)          (reg = ( (reg & ~VIN21_CH_TSTR1_RES0_MASK) | ((((1 << 12) - 1) & (val)) << 0) ))

/***Test Register 2***/

#define VIN21_CH_TSTR2                                ( (VIN21_CH + 0x0f) )

#define VIN21_CH_TSTR2_AC_EN                          (1 << 15)
#define VIN21_CH_TSTR2_AC_EN_GET(reg)                 ((reg & VIN21_CH_TSTR2_AC_EN) >> 15)
#define VIN21_CH_TSTR2_AC_EN_SET(reg, val)            (reg = ((reg & ~VIN21_CH_TSTR2_AC_EN) | (((val) & 0x0001) << 15)))
#define VIN21_CH_TSTR2_RES13_MASK                     (((1 << 2) - 1) << 13)
#define VIN21_CH_TSTR2_RES13_GET(reg)                 (((reg) >> 13) & ((1 << 2) - 1))
#define VIN21_CH_TSTR2_RES13_SET(val)                 ((((1 << 2) - 1) & (val)) << 13)
#define VIN21_CH_TSTR2_RES13_MODIFY(reg, val)         (reg = ( (reg & ~VIN21_CH_TSTR2_RES13_MASK) | ((((1 << 2) - 1) & (val)) << 13) ))
#define VIN21_CH_TSTR2_AC_DLB_16M                     (1 << 12)
#define VIN21_CH_TSTR2_AC_DLB_16M_GET(reg)            ((reg & VIN21_CH_TSTR2_AC_DLB_16M) >> 12)
#define VIN21_CH_TSTR2_AC_DLB_16M_SET(reg, val)       (reg = ((reg & ~VIN21_CH_TSTR2_AC_DLB_16M) | (((val) & 0x0001) << 12)))
#define VIN21_CH_TSTR2_RES10_MASK                     (((1 << 2) - 1) << 10)
#define VIN21_CH_TSTR2_RES10_GET(reg)                 (((reg) >> 10) & ((1 << 2) - 1))
#define VIN21_CH_TSTR2_RES10_SET(val)                 ((((1 << 2) - 1) & (val)) << 10)
#define VIN21_CH_TSTR2_RES10_MODIFY(reg, val)         (reg = ( (reg & ~VIN21_CH_TSTR2_RES10_MASK) | ((((1 << 2) - 1) & (val)) << 10) ))
#define VIN21_CH_TSTR2_AC_DLB_8K                      (1 << 9)
#define VIN21_CH_TSTR2_AC_DLB_8K_GET(reg)             ((reg & VIN21_CH_TSTR2_AC_DLB_8K) >> 9)
#define VIN21_CH_TSTR2_AC_DLB_8K_SET(reg, val)        (reg = ((reg & ~VIN21_CH_TSTR2_AC_DLB_8K) | (((val) & 0x0001) << 9)))
#define VIN21_CH_TSTR2_AC_SHORT                       (1 << 8)
#define VIN21_CH_TSTR2_AC_SHORT_GET(reg)              ((reg & VIN21_CH_TSTR2_AC_SHORT) >> 8)
#define VIN21_CH_TSTR2_AC_SHORT_SET(reg, val)         (reg = ((reg & ~VIN21_CH_TSTR2_AC_SHORT) | (((val) & 0x0001) << 8)))
#define VIN21_CH_TSTR2_AC_RGAIN                       (1 << 7)
#define VIN21_CH_TSTR2_AC_RGAIN_GET(reg)              ((reg & VIN21_CH_TSTR2_AC_RGAIN) >> 7)
#define VIN21_CH_TSTR2_AC_RGAIN_SET(reg, val)         (reg = ((reg & ~VIN21_CH_TSTR2_AC_RGAIN) | (((val) & 0x0001) << 7)))
#define VIN21_CH_TSTR2_AC_XGAIN_MASK                  (((1 << 2) - 1) << 5)
#define VIN21_CH_TSTR2_AC_XGAIN_GET(reg)              (((reg) >> 5) & ((1 << 2) - 1))
#define VIN21_CH_TSTR2_AC_XGAIN_SET(val)              ((((1 << 2) - 1) & (val)) << 5)
#define VIN21_CH_TSTR2_AC_XGAIN_MODIFY(reg, val)      (reg = ( (reg & ~VIN21_CH_TSTR2_AC_XGAIN_MASK) | ((((1 << 2) - 1) & (val)) << 5) ))
#define VIN21_CH_TSTR2_RES4                           (1 << 4)
#define VIN21_CH_TSTR2_RES4_GET(reg)                  ((reg & VIN21_CH_TSTR2_RES4) >> 4)
#define VIN21_CH_TSTR2_RES4_SET(reg, val)             (reg = ((reg & ~VIN21_CH_TSTR2_RES4) | (((val) & 0x0001) << 4)))
#define VIN21_CH_TSTR2_OPIM_AN                        (1 << 3)
#define VIN21_CH_TSTR2_OPIM_AN_GET(reg)               ((reg & VIN21_CH_TSTR2_OPIM_AN) >> 3)
#define VIN21_CH_TSTR2_OPIM_AN_SET(reg, val)          (reg = ((reg & ~VIN21_CH_TSTR2_OPIM_AN) | (((val) & 0x0001) << 3)))
#define VIN21_CH_TSTR2_RES2                           (1 << 2)
#define VIN21_CH_TSTR2_RES2_GET(reg)                  ((reg & VIN21_CH_TSTR2_RES2) >> 2)
#define VIN21_CH_TSTR2_RES2_SET(reg, val)             (reg = ((reg & ~VIN21_CH_TSTR2_RES2) | (((val) & 0x0001) << 2)))
#define VIN21_CH_TSTR2_COR_256                        (1 << 1)
#define VIN21_CH_TSTR2_COR_256_GET(reg)               ((reg & VIN21_CH_TSTR2_COR_256) >> 1)
#define VIN21_CH_TSTR2_COR_256_SET(reg, val)          (reg = ((reg & ~VIN21_CH_TSTR2_COR_256) | (((val) & 0x0001) << 1)))
#define VIN21_CH_TSTR2_COX_16                         (1 << 0)
#define VIN21_CH_TSTR2_COX_16_GET(reg)                ((reg & VIN21_CH_TSTR2_COX_16) >> 0)
#define VIN21_CH_TSTR2_COX_16_SET(reg, val)           (reg = ((reg & ~VIN21_CH_TSTR2_COX_16) | (((val) & 0x0001) << 0)))

/***Test Register 3***/

#define VIN21_CH_TSTR3                                ( (VIN21_CH + 0x10) )

#define VIN21_CH_TSTR3_RES13_MASK                     (((1 << 3) - 1) << 13)
#define VIN21_CH_TSTR3_RES13_GET(reg)                 (((reg) >> 13) & ((1 << 3) - 1))
#define VIN21_CH_TSTR3_RES13_SET(val)                 ((((1 << 3) - 1) & (val)) << 13)
#define VIN21_CH_TSTR3_RES13_MODIFY(reg, val)         (reg = ( (reg & ~VIN21_CH_TSTR3_RES13_MASK) | ((((1 << 3) - 1) & (val)) << 13) ))
#define VIN21_CH_TSTR3_DC_POFI_HI                     (1 << 12)
#define VIN21_CH_TSTR3_DC_POFI_HI_GET(reg)            ((reg & VIN21_CH_TSTR3_DC_POFI_HI) >> 12)
#define VIN21_CH_TSTR3_DC_POFI_HI_SET(reg, val)       (reg = ((reg & ~VIN21_CH_TSTR3_DC_POFI_HI) | (((val) & 0x0001) << 12)))
#define VIN21_CH_TSTR3_RES9_MASK                      (((1 << 3) - 1) << 9)
#define VIN21_CH_TSTR3_RES9_GET(reg)                  (((reg) >> 9) & ((1 << 3) - 1))
#define VIN21_CH_TSTR3_RES9_SET(val)                  ((((1 << 3) - 1) & (val)) << 9)
#define VIN21_CH_TSTR3_RES9_MODIFY(reg, val)          (reg = ( (reg & ~VIN21_CH_TSTR3_RES9_MASK) | ((((1 << 3) - 1) & (val)) << 9) ))
#define VIN21_CH_TSTR3_DC_EN                          (1 << 8)
#define VIN21_CH_TSTR3_DC_EN_GET(reg)                 ((reg & VIN21_CH_TSTR3_DC_EN) >> 8)
#define VIN21_CH_TSTR3_DC_EN_SET(reg, val)            (reg = ((reg & ~VIN21_CH_TSTR3_DC_EN) | (((val) & 0x0001) << 8)))
#define VIN21_CH_TSTR3_RES4_MASK                      (((1 << 4) - 1) << 4)
#define VIN21_CH_TSTR3_RES4_GET(reg)                  (((reg) >> 4) & ((1 << 4) - 1))
#define VIN21_CH_TSTR3_RES4_SET(val)                  ((((1 << 4) - 1) & (val)) << 4)
#define VIN21_CH_TSTR3_RES4_MODIFY(reg, val)          (reg = ( (reg & ~VIN21_CH_TSTR3_RES4_MASK) | ((((1 << 4) - 1) & (val)) << 4) ))
#define VIN21_CH_TSTR3_DCC_DIS                        (1 << 3)
#define VIN21_CH_TSTR3_DCC_DIS_GET(reg)               ((reg & VIN21_CH_TSTR3_DCC_DIS) >> 3)
#define VIN21_CH_TSTR3_DCC_DIS_SET(reg, val)          (reg = ((reg & ~VIN21_CH_TSTR3_DCC_DIS) | (((val) & 0x0001) << 3)))
#define VIN21_CH_TSTR3_RES0_MASK                      (((1 << 3) - 1) << 0)
#define VIN21_CH_TSTR3_RES0_GET(reg)                  (((reg) >> 0) & ((1 << 3) - 1))
#define VIN21_CH_TSTR3_RES0_SET(val)                  ((((1 << 3) - 1) & (val)) << 0)
#define VIN21_CH_TSTR3_RES0_MODIFY(reg, val)          (reg = ( (reg & ~VIN21_CH_TSTR3_RES0_MASK) | ((((1 << 3) - 1) & (val)) << 0) ))

/***Level Metering Result2***/

#define VIN21_CH_LMRES2                               ( (VIN21_CH + 0x12) )

#define VIN21_CH_LMRES2_LM_RES2

/***Level Metering Configuration Register 2***/

#define VIN21_CH_LMCR2                                ( (VIN21_CH + 0x13) )

#define VIN21_CH_LMCR2_RES13_MASK                     (((1 << 3) - 1) << 13)
#define VIN21_CH_LMCR2_RES13_GET(reg)                 (((reg) >> 13) & ((1 << 3) - 1))
#define VIN21_CH_LMCR2_RES13_SET(val)                 ((((1 << 3) - 1) & (val)) << 13)
#define VIN21_CH_LMCR2_RES13_MODIFY(reg, val)         (reg = ( (reg & ~VIN21_CH_LMCR2_RES13_MASK) | ((((1 << 3) - 1) & (val)) << 13) ))
#define VIN21_CH_LMCR2_LM2PCM                         (1 << 12)
#define VIN21_CH_LMCR2_LM2PCM_GET(reg)                ((reg & VIN21_CH_LMCR2_LM2PCM) >> 12)
#define VIN21_CH_LMCR2_LM2PCM_SET(reg, val)           (reg = ((reg & ~VIN21_CH_LMCR2_LM2PCM) | (((val) & 0x0001) << 12)))
#define VIN21_CH_LMCR2_RES9_MASK                      (((1 << 3) - 1) << 9)
#define VIN21_CH_LMCR2_RES9_GET(reg)                  (((reg) >> 9) & ((1 << 3) - 1))
#define VIN21_CH_LMCR2_RES9_SET(val)                  ((((1 << 3) - 1) & (val)) << 9)
#define VIN21_CH_LMCR2_RES9_MODIFY(reg, val)          (reg = ( (reg & ~VIN21_CH_LMCR2_RES9_MASK) | ((((1 << 3) - 1) & (val)) << 9) ))
#define VIN21_CH_LMCR2_LM_ITS                         (1 << 8)
#define VIN21_CH_LMCR2_LM_ITS_GET(reg)                ((reg & VIN21_CH_LMCR2_LM_ITS) >> 8)
#define VIN21_CH_LMCR2_LM_ITS_SET(reg, val)           (reg = ((reg & ~VIN21_CH_LMCR2_LM_ITS) | (((val) & 0x0001) << 8)))
#define VIN21_CH_LMCR2_RES5_MASK                      (((1 << 3) - 1) << 5)
#define VIN21_CH_LMCR2_RES5_GET(reg)                  (((reg) >> 5) & ((1 << 3) - 1))
#define VIN21_CH_LMCR2_RES5_SET(val)                  ((((1 << 3) - 1) & (val)) << 5)
#define VIN21_CH_LMCR2_RES5_MODIFY(reg, val)          (reg = ( (reg & ~VIN21_CH_LMCR2_RES5_MASK) | ((((1 << 3) - 1) & (val)) << 5) ))
#define VIN21_CH_LMCR2_LM_DCTX8K                      (1 << 4)
#define VIN21_CH_LMCR2_LM_DCTX8K_GET(reg)             ((reg & VIN21_CH_LMCR2_LM_DCTX8K) >> 4)
#define VIN21_CH_LMCR2_LM_DCTX8K_SET(reg, val)        (reg = ((reg & ~VIN21_CH_LMCR2_LM_DCTX8K) | (((val) & 0x0001) << 4)))
#define VIN21_CH_LMCR2_RES3                           (1 << 3)
#define VIN21_CH_LMCR2_RES3_GET(reg)                  ((reg & VIN21_CH_LMCR2_RES3) >> 3)
#define VIN21_CH_LMCR2_RES3_SET(reg, val)             (reg = ((reg & ~VIN21_CH_LMCR2_RES3) | (((val) & 0x0001) << 3)))
#define VIN21_CH_LMCR2_LM_FILT_SEL_MASK               (((1 << 3) - 1) << 0)
#define VIN21_CH_LMCR2_LM_FILT_SEL_GET(reg)           (((reg) >> 0) & ((1 << 3) - 1))
#define VIN21_CH_LMCR2_LM_FILT_SEL_SET(val)           ((((1 << 3) - 1) & (val)) << 0)
#define VIN21_CH_LMCR2_LM_FILT_SEL_MODIFY(reg, val)   (reg = ( (reg & ~VIN21_CH_LMCR2_LM_FILT_SEL_MASK) | ((((1 << 3) - 1) & (val)) << 0) ))

/***Mask Register for Status Register for Analog-Line-Mod
ule Interrupts 1***/

#define VIN21_CH_MR_SRS1                              ( (VIN21_CH + 0x02) )

#define VIN21_CH_MR_SRS1_M_AUTO_MET_DONE              (1 << 15)
#define VIN21_CH_MR_SRS1_M_AUTO_MET_DONE_GET(reg)     ((reg & VIN21_CH_MR_SRS1_M_AUTO_MET_DONE) >> 15)
#define VIN21_CH_MR_SRS1_M_AUTO_MET_DONE_SET(reg, val)(reg = ((reg & ~VIN21_CH_MR_SRS1_M_AUTO_MET_DONE) | (((val) & 0x0001) << 15)))
#define VIN21_CH_MR_SRS1_M_RAMP_READY                 (1 << 14)
#define VIN21_CH_MR_SRS1_M_RAMP_READY_GET(reg)        ((reg & VIN21_CH_MR_SRS1_M_RAMP_READY) >> 14)
#define VIN21_CH_MR_SRS1_M_RAMP_READY_SET(reg, val)   (reg = ((reg & ~VIN21_CH_MR_SRS1_M_RAMP_READY) | (((val) & 0x0001) << 14)))
#define VIN21_CH_MR_SRS1_M_HOOK                       (1 << 13)
#define VIN21_CH_MR_SRS1_M_HOOK_GET(reg)              ((reg & VIN21_CH_MR_SRS1_M_HOOK) >> 13)
#define VIN21_CH_MR_SRS1_M_HOOK_SET(reg, val)         (reg = ((reg & ~VIN21_CH_MR_SRS1_M_HOOK) | (((val) & 0x0001) << 13)))
#define VIN21_CH_MR_SRS1_M_GNDK                       (1 << 12)
#define VIN21_CH_MR_SRS1_M_GNDK_GET(reg)              ((reg & VIN21_CH_MR_SRS1_M_GNDK) >> 12)
#define VIN21_CH_MR_SRS1_M_GNDK_SET(reg, val)         (reg = ((reg & ~VIN21_CH_MR_SRS1_M_GNDK) | (((val) & 0x0001) << 12)))
#define VIN21_CH_MR_SRS1_M_GNKP                       (1 << 11)
#define VIN21_CH_MR_SRS1_M_GNKP_GET(reg)              ((reg & VIN21_CH_MR_SRS1_M_GNKP) >> 11)
#define VIN21_CH_MR_SRS1_M_GNKP_SET(reg, val)         (reg = ((reg & ~VIN21_CH_MR_SRS1_M_GNKP) | (((val) & 0x0001) << 11)))
#define VIN21_CH_MR_SRS1_M_GNDKH                      (1 << 10)
#define VIN21_CH_MR_SRS1_M_GNDKH_GET(reg)             ((reg & VIN21_CH_MR_SRS1_M_GNDKH) >> 10)
#define VIN21_CH_MR_SRS1_M_GNDKH_SET(reg, val)        (reg = ((reg & ~VIN21_CH_MR_SRS1_M_GNDKH) | (((val) & 0x0001) << 10)))
#define VIN21_CH_MR_SRS1_M_ICON                       (1 << 9)
#define VIN21_CH_MR_SRS1_M_ICON_GET(reg)              ((reg & VIN21_CH_MR_SRS1_M_ICON) >> 9)
#define VIN21_CH_MR_SRS1_M_ICON_SET(reg, val)         (reg = ((reg & ~VIN21_CH_MR_SRS1_M_ICON) | (((val) & 0x0001) << 9)))
#define VIN21_CH_MR_SRS1_M_VTRLIM                     (1 << 8)
#define VIN21_CH_MR_SRS1_M_VTRLIM_GET(reg)            ((reg & VIN21_CH_MR_SRS1_M_VTRLIM) >> 8)
#define VIN21_CH_MR_SRS1_M_VTRLIM_SET(reg, val)       (reg = ((reg & ~VIN21_CH_MR_SRS1_M_VTRLIM) | (((val) & 0x0001) << 8)))
#define VIN21_CH_MR_SRS1_M_LSUP                       (1 << 7)
#define VIN21_CH_MR_SRS1_M_LSUP_GET(reg)              ((reg & VIN21_CH_MR_SRS1_M_LSUP) >> 7)
#define VIN21_CH_MR_SRS1_M_LSUP_SET(reg, val)         (reg = ((reg & ~VIN21_CH_MR_SRS1_M_LSUP) | (((val) & 0x0001) << 7)))
#define VIN21_CH_MR_SRS1_M_LM_THRES                   (1 << 6)
#define VIN21_CH_MR_SRS1_M_LM_THRES_GET(reg)          ((reg & VIN21_CH_MR_SRS1_M_LM_THRES) >> 6)
#define VIN21_CH_MR_SRS1_M_LM_THRES_SET(reg, val)     (reg = ((reg & ~VIN21_CH_MR_SRS1_M_LM_THRES) | (((val) & 0x0001) << 6)))
#define VIN21_CH_MR_SRS1_M_LM_OK                      (1 << 5)
#define VIN21_CH_MR_SRS1_M_LM_OK_GET(reg)             ((reg & VIN21_CH_MR_SRS1_M_LM_OK) >> 5)
#define VIN21_CH_MR_SRS1_M_LM_OK_SET(reg, val)        (reg = ((reg & ~VIN21_CH_MR_SRS1_M_LM_OK) | (((val) & 0x0001) << 5)))
#define VIN21_CH_MR_SRS1_M_DU_IO_MASK                 (((1 << 5) - 1) << 0)
#define VIN21_CH_MR_SRS1_M_DU_IO_GET(reg)             (((reg) >> 0) & ((1 << 5) - 1))
#define VIN21_CH_MR_SRS1_M_DU_IO_SET(val)             ((((1 << 5) - 1) & (val)) << 0)
#define VIN21_CH_MR_SRS1_M_DU_IO_MODIFY(reg, val)     (reg = ( (reg & ~VIN21_CH_MR_SRS1_M_DU_IO_MASK) | ((((1 << 5) - 1) & (val)) << 0) ))

/***Mask Register for Status Register for Analog-Line-Mod
ule Interrupts 2***/

#define VIN21_CH_MR_SRS2                              ( (VIN21_CH + 0x03) )

#define VIN21_CH_MR_SRS2_RES15                        (1 << 15)
#define VIN21_CH_MR_SRS2_RES15_GET(reg)               ((reg & VIN21_CH_MR_SRS2_RES15) >> 15)
#define VIN21_CH_MR_SRS2_RES15_SET(reg, val)          (reg = ((reg & ~VIN21_CH_MR_SRS2_RES15) | (((val) & 0x0001) << 15)))
#define VIN21_CH_MR_SRS2_RES14                        (1 << 14)
#define VIN21_CH_MR_SRS2_RES14_GET(reg)               ((reg & VIN21_CH_MR_SRS2_RES14) >> 14)
#define VIN21_CH_MR_SRS2_RES14_SET(reg, val)          (reg = ((reg & ~VIN21_CH_MR_SRS2_RES14) | (((val) & 0x0001) << 14)))
#define VIN21_CH_MR_SRS2_RES13                        (1 << 13)
#define VIN21_CH_MR_SRS2_RES13_GET(reg)               ((reg & VIN21_CH_MR_SRS2_RES13) >> 13)
#define VIN21_CH_MR_SRS2_RES13_SET(reg, val)          (reg = ((reg & ~VIN21_CH_MR_SRS2_RES13) | (((val) & 0x0001) << 13)))
#define VIN21_CH_MR_SRS2_RES12                        (1 << 12)
#define VIN21_CH_MR_SRS2_RES12_GET(reg)               ((reg & VIN21_CH_MR_SRS2_RES12) >> 12)
#define VIN21_CH_MR_SRS2_RES12_SET(reg, val)          (reg = ((reg & ~VIN21_CH_MR_SRS2_RES12) | (((val) & 0x0001) << 12)))
#define VIN21_CH_MR_SRS2_RES11                        (1 << 11)
#define VIN21_CH_MR_SRS2_RES11_GET(reg)               ((reg & VIN21_CH_MR_SRS2_RES11) >> 11)
#define VIN21_CH_MR_SRS2_RES11_SET(reg, val)          (reg = ((reg & ~VIN21_CH_MR_SRS2_RES11) | (((val) & 0x0001) << 11)))
#define VIN21_CH_MR_SRS2_RES10                        (1 << 10)
#define VIN21_CH_MR_SRS2_RES10_GET(reg)               ((reg & VIN21_CH_MR_SRS2_RES10) >> 10)
#define VIN21_CH_MR_SRS2_RES10_SET(reg, val)          (reg = ((reg & ~VIN21_CH_MR_SRS2_RES10) | (((val) & 0x0001) << 10)))
#define VIN21_CH_MR_SRS2_RES9                         (1 << 9)
#define VIN21_CH_MR_SRS2_RES9_GET(reg)                ((reg & VIN21_CH_MR_SRS2_RES9) >> 9)
#define VIN21_CH_MR_SRS2_RES9_SET(reg, val)           (reg = ((reg & ~VIN21_CH_MR_SRS2_RES9) | (((val) & 0x0001) << 9)))
#define VIN21_CH_MR_SRS2_RES8                         (1 << 8)
#define VIN21_CH_MR_SRS2_RES8_GET(reg)                ((reg & VIN21_CH_MR_SRS2_RES8) >> 8)
#define VIN21_CH_MR_SRS2_RES8_SET(reg, val)           (reg = ((reg & ~VIN21_CH_MR_SRS2_RES8) | (((val) & 0x0001) << 8)))
#define VIN21_CH_MR_SRS2_RES7                         (1 << 7)
#define VIN21_CH_MR_SRS2_RES7_GET(reg)                ((reg & VIN21_CH_MR_SRS2_RES7) >> 7)
#define VIN21_CH_MR_SRS2_RES7_SET(reg, val)           (reg = ((reg & ~VIN21_CH_MR_SRS2_RES7) | (((val) & 0x0001) << 7)))
#define VIN21_CH_MR_SRS2_RES6                         (1 << 6)
#define VIN21_CH_MR_SRS2_RES6_GET(reg)                ((reg & VIN21_CH_MR_SRS2_RES6) >> 6)
#define VIN21_CH_MR_SRS2_RES6_SET(reg, val)           (reg = ((reg & ~VIN21_CH_MR_SRS2_RES6) | (((val) & 0x0001) << 6)))
#define VIN21_CH_MR_SRS2_RES5                         (1 << 5)
#define VIN21_CH_MR_SRS2_RES5_GET(reg)                ((reg & VIN21_CH_MR_SRS2_RES5) >> 5)
#define VIN21_CH_MR_SRS2_RES5_SET(reg, val)           (reg = ((reg & ~VIN21_CH_MR_SRS2_RES5) | (((val) & 0x0001) << 5)))
#define VIN21_CH_MR_SRS2_RES4                         (1 << 4)
#define VIN21_CH_MR_SRS2_RES4_GET(reg)                ((reg & VIN21_CH_MR_SRS2_RES4) >> 4)
#define VIN21_CH_MR_SRS2_RES4_SET(reg, val)           (reg = ((reg & ~VIN21_CH_MR_SRS2_RES4) | (((val) & 0x0001) << 4)))
#define VIN21_CH_MR_SRS2_M_OTEMP                      (1 << 3)
#define VIN21_CH_MR_SRS2_M_OTEMP_GET(reg)             ((reg & VIN21_CH_MR_SRS2_M_OTEMP) >> 3)
#define VIN21_CH_MR_SRS2_M_OTEMP_SET(reg, val)        (reg = ((reg & ~VIN21_CH_MR_SRS2_M_OTEMP) | (((val) & 0x0001) << 3)))
#define VIN21_CH_MR_SRS2_RES2                         (1 << 2)
#define VIN21_CH_MR_SRS2_RES2_GET(reg)                ((reg & VIN21_CH_MR_SRS2_RES2) >> 2)
#define VIN21_CH_MR_SRS2_RES2_SET(reg, val)           (reg = ((reg & ~VIN21_CH_MR_SRS2_RES2) | (((val) & 0x0001) << 2)))
#define VIN21_CH_MR_SRS2_RES1                         (1 << 1)
#define VIN21_CH_MR_SRS2_RES1_GET(reg)                ((reg & VIN21_CH_MR_SRS2_RES1) >> 1)
#define VIN21_CH_MR_SRS2_RES1_SET(reg, val)           (reg = ((reg & ~VIN21_CH_MR_SRS2_RES1) | (((val) & 0x0001) << 1)))
#define VIN21_CH_MR_SRS2_RES0                         (1 << 0)
#define VIN21_CH_MR_SRS2_RES0_GET(reg)                ((reg & VIN21_CH_MR_SRS2_RES0) >> 0)
#define VIN21_CH_MR_SRS2_RES0_SET(reg, val)           (reg = ((reg & ~VIN21_CH_MR_SRS2_RES0) | (((val) & 0x0001) << 0)))

/***Mask Register for Status Register for Analog-Line-Mod
ule Interrupts 1***/

#define VIN21_CH_MF_SRS1                              ( (VIN21_CH + 0x62) )

#define VIN21_CH_MF_SRS1_M_AUTO_MET_DONE              (1 << 15)
#define VIN21_CH_MF_SRS1_M_AUTO_MET_DONE_GET(reg)     ((reg & VIN21_CH_MF_SRS1_M_AUTO_MET_DONE) >> 15)
#define VIN21_CH_MF_SRS1_M_AUTO_MET_DONE_SET(reg, val)(reg = ((reg & ~VIN21_CH_MF_SRS1_M_AUTO_MET_DONE) | (((val) & 0x0001) << 15)))
#define VIN21_CH_MF_SRS1_M_RAMP_READY                 (1 << 14)
#define VIN21_CH_MF_SRS1_M_RAMP_READY_GET(reg)        ((reg & VIN21_CH_MF_SRS1_M_RAMP_READY) >> 14)
#define VIN21_CH_MF_SRS1_M_RAMP_READY_SET(reg, val)   (reg = ((reg & ~VIN21_CH_MF_SRS1_M_RAMP_READY) | (((val) & 0x0001) << 14)))
#define VIN21_CH_MF_SRS1_M_HOOK                       (1 << 13)
#define VIN21_CH_MF_SRS1_M_HOOK_GET(reg)              ((reg & VIN21_CH_MF_SRS1_M_HOOK) >> 13)
#define VIN21_CH_MF_SRS1_M_HOOK_SET(reg, val)         (reg = ((reg & ~VIN21_CH_MF_SRS1_M_HOOK) | (((val) & 0x0001) << 13)))
#define VIN21_CH_MF_SRS1_M_GNDK                       (1 << 12)
#define VIN21_CH_MF_SRS1_M_GNDK_GET(reg)              ((reg & VIN21_CH_MF_SRS1_M_GNDK) >> 12)
#define VIN21_CH_MF_SRS1_M_GNDK_SET(reg, val)         (reg = ((reg & ~VIN21_CH_MF_SRS1_M_GNDK) | (((val) & 0x0001) << 12)))
#define VIN21_CH_MF_SRS1_M_GNKP                       (1 << 11)
#define VIN21_CH_MF_SRS1_M_GNKP_GET(reg)              ((reg & VIN21_CH_MF_SRS1_M_GNKP) >> 11)
#define VIN21_CH_MF_SRS1_M_GNKP_SET(reg, val)         (reg = ((reg & ~VIN21_CH_MF_SRS1_M_GNKP) | (((val) & 0x0001) << 11)))
#define VIN21_CH_MF_SRS1_M_GNDKH                      (1 << 10)
#define VIN21_CH_MF_SRS1_M_GNDKH_GET(reg)             ((reg & VIN21_CH_MF_SRS1_M_GNDKH) >> 10)
#define VIN21_CH_MF_SRS1_M_GNDKH_SET(reg, val)        (reg = ((reg & ~VIN21_CH_MF_SRS1_M_GNDKH) | (((val) & 0x0001) << 10)))
#define VIN21_CH_MF_SRS1_M_ICON                       (1 << 9)
#define VIN21_CH_MF_SRS1_M_ICON_GET(reg)              ((reg & VIN21_CH_MF_SRS1_M_ICON) >> 9)
#define VIN21_CH_MF_SRS1_M_ICON_SET(reg, val)         (reg = ((reg & ~VIN21_CH_MF_SRS1_M_ICON) | (((val) & 0x0001) << 9)))
#define VIN21_CH_MF_SRS1_M_VTRLIM                     (1 << 8)
#define VIN21_CH_MF_SRS1_M_VTRLIM_GET(reg)            ((reg & VIN21_CH_MF_SRS1_M_VTRLIM) >> 8)
#define VIN21_CH_MF_SRS1_M_VTRLIM_SET(reg, val)       (reg = ((reg & ~VIN21_CH_MF_SRS1_M_VTRLIM) | (((val) & 0x0001) << 8)))
#define VIN21_CH_MF_SRS1_M_LSUP                       (1 << 7)
#define VIN21_CH_MF_SRS1_M_LSUP_GET(reg)              ((reg & VIN21_CH_MF_SRS1_M_LSUP) >> 7)
#define VIN21_CH_MF_SRS1_M_LSUP_SET(reg, val)         (reg = ((reg & ~VIN21_CH_MF_SRS1_M_LSUP) | (((val) & 0x0001) << 7)))
#define VIN21_CH_MF_SRS1_M_LM_THRES                   (1 << 6)
#define VIN21_CH_MF_SRS1_M_LM_THRES_GET(reg)          ((reg & VIN21_CH_MF_SRS1_M_LM_THRES) >> 6)
#define VIN21_CH_MF_SRS1_M_LM_THRES_SET(reg, val)     (reg = ((reg & ~VIN21_CH_MF_SRS1_M_LM_THRES) | (((val) & 0x0001) << 6)))
#define VIN21_CH_MF_SRS1_M_LM_OK                      (1 << 5)
#define VIN21_CH_MF_SRS1_M_LM_OK_GET(reg)             ((reg & VIN21_CH_MF_SRS1_M_LM_OK) >> 5)
#define VIN21_CH_MF_SRS1_M_LM_OK_SET(reg, val)        (reg = ((reg & ~VIN21_CH_MF_SRS1_M_LM_OK) | (((val) & 0x0001) << 5)))
#define VIN21_CH_MF_SRS1_M_DU_IO_MASK                 (((1 << 5) - 1) << 0)
#define VIN21_CH_MF_SRS1_M_DU_IO_GET(reg)             (((reg) >> 0) & ((1 << 5) - 1))
#define VIN21_CH_MF_SRS1_M_DU_IO_SET(val)             ((((1 << 5) - 1) & (val)) << 0)
#define VIN21_CH_MF_SRS1_M_DU_IO_MODIFY(reg, val)     (reg = ( (reg & ~VIN21_CH_MF_SRS1_M_DU_IO_MASK) | ((((1 << 5) - 1) & (val)) << 0) ))

/***Mask Register for Status Register for Analog-Line-Mod
ule Interrupts 2***/

#define VIN21_CH_MF_SRS2                              ( (VIN21_CH + 0x63) )

#define VIN21_CH_MF_SRS2_RES15                        (1 << 15)
#define VIN21_CH_MF_SRS2_RES15_GET(reg)               ((reg & VIN21_CH_MF_SRS2_RES15) >> 15)
#define VIN21_CH_MF_SRS2_RES15_SET(reg, val)          (reg = ((reg & ~VIN21_CH_MF_SRS2_RES15) | (((val) & 0x0001) << 15)))
#define VIN21_CH_MF_SRS2_RES14                        (1 << 14)
#define VIN21_CH_MF_SRS2_RES14_GET(reg)               ((reg & VIN21_CH_MF_SRS2_RES14) >> 14)
#define VIN21_CH_MF_SRS2_RES14_SET(reg, val)          (reg = ((reg & ~VIN21_CH_MF_SRS2_RES14) | (((val) & 0x0001) << 14)))
#define VIN21_CH_MF_SRS2_RES13                        (1 << 13)
#define VIN21_CH_MF_SRS2_RES13_GET(reg)               ((reg & VIN21_CH_MF_SRS2_RES13) >> 13)
#define VIN21_CH_MF_SRS2_RES13_SET(reg, val)          (reg = ((reg & ~VIN21_CH_MF_SRS2_RES13) | (((val) & 0x0001) << 13)))
#define VIN21_CH_MF_SRS2_RES12                        (1 << 12)
#define VIN21_CH_MF_SRS2_RES12_GET(reg)               ((reg & VIN21_CH_MF_SRS2_RES12) >> 12)
#define VIN21_CH_MF_SRS2_RES12_SET(reg, val)          (reg = ((reg & ~VIN21_CH_MF_SRS2_RES12) | (((val) & 0x0001) << 12)))
#define VIN21_CH_MF_SRS2_RES11                        (1 << 11)
#define VIN21_CH_MF_SRS2_RES11_GET(reg)               ((reg & VIN21_CH_MF_SRS2_RES11) >> 11)
#define VIN21_CH_MF_SRS2_RES11_SET(reg, val)          (reg = ((reg & ~VIN21_CH_MF_SRS2_RES11) | (((val) & 0x0001) << 11)))
#define VIN21_CH_MF_SRS2_RES10                        (1 << 10)
#define VIN21_CH_MF_SRS2_RES10_GET(reg)               ((reg & VIN21_CH_MF_SRS2_RES10) >> 10)
#define VIN21_CH_MF_SRS2_RES10_SET(reg, val)          (reg = ((reg & ~VIN21_CH_MF_SRS2_RES10) | (((val) & 0x0001) << 10)))
#define VIN21_CH_MF_SRS2_RES9                         (1 << 9)
#define VIN21_CH_MF_SRS2_RES9_GET(reg)                ((reg & VIN21_CH_MF_SRS2_RES9) >> 9)
#define VIN21_CH_MF_SRS2_RES9_SET(reg, val)           (reg = ((reg & ~VIN21_CH_MF_SRS2_RES9) | (((val) & 0x0001) << 9)))
#define VIN21_CH_MF_SRS2_RES8                         (1 << 8)
#define VIN21_CH_MF_SRS2_RES8_GET(reg)                ((reg & VIN21_CH_MF_SRS2_RES8) >> 8)
#define VIN21_CH_MF_SRS2_RES8_SET(reg, val)           (reg = ((reg & ~VIN21_CH_MF_SRS2_RES8) | (((val) & 0x0001) << 8)))
#define VIN21_CH_MF_SRS2_RES7                         (1 << 7)
#define VIN21_CH_MF_SRS2_RES7_GET(reg)                ((reg & VIN21_CH_MF_SRS2_RES7) >> 7)
#define VIN21_CH_MF_SRS2_RES7_SET(reg, val)           (reg = ((reg & ~VIN21_CH_MF_SRS2_RES7) | (((val) & 0x0001) << 7)))
#define VIN21_CH_MF_SRS2_RES6                         (1 << 6)
#define VIN21_CH_MF_SRS2_RES6_GET(reg)                ((reg & VIN21_CH_MF_SRS2_RES6) >> 6)
#define VIN21_CH_MF_SRS2_RES6_SET(reg, val)           (reg = ((reg & ~VIN21_CH_MF_SRS2_RES6) | (((val) & 0x0001) << 6)))
#define VIN21_CH_MF_SRS2_RES5                         (1 << 5)
#define VIN21_CH_MF_SRS2_RES5_GET(reg)                ((reg & VIN21_CH_MF_SRS2_RES5) >> 5)
#define VIN21_CH_MF_SRS2_RES5_SET(reg, val)           (reg = ((reg & ~VIN21_CH_MF_SRS2_RES5) | (((val) & 0x0001) << 5)))
#define VIN21_CH_MF_SRS2_RES4                         (1 << 4)
#define VIN21_CH_MF_SRS2_RES4_GET(reg)                ((reg & VIN21_CH_MF_SRS2_RES4) >> 4)
#define VIN21_CH_MF_SRS2_RES4_SET(reg, val)           (reg = ((reg & ~VIN21_CH_MF_SRS2_RES4) | (((val) & 0x0001) << 4)))
#define VIN21_CH_MF_SRS2_M_OTEMP                      (1 << 3)
#define VIN21_CH_MF_SRS2_M_OTEMP_GET(reg)             ((reg & VIN21_CH_MF_SRS2_M_OTEMP) >> 3)
#define VIN21_CH_MF_SRS2_M_OTEMP_SET(reg, val)        (reg = ((reg & ~VIN21_CH_MF_SRS2_M_OTEMP) | (((val) & 0x0001) << 3)))
#define VIN21_CH_MF_SRS2_RES2                         (1 << 2)
#define VIN21_CH_MF_SRS2_RES2_GET(reg)                ((reg & VIN21_CH_MF_SRS2_RES2) >> 2)
#define VIN21_CH_MF_SRS2_RES2_SET(reg, val)           (reg = ((reg & ~VIN21_CH_MF_SRS2_RES2) | (((val) & 0x0001) << 2)))
#define VIN21_CH_MF_SRS2_RES1                         (1 << 1)
#define VIN21_CH_MF_SRS2_RES1_GET(reg)                ((reg & VIN21_CH_MF_SRS2_RES1) >> 1)
#define VIN21_CH_MF_SRS2_RES1_SET(reg, val)           (reg = ((reg & ~VIN21_CH_MF_SRS2_RES1) | (((val) & 0x0001) << 1)))
#define VIN21_CH_MF_SRS2_RES0                         (1 << 0)
#define VIN21_CH_MF_SRS2_RES0_GET(reg)                ((reg & VIN21_CH_MF_SRS2_RES0) >> 0)
#define VIN21_CH_MF_SRS2_RES0_SET(reg, val)           (reg = ((reg & ~VIN21_CH_MF_SRS2_RES0) | (((val) & 0x0001) << 0)))

/***TG1 Freq***/

#define VIN21_CH_TG1F                                 ( (VIN21_CH + 0cC4) )

#define VIN21_CH_TG1F_TG1F

/***BP1 Freq***/

#define VIN21_CH_BP1F                                 ( (VIN21_CH + NULL) )

#define VIN21_CH_BP1F_BP1F

/***TG2 Freq***/

#define VIN21_CH_TG2F                                 ( (VIN21_CH + 0xC5) )

#define VIN21_CH_TG2F_TG2F

/***BP2 Freq***/

#define VIN21_CH_BP2F                                 ( (VIN21_CH + NULL) )

#define VIN21_CH_BP2F_BP2F

/***TG1 Amplitude***/

#define VIN21_CH_TG1A                                 ( (VIN21_CH + 0xC6) )

#define VIN21_CH_TG1A_TG1A

/***TG2 Amplitude***/

#define VIN21_CH_TG2A                                 ( (VIN21_CH + 0xC7) )

#define VIN21_CH_TG2A_TG2A

/***BP1, BP2 Quality***/

#define VIN21_CH_BP12Q                                ( (VIN21_CH + NULL) )

#define VIN21_CH_BP12Q_RES15                          (1 << 15)
#define VIN21_CH_BP12Q_RES15_GET(reg)                 ((reg & VIN21_CH_BP12Q_RES15) >> 15)
#define VIN21_CH_BP12Q_RES15_SET(reg, val)            (reg = ((reg & ~VIN21_CH_BP12Q_RES15) | (((val) & 0x0001) << 15)))
#define VIN21_CH_BP12Q_RES14                          (1 << 14)
#define VIN21_CH_BP12Q_RES14_GET(reg)                 ((reg & VIN21_CH_BP12Q_RES14) >> 14)
#define VIN21_CH_BP12Q_RES14_SET(reg, val)            (reg = ((reg & ~VIN21_CH_BP12Q_RES14) | (((val) & 0x0001) << 14)))
#define VIN21_CH_BP12Q_RES13                          (1 << 13)
#define VIN21_CH_BP12Q_RES13_GET(reg)                 ((reg & VIN21_CH_BP12Q_RES13) >> 13)
#define VIN21_CH_BP12Q_RES13_SET(reg, val)            (reg = ((reg & ~VIN21_CH_BP12Q_RES13) | (((val) & 0x0001) << 13)))
#define VIN21_CH_BP12Q_RES12                          (1 << 12)
#define VIN21_CH_BP12Q_RES12_GET(reg)                 ((reg & VIN21_CH_BP12Q_RES12) >> 12)
#define VIN21_CH_BP12Q_RES12_SET(reg, val)            (reg = ((reg & ~VIN21_CH_BP12Q_RES12) | (((val) & 0x0001) << 12)))
#define VIN21_CH_BP12Q_RES11                          (1 << 11)
#define VIN21_CH_BP12Q_RES11_GET(reg)                 ((reg & VIN21_CH_BP12Q_RES11) >> 11)
#define VIN21_CH_BP12Q_RES11_SET(reg, val)            (reg = ((reg & ~VIN21_CH_BP12Q_RES11) | (((val) & 0x0001) << 11)))
#define VIN21_CH_BP12Q_RES10                          (1 << 10)
#define VIN21_CH_BP12Q_RES10_GET(reg)                 ((reg & VIN21_CH_BP12Q_RES10) >> 10)
#define VIN21_CH_BP12Q_RES10_SET(reg, val)            (reg = ((reg & ~VIN21_CH_BP12Q_RES10) | (((val) & 0x0001) << 10)))
#define VIN21_CH_BP12Q_RES9                           (1 << 9)
#define VIN21_CH_BP12Q_RES9_GET(reg)                  ((reg & VIN21_CH_BP12Q_RES9) >> 9)
#define VIN21_CH_BP12Q_RES9_SET(reg, val)             (reg = ((reg & ~VIN21_CH_BP12Q_RES9) | (((val) & 0x0001) << 9)))
#define VIN21_CH_BP12Q_RES8                           (1 << 8)
#define VIN21_CH_BP12Q_RES8_GET(reg)                  ((reg & VIN21_CH_BP12Q_RES8) >> 8)
#define VIN21_CH_BP12Q_RES8_SET(reg, val)             (reg = ((reg & ~VIN21_CH_BP12Q_RES8) | (((val) & 0x0001) << 8)))
#define VIN21_CH_BP12Q_BP1Q_MASK                      (((1 << 4) - 1) << 4)
#define VIN21_CH_BP12Q_BP1Q_GET(reg)                  (((reg) >> 4) & ((1 << 4) - 1))
#define VIN21_CH_BP12Q_BP1Q_SET(val)                  ((((1 << 4) - 1) & (val)) << 4)
#define VIN21_CH_BP12Q_BP1Q_MODIFY(reg, val)          (reg = ( (reg & ~VIN21_CH_BP12Q_BP1Q_MASK) | ((((1 << 4) - 1) & (val)) << 4) ))
#define VIN21_CH_BP12Q_BP2Q_MASK                      (((1 << 4) - 1) << 0)
#define VIN21_CH_BP12Q_BP2Q_GET(reg)                  (((reg) >> 0) & ((1 << 4) - 1))
#define VIN21_CH_BP12Q_BP2Q_SET(val)                  ((((1 << 4) - 1) & (val)) << 0)
#define VIN21_CH_BP12Q_BP2Q_MODIFY(reg, val)          (reg = ( (reg & ~VIN21_CH_BP12Q_BP2Q_MASK) | ((((1 << 4) - 1) & (val)) << 0) ))

/***BP3 Freq***/

#define VIN21_CH_BP3F                                 ( (VIN21_CH + NULL) )

#define VIN21_CH_BP3F_BP3F

/***Ring Frequency***/

#define VIN21_CH_RGFR                                 ( (VIN21_CH + 0xB8) )

#define VIN21_CH_RGFR_RGFR

/***V1***/

#define VIN21_CH_V1                                   ( (VIN21_CH + 0xB4) )

#define VIN21_CH_V1_V1

/******/

#define VIN21_CH_RGTP                                 ( (VIN21_CH + NULL) )

#define VIN21_CH_RGTP_RGTP

/***CREST Factor***/

#define VIN21_CH_CREST                                ( (VIN21_CH + 0xB0) )

#define VIN21_CH_CREST_CREST

/***Ring Offset 1***/

#define VIN21_CH_RO1                                  ( (VIN21_CH + 0xB5) )

#define VIN21_CH_RO1_RO1

/***Ring Offset 2***/

#define VIN21_CH_RO2                                  ( (VIN21_CH + 0xB6) )

#define VIN21_CH_RO2_RO2

/***Ring Offset 3***/

#define VIN21_CH_RO3                                  ( (VIN21_CH + 0xB7) )

#define VIN21_CH_RO3_RO3

/***Ring Delay***/

#define VIN21_CH_RGD                                  ( (VIN21_CH + 0xB9) )

#define VIN21_CH_RGD_RGD

/******/

#define VIN21_CH_I2                                   ( (VIN21_CH + NULL) )

#define VIN21_CH_I2_I2

/******/

#define VIN21_CH_VLIM                                 ( (VIN21_CH + 0xAD) )

#define VIN21_CH_VLIM_VLIM

/******/

#define VIN21_CH_VK1                                  ( (VIN21_CH + 0xAC) )

#define VIN21_CH_VK1_VK1

/***IK1***/

#define VIN21_CH_IK1                                  ( (VIN21_CH + 0xA4) )

#define VIN21_CH_IK1_IK1

/******/

#define VIN21_CH_VRTL                                 ( (VIN21_CH + 0xAE) )

#define VIN21_CH_VRTL_VRTL

/******/

#define VIN21_CH_HPD                                  ( (VIN21_CH + NULL) )

#define VIN21_CH_HPD_HPD

/******/

#define VIN21_CH_HACT                                 ( (VIN21_CH + NULL) )

#define VIN21_CH_HACT_HACT

/******/

#define VIN21_CH_HRING                                ( (VIN21_CH + NULL) )

#define VIN21_CH_HRING_HRING

/******/

#define VIN21_CH_HACRT                                ( (VIN21_CH + NULL) )

#define VIN21_CH_HACRT_HACRT

/******/

#define VIN21_CH_HLS                                  ( (VIN21_CH + NULL) )

#define VIN21_CH_HLS_HLS

/******/

#define VIN21_CH_HMW                                  ( (VIN21_CH + NULL) )

#define VIN21_CH_HMW_HMW

/******/

#define VIN21_CH_HAHY                                 ( (VIN21_CH + NULL) )

#define VIN21_CH_HAHY_HAHY

/******/

#define VIN21_CH_FRTR                                 ( (VIN21_CH + NULL) )

#define VIN21_CH_FRTR_FRTR

/******/

#define VIN21_CH_CRAMP                                ( (VIN21_CH + 0xCA) )

#define VIN21_CH_CRAMP_CRAMP

/******/

#define VIN21_CH_LM_ITIME                             ( (VIN21_CH + 0xCB) )

#define VIN21_CH_LM_ITIME_LM_ITIME

/******/

#define VIN21_CH_LM_DELAY                             ( (VIN21_CH + 0xCC) )

#define VIN21_CH_LM_DELAY_LM_DELAY

/******/

#define VIN21_CH_LM_SHIFT                             ( (VIN21_CH + 0xCD) )

#define VIN21_CH_LM_SHIFT_LM_SHIFT

/***********************************************************************/
/*  Module      :  DEV register address and bits                       */
/***********************************************************************/

#define VIN_DEV                                       (0)

/***********************************************************************/


/***Mask Register for Hardware Status Register 1***/

#define VIN_DEV_MR_HWSR1                              ( (VIN_DEV + 0x12) )

#define VIN_DEV_MR_HWSR1_RES15                        (1 << 15)
#define VIN_DEV_MR_HWSR1_RES15_GET(reg)               ((reg & VIN_DEV_MR_HWSR1_RES15) >> 15)
#define VIN_DEV_MR_HWSR1_RES15_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_HWSR1_RES15) | (((val) & 0x0001) << 15)))
#define VIN_DEV_MR_HWSR1_RES14                        (1 << 14)
#define VIN_DEV_MR_HWSR1_RES14_GET(reg)               ((reg & VIN_DEV_MR_HWSR1_RES14) >> 14)
#define VIN_DEV_MR_HWSR1_RES14_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_HWSR1_RES14) | (((val) & 0x0001) << 14)))
#define VIN_DEV_MR_HWSR1_RES13                        (1 << 13)
#define VIN_DEV_MR_HWSR1_RES13_GET(reg)               ((reg & VIN_DEV_MR_HWSR1_RES13) >> 13)
#define VIN_DEV_MR_HWSR1_RES13_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_HWSR1_RES13) | (((val) & 0x0001) << 13)))
#define VIN_DEV_MR_HWSR1_RES12                        (1 << 12)
#define VIN_DEV_MR_HWSR1_RES12_GET(reg)               ((reg & VIN_DEV_MR_HWSR1_RES12) >> 12)
#define VIN_DEV_MR_HWSR1_RES12_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_HWSR1_RES12) | (((val) & 0x0001) << 12)))
#define VIN_DEV_MR_HWSR1_MR_SYNC_FAIL                 (1 << 11)
#define VIN_DEV_MR_HWSR1_MR_SYNC_FAIL_GET(reg)        ((reg & VIN_DEV_MR_HWSR1_MR_SYNC_FAIL) >> 11)
#define VIN_DEV_MR_HWSR1_MR_SYNC_FAIL_SET(reg, val)   (reg = ((reg & ~VIN_DEV_MR_HWSR1_MR_SYNC_FAIL) | (((val) & 0x0001) << 11)))
#define VIN_DEV_MR_HWSR1_MR_CLK_FAIL                  (1 << 10)
#define VIN_DEV_MR_HWSR1_MR_CLK_FAIL_GET(reg)         ((reg & VIN_DEV_MR_HWSR1_MR_CLK_FAIL) >> 10)
#define VIN_DEV_MR_HWSR1_MR_CLK_FAIL_SET(reg, val)    (reg = ((reg & ~VIN_DEV_MR_HWSR1_MR_CLK_FAIL) | (((val) & 0x0001) << 10)))
#define VIN_DEV_MR_HWSR1_MR_TXB_CRASH                 (1 << 9)
#define VIN_DEV_MR_HWSR1_MR_TXB_CRASH_GET(reg)        ((reg & VIN_DEV_MR_HWSR1_MR_TXB_CRASH) >> 9)
#define VIN_DEV_MR_HWSR1_MR_TXB_CRASH_SET(reg, val)   (reg = ((reg & ~VIN_DEV_MR_HWSR1_MR_TXB_CRASH) | (((val) & 0x0001) << 9)))
#define VIN_DEV_MR_HWSR1_MR_TXA_CRASH                 (1 << 8)
#define VIN_DEV_MR_HWSR1_MR_TXA_CRASH_GET(reg)        ((reg & VIN_DEV_MR_HWSR1_MR_TXA_CRASH) >> 8)
#define VIN_DEV_MR_HWSR1_MR_TXA_CRASH_SET(reg, val)   (reg = ((reg & ~VIN_DEV_MR_HWSR1_MR_TXA_CRASH) | (((val) & 0x0001) << 8)))
#define VIN_DEV_MR_HWSR1_MR_EDSP_WD_FAIL              (1 << 7)
#define VIN_DEV_MR_HWSR1_MR_EDSP_WD_FAIL_GET(reg)     ((reg & VIN_DEV_MR_HWSR1_MR_EDSP_WD_FAIL) >> 7)
#define VIN_DEV_MR_HWSR1_MR_EDSP_WD_FAIL_SET(reg, val)(reg = ((reg & ~VIN_DEV_MR_HWSR1_MR_EDSP_WD_FAIL) | (((val) & 0x0001) << 7)))
#define VIN_DEV_MR_HWSR1_RES6                         (1 << 6)
#define VIN_DEV_MR_HWSR1_RES6_GET(reg)                ((reg & VIN_DEV_MR_HWSR1_RES6) >> 6)
#define VIN_DEV_MR_HWSR1_RES6_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_HWSR1_RES6) | (((val) & 0x0001) << 6)))
#define VIN_DEV_MR_HWSR1_RES5                         (1 << 5)
#define VIN_DEV_MR_HWSR1_RES5_GET(reg)                ((reg & VIN_DEV_MR_HWSR1_RES5) >> 5)
#define VIN_DEV_MR_HWSR1_RES5_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_HWSR1_RES5) | (((val) & 0x0001) << 5)))
#define VIN_DEV_MR_HWSR1_RES4                         (1 << 4)
#define VIN_DEV_MR_HWSR1_RES4_GET(reg)                ((reg & VIN_DEV_MR_HWSR1_RES4) >> 4)
#define VIN_DEV_MR_HWSR1_RES4_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_HWSR1_RES4) | (((val) & 0x0001) << 4)))
#define VIN_DEV_MR_HWSR1_MR_MCLK_FAIL                 (1 << 3)
#define VIN_DEV_MR_HWSR1_MR_MCLK_FAIL_GET(reg)        ((reg & VIN_DEV_MR_HWSR1_MR_MCLK_FAIL) >> 3)
#define VIN_DEV_MR_HWSR1_MR_MCLK_FAIL_SET(reg, val)   (reg = ((reg & ~VIN_DEV_MR_HWSR1_MR_MCLK_FAIL) | (((val) & 0x0001) << 3)))
#define VIN_DEV_MR_HWSR1_MR_WOKE_UP                   (1 << 2)
#define VIN_DEV_MR_HWSR1_MR_WOKE_UP_GET(reg)          ((reg & VIN_DEV_MR_HWSR1_MR_WOKE_UP) >> 2)
#define VIN_DEV_MR_HWSR1_MR_WOKE_UP_SET(reg, val)     (reg = ((reg & ~VIN_DEV_MR_HWSR1_MR_WOKE_UP) | (((val) & 0x0001) << 2)))
#define VIN_DEV_MR_HWSR1_RES1                         (1 << 1)
#define VIN_DEV_MR_HWSR1_RES1_GET(reg)                ((reg & VIN_DEV_MR_HWSR1_RES1) >> 1)
#define VIN_DEV_MR_HWSR1_RES1_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_HWSR1_RES1) | (((val) & 0x0001) << 1)))
#define VIN_DEV_MR_HWSR1_MR_HW_ERR                    (1 << 0)
#define VIN_DEV_MR_HWSR1_MR_HW_ERR_GET(reg)           ((reg & VIN_DEV_MR_HWSR1_MR_HW_ERR) >> 0)
#define VIN_DEV_MR_HWSR1_MR_HW_ERR_SET(reg, val)      (reg = ((reg & ~VIN_DEV_MR_HWSR1_MR_HW_ERR) | (((val) & 0x0001) << 0)))

/***Mask Register for Hardware Status Register 2***/

#define VIN_DEV_MR_HWSR2                              ( (VIN_DEV + 0x13) )

#define VIN_DEV_MR_HWSR2_RES15                        (1 << 15)
#define VIN_DEV_MR_HWSR2_RES15_GET(reg)               ((reg & VIN_DEV_MR_HWSR2_RES15) >> 15)
#define VIN_DEV_MR_HWSR2_RES15_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_HWSR2_RES15) | (((val) & 0x0001) << 15)))
#define VIN_DEV_MR_HWSR2_RES14                        (1 << 14)
#define VIN_DEV_MR_HWSR2_RES14_GET(reg)               ((reg & VIN_DEV_MR_HWSR2_RES14) >> 14)
#define VIN_DEV_MR_HWSR2_RES14_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_HWSR2_RES14) | (((val) & 0x0001) << 14)))
#define VIN_DEV_MR_HWSR2_RES13                        (1 << 13)
#define VIN_DEV_MR_HWSR2_RES13_GET(reg)               ((reg & VIN_DEV_MR_HWSR2_RES13) >> 13)
#define VIN_DEV_MR_HWSR2_RES13_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_HWSR2_RES13) | (((val) & 0x0001) << 13)))
#define VIN_DEV_MR_HWSR2_RES12                        (1 << 12)
#define VIN_DEV_MR_HWSR2_RES12_GET(reg)               ((reg & VIN_DEV_MR_HWSR2_RES12) >> 12)
#define VIN_DEV_MR_HWSR2_RES12_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_HWSR2_RES12) | (((val) & 0x0001) << 12)))
#define VIN_DEV_MR_HWSR2_RES11                        (1 << 11)
#define VIN_DEV_MR_HWSR2_RES11_GET(reg)               ((reg & VIN_DEV_MR_HWSR2_RES11) >> 11)
#define VIN_DEV_MR_HWSR2_RES11_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_HWSR2_RES11) | (((val) & 0x0001) << 11)))
#define VIN_DEV_MR_HWSR2_RES10                        (1 << 10)
#define VIN_DEV_MR_HWSR2_RES10_GET(reg)               ((reg & VIN_DEV_MR_HWSR2_RES10) >> 10)
#define VIN_DEV_MR_HWSR2_RES10_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_HWSR2_RES10) | (((val) & 0x0001) << 10)))
#define VIN_DEV_MR_HWSR2_RES9                         (1 << 9)
#define VIN_DEV_MR_HWSR2_RES9_GET(reg)                ((reg & VIN_DEV_MR_HWSR2_RES9) >> 9)
#define VIN_DEV_MR_HWSR2_RES9_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_HWSR2_RES9) | (((val) & 0x0001) << 9)))
#define VIN_DEV_MR_HWSR2_MR_DL_RDY                    (1 << 8)
#define VIN_DEV_MR_HWSR2_MR_DL_RDY_GET(reg)           ((reg & VIN_DEV_MR_HWSR2_MR_DL_RDY) >> 8)
#define VIN_DEV_MR_HWSR2_MR_DL_RDY_SET(reg, val)      (reg = ((reg & ~VIN_DEV_MR_HWSR2_MR_DL_RDY) | (((val) & 0x0001) << 8)))
#define VIN_DEV_MR_HWSR2_RES7                         (1 << 7)
#define VIN_DEV_MR_HWSR2_RES7_GET(reg)                ((reg & VIN_DEV_MR_HWSR2_RES7) >> 7)
#define VIN_DEV_MR_HWSR2_RES7_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_HWSR2_RES7) | (((val) & 0x0001) << 7)))
#define VIN_DEV_MR_HWSR2_RES6                         (1 << 6)
#define VIN_DEV_MR_HWSR2_RES6_GET(reg)                ((reg & VIN_DEV_MR_HWSR2_RES6) >> 6)
#define VIN_DEV_MR_HWSR2_RES6_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_HWSR2_RES6) | (((val) & 0x0001) << 6)))
#define VIN_DEV_MR_HWSR2_RES5                         (1 << 5)
#define VIN_DEV_MR_HWSR2_RES5_GET(reg)                ((reg & VIN_DEV_MR_HWSR2_RES5) >> 5)
#define VIN_DEV_MR_HWSR2_RES5_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_HWSR2_RES5) | (((val) & 0x0001) << 5)))
#define VIN_DEV_MR_HWSR2_RES4                         (1 << 4)
#define VIN_DEV_MR_HWSR2_RES4_GET(reg)                ((reg & VIN_DEV_MR_HWSR2_RES4) >> 4)
#define VIN_DEV_MR_HWSR2_RES4_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_HWSR2_RES4) | (((val) & 0x0001) << 4)))
#define VIN_DEV_MR_HWSR2_RES3                         (1 << 3)
#define VIN_DEV_MR_HWSR2_RES3_GET(reg)                ((reg & VIN_DEV_MR_HWSR2_RES3) >> 3)
#define VIN_DEV_MR_HWSR2_RES3_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_HWSR2_RES3) | (((val) & 0x0001) << 3)))
#define VIN_DEV_MR_HWSR2_RES2                         (1 << 2)
#define VIN_DEV_MR_HWSR2_RES2_GET(reg)                ((reg & VIN_DEV_MR_HWSR2_RES2) >> 2)
#define VIN_DEV_MR_HWSR2_RES2_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_HWSR2_RES2) | (((val) & 0x0001) << 2)))
#define VIN_DEV_MR_HWSR2_MR_EDSP_CS_FAIL              (1 << 1)
#define VIN_DEV_MR_HWSR2_MR_EDSP_CS_FAIL_GET(reg)     ((reg & VIN_DEV_MR_HWSR2_MR_EDSP_CS_FAIL) >> 1)
#define VIN_DEV_MR_HWSR2_MR_EDSP_CS_FAIL_SET(reg, val)(reg = ((reg & ~VIN_DEV_MR_HWSR2_MR_EDSP_CS_FAIL) | (((val) & 0x0001) << 1)))
#define VIN_DEV_MR_HWSR2_MR_EDSP_MIPS_OL              (1 << 0)
#define VIN_DEV_MR_HWSR2_MR_EDSP_MIPS_OL_GET(reg)     ((reg & VIN_DEV_MR_HWSR2_MR_EDSP_MIPS_OL) >> 0)
#define VIN_DEV_MR_HWSR2_MR_EDSP_MIPS_OL_SET(reg, val)(reg = ((reg & ~VIN_DEV_MR_HWSR2_MR_EDSP_MIPS_OL) | (((val) & 0x0001) << 0)))

/***Mask Register for Hardware Status Register 1***/

#define VIN_DEV_MF_HWSR1                              ( (VIN_DEV + 0x72) )

#define VIN_DEV_MF_HWSR1_RES15                        (1 << 15)
#define VIN_DEV_MF_HWSR1_RES15_GET(reg)               ((reg & VIN_DEV_MF_HWSR1_RES15) >> 15)
#define VIN_DEV_MF_HWSR1_RES15_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_HWSR1_RES15) | (((val) & 0x0001) << 15)))
#define VIN_DEV_MF_HWSR1_RES14                        (1 << 14)
#define VIN_DEV_MF_HWSR1_RES14_GET(reg)               ((reg & VIN_DEV_MF_HWSR1_RES14) >> 14)
#define VIN_DEV_MF_HWSR1_RES14_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_HWSR1_RES14) | (((val) & 0x0001) << 14)))
#define VIN_DEV_MF_HWSR1_RES13                        (1 << 13)
#define VIN_DEV_MF_HWSR1_RES13_GET(reg)               ((reg & VIN_DEV_MF_HWSR1_RES13) >> 13)
#define VIN_DEV_MF_HWSR1_RES13_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_HWSR1_RES13) | (((val) & 0x0001) << 13)))
#define VIN_DEV_MF_HWSR1_RES12                        (1 << 12)
#define VIN_DEV_MF_HWSR1_RES12_GET(reg)               ((reg & VIN_DEV_MF_HWSR1_RES12) >> 12)
#define VIN_DEV_MF_HWSR1_RES12_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_HWSR1_RES12) | (((val) & 0x0001) << 12)))
#define VIN_DEV_MF_HWSR1_MF_SYNC_FAIL                 (1 << 11)
#define VIN_DEV_MF_HWSR1_MF_SYNC_FAIL_GET(reg)        ((reg & VIN_DEV_MF_HWSR1_MF_SYNC_FAIL) >> 11)
#define VIN_DEV_MF_HWSR1_MF_SYNC_FAIL_SET(reg, val)   (reg = ((reg & ~VIN_DEV_MF_HWSR1_MF_SYNC_FAIL) | (((val) & 0x0001) << 11)))
#define VIN_DEV_MF_HWSR1_MF_CLK_FAIL                  (1 << 10)
#define VIN_DEV_MF_HWSR1_MF_CLK_FAIL_GET(reg)         ((reg & VIN_DEV_MF_HWSR1_MF_CLK_FAIL) >> 10)
#define VIN_DEV_MF_HWSR1_MF_CLK_FAIL_SET(reg, val)    (reg = ((reg & ~VIN_DEV_MF_HWSR1_MF_CLK_FAIL) | (((val) & 0x0001) << 10)))
#define VIN_DEV_MF_HWSR1_MF_TXB_CRASH                 (1 << 9)
#define VIN_DEV_MF_HWSR1_MF_TXB_CRASH_GET(reg)        ((reg & VIN_DEV_MF_HWSR1_MF_TXB_CRASH) >> 9)
#define VIN_DEV_MF_HWSR1_MF_TXB_CRASH_SET(reg, val)   (reg = ((reg & ~VIN_DEV_MF_HWSR1_MF_TXB_CRASH) | (((val) & 0x0001) << 9)))
#define VIN_DEV_MF_HWSR1_MF_TXA_CRASH                 (1 << 8)
#define VIN_DEV_MF_HWSR1_MF_TXA_CRASH_GET(reg)        ((reg & VIN_DEV_MF_HWSR1_MF_TXA_CRASH) >> 8)
#define VIN_DEV_MF_HWSR1_MF_TXA_CRASH_SET(reg, val)   (reg = ((reg & ~VIN_DEV_MF_HWSR1_MF_TXA_CRASH) | (((val) & 0x0001) << 8)))
#define VIN_DEV_MF_HWSR1_MF_EDSP_WD_FAIL              (1 << 7)
#define VIN_DEV_MF_HWSR1_MF_EDSP_WD_FAIL_GET(reg)     ((reg & VIN_DEV_MF_HWSR1_MF_EDSP_WD_FAIL) >> 7)
#define VIN_DEV_MF_HWSR1_MF_EDSP_WD_FAIL_SET(reg, val)(reg = ((reg & ~VIN_DEV_MF_HWSR1_MF_EDSP_WD_FAIL) | (((val) & 0x0001) << 7)))
#define VIN_DEV_MF_HWSR1_RES6                         (1 << 6)
#define VIN_DEV_MF_HWSR1_RES6_GET(reg)                ((reg & VIN_DEV_MF_HWSR1_RES6) >> 6)
#define VIN_DEV_MF_HWSR1_RES6_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_HWSR1_RES6) | (((val) & 0x0001) << 6)))
#define VIN_DEV_MF_HWSR1_RES5                         (1 << 5)
#define VIN_DEV_MF_HWSR1_RES5_GET(reg)                ((reg & VIN_DEV_MF_HWSR1_RES5) >> 5)
#define VIN_DEV_MF_HWSR1_RES5_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_HWSR1_RES5) | (((val) & 0x0001) << 5)))
#define VIN_DEV_MF_HWSR1_RES4                         (1 << 4)
#define VIN_DEV_MF_HWSR1_RES4_GET(reg)                ((reg & VIN_DEV_MF_HWSR1_RES4) >> 4)
#define VIN_DEV_MF_HWSR1_RES4_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_HWSR1_RES4) | (((val) & 0x0001) << 4)))
#define VIN_DEV_MF_HWSR1_MF_MCLK_FAIL                 (1 << 3)
#define VIN_DEV_MF_HWSR1_MF_MCLK_FAIL_GET(reg)        ((reg & VIN_DEV_MF_HWSR1_MF_MCLK_FAIL) >> 3)
#define VIN_DEV_MF_HWSR1_MF_MCLK_FAIL_SET(reg, val)   (reg = ((reg & ~VIN_DEV_MF_HWSR1_MF_MCLK_FAIL) | (((val) & 0x0001) << 3)))
#define VIN_DEV_MF_HWSR1_MF_WOKE_UP                   (1 << 2)
#define VIN_DEV_MF_HWSR1_MF_WOKE_UP_GET(reg)          ((reg & VIN_DEV_MF_HWSR1_MF_WOKE_UP) >> 2)
#define VIN_DEV_MF_HWSR1_MF_WOKE_UP_SET(reg, val)     (reg = ((reg & ~VIN_DEV_MF_HWSR1_MF_WOKE_UP) | (((val) & 0x0001) << 2)))
#define VIN_DEV_MF_HWSR1_RES1                         (1 << 1)
#define VIN_DEV_MF_HWSR1_RES1_GET(reg)                ((reg & VIN_DEV_MF_HWSR1_RES1) >> 1)
#define VIN_DEV_MF_HWSR1_RES1_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_HWSR1_RES1) | (((val) & 0x0001) << 1)))
#define VIN_DEV_MF_HWSR1_MF_HW_ERR                    (1 << 0)
#define VIN_DEV_MF_HWSR1_MF_HW_ERR_GET(reg)           ((reg & VIN_DEV_MF_HWSR1_MF_HW_ERR) >> 0)
#define VIN_DEV_MF_HWSR1_MF_HW_ERR_SET(reg, val)      (reg = ((reg & ~VIN_DEV_MF_HWSR1_MF_HW_ERR) | (((val) & 0x0001) << 0)))

/***Mask Register for Hardware Status Register 2***/

#define VIN_DEV_MF_HWSR2                              ( (VIN_DEV + 0x73) )

#define VIN_DEV_MF_HWSR2_RES15                        (1 << 15)
#define VIN_DEV_MF_HWSR2_RES15_GET(reg)               ((reg & VIN_DEV_MF_HWSR2_RES15) >> 15)
#define VIN_DEV_MF_HWSR2_RES15_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_HWSR2_RES15) | (((val) & 0x0001) << 15)))
#define VIN_DEV_MF_HWSR2_RES14                        (1 << 14)
#define VIN_DEV_MF_HWSR2_RES14_GET(reg)               ((reg & VIN_DEV_MF_HWSR2_RES14) >> 14)
#define VIN_DEV_MF_HWSR2_RES14_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_HWSR2_RES14) | (((val) & 0x0001) << 14)))
#define VIN_DEV_MF_HWSR2_RES13                        (1 << 13)
#define VIN_DEV_MF_HWSR2_RES13_GET(reg)               ((reg & VIN_DEV_MF_HWSR2_RES13) >> 13)
#define VIN_DEV_MF_HWSR2_RES13_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_HWSR2_RES13) | (((val) & 0x0001) << 13)))
#define VIN_DEV_MF_HWSR2_RES12                        (1 << 12)
#define VIN_DEV_MF_HWSR2_RES12_GET(reg)               ((reg & VIN_DEV_MF_HWSR2_RES12) >> 12)
#define VIN_DEV_MF_HWSR2_RES12_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_HWSR2_RES12) | (((val) & 0x0001) << 12)))
#define VIN_DEV_MF_HWSR2_RES11                        (1 << 11)
#define VIN_DEV_MF_HWSR2_RES11_GET(reg)               ((reg & VIN_DEV_MF_HWSR2_RES11) >> 11)
#define VIN_DEV_MF_HWSR2_RES11_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_HWSR2_RES11) | (((val) & 0x0001) << 11)))
#define VIN_DEV_MF_HWSR2_RES10                        (1 << 10)
#define VIN_DEV_MF_HWSR2_RES10_GET(reg)               ((reg & VIN_DEV_MF_HWSR2_RES10) >> 10)
#define VIN_DEV_MF_HWSR2_RES10_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_HWSR2_RES10) | (((val) & 0x0001) << 10)))
#define VIN_DEV_MF_HWSR2_RES9                         (1 << 9)
#define VIN_DEV_MF_HWSR2_RES9_GET(reg)                ((reg & VIN_DEV_MF_HWSR2_RES9) >> 9)
#define VIN_DEV_MF_HWSR2_RES9_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_HWSR2_RES9) | (((val) & 0x0001) << 9)))
#define VIN_DEV_MF_HWSR2_MF_DL_RDY                    (1 << 8)
#define VIN_DEV_MF_HWSR2_MF_DL_RDY_GET(reg)           ((reg & VIN_DEV_MF_HWSR2_MF_DL_RDY) >> 8)
#define VIN_DEV_MF_HWSR2_MF_DL_RDY_SET(reg, val)      (reg = ((reg & ~VIN_DEV_MF_HWSR2_MF_DL_RDY) | (((val) & 0x0001) << 8)))
#define VIN_DEV_MF_HWSR2_RES7                         (1 << 7)
#define VIN_DEV_MF_HWSR2_RES7_GET(reg)                ((reg & VIN_DEV_MF_HWSR2_RES7) >> 7)
#define VIN_DEV_MF_HWSR2_RES7_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_HWSR2_RES7) | (((val) & 0x0001) << 7)))
#define VIN_DEV_MF_HWSR2_RES6                         (1 << 6)
#define VIN_DEV_MF_HWSR2_RES6_GET(reg)                ((reg & VIN_DEV_MF_HWSR2_RES6) >> 6)
#define VIN_DEV_MF_HWSR2_RES6_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_HWSR2_RES6) | (((val) & 0x0001) << 6)))
#define VIN_DEV_MF_HWSR2_RES5                         (1 << 5)
#define VIN_DEV_MF_HWSR2_RES5_GET(reg)                ((reg & VIN_DEV_MF_HWSR2_RES5) >> 5)
#define VIN_DEV_MF_HWSR2_RES5_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_HWSR2_RES5) | (((val) & 0x0001) << 5)))
#define VIN_DEV_MF_HWSR2_RES4                         (1 << 4)
#define VIN_DEV_MF_HWSR2_RES4_GET(reg)                ((reg & VIN_DEV_MF_HWSR2_RES4) >> 4)
#define VIN_DEV_MF_HWSR2_RES4_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_HWSR2_RES4) | (((val) & 0x0001) << 4)))
#define VIN_DEV_MF_HWSR2_RES3                         (1 << 3)
#define VIN_DEV_MF_HWSR2_RES3_GET(reg)                ((reg & VIN_DEV_MF_HWSR2_RES3) >> 3)
#define VIN_DEV_MF_HWSR2_RES3_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_HWSR2_RES3) | (((val) & 0x0001) << 3)))
#define VIN_DEV_MF_HWSR2_RES2                         (1 << 2)
#define VIN_DEV_MF_HWSR2_RES2_GET(reg)                ((reg & VIN_DEV_MF_HWSR2_RES2) >> 2)
#define VIN_DEV_MF_HWSR2_RES2_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_HWSR2_RES2) | (((val) & 0x0001) << 2)))
#define VIN_DEV_MF_HWSR2_MF_EDSP_CS_FAIL              (1 << 1)
#define VIN_DEV_MF_HWSR2_MF_EDSP_CS_FAIL_GET(reg)     ((reg & VIN_DEV_MF_HWSR2_MF_EDSP_CS_FAIL) >> 1)
#define VIN_DEV_MF_HWSR2_MF_EDSP_CS_FAIL_SET(reg, val)(reg = ((reg & ~VIN_DEV_MF_HWSR2_MF_EDSP_CS_FAIL) | (((val) & 0x0001) << 1)))
#define VIN_DEV_MF_HWSR2_MF_EDSP_MIPS_OL              (1 << 0)
#define VIN_DEV_MF_HWSR2_MF_EDSP_MIPS_OL_GET(reg)     ((reg & VIN_DEV_MF_HWSR2_MF_EDSP_MIPS_OL) >> 0)
#define VIN_DEV_MF_HWSR2_MF_EDSP_MIPS_OL_SET(reg, val)(reg = ((reg & ~VIN_DEV_MF_HWSR2_MF_EDSP_MIPS_OL) | (((val) & 0x0001) << 0)))

/***Mask Register for Mailbox Status Register 1***/

#define VIN_DEV_MR_BXSR1                              ( (VIN_DEV + 0x16) )

#define VIN_DEV_MR_BXSR1_RES15                        (1 << 15)
#define VIN_DEV_MR_BXSR1_RES15_GET(reg)               ((reg & VIN_DEV_MR_BXSR1_RES15) >> 15)
#define VIN_DEV_MR_BXSR1_RES15_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_BXSR1_RES15) | (((val) & 0x0001) << 15)))
#define VIN_DEV_MR_BXSR1_RES14                        (1 << 14)
#define VIN_DEV_MR_BXSR1_RES14_GET(reg)               ((reg & VIN_DEV_MR_BXSR1_RES14) >> 14)
#define VIN_DEV_MR_BXSR1_RES14_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_BXSR1_RES14) | (((val) & 0x0001) << 14)))
#define VIN_DEV_MR_BXSR1_RES13                        (1 << 13)
#define VIN_DEV_MR_BXSR1_RES13_GET(reg)               ((reg & VIN_DEV_MR_BXSR1_RES13) >> 13)
#define VIN_DEV_MR_BXSR1_RES13_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_BXSR1_RES13) | (((val) & 0x0001) << 13)))
#define VIN_DEV_MR_BXSR1_RES12                        (1 << 12)
#define VIN_DEV_MR_BXSR1_RES12_GET(reg)               ((reg & VIN_DEV_MR_BXSR1_RES12) >> 12)
#define VIN_DEV_MR_BXSR1_RES12_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_BXSR1_RES12) | (((val) & 0x0001) << 12)))
#define VIN_DEV_MR_BXSR1_RES11                        (1 << 11)
#define VIN_DEV_MR_BXSR1_RES11_GET(reg)               ((reg & VIN_DEV_MR_BXSR1_RES11) >> 11)
#define VIN_DEV_MR_BXSR1_RES11_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_BXSR1_RES11) | (((val) & 0x0001) << 11)))
#define VIN_DEV_MR_BXSR1_RES10                        (1 << 10)
#define VIN_DEV_MR_BXSR1_RES10_GET(reg)               ((reg & VIN_DEV_MR_BXSR1_RES10) >> 10)
#define VIN_DEV_MR_BXSR1_RES10_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_BXSR1_RES10) | (((val) & 0x0001) << 10)))
#define VIN_DEV_MR_BXSR1_RES9                         (1 << 9)
#define VIN_DEV_MR_BXSR1_RES9_GET(reg)                ((reg & VIN_DEV_MR_BXSR1_RES9) >> 9)
#define VIN_DEV_MR_BXSR1_RES9_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_BXSR1_RES9) | (((val) & 0x0001) << 9)))
#define VIN_DEV_MR_BXSR1_MR_CERR                      (1 << 8)
#define VIN_DEV_MR_BXSR1_MR_CERR_GET(reg)             ((reg & VIN_DEV_MR_BXSR1_MR_CERR) >> 8)
#define VIN_DEV_MR_BXSR1_MR_CERR_SET(reg, val)        (reg = ((reg & ~VIN_DEV_MR_BXSR1_MR_CERR) | (((val) & 0x0001) << 8)))
#define VIN_DEV_MR_BXSR1_RES7                         (1 << 7)
#define VIN_DEV_MR_BXSR1_RES7_GET(reg)                ((reg & VIN_DEV_MR_BXSR1_RES7) >> 7)
#define VIN_DEV_MR_BXSR1_RES7_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_BXSR1_RES7) | (((val) & 0x0001) << 7)))
#define VIN_DEV_MR_BXSR1_RES6                         (1 << 6)
#define VIN_DEV_MR_BXSR1_RES6_GET(reg)                ((reg & VIN_DEV_MR_BXSR1_RES6) >> 6)
#define VIN_DEV_MR_BXSR1_RES6_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_BXSR1_RES6) | (((val) & 0x0001) << 6)))
#define VIN_DEV_MR_BXSR1_RES5                         (1 << 5)
#define VIN_DEV_MR_BXSR1_RES5_GET(reg)                ((reg & VIN_DEV_MR_BXSR1_RES5) >> 5)
#define VIN_DEV_MR_BXSR1_RES5_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_BXSR1_RES5) | (((val) & 0x0001) << 5)))
#define VIN_DEV_MR_BXSR1_RES4                         (1 << 4)
#define VIN_DEV_MR_BXSR1_RES4_GET(reg)                ((reg & VIN_DEV_MR_BXSR1_RES4) >> 4)
#define VIN_DEV_MR_BXSR1_RES4_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_BXSR1_RES4) | (((val) & 0x0001) << 4)))
#define VIN_DEV_MR_BXSR1_RES3                         (1 << 3)
#define VIN_DEV_MR_BXSR1_RES3_GET(reg)                ((reg & VIN_DEV_MR_BXSR1_RES3) >> 3)
#define VIN_DEV_MR_BXSR1_RES3_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_BXSR1_RES3) | (((val) & 0x0001) << 3)))
#define VIN_DEV_MR_BXSR1_RES2                         (1 << 2)
#define VIN_DEV_MR_BXSR1_RES2_GET(reg)                ((reg & VIN_DEV_MR_BXSR1_RES2) >> 2)
#define VIN_DEV_MR_BXSR1_RES2_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_BXSR1_RES2) | (((val) & 0x0001) << 2)))
#define VIN_DEV_MR_BXSR1_RES1                         (1 << 1)
#define VIN_DEV_MR_BXSR1_RES1_GET(reg)                ((reg & VIN_DEV_MR_BXSR1_RES1) >> 1)
#define VIN_DEV_MR_BXSR1_RES1_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_BXSR1_RES1) | (((val) & 0x0001) << 1)))
#define VIN_DEV_MR_BXSR1_RES0                         (1 << 0)
#define VIN_DEV_MR_BXSR1_RES0_GET(reg)                ((reg & VIN_DEV_MR_BXSR1_RES0) >> 0)
#define VIN_DEV_MR_BXSR1_RES0_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_BXSR1_RES0) | (((val) & 0x0001) << 0)))

/***Mask Register for Mailbox Status Register 2***/

#define VIN_DEV_MR_BXSR2                              ( (VIN_DEV + 0x17) )

#define VIN_DEV_MR_BXSR2_RES15                        (1 << 15)
#define VIN_DEV_MR_BXSR2_RES15_GET(reg)               ((reg & VIN_DEV_MR_BXSR2_RES15) >> 15)
#define VIN_DEV_MR_BXSR2_RES15_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_BXSR2_RES15) | (((val) & 0x0001) << 15)))
#define VIN_DEV_MR_BXSR2_RES14                        (1 << 14)
#define VIN_DEV_MR_BXSR2_RES14_GET(reg)               ((reg & VIN_DEV_MR_BXSR2_RES14) >> 14)
#define VIN_DEV_MR_BXSR2_RES14_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_BXSR2_RES14) | (((val) & 0x0001) << 14)))
#define VIN_DEV_MR_BXSR2_RES13                        (1 << 13)
#define VIN_DEV_MR_BXSR2_RES13_GET(reg)               ((reg & VIN_DEV_MR_BXSR2_RES13) >> 13)
#define VIN_DEV_MR_BXSR2_RES13_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_BXSR2_RES13) | (((val) & 0x0001) << 13)))
#define VIN_DEV_MR_BXSR2_RES12                        (1 << 12)
#define VIN_DEV_MR_BXSR2_RES12_GET(reg)               ((reg & VIN_DEV_MR_BXSR2_RES12) >> 12)
#define VIN_DEV_MR_BXSR2_RES12_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_BXSR2_RES12) | (((val) & 0x0001) << 12)))
#define VIN_DEV_MR_BXSR2_RES11                        (1 << 11)
#define VIN_DEV_MR_BXSR2_RES11_GET(reg)               ((reg & VIN_DEV_MR_BXSR2_RES11) >> 11)
#define VIN_DEV_MR_BXSR2_RES11_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_BXSR2_RES11) | (((val) & 0x0001) << 11)))
#define VIN_DEV_MR_BXSR2_RES10                        (1 << 10)
#define VIN_DEV_MR_BXSR2_RES10_GET(reg)               ((reg & VIN_DEV_MR_BXSR2_RES10) >> 10)
#define VIN_DEV_MR_BXSR2_RES10_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_BXSR2_RES10) | (((val) & 0x0001) << 10)))
#define VIN_DEV_MR_BXSR2_RES9                         (1 << 9)
#define VIN_DEV_MR_BXSR2_RES9_GET(reg)                ((reg & VIN_DEV_MR_BXSR2_RES9) >> 9)
#define VIN_DEV_MR_BXSR2_RES9_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_BXSR2_RES9) | (((val) & 0x0001) << 9)))
#define VIN_DEV_MR_BXSR2_RES8                         (1 << 8)
#define VIN_DEV_MR_BXSR2_RES8_GET(reg)                ((reg & VIN_DEV_MR_BXSR2_RES8) >> 8)
#define VIN_DEV_MR_BXSR2_RES8_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_BXSR2_RES8) | (((val) & 0x0001) << 8)))
#define VIN_DEV_MR_BXSR2_RES7                         (1 << 7)
#define VIN_DEV_MR_BXSR2_RES7_GET(reg)                ((reg & VIN_DEV_MR_BXSR2_RES7) >> 7)
#define VIN_DEV_MR_BXSR2_RES7_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_BXSR2_RES7) | (((val) & 0x0001) << 7)))
#define VIN_DEV_MR_BXSR2_RES6                         (1 << 6)
#define VIN_DEV_MR_BXSR2_RES6_GET(reg)                ((reg & VIN_DEV_MR_BXSR2_RES6) >> 6)
#define VIN_DEV_MR_BXSR2_RES6_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MR_BXSR2_RES6) | (((val) & 0x0001) << 6)))
#define VIN_DEV_MR_BXSR2_MR_HOST_ERR                  (1 << 5)
#define VIN_DEV_MR_BXSR2_MR_HOST_ERR_GET(reg)         ((reg & VIN_DEV_MR_BXSR2_MR_HOST_ERR) >> 5)
#define VIN_DEV_MR_BXSR2_MR_HOST_ERR_SET(reg, val)    (reg = ((reg & ~VIN_DEV_MR_BXSR2_MR_HOST_ERR) | (((val) & 0x0001) << 5)))
#define VIN_DEV_MR_BXSR2_MR_POBX_DATA                 (1 << 4)
#define VIN_DEV_MR_BXSR2_MR_POBX_DATA_GET(reg)        ((reg & VIN_DEV_MR_BXSR2_MR_POBX_DATA) >> 4)
#define VIN_DEV_MR_BXSR2_MR_POBX_DATA_SET(reg, val)   (reg = ((reg & ~VIN_DEV_MR_BXSR2_MR_POBX_DATA) | (((val) & 0x0001) << 4)))
#define VIN_DEV_MR_BXSR2_MR_COBX_DATA                 (1 << 3)
#define VIN_DEV_MR_BXSR2_MR_COBX_DATA_GET(reg)        ((reg & VIN_DEV_MR_BXSR2_MR_COBX_DATA) >> 3)
#define VIN_DEV_MR_BXSR2_MR_COBX_DATA_SET(reg, val)   (reg = ((reg & ~VIN_DEV_MR_BXSR2_MR_COBX_DATA) | (((val) & 0x0001) << 3)))
#define VIN_DEV_MR_BXSR2_MR_PIBX_OF                   (1 << 2)
#define VIN_DEV_MR_BXSR2_MR_PIBX_OF_GET(reg)          ((reg & VIN_DEV_MR_BXSR2_MR_PIBX_OF) >> 2)
#define VIN_DEV_MR_BXSR2_MR_PIBX_OF_SET(reg, val)     (reg = ((reg & ~VIN_DEV_MR_BXSR2_MR_PIBX_OF) | (((val) & 0x0001) << 2)))
#define VIN_DEV_MR_BXSR2_MR_CIBX_OF                   (1 << 1)
#define VIN_DEV_MR_BXSR2_MR_CIBX_OF_GET(reg)          ((reg & VIN_DEV_MR_BXSR2_MR_CIBX_OF) >> 1)
#define VIN_DEV_MR_BXSR2_MR_CIBX_OF_SET(reg, val)     (reg = ((reg & ~VIN_DEV_MR_BXSR2_MR_CIBX_OF) | (((val) & 0x0001) << 1)))
#define VIN_DEV_MR_BXSR2_MR_MBX_EMPTY                 (1 << 0)
#define VIN_DEV_MR_BXSR2_MR_MBX_EMPTY_GET(reg)        ((reg & VIN_DEV_MR_BXSR2_MR_MBX_EMPTY) >> 0)
#define VIN_DEV_MR_BXSR2_MR_MBX_EMPTY_SET(reg, val)   (reg = ((reg & ~VIN_DEV_MR_BXSR2_MR_MBX_EMPTY) | (((val) & 0x0001) << 0)))

/***Mask Register for Status Register for GPIO interrupts
***/

#define VIN_DEV_MR_SRGPIO                             ( (VIN_DEV + 0x1a) )

#define VIN_DEV_MR_SRGPIO_RES15                       (1 << 15)
#define VIN_DEV_MR_SRGPIO_RES15_GET(reg)              ((reg & VIN_DEV_MR_SRGPIO_RES15) >> 15)
#define VIN_DEV_MR_SRGPIO_RES15_SET(reg, val)         (reg = ((reg & ~VIN_DEV_MR_SRGPIO_RES15) | (((val) & 0x0001) << 15)))
#define VIN_DEV_MR_SRGPIO_RES14                       (1 << 14)
#define VIN_DEV_MR_SRGPIO_RES14_GET(reg)              ((reg & VIN_DEV_MR_SRGPIO_RES14) >> 14)
#define VIN_DEV_MR_SRGPIO_RES14_SET(reg, val)         (reg = ((reg & ~VIN_DEV_MR_SRGPIO_RES14) | (((val) & 0x0001) << 14)))
#define VIN_DEV_MR_SRGPIO_RES13                       (1 << 13)
#define VIN_DEV_MR_SRGPIO_RES13_GET(reg)              ((reg & VIN_DEV_MR_SRGPIO_RES13) >> 13)
#define VIN_DEV_MR_SRGPIO_RES13_SET(reg, val)         (reg = ((reg & ~VIN_DEV_MR_SRGPIO_RES13) | (((val) & 0x0001) << 13)))
#define VIN_DEV_MR_SRGPIO_RES12                       (1 << 12)
#define VIN_DEV_MR_SRGPIO_RES12_GET(reg)              ((reg & VIN_DEV_MR_SRGPIO_RES12) >> 12)
#define VIN_DEV_MR_SRGPIO_RES12_SET(reg, val)         (reg = ((reg & ~VIN_DEV_MR_SRGPIO_RES12) | (((val) & 0x0001) << 12)))
#define VIN_DEV_MR_SRGPIO_RES11                       (1 << 11)
#define VIN_DEV_MR_SRGPIO_RES11_GET(reg)              ((reg & VIN_DEV_MR_SRGPIO_RES11) >> 11)
#define VIN_DEV_MR_SRGPIO_RES11_SET(reg, val)         (reg = ((reg & ~VIN_DEV_MR_SRGPIO_RES11) | (((val) & 0x0001) << 11)))
#define VIN_DEV_MR_SRGPIO_RES10                       (1 << 10)
#define VIN_DEV_MR_SRGPIO_RES10_GET(reg)              ((reg & VIN_DEV_MR_SRGPIO_RES10) >> 10)
#define VIN_DEV_MR_SRGPIO_RES10_SET(reg, val)         (reg = ((reg & ~VIN_DEV_MR_SRGPIO_RES10) | (((val) & 0x0001) << 10)))
#define VIN_DEV_MR_SRGPIO_RES9                        (1 << 9)
#define VIN_DEV_MR_SRGPIO_RES9_GET(reg)               ((reg & VIN_DEV_MR_SRGPIO_RES9) >> 9)
#define VIN_DEV_MR_SRGPIO_RES9_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_SRGPIO_RES9) | (((val) & 0x0001) << 9)))
#define VIN_DEV_MR_SRGPIO_RES8                        (1 << 8)
#define VIN_DEV_MR_SRGPIO_RES8_GET(reg)               ((reg & VIN_DEV_MR_SRGPIO_RES8) >> 8)
#define VIN_DEV_MR_SRGPIO_RES8_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MR_SRGPIO_RES8) | (((val) & 0x0001) << 8)))
#define VIN_DEV_MR_SRGPIO_MR_GPIO7                    (1 << 7)
#define VIN_DEV_MR_SRGPIO_MR_GPIO7_GET(reg)           ((reg & VIN_DEV_MR_SRGPIO_MR_GPIO7) >> 7)
#define VIN_DEV_MR_SRGPIO_MR_GPIO7_SET(reg, val)      (reg = ((reg & ~VIN_DEV_MR_SRGPIO_MR_GPIO7) | (((val) & 0x0001) << 7)))
#define VIN_DEV_MR_SRGPIO_MR_GPIO6                    (1 << 6)
#define VIN_DEV_MR_SRGPIO_MR_GPIO6_GET(reg)           ((reg & VIN_DEV_MR_SRGPIO_MR_GPIO6) >> 6)
#define VIN_DEV_MR_SRGPIO_MR_GPIO6_SET(reg, val)      (reg = ((reg & ~VIN_DEV_MR_SRGPIO_MR_GPIO6) | (((val) & 0x0001) << 6)))
#define VIN_DEV_MR_SRGPIO_MR_GPIO5                    (1 << 5)
#define VIN_DEV_MR_SRGPIO_MR_GPIO5_GET(reg)           ((reg & VIN_DEV_MR_SRGPIO_MR_GPIO5) >> 5)
#define VIN_DEV_MR_SRGPIO_MR_GPIO5_SET(reg, val)      (reg = ((reg & ~VIN_DEV_MR_SRGPIO_MR_GPIO5) | (((val) & 0x0001) << 5)))
#define VIN_DEV_MR_SRGPIO_MR_GPIO4                    (1 << 4)
#define VIN_DEV_MR_SRGPIO_MR_GPIO4_GET(reg)           ((reg & VIN_DEV_MR_SRGPIO_MR_GPIO4) >> 4)
#define VIN_DEV_MR_SRGPIO_MR_GPIO4_SET(reg, val)      (reg = ((reg & ~VIN_DEV_MR_SRGPIO_MR_GPIO4) | (((val) & 0x0001) << 4)))
#define VIN_DEV_MR_SRGPIO_MR_GPIO3                    (1 << 3)
#define VIN_DEV_MR_SRGPIO_MR_GPIO3_GET(reg)           ((reg & VIN_DEV_MR_SRGPIO_MR_GPIO3) >> 3)
#define VIN_DEV_MR_SRGPIO_MR_GPIO3_SET(reg, val)      (reg = ((reg & ~VIN_DEV_MR_SRGPIO_MR_GPIO3) | (((val) & 0x0001) << 3)))
#define VIN_DEV_MR_SRGPIO_MR_GPIO2                    (1 << 2)
#define VIN_DEV_MR_SRGPIO_MR_GPIO2_GET(reg)           ((reg & VIN_DEV_MR_SRGPIO_MR_GPIO2) >> 2)
#define VIN_DEV_MR_SRGPIO_MR_GPIO2_SET(reg, val)      (reg = ((reg & ~VIN_DEV_MR_SRGPIO_MR_GPIO2) | (((val) & 0x0001) << 2)))
#define VIN_DEV_MR_SRGPIO_MR_GPIO1                    (1 << 1)
#define VIN_DEV_MR_SRGPIO_MR_GPIO1_GET(reg)           ((reg & VIN_DEV_MR_SRGPIO_MR_GPIO1) >> 1)
#define VIN_DEV_MR_SRGPIO_MR_GPIO1_SET(reg, val)      (reg = ((reg & ~VIN_DEV_MR_SRGPIO_MR_GPIO1) | (((val) & 0x0001) << 1)))
#define VIN_DEV_MR_SRGPIO_MR_GPIO0                    (1 << 0)
#define VIN_DEV_MR_SRGPIO_MR_GPIO0_GET(reg)           ((reg & VIN_DEV_MR_SRGPIO_MR_GPIO0) >> 0)
#define VIN_DEV_MR_SRGPIO_MR_GPIO0_SET(reg, val)      (reg = ((reg & ~VIN_DEV_MR_SRGPIO_MR_GPIO0) | (((val) & 0x0001) << 0)))

/***Mask Register for Mailbox Status Register 1***/

#define VIN_DEV_MF_BXSR1                              ( (VIN_DEV + 0x76) )

#define VIN_DEV_MF_BXSR1_RES15                        (1 << 15)
#define VIN_DEV_MF_BXSR1_RES15_GET(reg)               ((reg & VIN_DEV_MF_BXSR1_RES15) >> 15)
#define VIN_DEV_MF_BXSR1_RES15_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_BXSR1_RES15) | (((val) & 0x0001) << 15)))
#define VIN_DEV_MF_BXSR1_RES14                        (1 << 14)
#define VIN_DEV_MF_BXSR1_RES14_GET(reg)               ((reg & VIN_DEV_MF_BXSR1_RES14) >> 14)
#define VIN_DEV_MF_BXSR1_RES14_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_BXSR1_RES14) | (((val) & 0x0001) << 14)))
#define VIN_DEV_MF_BXSR1_RES13                        (1 << 13)
#define VIN_DEV_MF_BXSR1_RES13_GET(reg)               ((reg & VIN_DEV_MF_BXSR1_RES13) >> 13)
#define VIN_DEV_MF_BXSR1_RES13_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_BXSR1_RES13) | (((val) & 0x0001) << 13)))
#define VIN_DEV_MF_BXSR1_RES12                        (1 << 12)
#define VIN_DEV_MF_BXSR1_RES12_GET(reg)               ((reg & VIN_DEV_MF_BXSR1_RES12) >> 12)
#define VIN_DEV_MF_BXSR1_RES12_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_BXSR1_RES12) | (((val) & 0x0001) << 12)))
#define VIN_DEV_MF_BXSR1_RES11                        (1 << 11)
#define VIN_DEV_MF_BXSR1_RES11_GET(reg)               ((reg & VIN_DEV_MF_BXSR1_RES11) >> 11)
#define VIN_DEV_MF_BXSR1_RES11_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_BXSR1_RES11) | (((val) & 0x0001) << 11)))
#define VIN_DEV_MF_BXSR1_RES10                        (1 << 10)
#define VIN_DEV_MF_BXSR1_RES10_GET(reg)               ((reg & VIN_DEV_MF_BXSR1_RES10) >> 10)
#define VIN_DEV_MF_BXSR1_RES10_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_BXSR1_RES10) | (((val) & 0x0001) << 10)))
#define VIN_DEV_MF_BXSR1_RES9                         (1 << 9)
#define VIN_DEV_MF_BXSR1_RES9_GET(reg)                ((reg & VIN_DEV_MF_BXSR1_RES9) >> 9)
#define VIN_DEV_MF_BXSR1_RES9_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_BXSR1_RES9) | (((val) & 0x0001) << 9)))
#define VIN_DEV_MF_BXSR1_MF_CERR                      (1 << 8)
#define VIN_DEV_MF_BXSR1_MF_CERR_GET(reg)             ((reg & VIN_DEV_MF_BXSR1_MF_CERR) >> 8)
#define VIN_DEV_MF_BXSR1_MF_CERR_SET(reg, val)        (reg = ((reg & ~VIN_DEV_MF_BXSR1_MF_CERR) | (((val) & 0x0001) << 8)))
#define VIN_DEV_MF_BXSR1_RES7                         (1 << 7)
#define VIN_DEV_MF_BXSR1_RES7_GET(reg)                ((reg & VIN_DEV_MF_BXSR1_RES7) >> 7)
#define VIN_DEV_MF_BXSR1_RES7_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_BXSR1_RES7) | (((val) & 0x0001) << 7)))
#define VIN_DEV_MF_BXSR1_RES6                         (1 << 6)
#define VIN_DEV_MF_BXSR1_RES6_GET(reg)                ((reg & VIN_DEV_MF_BXSR1_RES6) >> 6)
#define VIN_DEV_MF_BXSR1_RES6_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_BXSR1_RES6) | (((val) & 0x0001) << 6)))
#define VIN_DEV_MF_BXSR1_RES5                         (1 << 5)
#define VIN_DEV_MF_BXSR1_RES5_GET(reg)                ((reg & VIN_DEV_MF_BXSR1_RES5) >> 5)
#define VIN_DEV_MF_BXSR1_RES5_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_BXSR1_RES5) | (((val) & 0x0001) << 5)))
#define VIN_DEV_MF_BXSR1_RES4                         (1 << 4)
#define VIN_DEV_MF_BXSR1_RES4_GET(reg)                ((reg & VIN_DEV_MF_BXSR1_RES4) >> 4)
#define VIN_DEV_MF_BXSR1_RES4_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_BXSR1_RES4) | (((val) & 0x0001) << 4)))
#define VIN_DEV_MF_BXSR1_RES3                         (1 << 3)
#define VIN_DEV_MF_BXSR1_RES3_GET(reg)                ((reg & VIN_DEV_MF_BXSR1_RES3) >> 3)
#define VIN_DEV_MF_BXSR1_RES3_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_BXSR1_RES3) | (((val) & 0x0001) << 3)))
#define VIN_DEV_MF_BXSR1_RES2                         (1 << 2)
#define VIN_DEV_MF_BXSR1_RES2_GET(reg)                ((reg & VIN_DEV_MF_BXSR1_RES2) >> 2)
#define VIN_DEV_MF_BXSR1_RES2_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_BXSR1_RES2) | (((val) & 0x0001) << 2)))
#define VIN_DEV_MF_BXSR1_RES1                         (1 << 1)
#define VIN_DEV_MF_BXSR1_RES1_GET(reg)                ((reg & VIN_DEV_MF_BXSR1_RES1) >> 1)
#define VIN_DEV_MF_BXSR1_RES1_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_BXSR1_RES1) | (((val) & 0x0001) << 1)))
#define VIN_DEV_MF_BXSR1_RES0                         (1 << 0)
#define VIN_DEV_MF_BXSR1_RES0_GET(reg)                ((reg & VIN_DEV_MF_BXSR1_RES0) >> 0)
#define VIN_DEV_MF_BXSR1_RES0_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_BXSR1_RES0) | (((val) & 0x0001) << 0)))

/***Mask Register for Mailbox Status Register 2***/

#define VIN_DEV_MF_BXSR2                              ( (VIN_DEV + 0x77) )

#define VIN_DEV_MF_BXSR2_RES15                        (1 << 15)
#define VIN_DEV_MF_BXSR2_RES15_GET(reg)               ((reg & VIN_DEV_MF_BXSR2_RES15) >> 15)
#define VIN_DEV_MF_BXSR2_RES15_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_BXSR2_RES15) | (((val) & 0x0001) << 15)))
#define VIN_DEV_MF_BXSR2_RES14                        (1 << 14)
#define VIN_DEV_MF_BXSR2_RES14_GET(reg)               ((reg & VIN_DEV_MF_BXSR2_RES14) >> 14)
#define VIN_DEV_MF_BXSR2_RES14_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_BXSR2_RES14) | (((val) & 0x0001) << 14)))
#define VIN_DEV_MF_BXSR2_RES13                        (1 << 13)
#define VIN_DEV_MF_BXSR2_RES13_GET(reg)               ((reg & VIN_DEV_MF_BXSR2_RES13) >> 13)
#define VIN_DEV_MF_BXSR2_RES13_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_BXSR2_RES13) | (((val) & 0x0001) << 13)))
#define VIN_DEV_MF_BXSR2_RES12                        (1 << 12)
#define VIN_DEV_MF_BXSR2_RES12_GET(reg)               ((reg & VIN_DEV_MF_BXSR2_RES12) >> 12)
#define VIN_DEV_MF_BXSR2_RES12_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_BXSR2_RES12) | (((val) & 0x0001) << 12)))
#define VIN_DEV_MF_BXSR2_RES11                        (1 << 11)
#define VIN_DEV_MF_BXSR2_RES11_GET(reg)               ((reg & VIN_DEV_MF_BXSR2_RES11) >> 11)
#define VIN_DEV_MF_BXSR2_RES11_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_BXSR2_RES11) | (((val) & 0x0001) << 11)))
#define VIN_DEV_MF_BXSR2_RES10                        (1 << 10)
#define VIN_DEV_MF_BXSR2_RES10_GET(reg)               ((reg & VIN_DEV_MF_BXSR2_RES10) >> 10)
#define VIN_DEV_MF_BXSR2_RES10_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_BXSR2_RES10) | (((val) & 0x0001) << 10)))
#define VIN_DEV_MF_BXSR2_RES9                         (1 << 9)
#define VIN_DEV_MF_BXSR2_RES9_GET(reg)                ((reg & VIN_DEV_MF_BXSR2_RES9) >> 9)
#define VIN_DEV_MF_BXSR2_RES9_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_BXSR2_RES9) | (((val) & 0x0001) << 9)))
#define VIN_DEV_MF_BXSR2_RES8                         (1 << 8)
#define VIN_DEV_MF_BXSR2_RES8_GET(reg)                ((reg & VIN_DEV_MF_BXSR2_RES8) >> 8)
#define VIN_DEV_MF_BXSR2_RES8_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_BXSR2_RES8) | (((val) & 0x0001) << 8)))
#define VIN_DEV_MF_BXSR2_RES7                         (1 << 7)
#define VIN_DEV_MF_BXSR2_RES7_GET(reg)                ((reg & VIN_DEV_MF_BXSR2_RES7) >> 7)
#define VIN_DEV_MF_BXSR2_RES7_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_BXSR2_RES7) | (((val) & 0x0001) << 7)))
#define VIN_DEV_MF_BXSR2_RES6                         (1 << 6)
#define VIN_DEV_MF_BXSR2_RES6_GET(reg)                ((reg & VIN_DEV_MF_BXSR2_RES6) >> 6)
#define VIN_DEV_MF_BXSR2_RES6_SET(reg, val)           (reg = ((reg & ~VIN_DEV_MF_BXSR2_RES6) | (((val) & 0x0001) << 6)))
#define VIN_DEV_MF_BXSR2_MF_HOST_ERR                  (1 << 5)
#define VIN_DEV_MF_BXSR2_MF_HOST_ERR_GET(reg)         ((reg & VIN_DEV_MF_BXSR2_MF_HOST_ERR) >> 5)
#define VIN_DEV_MF_BXSR2_MF_HOST_ERR_SET(reg, val)    (reg = ((reg & ~VIN_DEV_MF_BXSR2_MF_HOST_ERR) | (((val) & 0x0001) << 5)))
#define VIN_DEV_MF_BXSR2_MF_POBX_DATA                 (1 << 4)
#define VIN_DEV_MF_BXSR2_MF_POBX_DATA_GET(reg)        ((reg & VIN_DEV_MF_BXSR2_MF_POBX_DATA) >> 4)
#define VIN_DEV_MF_BXSR2_MF_POBX_DATA_SET(reg, val)   (reg = ((reg & ~VIN_DEV_MF_BXSR2_MF_POBX_DATA) | (((val) & 0x0001) << 4)))
#define VIN_DEV_MF_BXSR2_MF_COBX_DATA                 (1 << 3)
#define VIN_DEV_MF_BXSR2_MF_COBX_DATA_GET(reg)        ((reg & VIN_DEV_MF_BXSR2_MF_COBX_DATA) >> 3)
#define VIN_DEV_MF_BXSR2_MF_COBX_DATA_SET(reg, val)   (reg = ((reg & ~VIN_DEV_MF_BXSR2_MF_COBX_DATA) | (((val) & 0x0001) << 3)))
#define VIN_DEV_MF_BXSR2_MF_PIBX_OF                   (1 << 2)
#define VIN_DEV_MF_BXSR2_MF_PIBX_OF_GET(reg)          ((reg & VIN_DEV_MF_BXSR2_MF_PIBX_OF) >> 2)
#define VIN_DEV_MF_BXSR2_MF_PIBX_OF_SET(reg, val)     (reg = ((reg & ~VIN_DEV_MF_BXSR2_MF_PIBX_OF) | (((val) & 0x0001) << 2)))
#define VIN_DEV_MF_BXSR2_MF_CIBX_OF                   (1 << 1)
#define VIN_DEV_MF_BXSR2_MF_CIBX_OF_GET(reg)          ((reg & VIN_DEV_MF_BXSR2_MF_CIBX_OF) >> 1)
#define VIN_DEV_MF_BXSR2_MF_CIBX_OF_SET(reg, val)     (reg = ((reg & ~VIN_DEV_MF_BXSR2_MF_CIBX_OF) | (((val) & 0x0001) << 1)))
#define VIN_DEV_MF_BXSR2_MF_MBX_EMPTY                 (1 << 0)
#define VIN_DEV_MF_BXSR2_MF_MBX_EMPTY_GET(reg)        ((reg & VIN_DEV_MF_BXSR2_MF_MBX_EMPTY) >> 0)
#define VIN_DEV_MF_BXSR2_MF_MBX_EMPTY_SET(reg, val)   (reg = ((reg & ~VIN_DEV_MF_BXSR2_MF_MBX_EMPTY) | (((val) & 0x0001) << 0)))

/***Mask Register for Status Register for GPIO interrupts
***/

#define VIN_DEV_MF_SRGPIO                             ( (VIN_DEV + 0x7a) )

#define VIN_DEV_MF_SRGPIO_RES15                       (1 << 15)
#define VIN_DEV_MF_SRGPIO_RES15_GET(reg)              ((reg & VIN_DEV_MF_SRGPIO_RES15) >> 15)
#define VIN_DEV_MF_SRGPIO_RES15_SET(reg, val)         (reg = ((reg & ~VIN_DEV_MF_SRGPIO_RES15) | (((val) & 0x0001) << 15)))
#define VIN_DEV_MF_SRGPIO_RES14                       (1 << 14)
#define VIN_DEV_MF_SRGPIO_RES14_GET(reg)              ((reg & VIN_DEV_MF_SRGPIO_RES14) >> 14)
#define VIN_DEV_MF_SRGPIO_RES14_SET(reg, val)         (reg = ((reg & ~VIN_DEV_MF_SRGPIO_RES14) | (((val) & 0x0001) << 14)))
#define VIN_DEV_MF_SRGPIO_RES13                       (1 << 13)
#define VIN_DEV_MF_SRGPIO_RES13_GET(reg)              ((reg & VIN_DEV_MF_SRGPIO_RES13) >> 13)
#define VIN_DEV_MF_SRGPIO_RES13_SET(reg, val)         (reg = ((reg & ~VIN_DEV_MF_SRGPIO_RES13) | (((val) & 0x0001) << 13)))
#define VIN_DEV_MF_SRGPIO_RES12                       (1 << 12)
#define VIN_DEV_MF_SRGPIO_RES12_GET(reg)              ((reg & VIN_DEV_MF_SRGPIO_RES12) >> 12)
#define VIN_DEV_MF_SRGPIO_RES12_SET(reg, val)         (reg = ((reg & ~VIN_DEV_MF_SRGPIO_RES12) | (((val) & 0x0001) << 12)))
#define VIN_DEV_MF_SRGPIO_RES11                       (1 << 11)
#define VIN_DEV_MF_SRGPIO_RES11_GET(reg)              ((reg & VIN_DEV_MF_SRGPIO_RES11) >> 11)
#define VIN_DEV_MF_SRGPIO_RES11_SET(reg, val)         (reg = ((reg & ~VIN_DEV_MF_SRGPIO_RES11) | (((val) & 0x0001) << 11)))
#define VIN_DEV_MF_SRGPIO_RES10                       (1 << 10)
#define VIN_DEV_MF_SRGPIO_RES10_GET(reg)              ((reg & VIN_DEV_MF_SRGPIO_RES10) >> 10)
#define VIN_DEV_MF_SRGPIO_RES10_SET(reg, val)         (reg = ((reg & ~VIN_DEV_MF_SRGPIO_RES10) | (((val) & 0x0001) << 10)))
#define VIN_DEV_MF_SRGPIO_RES9                        (1 << 9)
#define VIN_DEV_MF_SRGPIO_RES9_GET(reg)               ((reg & VIN_DEV_MF_SRGPIO_RES9) >> 9)
#define VIN_DEV_MF_SRGPIO_RES9_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_SRGPIO_RES9) | (((val) & 0x0001) << 9)))
#define VIN_DEV_MF_SRGPIO_RES8                        (1 << 8)
#define VIN_DEV_MF_SRGPIO_RES8_GET(reg)               ((reg & VIN_DEV_MF_SRGPIO_RES8) >> 8)
#define VIN_DEV_MF_SRGPIO_RES8_SET(reg, val)          (reg = ((reg & ~VIN_DEV_MF_SRGPIO_RES8) | (((val) & 0x0001) << 8)))
#define VIN_DEV_MF_SRGPIO_MF_GPIO7                    (1 << 7)
#define VIN_DEV_MF_SRGPIO_MF_GPIO7_GET(reg)           ((reg & VIN_DEV_MF_SRGPIO_MF_GPIO7) >> 7)
#define VIN_DEV_MF_SRGPIO_MF_GPIO7_SET(reg, val)      (reg = ((reg & ~VIN_DEV_MF_SRGPIO_MF_GPIO7) | (((val) & 0x0001) << 7)))
#define VIN_DEV_MF_SRGPIO_MF_GPIO6                    (1 << 6)
#define VIN_DEV_MF_SRGPIO_MF_GPIO6_GET(reg)           ((reg & VIN_DEV_MF_SRGPIO_MF_GPIO6) >> 6)
#define VIN_DEV_MF_SRGPIO_MF_GPIO6_SET(reg, val)      (reg = ((reg & ~VIN_DEV_MF_SRGPIO_MF_GPIO6) | (((val) & 0x0001) << 6)))
#define VIN_DEV_MF_SRGPIO_MF_GPIO5                    (1 << 5)
#define VIN_DEV_MF_SRGPIO_MF_GPIO5_GET(reg)           ((reg & VIN_DEV_MF_SRGPIO_MF_GPIO5) >> 5)
#define VIN_DEV_MF_SRGPIO_MF_GPIO5_SET(reg, val)      (reg = ((reg & ~VIN_DEV_MF_SRGPIO_MF_GPIO5) | (((val) & 0x0001) << 5)))
#define VIN_DEV_MF_SRGPIO_MF_GPIO4                    (1 << 4)
#define VIN_DEV_MF_SRGPIO_MF_GPIO4_GET(reg)           ((reg & VIN_DEV_MF_SRGPIO_MF_GPIO4) >> 4)
#define VIN_DEV_MF_SRGPIO_MF_GPIO4_SET(reg, val)      (reg = ((reg & ~VIN_DEV_MF_SRGPIO_MF_GPIO4) | (((val) & 0x0001) << 4)))
#define VIN_DEV_MF_SRGPIO_MF_GPIO3                    (1 << 3)
#define VIN_DEV_MF_SRGPIO_MF_GPIO3_GET(reg)           ((reg & VIN_DEV_MF_SRGPIO_MF_GPIO3) >> 3)
#define VIN_DEV_MF_SRGPIO_MF_GPIO3_SET(reg, val)      (reg = ((reg & ~VIN_DEV_MF_SRGPIO_MF_GPIO3) | (((val) & 0x0001) << 3)))
#define VIN_DEV_MF_SRGPIO_MF_GPIO2                    (1 << 2)
#define VIN_DEV_MF_SRGPIO_MF_GPIO2_GET(reg)           ((reg & VIN_DEV_MF_SRGPIO_MF_GPIO2) >> 2)
#define VIN_DEV_MF_SRGPIO_MF_GPIO2_SET(reg, val)      (reg = ((reg & ~VIN_DEV_MF_SRGPIO_MF_GPIO2) | (((val) & 0x0001) << 2)))
#define VIN_DEV_MF_SRGPIO_MF_GPIO1                    (1 << 1)
#define VIN_DEV_MF_SRGPIO_MF_GPIO1_GET(reg)           ((reg & VIN_DEV_MF_SRGPIO_MF_GPIO1) >> 1)
#define VIN_DEV_MF_SRGPIO_MF_GPIO1_SET(reg, val)      (reg = ((reg & ~VIN_DEV_MF_SRGPIO_MF_GPIO1) | (((val) & 0x0001) << 1)))
#define VIN_DEV_MF_SRGPIO_MF_GPIO0                    (1 << 0)
#define VIN_DEV_MF_SRGPIO_MF_GPIO0_GET(reg)           ((reg & VIN_DEV_MF_SRGPIO_MF_GPIO0) >> 0)
#define VIN_DEV_MF_SRGPIO_MF_GPIO0_SET(reg, val)      (reg = ((reg & ~VIN_DEV_MF_SRGPIO_MF_GPIO0) | (((val) & 0x0001) << 0)))

/***GPIO ConfigurationRegister 1***/

#define VIN_DEV_GCR1                                  ( (VIN_DEV + 0x47) )

#define VIN_DEV_GCR1_HOST_GPIO7                       (1 << 15)
#define VIN_DEV_GCR1_HOST_GPIO7_GET(reg)              ((reg & VIN_DEV_GCR1_HOST_GPIO7) >> 15)
#define VIN_DEV_GCR1_HOST_GPIO7_SET(reg, val)         (reg = ((reg & ~VIN_DEV_GCR1_HOST_GPIO7) | (((val) & 0x0001) << 15)))
#define VIN_DEV_GCR1_HOST_GPIO6                       (1 << 14)
#define VIN_DEV_GCR1_HOST_GPIO6_GET(reg)              ((reg & VIN_DEV_GCR1_HOST_GPIO6) >> 14)
#define VIN_DEV_GCR1_HOST_GPIO6_SET(reg, val)         (reg = ((reg & ~VIN_DEV_GCR1_HOST_GPIO6) | (((val) & 0x0001) << 14)))
#define VIN_DEV_GCR1_HOST_GPIO5                       (1 << 13)
#define VIN_DEV_GCR1_HOST_GPIO5_GET(reg)              ((reg & VIN_DEV_GCR1_HOST_GPIO5) >> 13)
#define VIN_DEV_GCR1_HOST_GPIO5_SET(reg, val)         (reg = ((reg & ~VIN_DEV_GCR1_HOST_GPIO5) | (((val) & 0x0001) << 13)))
#define VIN_DEV_GCR1_HOST_GPIO4                       (1 << 12)
#define VIN_DEV_GCR1_HOST_GPIO4_GET(reg)              ((reg & VIN_DEV_GCR1_HOST_GPIO4) >> 12)
#define VIN_DEV_GCR1_HOST_GPIO4_SET(reg, val)         (reg = ((reg & ~VIN_DEV_GCR1_HOST_GPIO4) | (((val) & 0x0001) << 12)))
#define VIN_DEV_GCR1_HOST_GPIO3                       (1 << 11)
#define VIN_DEV_GCR1_HOST_GPIO3_GET(reg)              ((reg & VIN_DEV_GCR1_HOST_GPIO3) >> 11)
#define VIN_DEV_GCR1_HOST_GPIO3_SET(reg, val)         (reg = ((reg & ~VIN_DEV_GCR1_HOST_GPIO3) | (((val) & 0x0001) << 11)))
#define VIN_DEV_GCR1_HOST_GPIO2                       (1 << 10)
#define VIN_DEV_GCR1_HOST_GPIO2_GET(reg)              ((reg & VIN_DEV_GCR1_HOST_GPIO2) >> 10)
#define VIN_DEV_GCR1_HOST_GPIO2_SET(reg, val)         (reg = ((reg & ~VIN_DEV_GCR1_HOST_GPIO2) | (((val) & 0x0001) << 10)))
#define VIN_DEV_GCR1_HOST_GPIO1                       (1 << 9)
#define VIN_DEV_GCR1_HOST_GPIO1_GET(reg)              ((reg & VIN_DEV_GCR1_HOST_GPIO1) >> 9)
#define VIN_DEV_GCR1_HOST_GPIO1_SET(reg, val)         (reg = ((reg & ~VIN_DEV_GCR1_HOST_GPIO1) | (((val) & 0x0001) << 9)))
#define VIN_DEV_GCR1_HOST_GPIO0                       (1 << 8)
#define VIN_DEV_GCR1_HOST_GPIO0_GET(reg)              ((reg & VIN_DEV_GCR1_HOST_GPIO0) >> 8)
#define VIN_DEV_GCR1_HOST_GPIO0_SET(reg, val)         (reg = ((reg & ~VIN_DEV_GCR1_HOST_GPIO0) | (((val) & 0x0001) << 8)))
#define VIN_DEV_GCR1_DU_GPIO7                         (1 << 7)
#define VIN_DEV_GCR1_DU_GPIO7_GET(reg)                ((reg & VIN_DEV_GCR1_DU_GPIO7) >> 7)
#define VIN_DEV_GCR1_DU_GPIO7_SET(reg, val)           (reg = ((reg & ~VIN_DEV_GCR1_DU_GPIO7) | (((val) & 0x0001) << 7)))
#define VIN_DEV_GCR1_DU_GPIO6                         (1 << 6)
#define VIN_DEV_GCR1_DU_GPIO6_GET(reg)                ((reg & VIN_DEV_GCR1_DU_GPIO6) >> 6)
#define VIN_DEV_GCR1_DU_GPIO6_SET(reg, val)           (reg = ((reg & ~VIN_DEV_GCR1_DU_GPIO6) | (((val) & 0x0001) << 6)))
#define VIN_DEV_GCR1_DU_GPIO5                         (1 << 5)
#define VIN_DEV_GCR1_DU_GPIO5_GET(reg)                ((reg & VIN_DEV_GCR1_DU_GPIO5) >> 5)
#define VIN_DEV_GCR1_DU_GPIO5_SET(reg, val)           (reg = ((reg & ~VIN_DEV_GCR1_DU_GPIO5) | (((val) & 0x0001) << 5)))
#define VIN_DEV_GCR1_DU_GPIO4                         (1 << 4)
#define VIN_DEV_GCR1_DU_GPIO4_GET(reg)                ((reg & VIN_DEV_GCR1_DU_GPIO4) >> 4)
#define VIN_DEV_GCR1_DU_GPIO4_SET(reg, val)           (reg = ((reg & ~VIN_DEV_GCR1_DU_GPIO4) | (((val) & 0x0001) << 4)))
#define VIN_DEV_GCR1_DU_GPIO3                         (1 << 3)
#define VIN_DEV_GCR1_DU_GPIO3_GET(reg)                ((reg & VIN_DEV_GCR1_DU_GPIO3) >> 3)
#define VIN_DEV_GCR1_DU_GPIO3_SET(reg, val)           (reg = ((reg & ~VIN_DEV_GCR1_DU_GPIO3) | (((val) & 0x0001) << 3)))
#define VIN_DEV_GCR1_DU_GPIO2                         (1 << 2)
#define VIN_DEV_GCR1_DU_GPIO2_GET(reg)                ((reg & VIN_DEV_GCR1_DU_GPIO2) >> 2)
#define VIN_DEV_GCR1_DU_GPIO2_SET(reg, val)           (reg = ((reg & ~VIN_DEV_GCR1_DU_GPIO2) | (((val) & 0x0001) << 2)))
#define VIN_DEV_GCR1_DU_GPIO1                         (1 << 1)
#define VIN_DEV_GCR1_DU_GPIO1_GET(reg)                ((reg & VIN_DEV_GCR1_DU_GPIO1) >> 1)
#define VIN_DEV_GCR1_DU_GPIO1_SET(reg, val)           (reg = ((reg & ~VIN_DEV_GCR1_DU_GPIO1) | (((val) & 0x0001) << 1)))
#define VIN_DEV_GCR1_DU_GPIO0                         (1 << 0)
#define VIN_DEV_GCR1_DU_GPIO0_GET(reg)                ((reg & VIN_DEV_GCR1_DU_GPIO0) >> 0)
#define VIN_DEV_GCR1_DU_GPIO0_SET(reg, val)           (reg = ((reg & ~VIN_DEV_GCR1_DU_GPIO0) | (((val) & 0x0001) << 0)))

/***GPIO Configuration Register 2***/

#define VIN_DEV_GCR2                                  ( (VIN_DEV + 0x48) )

#define VIN_DEV_GCR2_IEN_GPIO7                        (1 << 15)
#define VIN_DEV_GCR2_IEN_GPIO7_GET(reg)               ((reg & VIN_DEV_GCR2_IEN_GPIO7) >> 15)
#define VIN_DEV_GCR2_IEN_GPIO7_SET(reg, val)          (reg = ((reg & ~VIN_DEV_GCR2_IEN_GPIO7) | (((val) & 0x0001) << 15)))
#define VIN_DEV_GCR2_IEN_GPIO6                        (1 << 14)
#define VIN_DEV_GCR2_IEN_GPIO6_GET(reg)               ((reg & VIN_DEV_GCR2_IEN_GPIO6) >> 14)
#define VIN_DEV_GCR2_IEN_GPIO6_SET(reg, val)          (reg = ((reg & ~VIN_DEV_GCR2_IEN_GPIO6) | (((val) & 0x0001) << 14)))
#define VIN_DEV_GCR2_IEN_GPIO5                        (1 << 13)
#define VIN_DEV_GCR2_IEN_GPIO5_GET(reg)               ((reg & VIN_DEV_GCR2_IEN_GPIO5) >> 13)
#define VIN_DEV_GCR2_IEN_GPIO5_SET(reg, val)          (reg = ((reg & ~VIN_DEV_GCR2_IEN_GPIO5) | (((val) & 0x0001) << 13)))
#define VIN_DEV_GCR2_IEN_GPIO4                        (1 << 12)
#define VIN_DEV_GCR2_IEN_GPIO4_GET(reg)               ((reg & VIN_DEV_GCR2_IEN_GPIO4) >> 12)
#define VIN_DEV_GCR2_IEN_GPIO4_SET(reg, val)          (reg = ((reg & ~VIN_DEV_GCR2_IEN_GPIO4) | (((val) & 0x0001) << 12)))
#define VIN_DEV_GCR2_IEN_GPIO3                        (1 << 11)
#define VIN_DEV_GCR2_IEN_GPIO3_GET(reg)               ((reg & VIN_DEV_GCR2_IEN_GPIO3) >> 11)
#define VIN_DEV_GCR2_IEN_GPIO3_SET(reg, val)          (reg = ((reg & ~VIN_DEV_GCR2_IEN_GPIO3) | (((val) & 0x0001) << 11)))
#define VIN_DEV_GCR2_IEN_GPIO2                        (1 << 10)
#define VIN_DEV_GCR2_IEN_GPIO2_GET(reg)               ((reg & VIN_DEV_GCR2_IEN_GPIO2) >> 10)
#define VIN_DEV_GCR2_IEN_GPIO2_SET(reg, val)          (reg = ((reg & ~VIN_DEV_GCR2_IEN_GPIO2) | (((val) & 0x0001) << 10)))
#define VIN_DEV_GCR2_IEN_GPIO1                        (1 << 9)
#define VIN_DEV_GCR2_IEN_GPIO1_GET(reg)               ((reg & VIN_DEV_GCR2_IEN_GPIO1) >> 9)
#define VIN_DEV_GCR2_IEN_GPIO1_SET(reg, val)          (reg = ((reg & ~VIN_DEV_GCR2_IEN_GPIO1) | (((val) & 0x0001) << 9)))
#define VIN_DEV_GCR2_IEN_GPIO0                        (1 << 8)
#define VIN_DEV_GCR2_IEN_GPIO0_GET(reg)               ((reg & VIN_DEV_GCR2_IEN_GPIO0) >> 8)
#define VIN_DEV_GCR2_IEN_GPIO0_SET(reg, val)          (reg = ((reg & ~VIN_DEV_GCR2_IEN_GPIO0) | (((val) & 0x0001) << 8)))
#define VIN_DEV_GCR2_OEN_GPIO7                        (1 << 7)
#define VIN_DEV_GCR2_OEN_GPIO7_GET(reg)               ((reg & VIN_DEV_GCR2_OEN_GPIO7) >> 7)
#define VIN_DEV_GCR2_OEN_GPIO7_SET(reg, val)          (reg = ((reg & ~VIN_DEV_GCR2_OEN_GPIO7) | (((val) & 0x0001) << 7)))
#define VIN_DEV_GCR2_OEN_GPIO6                        (1 << 6)
#define VIN_DEV_GCR2_OEN_GPIO6_GET(reg)               ((reg & VIN_DEV_GCR2_OEN_GPIO6) >> 6)
#define VIN_DEV_GCR2_OEN_GPIO6_SET(reg, val)          (reg = ((reg & ~VIN_DEV_GCR2_OEN_GPIO6) | (((val) & 0x0001) << 6)))
#define VIN_DEV_GCR2_OEN_GPIO5                        (1 << 5)
#define VIN_DEV_GCR2_OEN_GPIO5_GET(reg)               ((reg & VIN_DEV_GCR2_OEN_GPIO5) >> 5)
#define VIN_DEV_GCR2_OEN_GPIO5_SET(reg, val)          (reg = ((reg & ~VIN_DEV_GCR2_OEN_GPIO5) | (((val) & 0x0001) << 5)))
#define VIN_DEV_GCR2_OEN_GPIO4                        (1 << 4)
#define VIN_DEV_GCR2_OEN_GPIO4_GET(reg)               ((reg & VIN_DEV_GCR2_OEN_GPIO4) >> 4)
#define VIN_DEV_GCR2_OEN_GPIO4_SET(reg, val)          (reg = ((reg & ~VIN_DEV_GCR2_OEN_GPIO4) | (((val) & 0x0001) << 4)))
#define VIN_DEV_GCR2_OEN_GPIO3                        (1 << 3)
#define VIN_DEV_GCR2_OEN_GPIO3_GET(reg)               ((reg & VIN_DEV_GCR2_OEN_GPIO3) >> 3)
#define VIN_DEV_GCR2_OEN_GPIO3_SET(reg, val)          (reg = ((reg & ~VIN_DEV_GCR2_OEN_GPIO3) | (((val) & 0x0001) << 3)))
#define VIN_DEV_GCR2_OEN_GPIO2                        (1 << 2)
#define VIN_DEV_GCR2_OEN_GPIO2_GET(reg)               ((reg & VIN_DEV_GCR2_OEN_GPIO2) >> 2)
#define VIN_DEV_GCR2_OEN_GPIO2_SET(reg, val)          (reg = ((reg & ~VIN_DEV_GCR2_OEN_GPIO2) | (((val) & 0x0001) << 2)))
#define VIN_DEV_GCR2_OEN_GPIO1                        (1 << 1)
#define VIN_DEV_GCR2_OEN_GPIO1_GET(reg)               ((reg & VIN_DEV_GCR2_OEN_GPIO1) >> 1)
#define VIN_DEV_GCR2_OEN_GPIO1_SET(reg, val)          (reg = ((reg & ~VIN_DEV_GCR2_OEN_GPIO1) | (((val) & 0x0001) << 1)))
#define VIN_DEV_GCR2_OEN_GPIO0                        (1 << 0)
#define VIN_DEV_GCR2_OEN_GPIO0_GET(reg)               ((reg & VIN_DEV_GCR2_OEN_GPIO0) >> 0)
#define VIN_DEV_GCR2_OEN_GPIO0_SET(reg, val)          (reg = ((reg & ~VIN_DEV_GCR2_OEN_GPIO0) | (((val) & 0x0001) << 0)))

/***PHI PRAM Checksum Register***/

#define VIN_DEV_PHICHKR                               ( (VIN_DEV + 0x49) )

#define VIN_DEV_PHICHKR_PRAM_CHECK

/***Revision Number(read-only)***/

#define VIN_DEV_REVISION                              ( (VIN_DEV + 0x40) )

#define VIN_DEV_REVISION_Type_MASK                    (((1 << 4) - 1) << 12)
#define VIN_DEV_REVISION_Type_GET(reg)                (((reg) >> 12) & ((1 << 4) - 1))
#define VIN_DEV_REVISION_Type_SET(val)                ((((1 << 4) - 1) & (val)) << 12)
#define VIN_DEV_REVISION_Type_MODIFY(reg, val)        (reg = ( (reg & ~VIN_DEV_REVISION_Type_MASK) | ((((1 << 4) - 1) & (val)) << 12) ))
#define VIN_DEV_REVISION_ANACHAN_MASK                 (((1 << 4) - 1) << 8)
#define VIN_DEV_REVISION_ANACHAN_GET(reg)             (((reg) >> 8) & ((1 << 4) - 1))
#define VIN_DEV_REVISION_ANACHAN_SET(val)             ((((1 << 4) - 1) & (val)) << 8)
#define VIN_DEV_REVISION_ANACHAN_MODIFY(reg, val)     (reg = ( (reg & ~VIN_DEV_REVISION_ANACHAN_MASK) | ((((1 << 4) - 1) & (val)) << 8) ))
#define VIN_DEV_REVISION_REV_MASK                     (((1 << 8) - 1) << 0)
#define VIN_DEV_REVISION_REV_GET(reg)                 (((reg) >> 0) & ((1 << 8) - 1))
#define VIN_DEV_REVISION_REV_SET(val)                 ((((1 << 8) - 1) & (val)) << 0)
#define VIN_DEV_REVISION_REV_MODIFY(reg, val)         (reg = ( (reg & ~VIN_DEV_REVISION_REV_MASK) | ((((1 << 8) - 1) & (val)) << 0) ))

/***Chip Identification 1(read-only)***/

#define VIN_DEV_CHIPID1                               ( (VIN_DEV + 0x41) )

#define VIN_DEV_CHIPID1_RES15                         (1 << 15)
#define VIN_DEV_CHIPID1_RES15_GET(reg)                ((reg & VIN_DEV_CHIPID1_RES15) >> 15)
#define VIN_DEV_CHIPID1_RES15_SET(reg, val)           (reg = ((reg & ~VIN_DEV_CHIPID1_RES15) | (((val) & 0x0001) << 15)))
#define VIN_DEV_CHIPID1_CHIP_Y_MASK                   (((1 << 7) - 1) << 8)
#define VIN_DEV_CHIPID1_CHIP_Y_GET(reg)               (((reg) >> 8) & ((1 << 7) - 1))
#define VIN_DEV_CHIPID1_CHIP_Y_SET(val)               ((((1 << 7) - 1) & (val)) << 8)
#define VIN_DEV_CHIPID1_CHIP_Y_MODIFY(reg, val)       (reg = ( (reg & ~VIN_DEV_CHIPID1_CHIP_Y_MASK) | ((((1 << 7) - 1) & (val)) << 8) ))
#define VIN_DEV_CHIPID1_RES7                          (1 << 7)
#define VIN_DEV_CHIPID1_RES7_GET(reg)                 ((reg & VIN_DEV_CHIPID1_RES7) >> 7)
#define VIN_DEV_CHIPID1_RES7_SET(reg, val)            (reg = ((reg & ~VIN_DEV_CHIPID1_RES7) | (((val) & 0x0001) << 7)))
#define VIN_DEV_CHIPID1_CHIP_X_MASK                   (((1 << 7) - 1) << 0)
#define VIN_DEV_CHIPID1_CHIP_X_GET(reg)               (((reg) >> 0) & ((1 << 7) - 1))
#define VIN_DEV_CHIPID1_CHIP_X_SET(val)               ((((1 << 7) - 1) & (val)) << 0)
#define VIN_DEV_CHIPID1_CHIP_X_MODIFY(reg, val)       (reg = ( (reg & ~VIN_DEV_CHIPID1_CHIP_X_MASK) | ((((1 << 7) - 1) & (val)) << 0) ))

/***Chip Identification 2(read-only)***/

#define VIN_DEV_CHIPID2                               ( (VIN_DEV + 0x42) )

#define VIN_DEV_CHIPID2_WAF_NR_MASK                   (((1 << 6) - 1) << 10)
#define VIN_DEV_CHIPID2_WAF_NR_GET(reg)               (((reg) >> 10) & ((1 << 6) - 1))
#define VIN_DEV_CHIPID2_WAF_NR_SET(val)               ((((1 << 6) - 1) & (val)) << 10)
#define VIN_DEV_CHIPID2_WAF_NR_MODIFY(reg, val)       (reg = ( (reg & ~VIN_DEV_CHIPID2_WAF_NR_MASK) | ((((1 << 6) - 1) & (val)) << 10) ))
#define VIN_DEV_CHIPID2_LOT_NR_MASK                   (((1 << 10) - 1) << 0)
#define VIN_DEV_CHIPID2_LOT_NR_GET(reg)               (((reg) >> 0) & ((1 << 10) - 1))
#define VIN_DEV_CHIPID2_LOT_NR_SET(val)               ((((1 << 10) - 1) & (val)) << 0)
#define VIN_DEV_CHIPID2_LOT_NR_MODIFY(reg, val)       (reg = ( (reg & ~VIN_DEV_CHIPID2_LOT_NR_MASK) | ((((1 << 10) - 1) & (val)) << 0) ))

/***Chip Identification 3(read-only)***/

#define VIN_DEV_CHIPID3                               ( (VIN_DEV + 0x43) )

#define VIN_DEV_CHIPID3_LOT_NR

/***Global Configuration Register***/

#define VIN_DEV_GCONF                                 ( (VIN_DEV + 0x44) )

#define VIN_DEV_GCONF_RES15                           (1 << 15)
#define VIN_DEV_GCONF_RES15_GET(reg)                  ((reg & VIN_DEV_GCONF_RES15) >> 15)
#define VIN_DEV_GCONF_RES15_SET(reg, val)             (reg = ((reg & ~VIN_DEV_GCONF_RES15) | (((val) & 0x0001) << 15)))
#define VIN_DEV_GCONF_RES14                           (1 << 14)
#define VIN_DEV_GCONF_RES14_GET(reg)                  ((reg & VIN_DEV_GCONF_RES14) >> 14)
#define VIN_DEV_GCONF_RES14_SET(reg, val)             (reg = ((reg & ~VIN_DEV_GCONF_RES14) | (((val) & 0x0001) << 14)))
#define VIN_DEV_GCONF_RES13                           (1 << 13)
#define VIN_DEV_GCONF_RES13_GET(reg)                  ((reg & VIN_DEV_GCONF_RES13) >> 13)
#define VIN_DEV_GCONF_RES13_SET(reg, val)             (reg = ((reg & ~VIN_DEV_GCONF_RES13) | (((val) & 0x0001) << 13)))
#define VIN_DEV_GCONF_RES12                           (1 << 12)
#define VIN_DEV_GCONF_RES12_GET(reg)                  ((reg & VIN_DEV_GCONF_RES12) >> 12)
#define VIN_DEV_GCONF_RES12_SET(reg, val)             (reg = ((reg & ~VIN_DEV_GCONF_RES12) | (((val) & 0x0001) << 12)))
#define VIN_DEV_GCONF_RES11                           (1 << 11)
#define VIN_DEV_GCONF_RES11_GET(reg)                  ((reg & VIN_DEV_GCONF_RES11) >> 11)
#define VIN_DEV_GCONF_RES11_SET(reg, val)             (reg = ((reg & ~VIN_DEV_GCONF_RES11) | (((val) & 0x0001) << 11)))
#define VIN_DEV_GCONF_RES10                           (1 << 10)
#define VIN_DEV_GCONF_RES10_GET(reg)                  ((reg & VIN_DEV_GCONF_RES10) >> 10)
#define VIN_DEV_GCONF_RES10_SET(reg, val)             (reg = ((reg & ~VIN_DEV_GCONF_RES10) | (((val) & 0x0001) << 10)))
#define VIN_DEV_GCONF_RES9                            (1 << 9)
#define VIN_DEV_GCONF_RES9_GET(reg)                   ((reg & VIN_DEV_GCONF_RES9) >> 9)
#define VIN_DEV_GCONF_RES9_SET(reg, val)              (reg = ((reg & ~VIN_DEV_GCONF_RES9) | (((val) & 0x0001) << 9)))
#define VIN_DEV_GCONF_RES8                            (1 << 8)
#define VIN_DEV_GCONF_RES8_GET(reg)                   ((reg & VIN_DEV_GCONF_RES8) >> 8)
#define VIN_DEV_GCONF_RES8_SET(reg, val)              (reg = ((reg & ~VIN_DEV_GCONF_RES8) | (((val) & 0x0001) << 8)))
#define VIN_DEV_GCONF_RES7                            (1 << 7)
#define VIN_DEV_GCONF_RES7_GET(reg)                   ((reg & VIN_DEV_GCONF_RES7) >> 7)
#define VIN_DEV_GCONF_RES7_SET(reg, val)              (reg = ((reg & ~VIN_DEV_GCONF_RES7) | (((val) & 0x0001) << 7)))
#define VIN_DEV_GCONF_RES6                            (1 << 6)
#define VIN_DEV_GCONF_RES6_GET(reg)                   ((reg & VIN_DEV_GCONF_RES6) >> 6)
#define VIN_DEV_GCONF_RES6_SET(reg, val)              (reg = ((reg & ~VIN_DEV_GCONF_RES6) | (((val) & 0x0001) << 6)))
#define VIN_DEV_GCONF_RES5                            (1 << 5)
#define VIN_DEV_GCONF_RES5_GET(reg)                   ((reg & VIN_DEV_GCONF_RES5) >> 5)
#define VIN_DEV_GCONF_RES5_SET(reg, val)              (reg = ((reg & ~VIN_DEV_GCONF_RES5) | (((val) & 0x0001) << 5)))
#define VIN_DEV_GCONF_RES4                            (1 << 4)
#define VIN_DEV_GCONF_RES4_GET(reg)                   ((reg & VIN_DEV_GCONF_RES4) >> 4)
#define VIN_DEV_GCONF_RES4_SET(reg, val)              (reg = ((reg & ~VIN_DEV_GCONF_RES4) | (((val) & 0x0001) << 4)))
#define VIN_DEV_GCONF_RES3                            (1 << 3)
#define VIN_DEV_GCONF_RES3_GET(reg)                   ((reg & VIN_DEV_GCONF_RES3) >> 3)
#define VIN_DEV_GCONF_RES3_SET(reg, val)              (reg = ((reg & ~VIN_DEV_GCONF_RES3) | (((val) & 0x0001) << 3)))
#define VIN_DEV_GCONF_PP_EN_RDYQ                      (1 << 2)
#define VIN_DEV_GCONF_PP_EN_RDYQ_GET(reg)             ((reg & VIN_DEV_GCONF_PP_EN_RDYQ) >> 2)
#define VIN_DEV_GCONF_PP_EN_RDYQ_SET(reg, val)        (reg = ((reg & ~VIN_DEV_GCONF_PP_EN_RDYQ) | (((val) & 0x0001) << 2)))
#define VIN_DEV_GCONF_WDG_EN                          (1 << 1)
#define VIN_DEV_GCONF_WDG_EN_GET(reg)                 ((reg & VIN_DEV_GCONF_WDG_EN) >> 1)
#define VIN_DEV_GCONF_WDG_EN_SET(reg, val)            (reg = ((reg & ~VIN_DEV_GCONF_WDG_EN) | (((val) & 0x0001) << 1)))
#define VIN_DEV_GCONF_PRG_FUSE                        (1 << 0)
#define VIN_DEV_GCONF_PRG_FUSE_GET(reg)               ((reg & VIN_DEV_GCONF_PRG_FUSE) >> 0)
#define VIN_DEV_GCONF_PRG_FUSE_SET(reg, val)          (reg = ((reg & ~VIN_DEV_GCONF_PRG_FUSE) | (((val) & 0x0001) << 0)))

/***PHI Miscellaneous Command Register***/

#define VIN_DEV_MISSCMD                               ( (VIN_DEV + 0x4a) )

#define VIN_DEV_MISSCMD_CHK                           (1 << 0)
#define VIN_DEV_MISSCMD_CHK_GET(reg)                  ((reg & VIN_DEV_MISSCMD_CHK) >> 0)
#define VIN_DEV_MISSCMD_CHK_SET(reg, val)             (reg = ((reg & ~VIN_DEV_MISSCMD_CHK) | (((val) & 0x0001) << 0)))


#define VIN_DEV_ACTRL      (VIN_DEV + 0x004E)
#define VIN_DEV_ACTRL_DR   (1 << 15)
#define VIN_DEV_ACTRL_UR   (1 << 14)
#define VIN_DEV_ABOOT      (VIN_DEV + 0x004F)
#define VIN_DEV_SYSCTRL    (VIN_DEV + 0x0055)


/***********************************************************************/
/*  Module      :  MOD register address and bits                       */
/***********************************************************************/

#define VIN_MOD                                       (0)

/***********************************************************************/


/******/

#define VIN_MOD_DCCHKR                                ( (VIN_MOD + 0x00) )

#define VIN_MOD_DCCHKR_DC_CHECK

/******/

#define VIN_MOD_DSCHKR                                ( (VIN_MOD + 0x01) )

#define VIN_MOD_DSCHKR_DS_CHECK

/******/

#define VIN_MOD_CCR                                   ( (VIN_MOD + 0x02) )

#define VIN_MOD_CCR_RES15                             (1 << 15)
#define VIN_MOD_CCR_RES15_GET(reg)                    ((reg & VIN_MOD_CCR_RES15) >> 15)
#define VIN_MOD_CCR_RES15_SET(reg, val)               (reg = ((reg & ~VIN_MOD_CCR_RES15) | (((val) & 0x0001) << 15)))
#define VIN_MOD_CCR_RES14                             (1 << 14)
#define VIN_MOD_CCR_RES14_GET(reg)                    ((reg & VIN_MOD_CCR_RES14) >> 14)
#define VIN_MOD_CCR_RES14_SET(reg, val)               (reg = ((reg & ~VIN_MOD_CCR_RES14) | (((val) & 0x0001) << 14)))
#define VIN_MOD_CCR_RES13                             (1 << 13)
#define VIN_MOD_CCR_RES13_GET(reg)                    ((reg & VIN_MOD_CCR_RES13) >> 13)
#define VIN_MOD_CCR_RES13_SET(reg, val)               (reg = ((reg & ~VIN_MOD_CCR_RES13) | (((val) & 0x0001) << 13)))
#define VIN_MOD_CCR_RES12                             (1 << 12)
#define VIN_MOD_CCR_RES12_GET(reg)                    ((reg & VIN_MOD_CCR_RES12) >> 12)
#define VIN_MOD_CCR_RES12_SET(reg, val)               (reg = ((reg & ~VIN_MOD_CCR_RES12) | (((val) & 0x0001) << 12)))
#define VIN_MOD_CCR_RES11                             (1 << 11)
#define VIN_MOD_CCR_RES11_GET(reg)                    ((reg & VIN_MOD_CCR_RES11) >> 11)
#define VIN_MOD_CCR_RES11_SET(reg, val)               (reg = ((reg & ~VIN_MOD_CCR_RES11) | (((val) & 0x0001) << 11)))
#define VIN_MOD_CCR_RES10                             (1 << 10)
#define VIN_MOD_CCR_RES10_GET(reg)                    ((reg & VIN_MOD_CCR_RES10) >> 10)
#define VIN_MOD_CCR_RES10_SET(reg, val)               (reg = ((reg & ~VIN_MOD_CCR_RES10) | (((val) & 0x0001) << 10)))
#define VIN_MOD_CCR_RES9                              (1 << 9)
#define VIN_MOD_CCR_RES9_GET(reg)                     ((reg & VIN_MOD_CCR_RES9) >> 9)
#define VIN_MOD_CCR_RES9_SET(reg, val)                (reg = ((reg & ~VIN_MOD_CCR_RES9) | (((val) & 0x0001) << 9)))
#define VIN_MOD_CCR_RES8                              (1 << 8)
#define VIN_MOD_CCR_RES8_GET(reg)                     ((reg & VIN_MOD_CCR_RES8) >> 8)
#define VIN_MOD_CCR_RES8_SET(reg, val)                (reg = ((reg & ~VIN_MOD_CCR_RES8) | (((val) & 0x0001) << 8)))
#define VIN_MOD_CCR_RES7                              (1 << 7)
#define VIN_MOD_CCR_RES7_GET(reg)                     ((reg & VIN_MOD_CCR_RES7) >> 7)
#define VIN_MOD_CCR_RES7_SET(reg, val)                (reg = ((reg & ~VIN_MOD_CCR_RES7) | (((val) & 0x0001) << 7)))
#define VIN_MOD_CCR_RES6                              (1 << 6)
#define VIN_MOD_CCR_RES6_GET(reg)                     ((reg & VIN_MOD_CCR_RES6) >> 6)
#define VIN_MOD_CCR_RES6_SET(reg, val)                (reg = ((reg & ~VIN_MOD_CCR_RES6) | (((val) & 0x0001) << 6)))
#define VIN_MOD_CCR_JUMP_DC                           (1 << 5)
#define VIN_MOD_CCR_JUMP_DC_GET(reg)                  ((reg & VIN_MOD_CCR_JUMP_DC) >> 5)
#define VIN_MOD_CCR_JUMP_DC_SET(reg, val)             (reg = ((reg & ~VIN_MOD_CCR_JUMP_DC) | (((val) & 0x0001) << 5)))
#define VIN_MOD_CCR_JUMP_AC3                          (1 << 4)
#define VIN_MOD_CCR_JUMP_AC3_GET(reg)                 ((reg & VIN_MOD_CCR_JUMP_AC3) >> 4)
#define VIN_MOD_CCR_JUMP_AC3_SET(reg, val)            (reg = ((reg & ~VIN_MOD_CCR_JUMP_AC3) | (((val) & 0x0001) << 4)))
#define VIN_MOD_CCR_JUMP_AC2                          (1 << 3)
#define VIN_MOD_CCR_JUMP_AC2_GET(reg)                 ((reg & VIN_MOD_CCR_JUMP_AC2) >> 3)
#define VIN_MOD_CCR_JUMP_AC2_SET(reg, val)            (reg = ((reg & ~VIN_MOD_CCR_JUMP_AC2) | (((val) & 0x0001) << 3)))
#define VIN_MOD_CCR_JUMP_AC1                          (1 << 2)
#define VIN_MOD_CCR_JUMP_AC1_GET(reg)                 ((reg & VIN_MOD_CCR_JUMP_AC1) >> 2)
#define VIN_MOD_CCR_JUMP_AC1_SET(reg, val)            (reg = ((reg & ~VIN_MOD_CCR_JUMP_AC1) | (((val) & 0x0001) << 2)))
#define VIN_MOD_CCR_PD_CVCM                           (1 << 1)
#define VIN_MOD_CCR_PD_CVCM_GET(reg)                  ((reg & VIN_MOD_CCR_PD_CVCM) >> 1)
#define VIN_MOD_CCR_PD_CVCM_SET(reg, val)             (reg = ((reg & ~VIN_MOD_CCR_PD_CVCM) | (((val) & 0x0001) << 1)))
#define VIN_MOD_CCR_PD_CBIAS                          (1 << 0)
#define VIN_MOD_CCR_PD_CBIAS_GET(reg)                 ((reg & VIN_MOD_CCR_PD_CBIAS) >> 0)
#define VIN_MOD_CCR_PD_CBIAS_SET(reg, val)            (reg = ((reg & ~VIN_MOD_CCR_PD_CBIAS) | (((val) & 0x0001) << 0)))

/***********************************************************************/
/*  Module      :  RES register address and bits                       */
/***********************************************************************/

#define VIN_RES                                       (0)

/***********************************************************************/


/***Mask Register for Status Register for EDSP Interrupts
 1***/

#define VIN_RES_MR_SRE1                               ( (VIN_RES + 0x00) )

#define VIN_RES_MR_SRE1_M_DTMFR_DT                    (1 << 15)
#define VIN_RES_MR_SRE1_M_DTMFR_DT_GET(reg)           ((reg & VIN_RES_MR_SRE1_M_DTMFR_DT) >> 15)
#define VIN_RES_MR_SRE1_M_DTMFR_DT_SET(reg, val)      (reg = ((reg & ~VIN_RES_MR_SRE1_M_DTMFR_DT) | (((val) & 0x0001) << 15)))
#define VIN_RES_MR_SRE1_M_DTMFR_PDT                   (1 << 14)
#define VIN_RES_MR_SRE1_M_DTMFR_PDT_GET(reg)          ((reg & VIN_RES_MR_SRE1_M_DTMFR_PDT) >> 14)
#define VIN_RES_MR_SRE1_M_DTMFR_PDT_SET(reg, val)     (reg = ((reg & ~VIN_RES_MR_SRE1_M_DTMFR_PDT) | (((val) & 0x0001) << 14)))
#define VIN_RES_MR_SRE1_M_DTMFR_DTC_MASK              (((1 << 4) - 1) << 10)
#define VIN_RES_MR_SRE1_M_DTMFR_DTC_GET(reg)          (((reg) >> 10) & ((1 << 4) - 1))
#define VIN_RES_MR_SRE1_M_DTMFR_DTC_SET(val)          ((((1 << 4) - 1) & (val)) << 10)
#define VIN_RES_MR_SRE1_M_DTMFR_DTC_MODIFY(reg, val)  (reg = ( (reg & ~VIN_RES_MR_SRE1_M_DTMFR_DTC_MASK) | ((((1 << 4) - 1) & (val)) << 10) ))
#define VIN_RES_MR_SRE1_M_UTD1_OK                     (1 << 9)
#define VIN_RES_MR_SRE1_M_UTD1_OK_GET(reg)            ((reg & VIN_RES_MR_SRE1_M_UTD1_OK) >> 9)
#define VIN_RES_MR_SRE1_M_UTD1_OK_SET(reg, val)       (reg = ((reg & ~VIN_RES_MR_SRE1_M_UTD1_OK) | (((val) & 0x0001) << 9)))
#define VIN_RES_MR_SRE1_M_UTD2_OK                     (1 << 8)
#define VIN_RES_MR_SRE1_M_UTD2_OK_GET(reg)            ((reg & VIN_RES_MR_SRE1_M_UTD2_OK) >> 8)
#define VIN_RES_MR_SRE1_M_UTD2_OK_SET(reg, val)       (reg = ((reg & ~VIN_RES_MR_SRE1_M_UTD2_OK) | (((val) & 0x0001) << 8)))
#define VIN_RES_MR_SRE1_RES7                          (1 << 7)
#define VIN_RES_MR_SRE1_RES7_GET(reg)                 ((reg & VIN_RES_MR_SRE1_RES7) >> 7)
#define VIN_RES_MR_SRE1_RES7_SET(reg, val)            (reg = ((reg & ~VIN_RES_MR_SRE1_RES7) | (((val) & 0x0001) << 7)))
#define VIN_RES_MR_SRE1_RES6                          (1 << 6)
#define VIN_RES_MR_SRE1_RES6_GET(reg)                 ((reg & VIN_RES_MR_SRE1_RES6) >> 6)
#define VIN_RES_MR_SRE1_RES6_SET(reg, val)            (reg = ((reg & ~VIN_RES_MR_SRE1_RES6) | (((val) & 0x0001) << 6)))
#define VIN_RES_MR_SRE1_M_DTMFG_BUF                   (1 << 5)
#define VIN_RES_MR_SRE1_M_DTMFG_BUF_GET(reg)          ((reg & VIN_RES_MR_SRE1_M_DTMFG_BUF) >> 5)
#define VIN_RES_MR_SRE1_M_DTMFG_BUF_SET(reg, val)     (reg = ((reg & ~VIN_RES_MR_SRE1_M_DTMFG_BUF) | (((val) & 0x0001) << 5)))
#define VIN_RES_MR_SRE1_M_DTMFG_REQ                   (1 << 4)
#define VIN_RES_MR_SRE1_M_DTMFG_REQ_GET(reg)          ((reg & VIN_RES_MR_SRE1_M_DTMFG_REQ) >> 4)
#define VIN_RES_MR_SRE1_M_DTMFG_REQ_SET(reg, val)     (reg = ((reg & ~VIN_RES_MR_SRE1_M_DTMFG_REQ) | (((val) & 0x0001) << 4)))
#define VIN_RES_MR_SRE1_M_DTMFG_ACT                   (1 << 3)
#define VIN_RES_MR_SRE1_M_DTMFG_ACT_GET(reg)          ((reg & VIN_RES_MR_SRE1_M_DTMFG_ACT) >> 3)
#define VIN_RES_MR_SRE1_M_DTMFG_ACT_SET(reg, val)     (reg = ((reg & ~VIN_RES_MR_SRE1_M_DTMFG_ACT) | (((val) & 0x0001) << 3)))
#define VIN_RES_MR_SRE1_M_CIS_BUF                     (1 << 2)
#define VIN_RES_MR_SRE1_M_CIS_BUF_GET(reg)            ((reg & VIN_RES_MR_SRE1_M_CIS_BUF) >> 2)
#define VIN_RES_MR_SRE1_M_CIS_BUF_SET(reg, val)       (reg = ((reg & ~VIN_RES_MR_SRE1_M_CIS_BUF) | (((val) & 0x0001) << 2)))
#define VIN_RES_MR_SRE1_M_CIS_REQ                     (1 << 1)
#define VIN_RES_MR_SRE1_M_CIS_REQ_GET(reg)            ((reg & VIN_RES_MR_SRE1_M_CIS_REQ) >> 1)
#define VIN_RES_MR_SRE1_M_CIS_REQ_SET(reg, val)       (reg = ((reg & ~VIN_RES_MR_SRE1_M_CIS_REQ) | (((val) & 0x0001) << 1)))
#define VIN_RES_MR_SRE1_M_CIS_ACT                     (1 << 0)
#define VIN_RES_MR_SRE1_M_CIS_ACT_GET(reg)            ((reg & VIN_RES_MR_SRE1_M_CIS_ACT) >> 0)
#define VIN_RES_MR_SRE1_M_CIS_ACT_SET(reg, val)       (reg = ((reg & ~VIN_RES_MR_SRE1_M_CIS_ACT) | (((val) & 0x0001) << 0)))

/***Mask Register for Status Register for EDSP Interrupts
 2***/

#define VIN_RES_MR_SRE2                               ( (VIN_RES + 0x01) )

#define VIN_RES_MR_SRE2_M_ATD1_DT                     (1 << 15)
#define VIN_RES_MR_SRE2_M_ATD1_DT_GET(reg)            ((reg & VIN_RES_MR_SRE2_M_ATD1_DT) >> 15)
#define VIN_RES_MR_SRE2_M_ATD1_DT_SET(reg, val)       (reg = ((reg & ~VIN_RES_MR_SRE2_M_ATD1_DT) | (((val) & 0x0001) << 15)))
#define VIN_RES_MR_SRE2_M_ATD1_NPR_MASK               (((1 << 2) - 1) << 13)
#define VIN_RES_MR_SRE2_M_ATD1_NPR_GET(reg)           (((reg) >> 13) & ((1 << 2) - 1))
#define VIN_RES_MR_SRE2_M_ATD1_NPR_SET(val)           ((((1 << 2) - 1) & (val)) << 13)
#define VIN_RES_MR_SRE2_M_ATD1_NPR_MODIFY(reg, val)   (reg = ( (reg & ~VIN_RES_MR_SRE2_M_ATD1_NPR_MASK) | ((((1 << 2) - 1) & (val)) << 13) ))
#define VIN_RES_MR_SRE2_M_ATD1_AM                     (1 << 12)
#define VIN_RES_MR_SRE2_M_ATD1_AM_GET(reg)            ((reg & VIN_RES_MR_SRE2_M_ATD1_AM) >> 12)
#define VIN_RES_MR_SRE2_M_ATD1_AM_SET(reg, val)       (reg = ((reg & ~VIN_RES_MR_SRE2_M_ATD1_AM) | (((val) & 0x0001) << 12)))
#define VIN_RES_MR_SRE2_M_ATD2_DT                     (1 << 11)
#define VIN_RES_MR_SRE2_M_ATD2_DT_GET(reg)            ((reg & VIN_RES_MR_SRE2_M_ATD2_DT) >> 11)
#define VIN_RES_MR_SRE2_M_ATD2_DT_SET(reg, val)       (reg = ((reg & ~VIN_RES_MR_SRE2_M_ATD2_DT) | (((val) & 0x0001) << 11)))
#define VIN_RES_MR_SRE2_M_ATD2_NPR_MASK               (((1 << 2) - 1) << 9)
#define VIN_RES_MR_SRE2_M_ATD2_NPR_GET(reg)           (((reg) >> 9) & ((1 << 2) - 1))
#define VIN_RES_MR_SRE2_M_ATD2_NPR_SET(val)           ((((1 << 2) - 1) & (val)) << 9)
#define VIN_RES_MR_SRE2_M_ATD2_NPR_MODIFY(reg, val)   (reg = ( (reg & ~VIN_RES_MR_SRE2_M_ATD2_NPR_MASK) | ((((1 << 2) - 1) & (val)) << 9) ))
#define VIN_RES_MR_SRE2_M_ATD2_AM                     (1 << 8)
#define VIN_RES_MR_SRE2_M_ATD2_AM_GET(reg)            ((reg & VIN_RES_MR_SRE2_M_ATD2_AM) >> 8)
#define VIN_RES_MR_SRE2_M_ATD2_AM_SET(reg, val)       (reg = ((reg & ~VIN_RES_MR_SRE2_M_ATD2_AM) | (((val) & 0x0001) << 8)))
#define VIN_RES_MR_SRE2_RES7                          (1 << 7)
#define VIN_RES_MR_SRE2_RES7_GET(reg)                 ((reg & VIN_RES_MR_SRE2_RES7) >> 7)
#define VIN_RES_MR_SRE2_RES7_SET(reg, val)            (reg = ((reg & ~VIN_RES_MR_SRE2_RES7) | (((val) & 0x0001) << 7)))
#define VIN_RES_MR_SRE2_M_ETU_OF                      (1 << 6)
#define VIN_RES_MR_SRE2_M_ETU_OF_GET(reg)             ((reg & VIN_RES_MR_SRE2_M_ETU_OF) >> 6)
#define VIN_RES_MR_SRE2_M_ETU_OF_SET(reg, val)        (reg = ((reg & ~VIN_RES_MR_SRE2_M_ETU_OF) | (((val) & 0x0001) << 6)))
#define VIN_RES_MR_SRE2_PVPU_OFFDP_ERR                (1 << 5)
#define VIN_RES_MR_SRE2_PVPU_OFFDP_ERR_GET(reg)      ((reg & VIN_RES_MR_SRE2_PVPU_OFFDP_ERR) >> 5)
#define VIN_RES_MR_SRE2_PVPU_OFFDP_ERR_SET(reg, val) (reg = ((reg & ~VIN_RES_MR_SRE2_PVPU_OFFDP_ERR) | (((val) & 0x0001) << 5)))
#define VIN_RES_MR_SRE2_RES4                          (1 << 4)
#define VIN_RES_MR_SRE2_RES4_GET(reg)                 ((reg & VIN_RES_MR_SRE2_RES4) >> 4)
#define VIN_RES_MR_SRE2_RES4_SET(reg, val)            (reg = ((reg & ~VIN_RES_MR_SRE2_RES4) | (((val) & 0x0001) << 4)))
#define VIN_RES_MR_SRE2_VPOU_JBL                      (1 << 3)
#define VIN_RES_MR_SRE2_VPOU_JBL_GET(reg)             ((reg & VIN_RES_MR_SRE2_VPOU_JBL) >> 3)
#define VIN_RES_MR_SRE2_VPOU_JBL_SET(reg, val)        (reg = ((reg & ~VIN_RES_MR_SRE2_VPOU_JBL) | (((val) & 0x0001) << 3)))
#define VIN_RES_MR_SRE2_VPOU_JBH                      (1 << 2)
#define VIN_RES_MR_SRE2_VPOU_JBH_GET(reg)             ((reg & VIN_RES_MR_SRE2_VPOU_JBH) >> 2)
#define VIN_RES_MR_SRE2_VPOU_JBH_SET(reg, val)        (reg = ((reg & ~VIN_RES_MR_SRE2_VPOU_JBH) | (((val) & 0x0001) << 2)))
#define VIN_RES_MR_SRE2_DEC_ERR                       (1 << 1)
#define VIN_RES_MR_SRE2_DEC_ERR_GET(reg)              ((reg & VIN_RES_MR_SRE2_DEC_ERR) >> 1)
#define VIN_RES_MR_SRE2_DEC_ERR_SET(reg, val)         (reg = ((reg & ~VIN_RES_MR_SRE2_DEC_ERR) | (((val) & 0x0001) << 1)))
#define VIN_RES_MR_SRE2_DEC_CHGFDP_REQ                (1 << 0)
#define VIN_RES_MR_SRE2_DEC_CHGFDP_REQ_GET(reg)      ((reg & VIN_RES_MR_SRE2_DEC_CHGFDP_REQ) >> 0)
#define VIN_RES_MR_SRE2_DEC_CHGFDP_REQ_SET(reg, val) (reg = ((reg & ~VIN_RES_MR_SRE2_DEC_CHGFDP_REQ) | (((val) & 0x0001) << 0)))

/***Mask Register for Status Register for EDSP Interrupts
 1***/

#define VIN_RES_MF_SRE1                               ( (VIN_RES + 0x60) )

#define VIN_RES_MF_SRE1_M_DTMFR_DT                    (1 << 15)
#define VIN_RES_MF_SRE1_M_DTMFR_DT_GET(reg)           ((reg & VIN_RES_MF_SRE1_M_DTMFR_DT) >> 15)
#define VIN_RES_MF_SRE1_M_DTMFR_DT_SET(reg, val)      (reg = ((reg & ~VIN_RES_MF_SRE1_M_DTMFR_DT) | (((val) & 0x0001) << 15)))
#define VIN_RES_MF_SRE1_M_DTMFR_PDT                   (1 << 14)
#define VIN_RES_MF_SRE1_M_DTMFR_PDT_GET(reg)          ((reg & VIN_RES_MF_SRE1_M_DTMFR_PDT) >> 14)
#define VIN_RES_MF_SRE1_M_DTMFR_PDT_SET(reg, val)     (reg = ((reg & ~VIN_RES_MF_SRE1_M_DTMFR_PDT) | (((val) & 0x0001) << 14)))
#define VIN_RES_MF_SRE1_M_DTMFR_DTC_MASK              (((1 << 4) - 1) << 10)
#define VIN_RES_MF_SRE1_M_DTMFR_DTC_GET(reg)          (((reg) >> 10) & ((1 << 4) - 1))
#define VIN_RES_MF_SRE1_M_DTMFR_DTC_SET(val)          ((((1 << 4) - 1) & (val)) << 10)
#define VIN_RES_MF_SRE1_M_DTMFR_DTC_MODIFY(reg, val)  (reg = ( (reg & ~VIN_RES_MF_SRE1_M_DTMFR_DTC_MASK) | ((((1 << 4) - 1) & (val)) << 10) ))
#define VIN_RES_MF_SRE1_M_UTD1_OK                     (1 << 9)
#define VIN_RES_MF_SRE1_M_UTD1_OK_GET(reg)            ((reg & VIN_RES_MF_SRE1_M_UTD1_OK) >> 9)
#define VIN_RES_MF_SRE1_M_UTD1_OK_SET(reg, val)       (reg = ((reg & ~VIN_RES_MF_SRE1_M_UTD1_OK) | (((val) & 0x0001) << 9)))
#define VIN_RES_MF_SRE1_M_UTD2_OK                     (1 << 8)
#define VIN_RES_MF_SRE1_M_UTD2_OK_GET(reg)            ((reg & VIN_RES_MF_SRE1_M_UTD2_OK) >> 8)
#define VIN_RES_MF_SRE1_M_UTD2_OK_SET(reg, val)       (reg = ((reg & ~VIN_RES_MF_SRE1_M_UTD2_OK) | (((val) & 0x0001) << 8)))
#define VIN_RES_MF_SRE1_RES7                          (1 << 7)
#define VIN_RES_MF_SRE1_RES7_GET(reg)                 ((reg & VIN_RES_MF_SRE1_RES7) >> 7)
#define VIN_RES_MF_SRE1_RES7_SET(reg, val)            (reg = ((reg & ~VIN_RES_MF_SRE1_RES7) | (((val) & 0x0001) << 7)))
#define VIN_RES_MF_SRE1_RES6                          (1 << 6)
#define VIN_RES_MF_SRE1_RES6_GET(reg)                 ((reg & VIN_RES_MF_SRE1_RES6) >> 6)
#define VIN_RES_MF_SRE1_RES6_SET(reg, val)            (reg = ((reg & ~VIN_RES_MF_SRE1_RES6) | (((val) & 0x0001) << 6)))
#define VIN_RES_MF_SRE1_M_DTMFG_BUF                   (1 << 5)
#define VIN_RES_MF_SRE1_M_DTMFG_BUF_GET(reg)          ((reg & VIN_RES_MF_SRE1_M_DTMFG_BUF) >> 5)
#define VIN_RES_MF_SRE1_M_DTMFG_BUF_SET(reg, val)     (reg = ((reg & ~VIN_RES_MF_SRE1_M_DTMFG_BUF) | (((val) & 0x0001) << 5)))
#define VIN_RES_MF_SRE1_M_DTMFG_REQ                   (1 << 4)
#define VIN_RES_MF_SRE1_M_DTMFG_REQ_GET(reg)          ((reg & VIN_RES_MF_SRE1_M_DTMFG_REQ) >> 4)
#define VIN_RES_MF_SRE1_M_DTMFG_REQ_SET(reg, val)     (reg = ((reg & ~VIN_RES_MF_SRE1_M_DTMFG_REQ) | (((val) & 0x0001) << 4)))
#define VIN_RES_MF_SRE1_M_DTMFG_ACT                   (1 << 3)
#define VIN_RES_MF_SRE1_M_DTMFG_ACT_GET(reg)          ((reg & VIN_RES_MF_SRE1_M_DTMFG_ACT) >> 3)
#define VIN_RES_MF_SRE1_M_DTMFG_ACT_SET(reg, val)     (reg = ((reg & ~VIN_RES_MF_SRE1_M_DTMFG_ACT) | (((val) & 0x0001) << 3)))
#define VIN_RES_MF_SRE1_M_CIS_BUF                     (1 << 2)
#define VIN_RES_MF_SRE1_M_CIS_BUF_GET(reg)            ((reg & VIN_RES_MF_SRE1_M_CIS_BUF) >> 2)
#define VIN_RES_MF_SRE1_M_CIS_BUF_SET(reg, val)       (reg = ((reg & ~VIN_RES_MF_SRE1_M_CIS_BUF) | (((val) & 0x0001) << 2)))
#define VIN_RES_MF_SRE1_M_CIS_REQ                     (1 << 1)
#define VIN_RES_MF_SRE1_M_CIS_REQ_GET(reg)            ((reg & VIN_RES_MF_SRE1_M_CIS_REQ) >> 1)
#define VIN_RES_MF_SRE1_M_CIS_REQ_SET(reg, val)       (reg = ((reg & ~VIN_RES_MF_SRE1_M_CIS_REQ) | (((val) & 0x0001) << 1)))
#define VIN_RES_MF_SRE1_M_CIS_ACT                     (1 << 0)
#define VIN_RES_MF_SRE1_M_CIS_ACT_GET(reg)            ((reg & VIN_RES_MF_SRE1_M_CIS_ACT) >> 0)
#define VIN_RES_MF_SRE1_M_CIS_ACT_SET(reg, val)       (reg = ((reg & ~VIN_RES_MF_SRE1_M_CIS_ACT) | (((val) & 0x0001) << 0)))

/***Mask Register for Status Register for EDSP Interrupts
 2***/

#define VIN_RES_MF_SRE2                               ( (VIN_RES + 0x61) )

#define VIN_RES_MF_SRE2_M_ATD1_DT                     (1 << 15)
#define VIN_RES_MF_SRE2_M_ATD1_DT_GET(reg)            ((reg & VIN_RES_MF_SRE2_M_ATD1_DT) >> 15)
#define VIN_RES_MF_SRE2_M_ATD1_DT_SET(reg, val)       (reg = ((reg & ~VIN_RES_MF_SRE2_M_ATD1_DT) | (((val) & 0x0001) << 15)))
#define VIN_RES_MF_SRE2_M_ATD1_NPR_MASK               (((1 << 2) - 1) << 13)
#define VIN_RES_MF_SRE2_M_ATD1_NPR_GET(reg)           (((reg) >> 13) & ((1 << 2) - 1))
#define VIN_RES_MF_SRE2_M_ATD1_NPR_SET(val)           ((((1 << 2) - 1) & (val)) << 13)
#define VIN_RES_MF_SRE2_M_ATD1_NPR_MODIFY(reg, val)   (reg = ( (reg & ~VIN_RES_MF_SRE2_M_ATD1_NPR_MASK) | ((((1 << 2) - 1) & (val)) << 13) ))
#define VIN_RES_MF_SRE2_M_ATD1_AM                     (1 << 12)
#define VIN_RES_MF_SRE2_M_ATD1_AM_GET(reg)            ((reg & VIN_RES_MF_SRE2_M_ATD1_AM) >> 12)
#define VIN_RES_MF_SRE2_M_ATD1_AM_SET(reg, val)       (reg = ((reg & ~VIN_RES_MF_SRE2_M_ATD1_AM) | (((val) & 0x0001) << 12)))
#define VIN_RES_MF_SRE2_M_ATD2_DT                     (1 << 11)
#define VIN_RES_MF_SRE2_M_ATD2_DT_GET(reg)            ((reg & VIN_RES_MF_SRE2_M_ATD2_DT) >> 11)
#define VIN_RES_MF_SRE2_M_ATD2_DT_SET(reg, val)       (reg = ((reg & ~VIN_RES_MF_SRE2_M_ATD2_DT) | (((val) & 0x0001) << 11)))
#define VIN_RES_MF_SRE2_M_ATD2_NPR_MASK               (((1 << 2) - 1) << 9)
#define VIN_RES_MF_SRE2_M_ATD2_NPR_GET(reg)           (((reg) >> 9) & ((1 << 2) - 1))
#define VIN_RES_MF_SRE2_M_ATD2_NPR_SET(val)           ((((1 << 2) - 1) & (val)) << 9)
#define VIN_RES_MF_SRE2_M_ATD2_NPR_MODIFY(reg, val)   (reg = ( (reg & ~VIN_RES_MF_SRE2_M_ATD2_NPR_MASK) | ((((1 << 2) - 1) & (val)) << 9) ))
#define VIN_RES_MF_SRE2_M_ATD2_AM                     (1 << 8)
#define VIN_RES_MF_SRE2_M_ATD2_AM_GET(reg)            ((reg & VIN_RES_MF_SRE2_M_ATD2_AM) >> 8)
#define VIN_RES_MF_SRE2_M_ATD2_AM_SET(reg, val)       (reg = ((reg & ~VIN_RES_MF_SRE2_M_ATD2_AM) | (((val) & 0x0001) << 8)))
#define VIN_RES_MF_SRE2_RES7                          (1 << 7)
#define VIN_RES_MF_SRE2_RES7_GET(reg)                 ((reg & VIN_RES_MF_SRE2_RES7) >> 7)
#define VIN_RES_MF_SRE2_RES7_SET(reg, val)            (reg = ((reg & ~VIN_RES_MF_SRE2_RES7) | (((val) & 0x0001) << 7)))
#define VIN_RES_MF_SRE2_M_ETU_OF                      (1 << 6)
#define VIN_RES_MF_SRE2_M_ETU_OF_GET(reg)             ((reg & VIN_RES_MF_SRE2_M_ETU_OF) >> 6)
#define VIN_RES_MF_SRE2_M_ETU_OF_SET(reg, val)        (reg = ((reg & ~VIN_RES_MF_SRE2_M_ETU_OF) | (((val) & 0x0001) << 6)))
#define VIN_RES_MF_SRE2_PVPU_OFFDP_ERR                (1 << 5)
#define VIN_RES_MF_SRE2_PVPU_OFFDP_ERR_GET(reg)      ((reg & VIN_RES_MF_SRE2_PVPU_OFFDP_ERR) >> 5)
#define VIN_RES_MF_SRE2_PVPU_OFFDP_ERR_SET(reg, val) (reg = ((reg & ~VIN_RES_MF_SRE2_PVPU_OFFDP_ERR) | (((val) & 0x0001) << 5)))
#define VIN_RES_MF_SRE2_RES4                          (1 << 4)
#define VIN_RES_MF_SRE2_RES4_GET(reg)                 ((reg & VIN_RES_MF_SRE2_RES4) >> 4)
#define VIN_RES_MF_SRE2_RES4_SET(reg, val)            (reg = ((reg & ~VIN_RES_MF_SRE2_RES4) | (((val) & 0x0001) << 4)))
#define VIN_RES_MF_SRE2_VPOU_JBL                      (1 << 3)
#define VIN_RES_MF_SRE2_VPOU_JBL_GET(reg)             ((reg & VIN_RES_MF_SRE2_VPOU_JBL) >> 3)
#define VIN_RES_MF_SRE2_VPOU_JBL_SET(reg, val)        (reg = ((reg & ~VIN_RES_MF_SRE2_VPOU_JBL) | (((val) & 0x0001) << 3)))
#define VIN_RES_MF_SRE2_VPOU_JBH                      (1 << 2)
#define VIN_RES_MF_SRE2_VPOU_JBH_GET(reg)             ((reg & VIN_RES_MF_SRE2_VPOU_JBH) >> 2)
#define VIN_RES_MF_SRE2_VPOU_JBH_SET(reg, val)        (reg = ((reg & ~VIN_RES_MF_SRE2_VPOU_JBH) | (((val) & 0x0001) << 2)))
#define VIN_RES_MF_SRE2_DEC_ERR                       (1 << 1)
#define VIN_RES_MF_SRE2_DEC_ERR_GET(reg)              ((reg & VIN_RES_MF_SRE2_DEC_ERR) >> 1)
#define VIN_RES_MF_SRE2_DEC_ERR_SET(reg, val)         (reg = ((reg & ~VIN_RES_MF_SRE2_DEC_ERR) | (((val) & 0x0001) << 1)))
#define VIN_RES_MF_SRE2_DEC_CHGFDP_REQ                (1 << 0)
#define VIN_RES_MF_SRE2_DEC_CHGFDP_REQ_GET(reg)      ((reg & VIN_RES_MF_SRE2_DEC_CHGFDP_REQ) >> 0)
#define VIN_RES_MF_SRE2_DEC_CHGFDP_REQ_SET(reg, val) (reg = ((reg & ~VIN_RES_MF_SRE2_DEC_CHGFDP_REQ) | (((val) & 0x0001) << 0)))

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
