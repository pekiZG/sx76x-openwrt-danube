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
   Module      : drv_vinetic_access.c
   Desription  : Implementation of low level access functions.
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_vinetic_api.h"

/* ============================= */
/* Local defines                 */
/* ============================= */

/* compile time check for SPI related defines */
#if ((VIN_ACCESS_MODE == VIN_ACCESS_MODE_SPI) || \
     (VIN_ACCESS_MODE == VIN_ACCESS_MODE_EVALUATION))
#if (!defined(SPI_MAXBYTES_SIZE) || !defined(SPI_CS_SET) ||\
     !defined(spi_ll_read_write))
#error Set SPI support macros in drv_user_config.h and compile again!
#endif /* SPI_MAXBYTES_SIZE| SPI_CS_SET | spi_ll_read_write */
#endif /* VIN_ACCESS_MODE == VIN_ACCESS_MODE_SPI */

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* ============================= */
/* Global variable definition    */
/* ============================= */

/* ============================= */
/* Local function definition     */
/* ============================= */

/* ============================= */
/* Global function definition    */
/* ============================= */

#if ((VIN_ACCESS_MODE == VIN_ACCESS_MODE_MOTOROLA) || \
     (VIN_ACCESS_MODE == VIN_ACCESS_MODE_EVALUATION))
/**
   Single register read access via motorola interface.

   \param pDev    - handle to device
   \param offset  - register offset
   \param pbuf    - read ptr (16bit)

   \remark
      Direct access for Registers of offset < 0x40.
      Indirect access for Registers of offset >= 0x40 via V2CPE_ADDR/V2CPE_DATA.
*/
IFX_void_t v2cpe_reg_read_motorola (VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                                  IFX_uint16_t *pbuf)
{
   if (offset < V2CPE_OFFSET_INDIRECT_ACCESS_MOTOROLA)
   {
      VIN_HOST_IF_PAR_READ (pDev, offset, pbuf);
   }
   else
   {
      VIN_HOST_IF_PAR_WRITE (pDev, V2CPE_ADDR, offset);
      VIN_HOST_IF_PAR_READ (pDev, V2CPE_DATA, pbuf);
   }

}

/**
   Single register write via motorola interface.

   \param pDev    - handle to device
   \param offset  - register offset
   \param val     - value to write (16bit)

   \remark
      Direct access for Registers of offset < 0x40.
      Indirect access for Registers of offset >= 0x40 via V2CPE_ADDR/V2CPE_DATA.
*/
IFX_void_t v2cpe_reg_write_motorola (VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                                   IFX_uint16_t val)
{
   if (offset < V2CPE_OFFSET_INDIRECT_ACCESS_MOTOROLA)
   {
      VIN_HOST_IF_PAR_WRITE (pDev, offset, val);
   }
   else
   {
      VIN_HOST_IF_PAR_WRITE (pDev, V2CPE_ADDR, offset);
      VIN_HOST_IF_PAR_WRITE (pDev, V2CPE_DATA, val);
   }
}

/**
   Multiple register read via motorola interface.

   \param pDev    - handle to device
   \param offset  - register offset
   \param pbuf    - read ptr (16bit)
   \param len     - number of data(16bit) to read

   \remark
      Direct access for Registers of offset < 0x40.
      Indirect access for Registers of offset >= 0x40 via V2CPE_ADDR/V2CPE_DATA.
      Assert for direct access, in case (offset + len) >= 0x40.
*/
IFX_void_t v2cpe_reg_readmulti_motorola (VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                                      IFX_uint16_t *pbuf, IFX_uint32_t len)
{
   if (offset < V2CPE_OFFSET_INDIRECT_ACCESS_MOTOROLA)
   {
      IFXOS_ASSERT((offset + len) < V2CPE_OFFSET_INDIRECT_ACCESS_MOTOROLA);
      VIN_HOST_IF_INCR_READ_MULTI (pDev, offset, pbuf, len);
   }
   else
   {
      VIN_HOST_IF_PAR_WRITE (pDev, V2CPE_ADDR, offset);
      VIN_HOST_IF_NOINCR_READ_MULTI (pDev, V2CPE_DATA, pbuf, len);
   }
}

