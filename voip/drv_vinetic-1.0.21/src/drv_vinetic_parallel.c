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
   Module      : drv_vinetic_parallel.c
   Description : This file contains the implementation of the functions need
                 to access VINETIC through parallel interface 8 bit and 16 bit.
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */

#include "drv_vinetic_api.h"
#include "drv_vinetic_parallel.h"

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

#ifndef PARACCESS_CS_RECOVER
#define PARACCESS_CS_RECOVER    do { /* empty */ } while (0)
#endif

/* ============================= */
/* Global variable definition    */
/* ============================= */

/* ============================= */
/* Global function declaration   */
/* ============================= */

/* ============================= */
/* Local function declaration    */
/* ============================= */

/* ============================= */
/* Local variable definition     */
/* ============================= */
/* ready bit polling counter */
#ifdef VIN_V14_SUPPORT
IFX_LOCAL IFX_int32_t cnt = 0;
#endif /* VIN_V14_SUPPORT */

/* Variable to read dummy values from the Vinetic. This
   is declared as global to avoid the compiler to remove
   it by setting the optimization */
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
IFX_uint16_t gOverflow = 0;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */

/* ============================= */
/* Local function definition     */
/* ============================= */

/* ============================= */
/* Global function definition    */
/* ============================= */

/*******************************************************************************
Description:
   read from parallel port for 16 bit
Arguments:
   pDev -  pointer to the device structure
   cmd - short command which allow a specific reading
   pData - pointer to the read buffer
   count - number of words to read, must be greater than 0
Return:
    IFX_SUCCESS if successful, otherwise IFX_ERROR
Remark:
   An error occurs in this function only if the RDQY bit isn't set in IR
*******************************************************************************/
IFX_int32_t parallel_read  (VINETIC_DEVICE *pDev,  IFX_uint16_t cmd,
                            IFX_uint16_t   *pData, IFX_int32_t  count)
{
   IFX_vuint16_t *pRead;
#ifdef VIN_V14_SUPPORT
   /* set interrupt register pointer */
   IFX_vuint16_t* pIr = pDev->pIntReg;
#endif /* VIN_V14_SUPPORT */

   IFXOS_ASSERT (count > 0);
   WAIT_FOR_READY_FLAG;
   /* write 1. word at VINETIC_NWD */
   *(pDev->pNwd) = cmd;
   pRead = pData;

   while (count > 1)
   {
      WAIT_FOR_READY_FLAG;
      /* read data at address NWD */
      *pRead = *(pDev->pNwd);
      /* set next data */
      count--;
      pRead++;
      PARACCESS_CS_RECOVER;
   }
   WAIT_FOR_READY_FLAG;
   /* read last word at EOM */
   *pRead = *(pDev->pEom);

   return IFX_SUCCESS;

#ifdef VIN_V14_SUPPORT
ACCESS_ERR:
   return IFX_ERROR;
#endif /* VIN_V14_SUPPORT */
}

/*******************************************************************************
Description:
   Write to parallel port for 16 bit
Arguments:
   pDev -  pointer to the device structure
   pData - pointer to the write buffer
   count - number of words to write, must be greater than 0
Return:
    IFX_SUCCESS if successful, otherwise IFX_ERROR
Remark:
   An error occurs in this function only if the RDQY bit isn't set in IR
*******************************************************************************/
IFX_int32_t parallel_write(VINETIC_DEVICE *pDev, IFX_uint16_t *pData,
                           IFX_int32_t count)
{
   IFX_uint16_t *pSource;
#ifdef VIN_V14_SUPPORT
   /* set interrupt register pointer */
   IFX_vuint16_t* pIr = pDev->pIntReg;
#endif /* VIN_V14_SUPPORT */

   IFXOS_ASSERT (count > 0);
   pSource = pData;
   while (count > 1)
   {
      WAIT_FOR_READY_FLAG;
      /* write data at NWD */
      *(pDev->pNwd) = *pSource;
      /* set next data */
      count--;
      pSource++;
      PARACCESS_CS_RECOVER;
   }
   WAIT_FOR_READY_FLAG;
   /* write last data word at EOM */
   *(pDev->pEom) = *pSource;
   return IFX_SUCCESS;

#ifdef VIN_V14_SUPPORT
ACCESS_ERR:
    return IFX_ERROR;
#endif /* VIN_V14_SUPPORT */
}

