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
   Module      : drv_vinetic_dwnld.c
   Date        : 2003-10-27
 This file contains the implementation of the vinetic host common download and
 the CRC Functions.
 Host related download functions (AC/DC/PHI/FPI/CRAM/) are implemented in the
 host specific file when needed.
*/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_vinetic_api.h"
#include "drv_vinetic_dwnld.h"
#include "drv_vinetic_main.h"

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

/* Memory Page on which the EDSP fw is downloaded */
#ifdef VIN_2CPE
/* For 2CPE, EDSP download is done in memory page 2. */
#define MEM_PAGE                          2
#else
/* for 4X, the momory is not divided into pages. */
#define MEM_PAGE                          0
#endif

/**  DRAM defines
*/
/* max DRAM data in a go */
#define MAX_DATAWRD_4A_CMD                253
/* length of dummies words added to DRAM firmware */
#define DRAM_DUMMY_LEN                    1
/* DRAM A-SP and B-SP areas to exclude for CRC calculation */
#define DRAM_ASP_ADDR_START               0x0000
#define DRAM_ASP_ADDR_STOP                0x3FFF
#ifdef VIN_2CPE
#define DRAM_BSP_ADDR_START               0xA000
#else
#define DRAM_BSP_ADDR_START               0xC000
#endif /* VIN_2CPE */
#define DRAM_BSP_ADDR_STOP                0xFFFF


/**  PRAM defines
*/
/* maximal PRAM data in a go */
#define MAX_INSTWRD_4A_CMD                252
/* length of dummies words added to PRAM firmware */
#define PRAM_DUMMY_LEN                    3
/* number of words in an PRAM instruction */
#define WORD_PER_INSTRUCT                 3
/* PRAM address step */
#define PRAM_ADDRESS_JUMP                 2

/* ============================= */
/* Global variable definition    */
/* ============================= */

/* not constant because CmdWrite ors the CMD1_RD bit to it */
IFX_LOCAL IFX_uint16_t pEdspCrcReset [2][2] =
{
   {CMD1_EOP, ECMD_CRC_DRAM},
   {CMD1_EOP, ECMD_CRC_PRAM}
};

/* not constant because CmdWrite ors the CMD1_RD bit to it */
IFX_LOCAL IFX_uint16_t pFwAutoDwld [4] =
{
   CMD1_EOP, ECMD_AUTODWLD, 0x0000, 0xA000
};

/* ============================= */
/* Global function declaration   */
/* ============================= */

/* IFX_LOCAL functions for EDSP Firmware download */
IFX_int32_t Dwld_setRAM  (VINETIC_DEVICE *pDev, VINETIC_FW_RAM ram,
                           IFX_uint32_t StartAddr, IFX_uint32_t StopAddr);
IFX_int32_t ActivateEdsp (VINETIC_DEVICE *pDev);

/* ============================= */
/* Local function declaration    */
/* ============================= */

/* IFX_LOCAL functions for EDSP download */

IFX_LOCAL IFX_int32_t  writeRAM   (VINETIC_DEVICE *pDev, IFX_uint8_t *pData,
                                   IFX_uint32_t addr, IFX_int32_t count,
                                   VINETIC_FW_RAM ram);
/* EDSP ram CRC calculation */
IFX_LOCAL IFX_int32_t readEdspCrc     (VINETIC_DEVICE *pDev, VINETIC_FW_RAM ram,
                                   IFX_uint32_t StartAddr, IFX_uint32_t StopAddr,
                                   IFX_int32_t count, IFX_uint16_t *pCrc);
#ifdef CRC_CHECK_V14
IFX_LOCAL IFX_int32_t calculateCrc (IFX_uint16_t nCrc, IFX_uint16_t *pBlock,
                                    IFX_int32_t count);
IFX_LOCAL IFX_int32_t calculateEdspCrc (IFX_uint16_t nCrc, IFX_uint8_t *pData,
                                        IFX_int32_t size, VINETIC_FW_RAM ram);