/**
   Multiple register write via motorola interface.

   \param pDev    - handle to device
   \param offset  - register offset
   \param pbuf    - write ptr (16bit)
   \param len     - number of data(16bit) to write

   \remark
      Direct access for Registers of offset < 0x40.
      Indirect access for Registers of offset >= 0x40 via V2CPE_ADDR/V2CPE_DATA.
      Assert for direct access, in case (offset + len) >= 0x40.
*/
IFX_void_t v2cpe_reg_writemulti_motorola (VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                                       IFX_uint16_t *pbuf, IFX_uint32_t len)
{
   if (offset < V2CPE_OFFSET_INDIRECT_ACCESS_MOTOROLA)
   {
      IFXOS_ASSERT((offset + len) < V2CPE_OFFSET_INDIRECT_ACCESS_MOTOROLA);
      VIN_HOST_IF_INCR_WRITE_MULTI (pDev, offset, pbuf, len);
   }
   else
   {
      VIN_HOST_IF_PAR_WRITE (pDev, V2CPE_ADDR, offset);
      VIN_HOST_IF_NOINCR_WRITE_MULTI (pDev, V2CPE_DATA, pbuf, len);
   }
}

#endif /* VIN_ACCESS_MODE_MOTOROLA || VIN_ACCESS_MODE_EVALUATION */

#if ((VIN_ACCESS_MODE == VIN_ACCESS_MODE_INTEL_DEMUX) || \
     (VIN_ACCESS_MODE == VIN_ACCESS_MODE_EVALUATION))
/**
   Single register read access via intel demux interface.

   \param pDev    - handle to device
   \param offset  - register offset
   \param pbuf    - read ptr (16bit)

   \remark
      Direct access for Registers of offset < 0x20.
      Indirect access for Registers of offset >= 0x20 via V2CPE_ADDR/V2CPE_DATA.
*/
IFX_void_t v2cpe_reg_read_intel_demux (VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                                  IFX_uint16_t *pbuf)
{
   if (offset < V2CPE_OFFSET_INDIRECT_ACCESS_INTEL)
   {
      VIN_HOST_IF_PAR_READ (pDev, offset, pbuf);
   }
   else
   {
      VIN_HOST_IF_PAR_WRITE (pDev, V2CPE_ADDR, offset);
      VIN_HOST_IF_PAR_READ (pDev, V2CPE_DATA, pbuf);
   }

}

/**
   Single register write via intel demux interface.

   \param pDev    - handle to device
   \param offset  - register offset
   \param val     - value to write (16bit)

   \remark
      Direct access for Registers of offset < 0x20.
      Indirect access for Registers of offset >= 0x20 via V2CPE_ADDR/V2CPE_DATA.
*/
IFX_void_t v2cpe_reg_write_intel_demux (VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                                   IFX_uint16_t val)
{
   if (offset < V2CPE_OFFSET_INDIRECT_ACCESS_INTEL)
   {
      VIN_HOST_IF_PAR_WRITE (pDev, offset, val);
   }
   else
   {
      VIN_HOST_IF_PAR_WRITE (pDev, V2CPE_ADDR, offset);
      VIN_HOST_IF_PAR_WRITE (pDev, V2CPE_DATA, val);
   }
}

