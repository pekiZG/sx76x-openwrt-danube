#ifndef _DRV_VINETIC_CID_H
#define _DRV_VINETIC_CID_H
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
   Module      : drv_vinetic_cid.h
   Date        : 2003-10-29
   Description : This file contains the declaration of the functions
                 for CID operations
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
extern IFX_int32_t  VINETIC_CidFskMachine  (VINETIC_CHANNEL *pCh);
extern IFX_void_t VINETIC_CidDtmfMachine   (VINETIC_CHANNEL *pCh);
extern IFX_int32_t VINETIC_DtmfStart       (VINETIC_CHANNEL *pCh, IFX_uint16_t *pDtmfData, 
                                            IFX_uint16_t nDtmfWords, IFX_uint32_t nFG, 
                                            IFX_void_t (*cbDtmfStatus)(VINETIC_CHANNEL *pCh),
                                            IFX_boolean_t bByteMode);
extern IFX_int32_t  VINETIC_DtmfOnRequest  (VINETIC_CHANNEL *pCh);
extern IFX_int32_t  VINETIC_DtmfOnActivate (VINETIC_CHANNEL *pCh, IFX_boolean_t bAct);
extern IFX_int32_t  VINETIC_DtmfOnUnderrun (VINETIC_CHANNEL *pCh);
extern IFX_int32_t  VINETIC_DtmfStop       (VINETIC_CHANNEL *pCh, IFX_int32_t nIsr);
#endif /* _DRV_VINETIC_CID_H */
