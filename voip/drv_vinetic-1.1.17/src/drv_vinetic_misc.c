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
   Module      : drv_vinetic_misc.c
   Description : This file contains the implementation of the functions
                 for usual vinetic operations.
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_vinetic_api.h"
#include "drv_vinetic_misc.h"
#include "drv_vinetic_main.h"
#include "drv_version.h"

/*
#include "drv_vinetic_dwnld.h"
#include "drv_vinetic_parallel.h"
*/
/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

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

/* ============================= */
/* Local function definition     */
/* ============================= */

/*******************************************************************************
Description:
   VINETIC Report Set for all vinetic messages
Arguments:
   pDev         - pointer to the device interface
   driver_level - new DBG_LEVEL
Return:
    IFX_SUCCESS if no error, otherwise IFX_ERROR
*******************************************************************************/
IFX_int32_t VINETIC_Report_Set(IFX_uint32_t driver_level)
{
   if ((driver_level > DBG_LEVEL_HIGH) || (driver_level < DBG_LEVEL_LOW))
   {
      SetTraceLevel(VINETIC, DBG_LEVEL_OFF);
      SetLogLevel(VINETIC, DBG_LEVEL_OFF);
   }
   else
   {
      SetTraceLevel(VINETIC, driver_level);
      SetLogLevel(VINETIC, driver_level);
   }

   return IFX_SUCCESS;
}

#ifndef VIN_2CPE
IFX_int32_t VINETIC_RuntimeTrace_Report_Set (IFX_uint32_t level)
{
#ifdef RUNTIME_TRACE
   SetTraceLevel (VINETIC_RUNTIME_TRACE, level);
   return IFX_SUCCESS;
#else
   return IFX_ERROR;
#endif /* RUNTIME_TRACE */
}
#endif /* VIN_2CPE */

/*******************************************************************************
Description:
   Get the VINETIC Chip Version
Arguments:
   pDev - pointer to the device interface
   pInd - pointer to the user interface
Return:
    IFX_SUCCESS if no error, otherwise IFX_ERROR
*******************************************************************************/
IFX_int32_t VINETIC_Version (VINETIC_DEVICE *pDev, VINETIC_IO_VERSION *pInd)
{
   IFX_int32_t err = IFX_SUCCESS;
   IFX_uint16_t pCmd[2] = {0}, pData[4] = {0}, nVers = 0;

#ifdef VIN_2CPE
   /* if we didn't do a firmware download yet, we'll have to read
      the chip version directly, otherwise it's cached already */
   if (!(pDev->nDevState & DS_FW_DLD))
   {
      /* Please note: interrupts are locked until the chip is initialised so
         do not try to protect the read access by locking them again doing so
         would lock up the driver. */
      if (pDev->nDevState & DS_DEV_INIT)
         REG_READ_PROT (pDev, V2CPE_DUPO_REVISION, &nVers);
      else
         REG_READ_UNPROT (pDev, V2CPE_DUPO_REVISION, &nVers);
      if (pDev->err != ERR_OK)
         err = IFX_ERROR;
      else
      {
         pInd->nType    = V2CPE_DUPO_REVISION_TYPE_GET (nVers);
         pInd->nChannel = V2CPE_DUPO_REVISION_CHAN_GET (nVers);
         pInd->nChip    = V2CPE_DUPO_REVISION_REV_GET (nVers);
      }
   }
   else /* take the cached version */
   {
      pInd->nType       = pDev->nChipType;
      pInd->nChannel    = pDev->nAnaChan;
      pInd->nChip       = pDev->nChipRev;
      pInd->nDCCtrlVers = pDev->nDCCTRLVers;
   }
#if (VIN_ACCESS_MODE == VIN_ACCESS_MODE_EVALUATION)
#if 0
   v2cpe_test_access (pDev, 0xFFFF);
#endif
#endif /* VIN_ACCESS_MODE_EVALUATION */
#else
   {
      IFX_uint16_t pRvCmd[2] = {0x8801, 0x8001};
      /* read chip version */
      if (pDev->nDevState & DS_DEV_INIT)
      {
         IFXOS_MutexLock((pDev)->mbxAcc);
         Vinetic_IrqLockDevice((pDev));
         err = pDev->hostDev.diop_read(pDev, pRvCmd, &nVers, 1);
         Vinetic_IrqUnlockDevice((pDev));
         IFXOS_MutexUnlock((pDev)->mbxAcc);
      }
      else
         err = pDev->hostDev.diop_read(pDev, pRvCmd, &nVers, 1);
      if (err == IFX_SUCCESS)
      {
         /* set type */
         pInd->nType    = (nVers & REVISION_TYPE) >> 12;
         /* set channel */
         pInd->nChannel = (nVers & REVISION_ANACHAN) >> 8;
         /* set chip revision */
         pInd->nChip    = (nVers & REVISION_REV);
      }
   }
#endif /* VIN_2CPE */
   if (err == IFX_ERROR)
   {
      pInd->nType    = 0;
      pInd->nChannel = 0;
      pInd->nChip    = 0;
      SET_ERROR(ERR_DEV_ERR);
      TRACE(VINETIC, DBG_LEVEL_HIGH,
           ("IFX_ERROR: Read Chip Revision Register failed\n\r"));
   }
   /* assume no EDSP as default */
   pInd->nEdspVers   = NOTVALID;
   pInd->nEdspIntern = NOTVALID;
   /* read firmware version only if download already done */
   if ((pDev->nDevState & DS_FW_DLD) && (err == IFX_SUCCESS))
   {
      /*read edsp firmware version */
      pCmd[0] = CMD1_EOP;
      pCmd[1] = ECMD_VERS;
      err = CmdRead(pDev, pCmd, pData, 2);
      if (err == IFX_SUCCESS)
      {
         /* Set external and internal versions */
         pInd->nEdspVers   = pData[2];
         pInd->nEdspIntern = pData[3];
      }
      else
         TRACE(VINETIC, DBG_LEVEL_HIGH,
              ("IFX_ERROR: Read EDSP Firmware Version failed\n\r"));
   }
   /* set TAPI version */
   pInd->nTapiVers = TAPI_VERS;
   /* set Driver version*/
   pInd->nDrvVers = MAJORSTEP << 24 | MINORSTEP << 16 |
                    VERSIONSTEP << 8 | VERS_TYPE;
   return err;
}