/**
   Multiple register read via intel demux interface.

   \param pDev    - handle to device
   \param offset  - register offset
   \param pbuf    - read ptr (16bit)
   \param len     - number of data(16bit) to read

   \remark
      Direct access for Registers of offset < 0x20.
      Indirect access for Registers of offset >= 0x20 via V2CPE_ADDR/V2CPE_DATA.
      Assert for direct access, in case (offset + len) >= 0x20.
*/
IFX_void_t v2cpe_reg_readmulti_intel_demux (VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                                            IFX_uint16_t *pbuf, IFX_uint32_t len)
{
   if (offset < V2CPE_OFFSET_INDIRECT_ACCESS_INTEL)
   {
      IFXOS_ASSERT((offset + len) < V2CPE_OFFSET_INDIRECT_ACCESS_INTEL);
      VIN_HOST_IF_INCR_READ_MULTI (pDev, offset, pbuf, len);
   }
   else
   {
      VIN_HOST_IF_PAR_WRITE (pDev, V2CPE_ADDR, offset);
      VIN_HOST_IF_NOINCR_READ_MULTI (pDev, V2CPE_DATA, pbuf, len);
   }
}

/**
   Multiple register write via intel demux interface.

   \param pDev    - handle to device
   \param offset  - register offset
   \param pbuf    - write ptr (16bit)
   \param len     - number of data(16bit) to write

   \remark
      Direct access for Registers of offset < 0x20.
      Indirect access for Registers of offset >= 0x20 via V2CPE_ADDR/V2CPE_DATA.
      Assert for direct access, in case (offset + len) >= 0x20.
*/
IFX_void_t v2cpe_reg_writemulti_intel_demux (VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                                       IFX_uint16_t *pbuf, IFX_uint32_t len)
{
   if (offset < V2CPE_OFFSET_INDIRECT_ACCESS_INTEL)
   {
      IFXOS_ASSERT((offset + len) < V2CPE_OFFSET_INDIRECT_ACCESS_INTEL);
      VIN_HOST_IF_INCR_WRITE_MULTI (pDev, offset, pbuf, len);
   }
   else
   {
      VIN_HOST_IF_PAR_WRITE (pDev, V2CPE_ADDR, offset);
      VIN_HOST_IF_NOINCR_WRITE_MULTI (pDev, V2CPE_DATA, pbuf, len);
   }
}

#endif /* VIN_ACCESS_MODE_INTEL_DEMUX || VIN_ACCESS_MODE_EVALUATION */

#if ((VIN_ACCESS_MODE == VIN_ACCESS_MODE_SPI) || \
     (VIN_ACCESS_MODE == VIN_ACCESS_MODE_EVALUATION))
/**
   Reads from VINETIC 2CPE via SPI interface

   \param   pDev    -  handle to device pointer
   \param   offset  -  host register(s) offset/start address
   \param   pbuf    -  data buffer in words (16-bit)
   \param   len     -  size of buffer (in words)

   \remark
      Caller takes care of protection.

*/
IFX_void_t v2cpe_read_spi    (VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                              IFX_uint16_t *pbuf, IFX_uint32_t len)
{
   IFX_uint16_t spi_cmd;
   IFX_int32_t ret, toread, read = 0, total = (len << 1);

   /* (len == 0) not allowed : assertion */
   IFXOS_ASSERT(len != 0);
   /* offset must be even, as we do 16bit access : assertion */
   IFXOS_ASSERT((offset & 0x1) == 0);
   /* set spi command in parameter 0 */
   spi_cmd = (V2CPE_SPI_CMD_R                                 |
             (pDev->hostDev.nSPIDevAddr << SPI_CMD_DEV_SHIFT) |
               offset);
   /* process according to len */
   switch (len)
   {
      case  1:
         /* read only 1 data word */
         toread = total;
         break;
      default:
         /* set auto increment if register isn't a box register */
         if ((offset != V2CPE_BOX_CDATA) &&
             (offset != V2CPE_BOX_VDATA))
         {
            spi_cmd |= V2CPE_SPI_CMD_I;
         }
         /* read 0 data word in the first go */
         toread = 0;
         break;
   }
   /* set CSQ = LOW */
   SPI_CS_SET (/*pDev->nDevNr,*/ IFX_LOW);
   /* access SPI : write command and read data if applicable */
   ret = spi_ll_read_write ((IFX_uint8_t*)&spi_cmd, sizeof(spi_cmd),
                            (IFX_uint8_t*)pbuf, toread);
   /* go out in case of error or (len == 1) */
   if ((ret == IFX_ERROR) || (len == 1))
      goto spi_end;
   /* while loop only if there are more than one word to read */
   while (read < total)
   {
      /* calculate rest of data */
      if ((total - read) > SPI_MAXBYTES_SIZE)
         toread = SPI_MAXBYTES_SIZE;
      else
         toread = total - read;
      /* read now : write ptr is NULL.
        Position in word buffer is 'read >> 1' because read is the number of
        byte already read and the buffer is a word buffer */
      ret = spi_ll_read_write (NULL, 0, (IFX_uint8_t*)&pbuf [read >> 1], toread);
      if (ret == IFX_ERROR)
         goto spi_end;
      /* increment read count */
      read += ret;
   }

spi_end:
   /* check error */
   if (ret == IFX_ERROR)
      SET_ERROR (ERR_HOSTREG_ACCESS);
   /* set CSQ = HIGH */
   SPI_CS_SET(/*pDev->nDevNr,*/IFX_HIGH);
}

