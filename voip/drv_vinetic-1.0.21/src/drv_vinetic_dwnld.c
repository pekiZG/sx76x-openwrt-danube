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
   Module      : drv_vinetic_dwnld.c
   Date        : 2003-10-27
 This file contains the implementation of the vinetic download
                 and the CRC Functions.
*/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_vinetic_api.h"
#include "drv_vinetic_dwnld.h"
#include "drv_vinetic_parallel.h"
/*#include "drv_vinetic_main.h"*/
#include "phi_VIP.h"
#include "acdwl.h"
#include "dcdwl.h"
#ifdef VIN_DEFAULT_FW
/*#include "dramdwl.h"
#include "pramdwl.h"*/
#include "dramfw.c"
#include "pramfw.c"
#endif /* VIN_DEFAULT_FW */

/* Include */
#ifdef VIN_V21_SUPPORT

#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
#define embd_ctrl_image    embd_ctrl_image_V2
#include "Embd_CTRL_2_14.c"
#undef embd_ctrl_image

#define embd_ctrl_image    embd_ctrl_image_V3
#include "Embd_CTRL_3_13.c"
#undef embd_ctrl_image
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */

#if (VIN_CFG_FEATURES & VIN_FEAT_VIN_S)
#define embd_ctrl_image    embd_ctrl_image_V4
#include "Embd_CTRL_4_04.c"
#undef embd_ctrl_image
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VIN_S) */

#endif /* VIN_V21_SUPPORT */

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

#define CRAM_DOWNLOAD_SIZE                29

/* a CRAM coefficient buffer has up to 183 words (including TSTR registers) */
#define CRAM_BUF_SIZE                     174
#define CRAM_BUF_SIZE_MAX                 183

/**  DRAM defines
*/
/* max DRAM data in a go */
#define MAX_DATAWRD_4A_CMD                253
/* length of dummies words added to DRAM firmware */
#define DRAM_DUMMY_LEN                    1
/* DRAM A-SP and B-SP areas to exclude for CRC calculation */
#define DRAM_ASP_ADDR_START               0x0000
#define DRAM_ASP_ADDR_STOP                0x3FFF
#define DRAM_BSP_ADDR_START               0xC000
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
/* Max word for FPI download */
#define MAX_FPI_WR                        29

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
IFX_LOCAL IFX_uint16_t pPhiCrcCmd [2] = {0x8801, 0x8901};

#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
/* not constant because CmdWrite ors the CMD1_RD bit to it */
IFX_LOCAL IFX_uint16_t pFwAutoDwld [4] = {CMD1_EOP, ECMD_AUTODWLD, 0x0000, 0xA000};
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */

/* DC/AC buffer references */
IFX_LOCAL IFX_uint16_t const * const pCode [2] = {pDcDataA, pAcDataA};

/* ============================= */
/* Global function declaration   */
/* ============================= */

/* IFX_LOCAL functions for EDSP Firmware download */
IFX_int32_t Dwld_setRAM   (VINETIC_DEVICE *pDev, VINETIC_FW_RAM ram,
                           IFX_uint32_t StartAddr, IFX_uint32_t StopAddr);
IFX_int32_t ActivateEdsp  (VINETIC_DEVICE *pDev);

/* ============================= */
/* Local function declaration    */
/* ============================= */

/* IFX_LOCAL functions for FPI download */
IFX_LOCAL IFX_int32_t  writeFPI       (VINETIC_DEVICE *pDev, IFX_uint16_t* pData, IFX_int32_t count);
IFX_LOCAL IFX_int32_t  writeRAM       (VINETIC_DEVICE *pDev, IFX_uint8_t *pData,
                                       IFX_uint32_t addr, IFX_int32_t count, VINETIC_FW_RAM ram);
#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
IFX_LOCAL IFX_int32_t  checkMbxEmpty  (VINETIC_DEVICE *pDev, IFX_int32_t full_size);
IFX_LOCAL IFX_int32_t  checkDldReady  (VINETIC_DEVICE *pDev);
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */

/* EDSP ram CRC calculation */
IFX_LOCAL IFX_int32_t readEdspCrc     (VINETIC_DEVICE *pDev, VINETIC_FW_RAM ram,
                                       IFX_uint32_t StartAddr, IFX_uint32_t StopAddr, IFX_int32_t count, IFX_uint16_t *pCrc);
#ifdef CRC_CHECK_V14
IFX_LOCAL IFX_int32_t calculateCrc    (IFX_uint16_t nCrc, IFX_uint16_t *pBlock, IFX_int32_t count);
IFX_LOCAL IFX_int32_t calculateEdspCrc(IFX_uint16_t nCrc, IFX_uint8_t *pData, IFX_int32_t size, VINETIC_FW_RAM ram);
IFX_LOCAL IFX_int32_t verifyEdspCrc   (VINETIC_DEVICE *pDev, IFX_uint8_t* pData, IFX_uint32_t StartAddr,
                                       IFX_int32_t count, VINETIC_FW_RAM ram, IFX_uint16_t *pCrcRd, IFX_uint16_t *pCrcCal);
#endif /* CRC_CHECK_V14 */

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* ============================= */
/* Local function definition     */
/* ============================= */