#ifdef VIN_8BIT
/*******************************************************************************
Description:
   read from parallel port for 8 bit
Arguments:
   pDev -  pointer to the device structure
   cmd - short command which allow a specific reading
   pData - pointer to the read buffer
   count - number of words to read
Return:
    IFX_SUCCESS if successful, otherwise IFX_ERROR
Remark:
   An error occurs in this function only if the RDQY bit isn't set in IR
*******************************************************************************/
IFX_int32_t par8bit_read  (VINETIC_DEVICE *pDev,  IFX_uint16_t cmd,
                           IFX_uint16_t   *pData, IFX_int32_t  count)
{
   IFX_vuint16_t *pNWD;
   IFX_vuint16_t *pRead;
   IFX_uint16_t data;
#ifdef VIN_V14_SUPPORT
   IFX_vuint16_t* pIr = (IFX_uint16_t*)pDev->pIntReg;
#endif /* VIN_V14_SUPPORT */

   /* cache NWD address */
   pNWD = (IFX_vuint16_t*) pDev->pNwd;
   /*wait the ready signal */
   WAIT_FOR_READY_FLAG;

   /* write 1. word at NWD */
   data = (IFX_uint16_t)((cmd & 0xff00) >> 8);
   *pNWD = data;
   PARACCESS_CS_RECOVER;
   WAIT_FOR_READY_FLAG;
   data = (IFX_uint16_t) (cmd & 0x00ff);
   *pNWD = data;

   pRead = pData;
   /* read more than 1 word, so first read at NWD until last word */
   while (count > 1)
   {
      PARACCESS_CS_RECOVER;
      /*wait the ready signal */
      WAIT_FOR_READY_FLAG;
      /* read high byte at NWD */
      data = (*pNWD);
      *pRead = data << 8;
      PARACCESS_CS_RECOVER;
      WAIT_FOR_READY_FLAG;
      /* read low byte at NWD */
      data = (*pNWD);
      *pRead |= (data & 0x00FF);
      /* set next data */
      count--;
      pRead++;
   }
   PARACCESS_CS_RECOVER;
   /* read last word */
   WAIT_FOR_READY_FLAG;
   data = (*pNWD);
   *pRead = data << 8;
   PARACCESS_CS_RECOVER;
   WAIT_FOR_READY_FLAG;
   data = *(pDev->pEom);
   *pRead |= (data & 0x00FF);
   return IFX_SUCCESS;
#ifdef VIN_V14_SUPPORT

ACCESS_ERR:
   return IFX_ERROR;
#endif /* VIN_V14_SUPPORT */
}

/*******************************************************************************
Description:
   write to parallel port
Arguments:
   pDev -  pointer to the device structure
   pData - pointer to the write buffer
   count - number of words to write
Return:
    IFX_SUCCESS if successful, otherwise IFX_ERROR
Remark:
   An error occurs in this function only if the RDQY bit isn't set in IR
*******************************************************************************/
IFX_int32_t par8bit_write(VINETIC_DEVICE *pDev, IFX_uint16_t *pData,
                          IFX_int32_t     count)
{
   IFX_vuint16_t *pNWD;
   IFX_uint16_t *pSource, data;
#ifdef VIN_V14_SUPPORT
   IFX_vuint16_t* pIr = (IFX_uint16_t*) pDev->pIntReg;
#endif /* VIN_V14_SUPPORT */

   pSource = pData;
   /* cache NWD address */
   pNWD = (IFX_vuint16_t*) pDev->pNwd;
   while (count > 1)
   {
      /* write high byte at NWD */
      data = (((*pSource) & 0xff00) >> 8);
      PARACCESS_CS_RECOVER;
      WAIT_FOR_READY_FLAG;
      *pNWD = data;
      /* write low byte at NWD */
      data = (IFX_uint16_t)(*pSource & 0x00FF);
      PARACCESS_CS_RECOVER;
      WAIT_FOR_READY_FLAG;
      *pNWD = data;
      /* set next data */
      count--;
      pSource++;
   }
   /* write last word */
   data = (((*pSource) & 0xff00) >> 8);
   PARACCESS_CS_RECOVER;
   WAIT_FOR_READY_FLAG;
   *pNWD = data;
   /* write last low byte at NWD */
   data = (IFX_uint16_t)(*pSource & 0x00FF);
   PARACCESS_CS_RECOVER;
   WAIT_FOR_READY_FLAG;
   *(pDev->pEom) = data;

   return IFX_SUCCESS;

#ifdef VIN_V14_SUPPORT
ACCESS_ERR:
   return IFX_ERROR;

#endif /* VIN_V14_SUPPORT */
}