/**
   Writes to VINETIC 2CPE via SPI interface

   \param   pDev    -  handle to device pointer
   \param   offset  -  host register(s) offset/start address
   \param   pbuf    -  data buffer in words (16-bit)
   \param   len     -  size of buffer (in words)

   \remark
      Caller takes care of protection.

*/
IFX_void_t v2cpe_write_spi   (VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                               IFX_uint16_t *pbuf, IFX_uint32_t len)
{
   IFX_uint16_t spi_cmd [2];
   IFX_int32_t ret, towrite, written = 0, total = (len << 1);


   /* (len == 0) not allowed : assertion */
   IFXOS_ASSERT(len != 0);
   /* offset must be even, as we do 16bit access : assertion */
   IFXOS_ASSERT((offset & 0x1) == 0);
   /* set spi command in parameter 0 */
   spi_cmd [0] = (V2CPE_SPI_CMD_W                                  |
                  (pDev->hostDev.nSPIDevAddr << SPI_CMD_DEV_SHIFT) |
                  offset);
   /* process according to len */
   switch (len)
   {
      case  1:
         /* write command word and data directly */
         spi_cmd [1] = pbuf [0];
         towrite = sizeof (spi_cmd);
         break;
      default:
         /* set auto increment if register isn't a box register */
         if ((offset != V2CPE_BOX_CDATA) &&
             (offset != V2CPE_BOX_VDATA))
         {
            spi_cmd [0] |= V2CPE_SPI_CMD_I;
         }
         /* write only SPI command word in the first go */
         towrite = sizeof (IFX_uint16_t);
         break;
   }

   /* set CSQ = LOW */
   SPI_CS_SET (/*pDev->nDevNr,*/IFX_LOW);
   /* access SPI for writing  */
   ret = spi_ll_read_write ((IFX_uint8_t*)&spi_cmd, towrite, NULL, 0);
   /* go out in case of error or (len == 1) */
   if ((ret == IFX_ERROR) || (len == 1))
      goto spi_end;
   /* while loop only if there are more than one word to write */
   while (written < total)
   {
      /* calculate rest of data */
      if ((total - written) > SPI_MAXBYTES_SIZE)
         towrite = SPI_MAXBYTES_SIZE;
      else
         towrite = total - written;
      /* write now : read ptr is NULL
        Position in word buffer is 'written >> 1' because read is the number of
        byte already written and the buffer is a word buffer
      */
      ret = spi_ll_read_write ((IFX_uint8_t*)&pbuf [written >> 1], towrite,
                                NULL, 0);
      if (ret == IFX_ERROR)
         goto spi_end;
      /* increment written count */
      written += ret;
   }

spi_end:
   /* check error */
   if (ret == IFX_ERROR)
      SET_ERROR (ERR_HOSTREG_ACCESS);
   /* set CSQ = HIGH */
   SPI_CS_SET(/*pDev->nDevNr,*/IFX_HIGH);
}

#endif /* VIN_ACCESS_MODE_SPI) || VIN_ACCESS_MODE_EVALUATION */