#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
/**
   Check by polling for dowload ready bit within timeout
\param
   pDev      - pointer to the device interface
\return
   IFX_SUCCESS or IFX_ERROR
Remark :
*/
IFX_LOCAL IFX_int32_t  checkDldReady  (VINETIC_DEVICE *pDev)
{
   IFX_int32_t usec, err;
   IFX_uint16_t pHwsr [2] = {0};

   /* set micro sec timeout */
   usec = WAIT_EDSP * 1000;
   /* check download ready */
   do
   {
      IFXOS_DELAYUS (WAIT_POLLTIME);
      usec -= WAIT_POLLTIME;
      err = ScRead (pDev, SC_RHWSR, pHwsr, 2);
      if (err != IFX_SUCCESS)
         return IFX_ERROR;
      if (pHwsr [1] & HWSR2_DL_RDY)
        return IFX_SUCCESS;
   }
   while (usec > 0);

   /* something went wrong if you reach here */
   SET_ERROR (ERR_NO_DLRDY);

   return IFX_ERROR;
}
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */

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
   pCmd[0] = CMD1_EOP;
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
   pCmd[0] = CMD1_EOP | CMD1_RD;
   /* set the crc-check variable */
   switch(ram)
   {
   case D_RAM:
      /* set CMD 1*/
      pCmd[1] = ECMD_CRC_DRAM;
      /* end addr = start addr + (word_length - 1) for one sector */
      if (StopAddr == 0)
         StopAddr = StartAddr + (count - 1);
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
         StopAddr = StartAddr +
                    ((count/WORD_PER_INSTRUCT) - 1) * PRAM_ADDRESS_JUMP;
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
IFX_LOCAL IFX_int32_t verifyEdspCrc (VINETIC_DEVICE *pDev, IFX_uint8_t *pData, IFX_uint32_t StartAddr,
                                     IFX_int32_t count, VINETIC_FW_RAM ram, IFX_uint16_t *pCrcRd,
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
   IFX_int32_t  err, pos = 0;
   IFX_uint8_t  len;
   IFX_uint8_t  ram_max_len [2] = {MAX_DATAWRD_4A_CMD, MAX_INSTWRD_4A_CMD};
   IFX_uint16_t eCmdRam [2]     = {ECMD_DRAM, ECMD_PRAM},
        pCmd [MAX_PACKET_WORD]  = {0};

   /* set ram address */
   err = Dwld_setRAM (pDev, ram, addr, 0);
   /* Set Cmd 1 and 2 */
   pCmd[0] = CMD1_EOP;
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
      /*cpb2w (&pCmd [2], &pData [pos << 1], (len * 2));*/
      {
         IFX_int32_t i = 0;
         IFX_uint16_t* pTmp = (IFX_uint16_t *)(&pData [pos << 1]);
         for ( i = 0; i < len; i++ )
         {
            pCmd[2 + i] = SWAP_UINT16 (pTmp [i]);
         }
      }

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

#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
/**
   check if the mailbox is empty
\param
   pDev         - handle to the device structure
\param
   full_size    - mail box size expected when empty (MAX_PACKET_WORD/MAX_CMD_WORD)
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   FIBXMS is polled within given timeout in ms. The polling is done in steps of
   WAIT_POLLTIME microsec */
IFX_LOCAL IFX_int32_t checkMbxEmpty  (VINETIC_DEVICE *pDev, IFX_int32_t full_size)
{
   IFX_int32_t usec = WAIT_MBX_EMPTY * 1000, err;
   IFX_uint16_t nFibxms, nSize;

   /* read FIBMX in timeout till enough memory */
   do
   {
      IFXOS_DELAYUS (WAIT_POLLTIME);
      usec -= WAIT_POLLTIME;
      /* read free inbox space */
      err = ScRead (pDev, SC_RFIBXMS, &nFibxms, 1);
      if (err != IFX_SUCCESS)
      {
         return IFX_ERROR;
      }
      /* check the size in FIBXMS */
      nSize = ((nFibxms & FIBXMS_CBOX) >> 8);
      if (nSize == full_size)
      {
         return IFX_SUCCESS;
      }
      else if (usec <= 0)
      {
         SET_ERROR (ERR_NO_MBXEMPTY);
         return IFX_ERROR;
      }
   }
   while (usec > 0);


   return IFX_ERROR;
}
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */

/**
   Writes Data to FPI ..real Download
\param
   pDev   - handle to device structure
\param
   pData  - ptr to data buffer
\param
   count  - size of data buffer
\return
   IFX_SUCCESS or IFX_ERROR
\remarks

*/
IFX_LOCAL IFX_int32_t writeFPI (VINETIC_DEVICE *pDev,
                                IFX_uint16_t *pData, IFX_int32_t count)
{
   IFX_int32_t err = 0, len, pos = 0;
   IFX_uint16_t pCmd [MAX_CMD_WORD] = {0};

   /* Set the commands words */
   pCmd[0] = CMD1_EOP;
   pCmd[1] = ECMD_ACCESS_FPI;
   /* write FPI fw */
   while ((count != 0) && (err == IFX_SUCCESS))
   {
      /* calculate length to download */
      if (count > MAX_FPI_WR)
         len = MAX_FPI_WR;
      else
         len = count;
      /* unmap data */
      memcpy (&pCmd [2], &pData [pos], (len * 2));
      /* write Data */
      err = CmdWrite (pDev, pCmd, len);
      /* increment position */
      pos += len;
      /* decrement count */
      count -= len;
   }
   if (err != IFX_SUCCESS)
      TRACE (VINETIC, DBG_LEVEL_HIGH, ("ERR : FPI Download unsuccesful!\n\r"));

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
   Download PHI
\param
   pDev   - pointer to the device interface
\param
   pCmd   - pointer to the user interface
\return
   IFX_SUCCESS or IFX_ERROR.
   The latter occurs if the size of the programm is to large
   or the basic access to the chips is not working
*/
IFX_int32_t VINETIC_DownloadPhi(VINETIC_DEVICE *pDev, VINETIC_IO_PHI* pCmd)
{
   IFX_int32_t err = IFX_SUCCESS;

   /* No download done before ?*/
   if ((pDev->nDevState & DS_PHI_DLD) == IFX_FALSE)
   {
      /* do phi download if size if ok  */
      if (pCmd->nSize > MAX_PHI_WORDS)
      {
         /* IFX_ERROR: PHI size too big */
         SET_ERROR(ERR_FUNC_PARM);
         return IFX_ERROR;
      }
      err = pDev->write (pDev, pCmd->pData, pCmd->nSize);
      pDev->nDevState &= ~DS_PHI_DLD;
   }
   if (err == IFX_SUCCESS)
   {
      /* wait before reading CRC. recommended time = 0,256 s */
      IFXOS_DELAYMS (PHI_CRC_TIME);
      /* PHI program has been downloaded so read phi checksum */
      err = pDev->diop_read (pDev, (IFX_uint16_t *)pPhiCrcCmd, &pCmd->nCrc, 1);
   }

   if (err == IFX_SUCCESS)
   {
      if (pCmd->nCrc == 0)
      {
         SET_ERROR (ERR_PHICRC0);
         return IFX_ERROR;
      }
      pDev->nDevState |= DS_PHI_DLD;
   }
   else
   {
      /* err is IFX_ERROR resulted from a device problem */
      SET_ERROR (ERR_NOPHI);
   }
   return err;
}

/**
  Downloads FPI and calculates CRC FPI
\param
   pDev   - pointer to the device interface
\param
   pCmd   - pointer toio interface structure
\return
   IFX_SUCCESS or IFX_ERROR
\remarks

*/
IFX_int32_t VINETIC_DownloadFpi(VINETIC_DEVICE *pDev, VINETIC_IO_FPI_DOWNLOAD* pCmd)
{
   IFX_int32_t err = IFX_SUCCESS;
   IFX_uint16_t pData [6] = {0}, pCrc [3] = {0};

   /* set start address for FPI */
   pData[0] = CMD1_EOP;
   pData[1] = ECMD_SET_FPI;
   pData[2] = HIGHWORD(pCmd->nStartAddr);
   pData[3] = LOWWORD (pCmd->nStartAddr);
   err =  CmdWrite (pDev, pData, 2);
   /* Write Data to FPI */
   if (err == IFX_SUCCESS)
   {
      err = writeFPI (pDev, pCmd->pData, pCmd->nSize);
   }
   /* Calculate CRC FPI */
   if (err == IFX_SUCCESS)
   {
      /* reset the CRC counter */
      pData[1] = ECMD_CRC_FPI;
      err = CmdWrite (pDev, pData, 0);
      /* set Start and Stop Addr for CRC */
      if (err == IFX_SUCCESS)
      {
         pData[1] = ECMD_SET_FPI;
         pData[2] = HIGHWORD(pCmd->nStartAddr);
         pData[3] = LOWWORD (pCmd->nStartAddr);
         pData[4] = HIGHWORD(pCmd->nStopAddr);
         pData[5] = LOWWORD (pCmd->nStopAddr);
         err =  CmdWrite (pDev, pData, 4);
      }
      /* calculate CRC now */
      if (err == IFX_SUCCESS)
      {
         pData[1] = ECMD_CRC_FPI;
         err = CmdRead (pDev, pData, pCrc, 1);
         if (err == IFX_SUCCESS)
            pCmd->nCrc  = pCrc [2];
      }
   }

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
*/
IFX_int32_t VINETIC_DwldBinary (VINETIC_DEVICE *pDev, IFX_uint8_t *pByte,
                                IFX_int32_t byte_size, VINETIC_FW_RAM ram,
                                IFX_uint16_t *pRamCrc)
{
   IFX_uint32_t ramAddr= 0;
   IFX_int32_t  err = IFX_SUCCESS, data_size = 0, actual_pos = 0;
   IFX_uint8_t  dummy_len [2] = {DRAM_DUMMY_LEN, PRAM_DUMMY_LEN};

#ifdef CRC_CHECK_V14
   IFX_uint16_t nCrcCal = 0;
#endif /* CRC_CHECK_V14 */

   if ((pByte == NULL) || (byte_size <= 0))
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
      if (data_size <= 0)
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
               cpb2dw (&ramAddrNext, &pByte[actual_pos + 4], sizeof (IFX_uint32_t));
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
#ifdef CRC_CHECK_V14
         err = verifyEdspCrc (pDev, &pByte [actual_pos], ramAddr, data_size,
                              ram, pRamCrc, &nCrcCal);
#else
         err =  readEdspCrc (pDev, ram, ramAddr, 0, data_size, pRamCrc);
#endif /* CRC_CHECK_V14 */
      }
   }

   return err;
}


#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
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
   IFX_uint16_t pDwldEnd [2] = {CMD1_EOP, ECMD_DWLD_END};
   IFX_uint16_t pData[4];

   /* Send command for download end */
   err =  CmdWrite (pDev, pDwldEnd, 0);
   /* check HWSR2 register if Download Ready Bit is set*/
   if (err == IFX_SUCCESS)
      err = checkDldReady  (pDev);
   /* check if MBX is empty . This must be DONE before minimizing the mailbox,
      otherwise Host Errors might occur */
   if (err == IFX_SUCCESS)
      err = checkMbxEmpty  (pDev, MAX_PACKET_WORD);
   /* Minimize the mailbox size. New cmd inbox space will be MAX_CMD_WORD */
   if (err == IFX_SUCCESS)
      err = ScWrite(pDev, SC_WMINCBX);
   /* Check mailbox size again, because the chip does not change the size
      immediately, we have to wait until it has switched back before 
      sending new commands. */
   if (err == IFX_SUCCESS)
      err = checkMbxEmpty (pDev, MAX_CMD_WORD);
   /* send start edsp command */
   if (err == IFX_SUCCESS)
      err = ScWrite (pDev, SC_WSTEDSP);
   if (err == IFX_SUCCESS)
   {
      IFXOS_DELAYMS (WAIT_AFTER_START_EDSP);
      /* read edsp firmware version */
      err = CmdRead(pDev, pCmd, pData, 2);
      if (err == IFX_SUCCESS)
      {
         /* Firmware Download OK */
         pDev->nDevState |= DS_FW_DLD;
         /* Set external and internal versions */
         pDev->nEdspVers = pData[2];
      }
      else if (pDev->err == ERR_OBXML_ZERO)
         SET_ERROR(ERR_NOFWVERS);
   }
   return err;
}
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */


/**
  Download PRAM and DRAM firmware
\param
   pDev      - pointer to the device interface
\param
   pInit     - handle to IO structure
\return
    IFX_SUCCESS or IFX_ERROR
\remarks
   This function sets the firmware to be download (local or external!).
   If the auto download flag is set, the function will activate the ROM EDSP
   before starting the EDSP it self
*/
IFX_int32_t Dwld_LoadFirmware (VINETIC_DEVICE *pDev, VINETIC_IO_INIT *pInit)
{
#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
   IFX_int32_t  err, bMapped = IFX_FALSE;
   IFX_uint16_t fibxms = 0;

   IFX_uint8_t *pPRAMfw = NULL, *pDRAMfw = NULL;
   IFX_uint32_t pram_size = 0, dram_size = 0;

#if (VIN_CFG_FEATURES & VIN_FEAT_VIN_S)
   if ((pDev->nChipType == VINETIC_TYPE_S) && (pDev->nChipMajorRev == VINETIC_V2x))
   {
      /* no download for S type */
      return IFX_SUCCESS;
   }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VIN_S) */

   /* load default firmware if no external firmware */
   if (!(pInit->nFlags & NO_FW_DWLD) &&
        ((pInit->pPRAMfw == NULL) || (pInit->pDRAMfw == NULL)))
   {
#ifdef VIN_DEFAULT_FW
#if (defined VIN_V21_SUPPORT) && (defined VIN_V14_SUPPORT)
      if (pDev->nChipMajorRev == VINETIC_V2x)
      {
#endif /* (defined VIN_V21_SUPPORT) && (defined VIN_V14_SUPPORT) */
#ifdef VIN_V21_SUPPORT
         /* Code for V2.x version */
         /* asked firmware download, but no binaries available */
         if (pDev->nChipType == VINETIC_TYPE_M ||
              pDev->nChipType == VINETIC_TYPE_C)
            /* Vin 4M V2.x has ROM */
            pInit->nFlags |= NO_FW_DWLD;
         else
         {
            SET_ERROR (ERR_NOFW);
            return IFX_ERROR;
         }
#endif /* VIN_V21_SUPPORT */
#if (defined VIN_V21_SUPPORT) && (defined VIN_V14_SUPPORT)
      }
      else
      {
#endif /* (defined VIN_V21_SUPPORT) && (defined VIN_V14_SUPPORT) */
#ifdef VIN_V14_SUPPORT
         /* Code for V1.6 and older version */
         pPRAMfw     = (IFX_uint8_t *)vinetic_fw_pram;
         pram_size   = sizeof (vinetic_fw_pram);
         pDRAMfw     = (IFX_uint8_t *)vinetic_fw_dram;
         dram_size   = sizeof (vinetic_fw_dram);
#endif /* VIN_V14_SUPPORT */
#if (defined VIN_V21_SUPPORT) && (defined VIN_V14_SUPPORT)
      }
#endif /* (defined VIN_V21_SUPPORT) && (defined VIN_V14_SUPPORT) */

#else
      /* asked firmware download, but no binaries available */
      SET_ERROR (ERR_NOFW);
      return IFX_ERROR;
#endif
   }
   else
   {
      pPRAMfw     = OS_MapBuffer(pInit->pPRAMfw, pInit->pram_size);
      pram_size   = pInit->pram_size;
      pDRAMfw     = OS_MapBuffer(pInit->pDRAMfw, pInit->dram_size);
      dram_size   = pInit->dram_size;
      if ((pPRAMfw == NULL) || (pDRAMfw == NULL))
      {
         OS_UnmapBuffer(pPRAMfw);
         OS_UnmapBuffer(pDRAMfw);

         SET_ERROR (ERR_NOFW);
         return IFX_ERROR;
      }
      bMapped = IFX_TRUE;
   }

   /* check if MBX is empty . This must be DONE before maximizing the mailbox */
   err = checkMbxEmpty  (pDev, MAX_CMD_WORD);
   /* Maximize Inbox . New cmd inbox space will be MAX_PACKET_WORD */
   if (err == IFX_SUCCESS)
      err = ScWrite (pDev, SC_WMAXCBX);
   if (err == IFX_SUCCESS)
      err = ScRead(pDev, SC_RFIBXMS, &fibxms, 1);
   if (err == IFX_SUCCESS && fibxms != 0xFF1F)
   {
      SET_ERROR(ERR_NOMAXCBX);
      err = IFX_ERROR;
   }
   /* send  WLEMP command before starting firmware download or autodownload */
   if (err == IFX_SUCCESS)
      err = ScWrite (pDev, SC_WLEMP);
   /* for Vinetic V.21, Autodownload is mandatory */
   if (pDev->nChipMajorRev == VINETIC_V2x)
      err = CmdWrite (pDev, (IFX_uint16_t*) pFwAutoDwld, 2);
   /* now download firmware if user allows */
   if (!(pInit->nFlags & NO_FW_DWLD))
   {
      /* write pram binary */
      if (err == IFX_SUCCESS)
      {
         err = VINETIC_DwldBinary (pDev, pPRAMfw, pram_size,
                                   P_RAM, &pInit->nPramCRC);
      }
      /* write dram binary */
      if (err == IFX_SUCCESS)
      {
         err = VINETIC_DwldBinary (pDev, pDRAMfw, dram_size,
                                   D_RAM, &pInit->nDramCRC);
      }
   }
   if ((err == IFX_SUCCESS) && !(pInit->nFlags & NO_EDSP_START))
   {
      /* load ROM firmware ? */
      if ((pDev->nChipMajorRev != VINETIC_V2x) && (pInit->nFlags & FW_AUTODWLD))
         err = CmdWrite (pDev, (IFX_uint16_t*)pFwAutoDwld, 2);
      /* start edsp */
      if (err == IFX_SUCCESS)
         err = ActivateEdsp (pDev);
   }
   /* reset buffers if fw from user space */
   if (bMapped)
   {
      OS_UnmapBuffer(pPRAMfw);
      OS_UnmapBuffer(pDRAMfw);
   }

   return err;
#else
   /* no download for S type */
   return IFX_SUCCESS;
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */
}

