#ifndef _DRV_VINETIC_BBD_H
#define _DRV_VINETIC_BBD_H
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
   Module      : drv_vinetic_bbd.h
   Description :
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */

#include "lib_bbd.h"

/* ============================= */
/* Global Defines                */
/* ============================= */

#define BBD_VIN_MAGIC                         0x32343130 /* "2410" */

/** Vinetic BBD blocks tags */
/* 0x1XXX tags : Vinetic DC/AC Coefficients */
#define BBD_COMPATIBILITY_BLOCK_TAG          0x000C
#define BBD_VIN_CRAM_BLOCK                   0x1001
#define BBD_VIN_SLIC_BLOCK                   0x1002
#define BBD_VINETIC_RING_CFG_BLOCK           0x1003
#define BBD_VINETIC_DC_THRESHOLDS_BLOCK      0x1004
/* 0x11XX tags : Vinetic Patches */
#define BBD_VIN_AC_BLOCK                     0x1101
/* 0x2XXX tags : Vinetic FW coefficients */
#define BBD_VIN_FWLECCOEFS_BLOCK              0x2001
/* 0x3XXX tags : Vinetic Device Driver Coefficients */


/** SLIC TYPE Defines */
#define BBD_VIN_SLIC_TYPE_DC                  0x0101
#define BBD_VIN_SLIC_TYPE_E                   0x0201


/* maximum of downloads per BBD block,
   can be set also at compile time  */
#ifndef BBD_VIN_BLOCK_MAXDWNLD
#define BBD_VIN_BLOCK_MAXDWNLD                10
#endif /* BBD_VIN_BLOCK_MAXDWNLD */

/* ============================= */
/* Global Structures             */
/* ============================= */


/* ============================= */
/* Global function declaration   */
/* ============================= */

/* Vinetic bbd helper functions */
IFX_void_t VINETIC_BBD_SetEdspPtr (bbd_format_t *pBBDUsr,
                                   VINETIC_EDSP_FWDWLD *pEdsp,
                                   VINETIC_FW_RAM ram);
IFX_void_t VINETIC_BBD_SetPtr     (bbd_format_t *pBBDUsr, bbd_format_t *pPtr,
                                   IFX_uint16_t nBBDTag);
/* host channel download functions based on decoded BBD format */
IFX_int32_t VINETIC_Host_BBD_DownloadChAC    (VINETIC_CHANNEL *pCh,
                                              bbd_block_t     *bbd_ac);
IFX_int32_t VINETIC_Host_BBD_DownloadChCram  (VINETIC_CHANNEL *pCh,
                                              bbd_block_t     *bbd_cram);
/* bbd io functions */
IFX_int32_t VINETIC_BBD_Download (VINETIC_CHANNEL *pCh, bbd_format_t *pBBD);
#endif /* _DRV_VINETIC_BBD_H */
