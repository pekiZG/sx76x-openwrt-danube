#ifndef _DRV_VINETICDOWNL_H
#define _DRV_VINETICDOWNL_H
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
   Module      : drv_vinetic_dwnld.h
   Date        : 2003-10-27
   Description : This file contains the declaration of the download and
                 CRC structures, macros and functions.
*******************************************************************************/

/* ============================= */
/* Global Defines & enums        */
/* ============================= */


/* ============================= */
/* Global Structures             */
/* ============================= */

/** structure used for VINETIC EDSP Firmware download
 */
typedef struct
{
   /** Flag to consider while doing FW download.
       Refer to \ref VINETIC_IO_INIT for more details.
      - NO_EDSP_START
      - FW_AUTODWLD
      - NO_FW_DWLD
   */
   unsigned long nEdspFlags;
   /** valid Firmware PRAM byte pointer */
   unsigned char *pPRAMfw ;
   /** size of PRAM firmware in bytes */
   unsigned long pram_size;
   /** valid Firmware DRAM byte pointer */
   unsigned char *pDRAMfw ;
   /** size of DRAM firmware in bytes */
   unsigned long dram_size;
   /** return values of PRAM CRC after firmware download */
   unsigned short nPramCRC;
   /** return values of DRAM CRC after firmware download */
   unsigned short nDramCRC;
} VINETIC_EDSP_FWDWLD;

/* ============================= */
/* Global function declaration   */
/* ============================= */

IFX_int32_t Dwld_LoadFirmware   (VINETIC_DEVICE *pDev,
                                 VINETIC_EDSP_FWDWLD *pEdsp);
IFX_int32_t VINETIC_ReadEdspCrc (VINETIC_DEVICE *pDev, VINETIC_IO_CRC* pCmd);
IFX_int32_t VINETIC_DwldBinary  (VINETIC_DEVICE *pDev, IFX_uint8_t *pByte,
                                 IFX_uint32_t byte_size, VINETIC_FW_RAM ram,
                                 IFX_uint16_t *pRamCrc);
#endif /* _DRV_VINETICDOWNL_H */

