#ifndef _DRV_VINETIC_ACCESS_H
#define _DRV_VINETIC_ACCESS_H
/****************************************************************************
       Copyright (c) 2002, Infineon Technologies.  All rights reserved.

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
   Module      : drv_vinetic_access.h
   Description : Low level access macros and functions declarations.
*******************************************************************************/

#ifdef HAVE_CONFIG_H
#include <drv_config.h>
#endif /* HAVE_CONFIG_H */
#include <sys_drv_ifxos.h>

/* ============================= */
/* Global Defines                */
/* ============================= */

/* Chip Access Modes  */

/** Motorola Access Mode, set at compile time. */
#define VIN_ACCESS_MODE_MOTOROLA                         1
/** Mux/Intel Access Mode, set at compile time. */
#define VIN_ACCESS_MODE_INTEL_MUX                        2
/** demux access mode, set at compile time. */
#define VIN_ACCESS_MODE_INTEL_DEMUX                      3
/** SPI access mode, set at compile time. */
#define VIN_ACCESS_MODE_SPI                              4
/** Combination of all access modes for evaluation.
       Access mode selected is stored in device structure. */
#define VIN_ACCESS_MODE_EVALUATION                       5


/** VINETIC device driver does 8bit accesses */
#define VIN_ACCESS_WIDTH_8                               8
/** VINETIC device driver does 16bit accesses (default).
    In case the VINETIC supports only 8bit bus accesses,
    the driver expects the memory controller do do two
    8bit bus accesses (with the correct timing). */
#define VIN_ACCESS_WIDTH_16                             16


/** Host register offset from which the indirect access is possible in
    intel demux mode */
#define V2CPE_OFFSET_INDIRECT_ACCESS_INTEL            0x20
/** Host register offset from which the indirect access is possible in
    motorola mode */
#define V2CPE_OFFSET_INDIRECT_ACCESS_MOTOROLA         0x40

/** \defgroup BASIC_HOST_IFCE_ACCESS_MACROS
*/
/*@{*/

/** Reads one data from Parallel Host Interface.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - read ptr (16bit)
\remark
   Basic read access.
*/
#if (VIN_ACCESS_WIDTH == VIN_ACCESS_WIDTH_8)
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
#define VIN_HOST_IF_PAR_READ(pDev,offset,pbuf)\
   do {\
   *((IFX_int16_t *)(pbuf)) = \
      (*((IFX_uint8_t *)  ((IFX_void_t *)(((IFX_uint8_t*)((pDev)->pBaseAddr)) + (offset))))); \
   *((IFX_int16_t *)(pbuf)) |= \
      ( (*((IFX_uint8_t *)  ((IFX_void_t *)(((IFX_uint8_t*)((pDev)->pBaseAddr)) + (offset+1))))) <<8 );\
   }while (0)
#elif (__BYTE_ORDER == __BIG_ENDIAN)
#define VIN_HOST_IF_PAR_READ(pDev,offset,pbuf) \
   do {\
   *((IFX_int16_t *)(pbuf)) = \
      ( (*((IFX_uint8_t *)  ((IFX_void_t *)(((IFX_uint8_t*)((pDev)->pBaseAddr)) + (offset+1))))) <<8 ); \
   *((IFX_int16_t *)(pbuf)) |= \
      (*((IFX_uint8_t *)  ((IFX_void_t *)(((IFX_uint8_t*)((pDev)->pBaseAddr)) + (offset)))));\
   }while (0)
#else
   #error could not find out endianess... :-(
#endif /* __BYTE_ORDER */
#else
#define VIN_HOST_IF_PAR_READ(pDev,offset,pbuf) \
   do {\
   *((IFX_int16_t *)(pbuf)) = \
      *((IFX_uint16_t *)  ((IFX_void_t *)(((IFX_uint8_t*)((pDev)->pBaseAddr)) + (offset))));\
   }while (0)
#endif /* VIN_ACCESS_WIDTH */

/** Writes one data to Parallel Host Interface.
\param pDev    - handle to device
\param offset  - register offset
\param val     - data to write (16bit)
\remark
   Basic write access.
*/
#if (VIN_ACCESS_WIDTH == VIN_ACCESS_WIDTH_8)
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
#define VIN_HOST_IF_PAR_WRITE(pDev,offset,val) \
   do {\
   (*((IFX_uint8_t *)((IFX_void_t *)(((IFX_uint8_t*)((pDev)->pBaseAddr)) + (offset)))) = (val & 0x000000FF));\
   (*((IFX_uint8_t *)((IFX_void_t *)(((IFX_uint8_t*)((pDev)->pBaseAddr)) + (offset+1)))) = ((val >> 8) & 0x000000FF));\
   }while (0)
#elif (__BYTE_ORDER == __BIG_ENDIAN)
#define VIN_HOST_IF_PAR_WRITE(pDev,offset,val) \
   do {\
   (*((IFX_uint8_t *)((IFX_void_t *)(((IFX_uint8_t*)((pDev)->pBaseAddr)) + (offset+1)))) = ((val >> 8) & 0x000000FF));\
   (*((IFX_uint8_t *)((IFX_void_t *)(((IFX_uint8_t*)((pDev)->pBaseAddr)) + (offset)))) = (val & 0x000000FF));\
   }while (0)
