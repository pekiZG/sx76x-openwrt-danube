#ifndef _DRV_VINETICMISC_H
#define _DRV_VINETICMISC_H
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
 ****************************************************************************/
/** \file  drv_vinetic_misc.h
   This file contains the declaration of the functions for usual vinetic
   operations
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

extern IFX_int32_t VINETIC_DrvVersion    (VINETIC_IO_VERSION* pInd);
extern IFX_int32_t VINETIC_Version       (VINETIC_DEVICE *pDev,
                                          VINETIC_IO_VERSION* pInd);
extern IFX_int32_t VINETIC_GetVersions   (VINETIC_DEVICE *pDev);
extern IFX_int32_t VINETIC_Report_Set    (IFX_uint32_t driver_level);
extern IFX_int32_t VINETIC_RuntimeTrace_Report_Set (IFX_uint32_t level);
IFX_int32_t VINETIC_Dev_GPIO_Cfg (VINETIC_DEVICE *pDev, VINETIC_IO_DEV_GPIO_CFG const *pCfg);
IFX_int32_t VINETIC_Dev_GPIO_Set (VINETIC_DEVICE *pDev, VINETIC_IO_DEV_GPIO_SET const *pSet);
extern IFX_int32_t VINETIC_AccessTest (VINETIC_DEVICE *pDev, IFX_uint16_t max_val);
#endif /* _DRV_VINETICMISC_H */

