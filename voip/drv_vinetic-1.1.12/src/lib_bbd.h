#ifndef _LIB_BBD_H
#define _LIB_BBD_H
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
   Module      : lib_bbd.h
   Description : Declaration of generic functions for block based download
                 handling
   Remarks     : Relevant for the implementation is the block based download
                 specification located in the doc/ directory.
*******************************************************************************/

/* ============================= */
/* Global Defines                */
/* ============================= */

/** bbd master block tag */
#define BBD_MASTER_BLOCK         0x000A
/** bbd master block magic */
#define BBD_MASTER_MAGIC         0x21626264 /*"!bbd"*/
/** bbd end block tag */
#define BBD_END_BLOCK            0x0000

/* errors domains */

/** bbd integrity error domain */
#define BBD_INTG_ERR_DOMAIN      0x0100

/* ============================= */
/* Global enumerations           */
/* ============================= */

typedef enum
{
   /** bbd buffer integrity OK */
   BBD_INTG_OK              = (BBD_INTG_ERR_DOMAIN | 0x0),
   /** bbd buffer invalid */
   BBD_INTG_ERR_INVALID     = (BBD_INTG_ERR_DOMAIN | 0x1),
   /** bbd buffer has no master block */
   BBD_INTG_ERR_NOMASTER    = (BBD_INTG_ERR_DOMAIN | 0x2),
   /** bbd buffer has master blocks with wrong magic */
   BBD_INTG_ERR_WRONGMASTER = (BBD_INTG_ERR_DOMAIN | 0x3),
   /** bbd buffer has empty valid master block */
   BBD_INTG_ERR_EMPTYMASTER = (BBD_INTG_ERR_DOMAIN | 0x4),
   /** bbd buffer has no master block with my family magic */
   BBD_INTG_ERR_NOFAMILY    = (BBD_INTG_ERR_DOMAIN | 0x5),
   /** bbd buffer has several master blocks
       with my family magic */
   BBD_INTG_ERR_MULTIFAMILY = (BBD_INTG_ERR_DOMAIN | 0x6),
   /** bbd buffer has no end block */
   BBD_INTG_ERR_NOEND       = (BBD_INTG_ERR_DOMAIN | 0x7),
   /** bbd buffer has several end blocks */
   BBD_INTG_ERR_MULTIEND    = (BBD_INTG_ERR_DOMAIN | 0x8)
   /* list continues here for future */
} bbd_error_t;

/* ============================= */
/* Global Structures             */
/* ============================= */

/** block based download format */
typedef struct
{
   /** block based download buffer,
       big-endian aligned */
   IFX_uint8_t *buf;
   /** size of buffer in bytes */
   IFX_uint32_t size;
} bbd_format_t;

/** block based download block */
typedef struct
{
   /** unique master block family
      identifier, in */
   IFX_uint32_t identifier;
   /** block tag, in */
   IFX_uint16_t tag;
   /** version tag */
   IFX_uint16_t version;
   /** block index from 0, in */
   IFX_uint16_t index;
   /** block data pointer, out */
   IFX_uint8_t *data;
   /** block data size in bytes, out */
   IFX_uint32_t size;
} bbd_block_t;

/* ============================= */
/* Global function declaration   */
/* ============================= */

bbd_error_t bbd_check_integrity (bbd_format_t *bbd, IFX_uint32_t identifier);
IFX_void_t  bbd_get_block       (bbd_format_t *bbd, bbd_block_t *block);

#endif /* _LIB_BBD_H */