IFX_LOCAL IFX_int32_t verifyEdspCrc (VINETIC_DEVICE *pDev, IFX_uint8_t* pData,
                                     IFX_uint32_t StartAddr, IFX_int32_t count,
                                     VINETIC_FW_RAM ram, IFX_uint16_t *pCrcRd,
                                     IFX_uint16_t *pCrcCal);
#endif /* CRC_CHECK_V14 */

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* ============================= */
/* Local function definition     */
/* ============================= */

/**
   Set RAM address
\param
   pDev         - pointer to the device interface
\param
   ram          - RAM flag : D_RAM or P_RAM
\param
   StartAddr    - RAM sector start address
\param
   StopAddr     - RAM sector end address
\return
   IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t Dwld_setRAM(VINETIC_DEVICE *pDev, VINETIC_FW_RAM ram,
                        IFX_uint32_t StartAddr, IFX_uint32_t StopAddr)
{
   IFX_int32_t cnt = 0, err = IFX_ERROR;
   IFX_uint16_t pCmd[6] = {0};

   /* set Cmd 1 */
   pCmd[0] = CMD1_EOP | MEM_PAGE;
   /* Set Cmd 2 and Data words */
   switch(ram)
   {
   case D_RAM:
      pCmd[1] = ECMD_SET_DRAM_ADR;
      pCmd[2] = LOWWORD(StartAddr);
      cnt     = 1;
      if (StopAddr != 0)
      {
         pCmd[3]  = LOWWORD(StopAddr);
         cnt      = 2;
      }
      break;
   case P_RAM:
      pCmd[1] = ECMD_SET_PRAM_ADR;
      pCmd[2] = HIGHWORD(StartAddr);
      pCmd[3] = LOWWORD(StartAddr);
      cnt     = 2;
      if (StopAddr != 0)
      {
         pCmd[4]  = HIGHWORD(StopAddr);
         pCmd[5]  = LOWWORD(StopAddr);
         cnt = 4;
      }
      break;
   }
   if (cnt != 0)
      err = CmdWrite (pDev, pCmd, cnt);

   return err;

}

/**
   read CRC Value of DRAM/PRAM Firmware
\param
   pDev         - pointer to the device interface
\param
   ram          - P_RAM or D_RAM
\param
   StartAddr    - start address of sector
\param
   StopAddr     - stop address of the sector
\param
   count        - number of words to write
\param
   pCrc         - ptr to Crc value
\return
   IFX_SUCCESS or IFX_ERROR
Remark :
   following applies to calculate the PRAM stop address by given start address
   and number of words to write:

   - Pram addresses are even : 0x0, 0x2, ....., 0x2*n
   - Every instruction is 48 bits : 1 instruction = 3 Words
   - Every PRAM address points to 1 instruction : 0x2n -> Word1, Word2, Word3

   a PRAM buffer looks as follows:
   \code
   addresses |    Instructions

   0x0       ->  0xXXXX, 0xXXXX, 0xXXXX
   0x2       ->  0xXXXX, 0xXXXX, 0xXXXX
   0x4       ->  0xXXXX, 0xXXXX, 0xXXXX
   0x6       ->  0xXXXX, 0xXXXX, 0xXXXX
   0x8       ->  0xXXXX, 0xXXXX, 0xXXXX
   .
   .
   .
   0x2n      ->  0xXXXX, 0xXXXX, 0xXXXX
   \endcode
   so, if 9 Words are written at address 0x2, the stop address is 0x6 as example
*/
IFX_LOCAL IFX_int32_t readEdspCrc (VINETIC_DEVICE *pDev, VINETIC_FW_RAM ram,
                                   IFX_uint32_t StartAddr, IFX_uint32_t StopAddr,
                                   IFX_int32_t count, IFX_uint16_t *pCrc)
{
   IFX_int32_t err = IFX_SUCCESS;
   IFX_uint16_t pCmd[2] = {0}, pData [3] = {0};

   if (count == 0)
      return IFX_SUCCESS;

   /* fill command */
   pCmd[0] = CMD1_RD | CMD1_EOP | MEM_PAGE;
   /* set the crc-check variable */
   switch(ram)
   {
   case D_RAM:
      /* set CMD 1*/
      pCmd[1] = ECMD_CRC_DRAM;
      /* end addr = start addr + (word_length - 1) for one sector */
      if (StopAddr == 0)
         StopAddr = StartAddr + ((IFX_uint32_t)count - 1);
      break;
   case P_RAM:
      /* set cmd 1 */
      pCmd[1] = ECMD_CRC_PRAM;
      /* calculate stop address under following considerations :
         1-  instruction number = (words number / 3)
         2-  address jump   = 2 (even addresses)
         3-  address Offset = (instruction number - 1) * Address jump
         4-  stop address = Start address + address offset
      */
      if (StopAddr == 0)
      {
         StopAddr = StartAddr + ((((IFX_uint32_t)count/WORD_PER_INSTRUCT) - 1) *
                                PRAM_ADDRESS_JUMP);
      }
      if(StartAddr % 2 != 0 || StopAddr % 2 != 0)
      {
         SET_ERROR (ERR_FUNC_PARM);
         err = IFX_ERROR;
         TRACE (VINETIC, DBG_LEVEL_HIGH,
               ("ERROR: Given PRAM Start/Stop address not even!\n\r"));
      }
      break;
   }
   /* Set the RAM area for CRC calculation */
   if (err == IFX_SUCCESS)
      err = Dwld_setRAM(pDev, ram, StartAddr,StopAddr);
   /* Read the CRC */
   if (err == IFX_SUCCESS)
      err = CmdRead (pDev, pCmd, pData, 1);
   /* set CRC value */
   if (err == IFX_SUCCESS)
      *pCrc = pData [2];

   return err;
}