/*******************************************************************************
Description:
   Get the VINETIC Driver Version
Arguments:
   pDev - pointer to the device interface
   pInd - pointer to the user interface
Return:
    IFX_SUCCESS if no error, otherwise IFX_ERROR
Remarks:
   The difference between this function and VINETIC_Version is just, that the
   vinetic chip is not accessed. This avoid error messages if just the driver
   version is of interest
*******************************************************************************/
IFX_int32_t VINETIC_DrvVersion (VINETIC_IO_VERSION *pInd)
{
   IFX_int32_t err = IFX_SUCCESS;
   /* set Driver version*/
   pInd->nDrvVers = MAJORSTEP << 24 | MINORSTEP << 16 |
                    VERSIONSTEP << 8 | VERS_TYPE;
   return err;
}

/*******************************************************************************
Description:
  get different versions (FW, HW, DRIVER, EDSP, ...)
Arguments:
   pDev - pointer to the device interface
Return:
    IFX_SUCCESS if no error, otherwise IFX_ERROR
Remarks:
   This function calls the IO function VINETIC_Version
*******************************************************************************/
IFX_int32_t VINETIC_GetVersions(VINETIC_DEVICE *pDev)
{
   VINETIC_IO_VERSION ioVers;
   IFX_int32_t err = IFX_SUCCESS;

   /* initialize version structure */
   memset(&ioVers, 0, sizeof(VINETIC_IO_VERSION));
   /* call version function */
   err = VINETIC_Version(pDev, &ioVers);
   if (err == IFX_SUCCESS)
   {
      /* set number of analog channels decoded from revision register */
      if ((ioVers.nEdspVers == NOTVALID) || (ioVers.nEdspIntern == NOTVALID))
         pDev->nAnaChan = ioVers.nChannel;
   }

   return err;
}

#ifndef VIN_2CPE
/* This code will be considered later, when GPIO support for 2CPE
   will be implemented */
/*******************************************************************************
 * GPIO support (outdated)
 ******************************************************************************/
/*******************************************************************************
Description:
   Configure device GPIO pins 0..7
Arguments:
   pDev     - pointer to device structure
   pCfg     - pointer to configuration structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remark :
   The configuration is written to the Vinetic only if the BoardInit has been
   done already (pDev->nDevState & DS_Board_Init).Otherwise the configuration
   is buffered and written when the chip is beeing initialized.
*******************************************************************************/
IFX_int32_t VINETIC_Dev_GPIO_Cfg (VINETIC_DEVICE *pDev,
                                  VINETIC_IO_DEV_GPIO_CFG const *pCfg)
{
   IFX_int32_t err = IFX_SUCCESS;

   if (pDev->nDevState & DS_BASIC_INIT)
      err = rdReg((VINETIC_CHANNEL*) pDev, GCR1_REG);

   if (err == IFX_SUCCESS)
   {
      GCR1.value &= (IFX_uint16_t)(0x00FF | (~ (pCfg->nGPIO<<8)));
      if (pCfg->ctrld == VINETIC_IO_DEV_GPIO_CONTROLL_RESERVED)
         GCR1.value |= (0xFF00 & (pCfg->nGPIO<<8));
      if (pDev->nDevState & DS_BASIC_INIT)
         err = wrReg((VINETIC_CHANNEL*) pDev, GCR1_REG);
   }
   if ((err == IFX_SUCCESS) && (pDev->nDevState & DS_BASIC_INIT))
      err = rdReg((VINETIC_CHANNEL*) pDev, GCR2_REG);
   if (err == IFX_SUCCESS)
   {
      /* Input Enable */
      GCR2.value &= (IFX_uint16_t)(0x00FF | (~ (pCfg->nGPIO<<8)));
      if (pCfg->drIn == VINETIC_IO_DEV_GPIO_DRIVER_ENABLED)
         GCR2.value |= (0xFF00 & (pCfg->nGPIO<<8));
      /* Output Enable */
      GCR2.value &= (0xFF00 | (~ pCfg->nGPIO));
      if (pCfg->drOut == VINETIC_IO_DEV_GPIO_DRIVER_ENABLED)
         GCR2.value |= (0x00FF & pCfg->nGPIO);
      if (pDev->nDevState & DS_BASIC_INIT)
         err = wrReg((VINETIC_CHANNEL*) pDev, GCR2_REG);
   }

   return err;
}

