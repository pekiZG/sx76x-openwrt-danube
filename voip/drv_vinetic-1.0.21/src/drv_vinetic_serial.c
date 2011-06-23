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
   Module      : drv_vinetic_serial.c
   Date        : 2005-03-02
   Description : This file implements the vinetic serial low level access via
                 SPI.
   Remarks     : This module requires availability of following defines:
                 1) SPI_MAXBYTES_SIZE
                 2) SPI_CS_SET
                 3) spi_ll_read_write

                 Use the file drv_vinetic_config_user.h to set these defines
                 for SPI

                 Before calling any function from this module, make sure that
                 the SPI interface was initialized, as this is not done by the
                 vinetic driver anymore.
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */

#include "drv_vinetic_api.h"
#include "drv_vinetic_serial.h"
#include "drv_vinetic_stream.h"

#ifdef VIN_SPI

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

/*******************************************************************************
Description:
   Write via serial SPI interface
Arguments:
   pDev -  pointer to the device structure
   pData - pointer to the write buffer
   count - number of words to write, must be greater than 0
Return:
    IFX_SUCCESS if successful, otherwise IFX_ERROR
*******************************************************************************/
IFX_int32_t serial_write (VINETIC_DEVICE *pDev, IFX_uint16_t *pData,
                          IFX_int32_t     count)
{
   IFX_int32_t ret = IFX_SUCCESS;
   IFX_uint32_t nSend = 0, len = 0;

   /* set CSQ = LOW */
   SPI_CS_SET(IFX_LOW);
   /* write all data */
   while ((nSend != (count * 2)) && (ret != IFX_ERROR))
   {
      if (((count * 2) - nSend) > SPI_MAXBYTES_SIZE)
         len = SPI_MAXBYTES_SIZE;
      else
         len = ((count * 2) - nSend);
      ret = spi_ll_read_write ((IFX_uint8_t *)&pData [nSend/2], len, NULL, 0);
      /* increase send count */
      nSend += ret;
   }
   /* Set CSQ = HIGH */
   SPI_CS_SET(IFX_HIGH);
   /* recover */
   IFXOS_DELAYUS (VINETIC_SPI_RECOVERY_TIME);

   if (ret != IFX_ERROR)
      ret = IFX_SUCCESS;

   return ret;
}

/*******************************************************************************
Description:
   read via serial SPI interface
Arguments:
   pDev -  pointer to the device structure
   cmd - short command which allow a specific reading
   pData - pointer to the read buffer
   count - number of words to read, must be greater than 0
Return:
    IFX_SUCCESS if successful, otherwise IFX_ERROR
*******************************************************************************/
IFX_int32_t serial_read (VINETIC_DEVICE *pDev,  IFX_uint16_t cmd,
                         IFX_uint16_t   *pData, IFX_int32_t  count)
{
   IFX_int32_t  ret = IFX_SUCCESS;
   IFX_uint16_t dummy = 0;
   IFX_uint32_t nRev = 0, len = 0;

   /* set CSQ = LOW */
   SPI_CS_SET(IFX_LOW);
   /* write command for reading and read dummy word */
   ret = spi_ll_read_write ((IFX_uint8_t *)&cmd, 2, (IFX_uint8_t *)&dummy, 2);
   /* read Data */
   while ((nRev != (count* 2)) && (ret != IFX_ERROR))
   {
      if (((count * 2) - nRev) > SPI_MAXBYTES_SIZE)
         len = SPI_MAXBYTES_SIZE;
      else
         len = ((count * 2) - nRev);
      ret = spi_ll_read_write (NULL, 0, (IFX_uint8_t *)&pData [nRev/2], len);
      /* increase receive count */
      nRev += ret;
   }
   /* Set CSQ = HIGH */
   SPI_CS_SET(IFX_HIGH);
   /* recover */
   IFXOS_DELAYUS (VINETIC_SPI_RECOVERY_TIME);

   if (ret != IFX_ERROR)
      ret = IFX_SUCCESS;

   return ret;
}