#ifdef CRC_CHECK_V14
/**
   calculates the CRC of a given data block
\param
   nCrc    - old Crc value
\param
   pBlock  - ptr to data block
\param
   count   - Size of data block : 1 for DRAM or 3 for PRAM
\return
   block CRC value
\remarks
*/
IFX_LOCAL IFX_int32_t calculateCrc (IFX_uint16_t nCrc, IFX_uint16_t *pBlock,
                                    IFX_int32_t count)
{
   IFX_int32_t i = 0;
   IFX_uint16_t newCrc = nCrc,
        defaultCrc = 0x8810,
        crcVal = 0;

   /* calculate CRC in a loop according to DRAM or PRAM.
      Notice : PRAM intruction  = 3 WORDS
               DRAM instruction = 1 IFX_uint16_t
   */
   for (i = 0; i < count; i++)
   {
      if ((newCrc & 0x0001) == 0)
      {
         /* shift lsb into Carry flag */
         crcVal = (IFX_uint16_t) newCrc >> 1;
         /* XOR with default CRC coz Carry = 0 */
         newCrc = (crcVal ^ defaultCrc);
      }
      else
      {
         /* shift lsb into Carry flag */
         newCrc = (IFX_uint16_t) newCrc >> 1;
      }
      /* XOR with ram Data */
      newCrc = (newCrc ^ pBlock [i]);
   }

   return newCrc;
}

/**
   calculates the total CRC of a DRAM/PRAM sector data
\param
   nCrc   - old CRC value
\param
   pData  - Data Buffer of one DRAM/PRAM sector
\param
   size   - sector length
\param
   ram    - D_RAM/P_RAM
\return
   CRC value
\remarks

*/
IFX_LOCAL IFX_int32_t calculateEdspCrc (IFX_uint16_t nCrc, IFX_uint8_t *pData,
                                        IFX_int32_t size, VINETIC_FW_RAM ram)
{
   IFX_int32_t i = 0,
       ramCount;
   IFX_uint16_t pBlock [3] = {0};

   if (ram == D_RAM)
      ramCount = 1;
   else
      ramCount = 3;

   /* calculate CRC in Loop */
   for (i = 0; i < size ; i+= ramCount)
   {
      /* set appropriate Data block */
      cpb2w (pBlock, &pData [i << 1], (ramCount * 2));
      /* calculate CRC of block */
      nCrc = calculateCrc (nCrc, pBlock, ramCount);
      /* reset block buffer */
      memset (pBlock, 0, sizeof (pBlock));
   }

   return nCrc;
}