/*******************************************************************************
Description:
   Direct chip access for reading DIOP commands in 8 bit mode
Arguments:
   pDev -  pointer to the device structure
   pCmd: pointer to the write buffer, size 2 words
   pData - pointer to read Data
   count - number of words to read
Return:
    IFX_SUCCESS if successful, otherwise IFX_ERROR
Remarks:
   An error occurs in this function only if the RDQY bit isn't set in ISR.
   Diop commands are two words to write and a particular word count
   immediately to read. This makes it differenct from usual commands.
*******************************************************************************/
IFX_int32_t par8bit_diop_read (VINETIC_DEVICE *pDev,  IFX_uint16_t *pCmd,
                               IFX_uint16_t   *pData, IFX_int32_t   count)
{
   IFX_vuint16_t *pNWD;
   IFX_vuint16_t *pRead;
   IFX_uint16_t data;
#ifdef VIN_V14_SUPPORT
   IFX_vuint16_t* pIr = (IFX_uint16_t*)pDev->pIntReg;
#endif /* VIN_V14_SUPPORT */

   /* cache NWD address */
   pNWD = (IFX_vuint16_t*) pDev->pNwd;
   WAIT_FOR_READY_FLAG;

   /* write first word at NWD */
   data = (IFX_uint16_t)((pCmd[0] & 0xff00) >> 8);
   *pNWD = data;
   WAIT_FOR_READY_FLAG;
   PARACCESS_CS_RECOVER;
   /* now the low byte */
   data = (IFX_uint16_t) (pCmd[0] & 0x00ff);
   *pNWD = data;
   WAIT_FOR_READY_FLAG;
   PARACCESS_CS_RECOVER;
   /* write second word at NWD */
   data = (IFX_uint16_t)((pCmd[1] & 0xff00) >> 8);
   *pNWD = data;
   WAIT_FOR_READY_FLAG;
   PARACCESS_CS_RECOVER;
   data = (IFX_uint16_t) (pCmd[1] & 0x00ff);
   *pNWD = data;

   /* read at eom if just one data to read */
   pRead = pData;
   /* read more than 1 word, so first read at NWD until last word */
   while (count > 1)
   {
      WAIT_FOR_READY_FLAG;
      PARACCESS_CS_RECOVER;
      /* read high byte at NWD */
      data = (*pNWD);
      *pRead = data << 8;
      WAIT_FOR_READY_FLAG;
      PARACCESS_CS_RECOVER;
      /* read low byte at NWD */
      data = (*pNWD);
      *pRead |= (data & 0x00FF);
      /* set next data */
      count--;
      pRead++;
   }
   /* read the last word */
   WAIT_FOR_READY_FLAG;
   PARACCESS_CS_RECOVER;
   data = (*pNWD);
   *pRead = data << 8;
   WAIT_FOR_READY_FLAG;
   PARACCESS_CS_RECOVER;
   /* now the last low byte at EOM */
   data = *(pDev->pEom);
   *pRead |= (data & 0x00FF);
   return IFX_SUCCESS;
#ifdef VIN_V14_SUPPORT

ACCESS_ERR:
   return IFX_ERROR;
#endif /* VIN_V14_SUPPORT */
}