#else
   #error could not find out endianess... :(
#endif /* __BYTE_ORDER */
#else
#define VIN_HOST_IF_PAR_WRITE(pDev,offset,val) \
   do{\
   (*((IFX_uint16_t *)((IFX_void_t *)(((IFX_uint8_t*)((pDev)->pBaseAddr)) + (offset)))) = val);\
   }while (0)
#endif /* VIN_ACCESS_WIDTH */
/** Reads data from same uC address, as autoincrement is done by the chip
    (mailbox or indirect access).
\param pDev      - handle to device
\param ai_offset - auto increment offset
                   (e.g V2CPE_BOX_CDATA/V2CPE_BOX_VDATA/V2CPE_DATA)
\param pbuf      - read ptr (16bit)
\param len       - number of data(16bit) to read
\remark
   all data are read from the same address. Offset is not incremented.
*/
#define VIN_HOST_IF_NOINCR_READ_MULTI(pDev,ai_offset,pbuf,len)\
   do {\
      IFX_uint32_t i;\
      for (i = 0; i < len; i++) {\
         VIN_HOST_IF_PAR_READ((pDev),(ai_offset),((IFX_uint16_t *)(pbuf)) + i);\
   }} while (0)

/** Writes data to same uC address, autoincrement is done by the chip
    (mailbox or indirect access).
\param pDev       - handle to device
\param ai_offset  - auto increment offset
                   (e.g V2CPE_BOX_CDATA/V2CPE_BOX_VDATA/V2CPE_DATA)
\param pbuf       - write ptr (16bit)
\param len        - number of data(16bit) to write
\remark
   all data are written to the same address. Offset is not incremented.
*/
#define VIN_HOST_IF_NOINCR_WRITE_MULTI(pDev,ai_offset,pbuf,len)\
   do {\
      IFX_uint32_t i;\
      for (i = 0; i < len; i++) {\
         VIN_HOST_IF_PAR_WRITE((pDev),(ai_offset),((IFX_uint16_t *)(pbuf))[i]);\
   }} while (0)

/** Reads data from incremented uC address.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - read ptr (16bit)
\param len     - number of data(16bit) to read
\remark
   offset is incremented here by 2 for each read, as the access is 16-bit.
   Autoincrement applies only for indirect and mailbox access.
*/
#define VIN_HOST_IF_INCR_READ_MULTI(pDev,offset,pbuf,len)\
   do {\
      IFX_uint32_t i;\
      for (i = 0; i < len; i++) {\
         VIN_HOST_IF_PAR_READ((pDev),(offset)+(i << 1),((IFX_uint16_t *)(pbuf)) + i);\
   }} while (0)

/** Writes data to incremented uc address.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - write ptr (16bit)
\param len     - number of data(16bit) to write
\remark
   offset is incremented here by 2 for each read, as the access is 16-bit.
   Autoincrement applies only for indirect and SPI access.
*/
#define VIN_HOST_IF_INCR_WRITE_MULTI(pDev,offset,pbuf,len)\
   do {\
      IFX_uint32_t i;\
      for (i = 0; i < len; i++) {\
         VIN_HOST_IF_PAR_WRITE((pDev),(offset)+(i << 1),((IFX_uint16_t *)(pbuf))[i]);\
   }} while (0)

/*@}*/

#if (VIN_ACCESS_MODE == VIN_ACCESS_MODE_MOTOROLA)

/** \defgroup 8BIT_MUX_ACCESS_MACROS
*/
/*@{*/

/** Unprotected single register read access.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - read ptr (16bit)
\remark
   For Mux/Motorola access mode, Basic host interface access macros are used.
*/
#define VIN_LL_UNPROT_REG_READ(pDev,offset,pbuf) \
   do {\
      v2cpe_reg_read_motorola ((pDev),(offset),(pbuf));\
   }while(0)

/** Unprotected single register write access.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - write ptr (16bit)
\remark
   For Mux/Motorola access mode, Basic host interface access macros are used.
*/
#define VIN_LL_UNPROT_REG_WRITE(pDev,offset,pbuf) \
   do{\
      v2cpe_reg_write_motorola ((pDev),(offset),(pbuf));\
   }while(0)

/** Unprotected multiple register read access.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - read ptr (16bit)
\param len     - number of data(16bit) to read
\remark
   For Mux/Motorola access mode, Basic host interface access macros are used.
*/
#define VIN_LL_UNPROT_REG_READ_MULTI(pDev,offset,pbuf,len) \
   do{\
      v2cpe_reg_readmulti_motorola ((pDev),(offset),(pbuf),(len));\
   }while (0)

/** Unprotected multiple register write access.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - write ptr (16bit)
\param len     - number of data(16bit) to write
\remark
   For Mux/Motorola access mode, Basic host interface access macros are used.
*/
#define VIN_LL_UNPROT_REG_WRITE_MULTI(pDev,offset,pbuf,len) \
   do{\
      v2cpe_reg_writemulti_motorola ((pDev),(offset),(pbuf),(len));\
   }while(0)