/**
   Byte Swap inside VINETIC.

   \param   pDev    -  handle to device pointer
*/
IFX_void_t v2cpe_byte_swap (VINETIC_DEVICE *pDev)
{
#if (defined (EVALUATION) || defined (VIN_BYTE_SWAP))
   IFX_uint16_t nVal;
#endif /* EVALUATION | VIN_BYTE_SWAP */

#ifndef EVALUATION
#ifdef VIN_BYTE_SWAP
   REG_READ_PROT (pDev, V2CPE_GLB_CFG, &nVal);
   V2CPE_GLB_CFG_END_MD_SET (nVal, 1);
   REG_WRITE_PROT (pDev, V2CPE_GLB_CFG, nVal);
#endif /* VIN_BYTE_SWAP */
#else
   switch (pDev->hostDev.nAccessMode)
   {
      case VIN_ACCESS_PARINTEL_MUX8:
      case VIN_ACCESS_PARINTEL_DMUX8:
         REG_READ_PROT (pDev, V2CPE_GLB_CFG, &nVal);
         V2CPE_GLB_CFG_END_MD_SET (nVal, 1);
         REG_WRITE_PROT (pDev, V2CPE_GLB_CFG, nVal);
         TRACE (VINETIC, DBG_LEVEL_HIGH,
               ("drv_vinetic switching endianess in VINETIC \n\r"));
         break;
      default:
         /* do nothing */
         break;
   }
#endif /* EVALUATION */
}


#if  (VIN_ACCESS_MODE == VIN_ACCESS_MODE_EVALUATION)
/**
   Reads from VINETIC 2CPE via interface set by access mode parameter.

   \param   pDev    -  handle to device pointer
   \param   offset  -  host register(s) offset/start address
   \param   pbuf    -  data buffer in words (16-bit)
   \param   len     -  size of buffer (in words)

   \remark
      Caller takes care of protection.
      This evaluation function implements all access modes.
*/
IFX_void_t v2cpe_read_eval (VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                            IFX_uint16_t *pbuf, IFX_uint32_t len)
{
   switch (pDev->hostDev.nAccessMode)
   {
      case VIN_ACCESS_PAR_8BIT:
         switch (offset)
         {
            case  V2CPE_BOX_CDATA:
            case  V2CPE_BOX_VDATA:
               /* mailbox */
               /* no offset increment: All data written to same address */
               VIN_HOST_IF_NOINCR_READ_MULTI (pDev, offset, pbuf, len);
               break;
            default:
               /* registers */
               /* offset increment for offset < 0x40.
                  Autoincrement for offset >= 0x40
               */
               if (len == 1)
               {
                  v2cpe_reg_read_motorola (pDev, offset, pbuf);
               }
               else
               {
                  /* offset incremented ins case of multiple write */
                  v2cpe_reg_readmulti_motorola (pDev, offset, pbuf, len);
               }
               break;
         }
         break;

      case VIN_ACCESS_PARINTEL_MUX8:
         /* direct host access */
         switch (offset)
         {
            case  V2CPE_BOX_CDATA:
            case  V2CPE_BOX_VDATA:
               /* mailbox */
               /* no offset increment: All data written to same address */
               VIN_HOST_IF_NOINCR_READ_MULTI (pDev, offset, pbuf, len);
               break;
            default:
               /* registers */
               if (len == 1)
               {
                  VIN_HOST_IF_PAR_READ(pDev, offset, pbuf);
               }
               else
               {
                  /* offset incremented ins case of multiple write */
                  VIN_HOST_IF_INCR_READ_MULTI(pDev, offset, pbuf, len);
               }
               break;
         }
         break;

      case VIN_ACCESS_PARINTEL_DMUX8:
         /* indirect access for offset >= 0x20 */
         switch (offset)
         {
            case  V2CPE_BOX_CDATA:
            case  V2CPE_BOX_VDATA:
               /* mailbox */
               /* no offset increment: All data written to same address */
               VIN_HOST_IF_NOINCR_READ_MULTI((pDev),offset,(pbuf),(len));
               break;
            default:
               /* registers */
               /* offset increment for offset < 0x20.
                  Autoincrement for offset >= 0x20
               */
               if (len == 1)
               {
                  v2cpe_reg_read_intel_demux (pDev, offset, pbuf);
               }
               else
               {
                  /* offset incremented ins case of multiple write */
                  v2cpe_reg_readmulti_intel_demux (pDev, offset, pbuf, len);
               }
               break;
         }
         break;

      case  VIN_ACCESS_SPI:
        v2cpe_read_spi (pDev, offset, pbuf, len);
         break;

      default:
         pDev->err = ERR_HOSTREG_ACCESS;
         break;
   }
}