#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
/*******************************************************************************
Description:
   Read all packets from packet outbox in 8bit mode
Arguments:
   pDev - pointer to the device structure
   size - amount of data to read
Return:
  rest of data not read. Should be 0
Remarks:
  All data is read. If not, there was a failure somewhere. The calling function
  must ensure that all data is read
  An error occurs in this function only if the RDQY bit isn't set in IR
*******************************************************************************/
IFX_int32_t par8bit_packet_read (VINETIC_DEVICE *pDev, IFX_int32_t size)
{
   IFX_vuint16_t *pNWD = NULL;
   IFX_uint16_t packet_length = 0, i = 0;
   /* buffer for first 2 packet commands */
   IFX_uint16_t pPacketCmd [CMD_HEADER_CNT] = {0};
   /* buffer handler */
   PACKET *pPacket = NULL;
   /* Fifo handler */
   FIFO *pFifo = NULL;
#ifdef VIN_V14_SUPPORT
   /* variable to write if no fifo buffer pointer available */
   IFX_vuint16_t* pIr = pDev->pIntReg;
#endif /* VIN_V14_SUPPORT */

   /* set pNWD and pEOM */
   pNWD = (IFX_vuint16_t*) pDev->pNwd;

   /* write POBX (packet outbox) read cmd */
   WAIT_FOR_READY_FLAG;
   /* write high byte at NWD */
   *pNWD = (IFX_uint16_t)((SC_RPOBX & 0xff00) >> 8);
   WAIT_FOR_READY_FLAG;
   /* write low byte at NWD */
   *pNWD = (IFX_uint16_t) (SC_RPOBX & 0x00ff);

   while (size > 0)
   {
      /* read command header words */
      for (i = 0; i < CMD_HEADER_CNT; i++)
      {
         WAIT_FOR_READY_FLAG;
         /* read high byte at NWD */
         pPacketCmd [i] = (*pNWD) << 8;
         WAIT_FOR_READY_FLAG;
         /* read low byte at NWD */
         pPacketCmd [i] |= (*pNWD & 0x00FF);
      }
      /* decrease the rest word counter */
      size -= CMD_HEADER_CNT;
      IFXOS_ASSERT((pPacketCmd [0] & CMD1_CH) < 8);

      /* analyze header to get data length and data ptr to store data  */
      packet_length = pDev->getDataBuf (pDev, pPacketCmd, &pFifo, &pPacket);
      IFXOS_ASSERT (packet_length <= size);
      if (pPacket == NULL)
      {
         /* if no buffer, throw data away */
         for (i = 0; i < packet_length; i ++)
         {
            WAIT_FOR_READY_FLAG;
            /* read high byte at NWD */
            gOverflow = (*pNWD);
            WAIT_FOR_READY_FLAG;
            if (size == 1)
               /* read low IFX_uint16_t at NWD */
               gOverflow = *(pDev->pEom);
            else
               gOverflow = *pNWD;
            --size;
         }
      }
      else
      {
         /* read data and store to fifo buffer */
         for (i = 0; i < packet_length; i ++)
         {
            WAIT_FOR_READY_FLAG;
            pPacket->pData [i] = (*pNWD) << 8;
            WAIT_FOR_READY_FLAG;
            if (size == 1)
               pPacket->pData [i] |= *(pDev->pEom) & 0x00FF;
            else
               pPacket->pData [i] |= (*pNWD & 0x00FF);
            /* decrease remaining count */
            --size;
         }
         /* set Fifo ptr */
         pDev->putDataBuf (pDev, pPacketCmd [0], &pFifo, &pPacket);
      }
   }

   return size;

#ifdef VIN_V14_SUPPORT
ACCESS_ERR:
   return IFX_ERROR;
#endif /* VIN_V14_SUPPORT */
}
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */

/*******************************************************************************
Description:
   read from parallel port for 8 bit
Arguments:
   pDev -  pointer to the device structure
   cmd - short command which allow a specific reading
   pData - pointer to the read buffer
   count - number of words to read, must be greater than 0
Return:
    IFX_SUCCESS if successful, otherwise IFX_ERROR
Remark:
   An error occurs in this function only if the RDQY bit isn't set in IR
*******************************************************************************/
IFX_int32_t parint8bit_read  (VINETIC_DEVICE *pDev,  IFX_uint16_t cmd,
                              IFX_uint16_t   *pData, IFX_int32_t  count)
{
   IFX_vuint16_t *pNWD;
   IFX_vuint16_t *pRead;
   IFX_uint16_t data;
#ifdef VIN_V14_SUPPORT
   IFX_vuint16_t* pIr = (IFX_uint16_t*)pDev->pIntReg;
#endif /* VIN_V14_SUPPORT */

   IFXOS_ASSERT (count > 0);
   /* cache NWD address */
   pNWD = (IFX_vuint16_t*) pDev->pNwd;
   /*wait the ready signal */
   WAIT_FOR_READY_FLAG;

   /* write 1. word at NWD */
   data = (IFX_uint16_t) (cmd & 0x00ff);
   *pNWD = data;
   PARACCESS_CS_RECOVER;
   WAIT_FOR_READY_FLAG;
   data = (IFX_uint16_t)((cmd & 0xff00) >> 8);
   *pNWD = data;

   pRead = pData;
   /* read more than 1 word, so first read at NWD until last word */
   while (count > 1)
   {
      PARACCESS_CS_RECOVER;
      /*wait the ready signal */
      WAIT_FOR_READY_FLAG;
      /* read high byte at NWD */
      data = (*pNWD);
      *pRead = (data & 0x00FF);
      PARACCESS_CS_RECOVER;
      WAIT_FOR_READY_FLAG;
      /* read low byte at NWD */
      data = (*pNWD);
      *pRead |= (data & 0x00FF) << 8;
      /* set next data */
      count--;
      pRead++;
   }
   PARACCESS_CS_RECOVER;
   /* read last word */
   WAIT_FOR_READY_FLAG;
   data = (*pNWD);
   *pRead = (data & 0x00FF);
   PARACCESS_CS_RECOVER;
   WAIT_FOR_READY_FLAG;
   data = *(pDev->pEom);
   *pRead |= (data & 0x00FF) << 8;
   return IFX_SUCCESS;

#ifdef VIN_V14_SUPPORT
ACCESS_ERR:
   return IFX_ERROR;
#endif /* VIN_V14_SUPPORT */
}

