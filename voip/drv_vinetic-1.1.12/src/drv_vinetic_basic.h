#ifndef _DRV_VINETIC_BASIC_H
#define _DRV_VINETIC_BASIC_H
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
   Module      : drv_vinetic_basic.h
   Date        : 2002-01-15
   Description : This file contains the declaration of the functions
                 for usual vinetic operations.
*******************************************************************************/

/* ============================= */
/* Global Defines                */
/* ============================= */

/* ============================= */
/* Global Structures             */
/* ============================= */

/* ============================= */
/* Global variables declarations */
/* ============================= */
/* array of partial values used to approach the function 10^X */
extern const IFX_uint32_t  tens [][2];

/* ============================= */
/* Global function declaration   */
/* ============================= */

IFX_int32_t VINETIC_Read_Cmd     (VINETIC_DEVICE *pDev, VINETIC_IO_MB_CMD* pCmd);
IFX_int32_t VINETIC_Write_Cmd    (VINETIC_DEVICE *pDev, VINETIC_IO_MB_CMD* pCmd);
#ifdef VIN_2CPE
IFX_int32_t VINETIC_HostRegWr_Cmd(VINETIC_DEVICE *pDev, VINETIC_IO_REG_ACCESS *pCmd);
IFX_int32_t VINETIC_HostRegRd_Cmd(VINETIC_DEVICE *pDev, VINETIC_IO_REG_ACCESS *pCmd);
#else
IFX_int32_t VINETIC_Write_Sc     (VINETIC_DEVICE *pDev, VINETIC_IO_WRITE_SC* pCmd);
IFX_int32_t VINETIC_Read_Sc      (VINETIC_DEVICE *pDev, VINETIC_IO_READ_SC* pCmd);
#endif /* VIN_2CPE */
#ifdef TAPI
IFX_int32_t Basic_VoIPConf       (VINETIC_CHANNEL *pCh);
IFX_int32_t Basic_PcmConf        (VINETIC_CHANNEL *pCh, IFX_uint8_t nPcmType);
#endif /* TAPI */
#endif /* _DRV_VINETIC_BASIC_H */