/**
  Download PHI Micro Programm
\param
   pDev    - pointer to the device interface
\param
   pInit    - command structure for initialization containing pointer to
            PHI program
\return
    IFX_SUCCESS or IFX_ERROR
\remarks
   This function downloads the  phi micro programm . If no micro program
   is given in settings, the function downloads a default micro program
   according to header file "phi_VIP.h"
   It is called upon initialization from the main module
   Returns IFX_SUCCESS if PHI is already download, it is not desired or
   the PHI has been downloaded successful.
   The chip version will be evaluated for checking if download of internal
   program can be done
*/
IFX_int32_t Dwld_LoadPhi (VINETIC_DEVICE *pDev, VINETIC_IO_INIT *pInit)
{
   VINETIC_IO_PHI phi;
   IFX_int32_t err = IFX_SUCCESS;

   if ((pInit->nFlags & NO_PHI_DWLD) != 0)
   {
      /* already downloaded or not wanted */
      return IFX_SUCCESS;
   }
#ifdef VIN_V21_SUPPORT
   if (pDev->nChipMajorRev == VINETIC_V2x)
   {
      switch (pDev->nChipRev)
      {
#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
         case VINETIC_V21:
            return Dwld_Load_EmbdCtrl (pDev, (IFX_uint32_t*)embd_ctrl_image_V2,
                                       sizeof(embd_ctrl_image_V2));
            /* break; */
         case VINETIC_V22:
            return Dwld_Load_EmbdCtrl (pDev, (IFX_uint32_t*)embd_ctrl_image_V3,
                                       sizeof(embd_ctrl_image_V3));
            /* break; */
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */
#if (VIN_CFG_FEATURES & VIN_FEAT_VIN_S)
         case VINETIC_V21_S:
            return Dwld_Load_EmbdCtrl (pDev, (IFX_uint32_t*)embd_ctrl_image_V4,
                                       sizeof(embd_ctrl_image_V4));
            /* break; */
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VIN_S) */
         default:
            break;
      }
   }
