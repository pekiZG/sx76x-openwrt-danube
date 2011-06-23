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

/* ============================= */
/* Global function declaration   */
/* ============================= */

extern IFX_int32_t Dsp_SetCidSender     (VINETIC_CHANNEL *pCh, IFX_boolean_t bEn);
extern IFX_int32_t Dsp_SetDtmfRec       (VINETIC_CHANNEL *pCh, IFX_boolean_t bEn);
extern IFX_int32_t Dsp_SetToneDetectors (VINETIC_CHANNEL *pCh, IFX_boolean_t bEn);
extern IFX_int32_t Dsp_AtdConf          (VINETIC_CHANNEL *pCh, IFX_uint32_t nSignal);
extern IFX_int32_t Dsp_AtdSigEnable     (VINETIC_CHANNEL *pCh, IFX_uint32_t nSignal);
extern IFX_int32_t Dsp_UtdConf          (VINETIC_CHANNEL *pCh, IFX_uint32_t nSignal);
extern IFX_int32_t Dsp_UtdSigEnable     (VINETIC_CHANNEL *pCh, IFX_uint32_t nSignal);
extern IFX_int32_t Dsp_Cptd_Conf        (VINETIC_CHANNEL *pCh, TAPI_SIMPLE_TONE const *pTone,
                                         IFX_int32_t signal, IFX_uint16_t* pCmd);
extern IFX_int32_t Dsp_Cptd_SetCoeff    (VINETIC_CHANNEL *pCh,
                                         TAPI_SIMPLE_TONE const *pTone, IFX_uint16_t *pCmd);
extern IFX_int32_t Dsp_DtmfConf         (VINETIC_CHANNEL *pCh, IFX_uint8_t nLevel1, IFX_uint8_t nLevel2,
                                         IFX_uint8_t nTIMT, IFX_uint8_t nTIMP, IFX_uint8_t nAttA1, IFX_uint8_t nAttA2);
extern IFX_int32_t Dsp_DtmfGen          (VINETIC_CHANNEL *pCh, IFX_uint16_t nMode, IFX_uint16_t nMask,
                                         IFX_boolean_t nIsr);
extern IFX_int32_t Dsp_AlmNlpEnable      (VINETIC_CHANNEL *pCh);
extern IFX_int32_t Dsp_SwitchSignModules (VINETIC_CHANNEL *pCh, IFX_boolean_t bOn);
extern IFX_int32_t Dsp_SetDPDemod       (VINETIC_CHANNEL *pCh, IFX_uint8_t nSt1,
                                         IFX_uint8_t nSt2, IFX_uint8_t nEq, IFX_uint8_t nTr);
extern IFX_int32_t Dsp_SetDPMod          (VINETIC_CHANNEL *pCh, IFX_uint8_t nSt,
                                          IFX_uint16_t nLen, IFX_uint8_t nDbm,
                                          IFX_uint8_t nTEP, IFX_uint8_t nTr);
extern IFX_int32_t Dsp_SetDatapump      (VINETIC_CHANNEL *pCh, IFX_boolean_t bEn,
                                         IFX_boolean_t bMod, IFX_uint16_t gain, IFX_uint16_t mod_start,
                                         IFX_uint16_t mod_req, IFX_uint16_t demod_send);
extern IFX_int32_t Dsp_SetNeLec_Alm(VINETIC_CHANNEL *pCh, IFX_boolean_t bEnLec, IFX_boolean_t bEnNlp);
extern IFX_int32_t Dsp_SetNeLec_Pcm(VINETIC_CHANNEL *pCh, IFX_boolean_t bEnLec, IFX_boolean_t bEnNlp);

#endif /* _DRV_VINETIC_DSPCONF_H */