/** Unprotected command mailbox read access.
\param pDev    - handle to device
\param pbuf    - read ptr (16bit)
\param len     - number of data(16bit) to read
\remark
   For Mux/Motorola access mode, Basic host interface access macros are used.
*/
#define VIN_LL_UNPROT_CMD_MBX_READ(pDev,pbuf,len) \
   do{\
      IFXOS_ASSERT(V2CPE_BOX_CDATA < V2CPE_OFFSET_INDIRECT_ACCESS_MOTOROLA);\
      VIN_HOST_IF_NOINCR_READ_MULTI((pDev),V2CPE_BOX_CDATA,(pbuf),(len));\
   } while(0)

/** Unprotected command mailbox write access.
\param pDev    - handle to device
\param pbuf    - write ptr (16bit)
\param len     - number of data(16bit) to write
\remark
   For Mux/Motorola access mode, Basic host interface access macros are used.
*/
#define VIN_LL_UNPROT_CMD_MBX_WRITE(pDev,pbuf,len) \
   do{\
      IFXOS_ASSERT(V2CPE_BOX_CDATA < V2CPE_OFFSET_INDIRECT_ACCESS_MOTOROLA);\
      VIN_HOST_IF_NOINCR_WRITE_MULTI((pDev),V2CPE_BOX_CDATA,(pbuf),(len));\
   } while(0)

/** Unprotected voice mailbox read access.
\param pDev    - handle to device
\param pbuf    - read ptr (16bit)
\param len     - number of data(16bit) to read
\remark
   For Mux/Motorola access mode, Basic host interface access macros are used.
*/
#define VIN_LL_UNPROT_VOICE_MBX_READ(pDev,pbuf,len) \
   do{\
      IFXOS_ASSERT(V2CPE_BOX_VDATA < V2CPE_OFFSET_INDIRECT_ACCESS_MOTOROLA);\
      VIN_HOST_IF_NOINCR_READ_MULTI(pDev,V2CPE_BOX_VDATA,(pbuf),(len));\
      LOG_RD_PKT(pbuf, len, pDev->err);\
   }while (0)

/** Unprotected voice mailbox write access.
\param pDev    - handle to device
\param pbuf    - write ptr (16bit)
\param len     - number of data(16bit) to write
\remark
   For Mux/Motorola access mode, Basic host interface access macros are used.
*/
#define VIN_LL_UNPROT_VOICE_MBX_WRITE(pDev,pbuf,len) \
   do {\
      IFXOS_ASSERT(V2CPE_BOX_VDATA < V2CPE_OFFSET_INDIRECT_ACCESS_MOTOROLA);\
      VIN_HOST_IF_NOINCR_WRITE_MULTI(pDev,V2CPE_BOX_VDATA,(pbuf),(len));\
      LOG_WR_PKT(pbuf, len, pDev->err);\
   }while (0)

/*@}*/

#elif (VIN_ACCESS_MODE == VIN_ACCESS_MODE_INTEL_MUX)

/** \defgroup 8BIT_MUX_ACCESS_MACROS
*/
/*@{*/

/** Unprotected single register read access.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - read ptr (16bit)
\remark
   For Intel access mode, Basic host interface access macros are used.
*/
#define VIN_LL_UNPROT_REG_READ(pDev,offset,pbuf) \
   do {\
      VIN_HOST_IF_PAR_READ((pDev),(offset),(pbuf));\
   } while(0)

/** Unprotected single register write access.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - write ptr (16bit)
\remark
   For Intel access mode, Basic host interface access macros are used.
*/
#define VIN_LL_UNPROT_REG_WRITE(pDev,offset,pbuf) \
   do {\
   VIN_HOST_IF_PAR_WRITE((pDev),(offset),(pbuf));\
   } while(0)

/** Unprotected multiple register read access.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - read ptr (16bit)
\param len     - number of data(16bit) to read
\remark
   For Intel access mode, Basic host interface access macros are used.
*/
#define VIN_LL_UNPROT_REG_READ_MULTI(pDev,offset,pbuf,len) \
   do{\
      VIN_HOST_IF_INCR_READ_MULTI((pDev),(offset),(pbuf),(len));\
   } while(0)

/** Unprotected multiple register write access.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - write ptr (16bit)
\param len     - number of data(16bit) to write
\remark
   For Intel access mode, Basic host interface access macros are used.
*/
#define VIN_LL_UNPROT_REG_WRITE_MULTI(pDev,offset,pbuf,len) \
   do{\
      VIN_HOST_IF_INCR_WRITE_MULTI((pDev),(offset),(pbuf),(len));\
   } while(0)

/** Unprotected command mailbox read access.
\param pDev    - handle to device
\param pbuf    - read ptr (16bit)
\param len     - number of data(16bit) to read
\remark
   For Intel access mode, Basic host interface access macros are used.
*/
#define VIN_LL_UNPROT_CMD_MBX_READ(pDev,pbuf,len) \
   do{\
      VIN_HOST_IF_NOINCR_READ_MULTI((pDev),V2CPE_BOX_CDATA,(pbuf),(len));\
   } while(0)