/**
   Compares read CRC with calculated CRC of DRAM/PRAMfirmware block
\param
   pDev         - pointer to the device interface
\param
   pData        - pointer to the data words
\param
   StartAddr    - start address of sector
\param
   count        - number of words to write
\param
   ram          - P_RAM or D_RAM
\param
   pCrcRd       - ptr to read CRC value
\param
   pCrcCal      - ptr to calculated CRC value
\return
   IFX_SUCCESS or IFX_ERROR
*/
IFX_LOCAL IFX_int32_t verifyEdspCrc (VINETIC_DEVICE *pDev, IFX_uint8_t *pData,
                                     IFX_uint32_t StartAddr, IFX_int32_t count,
                                     VINETIC_FW_RAM ram, IFX_uint16_t *pCrcRd,
                                     IFX_uint16_t *pCrcCal)
{
   IFX_int32_t err = IFX_SUCCESS;
   const IFX_char_t * const sRam [2] = {"DRAM", "PRAM"};

   if (count == 0)
      return IFX_SUCCESS;

   err = readEdspCrc (pDev, ram, StartAddr, 0, count, pCrcRd);
   if (err == IFX_SUCCESS)
   {
      *pCrcCal = calculateEdspCrc (*pCrcCal, pData, count, ram);
   }
   if (*pCrcRd != *pCrcCal)
   {
      TRACE (VINETIC, DBG_LEVEL_HIGH,
            ("Warning : CRC Mismatch for %s sector\n\r", sRam [ram]));
      TRACE (VINETIC, DBG_LEVEL_HIGH,
            ("[CRC addr 0x%08lX, read 0x%04X, calc 0x%04X]\n\r",
             StartAddr, *pCrcRd, *pCrcCal));
   }

   return err;
}
#endif /* CRC_CHECK_V14 */

/**
   writes RAM firmware to chip
\param
   pDev      - pointer to the device interface
\param
   pData     - pointer to the data words
\param
   addr      - start address of sector
\param
   count     - number of words to write, assumed > 0.
\param
   ram       - P_RAM or D_RAM
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   Function implementation is Endiannes dependant. Appropriate macro is used to
   read IFX_uint16_t data from Byte Data ptr argument.
*/
IFX_LOCAL IFX_int32_t writeRAM (VINETIC_DEVICE *pDev, IFX_uint8_t* pData,
                                IFX_uint32_t addr, IFX_int32_t count,
                                VINETIC_FW_RAM ram)
{
   IFX_int32_t  err;
   IFX_uint32_t pos = 0;
   IFX_uint8_t  len;
   IFX_uint8_t  ram_max_len [2] = {MAX_DATAWRD_4A_CMD, MAX_INSTWRD_4A_CMD};
   IFX_uint16_t eCmdRam [2] = {ECMD_DRAM, ECMD_PRAM},
                pCmd [MAX_PACKET_WORD]  = {0};

   /* set ram address */
   err = Dwld_setRAM (pDev, ram, addr, 0);
   /* Set Cmd 1 and 2 */
   pCmd[0] = CMD1_EOP | MEM_PAGE;
   pCmd[1] = eCmdRam [ram];

   /* do a check for PRAM to be sure that the sector contains entire set of
      instructions */
   if ((ram == P_RAM) && (count % 3 != 0))
   {
      SET_ERROR (ERR_PRAM_FW);
      return IFX_ERROR;
   }
   /* write all data */
   while ((count > 0) && (err == IFX_SUCCESS))
   {
      /* calculate length to download */
      if (count > ram_max_len [ram])
         len = ram_max_len [ram];
      else
         len = count;
      /* unmap data */
      cpb2w (&pCmd [2], &pData [pos << 1], (len * 2));
      /* write Data */
      err = CmdWrite (pDev, pCmd, len);
      /* increment position */
      pos += len;
      /* decrement count */
      count -= len;
#if (defined VIN_V21_SUPPORT) && (defined VIN_V14_SUPPORT)
      if (pDev->nChipMajorRev == VINETIC_V2x)
      {
#endif /* (defined VIN_V21_SUPPORT) && (defined VIN_V14_SUPPORT) */
#ifdef VIN_V21_SUPPORT
         /* Code for V2.1 version */
         if (ram == D_RAM && count > 0)
         {
            /* set ram address */
            addr += len;
            err = Dwld_setRAM (pDev, ram, addr, 0);
         }
#endif /* VIN_V21_SUPPORT */
#if (defined VIN_V21_SUPPORT) && (defined VIN_V14_SUPPORT)
      }
#endif /* (defined VIN_V21_SUPPORT) && (defined VIN_V14_SUPPORT) */
   }

   return err;
}