/*******************************************************************************
Description:
   Direct chip access for reading DIOP commands via serial SPI interface
Arguments:
   pDev  -  pointer to the device structure
   pCmd  - pointer to the write buffer, size 2 words
   pData - pointer to read Data
   count - number of words to read
Return:
    IFX_SUCCESS if successful, otherwise IFX_ERROR
*******************************************************************************/
IFX_int32_t serial_diop_read (VINETIC_DEVICE *pDev,  IFX_uint16_t *pCmd,
                              IFX_uint16_t   *pData, IFX_int32_t   count)
{
   IFX_int32_t  ret = IFX_SUCCESS;
   IFX_uint8_t  pSpiRead [SPI_MAXBYTES_SIZE];
   IFX_uint32_t nRev = 0;

   /* set CSQ = LOW */
   SPI_CS_SET(IFX_LOW);
   /* do read operation, considering dummy word in the size to read */
   nRev = spi_ll_read_write ((IFX_uint8_t *)pCmd, 4, pSpiRead,
                             ((count + 1) * 2));
   /* Set CSQ = HIGH */
   SPI_CS_SET(IFX_HIGH);
   /* Recovery time */
   IFXOS_DELAYUS (VINETIC_SPI_RECOVERY_TIME);
   /* copy data without dummy into user buffer */
   if (nRev == ((count+1) *2))
   {
	   memcpy(pData, &pSpiRead[2], (count * 2));
      /* cpb2w(pData, &pSpiRead[2], (count * 2)); */
   }
   else
      ret = IFX_ERROR;

   return ret;
}

#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
/*******************************************************************************
Description:
   Read all packets from packet outbox via serial SPI interface
Arguments:
   pDev - pointer to the device structure
   size - amount of data to read
Return:
  rest of data not read or IFX_ERROR
*******************************************************************************/
IFX_int32_t serial_read_packet (VINETIC_DEVICE *pDev, IFX_int32_t size)
{
   IFX_int32_t   ret = IFX_SUCCESS;
   IFX_uint8_t   packet_length = 0, len = 0;
   IFX_uint32_t  nRev=0;
   IFX_uint16_t  cmd, dummy, pPacketCmd [2] = {0},
                 pOverflow [MAX_PACKET_WORD] = {0},
                 *pRead = pOverflow;
   PACKET        *pPacket = NULL;
   FIFO          *pFifo = NULL;
   IFX_boolean_t bFifo = IFX_FALSE;

   /* write the packet read command */
   cmd = SC_RPOBX;
   /* set CSQ = LOW */
   SPI_CS_SET(IFX_LOW);
   /* write command and read dummy word */
   ret = spi_ll_read_write ((IFX_uint8_t *)&cmd, 2, (IFX_uint8_t *)&dummy, 2);
   /* read command words and data */
   while ((size > 0) && (ret != IFX_ERROR))
   {
      /* read the 2 command words */
      ret = spi_ll_read_write (NULL, 0, (IFX_uint8_t *)pPacketCmd, 4);
      if (ret == IFX_ERROR)
         break;
      /* get packet length and packet buffer */
      packet_length = pDev->getDataBuf(pDev, pPacketCmd, &pFifo, &pPacket);
      if (pPacket != NULL)
      {
         bFifo = IFX_TRUE;
         pRead = pPacket->pData;
      }
      /* read packet */
      for (ret = 0, nRev = 0; nRev < (packet_length * 2); nRev += ret)
      {
         if (((packet_length * 2) - nRev) > SPI_MAXBYTES_SIZE)
            len = SPI_MAXBYTES_SIZE;
         else
            len = ((packet_length * 2) - nRev);
         ret = spi_ll_read_write (NULL, 0, (IFX_uint8_t *)&pRead [nRev/2], len);
         if (ret == IFX_ERROR)
            break;
      }
      if (ret != IFX_ERROR)
      {
         /* remove complete packet size from size */
         size -= (2 + packet_length);
         /* put into fifo */
         if (bFifo == IFX_TRUE)
         {
            pDev->putDataBuf (pDev, pPacketCmd [0], &pFifo, &pPacket);
         }
      }
   }
   /* Set CSQ = HIGH */
   SPI_CS_SET(IFX_HIGH);
   /* Recovery time */
   IFXOS_DELAYUS (VINETIC_SPI_RECOVERY_TIME);

   if (ret != IFX_ERROR)
         ret = size;

   return ret;
}
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */

#endif /* VIN_SPI */

