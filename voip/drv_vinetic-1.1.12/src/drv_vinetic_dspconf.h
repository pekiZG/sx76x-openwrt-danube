#ifndef _DRV_VINETIC_DSPCONF_H
#define _DRV_VINETIC_DSPCONF_H
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
   Module      : drv_vinetic_dspconf.h
   Date        : 2004-07-21
   Description : This file contains the implementations of vinetic dsp
                 related configuations
*******************************************************************************/

/* ============================= */
/* Global Defines                */
/* ============================= */

/* ============================= */
/* Global Structures             */
/* ============================= */

typedef enum {
   DSP_CPTD_FL_16                 = 0,
   DSP_CPTD_FL_32                 = 1,
   DSP_CPTD_FL_64                 = 2
} DSP_CPTD_FL_t;

typedef enum
{
   DSP_CPTD_TP_10_3400_HZ         = 0,
   DSP_CPTD_TP_250_3400_HZ        = 1
} DSP_CPTD_TP_t;

typedef enum
{
   DSP_CPTD_WS_HAMMING            = 0,
   DSP_CPTD_WS_BLACKMAN           = 1
} DSP_CPTD_WS_t;


/* ============================= */
/* Global function declaration   */
/* ============================= */

extern IFX_int32_t Dsp_SetIdleMode      (VINETIC_DEVICE *pDev, IFX_boolean_t bEn);
extern IFX_int32_t Dsp_SetCidSender     (VINETIC_CHANNEL *pCh, IFX_boolean_t bEn);
extern IFX_int32_t Dsp_SetDtmfRec       (VINETIC_CHANNEL *pCh, IFX_boolean_t bEn);
extern IFX_int32_t Dsp_SetToneDetectors (VINETIC_CHANNEL *pCh, IFX_boolean_t bEn);
extern IFX_int32_t Dsp_DtmfGen          (VINETIC_CHANNEL *pCh, IFX_uint16_t nMode,
                                         IFX_uint16_t nMask,
                                         IFX_boolean_t nIsr);
extern IFX_int32_t Dsp_SetDtmfCoeff     (VINETIC_CHANNEL *pCh, IFX_uint8_t nTIMT,
                                         IFX_uint8_t nTIMP);
extern IFX_int32_t Dsp_AlmNlpEnable      (VINETIC_CHANNEL *pCh);
extern IFX_int32_t Dsp_SwitchSignModules (VINETIC_CHANNEL *pCh, IFX_boolean_t bOn);
extern IFX_int32_t Dsp_SetDPDemod       (VINETIC_CHANNEL *pCh, IFX_uint8_t nSt1,
                                         IFX_uint8_t nSt2, IFX_uint8_t nEq,
                                         IFX_uint8_t nTr);
extern IFX_int32_t Dsp_SetDPMod          (VINETIC_CHANNEL *pCh, IFX_uint8_t nSt,
                                          IFX_uint16_t nLen, IFX_uint8_t nDbm,
                                          IFX_uint8_t nTEP, IFX_uint8_t nTr);
extern IFX_int32_t Dsp_SetDatapump      (VINETIC_CHANNEL *pCh, IFX_boolean_t bEn,
                                         IFX_boolean_t bMod, IFX_uint16_t gain,
                                         IFX_uint16_t mod_start,
                                         IFX_uint16_t mod_req, IFX_uint16_t demod_send);
extern IFX_int32_t Dsp_SetNeLec_Alm     (VINETIC_CHANNEL *pCh, IFX_boolean_t bEnLec,
                                         IFX_boolean_t bEnNlp, IFX_boolean_t bEnWLec);
extern IFX_int32_t Dsp_SetNeLec_Pcm     (VINETIC_CHANNEL *pCh, IFX_boolean_t bEnLec,
                                         IFX_boolean_t bEnNlp, IFX_boolean_t bEnWLec);
#ifdef TAPI
extern IFX_int32_t Dsp_AtdConf          (VINETIC_CHANNEL *pCh, IFX_uint32_t nSignal);
extern IFX_int32_t Dsp_AtdSigEnable     (VINETIC_CHANNEL *pCh, IFX_uint32_t nSignal);
extern IFX_int32_t Dsp_UtdConf          (VINETIC_CHANNEL *pCh, IFX_uint32_t nSignal);
extern IFX_int32_t Dsp_UtdSigEnable     (VINETIC_CHANNEL *pCh, IFX_uint32_t nSignal);
extern IFX_int32_t Dsp_Cptd_Conf        (VINETIC_CHANNEL *pCh,
                                         IFX_TAPI_TONE_SIMPLE_t const *pTone,
                                         IFX_int32_t signal, IFX_uint16_t* pCmd,
                                         DSP_CPTD_FL_t frameLength);
extern IFX_int32_t Dsp_Cptd_SetCoeff    (VINETIC_CHANNEL *pCh,
                                         IFX_TAPI_TONE_SIMPLE_t const *pTone,
                                         IFX_uint16_t* pCmd,
                                         DSP_CPTD_FL_t *pFrameLength);
extern IFX_int32_t Dsp_Utg_Stop         (VINETIC_CHANNEL *pCh, IFX_uint8_t utgNum);
extern IFX_int32_t Dsp_SetRTPPayloadType (VINETIC_CHANNEL *pCh, IFX_boolean_t bUp,
                                          IFX_uint8_t *pPTvalues);
#ifdef TAPI_CID
extern IFX_int32_t Dsp_SetCIDCoeff      (VINETIC_CHANNEL *pCh,
                                         IFX_TAPI_CID_HOOK_MODE_t cidHookMode,
                                         IFX_TAPI_CID_FSK_CFG_t *pFskConf,
                                         IFX_uint8_t nSize);
extern IFX_int32_t Dsp_SetCidRec        (VINETIC_CHANNEL const *pCh,
                                         IFX_boolean_t bEn);
#endif /* TAPI_CID */
#endif /* TAPI */

#endif /* _DRV_VINETIC_DSPCONF_H */