/* ============================= */
/* Global function definition    */
/* ============================= */

/** /defgroup VINETIC_DWNLD VINETIC Download */
/* @{ */

/**
   read EDSP CRC
\param
   pDev   - pointer to the device interface
\param
   pCmd   - pointer to the user interface
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   - write the appropriate command to reset EDSP CRC
   - read CRC for given input parameters.
*/
IFX_int32_t VINETIC_ReadEdspCrc(VINETIC_DEVICE *pDev, VINETIC_IO_CRC* pCmd)
{
   IFX_int32_t err = IFX_SUCCESS;

   /* reset EDSP Crc */
   err  = CmdWrite (pDev, (IFX_uint16_t *)pEdspCrcReset [pCmd->nMemId], 0);
   if (err == IFX_SUCCESS)
      err = readEdspCrc (pDev, (VINETIC_FW_RAM)pCmd->nMemId, pCmd->nStartAdr,
                         pCmd->nStopAdr, 0, &pCmd->nCrc);

   return err;
}

/**
  Download PRAM or DRAM bynary
\param
   pDev         - pointer to the device interface
\param
   pByte        - Byte Buffer to download
\param
   byte_size    - size of the byte buffer
\param
   ram          - P_RAM/D_RAM
\param
   pRamCrc      - CRC of D_RAM/P_RAM
\return
    IFX_SUCCESS or IFX_ERROR
\remarks
   -  This function receives a byte buffer in a defined format
      <datasize, address, data> and writes to RAM according to this buffer.
   -  Function implementation is Endiannes dependant. Appropriate macro is used
      to read IFX_uint32_t data size and address from Byte Data ptr argument.
   -  It is assumed that the buffers contain dummy words located at the end of
      the array  : PRAM = 3 dummy words, DRAM = 1 dummy word. These dummy words
      are downloaded also, but aren't considered for the CRC calculation.
*/
IFX_int32_t VINETIC_DwldBinary (VINETIC_DEVICE *pDev, IFX_uint8_t *pByte,
                                IFX_uint32_t byte_size, VINETIC_FW_RAM ram,
                                IFX_uint16_t *pRamCrc)
{
   IFX_int32_t  err = IFX_SUCCESS;
   IFX_uint32_t ramAddr= 0, data_size = 0, actual_pos = 0;
   IFX_uint8_t  dummy_len [2] = {DRAM_DUMMY_LEN, PRAM_DUMMY_LEN};
   IFX_uint16_t ramCrc;


   if ((pByte == NULL) || (byte_size == 0))
   {
      SET_ERROR(ERR_FUNC_PARM);
      return IFX_ERROR;
   }
   /* reset internal CRC register */
   err  = CmdWrite (pDev, (IFX_uint16_t *)pEdspCrcReset [ram], 0);
   /* do download */
   while ((actual_pos < byte_size) && (err == IFX_SUCCESS))
   {
      /* set address and data size */
      cpb2dw (&data_size, &pByte[actual_pos], sizeof (IFX_uint32_t));
      cpb2dw (&ramAddr, &pByte[actual_pos + 4], sizeof (IFX_uint32_t));
      /* calculate actual position */
      actual_pos += (2 * sizeof (IFX_uint32_t));
      if (data_size == 0)
      {
         SET_ERROR(ERR_FWINVALID);
         return IFX_ERROR;
      }
      /* do the download according to ram  */
      err = writeRAM(pDev, &pByte [actual_pos], ramAddr, data_size, ram);
      if (err == IFX_SUCCESS)
      {
         /* calculate actual position */
         actual_pos += (data_size * 2);
         /* remove dummy words from data size before the CRC calculation */
         if (actual_pos == byte_size)
            data_size -= dummy_len [ram];
         /* in case of DRAM , ignore last sector of A-SP/B-SP for CRC calculation */
         if (ram == D_RAM)
         {
            IFX_uint32_t ramAddrNext = 0;
            if (actual_pos != byte_size)
            {
               cpb2dw (&ramAddrNext, &pByte[actual_pos + 4],
                       sizeof (IFX_uint32_t));
            }
            if ((
#if (DRAM_ASP_ADDR_START > 0)
                 (ramAddr >= DRAM_ASP_ADDR_START)     &&
#endif /* #if (DRAM_ASP_ADDR_START > 0) */
                 (ramAddr <= DRAM_ASP_ADDR_STOP)      &&
                 (ramAddrNext >= DRAM_ASP_ADDR_STOP)) ||
                 (
#if (DRAM_BSP_ADDR_START > 0)
                 (ramAddr >= DRAM_BSP_ADDR_START) &&
#endif /* #if (DRAM_BSP_ADDR_START > 0) */
                 (ramAddr <= DRAM_BSP_ADDR_STOP) &&
                 (actual_pos == byte_size)))
            {
               data_size -= 2;
            }
         }

         /* be sure all packets are out of the mailbox before continue... */
         err = VINETIC_Host_CheckMbxEmpty (pDev, MAX_PACKET_WORD);

#ifdef CRC_CHECK_V14
         if (err == IFX_SUCCESS)
         {
            IFX_uint16_t nCrcCal = 0;
            err = verifyEdspCrc (pDev, &pByte [actual_pos], ramAddr, data_size,
                                 ram, pRamCrc, &nCrcCal);
         }
#else
         if (err == IFX_SUCCESS)
         {
            err = readEdspCrc (pDev, ram, ramAddr, 0, data_size, pRamCrc);
         }
#endif /* CRC_CHECK_V14 */
      }
   }
   /* read ram CRC from buffer and check */
   if (err == IFX_SUCCESS)
   {
      cpb2w (&ramCrc, &pByte[byte_size - 2], sizeof (IFX_uint16_t));
      if (ramCrc != *pRamCrc)
      {
         TRACE (VINETIC, DBG_LEVEL_HIGH, ("VINETIC CRC error: File=>CRC=0x%04X"
                " , EDSP=>CRC=0x%04X\r\n", ramCrc, *pRamCrc));
         SET_ERROR(ERR_FWCRC_FAIL);
         err = IFX_ERROR;
      }
   }

   return err;
}