/**
   Writes to VINETIC 2CPE via interface set by access mode parameter.

   \param   pDev    -  handle to device pointer
   \param   offset  -  host register(s) offset/start address
   \param   pbuf    -  data buffer in words (16-bit)
   \param   len     -  size of buffer (in words)

   \remark
      Caller takes care of protection.
      This evaluation function implements all access modes.
*/
IFX_void_t v2cpe_write_eval (VINETIC_DEVICE *pDev, IFX_uint8_t offset,
                             IFX_uint16_t *pbuf, IFX_uint32_t len)
{
   switch (pDev->hostDev.nAccessMode)
   {
      case VIN_ACCESS_PAR_8BIT:
         /* indirect access for offset >= 0x40 */
         switch (offset)
         {
            case  V2CPE_BOX_CDATA:
            case  V2CPE_BOX_VDATA:
               /* no offset increment: All data written to same address */
               VIN_HOST_IF_NOINCR_WRITE_MULTI(pDev, offset, pbuf, len);
               break;
            default:
               /* offset increment for offset < 0x40.
                  Autoincrement for offset >= 0x40 */
               if (len == 1)
               {
                  v2cpe_reg_write_motorola (pDev, offset, *pbuf);
               }
               else
               {
                  /* offset incremented ins case of multiple write */
                  v2cpe_reg_writemulti_motorola (pDev, offset, pbuf, len);
               }
               break;
         }
         break;

      case VIN_ACCESS_PARINTEL_MUX8:
         /* direct host access */
         switch (offset)
         {
            case  V2CPE_BOX_CDATA:
            case  V2CPE_BOX_VDATA:
               /* no offset increment: All data written to same address */
               VIN_HOST_IF_NOINCR_WRITE_MULTI(pDev, offset, pbuf, len);
               break;
            default:
               if (len == 1)
               {
                  VIN_HOST_IF_PAR_WRITE(pDev, offset, *pbuf);
               }
               else
               {
                  /* offset incremented ins case of multiple write */
                  VIN_HOST_IF_INCR_WRITE_MULTI(pDev, offset, pbuf, len);
               }
               break;
         }
         break;

      case  VIN_ACCESS_PARINTEL_DMUX8:
         /* indirect access for offset >= 0x20 */
         switch (offset)
         {
            case  V2CPE_BOX_CDATA:
            case  V2CPE_BOX_VDATA:
               /* no offset increment: All data written to same address */
               VIN_HOST_IF_NOINCR_WRITE_MULTI(pDev, offset, pbuf, len);
               break;
            default:
               /* offset increment for offset < 0x20.
                  Autoincrement for offset >= 0x20 */
               if (len == 1)
               {
                  v2cpe_reg_write_intel_demux (pDev, offset, *pbuf);
               }
               else
               {
                  /* offset incremented ins case of multiple write */
                  v2cpe_reg_writemulti_intel_demux (pDev, offset, pbuf, len);
               }
               break;
         }
         break;

      case  VIN_ACCESS_SPI:
        v2cpe_write_spi (pDev, offset, pbuf, len);
         break;

      default:
         pDev->err = ERR_HOSTREG_ACCESS;
         break;
   }
}

#endif /*  VIN_ACCESS_MODE_EVALUATION */
