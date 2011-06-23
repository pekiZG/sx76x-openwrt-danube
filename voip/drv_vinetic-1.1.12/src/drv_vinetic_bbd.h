#ifndef _DRV_VINETIC_BBD_H
#define _DRV_VINETIC_BBD_H
/****************************************************************************
       Copyright (c) 2005, Infineon Technologies.  All rights reserved.

                               No Warranty
   Because the program is licensed free of charge, there is no warranty for
   the program, to the extent permitted by applicable law.  Except when
   otherwise stated in writing the copyright holders and/or other parties
   provide the program "as is" without warranty of any kind, either
   expressed or implied, including, but not limited to, the implied
   warranties of merchantability and fitness for a particular purpose. The
   entire risk as to the quality and performance of the program is with
   you.  should the program prove defective, you assume the cost of all
   necessary servicing, repair or correction.

   In no event unless required by applicable law or agreed to in writing
   will any copyright holder, or any other party who may modify and/or
   redistribute the program as permitted above, be liable to you for
   damages, including any general, special, incidental or consequential
   damages arising out of the use or inability to use the program
   (including but not limited to loss of data or data being rendered
   inaccurate or losses sustained by you or third parties or a failure of
   the program to operate with any other programs), even if such holder or
   other party has been advised of the possibility of such damages.
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