/**
  This function activates the EDSP after the download
\param
   pDev      - pointer to the device
\return
   IFX_SUCCESS or IFX_ERROR. Following errors may occur:
   ERR_DEV_ERR, ERR_NOFWVERS, ERR_NO_MBXEMPTY, ERR_NO_DLRDY
\remarks
   1- HWSR2 is polled for the status of Download Ready bit.
   2- The Mailbox must be empty before minimizing it size.
*/
IFX_int32_t ActivateEdsp (VINETIC_DEVICE *pDev)
{
   IFX_int32_t err;
   IFX_uint16_t pCmd[] = { CMD1_EOP, ECMD_VERS };
   IFX_uint16_t pData[ CMD_HEADER_CNT + CMD_VERS_FEATURES_EXT_LEN ];
   IFX_uint32_t i;

#ifndef VIN_2CPE
   /* Download End oparates also a Start EDSP for 2CPE and should be done
      at Start EDSP time. */
   {
      IFX_uint16_t pDwldEnd [2] = {CMD1_EOP, ECMD_DWLD_END};

      /* Send command for download end */
      err =  CmdWrite (pDev, pDwldEnd, 0);
      /* check if Download Ready Bit is set*/
      if (err == IFX_SUCCESS)
         err = VINETIC_Host_CheckDldReady  (pDev);
      if (err == IFX_ERROR)
         return IFX_ERROR;
   }
#endif /* !VIN_2CPE */
   /* check if MBX is empty . This must be DONE before minimizing the mailbox,
      otherwise Host Errors might occur */
   err = VINETIC_Host_CheckMbxEmpty  (pDev, MAX_PACKET_WORD);
   /* Switching the mailbox shows problems sometimes on certain systems. The
      following two waits lets the controller recover from previous work.
      The problem results in a non switched mailbox */
   IFXOS_DELAYMS (MBX_SIZERECOVER);
   /* Minimize the mailbox size. New cmd inbox space will be MAX_CMD_WORD */
   if (err == IFX_SUCCESS)
   {
      err = VINETIC_Host_SwitchCmdMbxSize (pDev, IFX_FALSE);
   }
   IFXOS_DELAYMS (MBX_SIZERECOVER);

   /* Check mailbox size again, because the chip does not change the size
      immediately, we have to wait until it has switched back before
      sending new commands. */
   if (err == IFX_SUCCESS)
   {
      err = VINETIC_Host_CheckMbxEmpty (pDev, MAX_CMD_WORD);
   }
   /*  start edsp :
       - For 4VIP/4M : send Start EDSP short command
       - For 2CPE    : send Download End Command
                       After the FW has started, the Download Ready bit will be
                       set in the STAT register. The host specific
                       implementation will wait for the bit to be set.
                       In addition the controller has to wait at least for
                       another ms (1ms) before accessing the mailbox.
   */
   if (err == IFX_SUCCESS)
   {
      err = VINETIC_Host_StartEdsp (pDev);
   }
   if (err == IFX_SUCCESS)
   {
      IFXOS_DELAYMS (WAIT_AFTER_START_EDSP);
      /* read edsp firmware version */
      err = CmdRead (pDev, pCmd, pData, CMD_VERS_FEATURES_BASIC_LEN);
      if (err == IFX_SUCCESS)
      {
         /* Firmware Download OK */
         pDev->nDevState |= DS_FW_DLD;

         /* Set external and internal versions */
         for (i=0; i < CMD_VERS_FEATURES_EXT_LEN; i++)
            pDev->nEdspVers[i] = 0;
         for (i=0; i < CMD_VERS_FEATURES_BASIC_LEN; i++)
            pDev->nEdspVers[i] = pData[CMD_HEADER_CNT + i];

         /* Depending on the version of the firmware not all version registers
            may be valid. So we check the version first before we read the
            version register with more data.
            For the VIP familiy the third doubleword of the version register
            was added after major version 15 minor version 168.
            For the 2CPE the third doubleword was added with major version 16
            minor version 250. */

         if ((pDev->nChipMajorRev == VINETIC_V2x)  &&
             ((pDev->nChipRev == VINETIC_2CPE_V21) ||
              (pDev->nChipRev == VINETIC_2CPE_V22) ||
              (pDev->nChipRev == VINETIC_2CPE_AMR)))
         {
            if (  ((pData[2] & ECMD_VERS_VERSION) > 16) ||
                 (((pData[2] & ECMD_VERS_VERSION) == 16) && (pData[3] >= 250)) )
            {
               err = CmdRead (pDev, pCmd, pData, CMD_VERS_FEATURES_EXT_LEN);
               if (err == IFX_SUCCESS)
                  pDev->nEdspVers[2] = pData[CMD_HEADER_CNT + 2];
            }
         }
         else
         {
            if (  ((pData[2] & ECMD_VERS_VERSION) > 15) ||
                 (((pData[2] & ECMD_VERS_VERSION) == 15) && (pData[3] > 168)) )
            {
               err = CmdRead (pDev, pCmd, pData, CMD_VERS_FEATURES_EXT_LEN);
               if (err == IFX_SUCCESS)
                  pDev->nEdspVers[2] = pData[CMD_HEADER_CNT + 2];
            }
         }
      }
      else if (pDev->err == ERR_OBXML_ZERO)
         SET_ERROR(ERR_NOFWVERS);
   }

   return err;
}