#endif /* VIN_V21_SUPPORT */

   if (pDev->nDevState & DS_PHI_DLD)
   {
      /* PHI program has been downloaded so read phi checksum */
      err = pDev->diop_read (pDev, (IFX_uint16_t *) pPhiCrcCmd,
                             &pInit->nPhiCrc, 1);
      return IFX_SUCCESS;
   }
   /* reset phi io struct */
   memset (&phi, 0, sizeof (VINETIC_IO_PHI));
   /* we want to download a PHI program. Check if a PHI program
      was passed */
   if (pInit->pPHIfw == NULL)
   {
      switch(pDev->nChipRev)
      {
#ifdef VIN_8BIT
      case VINETIC_V14:
         if (pDev->write == par8bit_write ||
             pDev->write == parint8bit_write)
         {
            phi.nSize = (sizeof (pPHI8Bit) / 2);
            phi.pData = (IFX_uint16_t *)pPHI8Bit;
            err = VINETIC_DownloadPhi(pDev, &phi);
            /* compare with local CRC if successful */
            if (phi.nCrc != phi8bit_crc)
            {
               SET_ERROR (ERR_NOPHI);
               err = IFX_ERROR;
            }
            pInit->nPhiCrc = phi.nCrc;
         }
         break;
#endif /* VIN_8BIT */
      case VINETIC_V13:
         /* ok, nothing given, we can download a default PHI program */
         phi.nSize = (sizeof (pPHI) / 2);
         phi.pData = (IFX_uint16_t *)pPHI;
         err = VINETIC_DownloadPhi(pDev, &phi);
         /* compare with local CRC if successful */
         if (phi.nCrc != phi_crc)
         {
            SET_ERROR (ERR_NOPHI);
            err = IFX_ERROR;
         }
         pInit->nPhiCrc = phi.nCrc;
         break;
      default:
         /* PHI microprogramm is not neccessary */
         return IFX_SUCCESS;
      }
   }
   else
   {
      if (pInit->phi_size > 0)
      {
         phi.pData = OS_MapBuffer(pInit->pPHIfw, pInit->phi_size);
         phi.nSize = (pInit->phi_size / 2);
         err = VINETIC_DownloadPhi(pDev, &phi);
         pInit->nPhiCrc = phi.nCrc;
         OS_UnmapBuffer(phi.pData);
      }
   }

   return err;
}

/**
   Download CRAM Coefficients and set Basic Configuration Register 1 and 2
\param
   pDev     - handle to device
\param
   pInit    - command structure for initialization containing pointer to
               PHI program
\return
   IFX_SUCCESS if download has been done, or IFX_ERROR.
   This function checks the current chip revision and downloads an internal
   CRAM if no external has been provided. If no external CRAM is provided
   and chip revision differs from 0x42, ERR_FUNC_PARM will be set.
   Error occurs in case of wrong length of CRAM. Check if all 28 CRAM coeffs
   and BCR 1 & 2 settings included in buffer.
\remarks
   the chip version will be evaluated  for checking if download of internal
   program can be done
*/
IFX_int32_t Dwld_LoadCram (VINETIC_DEVICE *pDev, VINETIC_IO_INIT *pInit)
{
   IFX_int32_t err = IFX_SUCCESS, count = 0, size = 0;
   IFX_uint16_t *pCram = NULL;

   /* check if no external byte file */
   if (pInit->pCram == NULL)
   {
      TRACE (VINETIC, DBG_LEVEL_LOW, ("Warning: No CRAM data!\n\r"));
      return IFX_SUCCESS;
   }
   else
   {
      pCram = OS_MapBuffer(pInit->pCram, pInit->cram_size);
      size = pInit->cram_size / 2;
   }
   /* check if file contains all CRAM coeffs and BCR reg settings */
   if (size != CRAM_BUF_SIZE)
   {
      TRACE (VINETIC, DBG_LEVEL_HIGH, ("Error: CRAM buffer not ok!\n\r"));
      err = IFX_ERROR;
      SET_ERROR (ERR_INVALID);
   }
   /* parse CRAM Buffer to write settings */
   while ((count != size) && (err == IFX_SUCCESS))
   {
      /* set Broadcast, coz same Config for all channels */
      pCram [count] |= CMD1_BC;
      /* write CRAM coeffs */
      if (count < (CRAM_REG_NR * CRAM_CMD_LEN))
      {
         err = CmdWrite (pDev, &pCram[count], CRAM_CMD_LEN - 2);
         count += CRAM_CMD_LEN;
      }
      /* Write BCR Registers */
      else
      {
         err = CmdWrite (pDev, &pCram[count], BCR_CMD_LEN - 2);
         count += BCR_CMD_LEN;
      }
   }
   /* store state "CRAM Download" into driver states */
   if (err == IFX_SUCCESS)
      pDev->nDevState |= DS_CRAM_DLD;
   else
      pDev->nDevState &= ~DS_CRAM_DLD;

   OS_UnmapBuffer(pCram);
   return err;
}

#ifdef VIN_V21_SUPPORT

#define EMBDCTRL_BUSY_WAITLOOP 100
#define STAT_BOOT 1
#define STAT_BOOTFAIL 3

/* BITFIELD DEFINITIONS  for SYS_ACTRL */
#define EMBDCTRL_DL              0x0003

/**
   does handshake with actrl reg. used in embd_ctrl_download
\param
   pDev    - handle to device
\return
   IFX_SUCCESS if download has been done successfully or IFX_ERROR.
*/
IFX_LOCAL IFX_int32_t embd_ctrl_ulhandshake(VINETIC_DEVICE  *pDev)
{
  IFX_uint16_t busy;
  IFX_int32_t cnt = EMBDCTRL_BUSY_WAITLOOP;
  IFX_int32_t err;

   do
   {
      err = RegRead (pDev, CMD1_IOP, &busy, 1, VIN_DEV_ACTRL);
      /* timeout */
      if (cnt-- == 0 || err != IFX_SUCCESS)
         return IFX_ERROR;
   }
   while ((busy & VIN_DEV_ACTRL_UR) == 0);

   return IFX_SUCCESS;
}

/**
   Download Embedded Controler program for VINETIC version V2.1
\param
   pDev    - handle to device
\param
   pData   - pointer to the program
\param
   size    - size of program in bytes
\return
   IFX_SUCCESS if download has been done successfully or IFX_ERROR.
*/
IFX_int32_t Dwld_Load_EmbdCtrl(VINETIC_DEVICE *pDev, IFX_uint32_t *pData,
                               IFX_uint16_t size)
{
   IFX_uint16_t control, data, result;
   IFX_int32_t err;
   IFX_uint32_t data_cnt;
   IFX_uint16_t bin_type;

   bin_type = (*pData & 0xF0000)>>16;
   if (((pDev->nChipRev == VINETIC_V21)   && (bin_type != 2)) ||
       ((pDev->nChipRev == VINETIC_V22)   && (bin_type != 3)) ||
       ((pDev->nChipRev == VINETIC_V21_S) && (bin_type != 4)))
   {
      SET_ERROR (ERR_FWINVALID);
      return IFX_ERROR;
   }

   size = size >> 2;
   /* 1. reset Embedded Controller */
   control =  0x1810 | (EMBDCTRL_DL);
   err = RegWrite (pDev, CMD1_IOP, &control, 1, VIN_DEV_ACTRL);
   if (err == IFX_ERROR)
      return IFX_ERROR;
   /* 2. reset ul handshake bit (and check if bootloader is active) */
   result=0;
   do {
      if (embd_ctrl_ulhandshake(pDev) == IFX_ERROR)
      {
         SET_ERROR (ERR_EMBDCTRL_DWLD_FAIL);
         return IFX_ERROR;
      }
      err = RegRead (pDev, CMD1_IOP, &result, 1, VIN_DEV_ABOOT);
      if (err == IFX_ERROR)
      {
         SET_ERROR(ERR_DEV_ERR);
         return IFX_ERROR;
      }
   }
   while (result != STAT_BOOT);

   /* 3. copy data image */
   data_cnt=0;

   while (data_cnt < size)
   {
     data  = pData[data_cnt] & 0xFFFF;
     /*embd_ctrl_ulhandshake(pDev);*/
     err = RegWrite (pDev, CMD1_IOP, &data, 1, VIN_DEV_ABOOT);
     if (err == IFX_ERROR)
     {
        SET_ERROR(ERR_DEV_ERR);
        return IFX_ERROR;
     }
     data = (pData[data_cnt] >> 16) & 0xFFFF;
     /*embd_ctrl_ulhandshake(pDev);*/
     err = RegWrite (pDev, CMD1_IOP, &data, 1, VIN_DEV_ABOOT);
     if (err == IFX_ERROR)
     {
        SET_ERROR(ERR_DEV_ERR);
        return IFX_ERROR;
     }
     data_cnt++;
   }

   /* 4. check download status */
   embd_ctrl_ulhandshake(pDev);
   err = RegRead (pDev, CMD1_IOP, &result, 1, VIN_DEV_ABOOT);
   if (err == IFX_ERROR)
   {
      SET_ERROR(ERR_DEV_ERR);
      return IFX_ERROR;
   }
   if (result == STAT_BOOTFAIL)
   {
      SET_ERROR (ERR_EMBDCTRL_DWLD_BOOT);
      TRACE (VINETIC,DBG_LEVEL_HIGH, ("embdctrl_start: Download Fail\n\r"));
      return IFX_ERROR;
   }
   return IFX_SUCCESS;
}
#endif /* VIN_V21_SUPPORT */

#ifdef VIN_V14_SUPPORT
/**
   Download DC/AC code needed for VINETIC version V1.4 and upper
\param
   pDev    - handle to device
\param
   pInit    - handle to the init structure
\param
   nCode   - DC/AC Code
\return
   IFX_SUCCESS if download has been done or is not needed, or IFX_ERROR.
   This function checks the current chip revision and downloads an internal
   AC code. If chip revision differs from 0x82 (V1.4) no download will be done
   and the function returns with IFX_SUCCESS.
*/
IFX_int32_t Dwld_LoadFpiCode (VINETIC_DEVICE *pDev, VINETIC_IO_INIT *pInit,
                              IFX_int32_t nCode)
{
   IFX_int32_t err = IFX_SUCCESS, i = 0;
   VINETIC_IO_FPI_DOWNLOAD Cmd;
   IFX_uint32_t const pAddrA [2][2] = {{START_ADR_DCA, END_ADR_DCA},
                                       {START_ADR_ACA, END_ADR_ACA}};
   IFX_uint32_t const pAddrB [2][2] = {{START_ADR_DCB, END_ADR_DCB},
                                       {START_ADR_ACB, END_ADR_ACB}};
   IFX_uint16_t const pCrc [2]      = {DC_CRC_A, AC_CRC_A};
   IFX_uint8_t const pErr [2]       = {ERR_DCCRC_FAIL, ERR_ACCRC_FAIL};
   IFX_uint16_t nVal;

   /* do this for appropriate chips */
   if (pDev->nChipRev != VINETIC_V14)
      return IFX_SUCCESS;

   /* set code ptr and size */
   Cmd.pData = (IFX_uint16_t *)pCode [nCode];
   /* set start/stop of Path A/B addresses and load */
   while (i < 3 && err == IFX_SUCCESS)
   {
      if (i == 0)
      {
         Cmd.nStartAddr = pAddrA [nCode][0];
         Cmd.nStopAddr  = pAddrA [nCode][1];
      }
      else if (i == 1)
      {
         Cmd.nStartAddr = pAddrB [nCode][0];
         Cmd.nStopAddr  = pAddrB [nCode][1];
      }

      if (i != 2)
      {
         Cmd.nSize = 1 + Cmd.nStopAddr - Cmd.nStartAddr;
         err = VINETIC_DownloadFpi (pDev, &Cmd);
         if ((err == IFX_SUCCESS) && (Cmd.nCrc != pCrc [nCode]))
         {
            SET_ERROR (pErr [nCode]);
            err = IFX_ERROR;
         }
      }
      else
      {
         if (nCode == DC_CODE)
         {
            /* activate DCCTRL */
            pInit->nDcCrc = Cmd.nCrc;
            err = RegModify(pDev, (CMD1_SOP | CMD1_BC), SOP_OFFSET_BCR1,
                            BCR1_PRAM_DCC, BCR1_PRAM_DCC);
            if (err == IFX_SUCCESS)
               pDev->nDevState |= DS_DC_DWLD;
         }
         else
         {
            /* activate ALM-DSP */
            pInit->nAcCrc = Cmd.nCrc;
            nVal = CCR_JUMP_AC3;
            err = RegWrite(pDev, CMD1_SOP, &nVal, 1, SOP_OFFSET_CCR);
            if (err == IFX_SUCCESS)
               err = RegWrite(pDev, CMD1_SOP | 2, &nVal, 1, SOP_OFFSET_CCR);
            if (err == IFX_SUCCESS)
               pDev->nDevState |= DS_AC_DWLD;
         }
      }
      i ++;
   }

   return err;
}
#endif /* VIN_V14_SUPPORT */

/**
  Downloads CRAM
\param
   pCh    - pointer to the channel structure
\param
   pCmd   - pointer to io interface structure
\return
   IFX_SUCCESS or IFX_ERROR
\remarks

*/
IFX_int32_t VINETIC_DownloadCram (VINETIC_CHANNEL *pCh, VINETIC_IO_CRAM *pCmd)
{
   VINETIC_DEVICE *pDev         = pCh->pParent;
   IFX_uint32_t   nWordsToSend;
   IFX_uint16_t   nWords;
   IFX_uint16_t   nAddress;
   IFX_uint16_t   *pCram;
   IFX_uint16_t   nCmd          = CMD1_COP;
   IFX_int32_t    nCh;
   IFX_int32_t    nChStart, nChStop;
   IFX_int32_t    err           = IFX_SUCCESS;

   switch (pCmd->nFormat)
   {
      case VINETIC_IO_CRAM_FORMAT_2_1:
      case VINETIC_IO_CRAM_FORMAT_2_2:
         nAddress     = pCmd->nStartAddr;
         nWordsToSend = pCmd->nLength;
         nCmd        |= (pCh->nChannel-1);
         if (pCmd->bBroadCast)
         {
            nCmd     |= CMD1_BC;
            nChStart = 0;
            nChStop  = pDev->nAnaChan;
         }
         else
         {
            nChStart = pCh->nChannel - 1;
            nChStop  = nChStart + 1;
         }

         while ((err == IFX_SUCCESS) && (nWordsToSend > 0))
         {
            if (nWordsToSend > CRAM_DOWNLOAD_SIZE)
               nWords = CRAM_DOWNLOAD_SIZE;
            else
               nWords = nWordsToSend;

            err = RegWrite (pCh->pParent, nCmd,
                            &pCmd->aData[pCmd->nLength - nWordsToSend],
                            nWords, nAddress);

            nAddress     += nWords;
            nWordsToSend -= nWords;
         }

         if (err != IFX_SUCCESS)
            break;

         /* configure BCR1, BCR2 and TSTR2 register */
         for (nCh = nChStart; nCh < nChStop; nCh++)
         {
            if (pCmd->bcr1.nMask)
            {
               err = RegModify (pDev, (CMD1_SOP | nCh), SOP_OFFSET_BCR1,
                                pCmd->bcr1.nMask, pCmd->bcr1.nData);
               if (err != IFX_SUCCESS)
                  break;
            }
            if (pCmd->bcr2.nMask)
            {
               /* Modify the cached value inside the driver and write the
                  modified cached value to the device mailbox */
               BCR2.value = (BCR2.value & ~pCmd->bcr2.nMask) |
                  (pCmd->bcr2.nData & pCmd->bcr2.nMask);

               err = wrReg (pCh, BCR2_REG);
               if (err != IFX_SUCCESS)
                  break;
            }

            if (pCmd->tstr2.nMask)
            {
               err = RegModify (pDev, (CMD1_SOP | nCh), SOP_OFFSET_TSTR2,
                                pCmd->tstr2.nMask, pCmd->tstr2.nData);
               if (err != IFX_SUCCESS)
                  break;
            }
         }
         break;
      case VINETIC_IO_CRAM_FORMAT_1:
         /* Channel specific CRAM download for VINETIC V1.6 and older */
         nCh            = pCh->nChannel - 1;
         pCram          = pCmd->aData;
         nWordsToSend   = pCmd->nLength;
         nWords         = 0;
         /* Check if file contains all CRAM coeffs and BCR reg settings */
         if ((nWordsToSend < CRAM_BUF_SIZE) || (nWordsToSend > CRAM_BUF_SIZE_MAX))
         {
            TRACE (VINETIC, DBG_LEVEL_HIGH, ("Error: CRAM buffer not ok!\n\r"));
            SET_ERROR (ERR_FUNC_PARM);
            return IFX_ERROR;
         }

         /* Reset CRAM_EN for security reasons
            Modify the cached value inside the driver and write the modified
            cached value to the device mailbox */
         BCR2.value &= ~BCR2_CRAM_EN;

         err = wrReg (pCh, BCR2_REG);

         /* Parse CRAM Buffer to write settings */
         while ((nWords != nWordsToSend) && (err == IFX_SUCCESS))
         {
            if (pCmd->bBroadCast == IFX_TRUE)
            {
               /* broadcast download */
               pCram [nWords] |= CMD1_BC;
            }
            else
            {
               /* channel specific download */
               pCram [nWords] |= nCh;
            }
            /* Write CRAM coeffs */
            if (nWords < (CRAM_REG_NR * CRAM_CMD_LEN))
            {
               err = CmdWrite (pCh->pParent, &pCram[nWords], CRAM_CMD_LEN - 2);
               nWords += CRAM_CMD_LEN;
            }
            /* Write BCR or TSTR registers */
            else
            {
               /* Update the cached value if it is BCR2 register */
               if (pCram[nWords + 1] == 0x0801)
               {
                  /* 0xC8C0 is the bit-mask that CRAM concerned */
                  BCR2.value = (BCR2.value & ~0xC8C0) |
                                (pCram[nWords + 2] & 0xC8C0);
                  err = wrReg(pCh, BCR2_REG);
               }
               else
               {
                  err = CmdWrite (pCh->pParent, &pCram[nWords], BCR_CMD_LEN - 2);
               }

               nWords += BCR_CMD_LEN;
            }
         }
         break;
      default:
         TRACE(VINETIC, DBG_LEVEL_HIGH,
              ("IFX_ERROR: unsupported CRAM download format\n"));
         err = IFX_ERROR;
         break;
   }

   /* store state "CRAM Download" into driver states */
   if (err == IFX_SUCCESS)
      pDev->nDevState |= DS_CRAM_DLD;
   else
      pDev->nDevState &= ~DS_CRAM_DLD;

   return err;
}

/* @} */


