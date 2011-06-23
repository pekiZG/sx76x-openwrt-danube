#ifndef _DRV_VINETIC_MAIN_H
#define _DRV_VINETIC_MAIN_H
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
   Module      : drv_vinetic_main
   Description :
      Main driver operations as initializations called in open driver
 *****************************************************************************/
/* ============================= */
/* Includes                      */
/* ============================= */

#include "drv_version.h"

/* ============================= */
/* Defines                       */
/* ============================= */

#ifndef _MKSTR_1
#define _MKSTR_1(x)    #x
#define _MKSTR(x)      _MKSTR_1(x)
#endif


/** driver version string */
#define DRV_VINETIC_VER_STR      _MKSTR(MAJORSTEP)    "."   \
                                 _MKSTR(MINORSTEP)    "."   \
                                 _MKSTR(VERSIONSTEP)  "."   \
                                 _MKSTR(VERS_TYPE)

/** what compatible driver version */
#define DRV_VINETIC_WHAT_STR "@(#)Vinetic driver module, version " DRV_VINETIC_VER_STR

/* ============================= */
/* Global variable declaration   */
/* ============================= */

extern const IFX_char_t DRV_VINETIC_WHATVERSION[] ;

/* ============================= */
/* Global function definition    */
/* ============================= */

extern IFX_int32_t BasicDeviceInit(VINETIC_DEVICE *pDev,
                                   VINETIC_BasicDeviceInit_t *pBasicDeviceInit);
extern IFX_int32_t VINETIC_Setup_Access_Function(VINETIC_DEVICE *pDev,
                                                 VIN_ACCESS nAccess);
extern IFX_int32_t VINETIC_Setup_Access_Pointer(VINETIC_DEVICE *pDev,
                                                VIN_ACCESS nAccess);
extern IFX_int32_t VINETIC_Init(VINETIC_DEVICE *pDev, VINETIC_IO_INIT *pInit);
extern IFX_int32_t InitDevMember(VINETIC_DEVICE *pDev);
extern IFX_int32_t ResetDevMember(VINETIC_DEVICE *pDev);
extern IFX_int32_t ResetChMember(VINETIC_CHANNEL*pCh);
extern IFX_int32_t InitVinetic(VINETIC_DEVICE *pDev);
extern IFX_void_t  ResetDevState(VINETIC_DEVICE *pDev);
extern IFX_void_t  ExitDev(VINETIC_DEVICE *pDev);
extern IFX_int32_t Device_Reset(VINETIC_DEVICE *pDev);
extern IFX_int32_t InitChTimers    (VINETIC_CHANNEL* pCh);

#endif /* _DRV_VINETIC_MAIN_H */