/*******************************************************************************
Description:
   write to parallel port
Arguments:
   pDev -  pointer to the device structure
   pData - pointer to the write buffer
   count - number of words to write, must be greater than 0
Return:
    IFX_SUCCESS if successful, otherwise IFX_ERROR
Remark:
   An error occurs in this function only if the RDQY bit isn't set in IR
*******************************************************************************/
IFX_int32_t parint8bit_write(VINETIC_DEVICE *pDev, IFX_uint16_t *pData,
                             IFX_int32_t     count)
{
   IFX_vuint16_t *pNWD;
   IFX_uint16_t *pSource, data;

   IFXOS_ASSERT (count > 0);
   pSource = pData;
   /* cache NWD address */
   pNWD = (IFX_vuint16_t*) pDev->pNwd;
   while (count > 1)
   {
      /* write low byte at NWD */
      data = (IFX_uint16_t)(*pSource & 0x00FF);
      PARACCESS_CS_RECOVER;
      *pNWD = data;
      /* write high byte at NWD */
      data = (((*pSource) & 0xff00) >> 8);
      PARACCESS_CS_RECOVER;
      *pNWD = data;
      /* set next data */
      count--;
      pSource++;
   }
   /* write last word */
   data = (IFX_uint16_t)(*pSource & 0x00FF);
   PARACCESS_CS_RECOVER;
   *pNWD = data;
   /* write last high byte at NWD */
   data = (((*pSource) & 0xff00) >> 8);
   PARACCESS_CS_RECOVER;
   *(pDev->pEom) = data;

   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   Direct chip access for reading DIOP commands in 8 bit mode
Arguments:
   pDev -  pointer to the device structure
   pCmd: pointer to the write buffer, size 2 words
   pData - pointer to read Data
   count - number of words to read
Return:
    IFX_SUCCESS if successful, otherwise IFX_ERROR
Remarks:
   An error occurs in this function only if the RDQY bit isn't set in ISR.
   Diop commands are two words to write and a particular word count
   immediately to read. This makes it differenct from usual commands.
*******************************************************************************/
IFX_int32_t parint8bit_diop_read (VINETIC_DEVICE *pDev, IFX_uint16_t *pCmd,
                                  IFX_uint16_t   *pData, IFX_int32_t  count)
{
   IFX_vuint16_t *pNWD;
   IFX_vuint16_t *pRead;
   IFX_uint16_t data;

   /* cache NWD address */
   pNWD = (IFX_vuint16_t*) pDev->pNwd;

   /* write first word at NWD */
   data = (IFX_uint16_t) (pCmd[0] & 0x00ff);
   *pNWD = data;
   PARACCESS_CS_RECOVER;
   /* now the low byte */
   data = (IFX_uint16_t)((pCmd[0] & 0xff00) >> 8);
   *pNWD = data;
   PARACCESS_CS_RECOVER;
   /* write second word at NWD */
   data = (IFX_uint16_t) (pCmd[1] & 0x00ff);
   *pNWD = data;
   PARACCESS_CS_RECOVER;
   data = (IFX_uint16_t)((pCmd[1] & 0xff00) >> 8);
   *pNWD = data;

   /* read at eom if just one data to read */
   pRead = pData;
   /* read more than 1 word, so first read at NWD until last word */
   while (count > 1)
   {
      PARACCESS_CS_RECOVER;
      /* read high byte at NWD */
      data = (*pNWD);
      *pRead = (data & 0x00FF);
      PARACCESS_CS_RECOVER;
      /* read low byte at NWD */
      data = (*pNWD);
      *pRead |= (data & 0x00FF) << 8;
      /* set next data */
      count--;
      pRead++;
   }
   /* read the last word */
   PARACCESS_CS_RECOVER;
   data = (*pNWD);
   *pRead = (data & 0x00FF);
   PARACCESS_CS_RECOVER;
   /* now the last low byte at EOM */
   data = *(pDev->pEom);
   *pRead |= (data & 0x00FF) << 8;
   return IFX_SUCCESS;
}


#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
/*******************************************************************************
Description:
   Read all packets from packet outbox in 8bit mode
Arguments:
   pDev - pointer to the device structure
   size - amount of data to read
Return:
  rest of data not read. Should be 0
Remarks:
  All data is read. If not, there was a failure somewhere. The calling function
  must ensure that all data is read
  An error occurs in this function only if the RDQY bit isn't set in IR
*******************************************************************************/
IFX_int32_t parint8bit_packet_read (VINETIC_DEVICE *pDev, IFX_int32_t size)
{
   IFX_vuint16_t *pNWD = NULL;
   IFX_uint16_t packet_length = 0, i = 0;
   /* buffer for first 2 packet commands */
   IFX_uint16_t pPacketCmd [CMD_HEADER_CNT] = {0};
   /* buffer handler */
   PACKET *pPacket = NULL;
   /* Fifo handler */
   FIFO *pFifo = NULL;

   /* set pNWD and pEOM */
   pNWD = (IFX_vuint16_t*) pDev->pNwd;

   /* write POBX (packet outbox) read cmd */
   /* write low byte at NWD */
   *pNWD = (IFX_uint16_t) (SC_RPOBX & 0x00ff);
   /* write high byte at NWD */
   *pNWD = (IFX_uint16_t)((SC_RPOBX & 0xff00) >> 8);

   while (size > 0)
   {
      /* read command header words */
      for (i = 0; i < CMD_HEADER_CNT; i++)
      {
         /* read low byte at NWD */
         pPacketCmd [i] = (*pNWD & 0x00FF);
         /* read high byte at NWD */
         pPacketCmd [i] |= (((*pNWD) << 8) & 0xFF00);
      }
      /* decrease the rest word counter */
      size -= CMD_HEADER_CNT;
      IFXOS_ASSERT((pPacketCmd [0] & CMD1_CH) < 8);

      /* analyze header to get data length and data ptr to store data  */
      packet_length = pDev->getDataBuf (pDev, pPacketCmd, &pFifo, &pPacket);
      IFXOS_ASSERT (packet_length <= size);
      if (pPacket == NULL)
      {
         /* if no buffer, throw data away */
         for (i = 0; i < packet_length; i ++)
         {
            /* read high byte at NWD */
            gOverflow = (*pNWD);
            if (size == 1)
               /* read low Word at NWD */
               gOverflow = *(pDev->pEom);
            else
               gOverflow = *pNWD;
            --size;
         }
      }
      else
      {
         /* read data and store to fifo buffer */
         for (i = 0; i < packet_length; i ++)
         {
            pPacket->pData [i] = (*pNWD) & 0x00FF;
            if (size == 1)
               pPacket->pData [i] |= (((*pDev->pEom) << 8) & 0xFF00);
            else
               pPacket->pData [i] |= (((*pNWD) << 8) & 0xFF00);
            /* decrease remaining count */
            --size;
         }
         /* set Fifo ptr */
         pDev->putDataBuf (pDev, pPacketCmd [0], &pFifo, &pPacket);
      }
   }

   return size;
}
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
#endif /* VIN_8BIT */

/*******************************************************************************
Description:
   Direct chip access for reading DIOP commands
Arguments:
   pDev  -  pointer to the device structure
   pCmd  - pointer to the write buffer, size 2 words
   pData - pointer to read Data
   count - number of words to read
Return:
    IFX_SUCCESS if successful, otherwise IFX_ERROR
Remarks:
   An error occurs in this function only if the RDQY bit isn't set in ISR
   Diop commands are two words to write and a particular word count
   immediately to read. This makes it differenct from usual commands.
*******************************************************************************/
IFX_int32_t parallel_diop_read (VINETIC_DEVICE *pDev, IFX_uint16_t *pCmd,
                                IFX_uint16_t *pData, IFX_int32_t count)
{
   IFX_vuint16_t *pNWD = NULL;
   IFX_vuint16_t *pRead;
   /* set interrupt register pointer */
#ifdef VIN_V14_SUPPORT
   IFX_vuint16_t* pIr = pDev->pIntReg;
#endif /* VIN_V14_SUPPORT */

   /* chache NWD pointers */
   pNWD = (IFX_vuint16_t*) pDev->pNwd;
   WAIT_FOR_READY_FLAG;
   /* write first word at NWD */
   *pNWD = pCmd[0];

   WAIT_FOR_READY_FLAG;
   /* write 2nd word at NWD */
   *pNWD = pCmd[1];
   pRead = pData;
   while (count > 1)
   {
      WAIT_FOR_READY_FLAG;
      /* read data at NWD */
      *pRead = *pNWD;
      /* set next data */
      count--;
      pRead++;
   }
   WAIT_FOR_READY_FLAG;
   /* read last word at EOM */
   *pRead = *(pDev->pEom);

   return IFX_SUCCESS;

#ifdef VIN_V14_SUPPORT
ACCESS_ERR:
   return IFX_ERROR;
#endif /* VIN_V14_SUPPORT */
}


#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
/*******************************************************************************
Description:
   Read all packets from packet outbox via parallel interface
Arguments:
   pDev - pointer to the device structure
   size - amount of data to read
Return:
  rest of data not read. Should be 0
Remarks:
  All data is read. If not, there was a failure somewhere. The calling function
  must ensure that all data is read
  An error occurs in this function only if the RDQY bit isn't set in IR
*******************************************************************************/
IFX_int32_t parallel_packet_read (VINETIC_DEVICE *pDev, IFX_int32_t size)
{
   IFX_vuint16_t *pNWD = NULL;
   IFX_uint16_t packet_length = 0, i = 0;
   /* buffer for first 2 packet commands */
   IFX_uint16_t pPacketCmd [CMD_HEADER_CNT] = {0};
   /* buffer handler */
   PACKET *pPacket = NULL;
   /* Fifo handler */
   FIFO *pFifo = NULL;
#ifdef VIN_V14_SUPPORT
   /* variable to write if no fifo buffer pointer available */
   IFX_vuint16_t* pIr = pDev->pIntReg;
#endif /* VIN_V14_SUPPORT */

   /* cache pNWD */
   pNWD = (IFX_vuint16_t*) pDev->pNwd;
   WAIT_FOR_READY_FLAG;
   /* write POBX (packet outbox) read cmd */
   *pNWD = SC_RPOBX;

   while (size > 0)
   {
      /* read command header words */
      for (i = 0; i < CMD_HEADER_CNT; i++)
      {
         WAIT_FOR_READY_FLAG;
         /* read first data at NWD */
         pPacketCmd [i] = *pNWD;
      }
      /* decrease remaining words counter */
      size -= CMD_HEADER_CNT;
      IFXOS_ASSERT ((pPacketCmd [0] & CMD1_CH) < 8);

      /* analyze header to get data length and data ptr to store data  */
      packet_length = pDev->getDataBuf (pDev, pPacketCmd, &pFifo, &pPacket);
      IFXOS_ASSERT (packet_length <= size);
      if (pPacket == NULL)
      {
         /* if no buffer, throw data away */
         for (i = 0; i < packet_length; i ++)
         {
            WAIT_FOR_READY_FLAG;
            if (size == 1)
               gOverflow = *(pDev->pEom);
            else
               gOverflow = *pNWD;
            --size;
         }
      }
      else
      {
         /* read data and store to fifo buffer */
         for (i = 0; i < packet_length; i ++)
         {
            WAIT_FOR_READY_FLAG;
            if (size == 1)
               pPacket->pData [i] = *(pDev->pEom);
            else
               pPacket->pData [i] = *pNWD;
            /* decrease remaining count */
            --size;
         }
         /* set Fifo ptr */
         pDev->putDataBuf (pDev, pPacketCmd [0], &pFifo, &pPacket);
      }
   }

   return size;

#ifdef VIN_V14_SUPPORT
ACCESS_ERR:
   return IFX_ERROR;
#endif /* VIN_V14_SUPPORT */
}
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */

