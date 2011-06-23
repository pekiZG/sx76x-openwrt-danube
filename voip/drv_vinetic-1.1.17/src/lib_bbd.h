#ifndef _LIB_BBD_H
#define _LIB_BBD_H
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
