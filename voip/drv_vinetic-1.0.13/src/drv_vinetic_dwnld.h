#ifndef _DRV_VINETICDOWNL_H
#define _DRV_VINETICDOWNL_H
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
   Module      : drv_vinetic_dwnld.h
   Date        : 2003-10-27
   Description : This file contains the declaration of the download and
                 CRC structures, macros and functions.
*******************************************************************************/

/* ============================= */
/* Global Defines & enums        */
/* ============================= */

/* FPI DC/AC Code */
enum
{
  DC_CODE = 0,
  AC_CODE = 1
};

/* ============================= */
/* Global Structures             */
/* ============================= */


/* ============================= */
/* Global function declaration   */
/* ============================= */

IFX_int32_t VINETIC_DownloadCram         (VINETIC_CHANNEL *pCh, VINETIC_IO_CRAM *pCmd);

extern IFX_int32_t VINETIC_ReadEdspCrc   (VINETIC_DEVICE *pDev, VINETIC_IO_CRC* pCmd);
extern IFX_int32_t VINETIC_DownloadPhi   (VINETIC_DEVICE *pDev, VINETIC_IO_PHI* pCmd);
extern IFX_int32_t VINETIC_DownloadFpi   (VINETIC_DEVICE *pDev,
                                          VINETIC_IO_FPI_DOWNLOAD* pCmd);
extern IFX_int32_t VINETIC_DwldBinary    (VINETIC_DEVICE *pDev, IFX_uint8_t *pByte,
                                          IFX_int32_t byte_size, VINETIC_FW_RAM ram, IFX_uint16_t *pRamCrc);
#ifdef VIN_V21_SUPPORT
extern IFX_int32_t VINETIC_Download_EmbdCtrl (VINETIC_DEVICE *pDev, VINETIC_IO_EMBDCTRL *pCmd);
#endif /* VIN_V21_SUPPORT */

/* Download/CRC driver global functions */
extern IFX_int32_t Dwld_LoadFirmware     (VINETIC_DEVICE *pDev, VINETIC_IO_INIT *pInit);
extern IFX_int32_t Dwld_LoadPhi          (VINETIC_DEVICE *pDev, VINETIC_IO_INIT *pInit);
extern IFX_int32_t Dwld_LoadCram         (VINETIC_DEVICE *pDev, VINETIC_IO_INIT *pInit);

#ifdef VIN_V14_SUPPORT
extern IFX_int32_t Dwld_LoadFpiCode      (VINETIC_DEVICE *pDev, VINETIC_IO_INIT *pInit,
                                          IFX_int32_t nCode);
#endif /* VIN_V14_SUPPORT */

#ifdef VIN_V21_SUPPORT
extern IFX_int32_t Dwld_Load_EmbdCtrl    (VINETIC_DEVICE *pDev, IFX_uint32_t *pData,
                                          IFX_uint16_t size);
#endif /* VIN_V21_SUPPORT */

#endif /* _DRV_VINETICDOWNL_H */