/** Unprotected command mailbox write access.
\param pDev    - handle to device
\param pbuf    - write ptr (16bit)
\param len     - number of data(16bit) to write
\remark
   For Intel access mode, Basic host interface access macros are used.
*/
#define VIN_LL_UNPROT_CMD_MBX_WRITE(pDev,pbuf,len) \
   do{\
      VIN_HOST_IF_NOINCR_WRITE_MULTI((pDev),V2CPE_BOX_CDATA,(pbuf),(len));\
   } while(0)

/** Unprotected voice mailbox read access.
\param pDev    - handle to device
\param pbuf    - read ptr (16bit)
\param len     - number of data(16bit) to read
\remark
   For Intel access mode, Basic host interface access macros are used.
*/
#define VIN_LL_UNPROT_VOICE_MBX_READ(pDev,pbuf,len) \
   do{\
      VIN_HOST_IF_NOINCR_READ_MULTI(pDev,V2CPE_BOX_VDATA,(pbuf),(len));\
      LOG_RD_PKT(pbuf, len, pDev->err);\
   } while(0)

/** Unprotected voice mailbox write access.
\param pDev    - handle to device
\param pbuf    - write ptr (16bit)
\param len     - number of data(16bit) to write
\remark
   For Intel access mode, Basic host interface access macros are used.
*/
#define VIN_LL_UNPROT_VOICE_MBX_WRITE(pDev,pbuf,len) \
   do{\
      VIN_HOST_IF_NOINCR_WRITE_MULTI(pDev,V2CPE_BOX_VDATA,(pbuf),(len));\
      LOG_WR_PKT(pbuf, len, pDev->err);\
   } while (0)


/*@}*/

#elif (VIN_ACCESS_MODE == VIN_ACCESS_MODE_INTEL_DEMUX)

/** \defgroup 8BIT_DEMUX_ACCESS_MACROS
*/
/*@{*/

/** Unprotected single register read access.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - read ptr (16bit)
\remark
   Direct access for Registers of offset < 0x20.
   Indirect access for Registers of offset >= 0x20 via V2CPE_ADDR/V2CPE_DATA.
*/
#define VIN_LL_UNPROT_REG_READ(pDev,offset,pbuf)\
   do{\
      v2cpe_reg_read_intel_demux ((pDev),(offset),(pbuf));\
   }while(0)

/** Unprotected single register write access.
\param pDev    - handle to device
\param offset  - register offset
\param val     - value to write (16bit)
\remark
   Direct access for Registers of offset < 0x20.
   Indirect access for Registers of offset >= 0x20 via V2CPE_ADDR/V2CPE_DATA.
*/
#define VIN_LL_UNPROT_REG_WRITE(pDev,offset,val)\
   do{\
      v2cpe_reg_write_intel_demux ((pDev),(offset),(val));\
   }while(0)

/** Unprotected multiple register read access.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - read ptr (16bit)
\param len     - number of data(16bit) to read
\remark
   Direct access for Registers of offset < 0x20.
   Indirect access for Registers of offset >= 0x20 via V2CPE_ADDR/V2CPE_DATA.
   Assert for direct access, in case (offset + len) >= 0x20.
*/
#define VIN_LL_UNPROT_REG_READ_MULTI(pDev,offset,pbuf,len)\
   do{\
      v2cpe_reg_readmulti_intel_demux ((pDev),(offset),(pbuf),(len));\
   }while(0)

/** Unprotected multiple register write access.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - write ptr (16bit)
\param len     - number of data(16bit) to write
\remark
   Direct access for Registers of offset < 0x20.
   Indirect access for Registers of offset >= 0x20 via V2CPE_ADDR/V2CPE_DATA.
   Assert for direct access, in case (offset + len) >= 0x20.
*/
#define VIN_LL_UNPROT_REG_WRITE_MULTI(pDev,offset,pbuf,len)\
   do{\
      v2cpe_reg_writemulti_intel_demux ((pDev),(offset),(pbuf),(len));\
   }while(0)

/** Unprotected command mailbox read access.
\param pDev    - handle to device
\param pbuf    - read ptr (16bit)
\param len     - number of data(16bit) to read
\remark
   For Demux access mode, indirect access is done for offset >= 0x20. Because
   the mailbox offset is below this offset, direct acces is used. Nevertheless,
   an assert is done, for the case that this offset changes later.
*/
#define VIN_LL_UNPROT_CMD_MBX_READ(pDev,pbuf,len) \
   do{\
      IFXOS_ASSERT(V2CPE_BOX_CDATA < V2CPE_OFFSET_INDIRECT_ACCESS_INTEL);\
      VIN_HOST_IF_NOINCR_READ_MULTI((pDev),V2CPE_BOX_CDATA,(pbuf),(len));\
   }while(0)