/*******************************************************************************
Description:
   Set levels to device GPIO pins 0..7 (if configured as output)
Arguments:
   pDev     - pointer to device structure
   pCfg     - pointer to configuration structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remark :
   The configuration is written to the Vinetic only if the BoardInit has been
   done already (pDev->nDevState & DS_Board_Init).Otherwise the configuration
   is buffered and written when the chip is beeing initialized.
*******************************************************************************/
IFX_int32_t VINETIC_Dev_GPIO_Set (VINETIC_DEVICE *pDev,
                                  VINETIC_IO_DEV_GPIO_SET const *pSet)
{
   IFX_int32_t err = IFX_SUCCESS;

   if (pDev->nDevState & DS_BASIC_INIT)
      err = rdReg((VINETIC_CHANNEL*) pDev, GCR1_REG);

   if (err == IFX_SUCCESS)
   {
      GCR1.value &= (0xFF00 | (~pSet->mask));
      GCR1.value |= (0x00FF & pSet->value);
      if (pDev->nDevState & DS_BASIC_INIT)
         err = wrReg((VINETIC_CHANNEL*) pDev, GCR1_REG);
   }

   return err;
}
#endif /* VIN_2CPE */

/*******************************************************************************
Description:
   copy a byte buffer into a dword buffer respecting the endianess.
Argiments
   pDWbuf : DWord buffer
   pBbuf  : Byte buffer
   nB     : size of Bytes to be copied (must be a multiple of 4)
Return:
Remarks:
   No matter if uC supports big endian or little endian, this macro will still
   copy the data in the apropriate way for a big endian target chip like Vinetic
   The pointer pBbuf might be not aligned to 32bit - therefore the dword is
   first copied byte wise into a 32bit variable (tmp) before swapping.
*******************************************************************************/
void cpb2dw(IFX_uint32_t* pDWbuf, IFX_uint8_t* pBbuf, IFX_uint32_t nB)
{
   IFX_uint32_t i = 0, nDW = nB >> 2;
   IFX_uint32_t tmp;
   IFX_uint8_t *pBtmp = (IFX_uint8_t *)pBbuf;
   IFXOS_ASSERT (!(nB % 4));
   for ( i = 0; i < nDW; i++ )
   {
      tmp = ((pBtmp[4*i+0] << 24) |
             (pBtmp[4*i+1] << 16) |
             (pBtmp[4*i+2] <<  8) |
             (pBtmp[4*i+3] <<  0));
             (pDWbuf)[i] = (tmp);
   }
}

/*******************************************************************************
Description:
   copy a byte buffer into a word buffer respecting the endianess.
Argiments
   pWbuf  : Word buffer
   pBbuf  : Byte buffer
   nB     : size of Bytes to be copied (must be a multiple of 2)
Return:
Remarks:
   No matter if uC supports big endian or little endian, this macro will still
   copy the data in the apropriate way for a big endian target chip like Vinetic
   The pointer pBbuf might be not aligned to 32bit - therefore the word is
   first copied byte wise into a 16bit variable (tmp) before swapping.
*******************************************************************************/
void cpb2w (IFX_uint16_t* pWbuf,IFX_uint8_t *pBbuf, IFX_uint32_t nB)
{
   IFX_uint32_t i = 0, nW = nB >> 1;
   IFX_uint16_t tmp;
   IFX_uint8_t *pBtmp = (IFX_uint8_t *) pBbuf;
   IFXOS_ASSERT (!(nB % 2));
   for ( i = 0; i < nW; i++ )
   {
      tmp = ((pBtmp[2*i+0] << 8) |
             (pBtmp[2*i+1] << 0));
      (pWbuf)[i] = (tmp);
   }
}