/**
  Download PRAM and DRAM firmware
\param
   pDev      - pointer to the device interface
\param
   pEdsp     - handle to VINETIC_EDSP_FWDWLD structure
\return
    IFX_SUCCESS or IFX_ERROR
\remarks
    - This function assumes that pEdsp - if valid - has valid firmware DRAM and
      PRAM pointers. So the caller must make sure that the pointers are
      mapped accordingly.

    - This function executes the whole flow necessary for EDSP fimware download
      and returns DRAM and PRAM checksums after a sucessfull downlod.
*/
IFX_int32_t Dwld_LoadFirmware (VINETIC_DEVICE *pDev, VINETIC_EDSP_FWDWLD *pEdsp)
{
   IFX_int32_t  err;

   /* valid pointer ? */
   if (pEdsp == NULL)
   {
      SET_ERROR(ERR_FUNC_PARM);
      return IFX_ERROR;
   }
   /* check if MBX is empty . This must be DONE before maximizing the mailbox */
   err = VINETIC_Host_CheckMbxEmpty  (pDev, MAX_CMD_WORD);
   /* Maximize Inbox . New cmd inbox space will be MAX_PACKET_WORD */
   if (err == IFX_SUCCESS)
   {
      err = VINETIC_Host_SwitchCmdMbxSize (pDev, IFX_TRUE);
   }
   /* Reset EDSP before stating the download */
   if (err == IFX_SUCCESS)
   {
      err =  VINETIC_Host_ResetEdsp (pDev);
   }
   /* for Vinetic 4VIP/M/C/S V.21 (not CPEv2.x), autodownload is mandatory */
   if (pDev->nChipMajorRev == VINETIC_V2x &&  
       pDev->nChipRev != VINETIC_2CPE_V21 &&
       pDev->nChipRev != VINETIC_2CPE_V22 &&
       pDev->nChipRev != VINETIC_2CPE_AMR)
      err = CmdWrite (pDev, (IFX_uint16_t *) pFwAutoDwld, 2);

   /* now download firmware if user allows */
   if (!(pEdsp->nEdspFlags & NO_FW_DWLD))
   {
      /* write pram binary */
      if (err == IFX_SUCCESS)
      {
         err = VINETIC_DwldBinary (pDev, pEdsp->pPRAMfw, pEdsp->pram_size,
                                   P_RAM, &pEdsp->nPramCRC);
      }
      /* write dram binary */
      if (err == IFX_SUCCESS)
      {
         err = VINETIC_DwldBinary (pDev, pEdsp->pDRAMfw, pEdsp->dram_size,
                                   D_RAM, &pEdsp->nDramCRC);
      }
   }
   if ((err == IFX_SUCCESS) && !(pEdsp->nEdspFlags & NO_EDSP_START))
   {
      /* Load ROM firmware if required and chip isn't Vinetic VIP/M/C/S 2.1
         or if chip is Vinetic-CPE v2.x and no RAM firmware downloaded */
      if ( (pDev->nChipMajorRev == VINETIC_V1x || 
            pDev->nChipRev == VINETIC_2CPE_V21 ||
            pDev->nChipRev == VINETIC_2CPE_V22 ||
            pDev->nChipRev == VINETIC_2CPE_AMR) &&
           (pEdsp->nEdspFlags & FW_AUTODWLD))
      {
         err = CmdWrite (pDev, (IFX_uint16_t *)pFwAutoDwld, 2);
#ifdef VIN_2CPE
         if (err == IFX_SUCCESS)
            pDev->hostDev.bRomFirmware = IFX_TRUE;
#endif
      }

      /* start edsp */
      if (err == IFX_SUCCESS)
         err = ActivateEdsp (pDev);
   }

   return err;
}

/* @} */