/** Unprotected command mailbox write access.
\param pDev    - handle to device
\param pbuf    - write ptr (16bit)
\param len     - number of data(16bit) to write
\remark
   For Demux access mode, indirect access is done for offset >= 0x20. Because
   the mailbox offset is below this offset, direct acces is used. Nevertheless,
   an assert is done, for the case that this offset changes later.
*/
#define VIN_LL_UNPROT_CMD_MBX_WRITE(pDev,pbuf,len) \
   do{\
      IFXOS_ASSERT(V2CPE_BOX_VDATA < V2CPE_OFFSET_INDIRECT_ACCESS_INTEL);\
      VIN_HOST_IF_NOINCR_WRITE_MULTI((pDev),V2CPE_BOX_CDATA,(pbuf),(len));\
   } while(0)

/** Unprotected voice mailbox read access.
\param pDev  - handle to device
\param pbuf  - read ptr (16bit)
\param len   - number of data(16bit) to read
\remark
   For Demux access mode, indirect access is done for offset >= 0x20. Because
   the mailbox offset is below this offset, direct acces is used. Nevertheless,
   an assert is done, for the case that this offset changes later.
*/
#define VIN_LL_UNPROT_VOICE_MBX_READ(pDev,pbuf,len) \
   do{\
      IFXOS_ASSERT(V2CPE_BOX_VDATA < V2CPE_OFFSET_INDIRECT_ACCESS_INTEL);\
      VIN_HOST_IF_NOINCR_READ_MULTI(pDev,V2CPE_BOX_VDATA,(pbuf),(len));\
      LOG_RD_PKT(pbuf, len, pDev->err);\
   }while(0)

/** Unprotected voice mailbox write access.
\param pDev    - handle to device
\param pbuf    - write ptr (16bit)
\param len     - number of data(16bit) to write
\remark
   For Demux access mode, indirect access is done for offset >= 0x20. Because
   the mailbox offset is below this offset, direct acces is used. Nevertheless,
   an assert is done, for the case that this offset changes later.
*/
#define VIN_LL_UNPROT_VOICE_MBX_WRITE(pDev,pbuf,len) \
   do{\
      IFXOS_ASSERT(V2CPE_BOX_VDATA < V2CPE_OFFSET_INDIRECT_ACCESS_INTEL);\
      VIN_HOST_IF_NOINCR_WRITE_MULTI(pDev,V2CPE_BOX_VDATA,(pbuf),(len));\
      LOG_WR_PKT(pbuf, len, pDev->err);\
   } while(0)

/*@}*/

#elif (VIN_ACCESS_MODE == VIN_ACCESS_MODE_SPI)

/** \defgroup 8BIT_DEMUX_ACCESS_MACROS
*/
/*@{*/

/** Unprotected single register read access.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - read ptr (16bit)
*/
#define VIN_LL_UNPROT_REG_READ(pDev,offset,pbuf)\
   do{\
      v2cpe_read_spi ((pDev),(offset),(pbuf), 1);\
   }while(0)

/** Unprotected single register write access.
\param pDev    - handle to device
\param offset  - register offset
\param val     - value to write (16bit)
\remark
   Because the value passed with val can be a constant also, this value is
   copied to a temporary variable which is passed by reference to the spi
   handling function.
*/
#define VIN_LL_UNPROT_REG_WRITE(pDev,offset,val)\
   do {\
      IFX_uint16_t tmp = val;\
      v2cpe_write_spi((pDev),(offset),&tmp,1);\
   }while(0)


/** Unprotected multiple register read access.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - read ptr (16bit)
\param len     - number of data(16bit) to read
\remark
   Autoincrement is done by SPI interface for more than one access
*/
#define VIN_LL_UNPROT_REG_READ_MULTI(pDev,offset,pbuf,len) \
   do {\
      v2cpe_read_spi ((pDev),(offset),(pbuf),(len));\
   } while(0)

/** Unprotected multiple register write access.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - write ptr (16bit)
\param len     - number of data(16bit) to write
\remark
   Autoincrement is done by SPI interface for more than one access
*/
#define VIN_LL_UNPROT_REG_WRITE_MULTI(pDev,offset,pbuf,len) \
   do{\
      v2cpe_write_spi((pDev),(offset),(pbuf),(len));\
   } while(0)

/** Unprotected command mailbox read access.
\param pDev    - handle to device
\param pbuf    - read ptr (16bit)
\param len     - number of data(16bit) to read
*/
#define VIN_LL_UNPROT_CMD_MBX_READ(pDev,pbuf,len) \
   do{\
      v2cpe_read_spi((pDev),V2CPE_BOX_CDATA,(pbuf),(len));\
   } while(0)

/** Unprotected command mailbox write access.
\param pDev    - handle to device
\param pbuf    - write ptr (16bit)
\param len     - number of data(16bit) to write
*/
#define VIN_LL_UNPROT_CMD_MBX_WRITE(pDev,pbuf,len) \
   do{\
      v2cpe_write_spi((pDev),V2CPE_BOX_CDATA,(pbuf),(len));\
   } while(0)

/** Unprotected voice mailbox read access.
\param pDev  - handle to device
\param pbuf  - read ptr (16bit)
\param len   - number of data(16bit) to read
*/
#define VIN_LL_UNPROT_VOICE_MBX_READ(pDev,pbuf,len) \
   do{\
      v2cpe_read_spi (pDev, V2CPE_BOX_VDATA,(pbuf),(len));\
      LOG_RD_PKT(pbuf, len, pDev->err);\
   } while(0)

/** Unprotected voice mailbox write access.
\param pDev    - handle to device
\param pbuf    - write ptr (16bit)
\param len     - number of data(16bit) to write
*/
#define VIN_LL_UNPROT_VOICE_MBX_WRITE(pDev,pbuf,len) \
   do{\
   v2cpe_write_spi(pDev,V2CPE_BOX_VDATA,(pbuf),(len));\
   LOG_WR_PKT(pbuf, len, pDev->err);\
   } while(0)

/*@}*/

#elif (VIN_ACCESS_MODE == VIN_ACCESS_MODE_EVALUATION)

/** \defgroup 8BIT_DEMUX_ACCESS_MACROS
*/
/*@{*/

/** Unprotected single register read access.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - read ptr (16bit)
*/
#define VIN_LL_UNPROT_REG_READ(pDev,offset,pbuf) \
   do{\
   v2cpe_read_eval ((pDev),(offset),(pbuf), 1);\
   } while(0)

/** Unprotected single register write access.
\param pDev    - handle to device
\param offset  - register offset
\param val     - value to write (16bit)
\remark
   Because the value passed with val can be a constant also, this value is
   copied to a temporary variable which is passed by reference to the spi
   handling function.
*/
#define VIN_LL_UNPROT_REG_WRITE(pDev,offset,val) \
   do {\
      IFX_uint16_t tmp = val;\
      v2cpe_write_eval((pDev),(offset),&tmp,1);\
   }while(0)

/** Unprotected multiple register read access.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - read ptr (16bit)
\param len     - number of data(16bit) to read
\remark
   Autoincrement is done by SPI interface for more than one access
*/
#define VIN_LL_UNPROT_REG_READ_MULTI(pDev,offset,pbuf,len) \
   do{\
      v2cpe_read_eval ((pDev),(offset),(pbuf),(len));\
   } while(0)

/** Unprotected multiple register write access.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - write ptr (16bit)
\param len     - number of data(16bit) to write
*/
#define VIN_LL_UNPROT_REG_WRITE_MULTI(pDev,offset,pbuf,len) \
   do{\
      v2cpe_write_eval((pDev),(offset),(pbuf),(len));\
   } while (0);

/** Unprotected command mailbox read access.
\param pDev    - handle to device
\param pbuf    - read ptr (16bit)
\param len     - number of data(16bit) to read
*/
#define VIN_LL_UNPROT_CMD_MBX_READ(pDev,pbuf,len) \
   do{\
      v2cpe_read_eval((pDev),V2CPE_BOX_CDATA,(pbuf),(len));\
   } while (0);

/** Unprotected command mailbox write access.
\param pDev    - handle to device
\param pbuf    - write ptr (16bit)
\param len     - number of data(16bit) to write
*/
#define VIN_LL_UNPROT_CMD_MBX_WRITE(pDev,pbuf,len) \
   do{\
      v2cpe_write_eval((pDev),V2CPE_BOX_CDATA,(pbuf),(len));\
   } while (0);

/** Unprotected voice mailbox read access.
\param pDev  - handle to device
\param pbuf  - read ptr (16bit)
\param len   - number of data(16bit) to read
*/
#define VIN_LL_UNPROT_VOICE_MBX_READ(pDev,pbuf,len) \
   do{\
      v2cpe_read_eval (pDev, V2CPE_BOX_VDATA,(pbuf),(len));\
      LOG_RD_PKT(pbuf, len, pDev->err);\
   } while (0);

/** Unprotected voice mailbox write access.
\param pDev    - handle to device
\param pbuf    - write ptr (16bit)
\param len     - number of data(16bit) to write
*/
#define VIN_LL_UNPROT_VOICE_MBX_WRITE(pDev,pbuf,len) \
   do{\
      v2cpe_write_eval(pDev,V2CPE_BOX_VDATA,(pbuf),(len));\
      LOG_WR_PKT(pbuf, len, pDev->err);\
   } while(0)

/*@}*/
#else
#error undefined/unhandled VIN_ACCESS_MODE
#endif /* VIN_ACCESS_MODE */

/** \defgroup HOST_MBX_ACCESS_PROTECTION
*/
/*@{*/

/** Protects host mailbox access.
\param  pDev - handle to device
\remark Protection is done against concurrent tasks and interrupts
*/
#define VIN_HOST_PROTECT(pDev) \
   do{\
         IFXOS_MutexLock((pDev)->mbxAcc);\
         Vinetic_IrqLockDevice((pDev));\
   } while(0)

/** Releases host mailbox access protection
\param  pDev - handle to device
*/
#define VIN_HOST_RELEASE(pDev) \
   do{\
         Vinetic_IrqUnlockDevice((pDev));\
         IFXOS_MutexUnlock((pDev)->mbxAcc);\
   } while(0)

/*@}*/

/** \defgroup GENERIC_REGISTER_ACCESS_MACROS
*/
/*@{*/

/** Unprotected single register read.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - read ptr (16-bit)
\remark
   This macros calls the appropriate event logger macro to log
   the read register.
*/
#define REG_READ_UNPROT(pDev,offset,pbuf)\
   do {\
      VIN_LL_UNPROT_REG_READ((pDev),(offset),(pbuf));\
      LOG_RD_REG((offset),(pbuf),1);\
   } while(0)

/** Unprotected multiple register read.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - read ptr (16bit)
\param len     - size of data to read (16bit)
\remark
   This macros calls the appropriate event logger macro to log
   the read registers.
*/
#define REG_READ_UNPROT_MULTI(pDev,offset,pbuf,len)\
   do {\
      VIN_LL_UNPROT_REG_READ_MULTI((pDev),(offset),(pbuf),(len));\
      LOG_RD_REG((offset),(pbuf),(len));\
   } while(0)

/** Protected single register read.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - read ptr (16-bit)
\remark
   This macros calls the appropriate event logger macro to log
   the read register.
*/
#define REG_READ_PROT(pDev,offset,pbuf)\
   do {\
      VIN_HOST_PROTECT(pDev);\
      VIN_LL_UNPROT_REG_READ((pDev),(offset),(pbuf));\
      VIN_HOST_RELEASE(pDev);\
      LOG_RD_REG((offset),(pbuf),1);\
   } while(0)

/** Protected multiple register read.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - read ptr (16bit)
\param len     - size of data to read (16bit)
\remark
   This macros calls the appropriate event logger macro to log
   the read registers.
*/
#define REG_READ_PROT_MULTI(pDev,offset,pbuf,len)\
   do {\
      VIN_HOST_PROTECT(pDev);\
      VIN_LL_UNPROT_REG_READ_MULTI((pDev),(offset),(pbuf),(len));\
      VIN_HOST_RELEASE(pDev);\
      LOG_RD_REG((offset),(pbuf),(len));\
   } while (0)

/** Unprotected single register write.
\param pDev    - handle to device
\param offset  - register offset
\param val     - value to write (16-bit)
\remark
   This macros calls the appropriate event logger macro to log
   the register and the written value.
   In case val is a constant, it is copied into a temporary variable tmp
   which is passed by reference to log macro.
*/
#define REG_WRITE_UNPROT(pDev,offset,val)\
   do {\
      VIN_LL_UNPROT_REG_WRITE((pDev),(offset),(val));\
      LOG_WR_REG((offset),(val));\
   } while (0)

/** Unprotected multiple register write.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - read ptr (16-bit)
\param len     - size of data to write (16bit)
\remark
   This macros calls the appropriate event logger macro to log
   the registers and the written values.
*/
#define REG_WRITE_UNPROT_MULTI(pDev,offset,pbuf,len)\
   do {\
      VIN_LL_UNPROT_REG_WRITE_MULTI((pDev),(offset),(pbuf),(len));\
      LOG_WR_REG_MULTI((offset),(pbuf),(len));\
   } while(0)

/** Protected single register write.
\param pDev    - handle to device
\param offset  - register offset
\param val     - value to write (16-bit)
\remark
   This macros calls the appropriate event logger macro to log
   the register and the written value.
   In case val is a constant, it is copied into a temporary variable tmp
   which is passed by reference to log macro.
*/
#define REG_WRITE_PROT(pDev,offset,val)\
   do {\
      VIN_HOST_PROTECT(pDev);\
      VIN_LL_UNPROT_REG_WRITE((pDev),(offset),(val));\
      VIN_HOST_RELEASE(pDev);\
      LOG_WR_REG((offset),(val));\
   } while(0)

/** Protected multiple register write.
\param pDev    - handle to device
\param offset  - register offset
\param pbuf    - write ptr (16-bit)
\param len     - length of write buffer (16bit)
\remark
   This macros calls the appropriate event logger macro to log
   the registers and the written values.
*/
#define REG_WRITE_PROT_MULTI(pDev,offset,pbuf,len)\
   do {\
      VIN_HOST_PROTECT(pDev);\
      VIN_LL_UNPROT_REG_WRITE_MULTI((pDev),(offset),(pbuf),(len));\
      VIN_HOST_RELEASE(pDev);\
      LOG_WR_REG_MULTI((offset),(pbuf),(len));\
   } while (0)

/*@}*/

/** \defgroup GENERIC_MBX_ACCESS_MACROS
*/
/*@{*/

/** Unprotected write to voice mailbox.
\param pDev    - handle to device
\param pbuf    - voice write ptr (16-bit)
\param len     - length of voice buffer (16bit)
\remark
   This macros should be used in task context to write to the voice mailbox.
   Protection against task and interrupt is done.
*/
#define VIN_UNPROT_VOICE_MBX_WRITE(pDev,pbuf,len)\
   do {\
      V2CPE_PKT_MBX_SEM_TAKE(pDev);\
      VIN_LL_UNPROT_VOICE_MBX_WRITE(pDev,pbuf,len);\
      V2CPE_PKT_MBX_SEM_GIVE(pDev);\
   } while (0)

/** Protected read of voice mailbox.
\param pDev    - handle to device
\param pbuf    - voice read ptr (16-bit)
\param len     - length of voice buffer (16bit)
\remark
   This macros should be used in task context to read the voice mailbox.
   Protection against task and interrupt is done.
*/
#define VIN_PROT_VOICE_MBX_READ(pDev,pbuf,len)\
   do {\
      VIN_HOST_PROTECT(pDev);\
      VIN_LL_UNPROT_VOICE_MBX_READ(pDev,pbuf,len);\
      VIN_HOST_RELEASE(pDev);\
   } while (0)

/** Protected write to voice mailbox.
\param pDev    - handle to device
\param pbuf    - voice write ptr (16-bit)
\param len     - length of voice buffer (16bit)
\remark
   This macros should be used in task context to write to the voice mailbox.
   Protection against task and interrupt is done.
*/
#define VIN_PROT_VOICE_MBX_WRITE(pDev,pbuf,len)\
   do {\
      VIN_HOST_PROTECT(pDev);\
      VIN_UNPROT_VOICE_MBX_WRITE(pDev,pbuf,len);\
      VIN_HOST_RELEASE(pDev);\
   } while (0)

/** Protected read of command mailbox.
\param pDev    - handle to device
\param pbuf    - command read ptr (16-bit)
\param len     - length of command buffer (16bit)
\remark
   This macros should be used in task context to read the command mailbox.
   Protection against task and interrupt is done.
*/
#define VIN_PROT_CMD_MBX_READ(pDev,pbuf,len)\
   do {\
      VIN_HOST_PROTECT(pDev);\
      VIN_LL_UNPROT_CMD_MBX_READ(pDev,pbuf,len);\
      VIN_HOST_RELEASE(pDev);\
   } while (0)

/** Protected write of command mailbox.
\param pDev    - handle to device
\param pbuf    - voice write ptr (16-bit)
\param len     - length of voice buffer (16bit)
\remark
   This macros should be used in task context to write to the command mailbox.
   Protection against task and interrupt is done.

   Note: Currently this macro is not used, i.e. not tested in detail...
*/
#define VIN_PROT_CMD_MBX_WRITE(pDev,pbuf,len)\
   do {\
      VIN_HOST_PROTECT(pDev);\
      V2CPE_CMD_MBX_SEM_TAKE(pDev);\
      VIN_LL_UNPROT_CMD_MBX_WRITE(pDev,pbuf,len);\
      V2CPE_CMD_MBX_SEM_GIVE(pDev);\
      VIN_HOST_RELEASE(pDev);\
   } while (0)

/*@}*/

/* ============================= */
/* Global Structures             */
/* ============================= */

/* ============================= */
/* Global function declaration   */
/* ============================= */

IFX_void_t   v2cpe_byte_swap (VINETIC_DEVICE *pDev);
IFX_uint32_t v2cpe_test_access (VINETIC_DEVICE *pDev, IFX_uint16_t max_val);

#if ((VIN_ACCESS_MODE == VIN_ACCESS_MODE_MOTOROLA))
IFX_void_t v2cpe_reg_read_motorola (VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                                 IFX_uint16_t *pbuf);
IFX_void_t v2cpe_reg_write_motorola (VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                                  IFX_uint16_t val);
IFX_void_t v2cpe_reg_readmulti_motorola (VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                                      IFX_uint16_t *pbuf, IFX_uint32_t len);
IFX_void_t v2cpe_reg_writemulti_motorola (VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                                       IFX_uint16_t *pbuf, IFX_uint32_t len);
#elif ((VIN_ACCESS_MODE == VIN_ACCESS_MODE_INTEL_DEMUX))
IFX_void_t v2cpe_reg_read_intel_demux (VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                                 IFX_uint16_t *pbuf);
IFX_void_t v2cpe_reg_write_intel_demux (VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                                  IFX_uint16_t val);
IFX_void_t v2cpe_reg_readmulti_intel_demux (VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                                      IFX_uint16_t *pbuf, IFX_uint32_t len);
IFX_void_t v2cpe_reg_writemulti_intel_demux (VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                                       IFX_uint16_t *pbuf, IFX_uint32_t len);
#elif (VIN_ACCESS_MODE == VIN_ACCESS_MODE_SPI)
IFX_void_t v2cpe_read_spi  (VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                            IFX_uint16_t *pbuf, IFX_uint32_t len);
IFX_void_t v2cpe_write_spi (VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                            IFX_uint16_t *pbuf, IFX_uint32_t len);
#elif (VIN_ACCESS_MODE == VIN_ACCESS_MODE_EVALUATION)
IFX_void_t   v2cpe_read_eval (VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                              IFX_uint16_t *pbuf, IFX_uint32_t len);
IFX_void_t   v2cpe_write_eval(VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                              IFX_uint16_t *pbuf, IFX_uint32_t len);
#endif
#endif /* _DRV_VINETIC_ACCESS_H */
